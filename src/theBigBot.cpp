#include "theBigBot.h"
#include "mediator.h"
#include "finite_state_machine.h"
#include "pathfinding.h"
#include "locations.h"
#include "utility.h"
#include "build_order_manager.h"
#include "definitions.h"

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
#include <stdexcept>

#define _USE_MATH_DEFINES
#include <math.h>


namespace sc2 {


#pragma region Overrides

    void TheBigBot::OnGameStart()
    {
		SetUpUnitTypeInfo();

		//Debug()->SendDebug();
		if (debug_mode)
		{
			std::cout << "start location: " << Observation()->GetStartLocation().x << ' ' << Observation()->GetStartLocation().y << "\n";
			std::cout << "enemy start location: " << Observation()->GetGameInfo().enemy_start_locations[0].x << ' ' << Observation()->GetGameInfo().enemy_start_locations[0].y << "\n";
			Debug()->DebugGiveAllResources();
			Debug()->DebugGiveAllResources();
			Debug()->DebugFastBuild();
			Debug()->DebugCreateUnit(REAPER, Observation()->GetStartLocation(), 1, 1);
			Debug()->SendDebug();
		}
    }

    void TheBigBot::OnStep()
    {
		try
		{
			if (!started)
			{
				//PrintNonPathablePoints();

				mediator.SetUpManagers(debug_mode);

				Actions()->SendChat("glhf", ChatChannel::All);

				started = true;
			}
			if (debug_mode)
			{
				UpdateEffectPositions();

				UpdateEnemyWeaponCooldowns();

				UpdateEnemyWeaponCooldowns();

				//DisplayEnemyAttacks();

				DisplayEnemyPositions();

				//DisplayAlliedAttackStatus();

				RemoveCompletedAtacks();

				//mediator.RunManagers();

				Debug()->DebugSphereOut(ToPoint3D(locations->first_pylon_location_protoss), .5, Color(255, 0, 255));

				/*if (!started && Observation()->GetGameLoop() < 1800)
				{
					for (const auto& probe : Observation()->GetUnits(IsUnit(PROBE)))
					{
						std::cout << probe->pos.x << ", " << probe->pos.y << std::endl;
					}
					started = false;
				}*/

				for (auto& point : locations->pylon_locations)
				{
					Debug()->DebugSphereOut(ToPoint3D(point), 1, Color(255, 255, 255));
				}
				Debug()->DebugSphereOut(ToPoint3D(locations->first_pylon_location_protoss), 1, Color(255, 0, 0));
				Debug()->DebugSphereOut(ToPoint3D(locations->first_pylon_location_terran), 1, Color(255, 0, 0));
				Debug()->DebugSphereOut(ToPoint3D(locations->first_pylon_location_zerg), 1, Color(255, 0, 0));

				for (auto& point : locations->gateway_locations)
				{
					Debug()->DebugSphereOut(ToPoint3D(point), 1.5, Color(255, 0, 0));
				}

				for (auto& point : locations->tech_locations)
				{
					Debug()->DebugSphereOut(ToPoint3D(point), 1.5, Color(0, 255, 255));
				}

				for (auto& point : locations->cyber_core_locations)
				{
					Debug()->DebugSphereOut(ToPoint3D(point), 1.5, Color(255, 0, 255));
				}

				/*for (auto& point : locations->attack_path_line.GetPoints())
				{
					Debug()->DebugSphereOut(ToPoint3D(point), .5, Color(0, 255, 255));
				}*/

				/*for (const auto& point : locations->attack_path)
				{
					Debug()->DebugSphereOut(ToPoint3D(point), .5, Color(255, 255, 0));
					Debug()->DebugTextOut(std::to_string(point.x) + ", " + std::to_string(point.y), ToPoint3D(point), Color(255, 0, 0), 20);
				}

				for (const auto& point : locations->blink_nat_attack_path_line.GetPoints())
				{
					Debug()->DebugSphereOut(ToPoint3D(point), .25, Color(255, 0, 255));
				}*/
				/*for (const auto& point : locations->blink_third_attack_path_lines[0].GetPoints())
				{
					Debug()->DebugSphereOut(ToPoint3D(point), .25, Color(255, 0, 255));
				}
				for (const auto& point : locations->blink_third_attack_path_lines[1].GetPoints())
				{
					Debug()->DebugSphereOut(ToPoint3D(point), .25, Color(255, 0, 255));
				}

				Debug()->DebugSphereOut(ToPoint3D(locations->blink_presure_consolidation), .25, Color(255, 255, 255));*/
				
				/*for (const auto &point : locations->attack_path_short)
				{
					Debug()->DebugSphereOut(ToPoint3D(point), .5, Color(255, 255, 0));
					Debug()->DebugTextOut(std::to_string(point.x) + ", " + std::to_string(point.y), ToPoint3D(point), Color(255, 0, 0), 20);
				}

				for (const auto &point : locations->attack_path_short_line.GetPoints())
				{
					Debug()->DebugSphereOut(ToPoint3D(point), .25, Color(255, 0, 255));
				}*/

				/*std::vector<Point2D> points2;
				Point2D point = path.GetStartPoint();
				while (Distance2D(point, path.GetEndPoint()) > 1)
				{
					points2.push_back(point);
					point = path.GetPointFrom(point, 1, true);
				}

				for each (Point2D point in points2)
				{
					Debug()->DebugSphereOut(ToPoint3D(point), .5, Color(255, 0, 255));
				}*/

				Debug()->SendDebug();
				return;
			}



			Units bla = Observation()->GetUnits(IsFriendlyUnit(ADEPT));
			Units funits = Observation()->GetUnits(Unit::Alliance::Self);
			Units nunits = Observation()->GetUnits(Unit::Alliance::Neutral);
			Units eunits = Observation()->GetUnits(Unit::Alliance::Enemy);
			frames_passed++;
			//std::cout << std::to_string(Observation()->GetGameLoop()) << '\n';




			/*for (const auto& unit : Observation()->GetUnits(Unit::Alliance::Neutral))
			{
				Debug()->DebugSphereOut(unit->pos, .5, Color(255, 0, 0));
			}*/


			/*if (!debug_mode)
				ShowLocations();*/

			if ((float)Observation()->GetGameLoop() / FRAME_TIME > seconds_passed)
			{
				std::cerr << "Time: " << std::to_string(seconds_passed) << std::endl;
				seconds_passed++;
			}

			UpdateEffectPositions();

			UpdateEnemyWeaponCooldowns();

			UpdateEnemyWeaponCooldowns();

			RemoveCompletedAtacks();

			mediator.RunManagers();


#ifndef BUILD_FOR_LADDER

			//DisplayEnemyAttacks();

			DisplayEnemyPositions();

			DisplayKnownEffects();

			//DisplayAlliedAttackStatus();

			DisplayDebugHud();

			Debug()->SendDebug();

#endif // !BUILD_FOR_LADDER




		}
		catch (const std::exception& ex) {
			std::cerr << "OnStep Exception: " << ex.what() << std::endl;
			throw ex;
		}
		catch (...)
		{
			std::cerr << "OnStep Other exception" << std::endl;
			throw;
		}
    }

