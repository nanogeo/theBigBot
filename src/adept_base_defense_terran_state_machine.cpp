#pragma once

#include "adept_base_defense_terran_state_machine.h"
#include "theBigBot.h"

namespace sc2 {


#pragma region AdeptBaseDefenseTerranClearBase

// TODO adjust distances 4, 20, 15
void AdeptBaseDefenseTerranClearBase::TickState()
{
	if (state_machine->adept == NULL)
		return;
	if (checked_dead_space == false)
	{
		if (Distance2D(state_machine->adept->pos, state_machine->dead_space_spot) < 4)
		{
			checked_dead_space = true;
		}
		else
		{
			agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, state_machine->dead_space_spot);
			return;
		}
	}
	if (state_machine->target == NULL)
	{
		for (const auto& unit : agent->Observation()->GetUnits(Unit::Alliance::Enemy))
		{
			if (Distance2D(unit->pos, agent->locations->start_location) < 20 || Distance2D(unit->pos, agent->locations->nexi_locations[1]) < 15)
			{
				state_machine->target = unit;
				break;
			}
		}
		if (state_machine->target == NULL)
		{
			agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, state_machine->front_of_base[0]);
		}
	}
	else
	{
		if (state_machine->attack_status == false)
		{
			// TODO move infront of units based on distance away
			if (Distance2D(state_machine->target->pos, state_machine->adept->pos) <= 4 && state_machine->adept->weapon_cooldown == 0)
			{
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::ATTACK_ATTACK, state_machine->target);
				state_machine->attack_status = true;
			}
			else
			{
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, state_machine->target->pos);
				if (state_machine->frame_shade_used + 225 < agent->Observation()->GetGameLoop()) // TODO should be 246?
					agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, state_machine->target->pos);

			}
		}
		else if (state_machine->adept->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
		if (state_machine->shade != NULL)
		{
			agent->Actions()->UnitCommand(state_machine->shade, ABILITY_ID::GENERAL_MOVE, state_machine->target->pos);
		}
	}
}

void AdeptBaseDefenseTerranClearBase::EnterState()
{
	if (state_machine->adept == NULL)
		return;
	agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, state_machine->dead_space_spot);
}

void AdeptBaseDefenseTerranClearBase::ExitState()
{
	return;
}

State* AdeptBaseDefenseTerranClearBase::TestTransitions()
{
	if (state_machine->adept == NULL)
		return NULL;
	if (Distance2D(state_machine->adept->pos, state_machine->front_of_base[0]) < 5 || Distance2D(state_machine->adept->pos, state_machine->front_of_base[1]) < 5)
		return new AdeptBaseDefenseTerranDefendFront(agent, state_machine);

	if (state_machine->target == NULL)
	{
		for (const auto& unit : agent->Observation()->GetUnits(Unit::Alliance::Enemy))
		{
			if (Distance2D(unit->pos, agent->locations->start_location) < 20 || Distance2D(unit->pos, agent->locations->nexi_locations[0]) < 15)
			{
				state_machine->target = unit;
				break;
			}
		}
	}
	return NULL;
}

std::string AdeptBaseDefenseTerranClearBase::toString()
{
	return "clear base";
}

#pragma endregion

#pragma region AdeptBaseDefenseTerranDefendFront

