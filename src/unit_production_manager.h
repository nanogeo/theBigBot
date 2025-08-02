#pragma once

#include "definitions.h"


namespace sc2
{

class Mediator;


class UnitProductionManager
{
private:
	Mediator* mediator;

	Units warpgates, robos, stargates, gateways;

	UNIT_TYPEID warpgate_production = UNIT_TYPEID::INVALID;
	UNIT_TYPEID robo_production = UNIT_TYPEID::INVALID;
	UNIT_TYPEID stargate_production = UNIT_TYPEID::INVALID;
	std::map<UNIT_TYPEID, int> target_unit_comp;
	bool automatic_unit_production = false;
	int max_gateways_built_simultaneously = 3; // these may need tweaking
	int max_robos_built_simultaneously = 1;
	int max_stargates_built_simultaneously = 1;
	int total_target_gateway_units = 0;
	int total_target_robo_units = 0;
	int total_target_stargate_units = 0;

	std::map<const Unit*, WarpgateStatus> warpgate_status;
	bool warp_in_at_proxy = false;

	void RunAutomaticUnitProduction();
	void RunSpecificUnitProduction();

	void GetNeededUnits(std::map<UNIT_TYPEID, int>&, std::map<UNIT_TYPEID, int>&, std::map<UNIT_TYPEID, int>&);

public:
	UnitProductionManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}
	void DisplayBuildingStatuses();

	void SetAutomaticUnitProduction(bool);
	void SetWarpgateProduction(UNIT_TYPEID);
	void SetRoboProduction(UNIT_TYPEID);
	void SetStargateProduction(UNIT_TYPEID);
	UNIT_TYPEID GetWarpgateProduction() const;
	UNIT_TYPEID GetRoboProduction() const;
	UNIT_TYPEID GetStargateProduction() const;

	void OnBuildingConstructionComplete(const Unit*); // TODO on unit destroyed
	void RunUnitProduction();
	void IncreaseProduction(UnitCost);

	void SetWarpInAtProxy(bool);
	std::vector<Point2D> FindWarpInSpots(Point2D) const;
	std::vector<Point2D> FindWarpInSpotsAt(Point2D) const;
	void UpdateWarpgateStatus();
	int NumWarpgatesReady() const;
	UnitCost CalculateCostOfProduction(int) const;

	void WarpInUnit(UNIT_TYPEID, Point2D);
	bool WarpInUnits(UNIT_TYPEID, int, Point2D);
	bool WarpInUnitsAt(UNIT_TYPEID, int, Point2D);

	std::map<UNIT_TYPEID, int> GetTargetUnitComp() const;
	void IncreaseUnitAmountInTargetComposition(UNIT_TYPEID, int);
	void DecreaseUnitAmountInTargetComposition(UNIT_TYPEID, int);
};


}