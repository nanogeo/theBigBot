
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>


#include "army_group.h"
#include "theBigBot.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {


	ArmyGroup::~ArmyGroup()
	{

	}

	void ArmyGroup::ScourMap()
	{
		while (new_units.size() > 0)
		{
			AddUnit(new_units[0]);
		}
		Units enemy_buildings = mediator->GetUnits(Unit::Alliance::Enemy, IsBuilding());
		Units enemy_ground_buildings;
		Units enemy_flying_buildings;
		for (const auto& building : enemy_buildings)
		{
			if (building->is_flying)
				enemy_flying_buildings.push_back(building);
			else
				enemy_ground_buildings.push_back(building);
		}
		ImageData raw_map = mediator->GetPathingGrid();
		for (const auto& unit : all_units)
		{
			if (unit->orders.size() == 0)
			{
				if (Utility::GetGroundRange(unit) == 0 && enemy_flying_buildings.size() > 0)
				{
					const Unit* closest = Utility::ClosestTo(enemy_flying_buildings, unit->pos);
					mediator->SetUnitCommand(unit, A_ATTACK, closest->pos, CommandPriorty::low);
				}
				else if (Utility::GetAirRange(unit) == 0 && enemy_ground_buildings.size() > 0)
				{
					const Unit* closest = Utility::ClosestTo(enemy_ground_buildings, unit->pos);
					mediator->SetUnitCommand(unit, A_ATTACK, closest->pos, CommandPriorty::low);
				}
				else if (Utility::GetGroundRange(unit) > 0 &&
					Utility::GetAirRange(unit) > 0 &&
					enemy_buildings.size() > 0)
				{
					const Unit* closest = Utility::ClosestTo(enemy_buildings, unit->pos);
					mediator->SetUnitCommand(unit, A_ATTACK, closest->pos, CommandPriorty::low);
				}
				else
				{
					std::srand((unsigned int)(unit->tag + mediator->GetGameLoop()));
					int x = std::rand() % raw_map.width;
					int y = std::rand() % raw_map.height;
					Point2D pos = Point2D((float)x, (float)y);
					while (!unit->is_flying && !mediator->IsPathable(pos))
					{
						x = std::rand() % raw_map.width;
						y = std::rand() % raw_map.height;
						pos = Point2D((float)x, (float)y);
					}
					mediator->SetUnitCommand(unit, A_ATTACK, pos, CommandPriorty::low);
				}
			}
		}
	}

	void ArmyGroup::AddUnit(const Unit* unit)
	{
		if (std::find(all_units.begin(), all_units.end(), unit) != all_units.end())
			return;

		all_units.push_back(unit);
		new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());
	}

	void ArmyGroup::AddNewUnit(const Unit* unit)
	{
		if (!ready)
		{
			AddUnit(unit);
			return;
		}

		if (std::find(new_units.begin(), new_units.end(), unit) != new_units.end())
			return;

		new_units.push_back(unit);
	}

	void ArmyGroup::RemoveUnit(const Unit* unit)
	{
		if (unit == nullptr)
			return; // TODO log error and callstack

		mediator->SetUnitCommand(unit, A_STOP, CommandPriorty::normal);

		all_units.erase(std::remove(all_units.begin(), all_units.end(), unit), all_units.end());
		new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());
	}

	Units ArmyGroup::GetExtraUnits() const
	{
		Units extra_units;
		int extra_num = (int)(all_units.size() + new_units.size() - desired_units);
		if (extra_num <= 0)
			return extra_units;

		for (int i = (int)new_units.size() - 1; i >= 0; i --)
		{
			extra_units.push_back(new_units[i]);
			if (extra_units.size() == extra_num)
				return extra_units;
		}
		for (int i = (int)all_units.size() - 1; i >= 0; i--)
		{
			extra_units.push_back(all_units[i]);
			if (extra_units.size() == extra_num)
				break;
		}
		return extra_units;
	}

	int ArmyGroup::GetDesiredUnits() const
	{
		return desired_units;
	}

	void ArmyGroup::AddDesiredUnits(int num)
	{
		desired_units += num;
	}

	void ArmyGroup::AddMaxUnits(int num)
	{
		max_units += num;
	}

#pragma warning(push)
#pragma warning(disable : 4100)
	void ArmyGroup::OnUnitDamagedListener(const Unit* unit, float health_damage, float shields_damage)
	{
		
	}
#pragma warning(pop)

	void ArmyGroup::OnUnitDestroyedListener(const Unit* unit)
	{

		if (std::find(all_units.begin(), all_units.end(), unit) != all_units.end())
			RemoveUnit(unit);
		if (std::find(new_units.begin(), new_units.end(), unit) != new_units.end())
			RemoveUnit(unit);
	}

}