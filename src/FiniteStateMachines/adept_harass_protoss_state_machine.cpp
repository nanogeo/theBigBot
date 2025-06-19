
#include "adept_harass_protoss_state_machine.h"
#include "mediator.h"


namespace sc2 {


#pragma region AdeptHarassProtossMoveAcross

void AdeptHarassProtossMoveAcross::TickState()
{
	/*if (state_machine->target == nullptr)
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
			mediator->SetUnitCommand(state_machine->adept, A_MOVE, mediator->GetAdeptScoutRunawayLocation());
			if (state_machine->frame_shade_used + 254 < mediator->GetGameLoop())
				mediator->SetUnitCommand(state_machine->adept, A_SHADE, mediator->GetAdeptScoutRunawayLocation());

			if (state_machine->shade != nullptr)
				mediator->SetUnitCommand(state_machine->shade, A_MOVE, mediator->GetAdeptScoutRunawayLocation());
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
				mediator->SetUnitCommand(state_machine->adept, A_ATTACK, state_machine->target);
				state_machine->attack_status = true;
			}
			else
			{
				mediator->SetUnitCommand(state_machine->adept, A_MOVE, state_machine->target->pos);
				if (state_machine->frame_shade_used + 254 < mediator->GetGameLoop())
					mediator->SetUnitCommand(state_machine->adept, A_SHADE, mediator->GetAdeptScoutRunawayLocation());

			}
		}
		else if (state_machine->adept->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
		if (state_machine->shade != nullptr)
		{
			mediator->SetUnitCommand(state_machine->shade, A_MOVE, mediator->GetAdeptScoutRunawayLocation());
		}
	}*/

	mediator->SetUnitsCommand(state_machine->adepts, A_MOVE, state_machine->consolidation_points[0], 0);

}

void AdeptHarassProtossMoveAcross::EnterState()
{
	return;
}

void AdeptHarassProtossMoveAcross::ExitState()
{
	return;
}

State* AdeptHarassProtossMoveAcross::TestTransitions()
{
	for (const auto adept : state_machine->adepts)
	{
		if (Distance2D(adept->pos, state_machine->consolidation_points[0]) > 3)
			return nullptr;
	}
	return new AdeptHarassProtossConsolidate(mediator, state_machine);
}

std::string AdeptHarassProtossMoveAcross::toString()
{
	return "adepts move across";
}

#pragma endregion

#pragma region AdeptHarassProtossConsolidate

void AdeptHarassProtossConsolidate::TickState()
{
	Point2D adept_center = Utility::MedianCenter(state_machine->adepts);
	const Unit* closest_enemy = Utility::ClosestUnitTo(mediator->GetUnits(Unit::Alliance::Enemy), adept_center);
	for (const auto& adept : state_machine->adepts)
	{
		float dist = Distance2D(adept_center, closest_enemy->pos);
		if (dist < 9/*Utility::GetRange(closest_enemy) + 2*/)
		{
			mediator->SetUnitCommand(adept, A_MOVE, Utility::PointBetween(closest_enemy->pos, adept_center, dist + 2), 0);
		}
		else
		{
			mediator->SetUnitCommand(adept, A_MOVE, state_machine->consolidation_points[state_machine->index], 0);
		}
	}
}

void AdeptHarassProtossConsolidate::EnterState()
{
	return;
}

void AdeptHarassProtossConsolidate::ExitState()
{
	return;
}

State* AdeptHarassProtossConsolidate::TestTransitions()
{
	if (state_machine->frame_shade_used + 255 < mediator->GetGameLoop())
	{
		if (Distance2D(Utility::MedianCenter(state_machine->adepts), state_machine->consolidation_points[state_machine->index]) < 1)
		{
			return new AdeptHarassProtossShadeIntoBase(mediator, state_machine);
		}
		else if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), Utility::MedianCenter(state_machine->adepts)) < 8)
		{
			return new AdeptHarassProtossShadeToOtherSide(mediator, state_machine);
		}
	}
	return nullptr;
}

std::string AdeptHarassProtossConsolidate::toString()
{
	return "adepts consolidate";
}

#pragma endregion

#pragma region AdeptHarassProtossShadeIntoBase

