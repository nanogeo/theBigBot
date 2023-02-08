#pragma once
#include "pathfinding.h"
#include "nav_mesh_pathfinding.h"

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

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
    //TODO just store units
    std::vector<Tag> observer_ids;
    std::vector<Tag> immortal_ids;
    std::vector<Tag> prism_ids;
    std::vector<Tag> stalker_ids;
    std::vector<Tag> oracle_ids;
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
    Army(std::vector<Tag> obs, std::vector<Tag> immortals, std::vector<Tag> prisms, std::vector<Tag> stalkers,
        std::vector<Tag> oracles, std::vector<Point2D> path, int index)
    {
        observer_ids = obs;
        immortal_ids = immortals;
        prism_ids = prisms;
        stalker_ids = stalkers;
        oracle_ids = oracles;
        attack_path = path;
        current_attack_index = 3;
        high_ground_index = index;
    }
};

struct mineral_patch_data
{
    bool is_close;
    const Unit* workers[3];
    mineral_patch_data()
    {
        is_close = false;
        workers[0] = NULL;
        workers[1] = NULL;
        workers[2] = NULL;
    }
    mineral_patch_data(bool a)
    {
        is_close = a;
        workers[0] = NULL;
        workers[1] = NULL;
        workers[2] = NULL;
    }
    mineral_patch_data(bool a, Unit* x, Unit* y, Unit* z)
    {
        is_close = a;
        workers[0] = x;
        workers[1] = y;
        workers[2] = z;
    }
};

struct assimilator_data
{
    const Unit* workers[3];
};

struct mineral_patch_reversed_data
{
    const Unit* mineral_tag;
    Point2D drop_off_point;
    Point2D pick_up_point;
};

struct assimilator_reversed_data
{
    const Unit* assimilator_tag;
    Point2D drop_off_point;
    Point2D pick_up_point;
};

struct mineral_patch_space
{
    const Unit** worker;
    const Unit* mineral_patch;
    mineral_patch_space(const Unit** unit, const Unit* patch)
    {
        worker = unit;
        mineral_patch = patch;
    }
};

struct BuildOrderConditionArgData
{
    float time;
    UNIT_TYPEID unitId;
    int amount;
    BuildOrderConditionArgData() {};
    BuildOrderConditionArgData(float x)
    {
        time = x;
    }
    BuildOrderConditionArgData(UNIT_TYPEID x)
    {
        unitId = x;
    }
    BuildOrderConditionArgData(int x)
    {
        amount = x;
    }
};

struct BuildOrderResultArgData
{
    UPGRADE_ID upgradeId;
    UNIT_TYPEID unitId;
    std::vector<UNIT_TYPEID> unitIds;
    int amount;
    BuildOrderResultArgData() {};
    BuildOrderResultArgData(UPGRADE_ID x)
    {
        upgradeId = x;
    }
    BuildOrderResultArgData(UNIT_TYPEID x)
    {
        unitId = x;
    }
    BuildOrderResultArgData(std::vector<UNIT_TYPEID> x)
    {
        unitIds = x;
    }
    BuildOrderResultArgData(int x)
    {
        amount = x;
    }
    BuildOrderResultArgData(UNIT_TYPEID x, int y)
    {
        unitId = x;
        amount = y;
    }
};

struct BuildOrderData;

enum BuildOrder {
    blank,
    blink_proxy_robo_pressure,
    oracle_gatewayman_pvz,
    chargelot_allin,
    chargelot_allin_old,
    four_gate_adept_pressure,
    fastest_dts,
	proxy_double_robo
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


struct UnitCost
{
    int mineral_cost;
    int vespene_cost;
    int supply;
    UnitCost() {};
    UnitCost(int x, int y, int z)
    {
        mineral_cost = x;
        vespene_cost = y;
        supply = z;
    }
};

struct IsFinishedUnit {
    explicit IsFinishedUnit(UNIT_TYPEID type_);

    bool operator()(const Unit& unit_) const;

private:
    UNIT_TYPEID m_type;
};

class TossBot : public sc2::Agent
{
public:
    TossBot() : Agent() {};
    bool debug_mode = false;
    std::map<Tag, const Unit*> tag_to_unit;
    std::map<const Unit*, mineral_patch_data> mineral_patches;
    std::map<const Unit*, mineral_patch_reversed_data> mineral_patches_reversed;
    std::map<const Unit*, assimilator_data> assimilators;
    std::map<const Unit*, assimilator_reversed_data> assimilators_reversed;
    std::vector<mineral_patch_space*> first_2_mineral_patch_spaces;
    std::vector<mineral_patch_space*> far_3_mineral_patch_spaces;
    std::vector<mineral_patch_space*> close_3_mineral_patch_spaces;
    std::vector<mineral_patch_space*> gas_spaces;
    std::vector<mineral_patch_space*> far_3_mineral_patch_extras;
    std::vector<mineral_patch_space*> close_3_mineral_patch_extras;
    ScoutInfoZerg scout_info_zerg;
    ScoutInfoTerran scout_info_terran;
    Race enemy_race;
    std::vector<std::vector<bool>> grid_map;
    NavMesh nav_mesh;
    const Unit* probe;
	std::vector<Triangle*> overlaps;


