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
#include "pathing_manager.h"

#include "definitions.h"

namespace sc2
{

class Mediator
{
	friend TheBigBot;
private:
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
	PathingManager pathing_manager;

	bool minor_error_logged = false;
	std::vector<Point2D> do_not_rebuild;

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
		resource_manager(this),
		pathing_manager(this)
	{
		this->agent = agent;
	}

	void SetUpManagers(bool);
	void RunManagers();

	uint32_t GetGameLoop();
	float GetCurrentTime() const;
	int GetUniqueId();
	int GetNumWorkers();
	int GetSupplyUsed();
	int GetArmySupply();
	int GetSupplyCap();
	const std::vector<Effect>& GetEffects();
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
	int GetUpgradeLevel(UpgradeType);
	const Unit* GetMostRecentBuilding(UNIT_TYPEID);
	float GetLineDangerLevel(PiecewisePath);
	const std::vector<Point2D>& GetCorrosiveBilePositions();
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
	void DebugText(std::string, Point2D, Color, int);

	const Unit* GetBuilder(Point2D);
	bool BuildBuilding(UNIT_TYPEID);
	void BuildBuilding(UNIT_TYPEID, Point2D, const Unit*);
	void BuildBuildingMulti(std::vector<UNIT_TYPEID>, Point2D, const Unit*);
	void BuildBuildingWhenAble(UNIT_TYPEID);
	//void BuildBuilding(UNIT_TYPEID);
	//void BuildGas(const Unit*);
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
	Point2D GetLocation(UNIT_TYPEID, int) const;
	Point2D GetProxyLocation(UNIT_TYPEID);
	std::vector<Point2D> GetProxyLocations(UNIT_TYPEID);
	Point2D GetNaturalDefensiveLocation(UNIT_TYPEID);
	Point2D GetFirstPylonLocation();
	Point2D FindLocation(UNIT_TYPEID, Point2D);
	Point2D FindBuildLocationNear(UNIT_TYPEID, Point2D);
	Point2D FindBuildLocationNearWithinNexusRange(UNIT_TYPEID, Point2D);
	Point2D GetWallOffLocation(UNIT_TYPEID);
	const Locations& GetLocations() const;

	void ContinueBuildingPylons();
	void ContinueMakingWorkers();
	void ContinueUpgrades();
	void ContinueChronos();
	void ContinueExpanding();
	bool TrainFromProxyRobo();
	int GetNumBuildActions(UNIT_TYPEID);
	void AddAction(bool(sc2::ActionManager::* action)(ActionArgData*), ActionArgData*);
	void AddUniqueAction(bool(sc2::ActionManager::* action)(ActionArgData*), ActionArgData*);
	bool HasActionOfType(bool(sc2::ActionManager::* action)(ActionArgData*));
	void CancelAllBuildActions();
	void CancelAllActionsOfType(bool(sc2::ActionManager::* action)(ActionArgData*));
	UnitCost CalculateCostOfCurrentBuildActions();

	float GetFirstGasTiming() const;
	float GetSecondGasTiming() const;
	float GetNaturalTiming() const;
	float GetThirdTiming() const;
	float GetFirstBarrackTiming() const;
	FirstRaxProduction GetFirstBarrackProduction() const;
	void SetFirstBarrackProduction(FirstRaxProduction);
	float GetFactoryTiming() const;
	float GetFirstGateTiming() const;
	float GetSecondGateTiming() const;
	float GetFirstPylonTiming() const;
	float GetSecondPylonTiming() const;
	UNIT_TYPEID GetTechChoice() const;
	float GetSpawningPoolTiming() const;
	float GetRoachWarrenTiming() const;
	int GetEnemyUnitCount(UNIT_TYPEID);
	void InitializeGameState();
	GameState GetGameState() const;
	int GetIncomingDamage(const Unit*);

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
	void CreateAttack(std::vector<UNIT_TYPEID>, int, int, int, int);
	void CreateSimpleAttack(std::vector<UNIT_TYPEID>, int, int);
	void StartCannonRushDefense();
	void ScoutBases();
	void DefendMainRamp(Point2D);
	void AddToDefense(int, int);
	void AddToDefense(Point2D, int);
	void BuildDefensiveBuilding(UNIT_TYPEID, Point2D);

	float JudgeFight(Units, Units, float, float, bool);
	std::vector<OngoingAttack> GetOngoingAttacks() const;
	float GetWorstOngoingAttackValue();
	void SetAllowProductionInterrupt(bool);

