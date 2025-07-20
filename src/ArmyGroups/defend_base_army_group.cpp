
#include "defend_base_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {


DefendBaseArmyGroup::DefendBaseArmyGroup(Mediator* mediator, Point2D base_pos, std::vector<UNIT_TYPEID> unit_types, int desired_units, int max_units) : ArmyGroup(mediator)
{
	this->unit_types = unit_types;

	this->desired_units = desired_units;
	this->max_units = max_units;
	this->base_pos = base_pos;
}
	
void DefendBaseArmyGroup::SetUp()
{
	ready = true;
}

void DefendBaseArmyGroup::Run()
{
	// find mineral patches for probes to mineral walk to
	if (probes.size() > 0)
	{
		if (enemy_minerals == nullptr)
		{
			enemy_minerals = Utility::ClosestTo(mediator->GetUnits(IsUnits(MINERAL_PATCH)), mediator->GetEnemyStartLocation());
			if (enemy_minerals == nullptr)
			{
				std::cerr << "Error could not find minerals close to " << std::to_string(mediator->GetEnemyStartLocation().x) << ", " <<
					std::to_string(mediator->GetEnemyStartLocation().y) << " in ArmyGroup::DefendLocation" << std::endl;
				mediator->LogMinorError();
			}
		}
		if (base_minerals == nullptr)
		{
			base_minerals = Utility::ClosestTo(mediator->GetUnits(IsUnits(MINERAL_PATCH)), base_pos);
			if (base_minerals == nullptr)
			{
				std::cerr << "Error could not find minerals close to " << std::to_string(base_pos.x) << ", " << std::to_string(base_pos.y) <<
					" in ArmyGroup::DefendLocation" << std::endl;
				mediator->LogMinorError();
			}
		}
	}
	
	for (const auto& unit : all_units)
	{
		if (unit->unit_type == PROBE)
		{
			if (enemy_minerals == nullptr)
			{
				enemy_minerals = Utility::ClosestTo(mediator->GetUnits(IsUnits(MINERAL_PATCH)), mediator->GetEnemyStartLocation());
				if (enemy_minerals == nullptr)
				{
					std::cerr << "Error could not find minerals close to " << std::to_string(mediator->GetEnemyStartLocation().x) << ", " <<
						std::to_string(mediator->GetEnemyStartLocation().y) << " in ArmyGroup::DefendLocation" << std::endl;
					mediator->LogMinorError();
					continue;
				}
			}
			if (base_minerals == nullptr)
			{
				base_minerals = Utility::ClosestTo(mediator->GetUnits(IsUnits(MINERAL_PATCH)), base_pos);
				if (base_minerals == nullptr)
				{
					std::cerr << "Error could not find minerals close to " << std::to_string(base_pos.x) << ", " << std::to_string(base_pos.y) <<
						" in ArmyGroup::DefendLocation" << std::endl;
					mediator->LogMinorError();
					continue;
				}
			}
			if (unit->weapon_cooldown == 0)
			{
				Units units_in_range = Utility::GetUnitsInRange(mediator->GetUnits(Unit::Alliance::Enemy), unit, 0);
				if (units_in_range.size() > 0)
					mediator->SetUnitCommand(unit, A_ATTACK, units_in_range[0], CommandPriorty::normal);
				else if (Distance2D(unit->pos, base_pos) > 15)
					mediator->SetUnitCommand(unit, A_ATTACK, unit->pos, CommandPriorty::low);
				else
					mediator->SetUnitCommand(unit, A_SMART, enemy_minerals, CommandPriorty::low);
			}
			else
			{
				mediator->SetUnitCommand(unit, A_SMART, base_minerals, CommandPriorty::low);
			}
			continue;
		}
		
		if (mediator->GetAttackStatus(unit))
			continue;
		if (Distance2D(unit->pos, base_pos) > leash_range)
		{
			mediator->SetUnitCommand(unit, A_MOVE, base_pos, CommandPriorty::low);
		}
		else if (unit->weapon_cooldown == 0)
		{
			mediator->AddUnitToAttackers(unit);

			Units enemy_units = Utility::CloserThan(mediator->GetUnits(Unit::Alliance::Enemy), leash_range + Utility::GetGroundRange(unit), base_pos);
			if (enemy_units.size() == 0)
			{
				if (unit->orders.size() == 0)
					mediator->SetUnitCommand(unit, A_MOVE, base_pos, CommandPriorty::low);
				continue;
			}
			Point2D base_pos_L = base_pos;
			std::sort(enemy_units.begin(), enemy_units.end(),
				[&unit, &base_pos_L](const Unit* a, const Unit* b) -> bool
			{
				return Distance2D(unit->pos, a->pos) + Distance2D(base_pos_L, a->pos) <
					Distance2D(unit->pos, b->pos) + Distance2D(base_pos_L, b->pos);
			});
			mediator->SetUnitCommand(unit, A_MOVE, enemy_units[0]->pos, CommandPriorty::low);
		}
		else
		{
			const Unit* closest = Utility::ClosestTo(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), unit->pos);
			if (closest == NULL)
				closest = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), unit->pos);
			if (closest == NULL || Distance2D(closest->pos, unit->pos) < Utility::GetGroundRange(unit) - 1)
				mediator->SetUnitCommand(unit, A_MOVE, base_pos, CommandPriorty::low);
			else
				mediator->SetUnitCommand(unit, A_MOVE, closest->pos, CommandPriorty::low);
		}
	}
}

void DefendBaseArmyGroup::AddNewUnit(const Unit* unit)
{
	AddUnit(unit);
}

void DefendBaseArmyGroup::AddUnit(const Unit* unit)
{
	if (unit->unit_type == PROBE)
	{
		if (std::find(probes.begin(), probes.end(), unit) == probes.end())
			probes.push_back(unit);
	}
	ArmyGroup::AddUnit(unit);
}

void DefendBaseArmyGroup::RemoveUnit(const Unit* unit)
{
	probes.erase(std::remove(probes.begin(), probes.end(), unit), probes.end());
	ArmyGroup::RemoveUnit(unit);
}

Point2D DefendBaseArmyGroup::GetBasePos() const
{
	return base_pos;
}

}