#include "TossBot.h"
#include "finish_state_machine.h"
#include "pathfinding.h"
#include "locations.h"
#include "fire_control.h"
#include "utility.h"
#include "build_order_manager.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <algorithm>
#include <random>
#include <iterator>
#include <typeinfo>
#include <ctime>
#include <cstdlib>

#define _USE_MATH_DEFINES
#include <math.h>

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"

namespace sc2 {


#pragma region Overrides

    void TossBot::OnGameStart()
    {
        std::cout << "Hello World!" << std::endl;
		SetUpUnitTypeInfo();
		Debug()->SendDebug();
		if (debug_mode)
		{
			Debug()->DebugGiveAllResources();
			Debug()->DebugFastBuild();
			Debug()->SendDebug();
		}
    }

    void TossBot::OnStep()
    {
		std::chrono::microseconds startTime = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);

		Units bla = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ADEPT));
		frames_passed++;
        //std::cout << std::to_string(Observation()->GetGameLoop()) << '\n';
        if (debug_mode)
        {


            if (!started)
            {
                build_order_manager.SetBuildOrder(BuildOrder::testing);
				probe = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_PROBE))[0];
				started = true;
            }
            if (Observation()->GetGameLoop() > 1)
            {
                /*for (int i = 0; i < grid_map.size(); i++)
                {
                    for (int j = 0; j < grid_map[i].size(); j++)
                    {
                        Point3D pos = Point3D(i + .5, j + .5, Observation()->TerrainHeight(Point2D(i + .5, j + .5)));
                        if (grid_map[i][j])
                            Debug()->DebugSphereOut(pos, .5, Color(0, 255, 0));
                        else
                            Debug()->DebugSphereOut(pos, .5, Color(255, 0, 0));
                    }
                }
                if (nav_mesh.triangles.size() == 0)
                {
					nav_mesh.LoadOrBuildNavmesh(Observation()->GetGameInfo().pathing_grid, Observation()->GetGameInfo().map_name);
					
                }
                
                for (const auto &triangle : nav_mesh.triangles)
                {
                    for (int i = 0; i < triangle->verticies.size(); i++)
                    {
                        Point3D start = Point3D(triangle->verticies[i].x, triangle->verticies[i].y, Observation()->TerrainHeight(triangle->verticies[i]) + .1);
                        Point3D end;
                        if (i + 1 < triangle->verticies.size())
                            end = Point3D(triangle->verticies[i + 1].x, triangle->verticies[i + 1].y, Observation()->TerrainHeight(triangle->verticies[i]) + .1);
                        else
                            end = Point3D(triangle->verticies[0].x, triangle->verticies[0].y, Observation()->TerrainHeight(triangle->verticies[i]) + .1);
                        Debug()->DebugLineOut(start, end, Color(255, 0, 128));
                        Debug()->DebugSphereOut(start, .1, Color(0, 0, 255));
                        Point3D center = Point3D(triangle->center.x, triangle->center.y, Observation()->TerrainHeight(triangle->verticies[i]) + .1);
						if (triangle->pathable)
	                        Debug()->DebugSphereOut(center, .1, Color(0, 255, 255));
						else
							Debug()->DebugSphereOut(center, .1, Color(255, 0, 0));
                    }
                    for (const auto &connection : triangle->connections)
                    {
                        Point3D start = Point3D(triangle->center.x, triangle->center.y, Observation()->TerrainHeight(triangle->center) + .1);
                        Point3D end = Point3D(connection->center.x, connection->center.y, Observation()->TerrainHeight(connection->center) + .1);
                        Debug()->DebugLineOut(start, end, Color(255, 255, 0));
                    }
                }

				for (const auto &triangle : overlaps)
				{
					for (int i = 0; i < triangle->verticies.size(); i++)
					{
						Point3D center = Point3D(triangle->center.x, triangle->center.y, Observation()->GetStartLocation().z + .1);
						Debug()->DebugSphereOut(center, .3, Color(255, 255, 255));
					}
				}

                std::vector<Point2D> path = nav_mesh.FindPath(probe->pos, Observation()->GetGameInfo().enemy_start_locations[0]);

                for (int i = 0; i < path.size() - 1; i++)
                {
                    Point3D start = Point3D(path[i].x, path[i].y, Observation()->TerrainHeight(path[i]) + .1);
                    Point3D end = Point3D(path[i+1].x, path[i+1].y, Observation()->TerrainHeight(path[i+1]) + .1);
                    Debug()->DebugLineOut(start, end, Color(0, 255, 0));
                }
                Point3D start = Point3D(probe->pos.x, probe->pos.y, Observation()->TerrainHeight(probe->pos) + .1);
                Point3D end = Point3D(path[0].x, path[0].y, Observation()->TerrainHeight(path[0]) + .1);
                Debug()->DebugLineOut(start, end, Color(0, 255, 0));*/


				UpdateEnemyUnitPositions();
				UpdateEnemyWeaponCooldowns();
				/*for (const auto &unit : enemy_unit_saved_position)
				{
					Color col = Color(255, 255, 0);
					if (unit.second.frames > 0)
					{
						col = Color(0, 255, 255);
					}
					Debug()->DebugSphereOut(unit.first->pos, .7, col);
					Debug()->DebugTextOut(std::to_string(unit.second.frames), unit.first->pos, col, 20);
				}*/

				if (!initial_set_up)
				{
					RunInitialSetUp();
				}
				if (tests_set_up)
				{
					RunTests();
				}

            }
			if (Observation()->GetGameLoop() > 10 && !tests_set_up)
			{
				SetUpArmies();
			}
            if (Observation()->GetGameLoop() % 1000 == 0)
            {
                std::cout << "1000\n";
            }


			build_order_manager.CheckBuildOrder();
			action_manager.ProcessActions();
			ProcessFSMs();
			DisplayEnemyAttacks();
			DisplayAlliedAttackStatus();
			RemoveCompletedAtacks();
			DisplayDebugHud();
			Debug()->SendDebug();
            return;
        }

        
        worker_manager.DistributeWorkers();

		std::chrono::microseconds postDistributeWorkers = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);

        if (worker_manager.new_base != NULL)
        {
            std::cout << "add new base\n";
            std::cout << worker_manager.new_base->pos.x << ' ' << worker_manager.new_base->pos.y << '\n';;
            worker_manager.AddNewBase();
            if (Observation()->GetGameLoop() < 5)
            {
                worker_manager.SplitWorkers();
            }
        }

		std::chrono::microseconds postNewBase = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);

        if (Observation()->GetGameLoop() == 1)
        {
			std::ofstream frame_time_file;
			frame_time_file.open("frame_time.txt", std::ios_base::out);
			frame_time_file << "Distribute workers,New base,Update enemy pos,Update enemy weapon cd,Build workers,Check build order,Process actions,Process FSM,Display debug,Send debug\n";
			frame_time_file.close();

			std::ofstream action_time_file;
			action_time_file.open("action_time.txt", std::ios_base::out);
			action_time_file << "New units,Closest units,Concaves,Positions,Debug,ApplyPressure,UpdateIndex\n";
			action_time_file.close();

			std::ofstream find_targets_time_file;
			find_targets_time_file.open("find_targets.txt", std::ios_base::out);
			find_targets_time_file << "Set up,Constructor\n";
			find_targets_time_file.close();

			std::ofstream fire_control_time_file;
			fire_control_time_file.open("fire_control_time.txt", std::ios_base::out);
			fire_control_time_file << "Single target,Enemy min heap,Friendly min heap\n";
			fire_control_time_file.close();

			std::ofstream oracle_time_file;
			oracle_time_file.open("oracle_time.txt", std::ios_base::out);
			oracle_time_file << "Enemy in range,Query abilities,Beam active,Beam activatable,Neither,Debug text,No enemy in range\n";
			oracle_time_file.close();

			std::ofstream pressure_time_file;
			pressure_time_file.open("pressure_time.txt", std::ios_base::out);
			pressure_time_file << "Ready check,Find targets,Print attacks,Give targets,Not ready,Pick up\n";
			pressure_time_file.close();

			std::ofstream debug_hud_file;
			debug_hud_file.open("debug_hud_time.txt", std::ios_base::out);
			debug_hud_file << "Worker status,Build order,Active actions,Active FSM,Building status,Army groups,Supply info\n";
			debug_hud_file.close();

            auto infos = Observation()->GetGameInfo().player_info;
            if (infos.size() > 0)
            {
                if (infos[0].race_requested == Race::Protoss)
                    enemy_race = infos[1].race_requested;
                else
                    enemy_race = infos[0].race_requested;
            }

            const Unit *building = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS))[0];
            std::cout << Utility::UnitTypeIdToString(building->unit_type);
            if (building->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
            {
				worker_manager.new_base = building;
            }
            else if (building->unit_type == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
            {
				worker_manager.assimilators[building] = assimilator_data();
                //SaturateGas(building->tag);
            }
            
			build_order_manager.SetBuildOrder(BuildOrder::oracle_gatewayman_pvz);

        }

		/*if (Observation()->GetGameLoop() >= 2)
		{
			Point2D fallback = Point2D(75, 120);
			for (const auto &unit : Observation()->GetUnits(IsUnit(UNIT_TYPEID::ZERG_ZERGLING)))
			{
				Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, fallback);
			}
			std::map<const Unit*, std::vector<const Unit*>> units;
			Units Funits;
			Units Eunits;
			for (const auto &unit : Observation()->GetUnits())
			{
				if (unit->alliance == Unit::Alliance::Self && unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_STALKER)
					Funits.push_back(unit);
				else if (unit->alliance == Unit::Alliance::Enemy)
					Eunits.push_back(unit);
			}
			for (const auto &unit : Funits)
			{
				Units units_in_range;
				for (const auto &Eunit : Eunits)
				{
					if (Distance2D(unit->pos, Eunit->pos) <= RealGroundRange(unit, Eunit))
						units_in_range.push_back(Eunit);
				}
				units[unit] = units_in_range;
			}

			FireControl* fire_control = new FireControl(this, units, { UNIT_TYPEID::TERRAN_SCV, UNIT_TYPEID::TERRAN_MARINE });
			std::map<const Unit*, const Unit*> attacks = fire_control->FindAttacks();

			for (const auto &attack : attacks)
			{
				Debug()->DebugLineOut(attack.first->pos + Point3D(0, 0, .2), attack.second->pos + Point3D(0, 0, .2), Color(0, 0, 255));
			}

			bool all_ready = true;
			for (const auto &unit : Funits)
			{
				if (unit->weapon_cooldown > 0)
					all_ready = false;
			}
			if (all_ready)
			{
				std::cout << "fire volley\n";
				for (const auto &attack : attacks)
				{
					Actions()->UnitCommand(attack.first, ABILITY_ID::ATTACK, attack.second);
					std::cout << attack.first->tag << " " << attack.second->tag << std::endl;
				}
				std::cout << "\n";
			}
			else
			{
				for (const auto &unit : Funits)
				{
					std::cout << "move\n";
					if (unit->weapon_cooldown > 0)
						Actions()->UnitCommand(unit, ABILITY_ID::MOVE_MOVE, fallback);
				}
			}
		}*/

		UpdateEnemyUnitPositions();
		std::chrono::microseconds postUpdateEnemyUnitPositions = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
		UpdateEnemyWeaponCooldowns();
		std::chrono::microseconds postUpdateEnemyWeaponCooldowns = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);

		std::chrono::microseconds postBuildWorkers = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);;
		std::chrono::microseconds postCheckBuildOrder = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);;
		std::chrono::microseconds postProcessActions = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);;

        if (Observation()->GetGameLoop() % 2 == 0)
        {
			
            worker_manager.BuildWorkers();
			postBuildWorkers = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				);
			build_order_manager.CheckBuildOrder();
			postCheckBuildOrder = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				);
			action_manager.ProcessActions();
			postProcessActions = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				);
        }

        ProcessFSMs();
		std::chrono::microseconds postProcessFSM = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);

        //DisplayDebugHud();
		std::chrono::microseconds postDisplayDebug = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
        Debug()->SendDebug();
		std::chrono::microseconds postSendDebug = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);

		std::ofstream frame_time_file;
		frame_time_file.open("frame_time.txt", std::ios_base::app);
		frame_time_file << postDistributeWorkers.count() - startTime.count() << ", ";
		frame_time_file << postNewBase.count() - postDistributeWorkers.count() << ", ";
		frame_time_file << postUpdateEnemyUnitPositions.count() - postNewBase.count() << ", ";
		frame_time_file << postUpdateEnemyWeaponCooldowns.count() - postUpdateEnemyUnitPositions.count() << ", ";
		frame_time_file << postBuildWorkers.count() - postUpdateEnemyWeaponCooldowns.count() << ", ";
		frame_time_file << postCheckBuildOrder.count() - postBuildWorkers.count() << ", ";
		frame_time_file << postProcessActions.count() - postCheckBuildOrder.count() << ", ";
		frame_time_file << postProcessFSM.count() - postProcessActions.count() << ", ";
		frame_time_file << postDisplayDebug.count() - postProcessFSM.count() << ", ";
		frame_time_file << postSendDebug.count() - postDisplayDebug.count() << "\n";

		frame_time_file.close();
    }

    void TossBot::OnBuildingConstructionComplete(const Unit *building)
    {
        if (debug_mode)
        {
            std::cout << Utility::UnitTypeIdToString(building->unit_type) << ' ' << building->pos.x << ", " << building->pos.y << '\n';
			//nav_mesh.AddNewObstacle(building);
            return;
        }
        if (building->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
        {
			worker_manager.SetNewBase(building);
        }
        else if (building->unit_type == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
        {
			worker_manager.AddAssimilator(building);
            if (immediatelySaturateGasses)
                worker_manager.SaturateGas(building);
        }
        else if (building->unit_type == UNIT_TYPEID::PROTOSS_PYLON)
        {
            for (const auto &location : locations->proxy_pylon_locations)
            {
                if (Point2D(building->pos) == location)
                {
                    proxy_pylons.push_back(building->tag);
                }
            }
        }
    }

    void TossBot::OnNeutralUnitCreated(const Unit *building)
    {
        /*if (debug_mode)
        {
            std::cout << UnitTypeIdToString(building->unit_type) << ' ' << building->pos.x << ", " << building->pos.y << '\n';
            return;
        }*/
    }

    void TossBot::OnUnitCreated(const Unit *unit)
    {
        if (Observation()->GetGameLoop() == 0)
            return;
		CallOnUnitCreatedEvent(unit);
        if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE)
        {
            worker_manager.PlaceWorker(unit);
        }
        /*if len(self.adept_groups) > 0:
            if unit.type_id == UnitTypeId.ADEPT :
                self.adept_groups[0].adept_ids.append(unit.tag)
                self.adept_groups[0].adept_order_status.append(False)
                print("adding " + str(unit.tag) + " <Adept> to army group 0")*/
    }

    void TossBot::OnUnitDamaged(const Unit *unit, float health_damage, float shield_damage)
    {
        //std::cout << UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health_damage) << " damage\n";
		if (unit->alliance == Unit::Alliance::Enemy)
	        std::cout << unit->tag << " took " << std::to_string(health_damage) << " damage\n";
        CallOnUnitDamagedEvent(unit, health_damage, shield_damage);
    }

    void TossBot::OnUnitDestroyed(const Unit *unit)
    {
        //std::cout << UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed\n";
		if (enemy_unit_saved_position.find(unit) != enemy_unit_saved_position.end())
			enemy_unit_saved_position.erase(unit);
		if (enemy_weapon_cooldown.find(unit) != enemy_weapon_cooldown.end())
			enemy_weapon_cooldown.erase(unit);
		if (unit->alliance == Unit::Alliance::Enemy)
			std::cout << unit->tag << " destroyed\n";
        CallOnUnitDestroyedEvent(unit);
		nav_mesh.RemoveObstacle(unit);

		for (auto &army : army_groups)
		{
			army->RemoveUnit(unit);
		}

		if (debug_mode)
		{
			if (std::find(test_army.stalkers.begin(), test_army.stalkers.end(), unit) != test_army.stalkers.end())
			{
				test_army.stalkers.erase(std::remove(test_army.stalkers.begin(), test_army.stalkers.end(), unit), test_army.stalkers.end());
				test_army.attack_status.erase(unit);
			}
		}
    }


#pragma endregion

#pragma region testing

	void TossBot::RunInitialSetUp()
	{
		//Debug()->DebugFastBuild();
		//Debug()->DebugGiveAllResources();
		Debug()->DebugShowMap();
		Debug()->DebugGiveAllUpgrades();
		SpawnArmies();
		initial_set_up = true;
	}

	void TossBot::RunTests()
	{
		/*const Unit* closest_enemy = Utility::ClosestTo(Observation()->GetUnits(Unit::Alliance::Enemy), Utility::MedianCenter(test_army.stalkers));
		const Unit* closest_unit_to_enemies = Utility::ClosestTo(test_army.stalkers, closest_enemy->pos);
		const Unit* furthest_unit_from_enemies = Utility::FurthestFrom(test_army.stalkers, closest_enemy->pos);

		Debug()->DebugSphereOut(closest_unit_to_enemies->pos, .625, Color(255, 0, 255));
		Debug()->DebugSphereOut(furthest_unit_from_enemies->pos, .625, Color(0, 255, 255));

		float unit_size = .625;
		float unit_dispersion = 0;
		Point2D retreating_concave_origin = Utility::PointBetween(Utility::ClosestPointOnLine(closest_unit_to_enemies->pos, enemy_army_spawn, fallback_point), fallback_point, unit_size + unit_dispersion);
		Point2D attacking_concave_origin = Utility::PointBetween(Utility::ClosestPointOnLine(furthest_unit_from_enemies->pos, enemy_army_spawn, fallback_point), enemy_army_spawn, unit_size + unit_dispersion);

		Debug()->DebugSphereOut(Point3D(retreating_concave_origin.x, retreating_concave_origin.y, Observation()->TerrainHeight(retreating_concave_origin)), .625, Color(255, 0, 128));
		Debug()->DebugSphereOut(Point3D(attacking_concave_origin.x, attacking_concave_origin.y, Observation()->TerrainHeight(attacking_concave_origin)), .625, Color(0, 255, 128));

		std::vector<Point2D> attacking_concave_positions = FindConcaveFromBack(attacking_concave_origin, fallback_point, test_army.stalkers.size(), .625, .2);
		std::vector<Point2D> retreating_concave_positions = FindConcave(retreating_concave_origin, fallback_point, test_army.stalkers.size(), .625, .2);

		std::map<const Unit*, Point2D> attacking_unit_positions = AssignUnitsToPositions(test_army.stalkers, attacking_concave_positions);
		std::map<const Unit*, Point2D> retreating_unit_positions = AssignUnitsToPositions(test_army.stalkers, retreating_concave_positions);
		
		for (const auto &pos : attacking_concave_positions)
		{
			Debug()->DebugSphereOut(Point3D(pos.x, pos.y, Observation()->TerrainHeight(pos)), .625, Color(255, 0, 0));
		}
		for (const auto &pos : retreating_concave_positions)
		{
			Debug()->DebugSphereOut(Point3D(pos.x, pos.y, Observation()->TerrainHeight(pos)), .625, Color(0, 255, 0));
		}

		for (const auto &unit : retreating_unit_positions)
		{
			Debug()->DebugLineOut(unit.first->pos + Point3D(0, 0, .2), Point3D(unit.second.x, unit.second.y, Observation()->TerrainHeight(unit.second) + .2), Color(0, 0, 0));
			//Actions()->UnitCommand(unit.first, ABILITY_ID::MOVE_MOVE, unit.second);
		}
		Actions()->UnitCommand(test_army.prisms[0], ABILITY_ID::MOVE_MOVE, Utility::PointBetween(Utility::MedianCenter(test_army.stalkers), fallback_point, 3));
		Actions()->UnitCommand(test_army.prisms[0], ABILITY_ID::UNLOADALLAT_WARPPRISM, test_army.prisms[0]);
		ApplyPressureGrouped(&test_army, enemy_army_spawn, fallback_point, retreating_unit_positions, attacking_unit_positions);

		
		
		Units enemy_attacking_units = Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
		//Actions()->UnitCommand(enemy_attacking_units, ABILITY_ID::ATTACK, fallback_point);*/
	}

	void TossBot::SpawnArmies()
	{
		Debug()->DebugEnemyControl();
		Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ROACH, locations->attack_path[locations->attack_path.size() - 1], 2, 10);
		//Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_RAVAGER, enemy_army_spawn, 2, 5);
		Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ZERGLING, locations->attack_path[locations->attack_path.size() - 1], 2, 32);

		//Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, enemy_army_spawn, 2, 8);
		//Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARAUDER, enemy_army_spawn, 2, 1);
		//Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, enemy_army_spawn, 2, 1);

		//Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_STALKER, enemy_army_spawn, 2, 8);

		Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_ADEPT, locations->attack_path[0], 1, 1);
		Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_STALKER, locations->attack_path[0], 1, 7);
		Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_ORACLE, locations->attack_path[0], 1, 3);
	}

	void TossBot::SetUpArmies()
	{
		/*OracleHarassStateMachine* state_machine = new OracleHarassStateMachine(this, Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ORACLE)), "Oracles");
		active_FSMs.push_back(state_machine);
		BuildOrderResultArgData data = BuildOrderResultArgData();
		build_order_manager.StalkerOraclePressure(data);*/
		tests_set_up = true;
	}



