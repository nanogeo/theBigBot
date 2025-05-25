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

struct LiberatorZone
{
	Point2D pos;
	bool current;
	LiberatorZone(Point2D pos)
	{
		this->pos = pos;
		current = true;
	}
	bool operator==(const LiberatorZone &b)
	{
		return pos == b.pos;
	}
};



struct EnemyAttack
{
	const Unit* unit;
	uint32_t impact_frame;
	EnemyAttack(const Unit* unit, int impact_frame)
	{
		this->unit = unit;
		this->impact_frame = impact_frame;
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

class BlankBot : public sc2::Agent {
public:
	BlankBot() : Agent() {};
	std::map<const Unit*, bool> attacks;

	virtual void OnGameStart()
	{
		//Debug()->DebugGiveAllUpgrades();
		//Debug()->DebugFastBuild();
		//Debug()->DebugGiveAllResources();
		Debug()->SendDebug();
	}

	virtual void OnStep()
	{
		if (Observation()->GetGameLoop() == 100)
		{
			Debug()->DebugCreateUnit(SPAWNING_POOL, Observation()->GetStartLocation() + Point2D(8, 0), 2, 1);
			Debug()->DebugCreateUnit(EXTRACTOR, Observation()->GetStartLocation() + Point2D(-8, 0), 2, 1);
			Debug()->SendDebug();
		}

		if (Observation()->GetGameLoop() >= 4700 && Observation()->GetGameLoop() % 700 == 500)
		{
			Debug()->DebugCreateUnit(ZERGLING, Observation()->GetStartLocation(), 2, 20);
			Debug()->DebugGiveAllUpgrades();
			Debug()->SendDebug();
		}
		Actions()->UnitCommand(Observation()->GetUnits(IsUnit(ZERGLING)), ABILITY_ID::ATTACK, Observation()->GetGameInfo().enemy_start_locations[0]);
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
					//std::cout << "attack launched" << std::endl;
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

class ZealotRushBot : public sc2::Agent {
public:
	ZealotRushBot() : Agent() {};
	const Unit* probe = nullptr;


	virtual void OnGameStart()
	{
		//Debug()->DebugGiveAllUpgrades();
		//Debug()->DebugFastBuild();
		//Debug()->DebugGiveAllResources();
		Debug()->SendDebug();
	}

	virtual void OnStep()
	{
		// abyssal reef
		
		Point2D cpylon = Point2D(42, 121);
		Point2D cforge = Point2D(42.5, 118.5);
		Point2D cpylon2 = Point2D(158, 35);
		std::vector<Point2D> ccannons = { Point2D(161, 35), Point2D(155, 31), Point2D(159, 30) };
		if (Observation()->GetStartLocation().x == 161.5 && Observation()->GetStartLocation().y == 21.5)
		{
			cpylon = Point2D(158, 23);
			cforge = Point2D(157.5, 25.5);
			cpylon2 = Point2D(44, 109);
			ccannons = { Point2D(41, 109), Point2D(47, 113), Point2D(43, 114) };
		}

		if (probe == nullptr && Observation()->GetUnits(IsUnit(PROBE)).size() > 0)
			probe = Observation()->GetUnits(IsUnit(PROBE))[0];

		/*if (Observation()->GetUnits(IsUnit(NEXUS)).size() > 0 &&
			Observation()->GetUnits(IsUnit(NEXUS))[0]->orders.size() == 0 &&
			Observation()->GetUnits(IsUnit(PROBE)).size() < 13)
			Actions()->UnitCommand(Observation()->GetUnits(IsUnit(NEXUS))[0], ABILITY_ID::TRAIN_PROBE);*/


		if (Observation()->GetUnits(IsUnit(FORGE)).size() == 0 && Observation()->GetUnits(IsUnit(PYLON)).size() == 1)
		{
			const Unit* p = Utility::ClosestTo(Observation()->GetUnits(IsUnit(PROBE)), cforge);
			if (Distance2D(p->pos, cforge) > 1)
				Actions()->UnitCommand(p, ABILITY_ID::MOVE_MOVE, cforge);
			else
				Actions()->UnitCommand(p, ABILITY_ID::BUILD_FORGE, cforge);
		}

		if (Observation()->GetUnits(IsUnit(PYLON)).size() == 0)
		{
			if (Distance2D(probe->pos, cpylon) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, cpylon);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, cpylon);
		}
		else if (Observation()->GetUnits(Unit::Alliance::Self, IsUnit(PYLON)).size() == 1)
		{
			if (Distance2D(probe->pos, cpylon2) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, cpylon2);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, cpylon2);
		}
		else if (Observation()->GetUnits(IsUnit(CANNON)).size() < 3)
		{
			Point2D cannon_pos = ccannons[Observation()->GetUnits(IsUnit(CANNON)).size()];

			if (Distance2D(probe->pos, cannon_pos) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, cannon_pos);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_PHOTONCANNON, cannon_pos);
		}

		Point2D rally = Point2D(56, 118);
		Point2D pylon = Point2D(77, 99);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(200, 144);
		if (Observation()->GetStartLocation().x == 38.5 && Observation()->GetStartLocation().y == 122.5)
		{
		// acropolis
		/*Point2D rally = Point2D(136, 50);
		Point2D pylon = Point2D(113, 65);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(176, 172);
		if (Observation()->GetStartLocation().x == 142.5 && Observation()->GetStartLocation().y == 33.5)
		{*/
		// automation
		/*Point2D rally = Point2D(34, 50);
		Point2D pylon = Point2D(62, 65);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(184, 180);
		if (Observation()->GetStartLocation().x == 29.5 && Observation()->GetStartLocation().y == 65.5)
		{*/
		// ephemeron
		/*Point2D rally = Point2D(34, 122);
		Point2D pylon = Point2D(60, 99);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(160, 160);
		if (Observation()->GetStartLocation().x == 29.5 && Observation()->GetStartLocation().y == 138.5)
		{*/
		// interloper
		/*Point2D rally = Point2D(34, 122);
		Point2D pylon = Point2D(50, 117);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(152, 168);
		if (Observation()->GetStartLocation().x == 26.5 && Observation()->GetStartLocation().y == 137.5)
		{*/
		// thunderbird
		/*Point2D rally = Point2D(40, 118);
		Point2D pylon = Point2D(70, 95);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(192, 156);
		if (Observation()->GetStartLocation().x == 38.5 && Observation()->GetStartLocation().y == 133.5)
		{*/
			rally = swap - rally;
			pylon = swap - pylon;
			pylon2 = swap - pylon2;
			for (int i = 0; i < 4; i++)
			{
				gates[i] = swap - gates[i];
			}

		}


		if (probe == nullptr && Observation()->GetUnits(IsUnit(PROBE)).size() > 0)
			probe = Observation()->GetUnits(IsUnit(PROBE))[0];
		
		if (Observation()->GetUnits(IsUnit(NEXUS)).size() > 0 &&
			Observation()->GetUnits(IsUnit(NEXUS))[0]->orders.size() == 0 &&
			Observation()->GetUnits(IsUnit(PROBE)).size() < 14)
			Actions()->UnitCommand(Observation()->GetUnits(IsUnit(NEXUS))[0], ABILITY_ID::TRAIN_PROBE);

		if (Observation()->GetUnits(IsUnit(PYLON)).size() == 0)
		{
			if (Distance2D(probe->pos, pylon) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, pylon);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon);
		}
		else if (Observation()->GetUnits(IsUnit(GATEWAY)).size() < 4)
		{
			Point2D gate_pos = gates[Observation()->GetUnits(IsUnit(GATEWAY)).size()];

			if (Distance2D(probe->pos, gate_pos) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, gate_pos);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_GATEWAY, gate_pos);
		}
		else if (Observation()->GetUnits(IsUnit(PYLON)).size() < 2)
		{
			if (Distance2D(probe->pos, pylon2) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, pylon2);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon2);
		}

		if (Observation()->GetGameLoop() % 5 == 0)
			return;

		for (const auto& gate : Observation()->GetUnits(IsUnit(GATEWAY)))
		{
			if (gate->build_progress < 1)
				Actions()->UnitCommand(gate, ABILITY_ID::SMART, rally);

			if (gate->build_progress == 1 && gate->orders.size() == 0)
				Actions()->UnitCommand(gate, ABILITY_ID::TRAIN_ZEALOT);
		}
		for (const auto& nexus : Observation()->GetUnits(IsUnit(NEXUS)))
		{
			if (nexus->energy > 50)
			{
				bool ignore = false;
				for (const auto& gate : Observation()->GetUnits(IsUnit(GATEWAY)))
				{
					for (const auto& buff : gate->buffs)
					{
						if (buff == BUFF_ID::CHRONOBOOSTENERGYCOST)
							ignore = true;
					}
					if (ignore == false)
					{
						Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, gate);
						break;
					}
				}
			}
		}
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
	std::map<const Unit*, float> enemy_weapon_cooldown;
	std::map<const Unit*, std::vector<EnemyAttack>> enemy_attacks;
	std::unordered_map<UNIT_TYPEID, UnitTypeInfo> unit_type_info;
	std::vector<Point2D> corrosive_bile_positions;
	std::vector<uint32_t> corrosive_bile_times;
	std::vector<LiberatorZone> liberator_zone_current;
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
	void UpdateEnemyWeaponCooldowns();
	void RemoveCompletedAtacks();
	std::vector<Point2D> FindConcave(Point2D, Point2D, int, float, float);
	std::vector<Point2D> FindConcaveFromBack(Point2D, Point2D, int, float, float);
	void SetUpUnitTypeInfo();
	void PrintAttacks(std::map<const Unit*, const Unit*>);
	Point3D ToPoint3D(Point2D);
	int GetUniqueId();
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
	void DisplayOngoingAttacks();
	void DisplayEnemyAttacks();
	void DisplayEnemyPositions();
	void DisplayKnownEffects();
	void DisplayAlliedAttackStatus();
	void PrintNonPathablePoints();
	void ShowLocations();


	// Other
	std::map<const Unit*, const Unit*> FindTargets(Units, std::vector<UNIT_TYPEID>, float);

};








}
