
#include "door_guard_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {


	DoorGuardArmyGroup::DoorGuardArmyGroup(Mediator* mediator, Point2D door_closed_pos, Point2D door_open_pos) : ArmyGroup(mediator)
	{
		unit_types = { ZEALOT, ADEPT, STALKER };

		desired_units = 1;
		max_units = 1;

		this->door_open_pos = door_open_pos;
		this->door_closed_pos = door_closed_pos;
	}

	void DoorGuardArmyGroup::Run()
	{
		if (guard == nullptr)
		{
			if (all_units.size() > 0)
				guard = all_units[0];
			else
				return;
		}

		Units enemies = mediator->GetUnits(Unit::Alliance::Enemy, IsNotFlyingUnit());
		for (int i = 0; i < enemies.size(); i++)
		{
			if (enemies[i]->is_building)
			{
				enemies.erase(enemies.begin() + i);
				i--;
			}
		}

		// no enemies
		if (enemies.size() == 0)
		{
			mediator->SetUnitCommand(guard, A_MOVE, door_open_pos, 0);
			return;
		}

		const Unit* closest_to_door = Utility::ClosestTo(enemies, door_closed_pos);
		double dist_to_closest = Distance2D(closest_to_door->pos, door_closed_pos);

		// no enemies near
		if (dist_to_closest > 10)
		{
			mediator->SetUnitCommand(guard, A_MOVE, door_open_pos, 0);
			return;
		}

		if (mediator->GetAttackStatus(guard))
			return;

		Point2D guard_move_to = Utility::PointBetween(door_closed_pos, closest_to_door->pos, (float)std::min(4.0, std::max(0.0, (dist_to_closest / 2) - 1)));

		// TODO use fire control to find the best target
		const Unit* closest_to_guard = Utility::ClosestTo(enemies, guard->pos);

		if (Distance2D(closest_to_guard->pos, guard->pos) < Utility::RealRange(guard, closest_to_guard))
		{
			if (mediator->GetAttackStatus(guard))
			{
				if (Distance2D(guard->pos, guard_move_to) > .5 &&
					Distance2D(guard->pos, Utility::ClosestTo(mediator->GetUnits(IsUnit(NEXUS)), guard->pos)->pos) >
					Distance2D(door_closed_pos, Utility::ClosestTo(mediator->GetUnits(IsUnit(NEXUS)), guard->pos)->pos))
				{
					mediator->CancelAttack(guard);
					mediator->SetUnitCommand(guard, A_MOVE, guard_move_to, 0);
				}
			}
			else if (guard->weapon_cooldown == 0)
			{
				mediator->AddUnitToAttackers(guard);
			}
			else
			{
				mediator->SetUnitCommand(guard, A_MOVE, guard_move_to, 0);
			}
		}
		else if (mediator->GetAttackStatus(guard) == false || Distance2D(guard->pos, guard_move_to) > .5)
		{
			mediator->SetUnitCommand(guard, A_MOVE, guard_move_to, 0);
		}
	}

	void DoorGuardArmyGroup::AddNewUnit(const Unit* unit)
	{
		AddUnit(unit);
		if (guard == nullptr)
			guard = unit;
	}

	void DoorGuardArmyGroup::RemoveUnit(const Unit* unit)
	{
		if (guard == unit)
			guard = nullptr;

		ArmyGroup::RemoveUnit(unit);
	}

}