#pragma once
#include "pathfinding.h"
#include "TossBot.h"

#include <string>

#include "sc2api/sc2_interfaces.h"


namespace sc2 {


class Locations
{
public:
    std::vector<Point2D> nexi_locations;
    std::vector<Point2D> pylon_locations;
    Point2D first_pylon_location_zerg;
    Point2D first_pylon_location_protoss;
    Point2D first_pylon_location_terran;
    std::vector<Point2D> assimilator_locations;
    std::vector<Point2D> gateway_locations;
    std::vector<Point2D> cyber_core_locations;
    std::vector<Point2D> tech_locations;

    std::vector<Point2D> proxy_pylon_locations;
    std::vector<Point2D> proxy_gateway_locations;
    std::vector<Point2D> proxy_tech_locations;

    std::vector<Point2D> attack_path;
	std::vector<Point2D> attack_path_alt;
    int high_ground_index;
	int high_ground_index_alt;
    Graph* air_graph;

    std::vector<Point2D> warp_prism_locations;
	std::vector<Point2D> immortal_drop_prism_locations;

    Point2D initial_scout_pos;
    std::vector<Point2D> main_scout_path;
    std::vector<Point2D> natural_scout_path;
    std::vector<Point2D> possible_3rds;
    Point2D enemy_natural;

	Point2D first_pylon_cannon_rush;
	std::vector<Point2D> pylon_locations_cannon_rush;
	std::vector<Point2D> gateway_locations_cannon_rush;
	std::vector<Point2D> cannon_locations_cannon_rush;
	std::vector<Point2D> shield_battery_locations_cannon_rush;
	std::vector<Point2D> robo_locations_cannon_rush;
	std::vector<Point2D> tech_locations_cannon_rush;
	std::vector<Point2D> gas_steal_locations_cannon_rush;

	Point2D third_base_pylon_gap;
	Point2D natural_door_closed;
	Point2D natural_door_open;

    Locations() {}
    Locations(Point3D start_location, BuildOrder build_order, std::string map_name)
    {
        if (map_name == "Lightshade LE")
        {
            SetLightshadeLocations(start_location, build_order);
        }
    }

    void SetLightshadeLocations(Point3D, BuildOrder);
};




}
