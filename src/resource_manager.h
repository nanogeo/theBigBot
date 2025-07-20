#pragma once

#include "utility.h"



namespace sc2
{

class Mediator;

struct ActiveCost
{
	Point2D position = Point2D(0, 0);
	const Unit* unit = nullptr;
	UNIT_TYPEID unit_type = UNIT_TYPEID::INVALID;
	UPGRADE_ID upgrade = UPGRADE_ID::INVALID;
	UnitCost cost;
	ActiveCost(Point2D position, UNIT_TYPEID unit_type)
	{
		this->position = position;
		this->unit_type = unit_type;
		this->cost = Utility::GetCost(unit_type);
	}
	ActiveCost(const Unit* unit, UNIT_TYPEID unit_type)
	{
		this->unit = unit;
		this->unit_type = unit_type;
		this->cost = Utility::GetCost(unit_type);
	}
	ActiveCost(const Unit* unit, UPGRADE_ID upgrade)
	{
		this->unit = unit;
		this->upgrade = upgrade;
		this->cost = Utility::GetCost(upgrade);
	}
};

class ResourceManager
{
private:
	Mediator* mediator;
	// oracles
	std::vector<ActiveCost> active_costs;
	UnitCost current_resources = UnitCost(0, 0, 0);

public:
	ResourceManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void UpdateResources();
	UnitCost GetAvailableResources() const;
	bool CanAfford(UNIT_TYPEID) const;
	bool CanAfford(UNIT_TYPEID, int) const;
	bool CanAfford(UPGRADE_ID) const;
	int MaxCanAfford(UNIT_TYPEID) const;
	bool SpendResources(UNIT_TYPEID, const Unit*);
	bool SpendResources(UNIT_TYPEID, Point2D);
	bool SpendResources(UPGRADE_ID, const Unit*);
};


}