	void PlaceWorker(const Unit*);
	RemoveWorkerResult RemoveWorker(const Unit*);
	bool PullOutOfGas();
	void PullOutOfGas(int);
	UnitCost CalculateIncome();

	void SetUnitProduction(UNIT_TYPEID);
	UNIT_TYPEID GetWarpgateProduction();
	UNIT_TYPEID GetRoboProduction();
	UNIT_TYPEID GetStargateProduction();
	void CancelWarpgateUnitProduction();
	void CancelRoboUnitProduction();
	void CancelStargateUnitProduction();
	void SetAutomaticUnitProduction(bool);
	std::map<UNIT_TYPEID, int> GetTargetUnitComp();
	void IncreaseUnitAmountInTargetComposition(UNIT_TYPEID, int);
	void DecreaseUnitAmountInTargetComposition(UNIT_TYPEID, int);
	void SetWarpInAtProxy(bool);
	UnitCost CalculateCostOfProduction();
	int GetNumWarpgatesReady();
	void WarpInUnit(UNIT_TYPEID, Point2D);
	bool WarpInUnits(UNIT_TYPEID, int, Point2D);
	bool WarpInUnitsAt(UNIT_TYPEID, int, Point2D);
	std::vector<Point2D> FindWarpInSpots(Point2D);
	bool TestWarpInSpot(Point2D);

	void StartUpgradeManager();
	void AddRequiredUpgrade(UPGRADE_ID);

	void AddZergTransitions();

	ArmyGroup* GetArmyGroupDefendingBase(Point2D);
	ArmyGroup* GetDoorGuardArmyGroup();
	void ScourMap();

	Point2D GetStartLocation() const;
	Point2D GetNaturalLocation() const;
	Point2D GetEnemyStartLocation() const;
	Point2D GetEnemyNaturalLocation() const;
	PiecewisePath GetDirectAttackLine() const;
	std::vector<Point2D> GetDirectAttackPath() const;
	PiecewisePath GetStalkerAttackLine() const;
	std::vector<Point2D> GetStalkerAttackPath() const;
	PiecewisePath GetIndirectAttackLine() const;
	std::vector<Point2D> GetIndirectAttackPath() const;
	std::vector<Point2D> GetAltAttackPath() const;
	std::vector<Point2D> GetBadWarpInSpots() const;
	const Unit* GetWorkerRushDefenseAttackingMineralPatch();
	const Unit* GetWorkerRushDefenseGroupingMineralPatch();
	std::vector<Point2D> GetAllBases() const;
	std::vector<Point2D> GetPossibleEnemyThirdBaseLocations() const;
	std::vector<Point2D> GetSelfMainScoutPath() const;
	std::vector<Point2D> GetSelfNaturalScoutPath() const;

	std::string GetMapName() const;
	Race GetEnemyRace() const;
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
	void SetUnitCommand(const Unit* unit, AbilityID ability, CommandPriorty prio, bool queued_command = false);
	void SetUnitCommand(const Unit* unit, AbilityID ability, const Point2D& point, CommandPriorty prio, bool queued_command = false);
	void SetUnitCommand(const Unit* unit, AbilityID ability, const Unit* target, CommandPriorty prio, bool queued_command = false);
	void SetUnitsCommand(const Units& units, AbilityID ability, CommandPriorty prio, bool queued_command = false);
	void SetUnitsCommand(const Units& units, AbilityID ability, const Point2D& point, CommandPriorty prio, bool queued_command = false);
	void SetUnitsCommand(const Units& units, AbilityID ability, const Unit* target, CommandPriorty prio, bool queued_command = false);
	void ForceUnitCommand(const Unit* unit, AbilityID ability, bool queued_command = false);
	void ForceUnitCommand(const Unit* unit, AbilityID ability, const Point2D& point, bool queued_command = false);
	void ForceUnitCommand(const Unit* unit, AbilityID ability, const Unit* target, bool queued_command = false);

	void OnBuildingConstructionComplete(const Unit*);
	void OnNeutralUnitCreated(const Unit*);
	void OnUnitCreated(const Unit*);
	void OnUnitEnterVision(const Unit*);
	void OnUnitDamaged(const Unit*, float, float);
	void OnUnitDestroyed(const Unit*); // remove from army units
	void OnUpgradeCompleted(UPGRADE_ID);

	void DisplayDebugHUD();
	void DisplaySupplyInfo();

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