#include "theBigBot.h"
#include "mediator.h"
#include "finite_state_machine.h"
#include "pathfinding.h"
#include "locations.h"
#include "fire_control.h"
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

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"

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

				mediator.RunManagers();


				/*if (!started && Observation()->GetGameLoop() < 1800)
				{
					for (const auto& probe : Observation()->GetUnits(IsUnit(PROBE)))
					{
						std::cout << probe->pos.x << ", " << probe->pos.y << std::endl;
					}
					started = false;
				}*/

				/*for (auto& point : locations->attack_path_line.GetPoints())
				{
					Debug()->DebugSphereOut(ToPoint3D(point), .5, Color(0, 255, 255));
				}*/



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



			Units bla = Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ADEPT));
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



			UpdateEffectPositions();

			UpdateEnemyWeaponCooldowns();

			UpdateEnemyWeaponCooldowns();

			//DisplayEnemyAttacks();

			DisplayEnemyPositions();

			//DisplayAlliedAttackStatus();

			RemoveCompletedAtacks();

			mediator.RunManagers();


#ifndef BUILD_FOR_LADDER

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
				std::cout << Utility::UnitTypeIdToString(building->unit_type) << ' ' << building->pos.x << ", " << building->pos.y << '\n';
				//nav_mesh.AddNewObstacle(building);
				return;
			}
        
			if (building->unit_type == UNIT_TYPEID::PROTOSS_PYLON)
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
            std::cout << UnitTypeIdToString(building->unit_type) << ' ' << building->pos.x << ", " << building->pos.y << '\n';
            return;
        }*/
    }

    void TheBigBot::OnUnitCreated(const Unit *unit)
    {
		try
		{
			if (Observation()->GetGameLoop() == 0)
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
        //std::cout << UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health_damage) << " damage\n";
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

    void TheBigBot::OnUnitDestroyed(const Unit *unit)
    {
        //std::cout << UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed\n";
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
		if (debug_mode)
		{
			if (std::find(test_army.stalkers.begin(), test_army.stalkers.end(), unit) != test_army.stalkers.end())
			{
				test_army.stalkers.erase(std::remove(test_army.stalkers.begin(), test_army.stalkers.end(), unit), test_army.stalkers.end());
				test_army.attack_status.erase(unit);
			}
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
			//Actions()->UnitCommand(unit.first, ABILITY_ID::GENERAL_MOVE, unit.second);
		}
		Actions()->UnitCommand(test_army.prisms[0], ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(Utility::MedianCenter(test_army.stalkers), fallback_point, 3));
		Actions()->UnitCommand(test_army.prisms[0], ABILITY_ID::UNLOADALLAT_WARPPRISM, test_army.prisms[0]);
		ApplyPressureGrouped(&test_army, enemy_army_spawn, fallback_point, retreating_unit_positions, attacking_unit_positions);

		
		
		Units enemy_attacking_units = Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
		//Actions()->UnitCommand(enemy_attacking_units, ABILITY_ID::ATTACK, fallback_point);*/
	}

	void TheBigBot::SpawnArmies()
	{
		//Debug()->DebugEnemyControl();
		//Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ROACH, locations->attack_path[locations->attack_path.size() - 1], 2, 10);
		//Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_RAVAGER, enemy_army_spawn, 2, 5);
		//Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ZERGLING, locations->attack_path[locations->attack_path.size() - 1], 2, 32);

		//Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, enemy_army_spawn, 1, 8);
		//Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARAUDER, enemy_army_spawn, 1, 1);
		//Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, enemy_army_spawn, 1, 1);

		//Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_STALKER, enemy_army_spawn, 2, 8);

		//Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_ADEPT, locations->attack_path[0], 1, 1);
		//Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_STALKER, locations->attack_path[0], 2, 12);
		//Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_WARPPRISM, locations->attack_path[0], 2, 1);
		//Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_ORACLE, locations->attack_path[0], 1, 3);
		//Debug()->SendDebug();
	}

	void TheBigBot::SetUpArmies()
	{
		/*OracleHarassStateMachine* state_machine = new OracleHarassStateMachine(this, Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ORACLE)), "Oracles");
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
			bool in_energy_field = (type == UNIT_TYPEID::PROTOSS_PYLON || type == UNIT_TYPEID::PROTOSS_ASSIMILATOR || type == UNIT_TYPEID::PROTOSS_NEXUS);
			for (const auto& building : Observation()->GetUnits(IsBuilding()))
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
		return Point2D(0, 0);

	}

    std::vector<Point2D> TheBigBot::GetProxyLocations(UNIT_TYPEID type)
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
            int percent = floor(order.progress * 10);
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
        for (const auto &pylon : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_PYLON)))
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

                    Point2D pos = Point2D(pylon->pos.x + i + .5, pylon->pos.y + j + .5);
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

        for (const auto &prism : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPPRISMPHASING)))
        {
            for (int i = -4; i <= 4; i += 1)
            {
                for (int j = -4; j <= 4; j += 1)
                {
                    Point2D pos = Point2D(prism->pos.x + i + .5, prism->pos.y + j + .5);
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
			if (pylon == NULL || pylon->is_alive == false)
				continue;
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
		if (spots.size() == 0)
		{
			spots = FindWarpInSpots(Observation()->GetGameInfo().enemy_start_locations[0], 5);
		}

		std::vector<Point2D> prism_spots;

		for (const auto& prism : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPPRISMPHASING)))
		{
			for (int i = -4; i <= 4; i += 1)
			{
				for (int j = -4; j <= 4; j += 1)
				{
					Point2D pos = Point2D(prism->pos.x + i + .5, prism->pos.y + j + .5);
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

		Units enemy_attacking_units = Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)); // TODO only ranged units

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
				enemy_weapon_cooldown[Eunit] -= 1 / 22.4;
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
			for (int i = attack.second.size() - 1; i >= 0; i--)
			{
				if (attack.second[i].impact_frame <= Observation()->GetGameLoop())
				{
					attack.second.erase(attack.second.begin() + i);
				}
			}
		}
	}

	std::vector<Point2D> TheBigBot::FindConcave(Point2D origin, Point2D fallback_point, int num_units, float unit_size, float dispersion)
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

	std::vector<Point2D> TheBigBot::FindConcaveFromBack(Point2D origin, Point2D fallback_point, int num_units, float unit_size, float dispersion)
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

	void TheBigBot::SetUpUnitTypeInfo()
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
	}


	Polygon TheBigBot::CreateNewBlocker(const Unit* unit)
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
                if (data.second.workers[0] != NULL)
                    close_patches += " X";
                if (data.second.workers[1] != NULL)
                    close_patches += " X";
                if (data.second.workers[2] != NULL)
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
                if (data.second.workers[0] != NULL)
                    far_patches += " X";
                if (data.second.workers[1] != NULL)
                    far_patches += " X";
                if (data.second.workers[2] != NULL)
                    far_patches += " X";
                far_patches += "\n";
            }
        }
        for (const auto &data : mediator.worker_manager.assimilators)
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
			Debug()->DebugTextOut(build_order_message, Point2D(.7, .1), Color(0, 255, 0), 20);
		else
	        Debug()->DebugTextOut(build_order_message, Point2D(.7, .1), Color(255, 0, 0), 20);
    }

    void TheBigBot::DisplayActiveActions()
    {
        std::string actions_message = "Active Actions:\n";
        for (int i = 0; i < mediator.action_manager.active_actions.size(); i++)
        {
            actions_message += mediator.action_manager.active_actions[i]->toString() + "\n";
            const Unit* unit = mediator.action_manager.active_actions[i]->action_arg->unit;
            if (unit != NULL)
                Debug()->DebugTextOut(mediator.action_manager.active_actions[i]->toString(), unit->pos, Color(0, 255, 0), 20);
        }
        Debug()->DebugTextOut(actions_message, Point2D(.1, 0), Color(0, 255, 0), 20);
    }

    void TheBigBot::DisplayActiveStateMachines()
    {
        std::string actions_message = "Active StateMachines:\n";
        for (int i = 0; i < mediator.finite_state_machine_manager.active_state_machines.size(); i++)
        {
            actions_message += mediator.finite_state_machine_manager.active_state_machines[i]->toString() + "\n";
        }
        Debug()->DebugTextOut(actions_message, Point2D(.3, 0), Color(0, 255, 0), 20);
    }

    void TheBigBot::DisplayBuildingStatuses()
    {
        std::string new_lines = "";
        std::vector<UNIT_TYPEID> builging_order = { UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_WARPGATE, UNIT_TYPEID::PROTOSS_FORGE, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL };
        for (const auto &building_type : builging_order)
        {
            std::vector<const Unit*> buildings = Observation()->GetUnits(IsFriendlyUnit(building_type));
            sort(begin(buildings), end(buildings), [](const Unit* a, const Unit* b) { return a->tag < b->tag; });
            for (const auto &building : buildings)
            {
                std::string info = Utility::UnitTypeIdToString(building_type) + " ";
                Color text_color = Color(0, 255, 0);
				
                if (building_type == UNIT_TYPEID::PROTOSS_WARPGATE)
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
            army_info += "Army " + std::to_string(i+1) + " - " + std::to_string(groups[i]->desired_units) + "/" + std::to_string(groups[i]->max_units) + "\n";
			army_info += "  Role - " + ARMY_ROLE_TO_STRING.at(groups[i]->role) + "\n";
			if (groups[i]->all_units.size() + groups[i]->new_units.size() > 0)
			{
				army_info += "  Units: \n    ";
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
						army_info += Utility::UnitTypeIdToString(type) + "-" + std::to_string(unit_totals[type]) + ", ";
						num_per_line++;
					}
				}
				army_info += "\n";
			}
        }
        Debug()->DebugTextOut(army_info, Point2D(.8, .3), Color(255, 255, 255), 20);
    }

    void TheBigBot::DisplaySupplyInfo()
    {
        std::string supply_message = "";
		supply_message += std::to_string(Observation()->GetGameLoop()) + " - " + std::to_string(Observation()->GetGameLoop() / 22.4) + '\n';
        int cap = Observation()->GetFoodCap();
        int used = Observation()->GetFoodUsed();
        supply_message += "supply: " + std::to_string(used) + '/' + std::to_string(cap) + '\n';
        int build_pylon_actions = 0;
        for (const auto &action : mediator.action_manager.active_actions)
        {
            if (action->action == &ActionManager::ActionBuildBuilding && action->action_arg->unitId == UNIT_TYPEID::PROTOSS_PYLON)
            {
                build_pylon_actions++;
            }
        }
        int pending_pylons = 0;
        for (const auto &pylon : Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_PYLON)))
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
        int nexi = Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)).size();
        supply_message += "nexi: " + std::to_string(nexi) + '\n';
        supply_message += "new supply: " + std::to_string(used + 2 * gates + 3 * other_prod + nexi) + '/' + std::to_string(cap + 8 * pending_pylons + 8 * build_pylon_actions) + '\n';

        Debug()->DebugTextOut(supply_message, Point2D(.9, .05), Color(0, 255, 0), 20);
    }

	void TheBigBot::DisplayEnemyAttacks()
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

	void TheBigBot::DisplayEnemyPositions()
	{
		for (const auto& unit : mediator.scouting_manager.enemy_unit_saved_position)
		{
			Debug()->DebugTextOut(Utility::UnitTypeIdToString(unit.first->unit_type), ToPoint3D(unit.second.pos), Color(255, 128, 128), 20);
			if (unit.first->unit_type == SIEGE_TANK || unit.first->unit_type == SIEGE_TANK_SIEGED)
				Debug()->DebugSphereOut(ToPoint3D(unit.second.pos), 14, Color(255, 128, 128));
		}
	}

	void TheBigBot::DisplayAlliedAttackStatus()
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
				std::vector<bool> x;
				map.push_back(x);
				map[row].push_back(false);
			}
		}
		map[row].push_back(false);


		std::vector<std::vector<bool>> flipped_map;
		for (int i = 0; i < map[0].size(); i++)
		{
			std::vector<bool> x;
			flipped_map.push_back(x);
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
