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
    int high_ground_index;
    Graph* air_graph;

    std::vector<Point2D> warp_prism_locations;

    Point2D initial_scout_pos;
    std::vector<Point2D> main_scout_path;
    std::vector<Point2D> natural_scout_path;
    std::vector<Point2D> possible_3rds;
    Point2D enemy_natural;

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
