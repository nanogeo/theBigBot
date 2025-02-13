#pragma once

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_unit_filters.h"

#include "action_manager.h"
#include "build_order_manager.h"
#include "worker_manager.h"
#include "unit_command_manager.h"
#include "upgrade_manager.h"
#include "finite_state_machine_manager.h"
#include "army_manager.h"
#include "unit_production_manager.h"
#include "definitions.h"

namespace sc2
{

class Mediator
{
public:

	Mediator(TheBigBot* agent) : 
		build_order_manager(this), 
		action_manager(this), 
		worker_manager(this), 
		unit_command_manager(this, agent),
		upgrade_manager(this),
		finite_state_machine_manager(this),
		army_manager(this),
		unit_production_manager(this)
	{
		this->agent = agent;
	}
//private:
	TheBigBot* agent;
	BuildOrderManager build_order_manager;
	ActionManager action_manager;
	WorkerManager worker_manager;
	UnitCommandManager unit_command_manager;
	UpgradeManager upgrade_manager;
	FiniteStateMachineManager finite_state_machine_manager;
	ArmyManager army_manager;
	UnitProductionManager unit_production_manager;


public:
	void SetUpManagers(BuildOrder);
	void RunManagers();

	int GetGameLoop();
	double GetCurrentTime();
	int GetUniqueId();
	int GetNumWorkers();
	int GetSupplyUsed();
	int GetSupplyCap();
	ImageData GetPathingGrid();
	bool IsPathable(Point2D);
	bool HasBuildingCompleted(UNIT_TYPEID);
	bool HasBuildingStarted(UNIT_TYPEID);
	bool IsResearching();
	bool HasResources(int, int, int);
	int GetNumUnits(UNIT_TYPEID);
	bool CanAfford(UNIT_TYPEID, int);
	bool CanAffordAfter(UNIT_TYPEID, UnitCost);
	int MaxCanAfford(UNIT_TYPEID);
	bool CanAffordUpgrade(UPGRADE_ID);
	bool CanBuildBuilding(UNIT_TYPEID);
	bool IsUnitOccupied(const Unit*);
	int GetUpgradeLevel(UpgradeType);

	const Unit* GetBuilder(Point2D);
	void BuildBuilding(UNIT_TYPEID, Point2D, const Unit*);
	void BuildBuildingMulti(std::vector<UNIT_TYPEID>, Point2D, const Unit*);
	//void BuildBuilding(UNIT_TYPEID);
	//void BuildGas(const Unit*);
	Units GetAllWorkersOnGas();
	int NumFar3rdWorkers();
	bool SendScout();
	bool SendCannonRushProbe1();

	void SetBuildWorkers(bool);
	void SetImmediatlySaturateGasses(bool);
	void SetImmediatlySemiSaturateGasses(bool);

	void SetBuildOrder(BuildOrder);
	Point2D GetLocation(UNIT_TYPEID);
	Point2D GetProxyLocation(UNIT_TYPEID);
	std::vector<Point2D> GetProxyLocations(UNIT_TYPEID);
	Point2D GetNaturalDefensiveLocation(UNIT_TYPEID);
	Point2D GetFirstPylonLocation();

	void ContinueBuildingPylons();
	void ContinueMakingWorkers();
	void ContinueUpgrades();
	void ContinueChronos();
	void ContinueExpanding();
	bool TrainFromProxyRobo();

	ScoutInfoTerran GetScoutInfoTerran();
	ScoutInfoZerg GetScoutInfoZerg();

	StateMachine* GetStateMachineByName(std::string);
	void RemoveStateMachine(StateMachine*);
	void CreateFourGateBlinkFSM();
	void CreateAdeptHarassProtossFSM();
	void AddOraclesToOracleHarassFSM();
	bool RemoveScoutToProxy(UNIT_TYPEID, int);
	void CreateAdeptBaseDefenseTerranFSM();

	void DefendThirdBaseZerg();

	void PlaceWorker(const Unit*);
	void RemoveWorker(const Unit*);

	void SetUnitProduction(UNIT_TYPEID);
	void SetWarpInAtProxy(bool);

	ArmyGroup* CreateArmyGroup(ArmyRole, std::vector<UNIT_TYPEID>, int, int);

	Point2D GetStartLocation();
	Point2D GetEnemyStartLocation();
	PathManager GetDirectAttackLine();
	std::vector<Point2D> GetDirectAttackPath();
	PathManager GetIndirectAttackLine();
	std::vector<Point2D> GetIndirectAttackPath();
	std::vector<Point2D> GetAltAttackPath();
	std::vector<Point2D> GetBadWarpInSpots();

	std::string GetMapName();
	Race GetEnemyRace();
	Point3D ToPoint3D(Point2D);

	Units GetUnits(Unit::Alliance, Filter);
	Units GetUnits(Unit::Alliance);
	Units GetUnits(Filter);
	const Unit* GetUnit(Tag);
	AvailableAbilities GetAbilitiesForUnit(const Unit*);

	void SetUnitCommand(const Unit* unit, AbilityID ability, bool queued_command = false);
	void SetUnitCommand(const Unit* unit, AbilityID ability, const Point2D& point, bool queued_command = false);
	void SetUnitCommand(const Unit* unit, AbilityID ability, const Unit* target, bool queued_command = false);
	void SetUnitsCommand(const Units& units, AbilityID ability, bool queued_command = false);
	void SetUnitsCommand(const Units& units, AbilityID ability, const Point2D& point, bool queued_command = false);
	void SetUnitsCommand(const Units& units, AbilityID ability, const Unit* target, bool queued_command = false);

	void OnBuildingConstructionComplete(const Unit*);
	void OnNeutralUnitCreated(const Unit*);
	void OnUnitCreated(const Unit*);
	void OnUnitEnterVision(const Unit*);
	void OnUnitDamaged(const Unit*, float, float);
	void OnUnitDestroyed(const Unit*); // remove from army units
	void OnUpgradeCompleted(UPGRADE_ID);
};


}