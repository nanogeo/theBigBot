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
	std::map<UNIT_TYPEID, uint16_t> target_unit_comp;
	bool automatic_unit_production = false;

	std::map<const Unit*, WarpgateStatus> warpgate_status;
	bool warp_in_at_proxy = false;

	UnitProductionManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}


	void ChooseUnitProduction();
	void SetWarpgateProduction(UNIT_TYPEID);
	void SetRoboProduction(UNIT_TYPEID);
	void SetStargateProduction(UNIT_TYPEID);

	void OnBuildingConstructionComplete(const Unit*); // TODO on unit destroyed
	void RunUnitProduction();
	void RunAutomaticUnitProduction();
	void RunSpecificUnitProduction();

	void SetWarpInAtProxy(bool);
	std::vector<Point2D> FindWarpInSpots(Point2D);
	std::vector<Point2D> FindWarpInSpotsAt(Point2D);
	void UpdateWarpgateStatus();
	int NumWarpgatesReady();
	UnitCost CalculateCostOfProduction();

	void WarpInUnit(UNIT_TYPEID, Point2D);
	bool WarpInUnits(UNIT_TYPEID, int, Point2D);
	bool WarpInUnitsAt(UNIT_TYPEID, int, Point2D);

	std::map<UNIT_TYPEID, uint16_t> GetTargetUnitComp();
	void IncreaseUnitAmountInTargetComposition(UNIT_TYPEID, uint16_t);
	void DecreaseUnitAmountInTargetComposition(UNIT_TYPEID, uint16_t);
};


}