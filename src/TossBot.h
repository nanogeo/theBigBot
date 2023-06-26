#pragma once
#include "pathfinding.h"
#include "nav_mesh_pathfinding.h"
#include "worker_manager.h"
#include "army_group.h"
#include "utility.h"
#include "build_order_manager.h"


#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_unit_filters.h"

namespace sc2
{


class State;
class StateMachine;
class Locations;
class TossBot;

struct OnUnitDamagedEvent
{
    std::vector<std::function<void(const Unit*, float, float)>> listeners;
    OnUnitDamagedEvent() {};
};

struct OnUnitDestroyedEvent
{
    std::vector<std::function<void(const Unit*)>> listeners;
    OnUnitDestroyedEvent() {};
};

struct OnUnitCreatedEvent
{
    std::vector<std::function<void(const Unit*)>*> listeners;
    OnUnitCreatedEvent() {};
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

struct ScoutInfoTerran
{
    float natural_timing = 0;
    float third_timing = 0;
    float gas_timing = 0;
    int gas_mined = 0;
    float barrackes_timing = 0;
    bool factory_timing = 0;
    int scv_count = 0;
    int num_barracks = 0;
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



struct ActionArgData
{
    int index;
    const Unit* unit = NULL;
    UNIT_TYPEID unitId;
    std::vector<UNIT_TYPEID> unitIds;
    UPGRADE_ID upgradeId;
    Point2D position;
    Army* army;
	ArmyGroup* army_group;
    ActionArgData() {}
    ActionArgData(const Unit* x, UNIT_TYPEID y, Point2D z)
    {
        unit = x;
        unitId = y;
        position = z;
    }
    ActionArgData(const Unit* x)
    {
        unit = x;
    }
    ActionArgData(const Unit* x, UNIT_TYPEID y)
    {
        unit = x;
        unitId = y;
    }
    ActionArgData(Army* x)
    {
        army = x;
    }
	ActionArgData(ArmyGroup* x)
	{
		army_group = x;
	}
    ActionArgData(const Unit* x, std::vector<UNIT_TYPEID> y, Point2D z, int i)
    {
        unit = x;
        unitIds = y;
        position = z;
        index = i;
    }
    ActionArgData(const Unit* x, UNIT_TYPEID y, Point2D z, int time)
    {
        unit = x;
        unitId = y;
        position = z;
        index = time;
    }
	ActionArgData(std::vector<UNIT_TYPEID> x)
	{
		unitIds = x;
	}
};

struct ActionData;


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

class BlankBot : public sc2::Agent {
public:
	BlankBot() : Agent() {};

	virtual void OnGameStart()
	{
		//Debug()->DebugGiveAllUpgrades();
		Debug()->SendDebug();
	}

	virtual void OnStep()
	{
		for (const auto &unit : Observation()->GetUnits(IsUnit(UNIT_TYPEID::ZERG_ROACH)))
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
		}
	}
};

class TossBot : public sc2::Agent
{
public:
	TossBot() : Agent(), worker_manager(this), build_order_manager(this) {};
	WorkerManager worker_manager;
	BuildOrderManager build_order_manager;
    bool debug_mode = false;
	int frames_passed = 0;
    std::map<Tag, const Unit*> tag_to_unit;
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
	std::unordered_map<UNIT_TYPEID, UnitTypeInfo> unit_type_info;

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
    std::vector<ActionData*> active_actions;
    std::vector<StateMachine*> active_FSMs;
    Locations* locations;
    //bool should_build_workers;
    int extra_pylons = 0;
    //int removed_gas_miners = 0;
    bool immediatelySaturateGasses = false;
    std::vector<Tag> proxy_pylons;
    std::vector<Army*> army_groups;
    OnUnitDamagedEvent on_unit_damaged_event;
    OnUnitDestroyedEvent on_unit_destroyed_event;

    void UpdateUnitTags();
    Units TagsToUnits(const std::vector<Tag>);
    std::vector<Point2D> GetLocations(UNIT_TYPEID);
    Point2D GetLocation(UNIT_TYPEID);
	Point2D GetProxyLocation(UNIT_TYPEID);
    std::vector<Point2D> GetProxyLocations(UNIT_TYPEID);
    std::vector<Point2D> FindWarpInSpots(Point2D);
    std::vector<Point2D> FindProxyWarpInSpots();
    void OraclesCoverStalkers(Units, Units);

    void ProcessFSMs();

    // Events
    void AddListenerToOnUnitDamagedEvent(std::function<void(const Unit*, float, float)>);
    void CallOnUnitDamagedEvent(const Unit*, float, float);
    void AddListenerToOnUnitDestroyedEvent(std::function<void(const Unit*)>);
    void CallOnUnitDestroyedEvent(const Unit*);

    // To strings
    static std::string OrdersToString(std::vector<UnitOrder>);

