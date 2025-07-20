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
				//DisplayEnemyAttacks();

				//DisplayAlliedAttackStatus();


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

			mediator.RunManagers();


#ifndef BUILD_FOR_LADDER

			//DisplayEnemyAttacks();

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

    

    std::vector<Point2D> TheBigBot::FindWarpInSpots(Point2D close_to, int num)
    {
		// order pylons first

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
                    Point2D pos = Point2D(pylon->pos.x + i + .5f, pylon->pos.y + j + .5f);
                    if (Observation()->IsPathable(pos) && Distance2D(pos, pylon->pos) <= 6 && ToPoint3D(pos).y > pylon->pos.y + .5)
                    {
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

                    }
					if (spots.size() >= num)
						break;
                }
				if (spots.size() >= num)
					break;
            }
        }

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

        sort(spots.begin(), spots.end(),
            [close_to](const Point2D & a, const Point2D & b) -> bool
        {
            return Distance2D(a, close_to) < Distance2D(b, close_to);
        });

		spots.insert(spots.begin(), prism_spots.begin(), prism_spots.end());

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
		mediator.DisplayDebugHUD();
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

#pragma endregion
}