#pragma endregion

#pragma region Utility


    std::vector<Point2D> TossBot::GetLocations(UNIT_TYPEID type)
    {
        switch (type)
        {
        case UNIT_TYPEID::PROTOSS_PYLON:
            return locations->pylon_locations;
            break;
        case UNIT_TYPEID::PROTOSS_GATEWAY:
            return locations->gateway_locations;
            break;
        case UNIT_TYPEID::PROTOSS_NEXUS:
            return locations->nexi_locations;
            break;
        case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
            return locations->cyber_core_locations;
            break;
        case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
            return locations->tech_locations;
            break;
        case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
            return locations->tech_locations;
            break;
        case UNIT_TYPEID::PROTOSS_STARGATE:
            return locations->tech_locations;
            break;
        case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
            return locations->assimilator_locations;
            break;
        default:
            //std::cout << "Error invalid type id in GetLocations" << std::endl;
            return std::vector<Point2D>();
        }
    }

    Point2D TossBot::GetLocation(UNIT_TYPEID type)
    {
		std::vector<Point2D> possible_locations;

		if (build_order_manager.current_build_order == BuildOrder::recessed_cannon_rush)
		{

			std::vector<UNIT_TYPEID> tech_buildings = { UNIT_TYPEID::PROTOSS_FORGE, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL,
				UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, UNIT_TYPEID::PROTOSS_ROBOTICSBAY, UNIT_TYPEID::PROTOSS_STARGATE,
				UNIT_TYPEID::PROTOSS_FLEETBEACON, UNIT_TYPEID::PROTOSS_DARKSHRINE, UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE };
			if (std::find(tech_buildings.begin(), tech_buildings.end(), type) != tech_buildings.end())
			{
				possible_locations = locations->tech_locations_cannon_rush;
			}
			else
			{
				switch (type)
				{
				case UNIT_TYPEID::PROTOSS_PYLON:
					possible_locations = locations->pylon_locations_cannon_rush;
					break;
				case UNIT_TYPEID::PROTOSS_NEXUS:
					possible_locations = locations->nexi_locations;
					break;
				case UNIT_TYPEID::PROTOSS_GATEWAY:
					possible_locations = locations->gateway_locations_cannon_rush;
					break;
				case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
					possible_locations = locations->assimilator_locations;
					break;
				case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
					possible_locations = locations->cannon_locations_cannon_rush;
					break;
				case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
					possible_locations = locations->shield_battery_locations_cannon_rush;
					break;
				default:
					std::cout << "Error invalid type id in GetLocation" << std::endl;
					return Point2D(0, 0);
				}
			}
		}
		else
		{
			std::vector<UNIT_TYPEID> tech_buildings = { UNIT_TYPEID::PROTOSS_FORGE, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL,
				UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, UNIT_TYPEID::PROTOSS_ROBOTICSBAY, UNIT_TYPEID::PROTOSS_STARGATE,
				UNIT_TYPEID::PROTOSS_FLEETBEACON, UNIT_TYPEID::PROTOSS_DARKSHRINE, UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE };
			if (std::find(tech_buildings.begin(), tech_buildings.end(), type) != tech_buildings.end())
			{
				possible_locations = locations->tech_locations;
			}
			else
			{
				switch (type)
				{
				case UNIT_TYPEID::PROTOSS_PYLON:
					possible_locations = locations->pylon_locations;
					break;
				case UNIT_TYPEID::PROTOSS_NEXUS:
					possible_locations = locations->nexi_locations;
					break;
				case UNIT_TYPEID::PROTOSS_GATEWAY:
					possible_locations = locations->gateway_locations;
					break;
				case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
					possible_locations = locations->assimilator_locations;
					break;
				case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
					possible_locations = locations->cyber_core_locations;
					break;
				default:
					std::cout << "Error invalid type id in GetLocation" << std::endl;
					return Point2D(0, 0);
				}
			}
		}

        for (const auto &point : possible_locations)
        {
            bool blocked = false;
            bool in_base = !(type == UNIT_TYPEID::PROTOSS_PYLON);
            bool in_energy_field = (type == UNIT_TYPEID::PROTOSS_PYLON || type == UNIT_TYPEID::PROTOSS_ASSIMILATOR || type == UNIT_TYPEID::PROTOSS_NEXUS);
            for (const auto &building : Observation()->GetUnits(IsBuilding()))
            {
                if (Point2D(building->pos) == point)
                {
                    blocked = true;
                    break;
                }
                if (!in_energy_field && building->unit_type == UNIT_TYPEID::PROTOSS_PYLON)
                {
                    if (Distance2D(Point2D(building->pos), point) < 6.5)
                    {
                        in_energy_field = true;
                    }
                }
                if (building->unit_type == UNIT_TYPEID::PROTOSS_NEXUS && Distance2D(building->pos, point) < 22)
                    in_base = true;
            }
            if (in_base && !blocked && in_energy_field)
                return point;
            
        }
        std::cout << "Error no viable point found in GetLocation" << std::endl;
        return Point2D(0, 0);

        /*
        if (std::find(tech_buildings.begin(), tech_buildings.end(), type) != tech_buildings.end())
        {
            int tech_buildings_in_production = 0;
            for (const auto &action : active_actions)
            {
                if (action->action == &TossBot::ActionBuildBuilding)
                {
                    for (const auto &building : tech_buildings)
                    {
                        if (building == action->action_arg->unitId)
                        {
                            tech_buildings_in_production++;
                        }
                    }
                }
            }
            int num = Observation()->GetUnits(IsUnits(tech_buildings)).size();
            return locations->tech_locations[num + tech_buildings_in_production];
        }

        int buildings_in_production = 0;
        for (const auto &action : active_actions)
        {
            if (action->action == &TossBot::ActionBuildBuilding)
            {
                if (type == action->action_arg->unitId)
                {
                    buildings_in_production++;
                }
            }
        }
        int num = 0;
        switch (type)
        {
        case UNIT_TYPEID::PROTOSS_PYLON:
            num = Observation()->GetUnits(IsUnit(type)).size();
            return locations->pylon_locations[num + buildings_in_production];
        case UNIT_TYPEID::PROTOSS_GATEWAY:
            num = Observation()->GetUnits(IsUnits({UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_WARPGATE})).size();
            buildings_in_production = 0;
            for (const auto &action : active_actions)
            {
                if (action->action == &TossBot::ActionBuildBuilding)
                {
                    if (UNIT_TYPEID::PROTOSS_GATEWAY == action->action_arg->unitId || UNIT_TYPEID::PROTOSS_WARPGATE == action->action_arg->unitId)
                    {
                        buildings_in_production++;
                    }
                }
            }
            return locations->gateway_locations[num + buildings_in_production];
        case UNIT_TYPEID::PROTOSS_NEXUS:
            num = Observation()->GetUnits(IsUnit(type)).size();
            return locations->nexi_locations[num + buildings_in_production];
        case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
            num = Observation()->GetUnits(IsUnit(type)).size();
            return locations->assimilator_locations[num + buildings_in_production];
        case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
            num = Observation()->GetUnits(IsUnit(type)).size();
            return locations->cyber_core_locations[num + buildings_in_production];
        default:
            std::cout << "Error invalid type id in GetLocation" << std::endl;
            return Point2D(0, 0);
        }*/
    }

	Point2D TossBot::GetProxyLocation(UNIT_TYPEID type)
	{
		std::vector<Point2D> possible_locations;

		if (build_order_manager.current_build_order == BuildOrder::recessed_cannon_rush)
		{
			std::vector<UNIT_TYPEID> tech_buildings = { UNIT_TYPEID::PROTOSS_FORGE, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL,
				UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, UNIT_TYPEID::PROTOSS_ROBOTICSBAY, UNIT_TYPEID::PROTOSS_STARGATE,
				UNIT_TYPEID::PROTOSS_FLEETBEACON, UNIT_TYPEID::PROTOSS_DARKSHRINE, UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE };
			if (std::find(tech_buildings.begin(), tech_buildings.end(), type) != tech_buildings.end())
			{
				possible_locations = locations->tech_locations_cannon_rush;
			}
			else
			{
				switch (type)
				{
				case UNIT_TYPEID::PROTOSS_PYLON:
					possible_locations = locations->pylon_locations_cannon_rush;
					break;
				case UNIT_TYPEID::PROTOSS_NEXUS:
					possible_locations = locations->nexi_locations;
					break;
				case UNIT_TYPEID::PROTOSS_GATEWAY:
					possible_locations = locations->gateway_locations_cannon_rush;
					break;
				case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
					possible_locations = locations->assimilator_locations;
					break;
				case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
					possible_locations = locations->cannon_locations_cannon_rush;
					break;
				case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
					possible_locations = locations->shield_battery_locations_cannon_rush;
					break;
				default:
					std::cout << "Error invalid type id in GetLocation" << std::endl;
					return Point2D(0, 0);
				}
			}
		}
		else
		{
			std::vector<UNIT_TYPEID> tech_buildings = { UNIT_TYPEID::PROTOSS_FORGE, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL,
				UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, UNIT_TYPEID::PROTOSS_ROBOTICSBAY, UNIT_TYPEID::PROTOSS_STARGATE,
				UNIT_TYPEID::PROTOSS_FLEETBEACON, UNIT_TYPEID::PROTOSS_DARKSHRINE, UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE };
			if (std::find(tech_buildings.begin(), tech_buildings.end(), type) != tech_buildings.end())
			{
				possible_locations = locations->proxy_tech_locations;
			}
			else
			{
				switch (type)
				{
				case UNIT_TYPEID::PROTOSS_PYLON:
					possible_locations = locations->proxy_pylon_locations;
					break;
				case UNIT_TYPEID::PROTOSS_NEXUS:
					possible_locations = locations->nexi_locations;
					break;
				case UNIT_TYPEID::PROTOSS_GATEWAY:
					possible_locations = locations->proxy_gateway_locations;
					break;
				default:
					std::cout << "Error invalid type id in GetLocation" << std::endl;
					return Point2D(0, 0);
				}
			}
		}

		for (const auto &point : possible_locations)
		{
			bool blocked = false;
			bool in_energy_field = (type == UNIT_TYPEID::PROTOSS_PYLON || type == UNIT_TYPEID::PROTOSS_ASSIMILATOR || type == UNIT_TYPEID::PROTOSS_NEXUS);
			for (const auto &building : Observation()->GetUnits(IsBuilding()))
			{
				if (Point2D(building->pos) == point)
				{
					blocked = true;
					break;
				}
				if (!in_energy_field && building->unit_type == UNIT_TYPEID::PROTOSS_PYLON)
				{
					if (Distance2D(Point2D(building->pos), point) < 6.5)
					{
						in_energy_field = true;
					}
				}
			}
			if (!blocked && in_energy_field)
				return point;

		}
		std::cout << "Error no viable point found in GetLocation" << std::endl;
		return Point2D(0, 0);
	}

    std::vector<Point2D> TossBot::GetProxyLocations(UNIT_TYPEID type)
    {
        switch (type)
        {
        case UNIT_TYPEID::PROTOSS_PYLON:
            return locations->proxy_pylon_locations;
            break;
        case UNIT_TYPEID::PROTOSS_GATEWAY:
            return locations->proxy_gateway_locations;
            break;
        default:
            std::cout << "Error invalid type id in GetProxyLocations" << std::endl;
            return std::vector<Point2D>();
        }
    }

    std::string TossBot::OrdersToString(std::vector<UnitOrder> orders)
    {
        std::string text = "";
        for (const auto &order : orders)
        {
            text += Utility::AbilityIdToString(order.ability_id);
            int percent = floor(order.progress * 10);
            std::string completed(percent, '|');
            std::string todo(10 - percent, '-');
            text += " <" + completed + todo + "> ";
        }
        return text;
    }

    std::vector<Point2D> TossBot::FindWarpInSpots(Point2D close_to)
    {
        std::vector<Point2D> spots;
        for (const auto &pylon : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_PYLON)))
        {
            for (int i = -7; i <= 6; i += 1)
            {
                for (int j = -7; j <= 6; j += 1)
                {
                    Point2D pos = Point2D(pylon->pos.x + i + .5, pylon->pos.y + j + .5);
                    if (Observation()->IsPathable(pos) && Distance2D(pos, pylon->pos) <= 6 && Utility::DistanceToClosest(Observation()->GetUnits(), pos) > 1.5)
                    {
                        bool blocked = false;
                        for (const auto &building : Observation()->GetUnits(IsBuilding()))
                        {
                            if (building->unit_type == UNIT_TYPEID::PROTOSS_PYLON || building->unit_type == UNIT_TYPEID::PROTOSS_PHOTONCANNON || building->unit_type == UNIT_TYPEID::PROTOSS_SHIELDBATTERY)
                            {
                                if (building->pos.x - .5 <= pos.x && building->pos.x + .5 >= pos.x && building->pos.y - .5 <= pos.y && building->pos.y + .5 >= pos.y)
                                {
                                    blocked = true;
                                    break;
                                }
                            }
                            else if (building->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
                            {
                                if (building->pos.x - 2 <= pos.x && building->pos.x + 2 >= pos.x && building->pos.y - 2 <= pos.y && building->pos.y + 2 >= pos.y)
                                {
                                    blocked = true;
                                    break;
                                }
                            }
                            else
                            {
                                if (building->pos.x - 1 <= pos.x && building->pos.x + 1 >= pos.x && building->pos.y - 1 <= pos.y && building->pos.y + 1 >= pos.y)
                                {
                                    blocked = true;
                                    break;
                                }
                            }

                        }
						for (const auto &spot : spots)
						{
							if (Distance2D(pos, spot) < 1.5)
							{
								blocked = true;
								break;
							}
						}
                        if (!blocked)
                            spots.push_back(pos);
                    }
                }
            }
        }

        for (const auto &prism : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPPRISMPHASING)))
        {
            for (int i = -4; i <= 4; i += 1)
            {
                for (int j = -4; j <= 4; j += 1)
                {
                    Point2D pos = Point2D(prism->pos.x + i + .5, prism->pos.y + j + .5);
                    if (Observation()->IsPathable(pos) && Distance2D(pos, prism->pos) <= 3.75 && Utility::DistanceToClosest(Observation()->GetUnits(), pos) > 1)
                    {
                        spots.push_back(pos);
                    }
                }
            }
        }
        sort(spots.begin(), spots.end(),
            [close_to](const Point2D & a, const Point2D & b) -> bool
        {
            return Distance2D(a, close_to) < Distance2D(b, close_to);
        });
        return spots;
    }

    std::vector<Point2D> TossBot::FindProxyWarpInSpots()
    {
        std::vector<Point2D> spots;
        for (const auto &tag : proxy_pylons)
        {
            Point2D pylon_pos = Observation()->GetUnit(tag)->pos;
            for (int i = -6; i <= 6; i += 2)
            {
                for (int j = -6; j <= 6; j += 2)
                {
                    Point2D pos = Point2D(pylon_pos.x + i + .5, pylon_pos.y + j + .5);
                    if (Observation()->IsPathable(pos) && Distance2D(pos, pylon_pos) <= 6 && Utility::DistanceToClosest(Observation()->GetUnits(), pos) > 1)
                        spots.push_back(pos);
                }
            }
        }
        return spots;
    }

    void TossBot::OraclesCoverStalkers(Units stalkers, Units oracles)
    {
        Point2D center = Utility::MedianCenter(stalkers);
        bool danger = Observation()->GetUnits(Unit::Alliance::Enemy).size() > 0 && Utility::DistanceToClosest(Observation()->GetUnits(Unit::Alliance::Enemy), center) < 5;
        
        for (const auto &oracle : oracles)
        {
            if (danger && Distance2D(oracle->pos, center) < 5)
            {
                for (const auto & ability : Query()->GetAbilitiesForUnit(oracle).abilities)
                {
                    if (ability.ability_id.ToType() == ABILITY_ID::BEHAVIOR_PULSARBEAMON)
                        Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMON, false);
                }
                const Unit* closest_unit = Utility::ClosestTo(Observation()->GetUnits(Unit::Alliance::Enemy), oracle->pos);
                if (oracle->orders.size() == 0 || oracle->orders[0].ability_id.ToType() == ABILITY_ID::GENERAL_MOVE)
                {
                    Actions()->UnitCommand(oracle, ABILITY_ID::ATTACK, closest_unit, false);
                }
            }
            else
            {
                for (const auto &ability : Query()->GetAbilitiesForUnit(oracle).abilities)
                {
                    if (ability.ability_id.ToType() == ABILITY_ID::BEHAVIOR_PULSARBEAMOFF)
                        Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
                }
                Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, center);
            }
        }
    }

    void TossBot::ProcessFSMs()
    {
		std::chrono::microseconds fsmStart = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			); 
		std::ofstream fsm_time_file;
		fsm_time_file.open("fsm_time_file.txt", std::ios_base::app);

        for (const auto &state_machine : active_FSMs)
        {
            state_machine->RunStateMachine();
			std::chrono::microseconds fsmNext = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				);
			fsm_time_file << fsmNext.count() - fsmStart.count() << ", ";

			fsmStart = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				);
        }
		fsm_time_file << "\n";
		fsm_time_file.close();
    }

