#pragma once
#include "pathfinding.h"
#include "nav_mesh_pathfinding.h"
#include "worker_manager.h"
#include "army_group.h"
#include "utility.h"
#include "action_manager.h"
#include "build_order_manager.h"
#include "definitions.h"


#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_unit_filters.h"

namespace sc2
{


class State;
class StateMachine;
class Locations;
class TheBigBot;

struct OnUnitDamagedEvent
{
    std::map<int, std::function<void(const Unit*, float, float)>> listeners;
    OnUnitDamagedEvent() {};
};

struct OnUnitDestroyedEvent
{
	std::map<int, std::function<void(const Unit*)>> listeners;
    OnUnitDestroyedEvent() {};
};

struct OnUnitCreatedEvent
{
	std::map<int, std::function<void(const Unit*)>> listeners;
    OnUnitCreatedEvent() {};
};

struct OnUnitEntersVisionEvent
{
	std::map<int, std::function<void(const Unit*)>> listeners;
	OnUnitEntersVisionEvent() {};
};

struct ScoutInfoZerg
{
    float natural_timing = 0;
    float third_timing = 0;
    float gas_timing = 0;
    int gas_mined = 0;
    float pool_timing = 0;
    bool pool_active = 0;
    int drone_count = 0;
    float roach_warren_timing = 0;
};


enum FirstRaxProduction {
	idle,
	marine,
	reaper,
	reactor,
	techlab
};

struct ScoutInfoTerran
{
    float natural_timing = 0;
    float third_timing = 0;
    float gas_timing = 0;
	Point3D gas_pos;
	float second_gas_timing = 0;
    int gas_mined = 0;
    float barrackes_timing = 0;
    bool factory_timing = 0;
    int scv_count = 0;
    int num_barracks = 0;
	FirstRaxProduction first_rax_production = FirstRaxProduction::idle;
};

struct BaseInfo
{
    bool taken;
    Point2D position;
    int num_workers;
    bool has_anti_air;
    float last_time_scouted;
    BaseInfo()
    {
        taken = false;
        position = Point2D(0, 0);
        num_workers = 0;
        has_anti_air = false;
        last_time_scouted = 0;
    }
};

struct Army
{
    Units observers;
	Units immortals;
	Units prisms;
	Units stalkers;
	Units oracles;
	std::vector<Point2D> attack_path;
    int current_attack_index;
    int high_ground_index;
    Army() {};
    Army(std::vector<Point2D> path, int index)
    {
        attack_path = path;
        current_attack_index = 2;
        high_ground_index = index;
    }
    Army(Units obs, Units immortals, Units prisms, Units stalkers,
		Units oracles, std::vector<Point2D> path, int index)
    {
        observers = obs;
        immortals = immortals;
        prisms = prisms;
        stalkers = stalkers;
        oracles = oracles;
        attack_path = path;
        current_attack_index = 3;
        high_ground_index = index;
    }
	Army(Units all_units, std::vector<Point2D> path, int index)
	{
		for (const auto &unit : all_units)
		{
			if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_OBSERVER)
				observers.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_IMMORTAL)
				immortals.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_WARPPRISM)
				prisms.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_STALKER)
				stalkers.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ORACLE)
				oracles.push_back(unit);
		}
        attack_path = path;
        current_attack_index = 3;
        high_ground_index = index;
	}
};




struct IsFinishedUnit {
    explicit IsFinishedUnit(UNIT_TYPEID type_);

    bool operator()(const Unit& unit_) const;

private:
    UNIT_TYPEID m_type;
};

struct IsFightingUnit {
	explicit IsFightingUnit(Unit::Alliance alliance_);

	bool operator()(const Unit& unit_) const;

private:
	Unit::Alliance m_type;
};

struct IsNonbuilding {
	explicit IsNonbuilding(Unit::Alliance alliance_);

	bool operator()(const Unit& unit_) const;

private:
	Unit::Alliance m_type;
};

struct IsNonPlaceholderUnit {
	explicit IsNonPlaceholderUnit(UNIT_TYPEID type_);

	bool operator()(const Unit& unit_) const;

private:
	UNIT_TYPEID m_type;
};

struct IsFriendlyUnit {
	explicit IsFriendlyUnit(UNIT_TYPEID type_);

	bool operator()(const Unit& unit_) const;

private:
	UNIT_TYPEID m_type;
};