    const Unit *new_base = NULL;
    std::vector<BuildOrderData> build_order;
    int build_order_step = 0;
    std::vector<ActionData*> active_actions;
    std::vector<StateMachine*> active_FSMs;
    Locations* locations;
    bool should_build_workers;
    int extra_pylons = 0;
    int removed_gas_miners = 0;
    bool immediatelySaturateGasses = false;
    std::vector<Tag> proxy_pylons;
    std::vector<Army*> army_groups;
    OnUnitDamagedEvent on_unit_damaged_event;
    OnUnitDestroyedEvent on_unit_destroyed_event;

    void UpdateUnitTags();
    Units TagsToUnits(const std::vector<Tag>);
    const Unit* ClosestTo(Units, Point2D);
    Point2D ClosestTo(std::vector<Point2D>, Point2D);
    float DistanceToClosest(Units, Point2D);
    float DistanceToClosest(std::vector<Point2D>, Point2D);
    Units CloserThan(Units, float, Point2D);
    bool HasBuff(const Unit*, BUFF_ID);
    std::vector<Point2D> GetLocations(UNIT_TYPEID);
    Point2D GetLocation(UNIT_TYPEID);
	Point2D GetProxyLocation(UNIT_TYPEID);
    std::vector<Point2D> GetProxyLocations(UNIT_TYPEID);
    float BuildingSize(UNIT_TYPEID);
    bool CanBuildBuilding(UNIT_TYPEID);
    ABILITY_ID GetBuildAbility(UNIT_TYPEID);
	ABILITY_ID GetTrainAbility(UNIT_TYPEID);
    bool CanAfford(UNIT_TYPEID, int);
    bool CanAffordUpgrade(UPGRADE_ID);
    int BuildingsReady(UNIT_TYPEID);
    std::vector<Point2D> FindWarpInSpots(Point2D);
    std::vector<Point2D> FindProxyWarpInSpots();
    static const Unit* GetLeastFullPrism(Units);
    void OraclesCoverStalkers(Units, Units);

    void ProcessFSMs();

    // Events
    void AddListenerToOnUnitDamagedEvent(std::function<void(const Unit*, float, float)>);
    void CallOnUnitDamagedEvent(const Unit*, float, float);
    void AddListenerToOnUnitDestroyedEvent(std::function<void(const Unit*)>);
    void CallOnUnitDestroyedEvent(const Unit*);

    // To strings
    static std::string BuildOrderToString(std::vector<BuildOrderData>);
    static std::string UnitTypeIdToString(UNIT_TYPEID);
    static std::string AbilityIdToString(ABILITY_ID);
    static std::string OrdersToString(std::vector<UnitOrder>);

    // Overrides
    virtual void OnBuildingConstructionComplete(const Unit*);
    virtual void OnNeutralUnitCreated(const Unit *);
    virtual void OnUnitCreated(const Unit*);
    virtual void OnUnitDamaged(const Unit*, float, float);
    virtual void OnUnitDestroyed(const Unit*);
    virtual void OnStep();
    virtual void OnGameStart();

    // Worker management
    const Unit* GetWorker();
    const Unit* GetBuilder(Point2D);
    void PlaceWorker(const Unit*);
    void PlaceWorkerInGas(const Unit*, const Unit*, int);
    void NewPlaceWorkerInGas(const Unit*, const Unit*);
    void PlaceWorkerOnMinerals(const Unit*, const Unit*, int);
    void NewPlaceWorkerOnMinerals(const Unit*, const Unit*);
    void RemoveWorker(const Unit*);
    void SplitWorkers();
    void SaturateGas(const Unit*);
    void AddNewBase(const Unit*);
    void DistributeWorkers();
    void BalanceWorers();
    void BuildWorkers();

