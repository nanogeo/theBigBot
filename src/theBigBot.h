#pragma once
#include "sc2api/sc2_common.h"
#include "sc2api/sc2_agent.h"

#include "pathfinding.h"
#include "nav_mesh_pathfinding.h"
#include "army_group.h"
#include "utility.h"
#include "definitions.h"
#include "mediator.h"



#include <map>

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


struct UnitTypeInfo
{
	bool is_melee = 0;
	bool is_army_unit = 0;
	bool has_anti_ground_attack = 0;
	bool has_anti_air_attack = 0;
	UnitTypeInfo() {};
	UnitTypeInfo(bool melee, bool army, bool anti_ground, bool anti_air)
	{
		is_melee = melee;
		is_army_unit = army;
		has_anti_ground_attack = anti_ground;
		has_anti_air_attack = anti_air;
	}
};


class TheBigBot : public sc2::Agent
{
public:
	TheBigBot() : Agent(), mediator(this) {};
	bool started = false;
	BuildOrder curr_build_order = BuildOrder::testing;
	Mediator mediator;
    bool debug_mode = false;
	int frames_passed = 0;
	int seconds_passed = 0;
    ScoutInfoTerran scout_info_terran;
    Race enemy_race = Race::Random;
    std::vector<std::vector<bool>> grid_map;
    NavMesh nav_mesh;
    const Unit* probe = nullptr;
	std::vector<Triangle*> overlaps;
	int current_unique_id = 0;


	// testing
	Point2D enemy_army_spawn = Point2D(34, 139);
	Point2D friendly_army_spawn = Point2D(59, 114);
	Point2D fallback_point = Point2D(142, 33);
	bool tests_set_up = false;
	bool initial_set_up = false;

	void RunInitialSetUp();
	void RunTests();

	void SpawnArmies();
	void SetUpArmies();


    Locations* locations = nullptr;
    //bool should_build_workers;
    int extra_pylons = 0;
    //int removed_gas_miners = 0;
    std::vector<Tag> proxy_pylons;
    OnUnitDamagedEvent on_unit_damaged_event;
    OnUnitDestroyedEvent on_unit_destroyed_event;
	OnUnitCreatedEvent on_unit_created_event;
	OnUnitEntersVisionEvent on_unit_enters_vision_event;

	Point2D GetNaturalDefensiveLocation(UNIT_TYPEID);
    std::vector<Point2D> GetProxyLocations(UNIT_TYPEID);
    std::vector<Point2D> FindWarpInSpots(Point2D, int);
    std::vector<Point2D> FindProxyWarpInSpots();

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
	void PrintAttacks(std::map<const Unit*, const Unit*>);
	Point3D ToPoint3D(Point2D);
	int GetUniqueId();


    // Pathing
	Polygon CreateNewBlocker(const Unit*);


    // Debug info
    void DisplayDebugHud();
	void DisplayAlliedAttackStatus();
	void PrintPathablePoints();
	void PrintNonPathablePoints();
	void ShowLocations();


};








}