    // Overrides
    virtual void OnBuildingConstructionComplete(const Unit*);
    virtual void OnNeutralUnitCreated(const Unit *);
    virtual void OnUnitCreated(const Unit*);
    virtual void OnUnitDamaged(const Unit*, float, float);
    virtual void OnUnitDestroyed(const Unit*);
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


    // Pathing
	Polygon CreateNewBlocker(const Unit*);

    // Actions
	void ProcessActions();

    bool ActionBuildBuilding(ActionArgData*);
    bool ActionBuildBuildingMulti(ActionArgData*);
	bool ActionBuildProxyMulti(ActionArgData*);
    bool ActionScoutZerg(ActionArgData*);
    bool ActionContinueMakingWorkers(ActionArgData*);
    bool ActionContinueBuildingPylons(ActionArgData*);
    bool ActionChronoTillFinished(ActionArgData*);
    bool ActionConstantChrono(ActionArgData*);
    bool ActionWarpInAtProxy(ActionArgData*);
    bool ActionTrainFromProxyRobo(ActionArgData*);
    bool ActionContain(ActionArgData*);
    bool ActionStalkerOraclePressure(ActionArgData*);
    bool ActionContinueWarpingInStalkers(ActionArgData*);
    bool ActionContinueWarpingInZealots(ActionArgData*);
    bool ActionPullOutOfGas(ActionArgData*);
    bool ActionRemoveScoutToProxy(ActionArgData*);
    bool ActionDTHarassTerran(ActionArgData*);
	bool ActionUseProxyDoubleRobo(ActionArgData*);
	bool ActionAllIn(ActionArgData*);


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




struct ActionData
{
    bool(sc2::TossBot::*action)(ActionArgData*);
    ActionArgData* action_arg;
    ActionData(bool(sc2::TossBot::*x)(ActionArgData*), ActionArgData* y)
    {
        action = x;
        action_arg = y;
    }
    std::string toString()
    {
        std::string str;
        if (action == &TossBot::ActionBuildBuilding)
        {
            str += "Build a ";
            str += Utility::UnitTypeIdToString(action_arg->unitId);
        }
        else if (action == &TossBot::ActionBuildBuildingMulti)
        {
            str += "Build a ";
            for (int i = action_arg->index; i < action_arg->unitIds.size(); i++)
            {
                str += Utility::UnitTypeIdToString(action_arg->unitIds[i]);
                str += ", ";
            }
            str.pop_back();
            str.pop_back();
        }
		else if (action == &TossBot::ActionBuildProxyMulti)
		{
			str += "Build a proxy ";
			for (int i = action_arg->index; i < action_arg->unitIds.size(); i++)
			{
				str += Utility::UnitTypeIdToString(action_arg->unitIds[i]);
				str += ", ";
			}
			str.pop_back();
			str.pop_back();
		}
        else if (action == &TossBot::ActionScoutZerg)
        {
            str += "Scout zerg UNUSED";
        }
        else if (action == &TossBot::ActionContinueMakingWorkers)
        {
            str += "Continue making workers";
        }
        else if (action == &TossBot::ActionContinueBuildingPylons)
        {
            str += "Continue building pylons";
        }
        else if (action == &TossBot::ActionChronoTillFinished)
        {
            str += "Chrono ";
            str += Utility::UnitTypeIdToString(action_arg->unitId);
            str += " till finished";
        }
        else if (action == &TossBot::ActionConstantChrono)
        {
            str += "Constant chrono on ";
            str += Utility::UnitTypeIdToString(action_arg->unitId);
        }
        else if (action == &TossBot::ActionWarpInAtProxy)
        {
            str += "Warp in at proxy";
        }
        else if (action == &TossBot::ActionTrainFromProxyRobo)
        {
            str += "Train units from proxy robo";
        }
        else if (action == &TossBot::ActionContain)
        {
            str += "Contain";
        }
        else if (action == &TossBot::ActionStalkerOraclePressure)
        {
            str += "Stalker Oracle pressure";
        }
        else if (action == &TossBot::ActionContinueWarpingInStalkers)
        {
            str += "Continue warping in stalkers";
        }
        else if (action == &TossBot::ActionContinueWarpingInZealots)
        {
            str += "Continue warping in zealots";
        }
        else if (action == &TossBot::ActionPullOutOfGas)
        {
            str += "pull out of gas";
        }
		else if (action == &TossBot::ActionUseProxyDoubleRobo)
		{
			str += "Build ";
			if (action_arg->unitIds.size() > 0)
			{
				for (const auto &unit : action_arg->unitIds)
				{
					str += Utility::UnitTypeIdToString(unit);
					str += ", ";
				}
			}
			else
			{
				str += "immortals ";
			}
			str += "from proxy robos";
		}
        return str;
    }
};





}
