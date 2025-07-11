#pragma once
#include "sc2api/sc2_map_info.h"

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
#include "transition_manager.h"
#include "resource_manager.h"

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
		fire_control_manager(this),
		transition_manager(this),
		resource_manager(this)
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
	TransitionManager transition_manager;
	ResourceManager resource_manager;

	bool minor_error_logged = false;
	std::vector<Point2D> do_not_rebuild;

public:
	void SetUpManagers(bool);
	void RunManagers();

	int GetGameLoop();
	float GetCurrentTime();
	int GetUniqueId();
	int GetNumWorkers();
	int GetSupplyUsed();
	int GetArmySupply();
	int GetSupplyCap();
	ImageData GetPathingGrid();
	bool IsPathable(Point2D);
	bool IsBuildable(Point2D);
	bool OnSameLevel(Point2D, Point2D);
	bool HasBuildingCompleted(UNIT_TYPEID);
	bool HasBuildingUnderConstruction(UNIT_TYPEID);
	bool HasBuildingStarted(UNIT_TYPEID);
	bool IsResearching();
	bool HasResources(int, int, int);
	int GetNumUnits(UNIT_TYPEID);
	bool CanAfford(UNIT_TYPEID, int);
	int MaxCanAfford(UNIT_TYPEID);
	bool CanAffordUpgrade(UPGRADE_ID);
	bool SpendResources(UNIT_TYPEID, const Unit*);
	bool SpendResources(UNIT_TYPEID, Point2D);
	bool SpendResources(UPGRADE_ID, const Unit*);
	bool CheckUpgrade(UPGRADE_ID);
	bool CanBuildBuilding(UNIT_TYPEID);
	bool HasTechForBuilding(UNIT_TYPEID);
	bool HasTechForUnit(UNIT_TYPEID);
	bool CanTrainUnit(UNIT_TYPEID);
	bool IsUnitOccupied(const Unit*);
	int GetUpgradeLevel(UpgradeType);
	const Unit* GetMostRecentBuilding(UNIT_TYPEID);
	float GetLineDangerLevel(PathManager);
	bool IsVisible(Point2D);
	std::vector<std::vector<UNIT_TYPEID>> GetPrio();
	UnitCost GetCurrentResources();
	UnitCost GetAvailableResources();
	void CancelBuilding(const Unit*);
	void CancelUnit(UNIT_TYPEID);
	void RebuildBuilding(Point2D, UNIT_TYPEID);
	void AddBuildingToDoNotRebuild(Point2D);

	void SendChat(std::string, ChatChannel);
	void LogMinorError();
	void DebugSphere(Point3D, float, Color);

	const Unit* GetBuilder(Point2D);
	bool BuildBuilding(UNIT_TYPEID);
	void BuildBuilding(UNIT_TYPEID, Point2D, const Unit*);
	void BuildBuildingMulti(std::vector<UNIT_TYPEID>, Point2D, const Unit*);
	//void BuildBuilding(UNIT_TYPEID);
	//void BuildGas(const Unit*);
	Units GetAllWorkersOnGas();
	int NumFar3rdWorkers();
	bool SendScout();
	bool SendCannonRushProbe1();

	void SetBuildWorkers(bool);
	bool CheckBuildWorkers();
	void SetImmediatlySaturateGasses(bool);
	void SetImmediatlySemiSaturateGasses(bool);
	void SetBalanceIncome(bool);

	void SetBuildOrder(BuildOrder);
	bool GetBuildOrderStatus();
	void PauseBuildOrder();
	void UnPauseBuildOrder();
	void SetWorkerRushDefenseBuidOrder();

	Point2D GetLocation(UNIT_TYPEID);
	Point2D GetLocation(UNIT_TYPEID, int);
	Point2D GetProxyLocation(UNIT_TYPEID);
	std::vector<Point2D> GetProxyLocations(UNIT_TYPEID);
	Point2D GetNaturalDefensiveLocation(UNIT_TYPEID);
	Point2D GetFirstPylonLocation();
	Point2D FindLocation(UNIT_TYPEID, Point2D);
	Point2D FindBuildLocationNear(UNIT_TYPEID, Point2D);
	Point2D FindBuildLocationNearWithinNexusRange(UNIT_TYPEID, Point2D);
	Point2D GetWallOffLocation(UNIT_TYPEID);
	const Locations& GetLocations();

	void ContinueBuildingPylons();
	void ContinueMakingWorkers();
	void ContinueUpgrades();
	void ContinueChronos();
	void ContinueExpanding();
	bool TrainFromProxyRobo();
	int GetNumBuildActions(UNIT_TYPEID);
	void AddAction(bool(sc2::ActionManager::* action)(ActionArgData*), ActionArgData*);
	bool HasActionOfType(bool(sc2::ActionManager::* action)(ActionArgData*));
	void CancelAllBuildActions();
	void CancelAllActionsOfType(bool(sc2::ActionManager::* action)(ActionArgData*));
	UnitCost CalculateCostOfCurrentBuildActions();

	float GetFirstGasTiming();
	float GetSecondGasTiming();
	float GetNaturalTiming();
	float GetThirdTiming();
	float GetFirstBarrackTiming();
	float GetFactoryTiming();
	float GetFirstGateTiming();
	float GetSecondGateTiming();
	float GetFirstPylonTiming();
	float GetSecondPylonTiming();
	UNIT_TYPEID GetTechChoice();
	float GetSpawningPoolTiming();
	float GetRoachWarrenTiming();
	int GetEnemyUnitCount(UNIT_TYPEID);
	void InitializeGameState();
	GameState GetGameState();

	StateMachine* GetStateMachineByName(std::string);
	void AddStateMachine(StateMachine*);
	void RemoveStateMachine(StateMachine*);
	void CreateFourGateBlinkFSM();
	void CreateAdeptHarassProtossFSM();
	void StartOracleHarassStateMachine(OutsideControlArmyGroup*);
	void StartChargelotAllInStateMachine();
	bool RemoveScoutToProxy(UNIT_TYPEID, int);
	void CreateAdeptBaseDefenseTerranFSM();
	void CreateWorkerRushDefenseFSM();
	void CreatePvPRampWallOffFSM();

	void MarkStateMachineForDeletion(StateMachine*);
	void MarkArmyGroupForDeletion(ArmyGroup* army_group);

	void DefendThirdBaseZerg();
	void SetDoorGuard();
	void CreateAttack(std::vector<UNIT_TYPEID>, uint16_t, uint16_t, uint16_t, uint16_t);
	void CreateSimpleAttack(std::vector<UNIT_TYPEID>, uint16_t, uint16_t);
	void StartCannonRushDefense();
	void ScoutBases();
	void DefendMainRamp(Point2D);
	void AddToDefense(int, int);
	void AddToDefense(Point2D, int);
	void BuildDefensiveBuilding(UNIT_TYPEID, Point2D);

	float JudgeFight(Units, Units, float, float, bool);
	std::vector<OngoingAttack> GetOngoingAttacks();
	float GetWorstOngoingAttackValue();
	void SetAllowProductionInterrupt(bool);

	void PlaceWorker(const Unit*);
	RemoveWorkerResult RemoveWorker(const Unit*);
	void PullOutOfGas();
	void PullOutOfGas(int);
	UnitCost CalculateIncome();

	void SetUnitProduction(UNIT_TYPEID);
	UNIT_TYPEID GetWarpgateProduction();
	UNIT_TYPEID GetRoboProduction();
	UNIT_TYPEID GetStargateProduction();
	void CancelWarpgateUnitProduction();
	void CancelRoboUnitProduction();
	void CancelStargateUnitProduction();
	void SetWarpInAtProxy(bool);
	UnitCost CalculateCostOfProduction();
	int GetNumWarpgatesReady();
	void WarpInUnit(UNIT_TYPEID, Point2D);
	bool WarpInUnits(UNIT_TYPEID, int, Point2D);
	bool WarpInUnitsAt(UNIT_TYPEID, int, Point2D);
	std::vector<Point2D> FindWarpInSpots(Point2D);
	bool TestWarpInSpot(Point2D);

	ArmyGroup* GetArmyGroupDefendingBase(Point2D);
	ArmyGroup* GetDoorGuardArmyGroup();
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
	const Unit* GetWorkerRushDefenseAttackingMineralPatch();
	const Unit* GetWorkerRushDefenseGroupingMineralPatch();
	std::vector<Point2D> GetAllBases();
	std::vector<Point2D> GetPossibleEnemyThirdBaseLocations();
	std::vector<Point2D> GetSelfMainScoutPath();
	std::vector<Point2D> GetSelfNaturalScoutPath();

	std::string GetMapName();
	Race GetEnemyRace();
	std::string GetEnemyName();
	Point3D ToPoint3D(Point2D);

	Units GetUnits(Unit::Alliance, Filter);
	Units GetUnits(Unit::Alliance);
	Units GetUnits(Filter);
	const Unit* GetUnit(Tag);
	AvailableAbilities GetAbilitiesForUnit(const Unit*);
	Point2D GetUnitPosition(const Unit*);
	const std::map<const Unit*, EnemyUnitPosition> GetEnemySavedPositions();

	bool IsStalkerBlinkOffCooldown(const Unit*);
	void SetStalkerOrder(const Unit*);
	bool IsOracleBeamActive(const Unit*);
	void SetOracleOrder(const Unit*, ABILITY_ID);
	bool IsOracleCasting(const Unit*);
	bool IsEnergyRechargeOffCooldown();
	void SetEnergyRechargeCooldown();
	bool IsNexusRecallOffCooldown();
	void SetNexusRecallCooldown();

	void AddUnitToAttackers(const Unit*);
	void AddUnitsToAttackers(Units);
	void ConfirmAttack(const Unit*, const Unit*);
	void CancelAttack(const Unit*);
	bool GetAttackStatus(const Unit*);

	TryActionResult TryBuildBuilding(const Unit*, UNIT_TYPEID, Point2D);
	TryActionResult TryTrainUnit(const Unit*, UNIT_TYPEID);
	TryActionResult TryTrainProbe(const Unit*);
	TryActionResult TryWarpIn(const Unit*, UNIT_TYPEID, Point2D);
	TryActionResult TryResearchUpgrade(const Unit*, UPGRADE_ID);
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