
#include "adept_base_defense_terran_state_machine.h"
#include "mediator.h"

namespace sc2 {


#pragma region AdeptBaseDefenseTerranClearBase

// TODO adjust distances 4, 20, 15
void AdeptBaseDefenseTerranClearBase::TickState()
{
	if (state_machine->adept == nullptr)
		return;
	if (checked_dead_space == false)
	{
		if (Distance2D(state_machine->adept->pos, state_machine->dead_space_spot) < 4)
		{
			checked_dead_space = true;
		}
		else
		{
			mediator->SetUnitCommand(state_machine->adept, A_MOVE, state_machine->dead_space_spot, CommandPriority::low);
			return;
		}
	}
	if (state_machine->target == nullptr)
	{
		for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy))
		{
			if (Distance2D(unit->pos, mediator->GetStartLocation()) < 20 || Distance2D(unit->pos, mediator->GetNaturalLocation()) < 15)
			{
				state_machine->target = unit;
				break;
			}
		}
		if (state_machine->target == nullptr)
		{
			mediator->SetUnitCommand(state_machine->adept, A_MOVE, state_machine->front_of_base[0], CommandPriority::low);
		}
	}
	else
	{
		if (state_machine->attack_status == false)
		{
			// TODO move infront of units based on distance away
			if (Distance2D(state_machine->target->pos, state_machine->adept->pos) <= 4 && state_machine->adept->weapon_cooldown == 0)
			{
				mediator->SetUnitCommand(state_machine->adept, A_ATTACK, state_machine->target, CommandPriority::low);
				state_machine->attack_status = true;
			}
			else
			{
				mediator->SetUnitCommand(state_machine->adept, A_MOVE, state_machine->target->pos, CommandPriority::low);
				if (state_machine->frame_shade_used + 225 < mediator->GetGameLoop()) // TODO should be 246?
					mediator->SetUnitCommand(state_machine->adept, A_SHADE, state_machine->target->pos, CommandPriority::low);

			}
		}
		else if (state_machine->adept->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
		if (state_machine->shade != nullptr)
		{
			mediator->SetUnitCommand(state_machine->shade, A_MOVE, state_machine->target->pos, CommandPriority::low);
		}
	}
}

void AdeptBaseDefenseTerranClearBase::EnterState()
{
	if (state_machine->adept == nullptr)
		return;
	mediator->SetUnitCommand(state_machine->adept, A_MOVE, state_machine->dead_space_spot, CommandPriority::low);
}

void AdeptBaseDefenseTerranClearBase::ExitState()
{
	return;
}

State* AdeptBaseDefenseTerranClearBase::TestTransitions()
{
	if (state_machine->adept == nullptr)
		return nullptr;
	if (Distance2D(state_machine->adept->pos, state_machine->front_of_base[0]) < 5 || Distance2D(state_machine->adept->pos, state_machine->front_of_base[1]) < 5)
		return new AdeptBaseDefenseTerranDefendFront(mediator, state_machine);

	if (state_machine->target == nullptr)
	{
		for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy))
		{
			if (Distance2D(unit->pos, mediator->GetStartLocation()) < 20 || Distance2D(unit->pos, mediator->GetNaturalLocation()) < 15)
			{
				state_machine->target = unit;
				break;
			}
		}
	}
	return nullptr;
}

std::string AdeptBaseDefenseTerranClearBase::toString() const
{
	return "clear base";
}

#pragma endregion

#pragma region AdeptBaseDefenseTerranDefendFront

#pragma warning(push)
#pragma warning(disable : 4702)
// TODO change for maps with natural ramp
void AdeptBaseDefenseTerranDefendFront::TickState()
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
				mediator->SetUnitCommand(state_machine->adept, A_MOVE, state_machine->front_of_base[1], CommandPriority::low);
				if (Distance2D(state_machine->adept->pos, state_machine->front_of_base[1]) < 1)
					forward = false;
			}
			else
			{
				mediator->SetUnitCommand(state_machine->adept, A_MOVE, state_machine->front_of_base[0], CommandPriority::low);
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
				mediator->SetUnitCommand(state_machine->adept, A_ATTACK, state_machine->target, CommandPriority::low);
				state_machine->attack_status = true;
			}
			else
			{
				mediator->SetUnitCommand(state_machine->adept, A_MOVE, state_machine->target->pos, CommandPriority::low);
				if (state_machine->frame_shade_used + 225 < mediator->GetGameLoop())// TODO should be 246?
					mediator->SetUnitCommand(state_machine->adept, A_SHADE, state_machine->target->pos, CommandPriority::low);

			}
		}
		else if (state_machine->adept->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
		if (state_machine->shade != nullptr)
		{
			mediator->SetUnitCommand(state_machine->shade, A_MOVE, state_machine->target->pos, CommandPriority::low);
		}
	}

}
#pragma warning(pop)

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
	Units gates = mediator->GetUnits(IsFriendlyUnit(GATEWAY));
	Units other_units = mediator->GetUnits(IsUnits({ ADEPT, STALKER }));
	if (other_units.size() > 1 || mediator->GetFirstBarrackProduction() != reaper || (gates.size() > 0 && gates[0]->orders.size() > 0 && gates[0]->orders[0].progress > .9))
		return new AdeptBaseDefenseTerranMoveAcross(mediator, state_machine);
	return nullptr;
}