    // Utility
    Point2D Center(Units);
    Point2D MedianCenter(Units);
    Point2D PointBetween(Point2D, Point2D, float);
    int DangerLevel(const Unit *);
    int GetDamage(const Unit*);
    float RealGroundRange(const Unit *, const Unit *);
    bool IsOnHighGround(Point3D, Point3D);
    float GetTimeBuilt(const Unit*);
    AbilityID UnitToWarpInAbility(UNIT_TYPEID);


    // Pathing
	Polygon CreateNewBlocker(const Unit*);

    void ProcessActions();
    // Actions
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

    void CheckBuildOrder();
    // Build order condition functions
    bool TimePassed(BuildOrderConditionArgData);
    bool NumWorkers(BuildOrderConditionArgData);
    bool HasBuilding(BuildOrderConditionArgData);
    bool IsResearching(BuildOrderConditionArgData);
    bool HasGas(BuildOrderConditionArgData);

    // Build order results
    bool BuildBuilding(BuildOrderResultArgData);
    bool BuildFirstPylon(BuildOrderResultArgData);
    bool BuildBuildingMulti(BuildOrderResultArgData);
    bool Scout(BuildOrderResultArgData);
    bool CutWorkers(BuildOrderResultArgData);
    bool UncutWorkers(BuildOrderResultArgData);
    bool ImmediatelySaturateGasses(BuildOrderResultArgData);
    bool TrainStalker(BuildOrderResultArgData);
    bool TrainAdept(BuildOrderResultArgData);
    bool TrainOracle(BuildOrderResultArgData);
    bool TrainPrism(BuildOrderResultArgData);
    bool ChronoBuilding(BuildOrderResultArgData);
    bool ResearchWarpgate(BuildOrderResultArgData);
    bool BuildProxy(BuildOrderResultArgData);
	bool BuildProxyMulti(BuildOrderResultArgData);
    bool ResearchBlink(BuildOrderResultArgData);
    bool ResearchCharge(BuildOrderResultArgData);
    bool ResearchGlaives(BuildOrderResultArgData);
    bool ResearchDTBlink(BuildOrderResultArgData);
    bool ChronoTillFinished(BuildOrderResultArgData);
    bool WarpInAtProxy(BuildOrderResultArgData);
    bool ContinueBuildingPylons(BuildOrderResultArgData);
    bool ContinueMakingWorkers(BuildOrderResultArgData);
    bool TrainFromProxy(BuildOrderResultArgData);
    bool ContinueChronoProxyRobo(BuildOrderResultArgData);
    bool Contain(BuildOrderResultArgData);
    bool StalkerOraclePressure(BuildOrderResultArgData);
    bool MicroOracles(BuildOrderResultArgData);
    bool SpawnUnits(BuildOrderResultArgData);
    bool ResearchAttackOne(BuildOrderResultArgData);
    bool ContinueWarpingInStalkers(BuildOrderResultArgData);
    bool ContinueWarpingInZealots(BuildOrderResultArgData);
    bool WarpInUnits(BuildOrderResultArgData);
    bool PullOutOfGas(BuildOrderResultArgData);
    bool IncreaseExtraPylons(BuildOrderResultArgData);
    bool MicroChargelotAllin(BuildOrderResultArgData);
    bool RemoveScoutToProxy(BuildOrderResultArgData);
    bool SafeRallyPoint(BuildOrderResultArgData);
    bool DTHarass(BuildOrderResultArgData);
	bool UseProxyDoubleRobo(BuildOrderResultArgData);

    // Bulid orders
    void SetBuildOrder(BuildOrder);
    void SetBlank();
    void SetBlinkProxyRoboPressureBuild();
    void SetOracleGatewaymanPvZ();
    void SetChargelotAllin();
    void SetChargelotAllinOld();
    void Set4GateAdept();
    void SetFastestDTsPvT();
	void SetProxyDoubleRobo();

    // Debug info
    void DisplayDebugHud();
    void DisplayWorkerStatus();
    void DisplayBuildOrder();
    void DisplayActiveActions();
    void DisplayActiveStateMachines();
    void DisplayBuildingStatuses();
    void DisplayArmyGroups();
    void DisplaySupplyInfo();

