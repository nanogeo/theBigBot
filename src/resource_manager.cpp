
#include "mediator.h"
#include "resource_manager.h"


namespace sc2
{

void ResourceManager::AddCost(ActiveCost new_cost)
{
	for (const auto& cost : active_costs)
	{
		if (cost == new_cost)
			return;
	}
	active_costs.push_back(new_cost);
	current_resources -= new_cost.cost;
}

void ResourceManager::UpdateResources()
{
	current_resources = mediator->GetCurrentResources();

	for (size_t i = 0; i < active_costs.size(); i++)
	{
		ActiveCost current_cost = active_costs[i];
		if (current_cost.unit == nullptr)
		{
			// warp in or build action
			const Unit* closest = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Self, IsUnit(current_cost.unit_type)), current_cost.position);
			if (closest && Distance2D(closest->pos, current_cost.position) < .5 && closest->display_type != Unit::DisplayType::Placeholder && closest->build_progress < 1)
			{
				active_costs.erase(active_costs.begin() + i);
				i--;
			}
			else
			{
				current_resources -= current_cost.cost;
			}
		}
		else if (current_cost.upgrade != UPGRADE_ID::INVALID)
		{
			// upgrade action
			if (current_cost.unit == nullptr ||
				(current_cost.unit->orders.size() > 0 && current_cost.unit->orders[0].ability_id == Utility::GetUpgradeAbility(current_cost.upgrade) && current_cost.unit->orders[0].progress < .25))
			{
				active_costs.erase(active_costs.begin() + i);
				i--;
			}
			else
			{
				current_resources -= current_cost.cost;
			}
		}
		else
		{
			// train action
			if (current_cost.unit == nullptr ||
				(current_cost.unit->orders.size() > 0 && current_cost.unit->orders[0].ability_id == Utility::GetTrainAbility(current_cost.unit_type) && current_cost.unit->orders[0].progress < .25) ||
				(current_cost.unit_type == PROBE && current_cost.unit->orders.size() >= 2 && current_cost.unit->orders[1].ability_id == ABILITY_ID::TRAIN_PROBE)) // special case for constant probe production
			{
				active_costs.erase(active_costs.begin() + i);
				i--;
			}
			else
			{
				current_resources -= current_cost.cost;
			}
		}
	}
}

UnitCost ResourceManager::GetAvailableResources() const
{
	return current_resources;
}

bool ResourceManager::CanAfford(UNIT_TYPEID unit_type) const
{
	UnitCost cost = Utility::GetCost(unit_type);
	if (current_resources.mineral_cost >= cost.mineral_cost &&
		current_resources.vespene_cost >= cost.vespene_cost &&
		current_resources.supply >= cost.supply)
		return true;

	return false;
}

bool ResourceManager::CanAfford(UNIT_TYPEID unit_type, int amount) const
{
	UnitCost cost = Utility::GetCost(unit_type) * amount;
	if (current_resources.mineral_cost >= cost.mineral_cost &&
		current_resources.vespene_cost >= cost.vespene_cost &&
		current_resources.supply >= cost.supply)
		return true;

	return false;
}

bool ResourceManager::CanAfford(UPGRADE_ID upgrade) const
{
	UnitCost cost = Utility::GetCost(upgrade);
	if (current_resources.mineral_cost >= cost.mineral_cost &&
		current_resources.vespene_cost >= cost.vespene_cost &&
		current_resources.supply >= cost.supply)
		return true;

	return false;
}

int ResourceManager::MaxCanAfford(UNIT_TYPEID unit) const
{
	int i = 1;
	while (true)
	{
		if (CanAfford(unit, i) == false)
			return i - 1;
		i++;
	}
}

bool ResourceManager::SpendResources(UNIT_TYPEID unit_type, const Unit* unit)
{
	if (CanAfford(unit_type) == false)
		return false;
	ActiveCost new_cost = ActiveCost(unit, unit_type);
	AddCost(new_cost);
	return true;
}

bool ResourceManager::SpendResources(UNIT_TYPEID unit_type, Point2D position)
{
	if (CanAfford(unit_type) == false)
		return false;
	ActiveCost new_cost = ActiveCost(position, unit_type);
	AddCost(new_cost);
	return true;
}

bool ResourceManager::SpendResources(UPGRADE_ID upgrade, const Unit* unit)
{
	if (CanAfford(upgrade) == false)
		return false;
	ActiveCost new_cost = ActiveCost(unit, upgrade);
	AddCost(new_cost);
	return true;
}

}