// TODO change for maps with natural ramp
void AdeptBaseDefenseTerranDefendFront::TickState()
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
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, state_machine->front_of_base[1]);
				if (Distance2D(state_machine->adept->pos, state_machine->front_of_base[1]) < 1)
					forward = false;
			}
			else
			{
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, state_machine->front_of_base[0]);
				if (Distance2D(state_machine->adept->pos, state_machine->front_of_base[0]) < 1)
					forward = true;
			}
		}
	}
	else
	{
		if (state_machine->attack_status == false)
		{
			// TODO move infront of units based on distance away
			if (Distance2D(state_machine->target->pos, state_machine->adept->pos) <= 4 && state_machine->adept->weapon_cooldown == 0)
			{
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::ATTACK_ATTACK, state_machine->target);
				state_machine->attack_status = true;
			}
			else
			{
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, state_machine->target->pos);
				if (state_machine->frame_shade_used + 225 < agent->Observation()->GetGameLoop())// TODO should be 246?
					agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, state_machine->target->pos);

			}
		}
		else if (state_machine->adept->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
		if (state_machine->shade != NULL)
		{
			agent->Actions()->UnitCommand(state_machine->shade, ABILITY_ID::GENERAL_MOVE, state_machine->target->pos);
		}
	}

}

void AdeptBaseDefenseTerranDefendFront::EnterState()
{
	return;
}

void AdeptBaseDefenseTerranDefendFront::ExitState()
{
	return;
}

State* AdeptBaseDefenseTerranDefendFront::TestTransitions()
{
	Units gates = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_GATEWAY));
	Units other_units = agent->Observation()->GetUnits(IsUnits({ UNIT_TYPEID::PROTOSS_ADEPT, UNIT_TYPEID::PROTOSS_STALKER }));
	if (other_units.size() > 1 || agent->scout_info_terran.first_rax_production != reaper || (gates.size() > 0 && gates[0]->orders.size() > 0 && gates[0]->orders[0].progress > .9))
		return new AdeptBaseDefenseTerranMoveAcross(agent, state_machine);
	return NULL;
}

std::string AdeptBaseDefenseTerranDefendFront::toString()
{
	return "defend front";
}

#pragma endregion

#pragma region AdeptBaseDefenseTerranMoveAcross

void AdeptBaseDefenseTerranMoveAcross::TickState()
{
	if (state_machine->target == NULL)
	{
		for (const auto& unit : agent->Observation()->GetUnits(Unit::Alliance::Enemy))
		{
			if (Distance2D(state_machine->adept->pos, unit->pos) < 8)
			{
				state_machine->target = unit;
				break;
			}
		}
		if (state_machine->target == NULL)
		{
			agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, agent->locations->adept_scout_runaway);
			if (state_machine->frame_shade_used + 225 < agent->Observation()->GetGameLoop()) // TODO should be 246?
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, agent->locations->adept_scout_runaway);

			if (state_machine->shade != NULL)
				agent->Actions()->UnitCommand(state_machine->shade, ABILITY_ID::GENERAL_MOVE, agent->locations->adept_scout_runaway);
		}
	}
	else
	{
		if (Distance2D(state_machine->adept->pos, state_machine->target->pos) > 8)
		{
			state_machine->target = NULL;
			return;
		}

		if (state_machine->attack_status == false)
		{
			// TODO move infront of units based on distance away
			if (Distance2D(state_machine->target->pos, state_machine->adept->pos) <= 4 && state_machine->adept->weapon_cooldown == 0)
			{
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::ATTACK_ATTACK, state_machine->target);
				state_machine->attack_status = true;
			}
			else
			{
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, state_machine->target->pos);
				if (state_machine->frame_shade_used + 225 < agent->Observation()->GetGameLoop()) // TODO should be 246?
					agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, agent->locations->adept_scout_runaway);

			}
		}
		else if (state_machine->adept->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
		if (state_machine->shade != NULL)
		{
			agent->Actions()->UnitCommand(state_machine->shade, ABILITY_ID::GENERAL_MOVE, agent->locations->adept_scout_runaway);
		}
	}
}

void AdeptBaseDefenseTerranMoveAcross::EnterState()
{
	return;
}

void AdeptBaseDefenseTerranMoveAcross::ExitState()
{
	return;
}