    // Micro
    void ObserveAttackPath(Units, Point2D, Point2D);
    void StalkerAttackTowards(Units, Point2D, Point2D, bool);
    void StalkerAttackTowardsWithPrism(Units, Units, Point2D, Point2D, bool);
    void ImmortalAttackTowards(Units, Point2D, Point2D, bool);
    void ImmortalAttackTowardsWithPrism(Units, Units, Point2D, Point2D, bool);

};






struct BuildOrderData
{
    bool(sc2::TossBot::*condition)(BuildOrderConditionArgData);
    BuildOrderConditionArgData condition_arg;
    bool(sc2::TossBot::*result)(BuildOrderResultArgData);
    BuildOrderResultArgData result_arg;
    BuildOrderData(bool(sc2::TossBot::*x)(BuildOrderConditionArgData), BuildOrderConditionArgData y, bool(sc2::TossBot::*z)(BuildOrderResultArgData), BuildOrderResultArgData a)
    {
        condition = x;
        condition_arg = y;
        result = z;
        result_arg = a;
    }
    std::string toString()
    {
        // Condition
        std::string str = "When ";
        if (condition == &TossBot::TimePassed)
        {
            int mins = std::floor(condition_arg.time / 60);
            int seconds = (int)condition_arg.time % 60;
            str += std::to_string(mins);
            str += ':';
            if (seconds < 10)
                str += '0';
            str += std::to_string(seconds);
            str += " time have passed, ";
        }
        else if (condition == &TossBot::NumWorkers)
        {
            str += "worker count = ";
            str += std::to_string(condition_arg.amount);
            str += ", ";
        }
        else if (condition == &TossBot::HasBuilding)
        {
            str += "a ";
            switch (condition_arg.unitId)
            {
            case UNIT_TYPEID::PROTOSS_PYLON:
                str += "pylon";
                break;
            case UNIT_TYPEID::PROTOSS_NEXUS:
                str += "nexus";
                break;
            case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
                str += "assimilator";
                break;
            case UNIT_TYPEID::PROTOSS_GATEWAY:
                str += "gateway";
                break;
            case UNIT_TYPEID::PROTOSS_FORGE:
                str += "forge";
                break;
            case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
                str += "cyber core";
                break;
            case UNIT_TYPEID::PHOTONCANNONWEAPON:
                str += "cannon";
                break;
            case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
                str += "shield battery";
                break;
            case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
                str += "twitlight";
                break;
            case UNIT_TYPEID::PROTOSS_STARGATE:
                str += "stargate";
                break;
            case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
                str += "robo";
                break;
            case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
                str += "robo bay";
                break;
            case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
                str += "templar archive";
                break;
            case UNIT_TYPEID::PROTOSS_DARKSHRINE:
                str += "dark shrine";
                break;
            case UNIT_TYPEID::PROTOSS_FLEETBEACON:
                str += "fleet beacon";
                break;
            default:
                str += "unknow building";
                break;
            }
            str += " is built, ";
        }
        else if (condition == &TossBot::IsResearching)
        {
            switch (condition_arg.unitId)
            {
            case UNIT_TYPEID::PROTOSS_FORGE:
                str += "forge";
                break;
            case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
                str += "cyber core";
                break;
            case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
                str += "twitlight";
                break;
            case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
                str += "robo bay";
                break;
            case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
                str += "templar archive";
                break;
            case UNIT_TYPEID::PROTOSS_DARKSHRINE:
                str += "dark shrine";
                break;
            case UNIT_TYPEID::PROTOSS_FLEETBEACON:
                str += "fleet beacon";
                break;
            default:
                str += "unknow building";
                break;
            }
            str += " is researching, ";
        }
        else if (condition == &TossBot::HasGas)
        {
            str += "vespene >= ";
            str += std::to_string(condition_arg.amount);
            str += ", ";
        }
        else
        {
            str += "unknown condition, ";
        }

        // Result
        if (result == &TossBot::BuildBuilding)
        {
            str += "build a ";
            str += TossBot::UnitTypeIdToString(result_arg.unitId);
        }
        else if (result == &TossBot::BuildFirstPylon)
        {
            str += "build first pylon";
        }
        else if (result == &TossBot::BuildBuildingMulti)
        {
            str += "build a ";
            for (const auto &building : result_arg.unitIds)
            {
                str += TossBot::UnitTypeIdToString(building);
                str += ", ";
            }
            str.pop_back();
            str.pop_back();
        }
		else if (result == &TossBot::BuildProxyMulti)
		{
			str += "build a proxy ";
			for (const auto &building : result_arg.unitIds)
			{
				str += TossBot::UnitTypeIdToString(building);
				str += ", ";
			}
			str.pop_back();
			str.pop_back();
		}
        else if (result == &TossBot::Scout)
        {
            str += "send scout";
        }
        else if (result == &TossBot::CutWorkers)
        {
            str += "stop building probes";
        }
        else if (result == &TossBot::UncutWorkers)
        {
            str += "start building probes again";
        }
        else if (result == &TossBot::ImmediatelySaturateGasses)
        {
            str += "immediately saturate gasses";
        }
        else if (result == &TossBot::TrainStalker)
        {
            str += "build a stalker";
        }
        else if (result == &TossBot::TrainAdept)
        {
            str += "build an adept";
        }
        else if (result == &TossBot::TrainOracle)
        {
            str += "build an oracle";
        }
        else if (result == &TossBot::ChronoBuilding)
        {
            str += "chrono ";
            str += TossBot::UnitTypeIdToString(result_arg.unitId);
        }
        else if (result == &TossBot::ResearchWarpgate)
        {
            str += "research warpgate";
        }
        else if (result == &TossBot::BuildProxy)
        {
            str += "build a proxy ";
            str += TossBot::UnitTypeIdToString(result_arg.unitId);
        }
        else if (result == &TossBot::ResearchBlink)
        {
            str += "research blink";
        }
        else if (result == &TossBot::ResearchCharge)
        {
            str += "research charge";
        }
        else if (result == &TossBot::ResearchGlaives)
        {
            str += "research glaives";
        }
        else if (result == &TossBot::ResearchDTBlink)
        {
            str += "research dt blink";
        }
        else if (result == &TossBot::ChronoTillFinished)
        {
            str += "chrono ";
            str += TossBot::UnitTypeIdToString(result_arg.unitId);
            str += " till it's finished";
        }
        else if (result == &TossBot::WarpInAtProxy)
        {
            str += "warp in stalkers at proxy";
        }
        else if (result == &TossBot::ContinueBuildingPylons)
        {
            str += "macro pylons";
        }
        else if (result == &TossBot::ContinueMakingWorkers)
        {
            str += "macro workers";
        }
        else if (result == &TossBot::TrainFromProxy)
        {
            str += "train units from proxy ";
            str += TossBot::UnitTypeIdToString(result_arg.unitId);
        }
        else if (result == &TossBot::ContinueChronoProxyRobo)
        {
            str += "continue chronoing proxy robo";
        }
        else if (result == &TossBot::Contain)
        {
            str += "contain";
        }
        else if (result == &TossBot::StalkerOraclePressure)
        {
            str += "stalker oracle pressure";
        }
        else if (result == &TossBot::MicroOracles)
        {
        str += "micro oracles";
        }
        else if (result == &TossBot::SpawnUnits)
        {
            str += "cheat in units ";
            //add what units
        }
        else if (result == &TossBot::ResearchAttackOne)
        {
            str += "research +1 attack";
        }
        else if (result == &TossBot::ContinueWarpingInStalkers)
        {
            str += "continue warping in stalkers";
        }
        else if (result == &TossBot::ContinueWarpingInZealots)
        {
            str += "continue warping in zealots";
        }
        else if (result == &TossBot::WarpInUnits)
        {
            str += "warp in ";
            str += std::to_string(result_arg.amount);
            str += " ";
            str += TossBot::UnitTypeIdToString(result_arg.unitId);
            str += 's';
        }
        else if (result == &TossBot::PullOutOfGas)
        {
            str += "pull ";
            str += std::to_string(result_arg.amount);
            str += " out of gas";
        }
        else if (result == &TossBot::IncreaseExtraPylons)
        {
            str += "increase extra pylons by ";
            str += std::to_string(result_arg.amount);
        }
        else if (result == &TossBot::MicroChargelotAllin)
        {
            str += "micro chargelot allin";
        }
        else if (result == &TossBot::RemoveScoutToProxy)
        {
            str += "remove scout and send to proxy";
        }
        else if (result == &TossBot::SafeRallyPoint)
        {
            str += "change rally point to safe pos";
        }
        else if (result == &TossBot::DTHarass)
        {
            str += "start DT harass";
        }
		else if (result == &TossBot::UseProxyDoubleRobo)
		{
		str += "use proxy double robo";
		}

        return str;
    }
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
            str += TossBot::UnitTypeIdToString(action_arg->unitId);
        }
        else if (action == &TossBot::ActionBuildBuildingMulti)
        {
            str += "Build a ";
            for (int i = action_arg->index; i < action_arg->unitIds.size(); i++)
            {
                str += TossBot::UnitTypeIdToString(action_arg->unitIds[i]);
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
				str += TossBot::UnitTypeIdToString(action_arg->unitIds[i]);
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
            str += TossBot::UnitTypeIdToString(action_arg->unitId);
            str += " till finished";
        }
        else if (action == &TossBot::ActionConstantChrono)
        {
            str += "Constant chrono on ";
            str += TossBot::UnitTypeIdToString(action_arg->unitId);
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
					str += TossBot::UnitTypeIdToString(unit);
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