void AdeptHarassProtossShadeIntoBase::TickState()
{
	Point2D adept_center = Utility::MedianCenter(state_machine->adepts);
	const Unit* closest_enemy = Utility::ClosestUnitTo(mediator->GetUnits(Unit::Alliance::Enemy), adept_center);
	for (const auto& adept : state_machine->adepts)
	{
		float dist = Distance2D(adept_center, closest_enemy->pos);
		if (dist < 8/*Utility::GetRange(closest_enemy) + 2*/)
		{
			mediator->SetUnitCommand(adept, A_MOVE, Utility::PointBetween(closest_enemy->pos, adept_center, dist + 1), 0);
		}
		else
		{
			mediator->SetUnitCommand(adept, A_MOVE, state_machine->consolidation_points[state_machine->index], 0);
		}
	}
}

void AdeptHarassProtossShadeIntoBase::EnterState()
{
	mediator->SetUnitsCommand(state_machine->adepts, A_SHADE, mediator->GetEnemyStartLocation(), 0);
	state_machine->frame_shade_used = mediator->GetGameLoop();
	return;
}

void AdeptHarassProtossShadeIntoBase::ExitState()
{
	return;
}

State* AdeptHarassProtossShadeIntoBase::TestTransitions()
{
	if (state_machine->frame_shade_used + 155 < mediator->GetGameLoop())
	{
		// last chance to cancel shade
		Point3D enemy_start_location = mediator->ToPoint3D(mediator->GetEnemyStartLocation());
		for (const auto& shade : state_machine->shades)
		{
			if (shade->pos.z < enemy_start_location.z - .1 || shade->pos.z > enemy_start_location.z + .1)
			{
				for (const auto& adept : state_machine->adepts)
				{
					mediator->SetUnitCommand(adept, A_CANCEL_SHADE, 0);
				}
				return new AdeptHarassProtossConsolidate(mediator, state_machine);
			}
		}
		return new AdeptHarassProtossKillProbes(mediator, state_machine);
	}
	return nullptr;
}

std::string AdeptHarassProtossShadeIntoBase::toString()
{
	return "adepts shade into base";
}

#pragma endregion

#pragma region AdeptHarassProtossShadeToOtherSide

void AdeptHarassProtossShadeToOtherSide::TickState()
{
	Point2D adept_center = Utility::MedianCenter(state_machine->adepts);
	const Unit* closest_enemy = Utility::ClosestUnitTo(mediator->GetUnits(Unit::Alliance::Enemy), adept_center);
	for (const auto& adept : state_machine->adepts)
	{
		float dist = Distance2D(adept_center, closest_enemy->pos);
		if (dist < 9/*Utility::GetRange(closest_enemy) + 2*/)
		{
			mediator->SetUnitCommand(adept, A_MOVE, Utility::PointBetween(closest_enemy->pos, adept_center, dist + 2), 0);
		}
		else
		{
			mediator->SetUnitCommand(adept, A_MOVE, state_machine->consolidation_points[state_machine->index], 0);
		}
	}
}

void AdeptHarassProtossShadeToOtherSide::EnterState()
{
	mediator->SetUnitsCommand(state_machine->adepts, A_SHADE, state_machine->consolidation_points[(state_machine->index * -1) + 1], 0);
	state_machine->frame_shade_used = mediator->GetGameLoop();
	return;
}

void AdeptHarassProtossShadeToOtherSide::ExitState()
{
	return;
}

State* AdeptHarassProtossShadeToOtherSide::TestTransitions()
{
	if (state_machine->frame_shade_used + 155 < mediator->GetGameLoop())
	{
		int swap_sides = 0;
		for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy))
		{
			if (unit->unit_type != PROBE)
			{
				if (Distance2D(unit->pos, state_machine->consolidation_points[0]) < 6)
				{
					if (state_machine->index == 0)
						swap_sides++;
					else
						swap_sides--;
				}
				else if (Distance2D(unit->pos, state_machine->consolidation_points[1]) < 6)
				{
					if (state_machine->index == 1)
						swap_sides++;
					else
						swap_sides--;
				}
			}
		}
		if (swap_sides > 0)
		{
			state_machine->index = (state_machine->index * -1) + 1;
			return new AdeptHarassProtossConsolidate(mediator, state_machine);
		}
		else
		{
			mediator->SetUnitsCommand(state_machine->adepts, A_CANCEL_SHADE, 0);
			return new AdeptHarassProtossConsolidate(mediator, state_machine);
		}
	}
	return nullptr;
}

