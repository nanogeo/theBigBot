#pragma once
#include "locations.h"
#include "theBigBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetLightshadeLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(92, 82);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(184, 164) - point;
			else
				return point;
		};

		bool swap = start_location.x == 40.5 && start_location.y == 131.5;

		this->start_location = start_location;

		nexi_locations = { P(143.5, 32.5),
							P(145.5, 61.5),
							P(147.3, 94.7),
							P(119.5, 53.5),
							P(104.5, 27.5),
							P(148.5, 125.5),
							P(124.5, 120.5) };

		pylon_locations = { P(140, 36),
							P(141, 65),
							P(124, 29),
							P(146.8, 99),
							P(146, 91),
							P(135, 36),
							P(116, 50),
							P(121, 57),
							P(150, 26),
							P(154, 64),
							P(156, 97),
							P(134, 24),
							P(149, 45),
							P(156, 92),
							P(150, 70),
							P(101, 27),
							P(106, 31),
							P(110, 21),
							P(99, 20),
							P(134, 26),
							P(140, 38),
							P(145, 39),
							P(126, 29) };

		pylon_locations_cannon_rush = { P(46, 90),
										P(47, 92),
										P(51, 100),
										P(54, 94) };

		first_pylon_location_zerg = P(141, 65);
		first_pylon_location_protoss = P(135, 47);
		first_pylon_location_terran = P(131, 45);

		gateway_locations = { P(138.5, 66.5),
								P(136.5, 62.5),
								P(136.5, 49.5),
								P(128.5, 43.5),
								P(133.5, 44.5),
								P(143.5, 100.5),
								P(142.5, 97.5),
								P(142.5, 94.5),
								P(143.5, 91.5),
								P(113.5, 46.5),
								P(113.5, 49.5),
								P(113.5, 52.5),
								P(115.5, 55.5),
								P(122.5, 32.5),
								P(125.5, 32.5),
								P(128.5, 32.5),
								P(126.5, 25.5),
								P(136.5, 22.5),
								P(139.5, 22.5),
								P(136.5, 25.5),
								P(136.5, 28.5),
								P(130.5, 22.5),
								P(130.5, 25.5),
								P(139.5, 33.5),
								P(136.5, 33.5),
								P(139.5, 30.5),
								P(137.5, 36.5),
								P(142.5, 36.5),
								P(145.5, 36.5) };

		assimilator_locations = { P(150.5, 35.5),
									P(139.5, 25.5),
									P(152.5, 58.5),
									P(148.5, 54.5),
									P(149.5, 87.5),
									P(149.5, 101.5),
									P(126.5, 56.5),
									P(116.5, 46.5) };


		cyber_core_locations = { P(141.7, 68.3),
									P(137.5, 46.5),
									P(132.5, 47.5) };

		tech_locations = { P(139.5, 33.5),
							P(128.5, 43.5),
							P(142.5, 36.5),
							P(137.5, 36.5),
							P(136.5, 33.5),
							P(145.5, 36.5),
							P(134.5, 38.5),
							P(137.5, 39.5),
							P(142.5, 39.5),
							P(133.5, 44.5) };

		proxy_pylon_locations = { P(32.0, 66.0),
								P(75.0, 136.0) };

		proxy_gateway_locations = { P(34.5, 65.5), 
									P(75.5, 133.5) };

		proxy_tech_locations = { P(32.5, 68.5),
								P(35.5, 68.5),
								P(77.5, 136.5),
								P(78.5, 133.5) };


		attack_path = { P(74, 35),
						P(50, 60),
						P(44, 69),
						P(44, 76),
						P(48, 82),
						P(52, 89),
						P(49, 97),
						P(44, 102),
						P(41, 111),
						P(44, 114),
						P(43, 124) };

		attack_path_line = PathManager({ new LineSegmentLinearY(-1, 108.5, 34, 65.5, swap, center_point, true),
										new LineSegmentCurveY(.0926, -13.056, 500.894, 65.5, 75.5, swap, center_point, true),
										new LineSegmentLinearY(1, -32.5, 75.5, 81.5, swap, center_point, true),
										new LineSegmentCurveY(-.071425, 12.607, -504.04, 81.5, 95, swap, center_point, true),
										new LineSegmentLinearY(-1, 144, 95, 99, swap, center_point, true), }, false, !swap);

		high_ground_index = 8;

		attack_path_alt = { P(77, 120),
							P(70, 111),
							P(63, 103),
							P(53, 97),
							P(44, 102),
							P(41, 111),
							P(44, 114),
							P(43, 124) };

		high_ground_index_alt = 5;

		std::vector<Point2D> air_path = { P(26, 17),
											P(102, 147),
											P(24, 85),
											P(58, 126),
											P(63, 148),
											P(24, 53),
											P(77, 16),
											P(24, 148),
											P(24, 115),
											P(59, 86),
											P(48, 112),
											P(83, 96),
											P(91, 118),
											P(49, 58),
											P(78, 49),
											P(57, 41),
											P(35, 101),
											P(62, 112),
											P(32, 69),
											P(34, 39),
											P(77, 138),
											P(37, 133) };


		std::vector<std::vector<int>> links = {
			{5, 6, 19},
			{4, 12, 20},
			{5, 8, 9, 16, 18},
			{4, 10, 12, 17, 20, 21},
			{1, 3, 7, 20, 21},
			{0, 2, 13, 15, 18, 19}, // 5
			{0, 14, 15},
			{4, 8, 21},
			{2, 7, 10, 16, 21},
			{2, 10, 11, 13, 14, 16, 17, 18},
			{3, 8, 9, 16, 17, 21}, // 10
			{9, 12, 14, 17},
			{1, 3, 11, 17, 20},
			{5, 9, 14, 15, 18},
			{6, 9, 11, 13, 15},
			{0, 5, 6, 13, 14, 19}, // 15
			{2, 8, 9, 10},
			{3, 9, 10, 11, 12},
			{2, 5, 9, 13},
			{0, 5, 15},
			{1, 3, 4, 12}, // 20
			{3, 4, 7, 8, 10}
		};

		air_graph = new Graph(air_path, links);


		 enemy_mineral_lines = { {P(40.5, 131.5), P(32, 131),P(42, 139)},
																{P(38.5, 102.5), P(38, 95),P(31.5, 105.5)},
																{P(36.5, 69.5), P(32, 63),P(32, 76)},
																{P(64.5, 110.5), P(57.5, 107.5),P(65, 118)},
																{P(35.5, 38.5), P(36, 32),P(28.5, 41.5)},
																{P(79.5, 136.5), P(71, 136),P(82.5, 143.5)},
																{P(59.5, 43.5), P(59, 36),P(52.5, 46.5)} };


		warp_prism_locations = { P(50, 60),
								P(76, 86),
								P(62, 131),
								P(76, 86) };

		immortal_drop_prism_locations = { P(52, 137),
											P(41, 141),
											P(33, 137),
											P(31, 128),
											P(33, 120),
											P(33, 111),
											P(29, 103),
											P(32, 96),
											P(38, 91),
											P(46, 91),
											P(51, 99),
											P(50, 107),
											P(51, 114),
											P(54, 123),
											P(55, 131) };

		initial_scout_pos = P(44, 114);

		main_scout_path = { P(39, 121),
							P(31, 126),
							P(34, 141),
							P(50, 141),
							P(57, 134),
							P(52, 122), };

		natural_scout_path = { P(31, 110),
								P(29, 97),
								P(42, 96), };

		possible_3rds = { P(36.5, 69.5),
						P(64.5, 110.5), };

		enemy_natural = P(38, 102);


		Point2D entrance_point = P(75, 144);
		Point2D exit_point = P(28, 46);
		std::vector<Point2D> entrance_points = { P(48, 143),
												 P(28, 111),
												 P(32, 82) };
		std::vector<Point2D> exit_points = { P(27, 124),
											P(40, 90),
											P(33, 56) };;
		std::vector<Point2D> base_points = { P(40.5, 131.5),
											P(38, 102),
											P(36.5, 69.5) };;
		std::vector<std::vector<Point2D>> exfi_paths = { {P(24, 124),
															P(26, 145),
															P(52, 147)},
															{ P(24, 85),
															P(24, 55)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);




		first_pylon_cannon_rush = P(137, 47);

		pylon_locations_cannon_rush = { P(46, 90),
										P(47, 92),
										P(51, 100),
										P(54, 94) };

		gateway_locations_cannon_rush = { P(46.5, 94.5),
										P(47.5, 84.5),
										P(58.5, 89.5) };

		cannon_locations_cannon_rush = { P(45, 92),
										P(49, 95),
										P(50, 98) };

		shield_battery_locations_cannon_rush = { P(49, 93),
												P(52, 98),
												P(54, 96),
												P(52, 96),
												P(48, 90),
												P(50, 90) };

		robo_locations_cannon_rush = { P(46.5, 87.5),
										P(0, 0) };

		tech_locations_cannon_rush = { P(136.5, 49.5),
										P(139.5, 46.5),
										P(44.5, 84.5) };

		gas_steal_locations_cannon_rush = { P(0, 0),
											P(0, 0),
											P(0, 0),
											P(0, 0) };

		third_base_pylon_gap = P(147, 97.5);
		natural_door_closed = P(137.5, 64.5);
		natural_door_open = P(135.5, 64.5);

		main_early_dead_space = P(125, 32);
		natural_front = { P(135, 65),
							P(140, 70) };

		base_defense_front = P(137, 67);
		base_defense_main = P(126, 25);
		base_defense_nat = P(154, 71);
		base_defense_center = P(137, 44);

		adept_scout_shade = P(50, 94);
		adept_scout_runaway = P(47, 81);
		adept_scout_ramptop = P(45, 115);
		adept_scout_nat_path = { P(37, 99),
									P(37, 107) };
		adept_scout_base_spots = { P(30, 126),
									P(38, 138),
									P(49, 139),
									P(57, 134) };

		blink_presure_consolidation = P(46, 77);
		blink_pressure_prism_consolidation = P(43, 72);
		blink_pressure_blink_up = P(58, 115);
		blink_pressure_blink_down = P(56, 121);

		blink_nat_attack_path_line = PathManager({ new LineSegmentCurveY(.0926, -13.056, 500.894, 65.5, 75.5, swap, center_point, true),
													new LineSegmentLinearY(1, -32.5, 75.5, 81.5, swap, center_point, true),
													new LineSegmentCurveY(-.071425, 12.607, -504.04, 81.5, 95, swap, center_point, true),
													new LineSegmentLinearY(-1, 144, 95, 99, swap, center_point, true), }, false, !swap);

		blink_main_attack_path_lines = { PathManager({ new LineSegmentCurveY(-.05, 12.5, -724.45, 121, 138, swap, center_point, true) }, false, !swap),
										PathManager({ new LineSegmentLinearY(10, -1154, 119, 121, swap, center_point, true) }, false, swap) };

		//blink_nat_attacK_path_line = PathManager({ new LineSegmentCurveX(-.04, 4.96, -52.76, 55, 67, swap, center_point),
		//										new LineSegmentCurveX(.08, -8, 297.04, 40, 55, swap, center_point) }, true, swap);

		cannon_rush_terran_pylon_walloffs = {
			CannonRushTriplePylonWalloff(P(49, 108), P(48, 106), P(49, 104), P(50, 106)),
			CannonRushTriplePylonWalloff(P(39, 92), P(37, 92), P(36, 90), P(38, 90)),
			CannonRushTriplePylonWalloff(P(47, 114), P(49, 114), P(51, 114), P(49, 112)),
			CannonRushTriplePylonWalloff(P(55, 120), P(56, 122), P(58, 122), P(57, 120)),
			CannonRushTriplePylonWalloff(P(36, 117), P(34, 118), P(32, 117), P(34, 116)),
			CannonRushTriplePylonWalloff(P(30, 121), P(30, 123), P(30, 125), P(28, 123)),
			CannonRushTriplePylonWalloff(P(30, 127), P(30, 129), P(30, 131), P(28, 129)),
			CannonRushTriplePylonWalloff(P(40, 143), P(42, 142), P(44, 143), P(42, 144)),
			CannonRushTriplePylonWalloff(P(49, 142), P(51, 142), P(53, 142), P(51, 144)) };

		cannon_rush_terran_positions = {
			/*CannonRushPosition(P(47, 114),
				{CannonPosition(P(49, 112), P(48.5, 113), P(49, 112)), 
					CannonPosition(P(48, 109), P(48, 109), P(48, 109))},
				{	{}, 
					{BuildingPlacement(PYLON, P(46, 109), P(46, 109)), BuildingPlacement(PYLON, P(48, 107), P(48, 107)), BuildingPlacement(PYLON, P(50, 107), P(50, 107))}},
				{	{BuildingPlacement(GATEWAY, P(50.5, 114.5), P(49, 114))},
					{BuildingPlacement(GATEWAY, P(47.5, 106.5), P(47.5, 106.5)), BuildingPlacement(PYLON, P(50, 107), P(50, 107))}},
					P(45, 97)),*/
			CannonRushPosition(P(50, 106),
					{CannonPosition(P(49, 108), P(48.25, 107.25), P(48.5, 107)), CannonPosition(P(49, 104), P(48.5, 103), P(48.25, 104.75))},
					{{BuildingPlacement(PYLON, P(47, 109), P(47, 108)),
				BuildingPlacement(PYLON, P(47, 107), P(47.5, 107))},
				{BuildingPlacement(PYLON, P(48, 106), P(47.5, 105.5)),
				BuildingPlacement(PYLON, P(47, 104), P(47.5, 103.5)),
				BuildingPlacement(PYLON, P(48, 102), P(48.5, 102.2))}},
					{{BuildingPlacement(GATEWAY, P(46.5, 108.5), P(47.5, 107.5)),
				BuildingPlacement(PYLON, P(48, 106), P(48.5, 107))},
				{BuildingPlacement(PYLON, P(48, 106), P(47.5, 105.5)),
				BuildingPlacement(GATEWAY, P(46.5, 103.5), P(47.5, 103))}},
					P(45, 97)),
			CannonRushPosition(P(30, 111),
				{CannonPosition(P(32, 113), P(32, 113), P(32, 113)), 
					CannonPosition(P(28, 109), P(28, 109), P(28, 109))},
				{	{BuildingPlacement(PYLON, P(32, 111), P(0, 0)), BuildingPlacement(PYLON, P(34, 113), P(0, 0))},
					{BuildingPlacement(PYLON, P(28, 107), P(0, 0)), BuildingPlacement(PYLON, P(30, 109), P(0, 0))}},
				{	{BuildingPlacement(PYLON, P(32, 111), P(0, 0)), BuildingPlacement(GATEWAY, P(34.5, 112.5), P(0, 0))},
					{BuildingPlacement(PYLON, P(30, 109), P(0, 0)), BuildingPlacement(GATEWAY, P(28.5, 106.5), P(0, 0))}},
				P(45, 97)),
			CannonRushPosition(P(38, 118),
				{CannonPosition(P(36, 117), P(36, 117), P(36, 117)), 
					CannonPosition(P(40, 118), P(40, 118), P(40, 118)), 
					CannonPosition(P(34, 113), P(34, 113), P(34, 113))},
				{	{BuildingPlacement(PYLON, P(42, 118), P(40, 120)), BuildingPlacement(PYLON, P(47, 107), P(47.5, 107))},
					{BuildingPlacement(PYLON, P(48, 106), P(47.5, 105.5)), BuildingPlacement(PYLON, P(47, 104), P(47.5, 103.5)), BuildingPlacement(PYLON, P(48, 102), P(48.5, 102.2))},
					{BuildingPlacement(PYLON, P(48, 106), P(47.5, 105.5)), BuildingPlacement(PYLON, P(47, 104), P(47.5, 103.5)), BuildingPlacement(PYLON, P(48, 102), P(48.5, 102.2))}},
				{	{BuildingPlacement(GATEWAY, P(46.5, 108.5), P(47.5, 107.5)), BuildingPlacement(PYLON, P(48, 106), P(48.5, 107))},
					{BuildingPlacement(PYLON, P(48, 106), P(47.5, 105.5)), BuildingPlacement(GATEWAY, P(46.5, 103.5), P(47.5, 103))},
					{BuildingPlacement(PYLON, P(48, 106), P(47.5, 105.5)), BuildingPlacement(GATEWAY, P(46.5, 103.5), P(47.5, 103))}},
				P(45, 97))
		};

		cannon_rush_terran_stand_by = P(45, 97);


		/*cannon_rush_terran_stand_by_loop = { P(65, 79),
											P(70, 86),
											P(63, 99) };*/

}


	void Locations::SetLightshadeLocations2(Point3D start_location, BuildOrder build_order)
	{
		bool swap = start_location.x == 40.5 && start_location.y == 131.5;
		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(184, 164) - point;
			else
				return point;
		};

		cannon_rush_terran_stand_by_loop = { P(65, 79),
											P(70, 86),
											P(63, 99) };

		adept_harrass_protoss_consolidation = { P(65, 106), P(45, 81) };

	}

void Locations::SetTestingLocations(Point3D start_location, BuildOrder build_order)
{
	Point2D center_point = Point2D(88, 86);
	auto convert_location = [](Point2D point, bool swap)
	{
		if (swap)
			return Point2D(176, 172) - point;
		else
			return point;
	};

	bool swap = start_location.x == 142.5 && start_location.y == 140.5;

	this->start_location = start_location;

	attack_path = { P(34, 139),
					P(59, 114.5),
					P(85, 89),
					P(112, 62.5),
					P(142, 33) };

	attack_path_line = PathManager({ new LineSegmentLinearX(-.98, 172.5, 30, 150, swap, center_point, true) }, true, !swap),

	third_base_pylon_gap = P(34, 139);

	nexi_locations = { P(0, 0),
						P(0, 0),
						P(0, 0) };

	Point2D entrance_point = P(34, 139);
	Point2D exit_point = P(34, 139);
	std::vector<Point2D> entrance_points = { P(34, 139) };
	std::vector<Point2D> exit_points = { P(34, 139) };
	std::vector<Point2D> base_points = { P(34, 139) };
	std::vector<std::vector<Point2D>> exfi_paths = { {P(34, 139) } };

	oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);

}


}

