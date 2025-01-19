#pragma once
#include "pathfinding.h"
#include "theBigBot.h"
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

struct CannonRushTriplePylonWalloff
{
	Point2D pylon_1;
	Point2D pylon_2;
	Point2D pylon_3;
	Point2D cannon;
	CannonRushTriplePylonWalloff(Point2D pylon_1, Point2D pylon_2, Point2D pylon_3, Point2D cannon)
	{
		this->pylon_1 = pylon_1;
		this->pylon_2 = pylon_2;
		this->pylon_3 = pylon_3;
		this->cannon = cannon;
	}
};

struct BuildingPlacement
{
	UNIT_TYPEID type;
	Point2D building_pos;
	Point2D move_to_pos;
	BuildingPlacement(UNIT_TYPEID type, Point2D building_pos, Point2D move_to_pos)
	{
		this->type = type;
		this->building_pos = building_pos;
		this->move_to_pos = move_to_pos;
	}
};

struct CannonPosition
{
	Point2D cannon_pos;
	Point2D with_gate_walloff;
	Point2D with_pylon_wallof;
	CannonPosition(Point2D cannon_pos, Point2D with_gate_walloff, Point2D with_pylon_wallof)
	{
		this->cannon_pos = cannon_pos;
		this->with_gate_walloff = with_gate_walloff;
		this->with_pylon_wallof = with_pylon_wallof;
	}
};

struct CannonRushPosition
{
	Point2D initial_pylon;
	std::vector<CannonPosition> cannon_position;
	std::vector<std::vector<BuildingPlacement>> pylon_walloff_positions;
	std::vector<std::vector<BuildingPlacement>> gateway_walloff_positions;
	Point2D stand_by;
	CannonRushPosition(Point2D initial_pylon, std::vector<CannonPosition> cannon_position, std::vector<std::vector<BuildingPlacement>> pylon_walloff_positions, 
		std::vector<std::vector<BuildingPlacement>> gateway_walloff_positions, Point2D stand_by)
	{
		this->initial_pylon = initial_pylon;
		this->cannon_position = cannon_position;
		this->pylon_walloff_positions = pylon_walloff_positions;
		this->gateway_walloff_positions = gateway_walloff_positions;
		this->stand_by = stand_by;
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
    Graph* air_graph; // unused
	std::vector<std::vector<Point2D>> enemy_mineral_lines; // unused

    std::vector<Point2D> warp_prism_locations; // unused
	std::vector<Point2D> immortal_drop_prism_locations; // unused

    Point2D initial_scout_pos;
    std::vector<Point2D> main_scout_path;
    std::vector<Point2D> natural_scout_path;
    std::vector<Point2D> possible_3rds;
    Point2D enemy_natural;

	OraclePath oracle_path;

	Point2D first_pylon_cannon_rush; // unused
	std::vector<Point2D> pylon_locations_cannon_rush; // unused
	std::vector<Point2D> gateway_locations_cannon_rush; // unused
	std::vector<Point2D> cannon_locations_cannon_rush; // unused
	std::vector<Point2D> shield_battery_locations_cannon_rush; // unused
	std::vector<Point2D> robo_locations_cannon_rush; // unused
	std::vector<Point2D> tech_locations_cannon_rush; // unused
	std::vector<Point2D> gas_steal_locations_cannon_rush; // unused

	Point2D third_base_pylon_gap;
	Point2D natural_door_closed;
	Point2D natural_door_open;

	Point2D main_early_dead_space;
	std::vector<Point2D> natural_front;
	Point2D base_defense_front; // unused
	Point2D base_defense_main; // unused
	Point2D base_defense_nat; // unused
	Point2D base_defense_center; // unused

	Point2D adept_scout_shade;
	Point2D adept_scout_runaway;
	Point2D adept_scout_ramptop;
	std::vector<Point2D> adept_scout_nat_path;
	std::vector<Point2D> adept_scout_base_spots;

	Point2D blink_presure_consolidation;
	Point2D blink_pressure_prism_consolidation;
	Point2D blink_pressure_blink_up;
	Point2D blink_pressure_blink_down;

	std::vector<PathManager> blink_main_attack_path_lines;
	PathManager blink_nat_attack_path_line;
	
	std::vector<CannonRushTriplePylonWalloff> cannon_rush_terran_pylon_walloffs;

	std::vector<CannonRushPosition> cannon_rush_terran_positions;
	Point2D cannon_rush_terran_stand_by;
	std::vector<Point2D> cannon_rush_terran_stand_by_loop;

	std::vector<Point2D> adept_harrass_protoss_consolidation;

	std::vector<Point2D> bad_warpin_spots;

    Locations() {}
    Locations(Point3D start_location, BuildOrder build_order, std::string map_name)
    {
        if (map_name == "Lightshade LE")
        {
            SetLightshadeLocations(start_location, build_order);
			SetLightshadeLocations2(start_location, build_order);
        }
		else if (map_name == "Equilibrium AIE")
		{
			std::cerr << "Set equilibrium locations\n";
			SetEquilibriumLocations(start_location, build_order);
		}
		else if (map_name == "Goldenaura AIE")
		{
			SetGoldenauraLocations(start_location, build_order);
		}
		else if (map_name == "Hard Lead AIE")
		{
			SetHardLeadLocations(start_location, build_order);
		}
		else if (map_name == "Oceanborn AIE")
		{
			SetOceanbornLocations(start_location, build_order);
		}
		else if (map_name == "Site Delta AIE")
		{
			SetSiteDeltaLocations(start_location, build_order);
		}
		else if (map_name == "GresvanAIE")
		{
			SetGresvanLocations(start_location, build_order);
		}
		else if (map_name == "Abyssal Reef AIE")
		{
			SetAbyssalReefLocations(start_location, build_order);
		}
		else if (map_name == "Acropolis AIE")
		{
			SetAcropolisLocations(start_location, build_order);
		}
		else if (map_name == "Automaton AIE")
		{
			SetAutomatonLocations(start_location, build_order);
		}
		else if (map_name == "Ephemeron AIE")
		{
			SetEphemeronLocations(start_location, build_order);
		}
		else if (map_name == "Interloper AIE")
		{
			SetInterloperLocations(start_location, build_order);
		}
		else if (map_name == "ThunderbirdAIE")
		{
			SetThunderbirdLocations(start_location, build_order);
		}
		else if (map_name == "Eternal Empire LE")
		{
			SetTestingLocations(start_location, build_order);
		}
		else
		{
			std::cerr << "Unknown map name " + map_name + '\n';
		}

    }
	void SetTestingLocations(Point3D, BuildOrder);

    void SetLightshadeLocations(Point3D, BuildOrder);
	void SetLightshadeLocations2(Point3D, BuildOrder);
	void SetEquilibriumLocations(Point3D, BuildOrder);
	void SetGoldenauraLocations(Point3D, BuildOrder);
	void SetHardLeadLocations(Point3D, BuildOrder);
	void SetOceanbornLocations(Point3D, BuildOrder);
	void SetSiteDeltaLocations(Point3D, BuildOrder);
	void SetGresvanLocations(Point3D, BuildOrder);
	void SetAbyssalReefLocations(Point3D, BuildOrder);
	void SetAcropolisLocations(Point3D, BuildOrder);
	void SetAutomatonLocations(Point3D, BuildOrder);
	void SetEphemeronLocations(Point3D, BuildOrder);
	void SetInterloperLocations(Point3D, BuildOrder);
	void SetThunderbirdLocations(Point3D, BuildOrder);
	

};




}