State* AdeptBaseDefenseTerranMoveAcross::TestTransitions()
{
	if (Distance2D(state_machine->adept->pos, agent->locations->adept_scout_runaway) < 3)
		return new AdeptBaseDefenseTerranScoutBase(agent, state_machine, agent->locations->adept_scout_shade, agent->locations->adept_scout_runaway,
			agent->locations->adept_scout_ramptop, agent->locations->adept_scout_nat_path, agent->locations->adept_scout_base_spots);
	return NULL;
}

std::string AdeptBaseDefenseTerranMoveAcross::toString()
{
	return "move across map";
}

#pragma endregion

#pragma region AdeptBaseDefenseTerranScoutBase

void AdeptBaseDefenseTerranScoutBase::TickState()
{
	Point2D furthest_point = adept_scout_shade;
	Units enemy_units = agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
	/*for (const auto& unit : enemy_units)
	{
		double range = Utility::RealRange(unit, state_machine->adept) + 2;
		Point2D intersection = agent->locations->attack_path_line.GetPointFrom(unit->pos, range, false); // TODO use adept runaway not the attack path
		if (intersection != Point2D(0, 0))
		{
			float dist = Distance2D(intersection, adept_scout_shade);
			if (dist > Distance2D(furthest_point, adept_scout_shade))
			{
				furthest_point = intersection;
			}
		}
	}*/
	Units close_enemies = Utility::GetUnitsThatCanAttack(enemy_units, state_machine->adept, 2);
	if (close_enemies.size() > 0)
	{
		const Unit* closest_enemy = close_enemies[0];
		for (const auto& enemy : close_enemies)
		{
			if (Utility::RealRange(enemy, state_machine->adept) - Distance2D(enemy->pos, state_machine->adept->pos) >
				Utility::RealRange(closest_enemy, state_machine->adept) - Distance2D(closest_enemy->pos, state_machine->adept->pos))
				closest_enemy = enemy;
		}
		furthest_point = Utility::PointBetween(closest_enemy->pos, state_machine->adept->pos, 9);
	}
	//agent->Debug()->DebugSphereOut(agent->ToPoint3D(furthest_point), .5, Color(255, 255, 0));

	const Unit* closest_unit = Utility::ClosestTo(agent->Observation()->GetUnits(Unit::Alliance::Enemy), state_machine->adept->pos);

	if (state_machine->attack_status == true)
	{
		if (state_machine->adept->weapon_cooldown > 0)
			state_machine->attack_status = false;
	}
	else if (state_machine->adept->shield < 5 || state_machine->adept->weapon_cooldown > 0)
	{
		if (state_machine->adept->shield < 5)
			shields_regening = true;
		agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, furthest_point);
	}
	else if (shields_regening)
	{
		if (state_machine->adept->shield >= 65)
			shields_regening = false;
		agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, furthest_point);
	}
	else if (Distance2D(closest_unit->pos, state_machine->adept->pos) <= 4)
	{
		agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::ATTACK_ATTACK, closest_unit);
		state_machine->attack_status = true;
	}
	else
	{
		agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, adept_scout_shade);
	}
	/*else if (Distance2D(closest_unit->pos, state_machine->adept->pos) <= 4)
	{
		agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::ATTACK_ATTACK, closest_unit);
		state_machine->attack_status = true;
	}
	else
	{
		if (shields_regening)
		{
			if (state_machine->adept->shield >= 65)
				shields_regening = false;
			agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, furthest_point);
		}
		else
		{
			agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, adept_scout_shade);
		}
	}*/


	if (agent->Observation()->GetGameLoop() > state_machine->frame_shade_used + 225) // TODO should be 246?
	{
		agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, shade_target);
	}
	/*else if ((state_machine->shade != NULL || Utility::DangerLevelAt(state_machine->adept, Utility::PointBetween(state_machine->adept->pos, adept_scout_shade, 1), agent->Observation()) > 0) && Distance2D(state_machine->adept->pos, adept_scout_runaway) > 1)
	{
		agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, adept_scout_runaway);
	}
	else if ((state_machine->shade == NULL || Utility::DangerLevel(state_machine->adept, agent->Observation()) == 0) && Distance2D(state_machine->adept->pos, adept_scout_shade) > 1)
	{
		agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, adept_scout_shade);
	}*/

	if (state_machine->shade != NULL)
	{
		if (agent->Observation()->GetGameLoop() > state_machine->frame_shade_used + 150)
		{
			agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::CANCEL_ADEPTPHASESHIFT);
			UpdateShadeTarget();
		}
		else
		{
			if (Distance2D(state_machine->shade->pos, shade_target) < 1)
			{
				if (shade_target == adept_scout_nat_path[0])
					shade_target = adept_scout_nat_path[1];
				else if (shade_target == adept_scout_ramptop)
					shade_target = adept_scout_base_spots[base_spots_index];
			}

			agent->Actions()->UnitCommand(state_machine->shade, ABILITY_ID::GENERAL_MOVE, shade_target);
		}
	}
}