#pragma endregion

#pragma region Events

    void TossBot::AddListenerToOnUnitDamagedEvent(long long id, std::function<void(const Unit*, float, float)> func)
    {
        on_unit_damaged_event.listeners[id] = func;
    }

	void TossBot::RemoveListenerToOnUnitDamagedEvent(long long id)
	{
		on_unit_damaged_event.listeners.erase(id);
	}

    void TossBot::CallOnUnitDamagedEvent(const Unit* unit, float health, float shields)
    {
        for (const auto &func : on_unit_damaged_event.listeners)
        {
            func.second(unit, health, shields);
        }
    }

    void TossBot::AddListenerToOnUnitDestroyedEvent(long long id, std::function<void(const Unit*)> func)
    {
        on_unit_destroyed_event.listeners[id] = func;
    }

	void TossBot::RemoveListenerToOnUnitDestroyedEvent(long long id)
	{
		on_unit_destroyed_event.listeners.erase(id);
	}

    void TossBot::CallOnUnitDestroyedEvent(const Unit* unit)
    {
        for (const auto &listener : on_unit_destroyed_event.listeners)
        {
			auto func = listener.second;
            func(unit);
        }
    }

	void TossBot::AddListenerToOnUnitCreatedEvent(long long id, std::function<void(const Unit*)> func)
	{
		on_unit_created_event.listeners[id] = func;
	}

	void TossBot::RemoveListenerToOnUnitCreatedEvent(long long id)
	{
		on_unit_created_event.listeners.erase(id);
	}

	void TossBot::CallOnUnitCreatedEvent(const Unit* unit)
	{
		for (const auto &listener : on_unit_created_event.listeners)
		{
			auto func = listener.second;
			func(unit);
		}
	}