    void TheBigBot::OnBuildingConstructionComplete(const Unit *building)
    {
		try
		{
			mediator.OnBuildingConstructionComplete(building);
			if (debug_mode)
			{
				std::cout << UnitTypeToName(building->unit_type) << ' ' << building->pos.x << ", " << building->pos.y << '\n';
				//nav_mesh.AddNewObstacle(building);
				return;
			}
        
			if (building->unit_type == PYLON)
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
		catch (const std::exception& ex) {
			std::cerr << "OnBuildingConstructionComplete Exception: " << ex.what() << std::endl;
			throw ex;
		}
		catch (...)
		{
			std::cerr << "OnBuildingConstructionComplete Other exception" << std::endl;
			throw;
		}
    }

    void TheBigBot::OnNeutralUnitCreated(const Unit *building)
    {
		try
		{
			mediator.OnNeutralUnitCreated(building);
		}
		catch (const std::exception& ex) {
			std::cerr << "OnNeutralUnitCreated Exception: " << ex.what() << std::endl;
			throw ex;
		}
		catch (...)
		{
			std::cerr << "OnNeutralUnitCreated Other exception" << std::endl;
			throw;
		}
        /*if (debug_mode)
        {
            std::cout << UnitTypeToName(building->unit_type) << ' ' << building->pos.x << ", " << building->pos.y << '\n';
            return;
        }*/
    }

    void TheBigBot::OnUnitCreated(const Unit *unit)
    {
		try
		{
			if (!started)
				return;
			CallOnUnitCreatedEvent(unit);

			mediator.OnUnitCreated(unit);
		}
		catch (const std::exception& ex) {
			std::cerr << "OnUnitCreated Exception: " << ex.what() << std::endl;
			throw ex;
		}
		catch (...)
		{
			std::cerr << "OnUnitCreated Other exception" << std::endl;
			throw;
		}

        
        /*if len(self.adept_groups) > 0:
            if unit.type_id == UNIT_TYPEID.ADEPT :
                self.adept_groups[0].adept_ids.append(unit.tag)
                self.adept_groups[0].adept_order_status.append(False)
                print("adding " + str(unit.tag) + " <Adept> to army group 0")*/

    }

	void TheBigBot::OnUnitEnterVision(const Unit* unit)
	{
		try
		{
			if (Observation()->GetGameLoop() == 0)
				return;
			CallOnUnitEntersVisionEvent(unit);

			mediator.OnUnitEnterVision(unit);
		}
		catch (const std::exception& ex) {
			std::cerr << "OnUnitEnterVision Exception: " << ex.what() << std::endl;
			throw ex;
		}
		catch (...)
		{
			std::cerr << "OnUnitEnterVision Other exception" << std::endl;
			throw;
		}
	}

    void TheBigBot::OnUnitDamaged(const Unit *unit, float health_damage, float shield_damage)
    {
        //std::cout << UnitTypeToName(unit->unit_type.ToType()) << " took " << std::to_string(health_damage) << " damage\n";
		//if (unit->alliance == Unit::Alliance::Self)
	        //std::cout << unit->tag << " took " << std::to_string(health_damage + shield_damage) << " damage\n";
		try
		{
			CallOnUnitDamagedEvent(unit, health_damage, shield_damage);
			mediator.OnUnitDamaged(unit, health_damage, shield_damage);
		}
		catch (const std::exception& ex) {
			std::cerr << "OnUnitDamaged Exception: " << ex.what() << std::endl;
			throw ex;
		}
		catch (...)
		{
			std::cerr << "OnUnitDamaged Other exception" << std::endl;
			throw;
		}
    }

    void TheBigBot::OnUnitDestroyed(const Unit* unit)
    {
		if (unit == nullptr)
		{
			std::cerr << "nullptr passed into OnUnitDestroyed" << std::endl;
			mediator.LogMinorError();
			return;
		}
        //std::cout << UnitTypeToName(unit->unit_type.ToType()) << " destroyed\n";
		try
		{
			if (enemy_weapon_cooldown.find(unit) != enemy_weapon_cooldown.end())
				enemy_weapon_cooldown.erase(unit);
			//if (unit->alliance == Unit::Alliance::Enemy)
				//std::cout << unit->tag << " destroyed\n";
			CallOnUnitDestroyedEvent(unit);
			nav_mesh.RemoveObstacle(unit);

			mediator.OnUnitDestroyed(unit);
		}
		catch (const std::exception& ex) {
			std::cerr << "OnUnitDestroyed Exception: " << ex.what() << std::endl;
			throw ex;
		}
		catch (...)
		{
			std::cerr << "OnUnitDestroyed Other exception" << std::endl;
			throw;
		}
    }

	void TheBigBot::OnUpgradeCompleted(UpgradeID upgrade)
	{
		try
		{
			mediator.OnUpgradeCompleted(upgrade);
		}
		catch (const std::exception& ex) {
			std::cerr << "OnUpgradeCompleted Exception: " << ex.what() << std::endl;
			throw ex;
		}
		catch (...)
		{
			std::cerr << "OnUpgradeCompleted Other exception" << std::endl;
			throw;
		}
	}


#pragma endregion

#pragma region testing

	void TheBigBot::RunInitialSetUp()
	{
		//Debug()->DebugFastBuild();
		//Debug()->DebugGiveAllResources();
		//Debug()->DebugShowMap();
		//Debug()->DebugGiveAllUpgrades();
		//SpawnArmies();
		initial_set_up = true;
	}

	void TheBigBot::RunTests()
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
			//Actions()->UnitCommand(unit.first, A_MOVE, unit.second);
		}
		Actions()->UnitCommand(test_army.prisms[0], A_MOVE, Utility::PointBetween(Utility::MedianCenter(test_army.stalkers), fallback_point, 3));
		Actions()->UnitCommand(test_army.prisms[0], A_UNLOAD_AT, test_army.prisms[0]);
		ApplyPressureGrouped(&test_army, enemy_army_spawn, fallback_point, retreating_unit_positions, attacking_unit_positions);

		
		
		Units enemy_attacking_units = Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
		//Actions()->UnitCommand(enemy_attacking_units, A_ATTACK, fallback_point);*/
	}

	void TheBigBot::SpawnArmies()
	{
		//Debug()->DebugEnemyControl();
		//Debug()->DebugCreateUnit(ROACH, locations->attack_path[locations->attack_path.size() - 1], 2, 10);
		//Debug()->DebugCreateUnit(RAVAGER, enemy_army_spawn, 2, 5);
		//Debug()->DebugCreateUnit(ZERGLING, locations->attack_path[locations->attack_path.size() - 1], 2, 32);

		//Debug()->DebugCreateUnit(MARINE, enemy_army_spawn, 1, 8);
		//Debug()->DebugCreateUnit(MARAUDER, enemy_army_spawn, 1, 1);
		//Debug()->DebugCreateUnit(SIEGE_TANK_SIEGED, enemy_army_spawn, 1, 1);

		//Debug()->DebugCreateUnit(STALKER, enemy_army_spawn, 2, 8);

		//Debug()->DebugCreateUnit(ADEPT, locations->attack_path[0], 1, 1);
		//Debug()->DebugCreateUnit(STALKER, locations->attack_path[0], 2, 12);
		//Debug()->DebugCreateUnit(PRISM, locations->attack_path[0], 2, 1);
		//Debug()->DebugCreateUnit(ORACLE, locations->attack_path[0], 1, 3);
		//Debug()->SendDebug();
	}

	void TheBigBot::SetUpArmies()
	{
		/*OracleHarassStateMachine* state_machine = new OracleHarassStateMachine(this, Observation()->GetUnits(IsUnit(ORACLE)), "Oracles");
		active_FSMs.push_back(state_machine);
		BuildOrderResultArgData data = BuildOrderResultArgData();
		build_order_manager.StalkerOraclePressure(data);*/
		tests_set_up = true;
	}



#pragma endregion

