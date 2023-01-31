
#include "TossBot.h"
#include "finish_state_machine.h"
#include "pathfinding.h"
#include "locations.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <iterator>
#include <typeinfo>
#include <ctime>

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"

namespace sc2 {


#pragma region Overrides

    void TossBot::OnGameStart()
    {
        std::cout << "Hello World!" << std::endl;
    }

    void TossBot::OnStep()
    {
        //std::cout << std::to_string(Observation()->GetGameLoop()) << '\n';
        if (debug_mode)
        {

            Units units = Observation()->GetUnits();
            std::vector<UNIT_TYPEID> types;
            for (const auto *unit : units)
            {
                switch (unit->unit_type.ToType())
                {
                case UNIT_TYPEID::NEUTRAL_MINERALFIELD750:
                    Debug()->DebugSphereOut(unit->pos + Point3D(-.5, 0, 0), 1, Color(255, 0, 0));
                    Debug()->DebugSphereOut(unit->pos + Point3D(.5, 0, 0), 1, Color(255, 0, 0));
                    break;
                case UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750:
                    Debug()->DebugSphereOut(unit->pos + Point3D(-.5, 0, 0), 1, Color(255, 128, 0));
                    Debug()->DebugSphereOut(unit->pos + Point3D(.5, 0, 0), 1, Color(255, 128, 0));
                    break;
                case UNIT_TYPEID::NEUTRAL_LABMINERALFIELD:
                    Debug()->DebugSphereOut(unit->pos + Point3D(-.5, 0, 0), 1, Color(255, 255, 0));
                    Debug()->DebugSphereOut(unit->pos + Point3D(.5, 0, 0), 1, Color(255, 255, 0));
                    break;
                case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEULBR:
                    Debug()->DebugSphereOut(unit->pos, 4, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, 0, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, -1, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, -1, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, 0, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, -2, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, -3, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(2, -2, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(2, -3, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(3, -4, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, 0, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, -1, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(2, -1, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(3, -2, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(3, -3, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(4, -3, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, -2, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, -2, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, -3, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, -4, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(2, -4, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(2, -5, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, 1, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, 2, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-3, 1, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-3, 2, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-4, 3, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, 1, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, 1, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, 2, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, 3, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-3, 3, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-3, 4, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, -1, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, 0, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-3, 0, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-4, 1, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-4, 2, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-5, 2, 1) + Point3D(.5, .5, -.5), .5, Color(255, 0, 255));
                    break;
                case UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER:
                    Debug()->DebugSphereOut(unit->pos, 2, Color(0, 255, 0));
                    break;
                case UNIT_TYPEID::NEUTRAL_MINERALFIELD:
                    Debug()->DebugSphereOut(unit->pos + Point3D(-.5, 0, 0), 1, Color(255, 255, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(.5, 0, 0), 1, Color(255, 255, 255));
                    break;
                case UNIT_TYPEID::NEUTRAL_VESPENEGEYSER:
                    Debug()->DebugSphereOut(unit->pos, 2, Color(0, 255, 128));
                    break;
                case UNIT_TYPEID::NEUTRAL_UNBUILDABLEPLATESDESTRUCTIBLE:
                    Debug()->DebugSphereOut(unit->pos, 1, Color(30, 30, 30));
                    break;
                case UNIT_TYPEID::DESTRUCTIBLEDEBRIS4X4:
                    Debug()->DebugSphereOut(unit->pos, 4, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, 0, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, -1, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, 0, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, -1, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, 0, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, -1, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, 0, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, -1, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, 1, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, 1, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, -2, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, -2, 1) + Point3D(.5, .5, 0), .5, Color(0, 0, 255));
                    break;
                case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRIS6X6:
                    Debug()->DebugSphereOut(unit->pos, 4, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, 0, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, -1, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, 0, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, -1, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, 0, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, -1, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(2, 0, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(2, -1, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, 0, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, -1, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-3, 0, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-3, -1, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, 1, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, 2, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, 1, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, 2, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, -2, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(0, -3, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, -2, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-1, -3, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, 1, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(1, -2, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, -2, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
					Debug()->DebugSphereOut(unit->pos + Point3D(-2, 1, 1) + Point3D(.5, .5, 0), .5, Color(0, 255, 255));
                    break;
                case UNIT_TYPEID::DESTRUCTIBLEROCKEX16X6:
                    Debug()->DebugSphereOut(unit->pos, 4, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(0, 0, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-1, 0, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(0, -1, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-1, -1, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(1, 0, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(2, 0, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(1, -1, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(2, -1, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-2, 0, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-3, 0, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-2, -1, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-3, -1, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(0, 1, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(0, 2, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-1, 1, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-1, 2, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(0, -2, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(0, -3, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-1, -2, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-1, -3, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(1, 1, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(1,-2 , 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-2, 1, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    Debug()->DebugSphereOut(unit->pos + Point3D(-2, -2, 1) + Point3D(.5, .5, -.5), .5, Color(128, 0, 255));
                    break;
                case UNIT_TYPEID::PROTOSS_PROBE:
                    break;
                case UNIT_TYPEID::PROTOSS_NEXUS:
                    break;
                default:
                    //std::cout << "ah";
                    break;

                }
            }

			for (const auto &polygon : blockers)
			{
				for (int i = 0; i < polygon.points.size(); i++)
				{
					Point3D start = Point3D(polygon.points[i].x, polygon.points[i].y, Observation()->TerrainHeight(polygon.points[i]) + .1);
					Point3D end;
					if (i + 1 < polygon.points.size())
						end = Point3D(polygon.points[i + 1].x, polygon.points[i + 1].y, Observation()->TerrainHeight(polygon.points[i + 1]) + .1);
					else
						end = Point3D(polygon.points[0].x, polygon.points[0].y, Observation()->TerrainHeight(polygon.points[0]) + .1);
					Debug()->DebugLineOut(start, end, Color(255, 0, 0));
					Debug()->DebugSphereOut(start, .1, Color(0, 0, 255));
				}
			}

            if (Observation()->GetGameLoop() == 1)
            {
                Debug()->DebugFastBuild();
                Debug()->DebugGiveAllResources();
                SetBuildOrder(BuildOrder::oracle_gatewayman_pvz);
                Debug()->DebugCreateUnit(UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEULBR, Point2D(66, 71));
				probe = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_PROBE))[0];
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
                }*/
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

               /* std::vector<Point2D> path = nav_mesh.FindPath(probe->pos, Observation()->GetGameInfo().enemy_start_locations[0]);

                for (int i = 0; i < path.size() - 1; i++)
                {
                    Point3D start = Point3D(path[i].x, path[i].y, Observation()->TerrainHeight(path[i]) + .1);
                    Point3D end = Point3D(path[i+1].x, path[i+1].y, Observation()->TerrainHeight(path[i+1]) + .1);
                    Debug()->DebugLineOut(start, end, Color(0, 255, 0));
                }
                Point3D start = Point3D(probe->pos.x, probe->pos.y, Observation()->TerrainHeight(probe->pos) + .1);
                Point3D end = Point3D(path[0].x, path[0].y, Observation()->TerrainHeight(path[0]) + .1);
                Debug()->DebugLineOut(start, end, Color(0, 255, 0));*/


                Debug()->SendDebug();
            }
            if (Observation()->GetGameLoop() == 3000)
            {
                for (const auto &gate : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY)))
                {
                    std::cout << std::to_string(gate->pos.x) << ' ' << std::to_string(gate->pos.y) << std::endl;
                }
            }
            if (Observation()->GetGameLoop() % 1000 == 0)
            {
                std::cout << "1000\n";
            }

            return;
        }

        
        UpdateUnitTags();
        DistributeWorkers();
        if (new_base != NULL)
        {
            std::cout << "add new base\n";
            std::cout << new_base->pos.x << ' ' << new_base->pos.y << '\n';;
            AddNewBase(new_base);
            if (Observation()->GetGameLoop() < 5)
            {
                SplitWorkers();
            }
        }
        if (Observation()->GetGameLoop() == 1)
        {
            auto infos = Observation()->GetGameInfo().player_info;
            if (infos.size() > 0)
            {
                if (infos[0].race_requested == Race::Protoss)
                    enemy_race = infos[1].race_requested;
                else
                    enemy_race = infos[0].race_requested;
            }

            const Unit *building = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS))[0];
            std::cout << UnitTypeIdToString(building->unit_type);
            if (building->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
            {
                new_base = building;
            }
            else if (building->unit_type == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
            {
                assimilators[building] = assimilator_data();
                //SaturateGas(building->tag);
            }
            
            SetBuildOrder(BuildOrder::fastest_dts);
        }

        
        if (Observation()->GetGameLoop() % 2 == 0)
        {
            BuildWorkers();
            CheckBuildOrder();
            ProcessActions();
        }

        ProcessFSMs();

        DisplayDebugHud();
        Debug()->SendDebug();
    }

    void TossBot::OnBuildingConstructionComplete(const Unit *building)
    {
        if (debug_mode)
        {
            std::cout << UnitTypeIdToString(building->unit_type) << ' ' << building->pos.x << ", " << building->pos.y << '\n';
			Polygon poly = CreateNewBlocker(building);
			blockers.push_back(poly);
			nav_mesh.AddNewObstacle(building);
            return;
        }
        if (building->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
        {
            new_base = building;
        }
        else if (building->unit_type == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
        {
            assimilators[building] = assimilator_data();
            gas_spaces.push_back(new mineral_patch_space(&assimilators[building].workers[0], building));
            gas_spaces.push_back(new mineral_patch_space(&assimilators[building].workers[1], building));
            gas_spaces.push_back(new mineral_patch_space(&assimilators[building].workers[2], building));
            if (immediatelySaturateGasses)
                SaturateGas(building);
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
        if (debug_mode)
        {
            std::cout << UnitTypeIdToString(building->unit_type) << ' ' << building->pos.x << ", " << building->pos.y << '\n';
            return;
        }
    }

    void TossBot::OnUnitCreated(const Unit *unit)
    {
        if (Observation()->GetGameLoop() == 0)
            return;
        if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE)
        {
            PlaceWorker(unit);
        }
        if (army_groups.size() > 0)
        {
            if (unit->unit_type == UNIT_TYPEID::PROTOSS_STALKER)
            {
                army_groups[0]->stalker_ids.push_back(unit->tag);
                std::cout << "add stalker\n";
            }
            else if (unit->unit_type == UNIT_TYPEID::PROTOSS_OBSERVER)
            {
                army_groups[0]->observer_ids.push_back(unit->tag);
                std::cout << "add ob\n";
            }
            else if (unit->unit_type == UNIT_TYPEID::PROTOSS_WARPPRISM)
            {
                army_groups[0]->prism_ids.push_back(unit->tag);
                std::cout << "add prism\n";
            }
            else if (unit->unit_type == UNIT_TYPEID::PROTOSS_IMMORTAL)
            {
                army_groups[0]->immortal_ids.push_back(unit->tag);
                std::cout << "add immortal\n";
            }
            else if (unit->unit_type == UNIT_TYPEID::PROTOSS_ORACLE)
            {
                army_groups[0]->oracle_ids.push_back(unit->tag);
                std::cout << "add oracle\n";
            }
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
        CallOnUnitDamagedEvent(unit, health_damage, shield_damage);
    }

    void TossBot::OnUnitDestroyed(const Unit *unit)
    {
        //std::cout << UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed\n";
        CallOnUnitDestroyedEvent(unit);
		nav_mesh.RemoveObstacle(unit);
    }


#pragma endregion

#pragma region Utility

    void TossBot::UpdateUnitTags()
    {
        tag_to_unit.clear();
        const ObservationInterface* observation = Observation();
        Units all_units = observation->GetUnits();
        for (const Unit * unit : all_units)
        {
            tag_to_unit[unit->tag] = unit;
        }
    }

    Units TossBot::TagsToUnits(const std::vector<Tag> tags)
    {
        Units units;
        for (const auto &tag : tags)
        {
            if (tag_to_unit[tag] != NULL)
                units.push_back(tag_to_unit[tag]);
        }
        return units;
    }

    const Unit* TossBot::ClosestTo(Units units, Point2D position)
    {
        const Unit* current_closest;
        float current_distance = INFINITY;
        for (const auto &unit : units)
        {
            float distance = Distance2D(unit->pos, position);
            if (distance < current_distance)
            {
                current_closest = unit;
                current_distance = distance;
            }
        }
        if (units.size() == 0 || current_closest == NULL)
        {
            std::cout << "Error current closest is NULL\n";
            return NULL;
        }
        return current_closest;
    }

    Point2D TossBot::ClosestTo(std::vector<Point2D> points, Point2D position)
    {
        Point2D current_closest;
        float current_distance = INFINITY;
        for (const auto &point : points)
        {
            float distance = Distance2D(point, position);
            if (distance < current_distance)
            {
                current_closest = point;
                current_distance = distance;
            }
        }
        /*if (points.size() == 0 || current_closest == NULL)
        {
            std::cout << "Error current closest is NULL\n";
            return NULL;
        }*/
        return current_closest;
    }

    float TossBot::DistanceToClosest(Units units, Point2D position)
    {
        const Unit* closest_unit = ClosestTo(units, position);
        if (closest_unit == NULL)
            return INFINITY;
        return Distance2D(closest_unit->pos, position);
    }

    float TossBot::DistanceToClosest(std::vector<Point2D> points, Point2D position)
    {
        const Point2D closest_point = ClosestTo(points, position);
        return Distance2D(closest_point, position);
    }

    Units TossBot::CloserThan(Units units, float distance, Point2D position)
    {
        Units close_units;
        for (const auto &unit : units)
        {
            if (Distance2D(unit->pos, position) <= distance)
            {
                close_units.push_back(unit);
            }
        }
        return close_units;
    }

    bool TossBot::HasBuff(const Unit *unit, BUFF_ID buffId)
    {
        for (const auto &buff : unit->buffs)
        {
            if (buff == buffId)
            {
                return true;
            }
        }
        return false;
    }

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
        std::vector<UNIT_TYPEID> tech_buildings = { UNIT_TYPEID::PROTOSS_FORGE, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL,
            UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, UNIT_TYPEID::PROTOSS_ROBOTICSBAY, UNIT_TYPEID::PROTOSS_STARGATE,
            UNIT_TYPEID::PROTOSS_FLEETBEACON, UNIT_TYPEID::PROTOSS_DARKSHRINE, UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE };
        std::vector<Point2D> possible_locations;
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
        case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
            return locations->proxy_robo_locations;
            break;
        default:
            std::cout << "Error invalid type id in GetProxyLocations" << std::endl;
            return std::vector<Point2D>();
        }
    }


    float TossBot::BuildingSize(UNIT_TYPEID buildingId)
    {
        if (buildingId == UNIT_TYPEID::PROTOSS_NEXUS)
            return 2.5;
        if (buildingId == UNIT_TYPEID::PROTOSS_PYLON || buildingId == UNIT_TYPEID::PROTOSS_SHIELDBATTERY || buildingId == UNIT_TYPEID::PHOTONCANNONWEAPON)
            return 1;
        return 1.5;
    }

    bool TossBot::CanBuildBuilding(UNIT_TYPEID buildingId)
    {
        if (CanAfford(buildingId, 1))
        {
            if (buildingId == UNIT_TYPEID::PROTOSS_GATEWAY)
                return BuildingsReady(UNIT_TYPEID::PROTOSS_PYLON) > 0;
            return true;
        }
        return false;
    }

    int TossBot::BuildingsReady(UNIT_TYPEID buildingId)
    {
        int ready = 0;
        for (const auto &building : Observation()->GetUnits(IsUnit(buildingId)))
        {
            if (building->build_progress == 1)
                ready++;
        }
        return ready;
    }

    ABILITY_ID TossBot::GetBuildAbility(UNIT_TYPEID buildingId)
    {
        switch (buildingId)
        {
        case UNIT_TYPEID::PROTOSS_PYLON:
            return ABILITY_ID::BUILD_PYLON;
        case UNIT_TYPEID::PROTOSS_NEXUS:
            return ABILITY_ID::BUILD_NEXUS;
        case UNIT_TYPEID::PROTOSS_GATEWAY:
            return ABILITY_ID::BUILD_GATEWAY;
        case UNIT_TYPEID::PROTOSS_FORGE:
            return ABILITY_ID::BUILD_FORGE;
        case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
            return ABILITY_ID::BUILD_CYBERNETICSCORE;
        case UNIT_TYPEID::PHOTONCANNONWEAPON:
            return ABILITY_ID::BUILD_PHOTONCANNON;
        case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
            return ABILITY_ID::BUILD_SHIELDBATTERY;
        case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
            return ABILITY_ID::BUILD_TWILIGHTCOUNCIL;
        case UNIT_TYPEID::PROTOSS_STARGATE:
            return ABILITY_ID::BUILD_STARGATE;
        case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
            return ABILITY_ID::BUILD_ROBOTICSFACILITY;
        case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
            return ABILITY_ID::BUILD_ROBOTICSBAY;
        case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
            return ABILITY_ID::BUILD_TEMPLARARCHIVE;
        case UNIT_TYPEID::PROTOSS_DARKSHRINE:
            return ABILITY_ID::BUILD_DARKSHRINE;
        case UNIT_TYPEID::PROTOSS_FLEETBEACON:
            return ABILITY_ID::BUILD_FLEETBEACON;
        case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
            return ABILITY_ID::BUILD_ASSIMILATOR;
        default:
            std::cout << "Error invalid building id in GetBuildAbility";
            return ABILITY_ID::BUILD_CANCEL;
        }
    }

    bool TossBot::CanAfford(UNIT_TYPEID unit, int amount)
    {
        UnitCost cost;
        switch (unit)
        {
        case UNIT_TYPEID::PROTOSS_PYLON:
            cost = UnitCost(100, 0, 0);
            break;
        case UNIT_TYPEID::PROTOSS_NEXUS:
            cost = UnitCost(400, 0, 0);
            break;
        case UNIT_TYPEID::PROTOSS_GATEWAY:
            cost = UnitCost(150, 0, 0);
            break;
        case UNIT_TYPEID::PROTOSS_FORGE:
            cost = UnitCost(150, 0, 0);
            break;
        case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
            cost = UnitCost(150, 0, 0);
            break;
        case UNIT_TYPEID::PHOTONCANNONWEAPON:
            cost = UnitCost(150, 0, 0);
            break;
        case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
            cost = UnitCost(100, 0, 0);
            break;
        case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
            cost = UnitCost(150, 100, 0);
            break;
        case UNIT_TYPEID::PROTOSS_STARGATE:
            cost = UnitCost(150, 150, 0);
            break;
        case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
            cost = UnitCost(150, 100, 0);
            break;
        case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
            cost = UnitCost(150, 150, 0);
            break;
        case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
            cost = UnitCost(150, 200, 0);
            break;
        case UNIT_TYPEID::PROTOSS_DARKSHRINE:
            cost = UnitCost(150, 150, 0);
            break;
        case UNIT_TYPEID::PROTOSS_FLEETBEACON:
            cost = UnitCost(300, 200, 0);
            break;
        case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
            cost = UnitCost(75, 0, 0);
            break;
        case UNIT_TYPEID::PROTOSS_PROBE:
            cost = UnitCost(50, 0, 1);
            break;
        case UNIT_TYPEID::PROTOSS_ZEALOT:
            cost = UnitCost(100, 0, 2);
            break;
        case UNIT_TYPEID::PROTOSS_STALKER:
            cost = UnitCost(125, 50, 2);
            break;
        case UNIT_TYPEID::PROTOSS_SENTRY:
            cost = UnitCost(50, 150, 2);
            break;
        case UNIT_TYPEID::PROTOSS_ADEPT:
            cost = UnitCost(100, 25, 2);
            break;
        case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
            cost = UnitCost(50, 150, 2);
            break;
        case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
            cost = UnitCost(125, 125, 2);
            break;
        case UNIT_TYPEID::PROTOSS_IMMORTAL:
            cost = UnitCost(275, 100, 4);
            break;
        case UNIT_TYPEID::PROTOSS_COLOSSUS:
            cost = UnitCost(300, 200, 6);
            break;
        case UNIT_TYPEID::PROTOSS_DISRUPTOR:
            cost = UnitCost(150, 150, 3);
            break;
        case UNIT_TYPEID::PROTOSS_OBSERVER:
            cost = UnitCost(25, 75, 1);
            break;
        case UNIT_TYPEID::PROTOSS_WARPPRISM:
            cost = UnitCost(250, 0, 2);
            break;
        case UNIT_TYPEID::PROTOSS_PHOENIX:
            cost = UnitCost(150, 100, 2);
            break;
        case UNIT_TYPEID::PROTOSS_VOIDRAY:
            cost = UnitCost(250, 150, 4);
            break;
        case UNIT_TYPEID::PROTOSS_ORACLE:
            cost = UnitCost(150, 150, 3);
            break;
        case UNIT_TYPEID::PROTOSS_CARRIER:
            cost = UnitCost(350, 250, 6);
            break;
        case UNIT_TYPEID::PROTOSS_TEMPEST:
            cost = UnitCost(250, 175, 5);
            break;
        case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
            cost = UnitCost(400, 400, 8);
            break;
        default:
            std::cout << "Error invalid unit id in CanAfford";
            return false;
        }
        bool enough_minerals = Observation()->GetMinerals() >= cost.mineral_cost * amount;
        bool enough_vespene = Observation()->GetVespene() >= cost.vespene_cost * amount;
        bool enough_supply = Observation()->GetFoodCap() - Observation()->GetFoodUsed() >= cost.supply * amount;
        return enough_minerals && enough_vespene && enough_supply;
    }

    bool TossBot::CanAffordUpgrade(UPGRADE_ID upgrade)
    {
        UnitCost cost;
        switch (upgrade)
        {
        case UPGRADE_ID::WARPGATERESEARCH:
            cost = UnitCost(50, 50, 0);
            break;
        case UPGRADE_ID::BLINKTECH:
            cost = UnitCost(150, 150, 0);
            break;
        case UPGRADE_ID::CHARGE:
            cost = UnitCost(100, 100, 0);
            break;
        case UPGRADE_ID::ADEPTPIERCINGATTACK:
            cost = UnitCost(100, 100, 0);
            break;
        case UPGRADE_ID::DARKTEMPLARBLINKUPGRADE:
            cost = UnitCost(100, 100, 0);
            break;
        case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1:
            cost = UnitCost(100, 100, 0);
            break;
        case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2:
            cost = UnitCost(150, 150, 0);
            break;
        case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3:
            cost = UnitCost(200, 200, 0);
            break;
        case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:
            cost = UnitCost(100, 100, 0);
            break;
        case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:
            cost = UnitCost(150, 150, 0);
            break;
        case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:
            cost = UnitCost(200, 200, 0);
            break;
        case UPGRADE_ID::PROTOSSSHIELDSLEVEL1:
            cost = UnitCost(150, 150, 0);
            break;
        case UPGRADE_ID::PROTOSSSHIELDSLEVEL2:
            cost = UnitCost(225, 225, 0);
            break;
        case UPGRADE_ID::PROTOSSSHIELDSLEVEL3:
            cost = UnitCost(300, 300, 0);
            break;
        default:
            std::cout << "Error invalid upgrade id in CanAffordUpgrade";
            return false;
        }
        bool enough_minerals = Observation()->GetMinerals() >= cost.mineral_cost;
        bool enough_vespene = Observation()->GetVespene() >= cost.vespene_cost;
        return enough_minerals && enough_vespene;
    }

    std::string TossBot::BuildOrderToString(std::vector<BuildOrderData> build)
    {
        std::string build_string = "";
        for (const auto &step : build)
        {
            std::string step_string = "";
            if (step.condition == &TossBot::TimePassed)
            {
                step_string += "At time: ";
                step_string += step.condition_arg.time;
            }
            else
            {
                step_string += "Unknown condition";
            }

            if (step.result == &TossBot::BuildBuilding)
            {
                step_string += " build ";
                step_string += UnitTypeIdToString(step.result_arg.unitId);
            }
            step_string += "\n";
            build_string += step_string;
        }
        return build_string;
    }

    std::string TossBot::UnitTypeIdToString(UNIT_TYPEID typeId)
    {
        switch (typeId)
        {
        case UNIT_TYPEID::PROTOSS_PYLON: // protoss buildings
            return "pylon";
        case UNIT_TYPEID::PROTOSS_NEXUS:
            return "nexus";
        case UNIT_TYPEID::PROTOSS_GATEWAY:
            return "gateway";
        case UNIT_TYPEID::PROTOSS_WARPGATE:
            return "warpgate";
        case UNIT_TYPEID::PROTOSS_FORGE:
            return "forge";
        case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
            return "cybercore";
        case UNIT_TYPEID::PHOTONCANNONWEAPON:
            return "photon cannon";
        case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
            return "shield battery";
        case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
            return "twilight";
        case UNIT_TYPEID::PROTOSS_STARGATE:
            return "stargate";
        case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
            return "robo";
        case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
            return "robo bay";
        case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
            return "templar archives";
        case UNIT_TYPEID::PROTOSS_DARKSHRINE:
            return "dark shrine";
        case UNIT_TYPEID::PROTOSS_FLEETBEACON:
            return "fleet beacon";
        case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
            return "assimilator";
        case UNIT_TYPEID::PROTOSS_PROBE: // protoss units
            return "probe";
        case UNIT_TYPEID::PROTOSS_ZEALOT:
            return "zealot";
        case UNIT_TYPEID::PROTOSS_SENTRY:
            return "sentry";
        case UNIT_TYPEID::PROTOSS_STALKER:
            return "stalker";
        case UNIT_TYPEID::PROTOSS_ADEPT:
            return "adept";
        case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
            return "high templar";
        case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
            return "dark templar";
        case UNIT_TYPEID::PROTOSS_IMMORTAL:
            return "immortal";
        case UNIT_TYPEID::PROTOSS_COLOSSUS:
            return "colossus";
        case UNIT_TYPEID::PROTOSS_DISRUPTOR:
            return "disruptor";
        case UNIT_TYPEID::PROTOSS_OBSERVER:
            return "observer";
        case UNIT_TYPEID::PROTOSS_WARPPRISM:
            return "warp prism";
        case UNIT_TYPEID::PROTOSS_PHOENIX:
            return "phoenis";
        case UNIT_TYPEID::PROTOSS_VOIDRAY:
            return "void ray";
        case UNIT_TYPEID::PROTOSS_ORACLE:
            return "oracle";
        case UNIT_TYPEID::PROTOSS_CARRIER:
            return "carrier";
        case UNIT_TYPEID::PROTOSS_TEMPEST:
            return "tempest";
        case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
            return "mothership";
        case UNIT_TYPEID::TERRAN_COMMANDCENTER: // terran buildings
            return "command center";
        case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
            return "planetary forttress";
        case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
            return "orbital command";
        case UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
            return "supply depot";
        case UNIT_TYPEID::TERRAN_REFINERY:
            return "refinery";
        case UNIT_TYPEID::TERRAN_BARRACKS:
            return "barracks";
        case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
            return "engineering bay";
        case UNIT_TYPEID::TERRAN_BUNKER:
            return "bunker";
        case UNIT_TYPEID::TERRAN_SENSORTOWER:
            return "sensor tower";
        case UNIT_TYPEID::TERRAN_MISSILETURRET:
            return "missile turret";
        case UNIT_TYPEID::TERRAN_FACTORY:
            return "factory";
        case UNIT_TYPEID::TERRAN_GHOSTACADEMY:
            return "ghost academy";
        case UNIT_TYPEID::TERRAN_STARPORT:
            return "starport";
        case UNIT_TYPEID::TERRAN_ARMORY:
            return "armory";
        case UNIT_TYPEID::TERRAN_FUSIONCORE:
            return "fusion core";
        case UNIT_TYPEID::TERRAN_TECHLAB:
            return "teck lab";
        case UNIT_TYPEID::TERRAN_REACTOR:
            return "reactor";
        case UNIT_TYPEID::TERRAN_SCV:
            return "SCV";
        case UNIT_TYPEID::TERRAN_MULE:
            return "MULE";
        case UNIT_TYPEID::TERRAN_MARINE: // terran units
            return "marine";
        case UNIT_TYPEID::TERRAN_MARAUDER:
            return "marauder";
        case UNIT_TYPEID::TERRAN_REAPER:
            return "reaper";
        case UNIT_TYPEID::TERRAN_GHOST:
            return "ghost";
        case UNIT_TYPEID::TERRAN_HELLION:
            return "hellion";
        case UNIT_TYPEID::TERRAN_HELLIONTANK:
            return "hellbat";
        case UNIT_TYPEID::TERRAN_SIEGETANK:
            return "siege tank";
        case UNIT_TYPEID::TERRAN_CYCLONE:
            return "cyclone";
        case UNIT_TYPEID::TERRAN_WIDOWMINE:
            return "widow mine";
        case UNIT_TYPEID::TERRAN_THOR:
            return "thor";
        case UNIT_TYPEID::TERRAN_AUTOTURRET:
            return "auto turret";
        case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
            return "viking assault";
        case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
            return "viking fighter";
        case UNIT_TYPEID::TERRAN_MEDIVAC:
            return "medivac";
        case UNIT_TYPEID::TERRAN_LIBERATOR:
            return "liberator";
        case UNIT_TYPEID::TERRAN_RAVEN:
            return "raven";
        case UNIT_TYPEID::TERRAN_BANSHEE:
            return "banshee";
        case UNIT_TYPEID::TERRAN_BATTLECRUISER:
            return "battlecruiser";
        case UNIT_TYPEID::ZERG_HATCHERY: // zerg buildings
            return "hatchery";
        case UNIT_TYPEID::ZERG_LAIR:
            return "lair";
        case UNIT_TYPEID::ZERG_HIVE:
            return "hive";
        case UNIT_TYPEID::ZERG_SPINECRAWLER:
            return "spine crawler";
        case UNIT_TYPEID::ZERG_SPORECRAWLER:
            return "spore crawler";
        case UNIT_TYPEID::ZERG_EXTRACTOR:
            return "extractor";
        case UNIT_TYPEID::ZERG_SPAWNINGPOOL:
            return "spawning pool";
        case UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:
            return "evolution chamber";
        case UNIT_TYPEID::ZERG_ROACHWARREN:
            return "roach warren";
        case UNIT_TYPEID::ZERG_BANELINGNEST:
            return "baneling next";
        case UNIT_TYPEID::ZERG_HYDRALISKDEN:
            return "hydralisk den";
        case UNIT_TYPEID::ZERG_LURKERDENMP:
            return "lurker den";
        case UNIT_TYPEID::ZERG_INFESTATIONPIT:
            return "infestation pit";
        case UNIT_TYPEID::ZERG_SPIRE:
            return "spire";
        case UNIT_TYPEID::ZERG_GREATERSPIRE:
            return "greater spire";
        case UNIT_TYPEID::ZERG_NYDUSNETWORK:
            return "nydus network";
        case UNIT_TYPEID::ZERG_ULTRALISKCAVERN:
            return "ultralisk cavern";
        case UNIT_TYPEID::ZERG_LARVA: // zerg units
            return "larva";
        case UNIT_TYPEID::ZERG_EGG:
            return "egg";
        case UNIT_TYPEID::ZERG_DRONE:
            return "drone";
        case UNIT_TYPEID::ZERG_QUEEN:
            return "queen";
        case UNIT_TYPEID::ZERG_ZERGLING:
            return "zergling";
        case UNIT_TYPEID::ZERG_BANELING:
            return "baneling";
        case UNIT_TYPEID::ZERG_ROACH:
            return "roach";
        case UNIT_TYPEID::ZERG_RAVAGER:
            return "ravager";
        case UNIT_TYPEID::ZERG_HYDRALISK:
            return "hydralisk";
        case UNIT_TYPEID::ZERG_LURKERMP:
            return "lurker";
        case UNIT_TYPEID::ZERG_INFESTOR:
            return "infestor";
        case UNIT_TYPEID::ZERG_SWARMHOSTMP:
            return "swarm host";
        case UNIT_TYPEID::ZERG_ULTRALISK:
            return "ultralisk";
        case UNIT_TYPEID::ZERG_OVERLORD:
            return "overlord";
        case UNIT_TYPEID::ZERG_OVERSEER:
            return "overseer";
        case UNIT_TYPEID::ZERG_MUTALISK:
            return "mutalisk";
        case UNIT_TYPEID::ZERG_CORRUPTOR:
            return "corruptor";
        case UNIT_TYPEID::ZERG_BROODLORD:
            return "broodlord";
        case UNIT_TYPEID::ZERG_VIPER:
            return "viper";
        case UNIT_TYPEID::ZERG_LOCUSTMP:
            return "locust";
        case UNIT_TYPEID::ZERG_BROODLING:
            return "broodling";
        case UNIT_TYPEID::ZERG_TRANSPORTOVERLORDCOCOON:
            return "dropperlord cocoon";
        case UNIT_TYPEID::ZERG_BANELINGCOCOON:
            return "baneling cocoon";
        case UNIT_TYPEID::ZERG_BROODLORDCOCOON:
            return "broodlord cocoon";
        case UNIT_TYPEID::ZERG_OVERLORDCOCOON:
            return "overseer cocoon";
        case UNIT_TYPEID::ZERG_RAVAGERCOCOON:
            return "ravager cocoon";
        case UNIT_TYPEID::ZERG_LURKERMPEGG:
            return "lurker cocoon";
        default:
            std::cout << "Error invalid typeId in UnitTypeIdToString";
            return "Error invalid abilityId in UnitTypeIdToString";
            break;
        }
    }

    std::string TossBot::AbilityIdToString(ABILITY_ID abilityId)
    {
        switch (abilityId)
        {
        case ABILITY_ID::TRAIN_PROBE:
            return "train probe";
        case ABILITY_ID::TRAIN_ZEALOT:
            return "train zealot";
        case ABILITY_ID::TRAIN_ADEPT:
            return "train adept";
        case ABILITY_ID::TRAIN_STALKER:
            return "train stalker";
        case ABILITY_ID::TRAIN_SENTRY:
            return "train sentry";
        case ABILITY_ID::TRAIN_IMMORTAL:
            return "train immortal";
        case ABILITY_ID::TRAIN_OBSERVER:
            return "train observer";
        case ABILITY_ID::TRAIN_WARPPRISM:
            return "train warpprism";
        case ABILITY_ID::RESEARCH_WARPGATE:
            return "research warpgate";
        case ABILITY_ID::RESEARCH_BLINK:
            return "research blink";
        case ABILITY_ID::RESEARCH_CHARGE:
            return "research charge";
        case ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS:
            return "research ground attack";
        case ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1:
            return "research +1 attack";
        case ABILITY_ID::MORPH_WARPGATE:
            return "morph into warpgate";
        default:
            std::cout << "Error invalid abilityId in AbilityIdToString\n";
            return "Error invalid abilityId in AbilityIdToString";
            break;
        }
    }

    std::string TossBot::OrdersToString(std::vector<UnitOrder> orders)
    {
        std::string text = "";
        for (const auto &order : orders)
        {
            text += AbilityIdToString(order.ability_id);
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
                    if (Distance2D(pos, pylon->pos) <= 6 && DistanceToClosest(Observation()->GetUnits(), pos) > 1.5)
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
                    if (Observation()->IsPathable(pos) && Distance2D(pos, prism->pos) <= 3.75 && DistanceToClosest(Observation()->GetUnits(), pos) > 1)
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
                    if (Observation()->IsPathable(pos) && Distance2D(pos, pylon_pos) <= 6 && DistanceToClosest(Observation()->GetUnits(), pos) > 1)
                        spots.push_back(pos);
                }
            }
        }
        return spots;
    }

    const Unit* TossBot::GetLeastFullPrism(Units units)
    {
        const Unit* least_full = NULL;
        for (const auto &unit : units)
        {
            if (least_full == NULL || unit->cargo_space_taken < least_full->cargo_space_taken)
                least_full = unit;
        }
        return least_full;
    }

    void TossBot::OraclesCoverStalkers(Units stalkers, Units oracles)
    {
        Point2D center = MedianCenter(stalkers);
        bool danger = Observation()->GetUnits(Unit::Alliance::Enemy).size() > 0 && DistanceToClosest(Observation()->GetUnits(Unit::Alliance::Enemy), center) < 5;
        
        for (const auto &oracle : oracles)
        {
            if (danger && Distance2D(oracle->pos, center) < 5)
            {
                for (const auto & ability : Query()->GetAbilitiesForUnit(oracle).abilities)
                {
                    if (ability.ability_id.ToType() == ABILITY_ID::BEHAVIOR_PULSARBEAMON)
                        Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMON, false);
                }
                const Unit* closest_unit = ClosestTo(Observation()->GetUnits(Unit::Alliance::Enemy), oracle->pos);
                if (oracle->orders[0].ability_id.ToType() == ABILITY_ID::GENERAL_MOVE)
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
        for (const auto &state_machine : active_FSMs)
        {
            state_machine->RunStateMachine();
        }
    }

#pragma endregion

#pragma region Events

    void TossBot::AddListenerToOnUnitDamagedEvent(std::function<void(const Unit*, float, float)> func)
    {
        on_unit_damaged_event.listeners.push_back(func);
    }

    void TossBot::CallOnUnitDamagedEvent(const Unit* unit, float health, float shields)
    {
        for (const auto &func : on_unit_damaged_event.listeners)
        {
            func(unit, health, shields);
        }
    }

    void TossBot::AddListenerToOnUnitDestroyedEvent(std::function<void(const Unit*)> func)
    {
        on_unit_destroyed_event.listeners.push_back(func);
    }

    void TossBot::CallOnUnitDestroyedEvent(const Unit* unit)
    {
        for (const auto &func : on_unit_destroyed_event.listeners)
        {
            func(unit);
        }
    }

#pragma endregion

#pragma region Worker Management

    const Unit* TossBot::GetWorker()
    {
        const ObservationInterface* observation = Observation();
        // Units mineral_fields = observation->GetUnits(IsMineralPatch());
        std::map<const Unit*, mineral_patch_data> close_patches;
        for (const auto &field : mineral_patches)
        {
            if (field.second.is_close && field.second.workers[0] != NULL)
            {
                close_patches[field.first] = field.second;
            }
        }
        std::map<const Unit*, mineral_patch_data> close_oversaturated_patches;
        for (const auto &field : close_patches)
        {
            if (field.second.workers[2] != NULL)
            {
                close_oversaturated_patches[field.first] = field.second;
            }
        }
        // check close mineral patches with 3 assigned harvesters
        if (close_oversaturated_patches.size() > 0)
        {
            for (const auto &field : close_oversaturated_patches)
            {
                return field.second.workers[2];
            }
        }

        std::map<const Unit*, mineral_patch_data> far_patches;
        for (const auto &field : mineral_patches)
        {
            if (!field.second.is_close && field.second.workers[0] != NULL)
            {
                far_patches[field.first] = field.second;
            }
        }
        std::map<const Unit*, mineral_patch_data> far_oversaturated_patches;
        for (const auto &field : far_patches)
        {
            if (field.second.workers[2] != NULL)
            {
                far_oversaturated_patches[field.first] = field.second;
            }
        }
        // check far mineral patches with 3 assigned harvesters
        if (far_oversaturated_patches.size() > 0)
        {
for (const auto &field : far_oversaturated_patches)
{
    return field.second.workers[2];
}
        }
        // check far mineral patches with <3 assigned harvesters
        if (far_patches.size() > 0)
        {
            for (const auto &field : far_patches)
            {
                if (field.second.workers[1] != NULL)
                {
                    return field.second.workers[1];
                }
            }
            for (const auto &field : far_patches)
            {
                if (field.second.workers[0] != NULL)
                {
                    return field.second.workers[0];
                }
            }
        }
        // check close mineral patches with <3 assigned harvesters
        if (close_patches.size() > 0)
        {
            for (const auto &field : close_patches)
            {
                if (field.second.workers[1] != NULL)
                {
                    return field.second.workers[1];
                }
            }
            for (const auto &field : close_patches)
            {
                if (field.second.workers[0] != NULL)
                {
                    return field.second.workers[0];
                }
            }
        }
        std::cout << "Error no available worker found";
        return NULL;
    }

    const Unit* TossBot::GetBuilder(Point2D position)
    {
        const ObservationInterface* observation = Observation();
        Units mineral_fields = observation->GetUnits(IsMineralPatch());
        Units mineral_patches_reversed_keys;
        Units far_only_mineral_patches_reversed_keys;
        for (const auto &patch : mineral_patches_reversed)
        {
            if (!mineral_patches[mineral_patches_reversed[patch.first].mineral_tag].is_close)
                far_only_mineral_patches_reversed_keys.push_back(patch.first);
            mineral_patches_reversed_keys.push_back(patch.first);

        }
        const Unit* closest;
        if (far_only_mineral_patches_reversed_keys.size() > 0)
        {
            closest = ClosestTo(far_only_mineral_patches_reversed_keys, position);
        }
        if (mineral_patches_reversed_keys.size() > 0)
        {
            const Unit* c = ClosestTo(mineral_patches_reversed_keys, position);
            if (Distance2D(closest->pos, position) < Distance2D(c->pos, position) * 1.2)
                return closest;
            else
                return c;
        }
        std::cout << "Error mineral patches reversed is empty in GetBuilder";
        return NULL;
    }

    void TossBot::PlaceWorker(const Unit* worker)
    {
        if (first_2_mineral_patch_spaces.size() > 0)
        {
            float distance = INFINITY;
            mineral_patch_space* closest = NULL;
            float distance_c = INFINITY;
            mineral_patch_space* closest_c = NULL;
            for (mineral_patch_space* &space : first_2_mineral_patch_spaces)
            {
                float dist = Distance2D(worker->pos, space->mineral_patch->pos);
                if (closest == NULL || dist < distance)
                {
                    distance = dist;
                    closest = space;
                }
                if (mineral_patches[space->mineral_patch].is_close && (closest_c == NULL || dist < distance_c))
                {
                    distance_c = dist;
                    closest_c = space;
                }
            }
            if (closest_c != NULL)
            {
                (*closest_c->worker) = worker;
                NewPlaceWorkerOnMinerals(worker, closest_c->mineral_patch);
                first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), closest_c), first_2_mineral_patch_spaces.end());
            }
            else
            {
                (*closest->worker) = worker;
                NewPlaceWorkerOnMinerals(worker, closest->mineral_patch);
                first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), closest), first_2_mineral_patch_spaces.end());
            }
            return;
        }
        if (gas_spaces.size() > removed_gas_miners)
        {
            float distance = INFINITY;
            mineral_patch_space* closest = NULL;
            for (mineral_patch_space* &space : gas_spaces)
            {
                float dist = Distance2D(worker->pos, space->mineral_patch->pos);
                if (closest == NULL || dist < distance)
                {
                    distance = dist;
                    closest = space;
                }
            }
            *(closest->worker) = worker;
            NewPlaceWorkerInGas(worker, closest->mineral_patch);
            gas_spaces.erase(std::remove(gas_spaces.begin(), gas_spaces.end(), closest), gas_spaces.end());

            return;
        }
        if (far_3_mineral_patch_spaces.size() > 0)
        {
            float distance = INFINITY;
            mineral_patch_space* closest = NULL;
            for (mineral_patch_space* &space : far_3_mineral_patch_spaces)
            {
                float dist = Distance2D(worker->pos, space->mineral_patch->pos);
                if (closest == NULL || dist < distance)
                {
                    distance = dist;
                    closest = space;
                }
            }
            *(closest->worker) = worker;
            NewPlaceWorkerOnMinerals(worker, closest->mineral_patch);
            far_3_mineral_patch_spaces.erase(std::remove(far_3_mineral_patch_spaces.begin(), far_3_mineral_patch_spaces.end(), closest), far_3_mineral_patch_spaces.end());
            mineral_patch_space* space = new mineral_patch_space(closest->worker, closest->mineral_patch);
            far_3_mineral_patch_extras.push_back(space);

            return;
        }
        if (close_3_mineral_patch_spaces.size() > 0)
        {
            float distance = INFINITY;
            mineral_patch_space* closest = NULL;
            for (mineral_patch_space* &space : close_3_mineral_patch_spaces)
            {
                float dist = Distance2D(worker->pos, space->mineral_patch->pos);
                if (closest == NULL || dist < distance)
                {
                    distance = dist;
                    closest = space;
                }
            }
            (*closest->worker) = worker;
            NewPlaceWorkerOnMinerals(worker, closest->mineral_patch);
            close_3_mineral_patch_spaces.erase(std::remove(close_3_mineral_patch_spaces.begin(), close_3_mineral_patch_spaces.end(), closest), close_3_mineral_patch_spaces.end());
            close_3_mineral_patch_extras.push_back(new mineral_patch_space(closest->worker, closest->mineral_patch));
            return;
        }
        std::cout << "Error no place for worker\n";
    }

    void TossBot::PlaceWorkerInGas(const Unit* worker, const Unit* gas, int index)
    {
        const ObservationInterface* observation = Observation();
        if (assimilators.find(gas) == assimilators.end())
        {
            assimilators[gas] = assimilator_data();
        }
        assimilators[gas].workers[index] = worker;
        Point2D assimilator_position = gas->pos;
        Units townhalls = observation->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));
        const Unit* closest_nexus = ClosestTo(townhalls, assimilator_position);
        Point2D vector = assimilator_position - closest_nexus->pos;
        Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
        Point2D drop_off_point = closest_nexus->pos + normal_vector * 2;
        Point2D pick_up_point = assimilator_position - normal_vector * .5;
        assimilator_reversed_data data;
        data.assimilator_tag = gas;
        data.drop_off_point = drop_off_point;
        data.pick_up_point = pick_up_point;
        assimilators_reversed[worker] = data;
    }

    void TossBot::NewPlaceWorkerInGas(const Unit* worker, const Unit* gas)
    {
        const ObservationInterface* observation = Observation();
        if (assimilators.find(gas) == assimilators.end())
        {
            assimilators[gas] = assimilator_data();
        }
        Point2D assimilator_position = gas->pos;
        Units townhalls = observation->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));
        const Unit* closest_nexus = ClosestTo(townhalls, assimilator_position);
        Point2D vector = assimilator_position - closest_nexus->pos;
        Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
        Point2D drop_off_point = closest_nexus->pos + normal_vector * 2;
        Point2D pick_up_point = assimilator_position - normal_vector * .5;
        assimilator_reversed_data data;
        data.assimilator_tag = gas;
        data.drop_off_point = drop_off_point;
        data.pick_up_point = pick_up_point;
        assimilators_reversed[worker] = data;
    }

    void TossBot::PlaceWorkerOnMinerals(const Unit* worker, const Unit* mineral, int index)
    {
        const ObservationInterface* observation = Observation();
        if (mineral_patches.find(mineral) == mineral_patches.end())
        {
            mineral_patches[mineral] = mineral_patch_data(mineral->mineral_contents == 1800);
        }
        mineral_patches[mineral].workers[index] = worker;
        Point2D mineral_position = mineral->pos;
        Units townhalls = observation->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));
        const Unit* closest_nexus = ClosestTo(townhalls, mineral_position);
        Point2D vector = mineral_position - closest_nexus->pos;
        Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
        Point2D drop_off_point = closest_nexus->pos + normal_vector * 2;
        Point2D pick_up_point = mineral_position - normal_vector * .5;
        mineral_patch_reversed_data data;
        data.mineral_tag = mineral;
        data.drop_off_point = drop_off_point;
        data.pick_up_point = pick_up_point;
        mineral_patches_reversed[worker] = data;
    }

    void TossBot::NewPlaceWorkerOnMinerals(const Unit* worker, const Unit* mineral)
    {
        const ObservationInterface* observation = Observation();
        if (mineral_patches.find(mineral) == mineral_patches.end())
        {
            mineral_patches[mineral] = mineral_patch_data(mineral->mineral_contents == 1800);
        }
        Point2D mineral_position = mineral->pos;
        Units townhalls = observation->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));
        const Unit* closest_nexus = ClosestTo(townhalls, mineral_position);
        Point2D vector = mineral_position - closest_nexus->pos;
        Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
        Point2D drop_off_point = closest_nexus->pos + normal_vector * 2;
        Point2D pick_up_point = mineral_position - normal_vector * .5;
        mineral_patch_reversed_data data;
        data.mineral_tag = mineral;
        data.drop_off_point = drop_off_point;
        data.pick_up_point = pick_up_point;
        mineral_patches_reversed[worker] = data;
    }

    void TossBot::RemoveWorker(const Unit* worker)
    {
        auto rem = [&](mineral_patch_space* space) -> bool
        {
            return *(space->worker) == worker;
        };
        if (mineral_patches_reversed.find(worker) != mineral_patches_reversed.end())
        {
            const Unit* mineral = mineral_patches_reversed[worker].mineral_tag;
            mineral_patches_reversed.erase(worker);
            bool is_close = mineral_patches[mineral].is_close;
            
            if (mineral_patches[mineral].workers[0] == worker)
            {
                if (mineral_patches[mineral].workers[1] == NULL)
                {
                    mineral_patches[mineral].workers[0] = NULL;
                    first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[0], mineral));
                }
                else
                {
                    mineral_patches[mineral].workers[0] = mineral_patches[mineral].workers[1];
                    if (mineral_patches[mineral].workers[2] == NULL)
                    {
                        mineral_patches[mineral].workers[1] = NULL;
                        first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[1], mineral));
                    }
                    else
                    {
                        if (is_close)
                        {
                            close_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
                            close_3_mineral_patch_extras.erase(std::remove_if(close_3_mineral_patch_extras.begin(), close_3_mineral_patch_extras.end(), rem), close_3_mineral_patch_extras.end());
                        }
                        else
                        {
                            far_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
                            far_3_mineral_patch_extras.erase(std::remove_if(far_3_mineral_patch_extras.begin(), far_3_mineral_patch_extras.end(), rem), far_3_mineral_patch_extras.end());
                        }
                        mineral_patches[mineral].workers[1] = mineral_patches[mineral].workers[2];
                        mineral_patches[mineral].workers[2] = NULL;
                    }
                }
            }
            else if (mineral_patches[mineral].workers[1] == worker)
            {
                if (mineral_patches[mineral].workers[2] == NULL)
                {
                    mineral_patches[mineral].workers[1] = NULL;
                    first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[1], mineral));
                }
                else
                {
                    if (is_close)
                    {
                        close_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
                        close_3_mineral_patch_extras.erase(std::remove_if(close_3_mineral_patch_extras.begin(), close_3_mineral_patch_extras.end(), rem), close_3_mineral_patch_extras.end());
                    }
                    else
                    {
                        far_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
                        far_3_mineral_patch_extras.erase(std::remove_if(far_3_mineral_patch_extras.begin(), far_3_mineral_patch_extras.end(), rem), far_3_mineral_patch_extras.end());
                    }
                    mineral_patches[mineral].workers[1] = mineral_patches[mineral].workers[2];
                    mineral_patches[mineral].workers[2] = NULL;
                }
            }
            else if (mineral_patches[mineral].workers[2] == worker)
            {
                if (is_close)
                {
                    close_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
                    close_3_mineral_patch_extras.erase(std::remove_if(close_3_mineral_patch_extras.begin(), close_3_mineral_patch_extras.end(), rem), close_3_mineral_patch_extras.end());
                }
                else
                {
                    far_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
                    far_3_mineral_patch_extras.erase(std::remove_if(far_3_mineral_patch_extras.begin(), far_3_mineral_patch_extras.end(), rem), far_3_mineral_patch_extras.end());
                }
                mineral_patches[mineral].workers[2] = NULL;
            }
        }
        else if (assimilators_reversed.find(worker) != assimilators_reversed.end())
        {
            const Unit* assimilator = assimilators_reversed[worker].assimilator_tag;
            assimilators_reversed.erase(worker);
            if (assimilators[assimilator].workers[0] == worker)
            {
                if (assimilators[assimilator].workers[1] == NULL)
                {
                    assimilators[assimilator].workers[0] = NULL;
                    gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[0], assimilator));
                }
                else
                {
                    assimilators[assimilator].workers[0] = assimilators[assimilator].workers[1];
                    if (assimilators[assimilator].workers[2] == NULL)
                    {
                        assimilators[assimilator].workers[1] = NULL;
                        gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[1], assimilator));
                    }
                    else
                    {
                        assimilators[assimilator].workers[1] = assimilators[assimilator].workers[2];
                        assimilators[assimilator].workers[2] = NULL;
                        gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[2], assimilator));
                    }
                }
            }
            else if (assimilators[assimilator].workers[1] == worker)
            {
                if (assimilators[assimilator].workers[2] == NULL)
                {
                    assimilators[assimilator].workers[1] = NULL;
                    gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[1], assimilator));
                }
                else
                {
                    assimilators[assimilator].workers[1] = assimilators[assimilator].workers[2];
                    assimilators[assimilator].workers[2] = NULL;
                    gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[2], assimilator));
                }
            }
            else if (assimilators[assimilator].workers[2] == worker)
            {
                assimilators[assimilator].workers[2] = NULL;
                gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[2], assimilator));
            }
        }
    }

    void TossBot::SplitWorkers()
    {
        const ObservationInterface* observation = Observation();
        Units workers = observation->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_PROBE));
        Units patches;
        for (const auto &mineral_patch : mineral_patches)
        {
            patches.push_back(mineral_patch.first);
        }
        for (const auto &patch : patches)
        {
            const Unit* closest_worker = ClosestTo(workers, patch->pos);
            PlaceWorkerOnMinerals(closest_worker, patch, 0);
            for (mineral_patch_space* &space : first_2_mineral_patch_spaces)
            {
                if (patch == space->mineral_patch)
                {
                    first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), space), first_2_mineral_patch_spaces.end());
                    break;
                }
            }
            workers.erase(std::remove(workers.begin(), workers.end(), closest_worker), workers.end());
        }
        for (const auto &patch : patches)
        {
            if (patch->mineral_contents == 1800)
            {
                const Unit* closest_worker = ClosestTo(workers, patch->pos);
                PlaceWorkerOnMinerals(closest_worker, patch, 1);
                for (mineral_patch_space* &space : first_2_mineral_patch_spaces)
                {
                    if (patch == space->mineral_patch)
                    {
                        first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), space), first_2_mineral_patch_spaces.end());
                        break;
                    }
                }
                workers.erase(std::remove(workers.begin(), workers.end(), closest_worker), workers.end());
            }
        }
    }

    void TossBot::SaturateGas(const Unit* gas)
    {
        for (int i = 0; i < 3; i++)
        {
            const Unit* worker = GetWorker();
            RemoveWorker(worker);
            PlaceWorkerInGas(worker, gas, i);
            for (const auto &space : gas_spaces)
            {
                if (*space->worker == worker)
                {
                    gas_spaces.erase(std::remove(gas_spaces.begin(), gas_spaces.end(), space), gas_spaces.end());
                    break;
                }
            }
        }
    }

    void TossBot::AddNewBase(const Unit *nexus)
    {
        Units minerals = Observation()->GetUnits(IsMineralPatch());
        Units close_minerals = CloserThan(minerals, 10, nexus->pos);
        for (const auto &mineral_field : close_minerals)
        {
            if (mineral_field->display_type == Unit::Snapshot)
            {
                return;
            }
        }
        for (const auto &mineral_field : close_minerals)
        {
            bool is_close = mineral_field->mineral_contents == 1800;
            mineral_patches[mineral_field] = mineral_patch_data(is_close);
            first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral_field].workers[0], mineral_field));
            first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral_field].workers[1], mineral_field));
            if (is_close)
                close_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral_field].workers[2], mineral_field));
            else
                far_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral_field].workers[2], mineral_field));
            new_base = NULL;
        }
    }

    void TossBot::DistributeWorkers()
    {
        BalanceWorers();

        Units workers;
        for (const auto &worker : mineral_patches_reversed)
        {
            workers.push_back(worker.first);
        }
        for (const auto &worker : workers)
        {
            if (IsCarryingMinerals(*worker))
            {
                // close to nexus then return the mineral
                Point2D closest_nexus = ClosestTo(Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)), worker->pos)->pos;
                if (DistanceSquared2D(closest_nexus, worker->pos) < 10 || CloserThan(workers, .75, worker->pos).size() > 1)
                {
                    Actions()->UnitCommand(worker, ABILITY_ID::HARVEST_RETURN_PROBE);
                    continue;
                }
                Point2D drop_off_point = mineral_patches_reversed[worker].drop_off_point;
                // don't get bugged out trying to move to the drop off point
                if (worker->orders.size() > 0 && worker->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE)
                {
                    if (DistanceSquared2D(worker->pos, drop_off_point) < 1)
                    {
                        continue;
                    }
                }
                // otherwise move to the drop off point
                const Unit *mineral_patch = mineral_patches_reversed[worker].mineral_tag;
                if (mineral_patch != NULL)
                {
                    Actions()->UnitCommand(worker, ABILITY_ID::GENERAL_MOVE, drop_off_point);
                }
                else
                {
                    std::cout << "Error null mineral patching in DistributeWorkers 1";
                }
            }
            else
            {
                const Unit *mineral_patch = mineral_patches_reversed[worker].mineral_tag;
                if (mineral_patch != NULL)
                {
                    if (DistanceSquared2D(worker->pos, mineral_patch->pos) < 4 || CloserThan(workers, .75, worker->pos).size() > 1)
                    {
                        if (worker->orders.size() == 0 || worker->orders[0].target_unit_tag != mineral_patch->tag)
                        {
                            Actions()->UnitCommand(worker, ABILITY_ID::HARVEST_GATHER_PROBE, mineral_patch);
                        }
                    }
                    else if (worker->orders.size() == 0 || worker->orders[0].ability_id != ABILITY_ID::GENERAL_MOVE || DistanceSquared2D(mineral_patches_reversed[worker].pick_up_point, worker->orders[0].target_pos) > 1)
                    {
                        Actions()->UnitCommand(worker, ABILITY_ID::GENERAL_MOVE, mineral_patches_reversed[worker].pick_up_point);
                    }
                }
                else
                {
                    std::cout << "Error null mineral patching in DistributeWorkers 2";
                }
            }
        }

        Units gas_workers;
        for (const auto &worker : assimilators_reversed)
        {
            gas_workers.push_back(worker.first);
        }
        for (const auto &worker : gas_workers)
        {
            const Unit* assimilator = assimilators_reversed[worker].assimilator_tag;
            if (assimilators[assimilator].workers[1] != NULL)
            {
                // 2 or 3 workers assigned to gas
                if (worker->orders.size() == 0)
                {
                    Actions()->UnitCommand(worker, ABILITY_ID::SMART, assimilator);
                }
                else
                {
                    UnitOrder current_order = worker->orders[0];
                    if (current_order.ability_id == ABILITY_ID::HARVEST_GATHER && current_order.target_unit_tag != assimilator->tag)
                    {
                        Actions()->UnitCommand(worker, ABILITY_ID::SMART, assimilator);
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            else if (IsCarryingVespene(*worker))
            {
                // close to nexus then return the vespene
                Point2D closest_nexus = ClosestTo(Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)), worker->pos)->pos;
                if (DistanceSquared2D(closest_nexus, worker->pos) < 10 || CloserThan(workers, .75, worker->pos).size() > 1)
                {
                    Actions()->UnitCommand(worker, ABILITY_ID::HARVEST_RETURN_PROBE);
                    continue;
                }
                Point2D drop_off_point = assimilators_reversed[worker].drop_off_point;
                // don't get bugged out trying to move to the drop off point
                if (worker->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE && DistanceSquared2D(worker->pos, drop_off_point) < 1)
                {
                    continue;
                }
                // otherwise move to the drop off point
                const Unit *assimilator = assimilators_reversed[worker].assimilator_tag;
                if (assimilator != NULL)
                {
                    Actions()->UnitCommand(worker, ABILITY_ID::GENERAL_MOVE, drop_off_point);
                }
                else
                {
                    std::cout << "Error null assimilator in DistributeWorkers 3";
                }
            }
            else
            {
                const Unit *assimilator = assimilators_reversed[worker].assimilator_tag;
                if (assimilator != NULL)
                {
                    if (DistanceSquared2D(worker->pos, assimilator->pos) < 4 || CloserThan(workers, .75, worker->pos).size() > 1)
                    {
                        if (worker->orders.size() == 0 || worker->orders[0].target_unit_tag != assimilator->tag)
                        {
                            Actions()->UnitCommand(worker, ABILITY_ID::HARVEST_GATHER_PROBE, assimilator);
                        }
                    }
                    else if (worker->orders.size() == 0 || worker->orders[0].ability_id != ABILITY_ID::GENERAL_MOVE || DistanceSquared2D(assimilators_reversed[worker].pick_up_point, worker->orders[0].target_pos) > 1)
                    {
                        Actions()->UnitCommand(worker, ABILITY_ID::GENERAL_MOVE, assimilators_reversed[worker].pick_up_point);
                    }
                }
                else
                {
                    std::cout << "Error null assimilator in DistributeWorkers 4";
                }
            }
        }
    }

    void TossBot::BalanceWorers()
    {
        while (first_2_mineral_patch_spaces.size() > 0 && (close_3_mineral_patch_extras.size() > 0 || far_3_mineral_patch_extras.size() > 0))
        {
            if (close_3_mineral_patch_extras.size() > 0)
            {
                const Unit* worker = *(close_3_mineral_patch_extras[0]->worker);
                RemoveWorker(worker);
                PlaceWorker(worker);
            }
            else if (far_3_mineral_patch_extras.size() > 0)
            {
                const Unit* worker = *(far_3_mineral_patch_extras[0]->worker);
                RemoveWorker(worker);
                PlaceWorker(worker);
            }
        }
        while (gas_spaces.size() > 0 && (close_3_mineral_patch_extras.size() > 0 || far_3_mineral_patch_extras.size() > 0))
        {
            if (close_3_mineral_patch_extras.size() > 0)
            {
                const Unit* worker = *(close_3_mineral_patch_extras[0]->worker);
                RemoveWorker(worker);
                PlaceWorker(worker);
            }
            else if (far_3_mineral_patch_extras.size() > 0)
            {
                const Unit* worker = *(far_3_mineral_patch_extras[0]->worker);
                RemoveWorker(worker);
                PlaceWorker(worker);
            }
        }
        while (far_3_mineral_patch_spaces.size() > 0 && close_3_mineral_patch_extras.size() > 0)
        {
            const Unit* worker = *(close_3_mineral_patch_extras[0]->worker);
            RemoveWorker(worker);
            PlaceWorker(worker);
        }
    }

    void TossBot::BuildWorkers()
    {
        if (should_build_workers)
        {
            for (const auto &nexus : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
            {
                bool probe_queued = false;
                for (const auto &order : nexus->orders)
                {
                    if (order.ability_id == ABILITY_ID::TRAIN_PROBE && order.progress == 0)
                    {
                        probe_queued = true;
                        break;
                    }
                }
                if (probe_queued)
                {
                    Actions()->UnitCommand(nexus, ABILITY_ID::CANCELSLOT_QUEUEPASSIVE);
                }
                if (nexus->orders.empty())
                {
                    Actions()->UnitCommand(nexus, ABILITY_ID::TRAIN_PROBE);
                }
            }
        }
    }

    Point2D TossBot::Center(Units units)
    {
        if (units.size() == 0)
            return Point2D(0, 0);

        Point2D total = Point2D(0, 0);
        for (const auto &unit : units)
        {
            total += unit->pos;
        }
        return total / units.size();
    }

    Point2D TossBot::MedianCenter(Units units)
    {
        if (units.size() == 0)
            return Point2D(0, 0);
        Point2D guess = Center(units);
        bool done = false;
        std::vector<Point2D> tests = { Point2D(0, 1), Point2D(1, 0), Point2D(0, -1), Point2D(-1, 0) };
        float distance = 0;
        for (const auto &unit : units)
        {
            distance += Distance2D(unit->pos, guess);
        }

        while (!done)
        {
            done = true;
            for (const auto &test : tests)
            {
                Point2D new_guess = guess + test;
                float new_distance = 0;
                for (const auto &unit : units)
                {
                    new_distance += Distance2D(unit->pos, new_guess);
                }
                if (new_distance < distance)
                {
                    guess = new_guess;
                    distance = new_distance;
                    done = false;
                    break;
                }
            }
        }
        return guess;
    }

    Point2D TossBot::PointBetween(Point2D start, Point2D end, float dist)
    {
        float total_dist = Distance2D(start, end);
        if (total_dist == 0)
        {
            std::cout << "Warning PointBetween called on the same point";
            return start;
        }
        return Point2D(start.x + dist * (end.x - start.x) / total_dist, start.y + dist * (end.y - start.y) / total_dist);
    }

    int TossBot::DangerLevel(const Unit* unit)
    {
        int possible_damage = 0;
        for (const auto &enemy_unit : Observation()->GetUnits(Unit::Alliance::Enemy))
        {
            if (!enemy_unit->is_building && Distance2D(unit->pos, enemy_unit->pos) <= RealGroundRange(enemy_unit, unit))
                possible_damage += GetDamage(enemy_unit);
        }
        return possible_damage;
    }

    int TossBot::GetDamage(const Unit* unit)
    {
        switch(unit->unit_type.ToType())
        {
        case UNIT_TYPEID::ZERG_ROACH:
            return 16;
        case UNIT_TYPEID::TERRAN_MARINE:
            return 6;
        case UNIT_TYPEID::TERRAN_MARAUDER:
            return 10;
        default:
            std::cout << "Error invalid unit type in GetDamage\n";
            return 0;
        }
    }

    float TossBot::RealGroundRange(const Unit* attacking_unit, const Unit * target)
    {
        float range = attacking_unit->radius + target->radius;
        switch (attacking_unit->unit_type.ToType())
        {
        case UNIT_TYPEID::PROTOSS_STALKER:
            range += 6;
            break;
        case UNIT_TYPEID::PROTOSS_IMMORTAL:
            range += 6;
            break;
        case UNIT_TYPEID::PROTOSS_ADEPT:
            range += 4;
            break;
        case UNIT_TYPEID::TERRAN_SCV:
            range += 0;
            break;
        case UNIT_TYPEID::TERRAN_MARINE:
            range += 5;
            break;
        case UNIT_TYPEID::TERRAN_MARAUDER:
            range += 6;
            break;
        default:
            std::cout << "Error invalid unit type in RealGroundRange\n";
            return 0;
        }
        return range;
    }

    bool TossBot::IsOnHighGround(Point3D unit, Point3D enemy_unit)
    {
        return unit.z + .5 < enemy_unit.z;
    }

    float TossBot::GetTimeBuilt(const Unit* unit)
    {
        float build_time = 0;
        switch (unit->unit_type.ToType())
        {
        case UNIT_TYPEID::ZERG_HATCHERY:
            build_time = 71;
            break;
        case UNIT_TYPEID::ZERG_EXTRACTOR:
            build_time = 21;
            break;
        case UNIT_TYPEID::ZERG_SPAWNINGPOOL:
            build_time = 46;
            break;
        case UNIT_TYPEID::ZERG_ROACHWARREN:
            build_time = 39;
            break;
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:
            build_time = 71;
            break;
        case UNIT_TYPEID::TERRAN_REFINERY:
            build_time = 30;
            break;
        case UNIT_TYPEID::TERRAN_BARRACKS:
            build_time = 46;
            break;
        case UNIT_TYPEID::TERRAN_FACTORY:
            build_time = 43;
            break;
        default:
            std::cout << "Error Unknown building in GetTimeBuilt\n";
            break;
        }
        return (Observation()->GetGameLoop() / 22.4) - (build_time * unit->build_progress);
    }

    AbilityID TossBot::UnitToWarpInAbility(UNIT_TYPEID type)
    {
        switch (type)
        {
        case UNIT_TYPEID::PROTOSS_ZEALOT:
            return ABILITY_ID::TRAINWARP_ZEALOT;
        case UNIT_TYPEID::PROTOSS_STALKER:
            return ABILITY_ID::TRAINWARP_STALKER;
        case UNIT_TYPEID::PROTOSS_ADEPT:
            return ABILITY_ID::TRAINWARP_ADEPT;
        case UNIT_TYPEID::PROTOSS_SENTRY:
            return ABILITY_ID::TRAINWARP_SENTRY;
        case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
            return ABILITY_ID::TRAINWARP_HIGHTEMPLAR;
        case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
            return ABILITY_ID::TRAINWARP_DARKTEMPLAR;
        default:
            return NULL;
        }
    }

#pragma endregion


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

#pragma region Actions

    void TossBot::ProcessActions()
    {
        for (int i = 0; i < active_actions.size(); i++)
        {
            bool(sc2::TossBot::*action)(ActionArgData*) = active_actions[i]->action;
            if ((*this.*action)(active_actions[i]->action_arg))
            {
                active_actions.erase(active_actions.begin() + i);
                i--;
            }

        }
    }

    bool TossBot::ActionBuildBuilding(ActionArgData* data)
    {
        UNIT_TYPEID buildingId = data->unitId;
        Point2D pos = data->position;
        const Unit *builder = data->unit;
        for (const auto &building : Observation()->GetUnits(IsUnit(buildingId)))
        {
            if (Distance2D(Point2D(building->pos), pos) < 1 && building->display_type != Unit::DisplayType::Placeholder)
            {
                PlaceWorker(builder);
                // finished buildings.remove building.tag
                return true;
            }
        }
        if (Distance2D(builder->pos, pos) < BuildingSize(buildingId) + 1 && CanBuildBuilding(buildingId))
        {
            if (buildingId == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
            {
                std::vector<UNIT_TYPEID> gas_types = { UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER };
                const Unit *gas = ClosestTo(Observation()->GetUnits(IsUnits(gas_types)), pos);
                Actions()->UnitCommand(builder, ABILITY_ID::BUILD_ASSIMILATOR, gas);
            }
            else
            {
                Actions()->UnitCommand(builder, GetBuildAbility(buildingId), pos);
            }
        }
        else if (Distance2D(builder->pos, pos) > BuildingSize(buildingId))
        {
            Actions()->UnitCommand(builder, ABILITY_ID::MOVE_MOVE, pos);
        }
        return false;
    }

    bool TossBot::ActionBuildBuildingMulti(ActionArgData* data)
    {
        UNIT_TYPEID buildingId = data->unitIds[data->index];
        Point2D pos = data->position;
        const Unit *builder = data->unit;
        for (const auto &building : Observation()->GetUnits(IsUnit(buildingId)))
        {
            if (Point2D(building->pos) == pos && building->display_type != Unit::DisplayType::Placeholder)
            {
                // finished buildings.remove building.tag
                data->index++;
                if (data->index < data->unitIds.size())
                {
                    data->position = GetLocation(data->unitIds[data->index]);
                    active_actions.push_back(new ActionData(&TossBot::ActionBuildBuildingMulti, data));
                }
                else
                {
                    PlaceWorker(builder);
                }
                return true;
            }
        }
        if (Distance2D(builder->pos, pos) < BuildingSize(buildingId) + 1 && CanBuildBuilding(buildingId))
        {
            if (buildingId == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
            {
                std::vector<UNIT_TYPEID> gas_types = { UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER };
                const Unit *gas = ClosestTo(Observation()->GetUnits(IsUnits(gas_types)), pos);
                Actions()->UnitCommand(builder, ABILITY_ID::BUILD_ASSIMILATOR, gas);
            }
            else
            {
                Actions()->UnitCommand(builder, GetBuildAbility(buildingId), pos);
            }
        }
        else if (Distance2D(builder->pos, pos) > BuildingSize(buildingId))
        {
            Actions()->UnitCommand(builder, ABILITY_ID::GENERAL_MOVE, pos);
        }
        return false;
    }

    bool TossBot::ActionScoutZerg(ActionArgData* data)
    {
        return true;
    }

    bool TossBot::ActionContinueMakingWorkers(ActionArgData* data)
    {
        int num_workers = Observation()->GetFoodWorkers();
        int num_nexi = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)).size();
        int num_assimilators = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ASSIMILATOR)).size();
        if (num_workers >= std::min(num_nexi * 16 + num_assimilators * 3, 70))
        {
            should_build_workers = false;
        }
        else
        {
            should_build_workers = true;
        }
        return false;
    }

    bool TossBot::ActionContinueBuildingPylons(ActionArgData* data)
    {
        int build_pylon_actions = 0;
        for (const auto &action : active_actions)
        {
            if (action->action == &TossBot::ActionBuildBuilding && action->action_arg->unitId == UNIT_TYPEID::PROTOSS_PYLON)
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
        int supply_used = Observation()->GetFoodUsed();
        int supply_cap = Observation()->GetFoodCap() - 8 * extra_pylons;
        supply_cap += 8 * (build_pylon_actions + pending_pylons);
        supply_used += Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)).size();
        supply_used += 2 * Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_WARPGATE)).size();
        supply_used += 2 * Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY)).size();
        supply_used += 3 * Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)).size();
        supply_used += 3 * Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_STARGATE)).size();
        if (supply_used >= supply_cap)
            BuildBuilding(UNIT_TYPEID::PROTOSS_PYLON);
        
        return false;
    }

    bool TossBot::ActionChronoTillFinished(ActionArgData* data)
    {
        const Unit* building = data->unit;
        if (building->orders.size() == 0)
        {
            return true;
        }
        for (const auto &buff : building->buffs)
        {
            if (buff == BUFF_ID::CHRONOBOOSTENERGYCOST)
                return false;
        }
        for (const auto &nexus : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
        {
            for (const auto &ability : Query()->GetAbilitiesForUnit(nexus).abilities)
            {
                if (ability.ability_id == ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST)
                {
                    Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building);
                    return false;
                }
            }
        }
        return false;
    }

    bool TossBot::ActionConstantChrono(ActionArgData* data)
    {
        const Unit* building = data->unit;
        for (const auto &buff : building->buffs)
        {
            if (buff == BUFF_ID::CHRONOBOOSTENERGYCOST)
                return false;
        }
    }

    bool TossBot::ActionWarpInAtProxy(ActionArgData* data)
    {
        std::vector<Point2D> possible_spots = FindProxyWarpInSpots();
        if (possible_spots.size() == 0)
            return false;
        for (const auto &warpgate : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE)))
        {
            for (const auto &ability : Query()->GetAbilitiesForUnit(warpgate).abilities)
            {
                if (ability.ability_id == ABILITY_ID::TRAINWARP_ZEALOT)
                {
                    if (CanAfford(UNIT_TYPEID::PROTOSS_STALKER, 1))
                    {
                        Actions()->UnitCommand(warpgate, ABILITY_ID::TRAINWARP_STALKER, possible_spots.back());
                        possible_spots.pop_back();
                        break;
                    }
                }
            }
            if (possible_spots.size() == 0)
                break;
        }
        return false;
    }

    bool TossBot::ActionTrainFromProxyRobo(ActionArgData* data)
    {
        const Unit * robo = data->unit;
        int num_prisms = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_WARPPRISM)).size();
        int num_obs = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_OBSERVER)).size();

        if (robo->build_progress == 1 && robo->orders.size() == 0)
        {
            if (num_prisms == 0 && CanAfford(UNIT_TYPEID::PROTOSS_WARPPRISM, 1))
                Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_WARPPRISM);
            else if (num_obs == 0 && CanAfford(UNIT_TYPEID::PROTOSS_OBSERVER, 1))
                Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_OBSERVER);
            else if (CanAfford(UNIT_TYPEID::PROTOSS_IMMORTAL, 1))
                Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_IMMORTAL);
        }
        return false;
    }

    bool TossBot::ActionContain(ActionArgData* data)
    {
        Army* army = data->army;
        Point2D retreat_point = army->attack_path[army->current_attack_index - 2];

        Point2D attack_point = army->attack_path[army->current_attack_index];

        Units prisms = TagsToUnits(army->prism_ids);
        Units stalkers = TagsToUnits(army->stalker_ids);
        Units observers = TagsToUnits(army->observer_ids);
        Units immortals = TagsToUnits(army->immortal_ids);


        bool obs_in_position = false;
        if (stalkers.size() > 0)
        {
            if (observers.size() > 0)
            {
                if (DistanceToClosest(observers, attack_point) < 10)
                    obs_in_position = true;
                ObserveAttackPath(observers, retreat_point, attack_point);
            }
            if (prisms.size() > 0)
            {
                StalkerAttackTowardsWithPrism(stalkers, prisms, retreat_point, attack_point, obs_in_position);
                if (immortals.size() > 0)
                    ImmortalAttackTowardsWithPrism(immortals, prisms, retreat_point, attack_point, obs_in_position);
            }
            else
            {
                StalkerAttackTowards(stalkers, retreat_point, attack_point, obs_in_position);
                if (immortals.size() > 0)
                    ImmortalAttackTowards(immortals, retreat_point, attack_point, obs_in_position);
            }
        }

        if (army->current_attack_index > 2 && Distance2D(Center(stalkers), retreat_point) < 3)
            army->current_attack_index--;
        if (army->current_attack_index < army->attack_path.size() - 1 && Distance2D(MedianCenter(stalkers), attack_point) < 3)
        {
            if (obs_in_position)
                army->current_attack_index++;
            else
                army->current_attack_index = std::min(army->current_attack_index + 1, army->high_ground_index - 1);
        }

        return false;
    }

    bool TossBot::ActionStalkerOraclePressure(ActionArgData* data)
    {
        Army* army = data->army;
        Point2D retreat_point = army->attack_path[army->current_attack_index - 2];

        Point2D attack_point = army->attack_path[army->current_attack_index];

        Units prisms = TagsToUnits(army->prism_ids);
        Units stalkers = TagsToUnits(army->stalker_ids);
        Units observers = TagsToUnits(army->observer_ids);
        Units oracles = TagsToUnits(army->oracle_ids);



        bool obs_in_position = false;
        if (stalkers.size() > 0)
        {
            if (observers.size() > 0)
            {
                if (DistanceToClosest(observers, attack_point) < 10)
                    obs_in_position = true;
                ObserveAttackPath(observers, retreat_point, attack_point);
            }
            if (prisms.size() > 0)
            {
                StalkerAttackTowardsWithPrism(stalkers, prisms, retreat_point, attack_point, obs_in_position);
            }
            else
            {
                StalkerAttackTowards(stalkers, retreat_point, attack_point, obs_in_position);
            }
            if (oracles.size() > 0)
            {
                OraclesCoverStalkers(stalkers, oracles);
            }
        }

        if (army->current_attack_index > 2 && Distance2D(Center(stalkers), retreat_point) < 3)
            army->current_attack_index--;
        if (army->current_attack_index < army->attack_path.size() - 1 && Distance2D(MedianCenter(stalkers), attack_point) < 3)
        {
            if (obs_in_position)
                army->current_attack_index++;
            else
                army->current_attack_index = std::min(army->current_attack_index + 1, army->high_ground_index - 1);
        }

        return false;
    }

    bool TossBot::ActionContinueWarpingInStalkers(ActionArgData* data)
    {
        bool all_gates_ready = true;
        Units gates = Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
        for (const auto &warpgate : gates)
        {
            bool gate_ready = false;
            for (const auto &ability : Query()->GetAbilitiesForUnit(warpgate).abilities)
            {
                if (ability.ability_id == ABILITY_ID::TRAINWARP_STALKER)
                {
                    gate_ready = true;
                    break;
                }
            }
            if (!gate_ready)
            {
                all_gates_ready = false;
                break;
            }
        }
        if (all_gates_ready && CanAfford(UNIT_TYPEID::PROTOSS_STALKER, gates.size()))
        {
            std::vector<Point2D> spots = FindWarpInSpots(Observation()->GetGameInfo().enemy_start_locations[0]);
            if (spots.size() >= gates.size())
            {
                for (int i = 0; i < gates.size(); i++)
                {
                    Actions()->UnitCommand(gates[i], ABILITY_ID::TRAINWARP_STALKER, spots[i]);
                }
            }
        }
        return false;
    }

    bool TossBot::ActionContinueWarpingInZealots(ActionArgData* data)
    {
        bool all_gates_ready = true;
        //if (Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_GATEWAY)).size() > 0)
        //    return false;
        Units gates = Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
        for (const auto &warpgate : gates)
        {
            bool gate_ready = false;
            for (const auto &ability : Query()->GetAbilitiesForUnit(warpgate).abilities)
            {
                if (ability.ability_id == ABILITY_ID::TRAINWARP_ZEALOT)
                {
                    gate_ready = true;
                    break;
                }
            }
            if (!gate_ready)
            {
                all_gates_ready = false;
                break;
            }
        }
        if (gates.size() > 0 && all_gates_ready && CanAfford(UNIT_TYPEID::PROTOSS_ZEALOT, gates.size()))
        {
            std::vector<Point2D> spots = FindWarpInSpots(Observation()->GetGameInfo().enemy_start_locations[0]);
            if (spots.size() >= gates.size())
            {
                for (int i = 0; i < gates.size(); i++)
                {
                    Point3D pos = Point3D(gates[i]->pos.x, gates[i]->pos.y, Observation()->TerrainHeight(gates[i]->pos));
                    Debug()->DebugSphereOut(pos, 1, Color(255, 0, 255));
                    Actions()->UnitCommand(gates[i], ABILITY_ID::TRAINWARP_ZEALOT, spots[i]);
                }
            }
        }
        return false;
    }

    bool TossBot::ActionPullOutOfGas(ActionArgData* data)
    {
        Units workers;
        for (const auto &data : assimilators_reversed)
        {
            workers.push_back(data.first);
        }

        for (const auto &worker : workers)
        {
            RemoveWorker(worker);
            PlaceWorker(worker);
        }
        if (assimilators_reversed.size() == 0)
            return true;
    }

    bool TossBot::ActionRemoveScoutToProxy(ActionArgData* data)
    {
        bool pylon_placed = false;
        for (const auto &pylon : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_PYLON)))
        {
            if (Distance2D(pylon->pos, data->position) < 1)
            {
                pylon_placed = true;
                break;
            }
        }
        int build_time = data->index;
        const Unit* scout = data->unit;

        if (Distance2D(scout->pos, data->position) > 1 && !pylon_placed)
        {
            Actions()->UnitCommand(scout, ABILITY_ID::MOVE_MOVE, data->position);
        }
        else if (Distance2D(scout->pos, data->position) < 1 && !pylon_placed && Observation()->GetGameLoop() / 22.4 >= data->index)
        {
            Actions()->UnitCommand(scout, ABILITY_ID::BUILD_PYLON, data->position);
        }
        else if (pylon_placed)
        {
            std::vector<Point2D> building_locations = GetProxyLocations(data->unitId);
            Point2D pos = building_locations[0];
            active_actions.push_back(new ActionData(&TossBot::ActionBuildBuilding, new ActionArgData(scout, data->unitId, pos)));
            return true;
        }
        return false;
    }

    bool TossBot::ActionDTHarassTerran(ActionArgData* data)
    {
        for (const auto &unit : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_DARKTEMPLAR)))
        {
            // if outside -> move into enemy main
            if ((unit->pos.z + .1 < Observation()->GetStartLocation().z || unit->pos.z - .1 > Observation()->GetStartLocation().z) && unit->orders.size() == 0)
            {
                Actions()->UnitCommand(unit, ABILITY_ID::MOVE_MOVE, locations->initial_scout_pos);
                Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_SHADOWSTRIDE, PointBetween(locations->initial_scout_pos, Observation()->GetGameInfo().enemy_start_locations[0], 7), true);
                continue;
            }
            // avoid scans
            // avoid ravens
            // spread out

            // target prio
            // repairing scvs
            // tech labs bulding on starports
            // tech labs on idle starports
            // active starports with tech labs
            // main orbital
            // mules
            // scvs
            // army units
        }
        return false;
    }