#pragma endregion


	int TossBot::IncomingDamage(const Unit* unit)
	{
		int damage = 0;
		if (enemy_attacks.count(unit) > 0)
		{
			for (const auto &attack : enemy_attacks[unit])
			{
				damage += Utility::GetDamage(attack.unit, unit, Observation());
			}
		}
		return damage;
	}

	void TossBot::UpdateEnemyUnitPositions()
	{
		for (const auto &unit : Observation()->GetUnits(Unit::Alliance::Enemy))
		{
			if (enemy_unit_saved_position.count(unit) > 0)
			{
				if (enemy_unit_saved_position[unit].pos == unit->pos)
				{
					enemy_unit_saved_position[unit].frames++;
				}
				else
				{
					enemy_unit_saved_position[unit].pos = unit->pos;
					enemy_unit_saved_position[unit].frames = 0;
				}
			}
			else
			{
				enemy_unit_saved_position[unit] = EnemyUnitPosition(unit->pos);
			}
		}
	}

	void TossBot::UpdateEnemyWeaponCooldowns()
	{
		Units allied_units = Observation()->GetUnits(Unit::Alliance::Self);
		Units enemy_attacking_units = Observation()->GetUnits(Unit::Alliance::Enemy); // TODO only ranged units
		for (const auto &Eunit : enemy_attacking_units)
		{
			if (!unit_type_info[Eunit->unit_type.ToType()].is_army_unit)
				continue;

			// melee are assumed to always attack whats next to them
			if (unit_type_info[Eunit->unit_type.ToType()].is_melee)
			{
				for (const auto &Funit : allied_units)
				{
					if (Distance2D(Eunit->pos, Funit->pos) < Utility::RealGroundRange(Eunit, Funit) && Utility::IsFacing(Eunit, Funit))
					{
						EnemyAttack attack = EnemyAttack(Eunit, Observation()->GetGameLoop());
						if (enemy_attacks.count(Funit) == 0)
							enemy_attacks[Funit] = { attack };
						else
							enemy_attacks[Funit].push_back(attack);
					}
				}
				continue;
			}

			if (enemy_weapon_cooldown.count(Eunit) == 0)
				enemy_weapon_cooldown[Eunit] = 0;

			const Unit* target = Utility::AimingAt(Eunit, Observation());

			if (target != NULL && enemy_weapon_cooldown[Eunit] == 0 && enemy_unit_saved_position[Eunit].frames > Utility::GetDamagePoint(Eunit) * 22.4)
			{
				float damage_point = Utility::GetDamagePoint(Eunit);
				if (damage_point == 0)
				{
					enemy_weapon_cooldown[Eunit] = Utility::GetWeaponCooldown(Eunit) - damage_point - (1 / 22.4);
					EnemyAttack attack = EnemyAttack(Eunit, Observation()->GetGameLoop() + Utility::GetProjectileTime(Eunit, Distance2D(Eunit->pos, target->pos) - Eunit->radius - target->radius) - 1);
					if (enemy_attacks.count(target) == 0)
						enemy_attacks[target] = { attack };
					else
						enemy_attacks[target].push_back(attack);
				}
				else
				{
					enemy_weapon_cooldown[Eunit] = Utility::GetWeaponCooldown(Eunit);
					EnemyAttack attack = EnemyAttack(Eunit, Observation()->GetGameLoop() + Utility::GetProjectileTime(Eunit, Distance2D(Eunit->pos, target->pos) - Eunit->radius - target->radius));
					if (enemy_attacks.count(target) == 0)
						enemy_attacks[target] = { attack };
					else
						enemy_attacks[target].push_back(attack);
				}
			}

			if (enemy_weapon_cooldown[Eunit] > 0)
				enemy_weapon_cooldown[Eunit] -= 1 / 22.4;
			if (enemy_weapon_cooldown[Eunit] < 0)
			{
				enemy_weapon_cooldown[Eunit] = 0;
				enemy_unit_saved_position[Eunit].frames = -1;
			}
			//Debug()->DebugTextOut(std::to_string(enemy_weapon_cooldown[Eunit]), Eunit->pos + Point3D(0, 0, .2), Color(255, 0, 255), 20);
		}
	}

	void TossBot::RemoveCompletedAtacks()
	{
		for (auto &attack : enemy_attacks)
		{
			for (int i = attack.second.size() - 1; i >= 0; i--)
			{
				if (attack.second[i].impact_frame <= Observation()->GetGameLoop())
				{
					attack.second.erase(attack.second.begin() + i);
				}
			}
		}
	}

	std::vector<Point2D> TossBot::FindConcave(Point2D origin, Point2D fallback_point, int num_units, float unit_size, float dispersion)
	{
		float range = 0; //r
		float unit_radius = unit_size + dispersion; //u
		float concave_degree = 30; //p
		int max_width = 4;

		Point2D backward_vector = fallback_point - origin;
		Point2D forward_vector = origin - fallback_point;
		forward_vector /= sqrt(forward_vector.x * forward_vector.x + forward_vector.y * forward_vector.y);

		Point2D offset_circle_center = Point2D(origin.x + concave_degree * forward_vector.x, origin.y + concave_degree * forward_vector.y);

		float backwards_direction = atan2(backward_vector.y, backward_vector.x);
		float arclength = (2 * unit_radius) / (range + concave_degree + unit_radius);
		
		std::vector<Point2D> concave_points;
		
		int row = 0;

		while(concave_points.size() < num_units)
		{
			row++;
			// even row
			bool left_limit = false;
			bool right_limit = false;
			float arclength = (2 * unit_radius) / (range + concave_degree + (((row * 2) - 1) * unit_radius));
			for (float i = .5; i <= max_width - .5; i += 1)
			{
				if (!right_limit)
				{
					float unit_direction = backwards_direction + i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
					if (Observation()->IsPathable(unit_position))
					{
						concave_points.push_back(unit_position);
					}
					else
					{
						right_limit = true;
					}
				}
				if ((right_limit && left_limit) || concave_points.size() >= num_units)
					break;

				if (!left_limit)
				{
					float unit_direction = backwards_direction - i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
					if (Observation()->IsPathable(unit_position))
					{
						concave_points.push_back(unit_position);
					}
					else
					{
						left_limit = true;
					}
				}
				if ((right_limit && left_limit) || concave_points.size() >= num_units)
					break;
			}
			if (concave_points.size() >= num_units)
				break;

			// odd row
			row++;
			// middle point
			float unit_direction = backwards_direction;
			Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
				offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
			if (Observation()->IsPathable(unit_position))
			{
				concave_points.push_back(unit_position);
			}

			left_limit = false;
			right_limit = false;
			arclength = (2 * unit_radius) / (range + concave_degree + (((row * 2) - 1) * unit_radius));
			for (int i = 1; i <= max_width - 1; i++)
			{
				if (!right_limit)
				{
					float unit_direction = backwards_direction + i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
					if (Observation()->IsPathable(unit_position))
					{
						concave_points.push_back(unit_position);
					}
					else
					{
						right_limit = true;
					}
				}
				if ((right_limit && left_limit) || concave_points.size() >= num_units)
					break;

				if (!left_limit)
				{
					float unit_direction = backwards_direction - i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
					if (Observation()->IsPathable(unit_position))
					{
						concave_points.push_back(unit_position);
					}
					else
					{
						left_limit = true;
					}
				}
				if ((right_limit && left_limit) || concave_points.size() >= num_units)
					break;
			
			}
		}
		return concave_points;
	}

	std::vector<Point2D> TossBot::FindConcaveFromBack(Point2D origin, Point2D fallback_point, int num_units, float unit_size, float dispersion)
	{
		Point2D current_origin = origin;
		while (true)
		{
			std::vector<Point2D> concave_points = FindConcave(current_origin, fallback_point, num_units, unit_size, dispersion);
			Point2D furthest_back = Utility::ClosestPointOnLine(concave_points.back(), origin, fallback_point);
			if (Distance2D(origin, fallback_point) < Distance2D(furthest_back, fallback_point))
				return concave_points;
			current_origin = Utility::PointBetween(current_origin, fallback_point, -(unit_size + dispersion));
		}

	}

	void TossBot::SetUpUnitTypeInfo()
	{
		unit_type_info[UNIT_TYPEID::PROTOSS_PROBE] =					UnitTypeInfo(true, false, true, false); // protoss
		unit_type_info[UNIT_TYPEID::PROTOSS_ZEALOT] =					UnitTypeInfo(true, true, true, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_STALKER] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::PROTOSS_SENTRY] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::PROTOSS_ADEPT] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_HIGHTEMPLAR] =				UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_DARKTEMPLAR] =				UnitTypeInfo(true, true, true, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_IMMORTAL] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_COLOSSUS] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_DISRUPTOR] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_ARCHON] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::PROTOSS_OBSERVER] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_WARPPRISM] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_WARPPRISMPHASING] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_PHOENIX] =					UnitTypeInfo(false, true, false, true);
		unit_type_info[UNIT_TYPEID::PROTOSS_VOIDRAY] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::PROTOSS_ORACLE] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_CARRIER] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::PROTOSS_TEMPEST] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::PROTOSS_MOTHERSHIP] =				UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::PROTOSS_NEXUS] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_PYLON] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_ASSIMILATOR] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_ASSIMILATORRICH] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_GATEWAY] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_WARPGATE] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_FORGE] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_CYBERNETICSCORE] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_PHOTONCANNON] =				UnitTypeInfo(false, false, true, true);
		unit_type_info[UNIT_TYPEID::PROTOSS_SHIELDBATTERY] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_STARGATE] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_ROBOTICSBAY] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_FLEETBEACON] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::PROTOSS_DARKSHRINE] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_SCV] =						UnitTypeInfo(true, false, true, false); // terran
		unit_type_info[UNIT_TYPEID::TERRAN_MULE] =						UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_MARINE] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::TERRAN_MARAUDER] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::TERRAN_REAPER] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::TERRAN_GHOST] =						UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::TERRAN_HELLION] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::TERRAN_HELLIONTANK] =				UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::TERRAN_SIEGETANK] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::TERRAN_SIEGETANKSIEGED] =			UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::TERRAN_CYCLONE] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::TERRAN_WIDOWMINE] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_THOR] =						UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::TERRAN_THORAP] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::TERRAN_VIKINGASSAULT] =				UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::TERRAN_VIKINGFIGHTER] = 			UnitTypeInfo(false, true, false, true);
		unit_type_info[UNIT_TYPEID::TERRAN_MEDIVAC] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_LIBERATOR] =					UnitTypeInfo(false, true, false, true);
		unit_type_info[UNIT_TYPEID::TERRAN_LIBERATORAG] =				UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::TERRAN_RAVEN] =						UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_BANSHEE] =					UnitTypeInfo(false, true, false, true);
		unit_type_info[UNIT_TYPEID::TERRAN_BATTLECRUISER] =				UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::TERRAN_AUTOTURRET] =				UnitTypeInfo(false, false, true, true);
		unit_type_info[UNIT_TYPEID::TERRAN_COMMANDCENTER] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_ORBITALCOMMAND] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_PLANETARYFORTRESS] =			UnitTypeInfo(false, false, true, false);
		unit_type_info[UNIT_TYPEID::TERRAN_SUPPLYDEPOT] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_REFINERY] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_REFINERYRICH] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_BARRACKS] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_BARRACKSFLYING] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_BARRACKSREACTOR] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_BARRACKSTECHLAB] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_ENGINEERINGBAY] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_BUNKER] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_PREVIEWBUNKERUPGRADED] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_SENSORTOWER] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_MISSILETURRET] =				UnitTypeInfo(false, false, false, true);
		unit_type_info[UNIT_TYPEID::TERRAN_FACTORY] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_FACTORYFLYING] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_FACTORYREACTOR] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_FACTORYTECHLAB] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_GHOSTACADEMY] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_STARPORT] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_STARPORTFLYING] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_STARPORTREACTOR] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_STARPORTTECHLAB] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_ARMORY] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::TERRAN_FUSIONCORE] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_LARVA] =						UnitTypeInfo(false, false, false, false); // zerg
		unit_type_info[UNIT_TYPEID::ZERG_BANELINGCOCOON] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_BROODLORDCOCOON] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_OVERLORDCOCOON] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_RAVAGERCOCOON] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_TRANSPORTOVERLORDCOCOON] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_DRONE] =						UnitTypeInfo(true, false, true, false);
		unit_type_info[UNIT_TYPEID::ZERG_DRONEBURROWED] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_QUEEN] =						UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::ZERG_QUEENBURROWED] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_ZERGLING] =					UnitTypeInfo(true, true, true, false);
		unit_type_info[UNIT_TYPEID::ZERG_ZERGLINGBURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_BANELING] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::ZERG_BANELINGBURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_ROACH] =						UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::ZERG_ROACHBURROWED] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_RAVAGER] =						UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::RAVAGERBURROWED] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_HYDRALISK] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::ZERG_HYDRALISKBURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_LURKERMP] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_LURKERMPBURROWED] =			UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::ZERG_LURKERMPEGG] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_INFESTOR] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_INFESTORBURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_SWARMHOSTMP] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_SWARMHOSTBURROWEDMP] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_ULTRALISK] =					UnitTypeInfo(true, true, true, false);
		unit_type_info[UNIT_TYPEID::ZERG_ULTRALISKBURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_OVERLORD] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_OVERLORDTRANSPORT] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_OVERSEER] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_OVERSEERSIEGEMODE] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_MUTALISK] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[UNIT_TYPEID::ZERG_CORRUPTOR] =					UnitTypeInfo(false, true, false, true);
		unit_type_info[UNIT_TYPEID::ZERG_BROODLORD] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::ZERG_VIPER] =						UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_LOCUSTMP] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[UNIT_TYPEID::ZERG_LOCUSTMPFLYING] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_BROODLING] =					UnitTypeInfo(true, true, true, false);
		unit_type_info[UNIT_TYPEID::ZERG_CHANGELING] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_CHANGELINGMARINE] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_CHANGELINGMARINESHIELD] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_CHANGELINGZEALOT] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_CHANGELINGZERGLING] = 			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_CHANGELINGZERGLINGWINGS] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_HATCHERY] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_LAIR] =						UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_HIVE] =						UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_SPINECRAWLER] =				UnitTypeInfo(false, false, true, false);
		unit_type_info[UNIT_TYPEID::ZERG_SPINECRAWLERUPROOTED] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_SPORECRAWLER] =				UnitTypeInfo(false, false, false, true);
		unit_type_info[UNIT_TYPEID::ZERG_SPORECRAWLERUPROOTED] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_EXTRACTOR] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_EXTRACTORRICH] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_SPAWNINGPOOL] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_ROACHWARREN] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_BANELINGNEST] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_HYDRALISKDEN] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_LURKERDENMP] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_INFESTATIONPIT] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_SPIRE] =						UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_GREATERSPIRE] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_ULTRALISKCAVERN] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_NYDUSCANAL] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_NYDUSNETWORK] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_CREEPTUMOR] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_CREEPTUMORBURROWED] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[UNIT_TYPEID::ZERG_CREEPTUMORQUEEN] =				UnitTypeInfo(false, false, false, false);
	}

	void TossBot::PrintAttacks(std::map<const Unit*, const Unit*> attacks)
	{
		std::cout << "Volley \n";
		for (const auto &attack : attacks)
		{
			std::cout << "    attack from " << std::to_string(attack.first->tag) << " to " << std::to_string(attack.second->tag) << " health " << std::to_string(attack.second->health) << "\n";
		}
	}

	bool TossBot::UnitIsOccupied(const Unit* unit)
	{
		for (ArmyGroup* army_group : army_groups)
		{
			for (const Unit* army_unit : army_group->all_units)
			{
				if (unit == army_unit)
					return true;
			}
		}
		return false;
	}

	Point3D TossBot::ToPoint3D(Point2D point)
	{
		float height = Observation()->TerrainHeight(point);
		return Point3D(point.x, point.y, height);
	}

	Polygon TossBot::CreateNewBlocker(const Unit* unit)
	{
		Polygon polygon;
		switch (unit->unit_type.ToType())
		{
		case UNIT_TYPEID::PROTOSS_PYLON:
			polygon.points.push_back(unit->pos + Point2D(3, 3));
			polygon.points.push_back(unit->pos + Point2D(3, -3));
			polygon.points.push_back(unit->pos + Point2D(-3, -3));
			polygon.points.push_back(unit->pos + Point2D(-3, 3));
			break;
		case UNIT_TYPEID::TERRAN_MISSILETURRET:
			polygon.points.push_back(unit->pos + Point2D(12, 3));
			polygon.points.push_back(unit->pos + Point2D(10, 7));
			polygon.points.push_back(unit->pos + Point2D(7, 10));
			polygon.points.push_back(unit->pos + Point2D(3, 12));
			polygon.points.push_back(unit->pos + Point2D(-4, 12));
			polygon.points.push_back(unit->pos + Point2D(-8, 10));
			polygon.points.push_back(unit->pos + Point2D(-11, 7));
			polygon.points.push_back(unit->pos + Point2D(-13, 3));
			polygon.points.push_back(unit->pos + Point2D(-13, -4));
			polygon.points.push_back(unit->pos + Point2D(-11, -8));
			polygon.points.push_back(unit->pos + Point2D(-8, -11));
			polygon.points.push_back(unit->pos + Point2D(-4, -13));
			polygon.points.push_back(unit->pos + Point2D(3, -13));
			polygon.points.push_back(unit->pos + Point2D(7, -11));
			polygon.points.push_back(unit->pos + Point2D(10, -8));
			polygon.points.push_back(unit->pos + Point2D(12, -4));
			break;
		default:
			break;
		}
		return polygon;
	}