#pragma region Utility



	Point2D TheBigBot::GetNaturalDefensiveLocation(UNIT_TYPEID type)
	{
		std::vector<Point2D> possible_locations;

		switch (type)
		{
		case BATTERY:
			possible_locations = locations->defensive_natural_battery_locations;
			break;
		default:
			std::cerr << "Unknown unit type in GetNaturalDefensiveLocation: " << UnitTypeToName(type) << std::endl;
			break;
		}

		for (const auto& point : possible_locations)
		{
			bool blocked = false;
			bool in_energy_field = (type == PYLON || type == ASSIMILATOR || type == NEXUS);
			for (const auto& building : Observation()->GetUnits(IsBuilding()))
			{
				if (Point2D(building->pos) == point)
				{
					blocked = true;
					break;
				}
				if (!in_energy_field && building->unit_type == PYLON)
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
		return Point2D(0, 0);

	}

    std::vector<Point2D> TheBigBot::GetProxyLocations(UNIT_TYPEID type)
    {
        switch (type)
        {
        case PYLON:
            return locations->proxy_pylon_locations;
            break;
        case GATEWAY:
            return locations->proxy_gateway_locations;
            break;
		case ROBO:
		case STARGATE:
			return locations->proxy_tech_locations;
        default:
            //std::cout << "Error invalid type id in GetProxyLocations" << std::endl;
            return std::vector<Point2D>();
        }
    }

    std::string TheBigBot::OrdersToString(std::vector<UnitOrder> orders)
    {
        std::string text = "";
        for (const auto &order : orders)
        {
            text += Utility::AbilityIdToString(order.ability_id);
            int percent = (int)std::floor(order.progress * 10);
            std::string completed(percent, '|');
            std::string todo(10 - percent, '-');
            text += " <" + completed + todo + "> ";
        }
        return text;
    }

    std::vector<Point2D> TheBigBot::FindWarpInSpots(Point2D close_to, int num)
    {
		// order pylons first
#ifdef DEBUG_TIMING
		unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();

		unsigned long long initial_check = 0;
		unsigned long long blocked_check = 0;
		unsigned long long spot_check = 0;
#endif


        std::vector<Point2D> spots;
        for (const auto &pylon : Observation()->GetUnits(IsFinishedUnit(PYLON)))
        {
			// ignore slow warpins
			if (!Utility::AnyUnitWithin(Observation()->GetUnits(IsUnits({ NEXUS, WARP_GATE })), pylon->pos, 6.5))
				continue;
			if (Utility::AnyUnitWithin(Observation()->GetUnits(Unit::Alliance::Enemy), pylon->pos, 6))
				continue;
            for (int i = -7; i <= 6; i += 1)
            {
                for (int j = -7; j <= 6; j += 1)
                {
#ifdef DEBUG_TIMING
					unsigned long long check_start = std::chrono::duration_cast<std::chrono::microseconds>(
						std::chrono::high_resolution_clock::now().time_since_epoch()
						).count();
#endif

                    Point2D pos = Point2D(pylon->pos.x + i + .5f, pylon->pos.y + j + .5f);
                    if (Observation()->IsPathable(pos) && Distance2D(pos, pylon->pos) <= 6 && ToPoint3D(pos).y > pylon->pos.y + .5)
                    {
#ifdef DEBUG_TIMING
						unsigned long long initial = std::chrono::duration_cast<std::chrono::microseconds>(
							std::chrono::high_resolution_clock::now().time_since_epoch()
							).count();
#endif
                        bool blocked = false;
						for (const auto& avoid_spot : locations->bad_warpin_spots)
						{
							if (Distance2D(pos, avoid_spot) < 3)
							{
								blocked = true;
								break;
							}
						}
                        for (const auto &building : Observation()->GetUnits(IsBuilding()))
                        {
                            if (building->unit_type == PYLON || building->unit_type == CANNON || building->unit_type == BATTERY)
                            {
                                if (building->pos.x - .5 <= pos.x && building->pos.x + .5 >= pos.x && building->pos.y - .5 <= pos.y && building->pos.y + .5 >= pos.y)
                                {
                                    blocked = true;
                                    break;
                                }
                            }
                            else if (building->unit_type == NEXUS)
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
#ifdef DEBUG_TIMING
						unsigned long long block = std::chrono::duration_cast<std::chrono::microseconds>(
							std::chrono::high_resolution_clock::now().time_since_epoch()
							).count();
#endif
						for (const auto &spot : spots)
						{
							if (Distance2D(pos, spot) < 1.5)
							{
								blocked = true;
								break;
							}
						}
                        if (!blocked && !Utility::AnyUnitWithin(Observation()->GetUnits(Unit::Alliance::Self), pos, 1.5) && !Utility::AnyUnitWithin(Observation()->GetUnits(Unit::Alliance::Neutral), pos, 1.5))
                            spots.push_back(pos);

#ifdef DEBUG_TIMING
						unsigned long long spot = std::chrono::duration_cast<std::chrono::microseconds>(
							std::chrono::high_resolution_clock::now().time_since_epoch()
							).count();

						initial_check += initial - check_start;
						blocked_check += block - initial;
						spot_check += spot - block;
#endif
                    }
					else
					{
#ifdef DEBUG_TIMING
						unsigned long long initial = std::chrono::duration_cast<std::chrono::microseconds>(
							std::chrono::high_resolution_clock::now().time_since_epoch()
							).count();

						initial_check += initial - check_start;
#endif
					}
					if (spots.size() >= num)
						break;
                }
				if (spots.size() >= num)
					break;
            }
        }

#ifdef DEBUG_TIMING
		unsigned long long end_find = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
#endif
		std::vector<Point2D> prism_spots;

        for (const auto &prism : Observation()->GetUnits(IsFinishedUnit(PRISM_SIEGED)))
        {
            for (int i = -4; i <= 4; i += 1)
            {
                for (int j = -4; j <= 4; j += 1)
                {
                    Point2D pos = Point2D(prism->pos.x + i + .5f, prism->pos.y + j + .5f);
                    if (Observation()->IsPathable(pos) && Distance2D(pos, prism->pos) <= 3.75 && Utility::DistanceToClosest(Observation()->GetUnits(), pos) > 1)
                    {
                        prism_spots.push_back(pos);
                    }
                }
            }
        }

#ifdef DEBUG_TIMING
		unsigned long long prism_spots = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
#endif

        sort(spots.begin(), spots.end(),
            [close_to](const Point2D & a, const Point2D & b) -> bool
        {
            return Distance2D(a, close_to) < Distance2D(b, close_to);
        });

		spots.insert(spots.begin(), prism_spots.begin(), prism_spots.end());

#ifdef DEBUG_TIMING
		unsigned long long sort_spots = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();

		std::ofstream wind_spots;
		wind_spots.open("wind_spots.txt", std::ios_base::app);

		wind_spots << end_find - start_time << ", ";
		wind_spots << initial_check << ", ";
		wind_spots << blocked_check << ", ";
		wind_spots << spot_check << ", ";
		wind_spots << prism_spots - end_find << ", ";
		wind_spots << sort_spots - prism_spots << "\n";
		wind_spots.close();
#endif

        return spots;
    }

    std::vector<Point2D> TheBigBot::FindProxyWarpInSpots()
    {
        std::vector<Point2D> spots;
        for (const auto &tag : proxy_pylons)
        {
			const Unit* pylon = Observation()->GetUnit(tag);
			if (pylon == nullptr || pylon->is_alive == false)
				continue;
            Point2D pylon_pos = Observation()->GetUnit(tag)->pos;
            for (int i = -6; i <= 6; i += 2)
            {
                for (int j = -6; j <= 6; j += 2)
                {
                    Point2D pos = Point2D(pylon_pos.x + i + .5f, pylon_pos.y + j + .5f);
                    if (Observation()->IsPathable(pos) && Distance2D(pos, pylon_pos) <= 6 && Utility::DistanceToClosest(Observation()->GetUnits(), pos) > 1)
                        spots.push_back(pos);
                }
            }
        }
		if (spots.size() == 0)
		{
			spots = FindWarpInSpots(Observation()->GetGameInfo().enemy_start_locations[0], 5);
		}

		std::vector<Point2D> prism_spots;

		for (const auto& prism : Observation()->GetUnits(IsFinishedUnit(PRISM_SIEGED)))
		{
			for (int i = -4; i <= 4; i += 1)
			{
				for (int j = -4; j <= 4; j += 1)
				{
					Point2D pos = Point2D(prism->pos.x + i + .5f, prism->pos.y + j + .5f);
					if (Observation()->IsPathable(pos) && Distance2D(pos, prism->pos) <= 3.75 && Utility::DistanceToClosest(Observation()->GetUnits(), pos) > 1)
					{
						prism_spots.push_back(pos);
					}
				}
			}
		}

		spots.insert(spots.begin(), prism_spots.begin(), prism_spots.end());

        return spots;
    }

#pragma endregion

#pragma region Events

    void TheBigBot::AddListenerToOnUnitDamagedEvent(int id, std::function<void(const Unit*, float, float)> func)
    {
        on_unit_damaged_event.listeners[id] = func;
    }

	void TheBigBot::RemoveListenerToOnUnitDamagedEvent(int id)
	{
		on_unit_damaged_event.listeners.erase(id);
	}

    void TheBigBot::CallOnUnitDamagedEvent(const Unit* unit, float health, float shields)
    {
        for (const auto &func : on_unit_damaged_event.listeners)
        {
            func.second(unit, health, shields);
        }
    }

    void TheBigBot::AddListenerToOnUnitDestroyedEvent(int id, std::function<void(const Unit*)> func)
    {
        on_unit_destroyed_event.listeners[id] = func;
    }

	void TheBigBot::RemoveListenerToOnUnitDestroyedEvent(int id)
	{
		on_unit_destroyed_event.listeners.erase(id);
	}

    void TheBigBot::CallOnUnitDestroyedEvent(const Unit* unit)
    {
        for (const auto &listener : on_unit_destroyed_event.listeners)
        {
			auto func = listener.second;
            func(unit);
        }
    }

	void TheBigBot::AddListenerToOnUnitCreatedEvent(int id, std::function<void(const Unit*)> func)
	{
		on_unit_created_event.listeners[id] = func;
	}

	void TheBigBot::RemoveListenerToOnUnitCreatedEvent(int id)
	{
		on_unit_created_event.listeners.erase(id);
	}

	void TheBigBot::CallOnUnitCreatedEvent(const Unit* unit)
	{
		for (const auto &listener : on_unit_created_event.listeners)
		{
			auto func = listener.second;
			func(unit);
		}
	}

	void TheBigBot::AddListenerToOnUnitEntersVisionEvent(int id, std::function<void(const Unit*)> func)
	{
		on_unit_enters_vision_event.listeners[id] = func;
	}

	void TheBigBot::RemoveListenerToOnUnitEntersVisionEvent(int id)
	{
		on_unit_enters_vision_event.listeners.erase(id);
	}

	void TheBigBot::CallOnUnitEntersVisionEvent(const Unit* unit)
	{
		for (const auto& listener : on_unit_enters_vision_event.listeners)
		{
			auto func = listener.second;
			func(unit);
		}
	}

#pragma endregion


	int TheBigBot::IncomingDamage(const Unit* unit)
	{
		int damage = 0;
		if (enemy_attacks.count(unit) > 0)
		{
			for (const auto &attack : enemy_attacks[unit])
			{
				damage += Utility::GetDamage(attack.unit, unit);
			}
		}
		for (int i = 0; i < corrosive_bile_times.size(); i++)
		{
			if (Observation()->GetGameLoop() + 5 > corrosive_bile_times[i])
			{
				if (Distance2D(corrosive_bile_positions[i], unit->pos) < .5 + unit->radius + .3) // TODO maybe change extra distance
				{
					damage += 60;
				}
			}
			else
			{
				break;
			}
		}
		for (const auto& enemy : Utility::GetUnitsThatCanAttack(Observation()->GetUnits(Unit::Alliance::Enemy, IsUnits(HIT_SCAN_UNIT_TYPES)), unit, 0))
		{
			damage += Utility::GetDamage(enemy, unit) / 2;
		}
		return damage;
	}

	

	void TheBigBot::UpdateEnemyWeaponCooldowns()
	{
#ifdef DEBUG_TIMING
		unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();

		std::ofstream update_weapon_cd_file;
		update_weapon_cd_file.open("update_weapon_cd_time.txt", std::ios_base::app);

		unsigned long long get_allied = 0;
		unsigned long long get_enemy = 0;
		unsigned long long melee_units = 0;
		unsigned long long count = 0;
		unsigned long long aiming_at = 0;
		unsigned long long add_attack = 0;
		unsigned long long total_time = 0;

		unsigned long long melee_units_total = 0;
		unsigned long long count_total = 0;
		unsigned long long aiming_at_total = 0;
		unsigned long long add_attack_total = 0;
#endif

		Units allied_units = Observation()->GetUnits(Unit::Alliance::Self);

#ifdef DEBUG_TIMING
		get_allied = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
#endif

		Units enemy_attacking_units = Observation()->GetUnits(Unit::Alliance::Enemy, IsUnits(PROJECTILE_UNIT_TYPES));

#ifdef DEBUG_TIMING
		get_enemy = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
#endif


		int loop = Observation()->GetGameLoop();

		for (const auto &Eunit : enemy_attacking_units)
		{
#ifdef DEBUG_TIMING
			unsigned long long unit_start = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				).count();
#endif

			if (!unit_type_info[Eunit->unit_type.ToType()].is_army_unit)
				continue;

			// melee are assumed to always attack whats next to them
			if (unit_type_info[Eunit->unit_type.ToType()].is_melee)
			{
				for (const auto &Funit : allied_units)
				{
					if (Distance2D(Eunit->pos, Funit->pos) < Utility::RealRange(Eunit, Funit)/* && Utility::IsFacing(Eunit, Funit)*/)
					{
						EnemyAttack attack = EnemyAttack(Eunit, loop);
						if (enemy_attacks.count(Funit) == 0)
							enemy_attacks[Funit] = { attack };
						else
							enemy_attacks[Funit].push_back(attack);
					}
				}
				continue;
			}

#ifdef DEBUG_TIMING
			melee_units = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				).count();
#endif

			if (enemy_weapon_cooldown.count(Eunit) == 0)
				enemy_weapon_cooldown[Eunit] = 0;

#ifdef DEBUG_TIMING
			count = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				).count();
#endif

			const Unit* target = Utility::AimingAt(Eunit, allied_units);

#ifdef DEBUG_TIMING
			aiming_at = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				).count();
#endif

			if (target != NULL && enemy_weapon_cooldown[Eunit] == 0 && mediator.scouting_manager.enemy_unit_saved_position[Eunit].frames > Utility::GetDamagePoint(Eunit) * 22.4)
			{
				float damage_point = Utility::GetDamagePoint(Eunit);
				if (damage_point == 0)
				{
					enemy_weapon_cooldown[Eunit] = Utility::GetWeaponCooldown(Eunit) - damage_point - (1 / FRAME_TIME);
					EnemyAttack attack = EnemyAttack(Eunit, Observation()->GetGameLoop() + (int)std::floor(Utility::GetProjectileTime(Eunit, Distance2D(Eunit->pos, target->pos) - Eunit->radius - target->radius) - 1));
					if (enemy_attacks.count(target) == 0)
						enemy_attacks[target] = { attack };
					else
						enemy_attacks[target].push_back(attack);
				}
				else
				{
					enemy_weapon_cooldown[Eunit] = Utility::GetWeaponCooldown(Eunit);
					EnemyAttack attack = EnemyAttack(Eunit, Observation()->GetGameLoop() + (int)std::floor(Utility::GetProjectileTime(Eunit, Distance2D(Eunit->pos, target->pos) - Eunit->radius - target->radius)));
					if (enemy_attacks.count(target) == 0)
						enemy_attacks[target] = { attack };
					else
						enemy_attacks[target].push_back(attack);
				}
			}

#ifdef DEBUG_TIMING
			add_attack = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				).count();

			melee_units_total += melee_units - unit_start;
			count_total += count - melee_units;
			aiming_at_total += aiming_at - count;
			add_attack_total += add_attack - aiming_at;