#pragma endregion

#pragma region Build

    void TossBot::CheckBuildOrder()
    {
        if (build_order_step < build_order.size())
        {
            BuildOrderData current_step = build_order[build_order_step];
            bool(sc2::TossBot::*condition)(BuildOrderConditionArgData) = current_step.condition;
            BuildOrderConditionArgData condition_arg = current_step.condition_arg;
            if ((*this.*condition)(condition_arg))
            {
                bool(sc2::TossBot::*result)(BuildOrderResultArgData) = current_step.result;
                BuildOrderResultArgData result_arg = current_step.result_arg;
                if ((*this.*result)(result_arg))
                {
                    build_order_step++;
                }
            }
        }
    }

    bool TossBot::TimePassed(BuildOrderConditionArgData data)
    {
        return Observation()->GetGameLoop() / 22.4 >= data.time;
    }

    bool TossBot::NumWorkers(BuildOrderConditionArgData data)
    {
        return Observation()->GetFoodWorkers() >= data.amount;
    }

    bool TossBot::HasBuilding(BuildOrderConditionArgData data)
    {
        for (const auto &building : Observation()->GetUnits(IsUnit(data.unitId)))
        {
            if (building->build_progress == 1)
            {
                return true;
            }
        }
        return false;
    }

    bool TossBot::IsResearching(BuildOrderConditionArgData data)
    {
        for (const auto &building : Observation()->GetUnits(IsUnit(data.unitId)))
        {
            if (!building->orders.empty())
                return true;
        }
        return false;
    }

    bool TossBot::HasGas(BuildOrderConditionArgData data)
    {
        return Observation()->GetVespene() >= data.amount;
    }



    bool TossBot::BuildBuilding(BuildOrderResultArgData data)
    {
        Point2D pos = GetLocation(data.unitId);
        const Unit* builder = GetBuilder(pos);
        if (builder == NULL)
        {
            std::cout << "Error could not find builder in BuildBuilding" << std::endl;
            return false;
        }
        RemoveWorker(builder);
        active_actions.push_back(new ActionData(&TossBot::ActionBuildBuilding, new ActionArgData(builder, data.unitId, pos)));
        return true;
    }

    bool TossBot::BuildFirstPylon(BuildOrderResultArgData data)
    {
        Point2D pos;
        switch (enemy_race)
        {
        case Race::Zerg:
            pos = locations->first_pylon_location_zerg;
            break;
        case Race::Protoss:
            pos = locations->first_pylon_location_protoss;
            break;
        case Race::Terran:
            pos = locations->first_pylon_location_terran;
            break;
        }
        const Unit* builder = GetBuilder(pos);
        if (builder == NULL)
        {
            std::cout << "Error could not find builder in BuildBuilding" << std::endl;
            return false;
        }
        RemoveWorker(builder);
        active_actions.push_back(new ActionData(&TossBot::ActionBuildBuilding, new ActionArgData(builder, data.unitId, pos)));
        return true;
    }

    bool TossBot::BuildBuildingMulti(BuildOrderResultArgData data)
    {
        Point2D pos = GetLocation(data.unitIds[0]);
        const Unit* builder = GetBuilder(pos);
        if (builder == NULL)
        {
            std::cout << "Error could not find builder in BuildBuilding" << std::endl;
            return false;
        }
        RemoveWorker(builder);
        active_actions.push_back(new ActionData(&TossBot::ActionBuildBuildingMulti, new ActionArgData(builder, data.unitIds, pos, 0)));
        return true;
    }

    bool TossBot::Scout(BuildOrderResultArgData data)
    {
        Point2D pos = Observation()->GetGameInfo().enemy_start_locations[0];
        const Unit* scouter = GetBuilder(pos);
        if (scouter == NULL)
        {
            std::cout << "Error could not find builder in Scout" << std::endl;
            return false;
        }
        RemoveWorker(scouter);
        if (enemy_race == Race::Zerg)
        {
            ScoutZergStateMachine* scout_fsm = new ScoutZergStateMachine(this, "Scout Zerg", scouter, locations->initial_scout_pos, locations->main_scout_path, locations->natural_scout_path, locations->enemy_natural, locations->possible_3rds);
            active_FSMs.push_back(scout_fsm);
        }
        else
        {
            ScoutTerranStateMachine* scout_fsm = new ScoutTerranStateMachine(this, "Scout Terran", scouter, locations->initial_scout_pos, locations->main_scout_path, locations->natural_scout_path, locations->enemy_natural);
            active_FSMs.push_back(scout_fsm);
        }
        return true;
    }

    bool TossBot::CutWorkers(BuildOrderResultArgData data)
    {
        should_build_workers = false;
        return true;
    }

    bool TossBot::UncutWorkers(BuildOrderResultArgData data)
    {
        should_build_workers = true;
        return true;
    }

    bool TossBot::ImmediatelySaturateGasses(BuildOrderResultArgData data)
    {
        immediatelySaturateGasses = true;
        return true;
    }

    bool TossBot::TrainStalker(BuildOrderResultArgData data)
    {
        if (CanAfford(UNIT_TYPEID::PROTOSS_STALKER, 1))
        {
            for (const auto &gateway : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY)))
            {
                if (gateway->build_progress == 1 && gateway->orders.size() == 0)
                {
                    for (const auto & ability : Query()->GetAbilitiesForUnit(gateway).abilities)
                    {
                        if (ability.ability_id.ToType() == ABILITY_ID::TRAIN_STALKER)
                        {
                            Actions()->UnitCommand(gateway, ABILITY_ID::TRAIN_STALKER);
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool TossBot::TrainAdept(BuildOrderResultArgData data)
    {
        if (CanAfford(UNIT_TYPEID::PROTOSS_ADEPT, 1))
        {
            for (const auto &gateway : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY)))
            {
                if (gateway->build_progress == 1 && gateway->orders.size() == 0)
                {
                    for (const auto & ability : Query()->GetAbilitiesForUnit(gateway).abilities)
                    {
                        if (ability.ability_id.ToType() == ABILITY_ID::TRAIN_ADEPT)
                        {
                            Actions()->UnitCommand(gateway, ABILITY_ID::TRAIN_ADEPT);
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool TossBot::TrainOracle(BuildOrderResultArgData data)
    {
        if (CanAfford(UNIT_TYPEID::PROTOSS_ORACLE, 1))
        {
            for (const auto &stargate : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_STARGATE)))
            {
                if (stargate->build_progress == 1 && stargate->orders.size() == 0)
                {
                    for (const auto & ability : Query()->GetAbilitiesForUnit(stargate).abilities)
                    {
                        if (ability.ability_id.ToType() == ABILITY_ID::TRAIN_ORACLE)
                        {
                            Actions()->UnitCommand(stargate, ABILITY_ID::TRAIN_ORACLE);
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool TossBot::TrainPrism(BuildOrderResultArgData data)
    {
        if (CanAfford(UNIT_TYPEID::PROTOSS_WARPPRISM, 1))
        {
            for (const auto &robo : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
            {
                if (robo->build_progress == 1 && robo->orders.size() == 0)
                {
                    for (const auto & ability : Query()->GetAbilitiesForUnit(robo).abilities)
                    {
                        if (ability.ability_id.ToType() == ABILITY_ID::TRAIN_WARPPRISM)
                        {
                            Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_WARPPRISM);
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool TossBot::ChronoBuilding(BuildOrderResultArgData data)
    {
        for (const auto &building : Observation()->GetUnits(IsUnit(data.unitId)))
        {
            if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
            {
                for (const auto &nexus : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
                {
                    for (const auto &ability : Query()->GetAbilitiesForUnit(nexus).abilities)
                    {
                        if (ability.ability_id == ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST)
                        {
                            Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building);
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool TossBot::ResearchWarpgate(BuildOrderResultArgData data)
    {
        for (const auto &cyber : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)))
        {
            if (CanAffordUpgrade(UPGRADE_ID::WARPGATERESEARCH))
            {
                Actions()->UnitCommand(cyber, ABILITY_ID::RESEARCH_WARPGATE);
                return true;
            }
        }
        return false;
    }

    bool TossBot::ResearchBlink(BuildOrderResultArgData data)
    {
        for (const auto &twilight : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)))
        {
            if (CanAffordUpgrade(UPGRADE_ID::BLINKTECH) && twilight->orders.size() == 0)
            {
                Actions()->UnitCommand(twilight, ABILITY_ID::RESEARCH_BLINK);
                return true;
            }
        }
        return false;
    }

    bool TossBot::ResearchCharge(BuildOrderResultArgData data)
    {
        for (const auto &twilight : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)))
        {
            if (CanAffordUpgrade(UPGRADE_ID::CHARGE) && twilight->orders.size() == 0)
            {
                Actions()->UnitCommand(twilight, ABILITY_ID::RESEARCH_CHARGE);
                return true;
            }
        }
        return false;
    }

    bool TossBot::ResearchGlaives(BuildOrderResultArgData data)
    {
        for (const auto &twilight : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)))
        {
            if (CanAffordUpgrade(UPGRADE_ID::ADEPTPIERCINGATTACK) && twilight->orders.size() == 0)
            {
                Actions()->UnitCommand(twilight, ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES);
                return true;
            }
        }
        return false;
    }

    bool TossBot::ResearchDTBlink(BuildOrderResultArgData data)
    {
        for (const auto &dark_shrine : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_DARKSHRINE)))
        {
            if (CanAffordUpgrade(UPGRADE_ID::DARKTEMPLARBLINKUPGRADE) && dark_shrine->orders.size() == 0)
            {
                Actions()->UnitCommand(dark_shrine, ABILITY_ID::RESEARCH_SHADOWSTRIKE);
                return true;
            }
        }
        return false;
    }

    bool TossBot::ResearchAttackOne(BuildOrderResultArgData data)
    {
        for (const auto &forge : Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_FORGE)))
        {
            if (CanAffordUpgrade(UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1))
            {
                Actions()->UnitCommand(forge, ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1);
                return true;
            }
        }
        return false;
    }

    bool TossBot::ChronoTillFinished(BuildOrderResultArgData data)
    {
        for (const auto &building : Observation()->GetUnits(IsFinishedUnit(data.unitId)))
        {
            if (building->orders.size() > 0)
            {
                active_actions.push_back(new ActionData(&TossBot::ActionChronoTillFinished, new ActionArgData(building, data.unitId)));
                return true;
            }
        }
        return false;
    }

    bool TossBot::WarpInAtProxy(BuildOrderResultArgData data)
    {
        active_actions.push_back(new ActionData(&TossBot::ActionWarpInAtProxy, new ActionArgData()));
        return true;
    }

    bool TossBot::BuildProxy(BuildOrderResultArgData data)
    {
        std::vector<Point2D> building_locations = GetProxyLocations(data.unitId);
        
        Point2D pos = building_locations[0];
        const Unit* builder = GetBuilder(pos);
        if (builder == NULL)
        {
            std::cout << "Error could not find builder in BuildBuilding" << std::endl;
            return false;
        }
        RemoveWorker(builder);
        active_actions.push_back(new ActionData(&TossBot::ActionBuildBuilding, new ActionArgData(builder, data.unitId, pos)));
        return true;
    }

    bool TossBot::ContinueBuildingPylons(BuildOrderResultArgData data)
    {
        active_actions.push_back(new ActionData(&TossBot::ActionContinueBuildingPylons, new ActionArgData()));
        return true;
    }

    bool TossBot::ContinueMakingWorkers(BuildOrderResultArgData data)
    {
        active_actions.push_back(new ActionData(&TossBot::ActionContinueMakingWorkers, new ActionArgData()));
        return true;
    }

    bool TossBot::TrainFromProxy(BuildOrderResultArgData data)
    {
        if (data.unitId == UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)
        {
            std::vector<const Unit*> proxy_robos;
            for (const auto &robo : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
            {
                if (DistanceToClosest(GetProxyLocations(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY), robo->pos) < 2)
                {
                    active_actions.push_back(new ActionData(&TossBot::ActionTrainFromProxyRobo, new ActionArgData(robo)));
                    return true;
                }
            }
        }
        return false;
    }

    bool TossBot::ContinueChronoProxyRobo(BuildOrderResultArgData data)
    {
        if (data.unitId == UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)
        {
            std::vector<const Unit*> proxy_robos;
            for (const auto &robo : Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
            {
                if (DistanceToClosest(GetProxyLocations(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY), robo->pos) < 2)
                {
                    active_actions.push_back(new ActionData(&TossBot::ActionConstantChrono, new ActionArgData(robo)));
                    return true;
                }
            }
        }
        return false;
    }

    bool TossBot::Contain(BuildOrderResultArgData data)
    {
        Army* army = new Army(locations->attack_path, locations->high_ground_index);
        army_groups.push_back(army);
        active_actions.push_back(new ActionData(&TossBot::ActionContain, new ActionArgData(army)));
        return true;
    }

    bool TossBot::StalkerOraclePressure(BuildOrderResultArgData data)
    {
        Army* army = new Army(locations->attack_path, locations->high_ground_index);
        army_groups.push_back(army);
        active_actions.push_back(new ActionData(&TossBot::ActionStalkerOraclePressure, new ActionArgData(army)));
        return true;
    }

    bool TossBot::MicroOracles(BuildOrderResultArgData data)
    {
        StateMachine* oracle_fsm = new StateMachine(this, new OracleDefend(this, Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ORACLE)), GetLocations(UNIT_TYPEID::PROTOSS_NEXUS)[2]), "Oracles");
        active_FSMs.push_back(oracle_fsm);
        return true;
    }

    bool TossBot::SpawnUnits(BuildOrderResultArgData data)
    {
        Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ZERGLING, GetLocations(UNIT_TYPEID::PROTOSS_NEXUS)[2], 2, 15);
        return true;
    }

    bool TossBot::ContinueWarpingInStalkers(BuildOrderResultArgData data)
    {
        active_actions.push_back(new ActionData(&TossBot::ActionContinueWarpingInStalkers, new ActionArgData()));
        return true;
    }

    bool TossBot::ContinueWarpingInZealots(BuildOrderResultArgData data)
    {
        active_actions.push_back(new ActionData(&TossBot::ActionContinueWarpingInZealots, new ActionArgData()));
        return true;
    }

    bool TossBot::WarpInUnits(BuildOrderResultArgData data)
    {
        int warp_ins = data.amount;
        UnitTypeID type = data.unitId;
        AbilityID warp_ability = UnitToWarpInAbility(type);
        int gates_ready = 0;
        Units gates = Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
        for (const auto &warpgate : gates)
        {
            for (const auto &ability : Query()->GetAbilitiesForUnit(warpgate).abilities)
            {
                if (ability.ability_id == warp_ability)
                {
                    gates_ready++;
                    break;
                }
            }
            if (gates_ready >= warp_ins)
            {
                break;
            }
        }
        if (gates.size() > 0 && gates_ready >= warp_ins && CanAfford(type, warp_ins))
        {
            std::vector<Point2D> spots = FindWarpInSpots(Observation()->GetGameInfo().enemy_start_locations[0]);
            if (spots.size() >= warp_ins)
            {
                for (int i = 0; i < warp_ins; i++)
                {
                    Actions()->UnitCommand(gates[i], warp_ability, spots[i]);
                }
                return true;
            }
        }
        return false;
    }

    bool TossBot::PullOutOfGas(BuildOrderResultArgData data)
    {
        removed_gas_miners += data.amount;
        Units workers;
        for (const auto &data : assimilators)
        {
            if (data.second.workers[2] != NULL)
                workers.push_back(data.second.workers[2]);
        }
        for (const auto &data : assimilators)
        {
            if (data.second.workers[1] != NULL)
                workers.push_back(data.second.workers[1]);
        }
        for (const auto &data : assimilators)
        {
            if (data.second.workers[0] != NULL)
                workers.push_back(data.second.workers[0]);
        }
        int num_removed = 0;
        for (const auto &worker : workers)
        {
            if (num_removed >= data.amount)
                break;
            RemoveWorker(worker);
            PlaceWorker(worker);
            num_removed++;
        }
        return true;
    }

    bool TossBot::IncreaseExtraPylons(BuildOrderResultArgData data)
    {
        extra_pylons += data.amount;
        return true;
    }

    bool TossBot::MicroChargelotAllin(BuildOrderResultArgData data)
    {
        if (Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_WARPPRISM)).size() > 0)
        {
            const Unit* prism = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_WARPPRISM))[0];
            ChargelotAllInStateMachine* chargelotFSM = new ChargelotAllInStateMachine(this, "Chargelot allin", locations->warp_prism_locations, Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ZEALOT)), prism, Observation()->GetGameLoop() / 22.4);
            active_FSMs.push_back(chargelotFSM);
            return true;
        }
        return false;
    }

    bool TossBot::RemoveScoutToProxy(BuildOrderResultArgData data)
    {
        for (const auto &fsm : active_FSMs)
        {
            const Unit* scout = NULL;
            if (fsm->name == "Scout Zerg")
            {
                scout = ((ScoutZergStateMachine*)fsm)->scout;
            }
            else if (fsm->name == "Scout Terran")
            {
                scout = ((ScoutTerranStateMachine*)fsm)->scout;
            }
            if (scout != NULL)
            {
                active_FSMs.erase(std::remove(active_FSMs.begin(), active_FSMs.end(), fsm), active_FSMs.end());
                Point2D pos = locations->proxy_pylon_locations[0];
                Actions()->UnitCommand(scout, ABILITY_ID::MOVE_MOVE, pos);
                active_actions.push_back(new ActionData(&TossBot::ActionRemoveScoutToProxy, new ActionArgData(scout, data.unitId, pos, data.amount)));
                return true;
            }
        }
        return false;
    }

    bool TossBot::SafeRallyPoint(BuildOrderResultArgData data)
    {
        for (const auto &building : Observation()->GetUnits(IsUnit(data.unitId)))
        {
            for (const auto & ability : Query()->GetAbilitiesForUnit(building).abilities)
            {
                Point2D pos = PointBetween(building->pos, Observation()->GetStartLocation(), 2);
                Actions()->UnitCommand(building, ABILITY_ID::SMART, pos);
            }
        }
        return true;
    }

    bool TossBot::DTHarass(BuildOrderResultArgData data)
    {
        if (enemy_race == Race::Terran)
            active_actions.push_back(new ActionData(&TossBot::ActionDTHarassTerran, new ActionArgData()));
        return true;
    }


#pragma endregion

#pragma region Build Orders

    void TossBot::SetBuildOrder(BuildOrder build)
    {
        locations = new Locations(Observation()->GetStartLocation(), build, Observation()->GetGameInfo().map_name);

        switch (build)
        {
        case BuildOrder::blank:
            SetBlank();
            break;
        case BuildOrder::blink_proxy_robo_pressure:
            SetBlinkProxyRoboPressureBuild();
            break;
        case BuildOrder::oracle_gatewayman_pvz:
            SetOracleGatewaymanPvZ();
            break;
        case BuildOrder::chargelot_allin:
            SetChargelotAllin();
            break;
        case BuildOrder::chargelot_allin_old:
            SetChargelotAllinOld();
            break;
        case BuildOrder::four_gate_adept_pressure:
            Set4GateAdept();
            break;
        case BuildOrder::fastest_dts:
            SetFastestDTsPvT();
            break;
        default:
            std::cout << "Error invalid build order in SetBuildOrder" << std::endl;
        }
    }

    void TossBot::SetBlank()
    {
        build_order = {};
    }

    void TossBot::SetBlinkProxyRoboPressureBuild()
    {
        build_order = { BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(6.5f),                                      &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(17.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(40.5f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(60.5f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(70.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(82.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(85.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::NumWorkers,        BuildOrderConditionArgData(21),                                        &TossBot::CutWorkers,               BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(117.0f),                                    &TossBot::ResearchWarpgate,         BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(117.0f),                                    &TossBot::Contain,                  BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(136.0f),                                    &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(136.0f),                                    &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(137.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(152.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(159.0f),                                    &TossBot::BuildProxy,               BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)) ,
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(166.0f),                                    &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(171.0f),                                    &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(176.5f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ResearchBlink,            BuildOrderResultArgData(UPGRADE_ID::BLINKTECH)),
                        BuildOrderData(&TossBot::IsResearching,     BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ChronoTillFinished,       BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(185.0f),                                    &TossBot::UncutWorkers,             BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(185.0f),                                    &TossBot::WarpInAtProxy,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(205.0f),                                    &TossBot::BuildProxy,               BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(205.0f),                                    &TossBot::ContinueBuildingPylons,   BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(205.0f),                                    &TossBot::ContinueMakingWorkers,    BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY),     &TossBot::TrainFromProxy,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY),     &TossBot::ContinueChronoProxyRobo,  BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY))
        };
    }

    void TossBot::SetOracleGatewaymanPvZ()
    {
        build_order = { BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(6.5f),                                      &TossBot::BuildBuildingMulti,       BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON, UNIT_TYPEID::PROTOSS_GATEWAY})),
                        //BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(30.0f),                                     &TossBot::Scout,                    BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(39.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(68.0f),                                     &TossBot::BuildBuildingMulti,       BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(94.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(102.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(102.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(123.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STARGATE)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::TrainAdept,               BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ADEPT)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(130.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(134.0f),                                    &TossBot::ResearchWarpgate,         BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(149.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(150.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(156.0f),                                    &TossBot::TrainAdept,               BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ADEPT)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_STARGATE),             &TossBot::TrainOracle,              BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STARGATE)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(173.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(186.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(191.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STARGATE)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(202.0f),                                    &TossBot::TrainOracle,              BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STARGATE)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(203.0f),                                    &TossBot::MicroOracles,             BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(203.0f),                                    &TossBot::StalkerOraclePressure,    BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(205.0f),                                    &TossBot::BuildBuildingMulti,       BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_PYLON})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(236.0f),                                    &TossBot::TrainOracle,              BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STARGATE)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(240.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(240.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(240.0f),                                    &TossBot::BuildBuildingMulti,       BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, UNIT_TYPEID::PROTOSS_FORGE})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(250.0f),                                    &TossBot::BuildBuildingMulti,       BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY})),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ResearchBlink,            BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ChronoTillFinished,       BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_FORGE),                &TossBot::ResearchAttackOne,        BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_FORGE),                &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_FORGE)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(270.0f),                                    &TossBot::ContinueBuildingPylons,   BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(270.0f),                                    &TossBot::ContinueMakingWorkers,    BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(270.0f),                                    &TossBot::ContinueWarpingInStalkers,BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(300.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        //BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(325.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ResearchCharge,           BuildOrderResultArgData()),
        };
    }

    void TossBot::SetChargelotAllin()
    {
        build_order = { BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(6.5f),                                      &TossBot::BuildFirstPylon,          BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(16.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(30.0f),                                     &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(34.0f),                                     &TossBot::Scout,                    BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(39.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(63.0f),                                     &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(69.0f),                                     &TossBot::BuildBuildingMulti,       BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(97.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(101.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(124.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(128.0f),                                    &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(135.0f),                                    &TossBot::ResearchWarpgate,         BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(158.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(160.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(172.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::HasGas,            BuildOrderConditionArgData(100),                                       &TossBot::PullOutOfGas,             BuildOrderResultArgData(6)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ResearchCharge,           BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ChronoTillFinished,       BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
                        BuildOrderData(&TossBot::NumWorkers,        BuildOrderConditionArgData(30),                                        &TossBot::CutWorkers,               BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(189.0f),                                    &TossBot::BuildBuildingMulti,       BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(215.0f),                                    &TossBot::TrainPrism,               BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_WARPPRISM)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(215.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
                        //BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(220.0f),                                    &TossBot::IncreaseExtraPylons,      BuildOrderResultArgData(1)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(220.0f),                                    &TossBot::ContinueBuildingPylons,   BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(235.0f),                                    &TossBot::WarpInUnits,              BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ZEALOT, 2)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(255.0f),                                    &TossBot::WarpInUnits,              BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ZEALOT, 2)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(255.0f),                                    &TossBot::MicroChargelotAllin,      BuildOrderResultArgData()),
        };
    }

    void TossBot::SetChargelotAllinOld()
    {
        build_order = { BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(6.5f),                                      &TossBot::BuildFirstPylon,          BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(16.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(34.0f),                                     &TossBot::Scout,                    BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(39.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(39.0f),                                     &TossBot::ImmediatelySaturateGasses,BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(68.0f),                                     &TossBot::BuildBuildingMulti,       BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(97.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(102.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(110.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::ResearchWarpgate,         BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(130.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(133.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(149.0f),                                    &TossBot::PullOutOfGas,             BuildOrderResultArgData(2)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(155.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(165.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(172.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::HasGas,            BuildOrderConditionArgData(100),                                       &TossBot::PullOutOfGas,             BuildOrderResultArgData(4)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ResearchCharge,           BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ChronoTillFinished,       BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
                        BuildOrderData(&TossBot::NumWorkers,        BuildOrderConditionArgData(30),                                        &TossBot::CutWorkers,               BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(189.0f),                                    &TossBot::BuildBuildingMulti,       BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(215.0f),                                    &TossBot::TrainPrism,               BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_WARPPRISM)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(215.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
                        //BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(220.0f),                                    &TossBot::IncreaseExtraPylons,      BuildOrderResultArgData(1)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(220.0f),                                    &TossBot::ContinueBuildingPylons,   BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(235.0f),                                    &TossBot::WarpInUnits,              BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ZEALOT, 2)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(255.0f),                                    &TossBot::WarpInUnits,              BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ZEALOT, 2)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(255.0f),                                    &TossBot::MicroChargelotAllin,      BuildOrderResultArgData()),
        };
    }

    void TossBot::Set4GateAdept()
    {
        build_order = { BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(6.5f),                                      &TossBot::BuildFirstPylon,          BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(16.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(34.0f),                                     &TossBot::Scout,                    BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(39.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(68.0f),                                     &TossBot::BuildBuildingMulti,       BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(94.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(102.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(102.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::ResearchWarpgate,         BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(130.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(130.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(158.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(170.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::NumWorkers,        BuildOrderConditionArgData(30),                                        &TossBot::CutWorkers,               BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ResearchGlaives,          BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),      &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),      &TossBot::TrainAdept,               BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ADEPT)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(191.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(198.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY),     &TossBot::TrainPrism,               BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_WARPPRISM)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY),     &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(211.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(205.0f),                                    &TossBot::UncutWorkers,             BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(230.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        //(&TossBot::TimePassed, 230, self.continue_warping_in_adepts, None),
                        BuildOrderData(&TossBot::NumWorkers,        BuildOrderConditionArgData(30),                                        &TossBot::CutWorkers,               BuildOrderResultArgData()),
                        //(self.has_unit, UnitTypeId.WARPPRISM, self.adept_pressure, None),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(250.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(250.0f),                                    &TossBot::ContinueBuildingPylons,   BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(250.0f),                                    &TossBot::ContinueMakingWorkers,    BuildOrderResultArgData()),
        };
    }

    void TossBot::SetFastestDTsPvT()
    {
        build_order = { BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(10.0f),                                     &TossBot::BuildFirstPylon,          BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(27.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY})),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(34.0f),                                     &TossBot::Scout,                    BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(34.0f),                                     &TossBot::SafeRallyPoint,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(39.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(39.0f),                                     &TossBot::ImmediatelySaturateGasses,BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(39.0f),                                     &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(47.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(73.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(85.0f),                                     &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(105.0f),                                    &TossBot::RemoveScoutToProxy,       BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY, 151)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(110.0f),                                    &TossBot::CutWorkers,               BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(112.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(115.0f),                                    &TossBot::ResearchWarpgate,         BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(116.0f),                                    &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(116.0f),                                    &TossBot::ChronoBuilding,           BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(126.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(136.0f),                                    &TossBot::Contain,                  BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(136.0f),                                    &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(149.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_DARKSHRINE)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(166.0f),                                    &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(177.0f),                                    &TossBot::TrainStalker,             BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(190.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_DARKSHRINE),           &TossBot::ResearchDTBlink,          BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::HasBuilding,       BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_DARKSHRINE),           &TossBot::ChronoTillFinished,       BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_DARKSHRINE)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(225.0f),                                    &TossBot::WarpInUnits,              BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER, 3)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(227.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(240.0f),                                    &TossBot::BuildBuilding,            BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(245.0f),                                    &TossBot::WarpInUnits,              BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER, 2)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(270.0f),                                    &TossBot::UncutWorkers,             BuildOrderResultArgData()),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(298.0f),                                    &TossBot::WarpInUnits,              BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_DARKTEMPLAR, 3)),
                        BuildOrderData(&TossBot::TimePassed,        BuildOrderConditionArgData(298.0f),                                    &TossBot::DTHarass,                 BuildOrderResultArgData()),
        };
    }
    

#pragma endregion

#pragma region other

    IsFinishedUnit::IsFinishedUnit(UNIT_TYPEID type_) : m_type(type_) {
    }

    bool IsFinishedUnit::operator()(const Unit& unit_) const {
        return unit_.unit_type == m_type && unit_.build_progress == 1;
    }

#pragma endregion

#pragma region debug info

    void TossBot::DisplayDebugHud()
    {
        DisplayWorkerStatus();
        DisplayBuildOrder();
        DisplayActiveActions();
        DisplayActiveStateMachines();
        DisplayBuildingStatuses();
        DisplayArmyGroups();
        DisplaySupplyInfo();
    }

    void TossBot::DisplayWorkerStatus()
    {
        Debug()->DebugTextOut("first 2 spaces: " + std::to_string(first_2_mineral_patch_spaces.size()), Point2D(0, 0), Color(0, 255, 255), 20);
        if (close_3_mineral_patch_extras.size() > 0)
            Debug()->DebugTextOut("\nclose 3rd extras: " + std::to_string(close_3_mineral_patch_extras.size()), Point2D(0, 0), Color(255, 0, 255), 20);
        else
            Debug()->DebugTextOut("\nclose 3rd spaces: " + std::to_string(close_3_mineral_patch_spaces.size()), Point2D(0, 0), Color(0, 255, 255), 20);
        if (far_3_mineral_patch_extras.size() > 0)
            Debug()->DebugTextOut("\n\nfar 3rd extras: " + std::to_string(far_3_mineral_patch_extras.size()), Point2D(0, 0), Color(255, 0, 255), 20);
        else
            Debug()->DebugTextOut("\n\nfar 3rd spaces: " + std::to_string(far_3_mineral_patch_spaces.size()), Point2D(0, 0), Color(0, 255, 255), 20);



        std::string close_patches = "\n\n\n";
        std::string far_patches = "\n\n\n";
        std::string gasses = "\n\n\n";
        for (const auto &data : mineral_patches)
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
        for (const auto &data : mineral_patches)
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
        for (const auto &data : assimilators)
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
        for (int i = build_order_step; i < build_order_step + 5; i++)
        {
            if (i >= build_order.size())
                break;
            build_order_message += build_order[i].toString() + "\n";
        }
        Debug()->DebugTextOut(build_order_message, Point2D(.7, .1), Color(0, 255, 0), 20);
    }

    void TossBot::DisplayActiveActions()
    {
        std::string actions_message = "Active Actions:\n";
        for (int i = 0; i < active_actions.size(); i++)
        {
            actions_message += active_actions[i]->toString() + "\n";
            const Unit* unit = active_actions[i]->action_arg->unit;
            if (unit != NULL)
                Debug()->DebugTextOut(active_actions[i]->toString(), unit->pos, Color(0, 255, 0), 20);
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
                std::string info = UnitTypeIdToString(building_type) + " ";
                Color text_color = Color(0, 255, 0);
                if (building_type == UNIT_TYPEID::PROTOSS_WARPGATE)
                {
                    for (const auto & ability : Query()->GetAbilitiesForUnit(building).abilities)
                    {
                        if (ability.ability_id.ToType() == ABILITY_ID::TRAINWARP_ZEALOT)
                        {
                            text_color = Color(255, 0, 0);
                        }
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
        }

    }

    void TossBot::DisplayArmyGroups()
    {
        std::string army_info = "Armies:\n";
        for (int i = 0; i < army_groups.size(); i++)
        {
            army_info += "    Army " + std::to_string(i+1);
            army_info += ":\n";
            army_info += "Stalkers: " + std::to_string(army_groups[i]->stalker_ids.size());
            army_info += ", ";
            army_info += "Observers: " + std::to_string(army_groups[i]->observer_ids.size());
            army_info += ", \n";
            army_info += "Prisms: " + std::to_string(army_groups[i]->prism_ids.size());
            army_info += ", ";
            army_info += "Immortals: " + std::to_string(army_groups[i]->immortal_ids.size());
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
        for (const auto &action : active_actions)
        {
            if (action->action == &TossBot::ActionBuildBuilding && action->action_arg->unitId == UNIT_TYPEID::PROTOSS_PYLON)
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
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, PointBetween(stalker->pos, retreat_point, 4));
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
            }
            else if (stalker->shield == 0)
            {
                if (DangerLevel(stalker) > 0 && blink_ready)
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, PointBetween(stalker->pos, retreat_point, 4));
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
            }
            else if (!weapon_ready)
            {
                if (DangerLevel(stalker) > stalker->shield && blink_ready)
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, PointBetween(stalker->pos, retreat_point, 4));
                else if (close_enemies.size() > 0 && DistanceToClosest(close_enemies, stalker->pos) - 2 < RealGroundRange(stalker, ClosestTo(close_enemies, stalker->pos)))
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attack_point);
            }
            else if (close_enemies.size() > 0)
            {
                if (!ob_in_position && IsOnHighGround(stalker->pos, ClosestTo(close_enemies, stalker->pos)->pos) || ClosestTo(close_enemies, stalker->pos)->display_type == Unit::DisplayType::Snapshot)
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, ClosestTo(close_enemies, stalker->pos));
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
                Actions()->UnitCommand(prism, ABILITY_ID::GENERAL_MOVE, MedianCenter(stalkers));
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
                const Unit* prism = GetLeastFullPrism(prisms);
                if (prism->cargo_space_max - prism->cargo_space_taken > 4)
                {
                    //Actions()->UnitCommand(prism, ABILITY_ID::LOAD_WARPPRISM, stalker, true);
                    Actions()->UnitCommand(stalker, ABILITY_ID::SMART, prism);
                }
                else if (blink_ready)
                {
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, PointBetween(stalker->pos, retreat_point, 4));
                }
                else
                {
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
                }
            }
            else if (stalker->shield == 0) // TODO pull this out?
            {
                if (DangerLevel(stalker) > 0 && blink_ready)
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, PointBetween(stalker->pos, retreat_point, 4));
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
            }
            else if (!weapon_ready)
            {
                const Unit* prism = GetLeastFullPrism(prisms);
                if (prism->cargo_space_max - prism->cargo_space_taken > 6)
                {
                    //Actions()->UnitCommand(prism, ABILITY_ID::LOAD_WARPPRISM, stalker, true);
                    Actions()->UnitCommand(stalker, ABILITY_ID::SMART, prism);
                }
                if (DangerLevel(stalker) > stalker->shield && blink_ready)
                    Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK_STALKER, PointBetween(stalker->pos, retreat_point, 4));
                else if (close_enemies.size() > 0 && DistanceToClosest(close_enemies, stalker->pos) - 2 < RealGroundRange(stalker, ClosestTo(close_enemies, stalker->pos)))
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attack_point);
            }
            else if (close_enemies.size() > 0)
            {
                if (!ob_in_position && IsOnHighGround(stalker->pos, ClosestTo(close_enemies, stalker->pos)->pos) || ClosestTo(close_enemies, stalker->pos)->display_type == Unit::DisplayType::Snapshot)
                    Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, retreat_point);
                else
                    Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, ClosestTo(close_enemies, stalker->pos));
            }
            else
            {
                Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attack_point);
            }
        }
    }

    void TossBot::ImmortalAttackTowards(Units stalkers, Point2D retreat_point, Point2D attack_point, bool ob_in_position)
    {

    }

    void TossBot::ImmortalAttackTowardsWithPrism(Units stalkers, Units prisms, Point2D retreat_point, Point2D attack_point, bool ob_in_position)
    {

    }

#pragma endregion
}