#pragma endregion


#pragma region other

    IsFinishedUnit::IsFinishedUnit(UNIT_TYPEID type_) : m_type(type_) {
    }

    bool IsFinishedUnit::operator()(const Unit& unit_) const {
        return unit_.unit_type == m_type && unit_.build_progress == 1;
    }

	IsFightingUnit::IsFightingUnit(Unit::Alliance alliance_) : m_type(alliance_) {
	}

	bool IsFightingUnit::operator()(const Unit& unit_) const {
		if (unit_.alliance != m_type)
			return false;
		for (const auto &type : { UNIT_TYPEID::PROTOSS_PHOTONCANNON, /*UNIT_TYPEID::PROTOSS_PROBE,*/ UNIT_TYPEID::PROTOSS_ZEALOT, UNIT_TYPEID::PROTOSS_SENTRY,
			UNIT_TYPEID::PROTOSS_STALKER, UNIT_TYPEID::PROTOSS_ADEPT, UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, UNIT_TYPEID::PROTOSS_DARKTEMPLAR, UNIT_TYPEID::PROTOSS_ARCHON,
			UNIT_TYPEID::PROTOSS_IMMORTAL, UNIT_TYPEID::PROTOSS_COLOSSUS, UNIT_TYPEID::PROTOSS_PHOENIX, UNIT_TYPEID::PROTOSS_VOIDRAY, UNIT_TYPEID::PROTOSS_ORACLE,
			UNIT_TYPEID::PROTOSS_CARRIER, UNIT_TYPEID::PROTOSS_TEMPEST, UNIT_TYPEID::PROTOSS_MOTHERSHIP, UNIT_TYPEID::TERRAN_PLANETARYFORTRESS, UNIT_TYPEID::TERRAN_MISSILETURRET,
			/*UNIT_TYPEID::TERRAN_SCV,*/ UNIT_TYPEID::TERRAN_MARINE, UNIT_TYPEID::TERRAN_MARAUDER, UNIT_TYPEID::TERRAN_REAPER, UNIT_TYPEID::TERRAN_GHOST,
			UNIT_TYPEID::TERRAN_HELLION, UNIT_TYPEID::TERRAN_HELLIONTANK, UNIT_TYPEID::TERRAN_SIEGETANK, UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, UNIT_TYPEID::TERRAN_CYCLONE,
			UNIT_TYPEID::TERRAN_THOR, UNIT_TYPEID::TERRAN_THORAP, UNIT_TYPEID::TERRAN_AUTOTURRET, UNIT_TYPEID::TERRAN_VIKINGASSAULT, UNIT_TYPEID::TERRAN_VIKINGFIGHTER,
			UNIT_TYPEID::TERRAN_LIBERATOR, UNIT_TYPEID::TERRAN_LIBERATORAG, UNIT_TYPEID::TERRAN_BANSHEE, UNIT_TYPEID::TERRAN_BATTLECRUISER, UNIT_TYPEID::ZERG_SPINECRAWLER,
			UNIT_TYPEID::ZERG_SPORECRAWLER, /*UNIT_TYPEID::ZERG_DRONE,*/ UNIT_TYPEID::ZERG_QUEEN, UNIT_TYPEID::ZERG_ZERGLING, UNIT_TYPEID::ZERG_BANELING, UNIT_TYPEID::ZERG_ROACH,
			UNIT_TYPEID::ZERG_RAVAGER, UNIT_TYPEID::ZERG_HYDRALISK, UNIT_TYPEID::ZERG_LURKERMP, UNIT_TYPEID::ZERG_ULTRALISK, UNIT_TYPEID::ZERG_MUTALISK, UNIT_TYPEID::ZERG_CORRUPTOR,
			UNIT_TYPEID::ZERG_BROODLORD, UNIT_TYPEID::ZERG_LOCUSTMP, UNIT_TYPEID::ZERG_BROODLING })
		{
			if (unit_.unit_type.ToType() == type)
				return true;
		}
		return false;

	}

