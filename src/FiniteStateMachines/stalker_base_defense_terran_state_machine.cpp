
#include "stalker_base_defense_terran_state_machine.h"
#include "theBigBot.h"

namespace sc2 {


#pragma region StalkerBaseDefenseTerranDefendFront

#pragma warning(push)
#pragma warning(disable : 4702)
// TODO change for maps with natural ramp
void StalkerBaseDefenseTerranDefendFront::TickState()
{
	if (state_machine->target == nullptr)
	{
		for (const auto& unit : mediator->GetUnits(IsUnit(REAPER)))
		{
			state_machine->target = unit;
			break;
		}
		if (state_machine->target == nullptr)
		{
			if (forward)
			{
				mediator->SetUnitCommand(state_machine->stalker, A_MOVE, state_machine->front_of_base[1], CommandPriority::low);
				if (Distance2D(state_machine->stalker->pos, state_machine->front_of_base[1]) < 1)
					forward = false;
			}
			else
			{
				mediator->SetUnitCommand(state_machine->stalker, A_MOVE, state_machine->front_of_base[0], CommandPriority::low);
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
				mediator->SetUnitCommand(state_machine->stalker, A_ATTACK, state_machine->target, CommandPriority::low);
				state_machine->attack_status = true;
			}
			else
			{
				mediator->SetUnitCommand(state_machine->stalker, A_MOVE, state_machine->target->pos, CommandPriority::low);

			}
		}
		else if (state_machine->stalker->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
	}

}
#pragma warning(pop)

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
	Units gates = mediator->GetUnits(IsFriendlyUnit(GATEWAY));
	Units other_units = mediator->GetUnits(IsUnits({ ADEPT, STALKER }));
	if (other_units.size() > 1 || mediator->GetFirstBarrackProduction() != reaper || (gates.size() > 0 && gates[0]->orders.size() > 0 && gates[0]->orders[0].progress > .9))
		return new StalkerBaseDefenseTerranMoveAcross(mediator, state_machine);
	return nullptr;
}

std::string StalkerBaseDefenseTerranDefendFront::toString() const
{
	return "defend front";
}

#pragma endregion

#pragma region StalkerBaseDefenseTerranMoveAcross

void StalkerBaseDefenseTerranMoveAcross::TickState()
{
	if (state_machine->target == nullptr)
	{
		for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy))
		{
			if (Distance2D(state_machine->stalker->pos, unit->pos) < 8)
			{
				state_machine->target = unit;
				break;
			}
		}
		if (state_machine->target == nullptr)
		{
			mediator->SetUnitCommand(state_machine->stalker, A_MOVE, mediator->GetLocations().adept_scout_runaway, CommandPriority::low);
		}
	}
	else
	{
		if (Distance2D(state_machine->stalker->pos, state_machine->target->pos) > 8)
		{
			state_machine->target = nullptr;
			return;
		}

		if (state_machine->attack_status == false)
		{
			// TODO move infront of units based on distance away
			if (Distance2D(state_machine->target->pos, state_machine->stalker->pos) <= 4 && state_machine->stalker->weapon_cooldown == 0)
			{
				mediator->SetUnitCommand(state_machine->stalker, A_ATTACK, state_machine->target, CommandPriority::low);
				state_machine->attack_status = true;
			}
			else
			{
				mediator->SetUnitCommand(state_machine->stalker, A_MOVE, state_machine->target->pos, CommandPriority::low);

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
	if (Distance2D(state_machine->stalker->pos, mediator->GetLocations().adept_scout_runaway) < 3)
		return new ScoutBaseDefenseTerranHarrassFront(mediator, state_machine, mediator->GetLocations().adept_scout_shade, mediator->GetLocations().adept_scout_runaway);
	return nullptr;
}

std::string StalkerBaseDefenseTerranMoveAcross::toString() const
{
	return "move across map";
}

#pragma endregion

#pragma region ScoutBaseDefenseTerranHarrassFront

void ScoutBaseDefenseTerranHarrassFront::TickState()
{
	Point2D furthest_point = attack_pos;
	Units enemy_units = mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
	for (const auto& unit : enemy_units)
	{
		double range = Utility::RealRange(unit, state_machine->stalker) + 2;
		Point2D intersection = mediator->GetIndirectAttackLine().GetPointFrom(unit->pos, (float)range, false);
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

	const Unit* closest_unit = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), state_machine->stalker->pos);

	if (state_machine->attack_status == true)
	{
		if (state_machine->stalker->weapon_cooldown > 0)
			state_machine->attack_status = false;
	}
	else if (state_machine->stalker->shield < 5 || state_machine->stalker->weapon_cooldown > 0)
	{
		if (state_machine->stalker->shield < 5)
			shields_regening = true;
		mediator->SetUnitCommand(state_machine->stalker, A_MOVE, furthest_point, CommandPriority::low);
	}
	else if (shields_regening)
	{
		if (state_machine->stalker->shield >= 75)
			shields_regening = false;
		mediator->SetUnitCommand(state_machine->stalker, A_MOVE, furthest_point, CommandPriority::low);
	}
	else if (Distance2D(closest_unit->pos, state_machine->stalker->pos) <= 6)
	{
		mediator->SetUnitCommand(state_machine->stalker, A_ATTACK, closest_unit, CommandPriority::low);
		state_machine->attack_status = true;
	}
	else
	{
		mediator->SetUnitCommand(state_machine->stalker, A_MOVE, closest_unit, CommandPriority::low);
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
	return nullptr;
}

std::string ScoutBaseDefenseTerranHarrassFront::toString() const
{
	return "harass front";
}

#pragma endregion



#pragma region StalkerBaseDefenseTerran

StalkerBaseDefenseTerran::StalkerBaseDefenseTerran(Mediator* mediator, std::string name, const Unit* stalker, 
	std::vector<Point2D> front_of_base) : StateMachine(mediator, name)
{
	current_state = new StalkerBaseDefenseTerranDefendFront(mediator, this);
	this->stalker = stalker;
	this->front_of_base = front_of_base;

	event_id = mediator->GetUniqueId();
	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	current_state->EnterState();
}
StalkerBaseDefenseTerran::~StalkerBaseDefenseTerran()
{
	mediator->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

void StalkerBaseDefenseTerran::OnUnitDestroyedListener(const Unit* unit)
{
	if (unit == target)
		target = nullptr;
}

#pragma endregion


}