void AdeptBaseDefenseTerranScoutBase::EnterState()
{
	return;
}

void AdeptBaseDefenseTerranScoutBase::ExitState()
{
	return;
}

State* AdeptBaseDefenseTerranScoutBase::TestTransitions()
{
	return NULL;
}

std::string AdeptBaseDefenseTerranScoutBase::toString()
{
	std::string str = "scout base ";
	if (shade_target == adept_scout_nat_path[0])
		str += "natural 1";
	else if (shade_target == adept_scout_nat_path[1])
		str += "natural 2";
	else if (shade_target == adept_scout_ramptop)
		str += "ramp";
	else
		str += "main " + std::to_string(base_spots_index + 1);
	return str;
}

void AdeptBaseDefenseTerranScoutBase::UpdateShadeTarget()
{
	auto found = std::find(adept_scout_nat_path.begin(), adept_scout_nat_path.end(), shade_target);
	if (found != adept_scout_nat_path.end())
	{
		shade_target = adept_scout_ramptop;
	}
	else
	{
		base_spots_index++;
		if (base_spots_index >= adept_scout_base_spots.size())
			base_spots_index = 0;

		if (base_spots_index % 2 == 0)
		{
			shade_target = adept_scout_nat_path[0];
		}
		else
		{
			shade_target = adept_scout_ramptop;
		}
	}
}

#pragma endregion


#pragma region AdeptBaseDefenseTerran

AdeptBaseDefenseTerran::AdeptBaseDefenseTerran(TheBigBot* agent, std::string name, Point2D dead_space_spot, std::vector<Point2D> front_of_base) {
	this->agent = agent;
	this->name = name;
	current_state = new AdeptBaseDefenseTerranClearBase(agent, this);
	this->dead_space_spot = dead_space_spot;
	this->front_of_base = front_of_base;

	event_id = agent->GetUniqueId();
	std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
		this->OnUnitCreatedListener(unit);
	};
	agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	current_state->EnterState();
}
AdeptBaseDefenseTerran::~AdeptBaseDefenseTerran()
{
	agent->RemoveListenerToOnUnitCreatedEvent(event_id);
	agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

bool AdeptBaseDefenseTerran::AddUnit(const Unit* unit)
{
	if (unit->unit_type != ADEPT || adept != NULL)
		return false;
	adept = unit;
	agent->Actions()->UnitCommand(adept, ABILITY_ID::GENERAL_MOVE, dead_space_spot);
	return true;
}

void AdeptBaseDefenseTerran::OnUnitCreatedListener(const Unit* unit)
{
	if (shade == NULL && unit->unit_type == UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT && Distance2D(unit->pos, adept->pos) < .5)
	{
		shade = unit;
		frame_shade_used = agent->Observation()->GetGameLoop();
	}
}

void AdeptBaseDefenseTerran::OnUnitDestroyedListener(const Unit* unit)
{
	if (unit == target)
		target = NULL;
	else if (unit == shade)
		shade = NULL;
}

#pragma endregion


}