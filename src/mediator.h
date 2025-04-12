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
#include "scouting_manager.h"
#include "defense_manager.h"
#include "ability_manager.h"
#include "fire_control_manager.h"

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
		unit_production_manager(this),
		scouting_manager(this),
		defense_manager(this),
		ability_manager(this),
		fire_control_manager(this)
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
	ScoutingManager scouting_manager;
	DefenseManager defense_manager;
	AbilityManager ability_manager;
	FireControlManager fire_control_manager;


public:
	void SetUpManagers(bool);
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
	bool CheckUpgrade(UPGRADE_ID);
	bool CanBuildBuilding(UNIT_TYPEID);
	bool IsUnitOccupied(const Unit*);
	int GetUpgradeLevel(UpgradeType);
	const Unit* GetMostRecentBuilding(UNIT_TYPEID);
	float GetLineDangerLevel(PathManager);
	bool IsVisible(Point2D);
	std::vector<std::vector<UNIT_TYPEID>> GetPrio();

	void SendChat(std::string, ChatChannel);

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
	void PauseBuildOrder();
	void UnPauseBuildOrder();
	Point2D GetLocation(UNIT_TYPEID);
	Point2D GetLocation(UNIT_TYPEID, int);
	Point2D GetProxyLocation(UNIT_TYPEID);
	std::vector<Point2D> GetProxyLocations(UNIT_TYPEID);
	Point2D GetNaturalDefensiveLocation(UNIT_TYPEID);
	Point2D GetFirstPylonLocation();
	Point2D FindLocation(UNIT_TYPEID, Point2D);

	void ContinueBuildingPylons();
	void ContinueMakingWorkers();
	void ContinueUpgrades();
	void ContinueChronos();
	void ContinueExpanding();
	bool TrainFromProxyRobo();
	int GetNumBuildActions(UNIT_TYPEID);
	void AddAction(bool(sc2::ActionManager::* action)(ActionArgData*), ActionArgData*);
	bool HasActionOfType(bool(sc2::ActionManager::* action)(ActionArgData*));

	ScoutInfoTerran GetScoutInfoTerran();
	ScoutInfoZerg GetScoutInfoZerg();

	StateMachine* GetStateMachineByName(std::string);
	void RemoveStateMachine(StateMachine*);
	void CreateFourGateBlinkFSM();
	void CreateAdeptHarassProtossFSM();
	void StartOracleHarassStateMachine(ArmyGroup*);
	bool RemoveScoutToProxy(UNIT_TYPEID, int);
	void CreateAdeptBaseDefenseTerranFSM();
	void MarkStateMachineForDeletion(StateMachine*);
	void MarkArmyGroupForDeletion(ArmyGroup* army_group);

	void DefendThirdBaseZerg();
	void AddToDefense(int, int);
	void AddToDefense(Point2D, int);
	void BuildDefensiveBuilding(UNIT_TYPEID, Point2D);
	
	std::vector<OngoingAttack> GetOngoingAttacks();
	float GetWorstOngoingAttackValue();

	void PlaceWorker(const Unit*);
	RemoveWorkerResult RemoveWorker(const Unit*);

	void SetUnitProduction(UNIT_TYPEID);
	UNIT_TYPEID GetWarpgateProduction();
	UNIT_TYPEID GetRoboProduction();
	UNIT_TYPEID GetStargateProduction();
	void CancelWarpgateUnitProduction();
	void CancelRoboUnitProduction();
	void CancelStargateUnitProduction();
	void SetWarpInAtProxy(bool);

	ArmyGroup* CreateArmyGroup(ArmyRole, std::vector<UNIT_TYPEID>, int, int);
	ArmyGroup* GetArmyGroupDefendingBase(Point2D);
	void ScourMap();

	Point2D GetStartLocation();
	Point2D GetNaturalLocation();
	Point2D GetEnemyStartLocation();
	Point2D GetEnemyNaturalLocation();
	PathManager GetDirectAttackLine();
	std::vector<Point2D> GetDirectAttackPath();
	PathManager GetStalkerAttackLine();
	std::vector<Point2D> GetStalkerAttackPath();
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
	Point2D GetUnitPosition(const Unit*);

	bool IsStalkerBlinkOffCooldown(const Unit*);
	void SetStalkerOrder(const Unit*);
	bool IsOracleBeamActive(const Unit*);
	void SetOracleOrder(const Unit*, ABILITY_ID);
	bool IsOracleCasting(const Unit*);
	bool IsBatteryOverchargeOffCooldown();
	void SetBatteryOverchargeCooldown();
	bool IsNexusRecallOffCooldown();
	void SetNexusRecallCooldown();

	void AddUnitToAttackers(const Unit*);
	void AddUnitsToAttackers(Units);
	void ConfirmAttack(const Unit*, const Unit*);
	void CancelAttack(const Unit*);
	bool GetAttackStatus(const Unit*);

	void SetUnitCommand(const Unit* unit, AbilityID ability, int prio, bool queued_command = false);
	void SetUnitCommand(const Unit* unit, AbilityID ability, const Point2D& point, int prio, bool queued_command = false);
	void SetUnitCommand(const Unit* unit, AbilityID ability, const Unit* target, int prio, bool queued_command = false);
	void SetUnitsCommand(const Units& units, AbilityID ability, int prio, bool queued_command = false);
	void SetUnitsCommand(const Units& units, AbilityID ability, const Point2D& point, int prio, bool queued_command = false);
	void SetUnitsCommand(const Units& units, AbilityID ability, const Unit* target, int prio, bool queued_command = false);

	void OnBuildingConstructionComplete(const Unit*);
	void OnNeutralUnitCreated(const Unit*);
	void OnUnitCreated(const Unit*);
	void OnUnitEnterVision(const Unit*);
	void OnUnitDamaged(const Unit*, float, float);
	void OnUnitDestroyed(const Unit*); // remove from army units
	void OnUpgradeCompleted(UPGRADE_ID);


	void AddListenerToOnUnitDamagedEvent(int, std::function<void(const Unit*, float, float)>);
	void RemoveListenerToOnUnitDamagedEvent(int);
	void CallOnUnitDamagedEvent(const Unit*, float, float);

	void AddListenerToOnUnitDestroyedEvent(int, std::function<void(const Unit*)>);
	void RemoveListenerToOnUnitDestroyedEvent(int);
	void CallOnUnitDestroyedEvent(const Unit*);

	void AddListenerToOnUnitCreatedEvent(int, std::function<void(const Unit*)>);
	void RemoveListenerToOnUnitCreatedEvent(int);
	void CallOnUnitCreatedEvent(const Unit*);

	void AddListenerToOnUnitEntersVisionEvent(int, std::function<void(const Unit*)>);
	void RemoveListenerToOnUnitEntersVisionEvent(int);
	void CallOnUnitEntersVisionEvent(const Unit*);
};


}