struct IsEnemyUnit {
	explicit IsEnemyUnit(UNIT_TYPEID type_);

	bool operator()(const Unit& unit_) const;

private:
	UNIT_TYPEID m_type;
};

struct EnemyUnitPosition
{
	Point2D pos;
	int frames;
	EnemyUnitPosition()
	{
		pos = Point2D(0, 0);
		frames = 0;
	}
	EnemyUnitPosition(Point2D pos)
	{
		this->pos = pos;
		frames = 0;
	}
};

struct EnemyAttack
{
	const Unit* unit;
	int impact_frame;
	EnemyAttack(const Unit* unit, int impact_frame)
	{
		this->unit = unit;
		this->impact_frame = impact_frame;
	}
};

struct UnitTypeInfo
{
	bool is_melee;
	bool is_army_unit;
	bool has_anti_ground_attack;
	bool has_anti_air_attack;
	UnitTypeInfo() {};
	UnitTypeInfo(bool melee, bool army, bool anti_ground, bool anti_air)
	{
		is_melee = melee;
		is_army_unit = army;
		has_anti_ground_attack = anti_ground;
		has_anti_air_attack = anti_air;
	}
};

struct WarpgateStatus
{
	bool used;
	int frame_ready;
	WarpgateStatus() {};
	WarpgateStatus(int curr_frame)
	{
		used = false;
		frame_ready = curr_frame + 1;
	}
};

class BlankBot : public sc2::Agent {
public:
	BlankBot() : Agent() {};
	std::map<const Unit*, bool> attacks;

	virtual void OnGameStart()
	{
		Debug()->DebugGiveAllUpgrades();
		Debug()->SendDebug();
	}

	virtual void OnStep()
	{
		for (const auto& unit : Observation()->GetUnits(IsUnit(MARAUDER)))
		{
			if (attacks.find(unit) == attacks.end())
			{
				attacks[unit] = false;
			}
			if (attacks[unit])
			{
				if (unit->weapon_cooldown == 0)
					attacks[unit] = false;
			}
			else
			{
				if (unit->weapon_cooldown > 0)
				{
					attacks[unit] = true;
					std::cout << "attack launched" << std::endl;
				}
			}
		}
		/*for (const auto &unit : Observation()->GetUnits(IsUnit(UNIT_TYPEID::ZERG_ROACH)))
		{
			if (Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_STALKER)).size() == 0)
				return;
			const Unit* stalker = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_STALKER))[0];
			Point2D vec = Point2D(stalker->pos.x - unit->pos.x, stalker->pos.y - unit->pos.y);
			float angle = atan2(vec.y, vec.x);
			if (angle < 0)
				angle += 2 * 3.1415926535;
			float facing = unit->facing;
			std::string text = "roach facing: " + std::to_string(facing);
			text += " angle to stalker: " + std::to_string(angle);
			text += " weapon cooldown: " + std::to_string(unit->weapon_cooldown) + "\n";
			Debug()->DebugTextOut(text, unit->pos, Color(0, 255, 255), 20);
			Debug()->SendDebug();
		}*/
	}
};

class TheBigBot : public sc2::Agent
{
public:
	TheBigBot() : Agent(), worker_manager(this), action_manager(this), build_order_manager(this) {};
	bool started = false;
	BuildOrder curr_build_order = BuildOrder::testing;
	WorkerManager worker_manager;
	ActionManager action_manager;
	BuildOrderManager build_order_manager;
    bool debug_mode = !true;
	int frames_passed = 0;
    ScoutInfoZerg scout_info_zerg;
    ScoutInfoTerran scout_info_terran;
    Race enemy_race;
    std::vector<std::vector<bool>> grid_map;
    NavMesh nav_mesh;
    const Unit* probe;
	std::vector<Triangle*> overlaps;
	std::map<const Unit*, EnemyUnitPosition> enemy_unit_saved_position;
	std::map<const Unit*, float> enemy_weapon_cooldown;
	std::map<const Unit*, std::vector<EnemyAttack>> enemy_attacks;
	std::map<const Unit*, WarpgateStatus> warpgate_status;
	std::unordered_map<UNIT_TYPEID, UnitTypeInfo> unit_type_info;
	std::vector<Point2D> corrosive_bile_positions;
	std::vector<int> corrosive_bile_times;
	int current_unique_id = 0;