std::string AdeptHarassProtossShadeToOtherSide::toString()
{
	return "adepts shade across";
}

#pragma endregion

#pragma region AdeptHarassProtossKillProbes

void AdeptHarassProtossKillProbes::TickState()
{
	if (state_machine->attack_status)
	{
		for (const auto& adept : state_machine->adepts)
		{
			if (adept->weapon_cooldown == 0)
				return;
		}
		state_machine->attack_status = false;
	}
	Units enemy_probes = mediator->GetUnits(IsEnemyUnit(PROBE));
	Point2D center = Utility::MedianCenter(state_machine->adepts);
	if (enemy_probes.size() > 0)
	{
		if (mediator->GetGameLoop() > state_machine->frame_shade_used + 255)
		{
			mediator->SetUnitsCommand(state_machine->adepts, A_SHADE, Utility::FurthestFrom(enemy_probes, center)->pos, 0);
		}
		if (state_machine->shades.size() > 0)
		{
			mediator->SetUnitsCommand(state_machine->shades, A_MOVE, Utility::FurthestFrom(enemy_probes, center)->pos, 0);
		}
		const Unit* closest_probe = Utility::ClosestTo(enemy_probes, center);
		if (Utility::DistanceToFurthest(state_machine->adepts, closest_probe->pos) < 4)
		{
			mediator->SetUnitsCommand(state_machine->adepts, A_ATTACK, closest_probe, 0);
			state_machine->attack_status = true;
		}
		else
		{
			mediator->SetUnitsCommand(state_machine->adepts, A_MOVE, closest_probe->pos, 0);
		}
	}
	else
	{
		Point2D enemy_start_pos = mediator->GetEnemyStartLocation();
		float dist = Distance2D(center, enemy_start_pos);
		if (mediator->GetGameLoop() > state_machine->frame_shade_used + 255)
		{
			mediator->SetUnitsCommand(state_machine->adepts, A_SHADE, Utility::PointBetween(center, enemy_start_pos, dist + 10), 0);
		}
		if (state_machine->shades.size() > 0)
		{
			mediator->SetUnitsCommand(state_machine->shades, A_MOVE, Utility::PointBetween(center, enemy_start_pos, dist + 10), 0);
		}
		mediator->SetUnitsCommand(state_machine->adepts, A_MOVE, Utility::PointBetween(center, enemy_start_pos, dist + 6), 0);
	}
}

void AdeptHarassProtossKillProbes::EnterState()
{
	return;
}

void AdeptHarassProtossKillProbes::ExitState()
{
	return;
}

State* AdeptHarassProtossKillProbes::TestTransitions()
{
	return nullptr;
}

std::string AdeptHarassProtossKillProbes::toString()
{
	return "adepts kill probes";
}

#pragma endregion


#pragma region AdeptHarassProtoss

AdeptHarassProtoss::AdeptHarassProtoss(Mediator* mediator, std::string name, Units adepts,
	std::vector<Point2D> consolidation_points) : StateMachine(mediator, name)
{
	this->adepts = adepts;
	this->consolidation_points = consolidation_points;

	attack_status = false;

	current_state = new AdeptHarassProtossMoveAcross(mediator, this);

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

AdeptHarassProtoss::~AdeptHarassProtoss()
{
	mediator->RemoveListenerToOnUnitCreatedEvent(event_id);
	mediator->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

void AdeptHarassProtoss::OnUnitCreatedListener(const Unit* unit)
{
	if (unit->unit_type == ADEPT_SHADE && Utility::DistanceToClosest(adepts, unit->pos) < .5)
	{
		frame_shade_used = mediator->GetGameLoop();
		shades.push_back(unit);
	}
}

void AdeptHarassProtoss::OnUnitDestroyedListener(const Unit* unit)
{
	if (unit == target)
	{
		target = nullptr;
	}
	else
	{
		auto shade = std::find(shades.begin(), shades.end(), unit);
		if (shade != shades.end())
			shades.erase(shade);
	}
}

#pragma endregion

}