std::string AdeptBaseDefenseTerranDefendFront::toString() const
{
	return "defend front";
}

#pragma endregion

#pragma region AdeptBaseDefenseTerranMoveAcross

void AdeptBaseDefenseTerranMoveAcross::TickState()
{
	if (state_machine->target == nullptr)
	{
		for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy))
		{
			if (Distance2D(state_machine->adept->pos, unit->pos) < 8)
			{
				state_machine->target = unit;
				break;
			}
		}
		if (state_machine->target == nullptr)
		{
			mediator->SetUnitCommand(state_machine->adept, A_MOVE, mediator->GetLocations().adept_scout_runaway, CommandPriority::low);
			if (state_machine->frame_shade_used + 225 < mediator->GetGameLoop()) // TODO should be 246?
				mediator->SetUnitCommand(state_machine->adept, A_SHADE, mediator->GetLocations().adept_scout_runaway, CommandPriority::low);

			if (state_machine->shade != nullptr)
				mediator->SetUnitCommand(state_machine->shade, A_MOVE, mediator->GetLocations().adept_scout_runaway, CommandPriority::low);
		}
	}
	else
	{
		if (Distance2D(state_machine->adept->pos, state_machine->target->pos) > 8)
		{
			state_machine->target = nullptr;
			return;
		}

		if (state_machine->attack_status == false)
		{
			// TODO move infront of units based on distance away
			if (Distance2D(state_machine->target->pos, state_machine->adept->pos) <= 4 && state_machine->adept->weapon_cooldown == 0)
			{
				mediator->SetUnitCommand(state_machine->adept, A_ATTACK, state_machine->target, CommandPriority::low);
				state_machine->attack_status = true;
			}
			else
			{
				mediator->SetUnitCommand(state_machine->adept, A_MOVE, state_machine->target->pos, CommandPriority::low);
				if (state_machine->frame_shade_used + 225 < mediator->GetGameLoop()) // TODO should be 246?
					mediator->SetUnitCommand(state_machine->adept, A_SHADE, mediator->GetLocations().adept_scout_runaway, CommandPriority::low);

			}
		}
		else if (state_machine->adept->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
		if (state_machine->shade != nullptr)
		{
			mediator->SetUnitCommand(state_machine->shade, A_MOVE, mediator->GetLocations().adept_scout_runaway, CommandPriority::low);
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
	if (Distance2D(state_machine->adept->pos, mediator->GetLocations().adept_scout_runaway) < 3)
		return new AdeptBaseDefenseTerranScoutBase(mediator, state_machine, mediator->GetLocations().adept_scout_shade, mediator->GetLocations().adept_scout_runaway,
			mediator->GetLocations().adept_scout_ramptop, mediator->GetLocations().adept_scout_nat_path, mediator->GetLocations().adept_scout_base_spots);
	return nullptr;
}

std::string AdeptBaseDefenseTerranMoveAcross::toString() const
{
	return "move across map";
}

#pragma endregion

#pragma region AdeptBaseDefenseTerranScoutBase

void AdeptBaseDefenseTerranScoutBase::TickState()
{
	Point2D furthest_point = adept_scout_shade;
	Units enemy_units = mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
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
	//agent->Debug()->DebugSphereOut(mediator->ToPoint3D(furthest_point), .5, Color(255, 255, 0));

	const Unit* closest_unit = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), state_machine->adept->pos);

	if (state_machine->attack_status == true)
	{
		if (state_machine->adept->weapon_cooldown > 0)
			state_machine->attack_status = false;
	}
	else if (state_machine->adept->shield < 5 || state_machine->adept->weapon_cooldown > 0)
	{
		if (state_machine->adept->shield < 5)
			shields_regening = true;
		mediator->SetUnitCommand(state_machine->adept, A_MOVE, furthest_point, CommandPriority::low);
	}
	else if (shields_regening)
	{
		if (state_machine->adept->shield >= 65)
			shields_regening = false;
		mediator->SetUnitCommand(state_machine->adept, A_MOVE, furthest_point, CommandPriority::low);
	}
	else if (Distance2D(closest_unit->pos, state_machine->adept->pos) <= 4)
	{
		mediator->SetUnitCommand(state_machine->adept, A_ATTACK, closest_unit, CommandPriority::low);
		state_machine->attack_status = true;
	}
	else
	{
		mediator->SetUnitCommand(state_machine->adept, A_MOVE, adept_scout_shade, CommandPriority::low);
	}
	/*else if (Distance2D(closest_unit->pos, state_machine->adept->pos) <= 4)
	{
		mediator->SetUnitCommand(state_machine->adept, A_ATTACK, closest_unit, 0);
		state_machine->attack_status = true;
	}
	else
	{
		if (shields_regening)
		{
			if (state_machine->adept->shield >= 65)
				shields_regening = false;
			mediator->SetUnitCommand(state_machine->adept, A_MOVE, furthest_point, 0);
		}
		else
		{
			mediator->SetUnitCommand(state_machine->adept, A_MOVE, adept_scout_shade, 0);
		}
	}*/


	if (mediator->GetGameLoop() > state_machine->frame_shade_used + 225) // TODO should be 246?
	{
		mediator->SetUnitCommand(state_machine->adept, A_SHADE, shade_target, CommandPriority::low);
	}
	/*else if ((state_machine->shade != nullptr || Utility::DangerLevelAt(state_machine->adept, Utility::PointBetween(state_machine->adept->pos, adept_scout_shade, 1), agent->Observation()) > 0) && Distance2D(state_machine->adept->pos, adept_scout_runaway) > 1)
	{
		mediator->SetUnitCommand(state_machine->adept, A_MOVE, adept_scout_runaway, 0);
	}
	else if ((state_machine->shade == nullptr || Utility::DangerLevel(state_machine->adept, agent->Observation()) == 0) && Distance2D(state_machine->adept->pos, adept_scout_shade) > 1)
	{
		mediator->SetUnitCommand(state_machine->adept, A_MOVE, adept_scout_shade, 0);
	}*/

	if (state_machine->shade != nullptr)
	{
		if (mediator->GetGameLoop() > state_machine->frame_shade_used + 150)
		{
			mediator->SetUnitCommand(state_machine->adept, A_CANCEL_SHADE, CommandPriority::low);
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

			mediator->SetUnitCommand(state_machine->shade, A_MOVE, shade_target, CommandPriority::low);
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
	return nullptr;
}

std::string AdeptBaseDefenseTerranScoutBase::toString() const
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

AdeptBaseDefenseTerran::AdeptBaseDefenseTerran(Mediator* mediator, std::string name, Point2D dead_space_spot, 
	std::vector<Point2D> front_of_base) : StateMachine(mediator, name)
{
	current_state = new AdeptBaseDefenseTerranClearBase(mediator, this);
	this->dead_space_spot = dead_space_spot;
	this->front_of_base = front_of_base;

	event_id = mediator->GetUniqueId();
	std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
		this->OnUnitCreatedListener(unit);
	};
	mediator->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	current_state->EnterState();
}
AdeptBaseDefenseTerran::~AdeptBaseDefenseTerran()
{
	mediator->RemoveListenerToOnUnitCreatedEvent(event_id);
	mediator->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

bool AdeptBaseDefenseTerran::AddUnit(const Unit* unit)
{
	if (unit->unit_type != ADEPT || adept != nullptr)
		return false;
	adept = unit;
	mediator->SetUnitCommand(adept, A_MOVE, dead_space_spot, CommandPriority::low);
	return true;
}

void AdeptBaseDefenseTerran::OnUnitCreatedListener(const Unit* unit)
{
	if (shade == nullptr && unit->unit_type == ADEPT_SHADE && Distance2D(unit->pos, adept->pos) < .5)
	{
		shade = unit;
		frame_shade_used = mediator->GetGameLoop();
	}
}

void AdeptBaseDefenseTerran::OnUnitDestroyedListener(const Unit* unit)
{
	if (unit == target)
		target = nullptr;
	else if (unit == shade)
		shade = nullptr;
}

#pragma endregion


}