#pragma endregion

#pragma region debug info

    void TossBot::DisplayDebugHud()
    {
		std::chrono::microseconds start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
        DisplayWorkerStatus();
		std::chrono::microseconds worker_status = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
        DisplayBuildOrder();
		std::chrono::microseconds build_order = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
        DisplayActiveActions();
		std::chrono::microseconds active_actions = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
        DisplayActiveStateMachines();
		std::chrono::microseconds active_fsm = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
        DisplayBuildingStatuses();
		std::chrono::microseconds building_status = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
        DisplayArmyGroups();
		std::chrono::microseconds army_groups = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
        DisplaySupplyInfo();
		std::chrono::microseconds suply_info = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);


		std::ofstream debug_hud_file;
		debug_hud_file.open("debug_hud_time.txt", std::ios_base::app);
		debug_hud_file << worker_status.count() - start_time.count() << ", ";
		debug_hud_file << build_order.count() - worker_status.count() << ", ";
		debug_hud_file << active_actions.count() - build_order.count() << ", ";
		debug_hud_file << active_fsm.count() - active_actions.count() << ", ";
		debug_hud_file << building_status.count() - active_fsm.count() << ", ";
		debug_hud_file << army_groups.count() - building_status.count() << ", ";
		debug_hud_file << suply_info.count() - army_groups.count() << "\n";

		debug_hud_file.close();
    }

    void TossBot::DisplayWorkerStatus()
    {
        Debug()->DebugTextOut("first 2 spaces: " + std::to_string(worker_manager.first_2_mineral_patch_spaces.size()), Point2D(0, 0), Color(0, 255, 255), 20);
        if (worker_manager.close_3_mineral_patch_extras.size() > 0)
            Debug()->DebugTextOut("\nclose 3rd extras: " + std::to_string(worker_manager.close_3_mineral_patch_extras.size()), Point2D(0, 0), Color(255, 0, 255), 20);
        else
            Debug()->DebugTextOut("\nclose 3rd spaces: " + std::to_string(worker_manager.close_3_mineral_patch_spaces.size()), Point2D(0, 0), Color(0, 255, 255), 20);
        if (worker_manager.far_3_mineral_patch_extras.size() > 0)
            Debug()->DebugTextOut("\n\nfar 3rd extras: " + std::to_string(worker_manager.far_3_mineral_patch_extras.size()), Point2D(0, 0), Color(255, 0, 255), 20);
        else
            Debug()->DebugTextOut("\n\nfar 3rd spaces: " + std::to_string(worker_manager.far_3_mineral_patch_spaces.size()), Point2D(0, 0), Color(0, 255, 255), 20);



        std::string close_patches = "\n\n\n";
        std::string far_patches = "\n\n\n";
        std::string gasses = "\n\n\n";
        for (const auto &data : worker_manager.mineral_patches)
        {
            if (data.second.is_close)
            {
                far_patches += "\n";
                gasses += "\n";
                close_patches += "close:";
                if (data.second.workers[0] != NULL)
                    close_patches += " X";
                if (data.second.workers[1] != NULL)
                    close_patches += " X";
                if (data.second.workers[2] != NULL)
                    close_patches += " X";
                close_patches += "\n";
            }
        }
        for (const auto &data : worker_manager.mineral_patches)
        {
            if (!data.second.is_close)
            {
                gasses += "\n";
                far_patches += "far:";
                if (data.second.workers[0] != NULL)
                    far_patches += " X";
                if (data.second.workers[1] != NULL)
                    far_patches += " X";
                if (data.second.workers[2] != NULL)
                    far_patches += " X";
                far_patches += "\n";
            }
        }
        for (const auto &data : worker_manager.assimilators)
        {
            gasses += "gas:";
            if (data.second.workers[0] != NULL)
                gasses += " X";
            if (data.second.workers[1] != NULL)
                gasses += " X";
            if (data.second.workers[2] != NULL)
                gasses += " X";
            gasses += "\n";
        }

        Debug()->DebugTextOut(close_patches, Point2D(0, 0), Color(255, 255, 0), 20);
        Debug()->DebugTextOut(far_patches, Point2D(0, 0), Color(255, 128, 0), 20);
        Debug()->DebugTextOut(gasses, Point2D(0, 0), Color(0, 255, 0), 20);
    }

    void TossBot::DisplayBuildOrder()
    {
        std::string build_order_message = "";
        for (int i = build_order_manager.build_order_step; i < build_order_manager.build_order_step + 5; i++)
        {
            if (i >= build_order_manager.build_order.size())
                break;
            build_order_message += build_order_manager.build_order[i].toString() + "\n";
        }
        Debug()->DebugTextOut(build_order_message, Point2D(.7, .1), Color(0, 255, 0), 20);
    }

    void TossBot::DisplayActiveActions()
    {
        std::string actions_message = "Active Actions:\n";
        for (int i = 0; i < action_manager.active_actions.size(); i++)
        {
            actions_message += action_manager.active_actions[i]->toString() + "\n";
            const Unit* unit = action_manager.active_actions[i]->action_arg->unit;
            if (unit != NULL)
                Debug()->DebugTextOut(action_manager.active_actions[i]->toString(), unit->pos, Color(0, 255, 0), 20);
        }
        Debug()->DebugTextOut(actions_message, Point2D(.1, 0), Color(0, 255, 0), 20);
    }

    void TossBot::DisplayActiveStateMachines()
    {
        std::string actions_message = "Active StateMachines:\n";
        for (int i = 0; i < active_FSMs.size(); i++)
        {
            actions_message += active_FSMs[i]->toString() + "\n";
        }
        Debug()->DebugTextOut(actions_message, Point2D(.3, 0), Color(0, 255, 0), 20);
    }

    void TossBot::DisplayBuildingStatuses()
    {
        std::string new_lines = "";
        std::vector<UNIT_TYPEID> builging_order = { UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_WARPGATE, UNIT_TYPEID::PROTOSS_FORGE, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL };
        for (const auto &building_type : builging_order)
        {
            std::vector<const Unit*> buildings = Observation()->GetUnits(IsUnit(building_type));
            sort(begin(buildings), end(buildings), [](const Unit* a, const Unit* b) { return a->tag < b->tag; });
            for (const auto &building : buildings)
            {
                std::string info = Utility::UnitTypeIdToString(building_type) + " ";
                Color text_color = Color(0, 255, 0);
                /*if (building_type == UNIT_TYPEID::PROTOSS_WARPGATE)
                {
                    for (const auto & ability : Query()->GetAbilitiesForUnit(building).abilities)
                    {
                        if (ability.ability_id.ToType() == ABILITY_ID::TRAINWARP_ZEALOT)
                        {
                            text_color = Color(255, 0, 0);
                        }
                    }
                }
                else */if (building->orders.empty())
                {
                    text_color = Color(255, 0, 0);
                }
                if (building->build_progress < 1)
                {
                    text_color = Color(255, 255, 0);
                }
                for (const auto & buff : building->buffs)
                {
                    if (buff == BUFF_ID::CHRONOBOOSTENERGYCOST)
                    {
                        text_color = Color(0, 255, 255);
                    }
                }
                if (!building->orders.empty())
                {
                    info += "Orders: " + OrdersToString(building->orders);
                }
                Debug()->DebugTextOut(new_lines + info, Point2D(0, .5), text_color, 20);
                new_lines += "\n";
            }
        }

    }

    void TossBot::DisplayArmyGroups()
    {
        std::string army_info = "Armies:\n";
        for (int i = 0; i < army_groups.size(); i++)
        {
            army_info += "    Army " + std::to_string(i+1);
            army_info += ":\n";
            army_info += "Stalkers: " + std::to_string(army_groups[i]->stalkers.size());
            army_info += ", ";
            army_info += "Observers: " + std::to_string(army_groups[i]->observers.size());
            army_info += ", \n";
            army_info += "Prisms: " + std::to_string(army_groups[i]->warp_prisms.size());
            army_info += ", ";
            army_info += "Immortals: " + std::to_string(army_groups[i]->immortals.size());
            army_info += "\n";
        }
        Debug()->DebugTextOut(army_info, Point2D(.8, .5), Color(255, 255, 255), 20);
    }

    void TossBot::DisplaySupplyInfo()
    {
        std::string supply_message = "";
        int cap = Observation()->GetFoodCap();
        int used = Observation()->GetFoodUsed();
        supply_message += "supply: " + std::to_string(used) + '/' + std::to_string(cap) + '\n';
        int build_pylon_actions = 0;
        for (const auto &action : action_manager.active_actions)
        {
            if (action->action == &ActionManager::ActionBuildBuilding && action->action_arg->unitId == UNIT_TYPEID::PROTOSS_PYLON)
            {
                build_pylon_actions++;
            }
        }
        int pending_pylons = 0;
        for (const auto &pylon : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_PYLON)))
        {
            if (pylon->build_progress < 1)
                pending_pylons++;
        }
        if (pending_pylons > 0)
            supply_message += "pylons pending: " + std::to_string(pending_pylons) + '\n';
        if (build_pylon_actions > 0)
            supply_message += "almost pending pylons: " + std::to_string(build_pylon_actions) + '\n';
        if (pending_pylons > 0 || build_pylon_actions > 0)
            supply_message += "new supply: " + std::to_string(used) + '/' + std::to_string(cap + 8 * pending_pylons + 8 * build_pylon_actions) + '\n';

        int gates = Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE)).size();
        int other_prod = Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)).size() + Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_STARGATE)).size();
        if (gates > 0)
            supply_message += "warpgates: " + std::to_string(gates) + '\n';
        if (other_prod > 0)
            supply_message += "other prod: " + std::to_string(other_prod) + '\n';
        int nexi = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)).size();
        supply_message += "nexi: " + std::to_string(nexi) + '\n';
        supply_message += "new supply: " + std::to_string(used + 2 * gates + 3 * other_prod + nexi) + '/' + std::to_string(cap + 8 * pending_pylons + 8 * build_pylon_actions) + '\n';

        Debug()->DebugTextOut(supply_message, Point2D(.9, .05), Color(0, 255, 0), 20);
    }

	void TossBot::DisplayEnemyAttacks()
	{
		std::string message = "Current frame: " + std::to_string(Observation()->GetGameLoop()) + "\n";
		message += "Current time: " + std::to_string(frames_passed / 22.4) + "\n";
		for (const auto &unit : enemy_attacks)
		{
			message += Utility::UnitTypeIdToString(unit.first->unit_type.ToType());
			message += ":\n";
			for (const auto &attack : unit.second)
			{
				message += "    ";
				message += Utility::UnitTypeIdToString(attack.unit->unit_type.ToType());
				message += " - " + std::to_string(attack.impact_frame) + "\n";
			}
		}
		Debug()->DebugTextOut(message, Point2D(.8, .4), Color(255, 0, 0), 20);
	}

	void TossBot::DisplayAlliedAttackStatus()
	{
		return;
		for (const auto &unit : test_army.attack_status)
		{
			Color col1 = Color(255, 0, 0);
			if (unit.first->weapon_cooldown == 0)
				col1 = Color(255, 255, 0);

			Color col2 = Color(255, 0, 255);
			if (unit.second == true)
			{
				col2 = Color(0, 255, 255);
				Debug()->DebugTextOut("true", unit.first->pos + Point3D(0, 0, .2), col2, 15);
			}
			else
			{
				Debug()->DebugTextOut("false", unit.first->pos + Point3D(0, 0, .2), col2, 15);
			}

			Debug()->DebugTextOut(std::to_string(unit.first->weapon_cooldown), unit.first->pos, col1, 15);
		}
	}


    void TossBot::ObserveAttackPath(Units observers, Point2D retreat_point, Point2D attack_point)
    {
        for (const auto &ob : observers)
        {
            Actions()->UnitCommand(ob, ABILITY_ID::GENERAL_MOVE, attack_point);
        }
    }

    void TossBot::StalkerAttackTowards(Units stalkers, Point2D retreat_point, Point2D attack_point, bool ob_in_position)
    {
        for (const auto &stalker : stalkers)
        {
            Units close_enemies;
            for (const auto &unit : Observation()->GetUnits(Unit::Alliance::Enemy))
            {
                if (unit->is_building)
                    continue;
                if (Distance2D(unit->pos, stalker->pos) < 10)
                    close_enemies.push_back(unit);
            }
            bool weapon_ready = stalker->weapon_cooldown == 0;
            bool blink_ready = false;
            for (const auto &abiliy : Query()->GetAbilitiesForUnit(stalker).abilities)
            {
                if (abiliy.ability_id == ABILITY_ID::EFFECT_BLINK_STALKER)
                {
                    blink_ready = true;
                    break;
                }
            }
            Debug()->DebugTextOut(std::to_string(stalker->weapon_cooldown), stalker->pos, Color(0, 255, 255), 20);
            if (stalker->weapon_cooldown == 0)
                Debug()->DebugSphereOut(stalker->pos, .7, Color(0, 255, 0));
            else
                Debug()->DebugSphereOut(stalker->pos, .7, Color(255, 0, 0));
            if (find(stalker->buffs.begin(), stalker->buffs.end(), BUFF_ID::LOCKON) != stalker->buffs.end())
            {
                if (blink_ready)
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, Utility::PointBetween(stalker->pos, retreat_point, 4));
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
            }
            else if (stalker->shield == 0)
            {
                if (Utility::DangerLevel(stalker, Observation()) > 0 && blink_ready)
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, Utility::PointBetween(stalker->pos, retreat_point, 4));
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
            }
            else if (!weapon_ready)
            {
                if (Utility::DangerLevel(stalker, Observation()) > stalker->shield && blink_ready)
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, Utility::PointBetween(stalker->pos, retreat_point, 4));
                else if (close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, stalker->pos) - 2 < Utility::RealGroundRange(stalker, Utility::ClosestTo(close_enemies, stalker->pos)))
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attack_point);
            }
            else if (close_enemies.size() > 0)
            {
                if (!ob_in_position && Utility::IsOnHighGround(stalker->pos, Utility::ClosestTo(close_enemies, stalker->pos)->pos) || Utility::ClosestTo(close_enemies, stalker->pos)->display_type == Unit::DisplayType::Snapshot)
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, Utility::ClosestTo(close_enemies, stalker->pos));
            }
            else
            {
                Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attack_point);
            }
        }
    }

    void TossBot::StalkerAttackTowardsWithPrism(Units stalkers, Units prisms, Point2D retreat_point, Point2D attack_point, bool ob_in_position)
    {
        for (const auto &prism : prisms)
        {
            Units close_enemies;
            for (const auto &unit : Observation()->GetUnits(Unit::Alliance::Enemy))
            {
                if (Distance2D(unit->pos, prism->pos) < 10)
                    close_enemies.push_back(unit);
            }
            if (close_enemies.size() == 0)
                Actions()->UnitCommand(prism, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(stalkers));
            else
                Actions()->UnitCommand(prism, ABILITY_ID::GENERAL_MOVE, retreat_point);
            Actions()->UnitCommand(prism, ABILITY_ID::UNLOADALLAT_WARPPRISM, prism);
        }

        for (const auto &stalker : stalkers)
        {
            Units close_enemies;
            for (const auto &unit : Observation()->GetUnits(Unit::Alliance::Enemy))
            {
                if (Distance2D(unit->pos, stalker->pos) < 10)
                    close_enemies.push_back(unit);
            }
            bool weapon_ready = stalker->weapon_cooldown == 0;
            bool blink_ready = false;
            for (const auto &abiliy : Query()->GetAbilitiesForUnit(stalker).abilities)
            {
                if (abiliy.ability_id == ABILITY_ID::EFFECT_BLINK_STALKER)
                {
                    blink_ready = true;
                    break;
                }
            }

            Debug()->DebugTextOut(std::to_string(stalker->weapon_cooldown), stalker->pos, Color(0, 255, 255), 20);
            if (stalker->weapon_cooldown == 0)
                Debug()->DebugSphereOut(stalker->pos, .7, Color(0, 255, 0));
            else
                Debug()->DebugSphereOut(stalker->pos, .7, Color(255, 0, 0));

            if (find(stalker->buffs.begin(), stalker->buffs.end(), BUFF_ID::LOCKON) != stalker->buffs.end())
            {
                const Unit* prism = Utility::GetLeastFullPrism(prisms);
                if (prism->cargo_space_max - prism->cargo_space_taken > 4)
                {
                    //Actions()->UnitCommand(prism, ABILITY_ID::LOAD_WARPPRISM, stalker, true);
                    Actions()->UnitCommand(stalker, ABILITY_ID::SMART, prism);
                }
                else if (blink_ready)
                {
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, Utility::PointBetween(stalker->pos, retreat_point, 4));
                }
                else
                {
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
                }
            }
            else if (stalker->shield == 0) // TODO pull this out?
            {
                if (Utility::DangerLevel(stalker, Observation()) > 0 && blink_ready)
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, Utility::PointBetween(stalker->pos, retreat_point, 4));
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
            }
            else if (!weapon_ready)
            {
                const Unit* prism = Utility::GetLeastFullPrism(prisms);
                if (prism->cargo_space_max - prism->cargo_space_taken > 6)
                {
                    //Actions()->UnitCommand(prism, ABILITY_ID::LOAD_WARPPRISM, stalker, true);
                    Actions()->UnitCommand(stalker, ABILITY_ID::SMART, prism);
                }
                if (Utility::DangerLevel(stalker, Observation()) > stalker->shield && blink_ready)
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, Utility::PointBetween(stalker->pos, retreat_point, 4));
                else if (close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, stalker->pos) - 2 < Utility::RealGroundRange(stalker, Utility::ClosestTo(close_enemies, stalker->pos)))
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attack_point);
            }
            else if (close_enemies.size() > 0)
            {
                if (!ob_in_position && Utility::IsOnHighGround(stalker->pos, Utility::ClosestTo(close_enemies, stalker->pos)->pos) || Utility::ClosestTo(close_enemies, stalker->pos)->display_type == Unit::DisplayType::Snapshot)
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, Utility::ClosestTo(close_enemies, stalker->pos));
            }
            else
            {
                Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attack_point);
            }
        }
    }

    void TossBot::ImmortalAttackTowards(Units immortals, Point2D retreat_point, Point2D attack_point, bool ob_in_position)
    {
		for (const auto &immortal : immortals)
		{
			Units close_enemies;
			for (const auto &unit : Observation()->GetUnits(Unit::Alliance::Enemy))
			{
				if (unit->is_building)
					continue;
				if (Distance2D(unit->pos, immortal->pos) < 10)
					close_enemies.push_back(unit);
			}
			bool weapon_ready = immortal->weapon_cooldown == 0;

			Debug()->DebugTextOut(std::to_string(immortal->weapon_cooldown), immortal->pos, Color(0, 255, 255), 20);
			if (immortal->weapon_cooldown == 0)
				Debug()->DebugSphereOut(immortal->pos, .7, Color(0, 255, 0));
			else
				Debug()->DebugSphereOut(immortal->pos, .7, Color(255, 0, 0));

			if (find(immortal->buffs.begin(), immortal->buffs.end(), BUFF_ID::LOCKON) != immortal->buffs.end())
			{
				Actions()->UnitCommand(immortal, ABILITY_ID::MOVE_MOVE, retreat_point);
			}
			else if (immortal->shield == 0)
			{
				Actions()->UnitCommand(immortal, ABILITY_ID::MOVE_MOVE, retreat_point);
			}
			else if (!weapon_ready)
			{
				if (close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, immortal->pos) - 2 < Utility::RealGroundRange(immortal, Utility::ClosestTo(close_enemies, immortal->pos)))
					Actions()->UnitCommand(immortal, ABILITY_ID::MOVE_MOVE, retreat_point);
				else
					Actions()->UnitCommand(immortal, ABILITY_ID::ATTACK, attack_point);
			}
			else if (close_enemies.size() > 0)
			{
				if (!ob_in_position && Utility::IsOnHighGround(immortal->pos, Utility::ClosestTo(close_enemies, immortal->pos)->pos) || Utility::ClosestTo(close_enemies, immortal->pos)->display_type == Unit::DisplayType::Snapshot)
					Actions()->UnitCommand(immortal, ABILITY_ID::MOVE_MOVE, retreat_point);
				else
					Actions()->UnitCommand(immortal, ABILITY_ID::ATTACK, Utility::ClosestTo(close_enemies, immortal->pos));
			}
			else
			{
				Actions()->UnitCommand(immortal, ABILITY_ID::ATTACK, attack_point);
			}
		}
    }

    void TossBot::ImmortalAttackTowardsWithPrism(Units stalkers, Units prisms, Point2D retreat_point, Point2D attack_point, bool ob_in_position)
    {

    }

	bool TossBot::FireVolley(Units units, std::vector<UNIT_TYPEID> prio)
	{
		std::map<const Unit*, const Unit*> attacks = FindTargets(units, prio, 0);
		if (attacks.size() == 0)
			return false;
		for (const auto &attack : attacks)
		{
			Actions()->UnitCommand(attack.first, ABILITY_ID::ATTACK, attack.second);
		}
		return true;
	}

	std::map<const Unit*, const Unit*> TossBot::FindTargets(Units units, std::vector<UNIT_TYPEID> prio, float extra_range)
	{
		std::ofstream find_targets;
		find_targets.open("find_targets.txt", std::ios_base::app);

		unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
		std::map<const Unit*, std::vector<const Unit*>> unit_targets;
		Units Eunits;
		for (const auto &unit : Observation()->GetUnits())
		{
			if (unit->alliance == Unit::Alliance::Enemy)
				Eunits.push_back(unit);
		}
		for (const auto &unit : units)
		{
			Units units_in_range;
			for (const auto &Eunit : Eunits)
			{
				if (Distance2D(unit->pos, Eunit->pos) <= Utility::RealGroundRange(unit, Eunit) + extra_range)
					units_in_range.push_back(Eunit);
			}
			unit_targets[unit] = units_in_range;
		}
		unsigned long long set_up_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count() - start_time;

		start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();

		FireControl* fire_control = new FireControl(this, unit_targets, prio);

		unsigned long long constructor_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count() - start_time;

		find_targets << set_up_time << ", ";
		find_targets << constructor_time << "\n";
		find_targets.close();

		return fire_control->FindAttacks();

	}

#pragma endregion
}