#endif

			if (enemy_weapon_cooldown[Eunit] > 0)
				enemy_weapon_cooldown[Eunit] -= 1 / FRAME_TIME;
			if (enemy_weapon_cooldown[Eunit] < 0)
			{
				enemy_weapon_cooldown[Eunit] = 0;
				mediator.scouting_manager.enemy_unit_saved_position[Eunit].frames = -1;
			}
			//Debug()->DebugTextOut(std::to_string(enemy_weapon_cooldown[Eunit]), Eunit->pos + Point3D(0, 0, .2), Color(255, 0, 255), 20);

		}

#ifdef DEBUG_TIMING
		total_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();

		update_weapon_cd_file << get_allied - start_time << ", ";
		update_weapon_cd_file << get_enemy - get_allied << ", ";
		update_weapon_cd_file << melee_units_total << ", ";
		update_weapon_cd_file << count_total << ", ";
		update_weapon_cd_file << aiming_at_total << ", ";
		update_weapon_cd_file << add_attack_total << ", ";
		update_weapon_cd_file << total_time - start_time << "\n";
		update_weapon_cd_file.close();
#endif
	}

	void TheBigBot::RemoveCompletedAtacks()
	{
		for (auto &attack : enemy_attacks)
		{
			for (int i = (int)attack.second.size() - 1; i >= 0; i--)
			{
				if (attack.second[i].impact_frame <= Observation()->GetGameLoop())
				{
					attack.second.erase(attack.second.begin() + i);
				}
			}
		}
	}


	void TheBigBot::SetUpUnitTypeInfo()
	{
		unit_type_info[PROBE] =					UnitTypeInfo(true, false, true, false); // protoss
		unit_type_info[ZEALOT] =					UnitTypeInfo(true, true, true, false);
		unit_type_info[STALKER] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[SENTRY] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[ADEPT] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[HIGH_TEMPLAR] =				UnitTypeInfo(false, true, true, false);
		unit_type_info[DARK_TEMPLAR] =				UnitTypeInfo(true, true, true, false);
		unit_type_info[IMMORTAL] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[COLOSSUS] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[DISRUPTOR] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[ARCHON] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[OBSERVER] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[PRISM] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[PRISM_SIEGED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[PHOENIX] =					UnitTypeInfo(false, true, false, true);
		unit_type_info[VOID_RAY] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[ORACLE] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[CARRIER] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[TEMPEST] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[MOTHERSHIP] =				UnitTypeInfo(false, true, true, true);
		unit_type_info[NEXUS] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[PYLON] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[ASSIMILATOR] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[ASSIMILATOR_RICH] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[GATEWAY] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[WARP_GATE] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[FORGE] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[CYBERCORE] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[CANNON] =				UnitTypeInfo(false, false, true, true);
		unit_type_info[BATTERY] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[ROBO] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[STARGATE] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[TWILIGHT] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[ROBO_BAY] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[FLEET_BEACON] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[TEMPLAR_ARCHIVE] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[DARK_SHRINE] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[SCV] =						UnitTypeInfo(true, false, true, false); // terran
		unit_type_info[MULE] =						UnitTypeInfo(false, false, false, false);
		unit_type_info[MARINE] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[MARAUDER] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[REAPER] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[GHOST] =						UnitTypeInfo(false, true, true, true);
		unit_type_info[HELLION] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[HELLBAT] =				UnitTypeInfo(false, true, true, false);
		unit_type_info[SIEGE_TANK] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[SIEGE_TANK_SIEGED] =			UnitTypeInfo(false, true, true, false);
		unit_type_info[CYCLONE] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[WIDOW_MINE] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[WIDOW_MINE_BURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[THOR_AOE] =						UnitTypeInfo(false, true, true, true);
		unit_type_info[THOR_AP] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[VIKING_LANDED] =				UnitTypeInfo(false, true, true, false);
		unit_type_info[VIKING] = 			UnitTypeInfo(false, true, false, true);
		unit_type_info[MEDIVAC] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[LIBERATOR] =					UnitTypeInfo(false, true, false, true);
		unit_type_info[LIBERATOR_SIEGED] =				UnitTypeInfo(false, true, true, false);
		unit_type_info[RAVEN] =						UnitTypeInfo(false, true, false, false);
		unit_type_info[BANSHEE] =					UnitTypeInfo(false, true, false, true);
		unit_type_info[BATTLECRUISER] =				UnitTypeInfo(false, true, true, true);
		unit_type_info[AUTO_TURRET] =				UnitTypeInfo(false, false, true, true);
		unit_type_info[COMMAND_CENTER] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[COMMAND_CENTER_FLYING] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[ORBITAL] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[ORBITAL_FLYING] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[PLANETARY] =			UnitTypeInfo(false, false, true, false);
		unit_type_info[SUPPLY_DEPOT] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[SUPPLY_DEPOT_LOWERED] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[REFINERY] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[REFINERY_RICH] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[BARRACKS] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[BARRACKS_FLYING] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[BARRACKS_REACTOR] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[BARRACKS_TECH_LAB] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[ENGINEERING_BAY] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[BUNKER] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[NEOSTEEL_BUNKER] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[SENSOR_TOWER] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[MISSILE_TURRET] =				UnitTypeInfo(false, false, false, true);
		unit_type_info[FACTORY] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[FACTORY_FLYING] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[FACTORY_REACTOR] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[FACTORY_TECH_LAB] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[GHOST_ACADEMY] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[STARPORT] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[STARPORT_FLYING] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[STARPORT_REACTOR] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[STARPORT_TECH_LAB] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[ARMORY] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[FUSION_CORE] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[LARVA] =						UnitTypeInfo(false, false, false, false); // zerg
		unit_type_info[BANELING_EGG] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[BROOD_LORD_EGG] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[OVERSEER_EGG] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[RAVAGER_EGG] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[DROPPERLORD_EGG] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[DRONE] =						UnitTypeInfo(true, false, true, false);
		unit_type_info[DRONE_BURROWED] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[QUEEN] =						UnitTypeInfo(false, true, true, true);
		unit_type_info[QUEEN_BURROWED] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[ZERGLING] =					UnitTypeInfo(true, true, true, false);
		unit_type_info[ZERGLING_BURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[BANELING] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[BANELING_BURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[ROACH] =						UnitTypeInfo(false, true, true, false);
		unit_type_info[ROACH_BURROWED] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[RAVAGER] =						UnitTypeInfo(false, true, true, false);
		unit_type_info[RAVAGER_BURROWED] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[HYDRA] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[HYDRA_BURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[LURKER] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[LURKER_BURROWED] =			UnitTypeInfo(false, true, true, false);
		unit_type_info[LURKER_EGG] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[INFESTOR] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[INFESTOR_BURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[SWARMHOST] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[SWARMHOST_BURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[ULTRALISK] =					UnitTypeInfo(true, true, true, false);
		unit_type_info[ULTRALISK_BURROWED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[OVERLORD] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[DROPPERLORD] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[OVERSEER] =					UnitTypeInfo(false, true, false, false);
		unit_type_info[OVERSEER_SIEGED] =			UnitTypeInfo(false, true, false, false);
		unit_type_info[MUTALISK] =					UnitTypeInfo(false, true, true, true);
		unit_type_info[CORRUPTER] =					UnitTypeInfo(false, true, false, true);
		unit_type_info[BROOD_LORD] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[VIPER] =						UnitTypeInfo(false, true, false, false);
		unit_type_info[LOCUST] =					UnitTypeInfo(false, true, true, false);
		unit_type_info[LOCUST_FLYING] =				UnitTypeInfo(false, true, false, false);
		unit_type_info[BROODLING] =					UnitTypeInfo(true, true, true, false);
		unit_type_info[CHANGELING] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[CHANGELING_ZEALOT] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[HATCHERY] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[LAIR] =						UnitTypeInfo(false, false, false, false);
		unit_type_info[HIVE] =						UnitTypeInfo(false, false, false, false);
		unit_type_info[SPINE_CRAWLER] =				UnitTypeInfo(false, false, true, false);
		unit_type_info[SPINE_CRAWLER_UPROOTED] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[SPORE_CRAWLER] =				UnitTypeInfo(false, false, false, true);
		unit_type_info[SPORE_CRAWLER_UPROOTED] =		UnitTypeInfo(false, false, false, false);
		unit_type_info[EXTRACTOR] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[EXTRACTOR_RICH] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[SPAWNING_POOL] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[EVO_CHAMBER] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[ROACH_WARREN] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[BANELING_NEST] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[HYDRA_DEN] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[LURKER_DEN] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[INFESTATION_PIT] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[SPIRE] =						UnitTypeInfo(false, false, false, false);
		unit_type_info[GREATER_SPIRE] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[ULTRALISK_CAVERN] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[NYDUS_WORM] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[NYDUS] =				UnitTypeInfo(false, false, false, false);
		unit_type_info[CREEP_TUMOR_1] =					UnitTypeInfo(false, false, false, false);
		unit_type_info[CREEP_TUMOR_2] =			UnitTypeInfo(false, false, false, false);
		unit_type_info[CREEP_TUMOR_3] =				UnitTypeInfo(false, false, false, false);
	}

	void TheBigBot::PrintAttacks(std::map<const Unit*, const Unit*> attacks)
	{
		//std::cout << "Volley \n";
		/*for (const auto &attack : attacks)
		{
			std::cout << "    attack from " << std::to_string(attack.first->tag) << " to " << std::to_string(attack.second->tag) << " health " << std::to_string(attack.second->health) << "\n";
		}*/
	}

	Point3D TheBigBot::ToPoint3D(Point2D point)
	{
		float height = Observation()->TerrainHeight(point);
		return Point3D(point.x, point.y, height);
	}

	int TheBigBot::GetUniqueId()
	{
		current_unique_id++;
		return current_unique_id;
	}

	void TheBigBot::UpdateEffectPositions()
	{
		for (auto& zone : liberator_zone_current)
		{
			zone.current = false;
		}

		for (const auto& effect : Observation()->GetEffects())
		{
			if (effect.effect_id == 11) // EFFECT_CORROSIVEBILE
			{
				for (const auto& pos : effect.positions)
				{
					if (std::find(corrosive_bile_positions.begin(), corrosive_bile_positions.end(), pos) == corrosive_bile_positions.end())
					{
						corrosive_bile_positions.push_back(pos);
						corrosive_bile_times.push_back(Observation()->GetGameLoop() + 48);
					}
				}
			}
			if (effect.effect_id == 9 || effect.effect_id == 8) // LIBERATORDEFENDERZONE || LIBERATORDEFENDERZONESETUP
			{
				for (const auto& pos : effect.positions)
				{
					auto itr = std::find_if(liberator_zone_current.begin(), liberator_zone_current.end(), [pos](const LiberatorZone& zone) { return zone.pos == pos; });

					if (itr == liberator_zone_current.end())
					{
						liberator_zone_current.push_back(LiberatorZone(pos));
					}
					else
					{
						itr->current = true;
					}
				}
			}
		}
		for (int i = 0; i < corrosive_bile_positions.size(); i++)
		{
			if (Observation()->GetGameLoop() > corrosive_bile_times[i])
			{
				corrosive_bile_positions.erase(corrosive_bile_positions.begin() + i);
				corrosive_bile_times.erase(corrosive_bile_times.begin() + i);
			}
		}
		/*for (int i = 0; i < corrosive_bile_positions.size(); i++)
		{
			Debug()->DebugSphereOut(ToPoint3D(corrosive_bile_positions[i]), .5, Color(255, 0, 255));
			Debug()->DebugTextOut(std::to_string(corrosive_bile_times[i]), ToPoint3D(corrosive_bile_positions[i]), Color(255, 0, 255), 14);
		}*/
		

		for (int k = 0; k < liberator_zone_current.size(); k++)
		{
			if (liberator_zone_current[k].current == false && mediator.IsVisible(liberator_zone_current[k].pos))
			{
				liberator_zone_current.erase(std::remove(liberator_zone_current.begin(), liberator_zone_current.end(), liberator_zone_current[k]), liberator_zone_current.end());
				k--;
			}
		}

	}


	Polygon TheBigBot::CreateNewBlocker(const Unit* unit)
	{
		Polygon polygon;
		switch (unit->unit_type.ToType())
		{
		case PYLON:
			polygon.points.push_back(unit->pos + Point2D(3, 3));
			polygon.points.push_back(unit->pos + Point2D(3, -3));
			polygon.points.push_back(unit->pos + Point2D(-3, -3));
			polygon.points.push_back(unit->pos + Point2D(-3, 3));
			break;
		case MISSILE_TURRET:
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


#pragma region debug info

    void TheBigBot::DisplayDebugHud()
    {
#ifdef DEBUG_TIMING
		std::chrono::microseconds start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
#endif
        DisplayWorkerStatus();
#ifdef DEBUG_TIMING
		std::chrono::microseconds worker_status = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
#endif
        DisplayBuildOrder();
#ifdef DEBUG_TIMING
		std::chrono::microseconds build_order = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
#endif
        DisplayActiveActions();
#ifdef DEBUG_TIMING
		std::chrono::microseconds active_actions = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
#endif
        DisplayActiveStateMachines();
#ifdef DEBUG_TIMING
		std::chrono::microseconds active_fsm = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
#endif
        DisplayBuildingStatuses();
#ifdef DEBUG_TIMING
		std::chrono::microseconds building_status = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
#endif
        DisplayArmyGroups();
#ifdef DEBUG_TIMING
		std::chrono::microseconds army_groups = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			);
#endif
        DisplaySupplyInfo();

		DisplayOngoingAttacks();
#ifdef DEBUG_TIMING
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
#endif
    }

    void TheBigBot::DisplayWorkerStatus()
    {
        Debug()->DebugTextOut("first 2 spaces: " + std::to_string(mediator.worker_manager.first_2_mineral_patch_spaces.size()), Point2D(0, 0), Color(0, 255, 255), 20);
        if (mediator.worker_manager.close_3_mineral_patch_extras.size() > 0)
            Debug()->DebugTextOut("\nclose 3rd extras: " + std::to_string(mediator.worker_manager.close_3_mineral_patch_extras.size()), Point2D(0, 0), Color(255, 0, 255), 20);
        else
            Debug()->DebugTextOut("\nclose 3rd spaces: " + std::to_string(mediator.worker_manager.close_3_mineral_patch_spaces.size()), Point2D(0, 0), Color(0, 255, 255), 20);
        if (mediator.worker_manager.far_3_mineral_patch_extras.size() > 0)
            Debug()->DebugTextOut("\n\nfar 3rd extras: " + std::to_string(mediator.worker_manager.far_3_mineral_patch_extras.size()), Point2D(0, 0), Color(255, 0, 255), 20);
        else
            Debug()->DebugTextOut("\n\nfar 3rd spaces: " + std::to_string(mediator.worker_manager.far_3_mineral_patch_spaces.size()), Point2D(0, 0), Color(0, 255, 255), 20);



        std::string close_patches = "\n\n\n";
        std::string far_patches = "\n\n\n";
        std::string gasses = "\n\n\n";
        for (const auto &data : mediator.worker_manager.mineral_patches)
        {
            if (data.second.is_close)
            {
                far_patches += "\n";
                gasses += "\n";
                close_patches += "close:";
                if (data.second.workers[0] != nullptr)
                    close_patches += " X";
                if (data.second.workers[1] != nullptr)
                    close_patches += " X";
                if (data.second.workers[2] != nullptr)
                    close_patches += " X";
                close_patches += "\n";
            }
        }
        for (const auto &data : mediator.worker_manager.mineral_patches)
        {
            if (!data.second.is_close)
            {
                gasses += "\n";
                far_patches += "far:";
                if (data.second.workers[0] != nullptr)
                    far_patches += " X";
                if (data.second.workers[1] != nullptr)
                    far_patches += " X";
                if (data.second.workers[2] != nullptr)
                    far_patches += " X";
                far_patches += "\n";
            }
        }
        for (const auto &data : mediator.worker_manager.assimilators)
        {
            gasses += "gas:";
            if (data.second.workers[0] != nullptr)
                gasses += " X";
            if (data.second.workers[1] != nullptr)
                gasses += " X";
            if (data.second.workers[2] != nullptr)
                gasses += " X";
            gasses += "\n";
        }

        Debug()->DebugTextOut(close_patches, Point2D(0, 0), Color(255, 255, 0), 20);
        Debug()->DebugTextOut(far_patches, Point2D(0, 0), Color(255, 128, 0), 20);
        Debug()->DebugTextOut(gasses, Point2D(0, 0), Color(0, 255, 0), 20);
    }

    void TheBigBot::DisplayBuildOrder()
    {
        std::string build_order_message = "";
        for (int i = mediator.build_order_manager.build_order_step; i < mediator.build_order_manager.build_order_step + 5; i++)
        {
            if (i >= mediator.build_order_manager.build_order.size())
                break;
            build_order_message += mediator.build_order_manager.build_order[i].toString() + "\n";
        }
		if (mediator.build_order_manager.run_build_order)
			Debug()->DebugTextOut(build_order_message, Point2D(.7f, .15f), Color(0, 255, 0), 20);
		else
	        Debug()->DebugTextOut(build_order_message, Point2D(.7f, .15f), Color(255, 0, 0), 20);
    }

    void TheBigBot::DisplayActiveActions()
    {
        std::string actions_message = "Active Actions:\n";
        for (int i = 0; i < mediator.action_manager.active_actions.size(); i++)
        {
            actions_message += mediator.action_manager.active_actions[i]->toString() + "\n";
            const Unit* unit = mediator.action_manager.active_actions[i]->action_arg->unit;
            if (unit != nullptr)
                Debug()->DebugTextOut(mediator.action_manager.active_actions[i]->toString(), unit->pos, Color(0, 255, 0), 20);
        }
        Debug()->DebugTextOut(actions_message, Point2D(.1f, 0), Color(0, 255, 0), 20);
    }

    void TheBigBot::DisplayActiveStateMachines()
    {
        std::string actions_message = "Active StateMachines:\n";
        for (int i = 0; i < mediator.finite_state_machine_manager.active_state_machines.size(); i++)
        {
            actions_message += mediator.finite_state_machine_manager.active_state_machines[i]->toString() + "\n";
        }
        Debug()->DebugTextOut(actions_message, Point2D(.3f, 0), Color(0, 255, 0), 20);
    }

    void TheBigBot::DisplayBuildingStatuses()
    {
        std::string new_lines = "";
        std::vector<UNIT_TYPEID> builging_order = { NEXUS, GATEWAY, WARP_GATE, FORGE, CYBERCORE, ROBO, TWILIGHT };
        for (const auto &building_type : builging_order)
        {
            std::vector<const Unit*> buildings = Observation()->GetUnits(IsFriendlyUnit(building_type));
            sort(begin(buildings), end(buildings), [](const Unit* a, const Unit* b) { return a->tag < b->tag; });
            for (const auto &building : buildings)
            {
				std::string info = UnitTypeToName(building_type);
				info += " ";
                Color text_color = Color(0, 255, 0);
				
                if (building_type == WARP_GATE)
                {
					if (mediator.unit_production_manager.warpgate_status.count(building) == 0)
					{
						std::string todo(10, '-');
						info += " <" + todo + "> ";
					}
					else if (mediator.unit_production_manager.warpgate_status[building].frame_ready == 0)
					{
						text_color = Color(255, 0, 0);
					}
					else
					{
						/*int curr_frame = Observation()->GetGameLoop();
						int start_frame = mediator.unit_production_manager.warpgate_status[building].frame_ready - 720;
						int percent = floor((curr_frame - start_frame) / 72);
						std::string completed(percent, '|');
						std::string todo(10 - percent, '-');
						info += " <" + completed + todo + "> ";*/
					}
                }
                else if (building->orders.empty())
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
			new_lines += "\n";
        }

    }

    void TheBigBot::DisplayArmyGroups()
    {
        std::string army_info = "Armies:\n";
		std::vector<ArmyGroup*> groups = { mediator.army_manager.unassigned_group };
		groups.insert(groups.begin() + 1, mediator.army_manager.army_groups.begin(), mediator.army_manager.army_groups.end());
        for (int i = 0; i < groups.size(); i++)
        {
			army_info += groups[i]->ToString() + "\n";
			if (groups[i]->all_units.size() + groups[i]->new_units.size() > 0)
			{
				army_info += "  Units: " + std::to_string(groups[i]->desired_units) + "/" + std::to_string(groups[i]->max_units) + "\n    ";
				std::map<UNIT_TYPEID, int> unit_totals;
				for (const auto& unit : groups[i]->all_units)
				{
					if (unit_totals.count(unit->unit_type) > 0)
						unit_totals[unit->unit_type] += 1;
					else
						unit_totals[unit->unit_type] = 1;
				}
				for (const auto& unit : groups[i]->new_units)
				{
					if (unit_totals.count(unit->unit_type) > 0)
						unit_totals[unit->unit_type] += 1;
					else
						unit_totals[unit->unit_type] = 1;
				}

				int num_per_line = 0;
				for (const auto& type : ALL_ARMY_UNITS)
				{
					if (unit_totals.count(type) > 0)
					{
						if (num_per_line > 3)
						{
							army_info += "\n    ";
							num_per_line = 0;
						}
						army_info += UnitTypeToName(type);
						army_info += "-" + std::to_string(unit_totals[type]) + ", ";
						num_per_line++;
					}
				}
				army_info += "\n";
			}
        }
        Debug()->DebugTextOut(army_info, Point2D(.8f, .3f), Color(255, 255, 255), 20);
    }

    void TheBigBot::DisplaySupplyInfo()
    {
        std::string supply_message = "";
		supply_message += std::to_string(Observation()->GetGameLoop()) + " - " + std::to_string(Observation()->GetGameLoop() / FRAME_TIME) + '\n';
        int cap = Observation()->GetFoodCap();
        int used = Observation()->GetFoodUsed();
        supply_message += "supply: " + std::to_string(used) + '/' + std::to_string(cap) + '\n';
        int build_pylon_actions = 0;
        for (const auto &action : mediator.action_manager.active_actions)
        {
            if (action->action == &ActionManager::ActionBuildBuilding && action->action_arg->unitId == PYLON)
            {
                build_pylon_actions++;
            }
        }
        int pending_pylons = 0;
        for (const auto &pylon : Observation()->GetUnits(IsFriendlyUnit(PYLON)))
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

        size_t gates = Observation()->GetUnits(IsFinishedUnit(WARP_GATE)).size();
        size_t other_prod = Observation()->GetUnits(IsFinishedUnit(ROBO)).size() + Observation()->GetUnits(IsFinishedUnit(STARGATE)).size();
        if (gates > 0)
            supply_message += "warpgates: " + std::to_string(gates) + '\n';
        if (other_prod > 0)
            supply_message += "other prod: " + std::to_string(other_prod) + '\n';
        size_t nexi = Observation()->GetUnits(IsFriendlyUnit(NEXUS)).size();
        supply_message += "nexi: " + std::to_string(nexi) + '\n';
        supply_message += "new supply: " + std::to_string(used + 2 * gates + 3 * other_prod + nexi) + '/' + std::to_string(cap + 8 * pending_pylons + 8 * build_pylon_actions) + '\n';

		UnitCost income = mediator.CalculateIncome();
		UnitCost production_cost = mediator.CalculateCostOfProduction();
		int mineral_income_size = ((income.mineral_cost / 10) ? 1 : 0) + ((income.mineral_cost / 100) ? 1 : 0) + ((income.mineral_cost / 1000) ? 1 : 0);
		int gas_income_size = ((income.vespene_cost / 10) ? 1 : 0) + ((income.vespene_cost / 100) ? 1 : 0) + ((income.vespene_cost / 1000) ? 1 : 0);
		int mineral_cost_size = ((production_cost.mineral_cost / 10) ? 1 : 0) + ((production_cost.mineral_cost / 100) ? 1 : 0) + ((production_cost.mineral_cost / 1000) ? 1 : 0);
		int gas_cost_size = ((production_cost.vespene_cost / 10) ? 1 : 0) + ((production_cost.vespene_cost / 100) ? 1 : 0) + ((production_cost.vespene_cost / 1000) ? 1 : 0);

		supply_message += "income:    " + std::string(3 - mineral_income_size, ' ') + std::to_string(income.mineral_cost) +
			" " + std::string(3 - gas_income_size, ' ') + std::to_string(income.vespene_cost) + '\n';
		supply_message += "prod cost: " + std::string(3 - mineral_cost_size, ' ') + std::to_string(production_cost.mineral_cost) +
			" " + std::string(3 - gas_cost_size, ' ') + std::to_string(production_cost.vespene_cost) + '\n';
			
        Debug()->DebugTextOut(supply_message, Point2D(.9f, .05f), Color(0, 255, 0), 20);
    }

	void TheBigBot::DisplayOngoingAttacks()
	{
		std::string attacks_message = "Ongoing attacks:\n";
		for (int i = 0; i < mediator.defense_manager.ongoing_attacks.size(); i++)
		{
			attacks_message += "pos: (" + std::to_string(mediator.defense_manager.ongoing_attacks[i].location.x) + ", " + std::to_string(mediator.defense_manager.ongoing_attacks[i].location.y) + ") ";
			attacks_message += "pulled workers: " + std::to_string(mediator.defense_manager.ongoing_attacks[i].pulled_workers.size()) + ", ";
			attacks_message += "status: " + std::to_string(mediator.defense_manager.ongoing_attacks[i].status) + "\n";

		}
		Debug()->DebugTextOut(attacks_message, Point2D(.5, 0), Color(0, 255, 0), 20);
	}

	void TheBigBot::DisplayEnemyAttacks()
	{
		std::string message = "Current frame: " + std::to_string(Observation()->GetGameLoop()) + "\n";
		message += "Current time: " + std::to_string(frames_passed / FRAME_TIME) + "\n";
		for (const auto &unit : enemy_attacks)
		{
			message += UnitTypeToName(unit.first->unit_type.ToType());
			message += ":\n";
			for (const auto &attack : unit.second)
			{
				message += "    ";
				message += UnitTypeToName(attack.unit->unit_type.ToType());
				message += " - " + std::to_string(attack.impact_frame) + "\n";
			}
		}
		Debug()->DebugTextOut(message, Point2D(.8f, .4f), Color(255, 0, 0), 20);
	}

	void TheBigBot::DisplayEnemyPositions()
	{
		for (const auto& unit : mediator.scouting_manager.enemy_unit_saved_position)
		{
			Debug()->DebugTextOut(UnitTypeToName(unit.first->unit_type), ToPoint3D(unit.second.pos), Color(255, 128, 128), 20);
			if (unit.first->unit_type == SIEGE_TANK || unit.first->unit_type == SIEGE_TANK_SIEGED)
				Debug()->DebugSphereOut(ToPoint3D(unit.second.pos), 14, Color(255, 128, 128));
		}
	}

	void TheBigBot::DisplayKnownEffects()
	{
		for (const auto& bile : corrosive_bile_positions)
		{
			Debug()->DebugTextOut("bile", ToPoint3D(bile), Color(255, 140, 0), 20);
			Debug()->DebugSphereOut(ToPoint3D(bile), .5, Color(255, 140, 0));
		}
		for (const auto& zone : liberator_zone_current)
		{
			Debug()->DebugTextOut("liberator zone", ToPoint3D(zone.pos), Color(0, 0, 160), 20);
			Debug()->DebugSphereOut(ToPoint3D(zone.pos), 5, Color(0, 0, 160));
		}
	}

	void TheBigBot::DisplayAlliedAttackStatus()
	{
		return;
	}

	void TheBigBot::PrintNonPathablePoints()
	{
		ImageData raw_map = Observation()->GetGameInfo().pathing_grid;
		std::vector<std::vector<bool>> map;
		std::vector<bool> x;
		map.push_back(x);
		int position = 0;
		int byte = 0;
		int row = 0;
		for (int i = 0; i < raw_map.data.size() * 8; i++)
		{
			if (((raw_map.data[byte] >> (7 - position)) & 0x1))
			{
				map[row].push_back(true);
			}
			else
			{
				map[row].push_back(false);
			}
			position++;
			if (position == 8)
			{
				position = 0;
				byte++;
			}
			if (i != 0 && i % raw_map.width == 0)
			{
				row++;
				std::vector<bool> y;
				map.push_back(y);
				map[row].push_back(false);
			}
		}
		map[row].push_back(false);


		std::vector<std::vector<bool>> flipped_map;
		for (int i = 0; i < map[0].size(); i++)
		{
			std::vector<bool> y;
			flipped_map.push_back(y);
		}
		for (int i = 0; i < map.size(); i++)
		{
			for (int j = 0; j < map[i].size(); j++)
			{
				flipped_map[j].push_back(map[i][j]);
			}
		}

		std::ofstream map_file;
		map_file.open("map.txt", std::ios_base::app);


		for (int i = 25; i < 160; i++)
		{
			for (int j = 17; j < 147; j++)
			{
				if (!flipped_map[i][j])
				{
					bool border = false;
					for (int k = i - 1; k < i + 2; k++)
					{
						for (int l = j - 1; l < j + 2; l++)
						{
							if (flipped_map[k][l])
								border = true;
						}
					}
					if (border)
						map_file << i << ", " << j << "\n";
				}
			}
		}
		map_file.close();

		std::ofstream mineral_file;
		mineral_file.open("minerals.txt", std::ios_base::app);

		for (const auto& unit : Observation()->GetUnits(Unit::Alliance::Neutral))
		{
			mineral_file << unit->pos.x << ", " << unit->pos.y << "\n";
			
		}
		mineral_file.close();
	}

	void TheBigBot::ShowLocations()
	{
		if (!started)
			return;
		for (int i = 0; i < locations->pylon_locations.size(); i++)
		{
			Debug()->DebugSphereOut(ToPoint3D(locations->pylon_locations[i]), 1, Color(255, 0, 255));
			Debug()->DebugTextOut("Pylon " + std::to_string(i), ToPoint3D(locations->pylon_locations[i]), Color(255, 0, 255), 14);
		}
		Debug()->DebugSphereOut(ToPoint3D(locations->first_pylon_location_protoss), 1, Color(255, 0, 255));
		Debug()->DebugTextOut("Pylon P", ToPoint3D(locations->first_pylon_location_protoss), Color(255, 0, 255), 14);
		Debug()->DebugSphereOut(ToPoint3D(locations->first_pylon_location_zerg), 1, Color(255, 0, 255));
		Debug()->DebugTextOut("Pylon Z", ToPoint3D(locations->first_pylon_location_zerg), Color(255, 0, 255), 14);
		Debug()->DebugSphereOut(ToPoint3D(locations->first_pylon_location_terran), 1, Color(255, 0, 255));
		Debug()->DebugTextOut("Pylon T", ToPoint3D(locations->first_pylon_location_terran), Color(255, 0, 255), 14);
		for (int i = 0; i < locations->nexi_locations.size(); i++)
		{
			Debug()->DebugSphereOut(ToPoint3D(locations->nexi_locations[i]), 2.5, Color(0, 0, 255));
			Debug()->DebugTextOut("Nexus " + std::to_string(i), ToPoint3D(locations->nexi_locations[i]), Color(0, 0, 255), 14);
		}
		for (int i = 0; i < locations->gateway_locations.size(); i++)
		{
			Debug()->DebugSphereOut(ToPoint3D(locations->gateway_locations[i]), 1.5, Color(255, 255, 0));
			Debug()->DebugTextOut("Gate " + std::to_string(i), ToPoint3D(locations->gateway_locations[i]), Color(255, 255, 0), 14);
		}
		for (int i = 0; i < locations->tech_locations.size(); i++)
		{
			Debug()->DebugSphereOut(ToPoint3D(locations->tech_locations[i]), 1.5, Color(0, 255, 0));
			Debug()->DebugTextOut("Tech " + std::to_string(i), ToPoint3D(locations->tech_locations[i]), Color(0, 255, 0), 14);
		}
		for (int i = 0; i < locations->cyber_core_locations.size(); i++)
		{
			Debug()->DebugSphereOut(ToPoint3D(locations->cyber_core_locations[i]), 1.5, Color(255, 0, 0));
			Debug()->DebugTextOut("Cyber " + std::to_string(i), ToPoint3D(locations->cyber_core_locations[i]), Color(255, 0, 0), 14);
		}

		for (const auto& pos : locations->blink_nat_attack_path_line.GetPoints())
		{
			Debug()->DebugSphereOut(ToPoint3D(pos), .5, Color(255, 255, 255));
		}
		for (const auto& pos : locations->blink_main_attack_path_lines[0].GetPoints())
		{
			Debug()->DebugSphereOut(ToPoint3D(pos), .5, Color(255, 255, 255));
		}
		for (const auto& pos : locations->blink_main_attack_path_lines[1].GetPoints())
		{
			Debug()->DebugSphereOut(ToPoint3D(pos), .5, Color(255, 255, 255));
		}
		for (const auto& pos : locations->attack_path_line.GetPoints())
		{
			Debug()->DebugSphereOut(ToPoint3D(pos), .5, Color(255, 255, 255));
		}
	}

	std::map<const Unit*, const Unit*> TheBigBot::FindTargets(Units units, std::vector<UNIT_TYPEID> prio, float max_extra_range)
	{
#ifdef DEBUG_TIMING
		std::ofstream find_targets;
		find_targets.open("find_targets.txt", std::ios_base::app);

		unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
#endif
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
			int extra_range = 0;
			while (units_in_range.size() == 0 && extra_range <= max_extra_range)
			{
				for (const auto& Eunit : Eunits)
				{
					if (Distance2D(unit->pos, Eunit->pos) <= Utility::RealRange(unit, Eunit) + extra_range)
						units_in_range.push_back(Eunit);
				}
				extra_range++;
			}
			unit_targets[unit] = units_in_range;
		}
#ifdef DEBUG_TIMING
		unsigned long long set_up_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count() - start_time;

		start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
#endif

		FireControl* fire_control = new FireControl(this, unit_targets, prio);

#ifdef DEBUG_TIMING
		unsigned long long constructor_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count() - start_time;

		find_targets << set_up_time << ", ";
		find_targets << constructor_time << "\n";
		find_targets.close();
#endif

		return fire_control->FindAttacks();

	}

#pragma endregion
}