	bool has_blink = true;
	bool has_charge = false;
	bool has_storm = false;
	bool has_dt_blink = false;
	int upgrade_ground_weapon = 0;
	int upgrade_ground_armor = 0;
	int upgrade_shields = 0;
	int upgrade_air_weapon = 0;
	int upgrade_air_armor = 0;

	// testing
	Point2D enemy_army_spawn = Point2D(34, 139);
	Point2D friendly_army_spawn = Point2D(59, 114);
	Point2D fallback_point = Point2D(142, 33);
	ArmyGroup test_army;
	bool tests_set_up = false;
	bool initial_set_up = false;

	void RunInitialSetUp();
	void RunTests();

	void SpawnArmies();
	void SetUpArmies();


    //const Unit *new_base = NULL;
    std::vector<StateMachine*> active_FSMs;
    Locations* locations;
    //bool should_build_workers;
    int extra_pylons = 0;
    //int removed_gas_miners = 0;
    bool immediatelySaturateGasses = false;
	bool immediatelySemiSaturateGasses = false;
    std::vector<Tag> proxy_pylons;
    std::vector<ArmyGroup*> army_groups;
    OnUnitDamagedEvent on_unit_damaged_event;
    OnUnitDestroyedEvent on_unit_destroyed_event;
	OnUnitCreatedEvent on_unit_created_event;
	OnUnitEntersVisionEvent on_unit_enters_vision_event;

    std::vector<Point2D> GetLocations(UNIT_TYPEID);
    Point2D GetLocation(UNIT_TYPEID);
	Point2D GetProxyLocation(UNIT_TYPEID);
    std::vector<Point2D> GetProxyLocations(UNIT_TYPEID);
    std::vector<Point2D> FindWarpInSpots(Point2D, int);
    std::vector<Point2D> FindProxyWarpInSpots();
    void OraclesCoverStalkers(Units, Units);

    void ProcessFSMs();
	void RemoveStateMachine(StateMachine*);

    // Events
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

    // To strings
    static std::string OrdersToString(std::vector<UnitOrder>);

    // Overrides
    virtual void OnBuildingConstructionComplete(const Unit*);
    virtual void OnNeutralUnitCreated(const Unit *);
    virtual void OnUnitCreated(const Unit*);
	virtual void OnUnitEnterVision(const Unit*);
    virtual void OnUnitDamaged(const Unit*, float, float);
    virtual void OnUnitDestroyed(const Unit*);
	virtual void OnUpgradeCompleted(UpgradeID);
    virtual void OnStep();
    virtual void OnGameStart();


    // Utility
	int IncomingDamage(const Unit*);
	void UpdateEnemyUnitPositions();
	void UpdateEnemyWeaponCooldowns();
	void RemoveCompletedAtacks();
	std::vector<Point2D> FindConcave(Point2D, Point2D, int, float, float);
	std::vector<Point2D> FindConcaveFromBack(Point2D, Point2D, int, float, float);
	void SetUpUnitTypeInfo();
	void PrintAttacks(std::map<const Unit*, const Unit*>);
	bool UnitIsOccupied(const Unit*);
	Point3D ToPoint3D(Point2D);
	int GetUniqueId();
	void UpdateWarpgateStatus();
	void UpdateEffectPositions();


    // Pathing
	Polygon CreateNewBlocker(const Unit*);


    // Debug info
    void DisplayDebugHud();
    void DisplayWorkerStatus();
    void DisplayBuildOrder();
    void DisplayActiveActions();
    void DisplayActiveStateMachines();
    void DisplayBuildingStatuses();
    void DisplayArmyGroups();
    void DisplaySupplyInfo();
	void DisplayEnemyAttacks();
	void DisplayAlliedAttackStatus();
	void PrintNonPathablePoints();
	void ShowLocations();

    // Micro
    void ObserveAttackPath(Units, Point2D, Point2D);
    void StalkerAttackTowards(Units, Point2D, Point2D, bool);
    void StalkerAttackTowardsWithPrism(Units, Units, Point2D, Point2D, bool);
    void ImmortalAttackTowards(Units, Point2D, Point2D, bool);
    void ImmortalAttackTowardsWithPrism(Units, Units, Point2D, Point2D, bool);

	// Other
	bool FireVolley(Units, std::vector<UNIT_TYPEID>);
	std::map<const Unit*, const Unit*> FindTargets(Units, std::vector<UNIT_TYPEID>, float);

};








}
