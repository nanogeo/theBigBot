#pragma once
#include "pathfinding.h"
#include "TossBot.h"
#include "path_manager.h"

#include <string>

#include "sc2api/sc2_interfaces.h"


namespace sc2 {

struct OraclePath
{
	Point2D entrance_point;
	Point2D exit_point;
	std::vector<Point2D> entrance_points;
	std::vector<Point2D> exit_points;
	std::vector<Point2D> base_points;
	std::vector<std::vector<Point2D>> exfi_paths;
	OraclePath() {};
	OraclePath(Point2D entrance_point, Point2D exit_point, std::vector<Point2D> entrance_points, std::vector<Point2D> exit_points, std::vector<Point2D> base_points, std::vector<std::vector<Point2D>> exfi_paths)
	{
		this->entrance_point = entrance_point;
		this->exit_point = exit_point;
		this->entrance_points = entrance_points;
		this->exit_points = exit_points;
		this->base_points = base_points;
		this->exfi_paths = exfi_paths;
	}
};

class Locations
{
public:
	Point2D start_location;
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
	PathManager attack_path_line;
    int high_ground_index;
	int high_ground_index_alt;
    Graph* air_graph;
	std::vector<std::vector<Point2D>> enemy_mineral_lines;

    std::vector<Point2D> warp_prism_locations;
	std::vector<Point2D> immortal_drop_prism_locations;

    Point2D initial_scout_pos;
    std::vector<Point2D> main_scout_path;
    std::vector<Point2D> natural_scout_path;
    std::vector<Point2D> possible_3rds;
    Point2D enemy_natural;

	OraclePath oracle_path;

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

	Point2D main_early_dead_space;
	std::vector<Point2D> natural_front;
	Point2D base_defense_front;
	Point2D base_defense_main;
	Point2D base_defense_nat;
	Point2D base_defense_center;

	Point2D adept_scout_shade;
	Point2D adept_scout_runaway;
	Point2D adept_scout_ramptop;
	std::vector<Point2D> adept_scout_nat_path;
	std::vector<Point2D> adept_scout_base_spots;

	Point2D blink_presure_consolidation;
	Point2D blink_pressure_prism_consolidation;
	Point2D blink_pressure_blink_up;
	Point2D blink_pressure_blink_down;
	

    Locations() {}
    Locations(Point3D start_location, BuildOrder build_order, std::string map_name)
    {
        if (map_name == "Lightshade LE")
        {
            SetLightshadeLocations(start_location, build_order);
        }

		else if (map_name == "Eternal Empire LE")
		{
			SetTestingLocations(start_location, build_order);
		}
    }

    void SetLightshadeLocations(Point3D, BuildOrder);
	void SetTestingLocations(Point3D, BuildOrder);
};




}
