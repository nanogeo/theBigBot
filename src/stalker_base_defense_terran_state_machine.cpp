#pragma once

#include "stalker_base_defense_terran_state_machine.h"
#include "theBigBot.h"

namespace sc2 {


#pragma region StalkerBaseDefenseTerranDefendFront

// TODO change for maps with natural ramp
void StalkerBaseDefenseTerranDefendFront::TickState()
{
	if (state_machine->target == NULL)
	{
		for (const auto& unit : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_REAPER)))
		{
			state_machine->target = unit;
			break;
		}
		if (state_machine->target == NULL)
		{
			if (forward)
			{
				agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::GENERAL_MOVE, state_machine->front_of_base[1]);
				if (Distance2D(state_machine->stalker->pos, state_machine->front_of_base[1]) < 1)
					forward = false;
			}
			else
			{
				agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::GENERAL_MOVE, state_machine->front_of_base[0]);
				if (Distance2D(state_machine->stalker->pos, state_machine->front_of_base[0]) < 1)
					forward = true;
			}
		}
	}
	else
	{
		if (state_machine->attack_status == false)
		{
			// TODO move infront of units based on distance away
			if (Distance2D(state_machine->target->pos, state_machine->stalker->pos) <= 6 && state_machine->stalker->weapon_cooldown == 0)
			{
				agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::ATTACK_ATTACK, state_machine->target);
				state_machine->attack_status = true;
			}
			else
			{
				agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::GENERAL_MOVE, state_machine->target->pos);

			}
		}
		else if (state_machine->stalker->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
	}

}

void StalkerBaseDefenseTerranDefendFront::EnterState()
{
	return;
}

void StalkerBaseDefenseTerranDefendFront::ExitState()
{
	return;
}

State* StalkerBaseDefenseTerranDefendFront::TestTransitions()
{
	Units gates = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_GATEWAY));
	Units other_units = agent->Observation()->GetUnits(IsUnits({ UNIT_TYPEID::PROTOSS_ADEPT, UNIT_TYPEID::PROTOSS_STALKER }));
	if (other_units.size() > 1 || agent->scout_info_terran.first_rax_production != reaper || (gates.size() > 0 && gates[0]->orders.size() > 0 && gates[0]->orders[0].progress > .9))
		return new StalkerBaseDefenseTerranMoveAcross(agent, state_machine);
	return NULL;
}

std::string StalkerBaseDefenseTerranDefendFront::toString()
{
	return "defend front";
}

#pragma endregion

#pragma region StalkerBaseDefenseTerranMoveAcross

void StalkerBaseDefenseTerranMoveAcross::TickState()
{
	if (state_machine->target == NULL)
	{
		for (const auto& unit : agent->Observation()->GetUnits(Unit::Alliance::Enemy))
		{
			if (Distance2D(state_machine->stalker->pos, unit->pos) < 8)
			{
				state_machine->target = unit;
				break;
			}
		}
		if (state_machine->target == NULL)
		{
			agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::GENERAL_MOVE, agent->locations->adept_scout_runaway);
		}
	}
	else
	{
		if (Distance2D(state_machine->stalker->pos, state_machine->target->pos) > 8)
		{
			state_machine->target = NULL;
			return;
		}

		if (state_machine->attack_status == false)
		{
			// TODO move infront of units based on distance away
			if (Distance2D(state_machine->target->pos, state_machine->stalker->pos) <= 4 && state_machine->stalker->weapon_cooldown == 0)
			{
				agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::ATTACK_ATTACK, state_machine->target);
				state_machine->attack_status = true;
			}
			else
			{
				agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::GENERAL_MOVE, state_machine->target->pos);

			}
		}
		else if (state_machine->stalker->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
	}

}

void StalkerBaseDefenseTerranMoveAcross::EnterState()
{
	return;
}

void StalkerBaseDefenseTerranMoveAcross::ExitState()
{
	return;
}

State* StalkerBaseDefenseTerranMoveAcross::TestTransitions()
{
	if (Distance2D(state_machine->stalker->pos, agent->locations->adept_scout_runaway) < 3)
		return new ScoutBaseDefenseTerranHarrassFront(agent, state_machine, agent->locations->adept_scout_shade, agent->locations->adept_scout_runaway);
	return NULL;
}

std::string StalkerBaseDefenseTerranMoveAcross::toString()
{
	return "move across map";
}

#pragma endregion

#pragma region ScoutBaseDefenseTerranHarrassFront

void ScoutBaseDefenseTerranHarrassFront::TickState()
{
	Point2D furthest_point = attack_pos;
	Units enemy_units = agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
	for (const auto& unit : enemy_units)
	{
		double range = Utility::RealGroundRange(unit, state_machine->stalker) + 2;
		Point2D intersection = agent->locations->attack_path_line.GetPointFrom(unit->pos, range, false);
		if (intersection != Point2D(0, 0))
		{
			float dist = Distance2D(intersection, attack_pos);
			if (dist > Distance2D(furthest_point, attack_pos))
			{
				furthest_point = intersection;
			}
		}
	}

	//agent->Debug()->DebugSphereOut(agent->ToPoint3D(furthest_point), .7, Color(255, 255, 0));

	const Unit* closest_unit = Utility::ClosestTo(agent->Observation()->GetUnits(Unit::Alliance::Enemy), state_machine->stalker->pos);

	if (state_machine->attack_status == true)
	{
		if (state_machine->stalker->weapon_cooldown > 0)
			state_machine->attack_status = false;
	}
	else if (state_machine->stalker->shield < 5 || state_machine->stalker->weapon_cooldown > 0)
	{
		if (state_machine->stalker->shield < 5)
			shields_regening = true;
		agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::GENERAL_MOVE, furthest_point);
	}
	else if (shields_regening)
	{
		if (state_machine->stalker->shield >= 75)
			shields_regening = false;
		agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::GENERAL_MOVE, furthest_point);
	}
	else if (Distance2D(closest_unit->pos, state_machine->stalker->pos) <= 6)
	{
		agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::ATTACK_ATTACK, closest_unit);
		state_machine->attack_status = true;
	}
	else
	{
		agent->Actions()->UnitCommand(state_machine->stalker, ABILITY_ID::GENERAL_MOVE, closest_unit);
	}
}

void ScoutBaseDefenseTerranHarrassFront::EnterState()
{
	return;
}

void ScoutBaseDefenseTerranHarrassFront::ExitState()
{
	return;
}

State* ScoutBaseDefenseTerranHarrassFront::TestTransitions()
{
	return NULL;
}

std::string ScoutBaseDefenseTerranHarrassFront::toString()
{
	return "harass front";
}

#pragma endregion



#pragma region StalkerBaseDefenseTerran

StalkerBaseDefenseTerran::StalkerBaseDefenseTerran(TheBigBot* agent, std::string name, const Unit* stalker, std::vector<Point2D> front_of_base) {
	this->agent = agent;
	this->name = name;
	current_state = new StalkerBaseDefenseTerranDefendFront(agent, this);
	this->stalker = stalker;
	this->front_of_base = front_of_base;

	event_id = agent->GetUniqueId();
	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	current_state->EnterState();
}
StalkerBaseDefenseTerran::~StalkerBaseDefenseTerran()
{
	agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

void StalkerBaseDefenseTerran::OnUnitDestroyedListener(const Unit* unit)
{
	if (unit == target)
		target = NULL;
}

#pragma endregion


}