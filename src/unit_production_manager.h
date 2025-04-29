#pragma once

#include "definitions.h"


namespace sc2
{

class Mediator;


class UnitProductionManager
{
public:
	Mediator* mediator;

	Units warpgates, robos, stargates, gateways;

	UNIT_TYPEID warpgate_production = UNIT_TYPEID::INVALID;
	UNIT_TYPEID robo_production = UNIT_TYPEID::INVALID;
	UNIT_TYPEID stargate_production = UNIT_TYPEID::INVALID;

	std::map<const Unit*, WarpgateStatus> warpgate_status;
	bool warp_in_at_proxy = false;

	UnitCost resources_used_this_frame = UnitCost(0, 0, 0);

	UnitProductionManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}


	void SetWarpgateProduction(UNIT_TYPEID);
	void SetRoboProduction(UNIT_TYPEID);
	void SetStargateProduction(UNIT_TYPEID);

	void OnBuildingConstructionComplete(const Unit*); // TODO on unit destroyed
	void RunUnitProduction();

	void SetWarpInAtProxy(bool);
	std::vector<Point2D> FindWarpInSpots(Point2D);
	void UpdateWarpgateStatus();
	UnitCost CalculateCostOfProduction();
};


}