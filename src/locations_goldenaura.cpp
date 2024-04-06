#pragma once
#include "locations.h"
#include "TossBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetGoldenauraLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(0, 0);
		// mirror line: y=mx+n
		float m = 1;
		float n = -2;

		auto convert_location = [m, n](Point2D point, bool swap)
		{
			if (swap)
			{
				float px = (point.x + (point.y * m) - (m * n)) / (m + 1);
				float py = (m * px) + n;
				return Point2D((2 * px) - point.x, (2 * py) - point.y);
			}
			else
			{
				return point;
			}
		};

		bool swap = start_location.x == 69.5 && start_location.y == 168.5;

		this->start_location = start_location;

		nexi_locations = { P(170.5, 67.5),
							P(148.5, 59.5),
							P(123.5, 55.5),
							P(142.5, 89.5),
							P(168.5, 114.5),
							P(104.5, 61.5) };

		pylon_locations = { P(124.25, 60),
							P(144, 93),
							P(139, 89),
							P(169, 71),
							P(169, 73),
							P(174, 59),
							P(179, 68),
							P(172, 86),
							P(151, 52),
							P(123, 48),
							P(142, 54),
							P(164, 67) };

		first_pylon_location_zerg = P(144, 64);
		first_pylon_location_protoss = P(158, 75);
		first_pylon_location_terran = P(156, 80);

		gateway_locations = { P(154.5, 73.5),
								P(160.5, 74.5),
								P(153.5, 79.5),
								P(141.5, 67.5),
								P(140.5, 60.5),
								P(121.25, 60.75),
								P(124.25, 62.5),
								P(119.25, 57.25),
								P(146.5, 95.5),
								P(143.5, 95.5),
								P(140.5, 94.5),
								P(174.5, 56.5),
								P(171.5, 56.5),
								P(181.5, 65.5),
								P(181.5, 68.5),
								P(181.5, 71.5),
								P(159.5, 82.5),
								P(158.5, 79.5),
								P(161.5, 79.5),
								P(168.5, 87.5),
								P(168.5, 84.5),
								P(168.5, 81.5),
								P(172.5, 83.5),
								P(172.5, 80.5) };

		assimilator_locations = { P(167.5, 60.5),
									P(177.5, 70.5),
									P(155.5, 62.5),
									P(144.5, 52.5),
									P(119.5, 48.5),
									P(116.5, 52.5),
									P(140.5, 82.5),
									P(149.5, 90.5),
									P(166.5, 107.5),
									P(175.5, 117.5),
									P(111.5, 60.5),
									P(97.5, 60.5),
									P(59.5, 48.5),
									P(50.5, 56.5),
									P(174.5, 162.5),
									P(163.5, 172.5) };


		cyber_core_locations = { P(157.5, 72.5),
								P(156.5, 82.5),
								P(141.5, 64.5) };

		tech_locations = { P(166.5, 66.25),
							P(166.25, 69.5),
							P(166.25, 72.75),
							P(171.75, 71.5),
							P(171.5, 74.5),
							P(166.5, 75.5),
							P(163.5, 69.5) };

		proxy_pylon_locations = { P(107, 127) };

		proxy_gateway_locations = {  };

		proxy_tech_locations = {  };


		attack_path = { P(97, 95),
						P(72, 112),
						P(66, 116.5),
						P(63, 120),
						P(62.5, 123),
						P(64, 127.5),
						P(67, 132),
						P(67, 139),
						P(66, 143),
						P(67, 146),
						P(70, 149.5),
						P(75, 155) };

		if (swap)
		{
			attack_path_line = PathManager({ new LineSegmentLinearX(-1.5, 68, 97, 116, false, center_point, true),
											new LineSegmentCurveX(.08, -20, 1310.4, 116, 130, false, center_point, true),
											new LineSegmentCurveX(-.06, 16.5, -1068.68, 130, 144, false, center_point, true),
											new LineSegmentCurveX(.1, -28.92, 2154.72, 144, 151.5, false, center_point, true),
											new LineSegmentLinearX(.93, -72.86, 151.5, 159, false, center_point, true) }, true, true);
		}
		else
		{
			attack_path_line = PathManager({ new LineSegmentLinearY(-1.5, 240, 95, 114, false, center_point, true),
											new LineSegmentCurveY(.08, -19.68, 1272.72, 114, 128, false, center_point, true),
											new LineSegmentCurveY(-.06, 16.26, -1033.92, 128, 142, false, center_point, true),
											new LineSegmentCurveY(.1, -28.52, 2099.28, 142, 149.5, false, center_point, true),
											new LineSegmentLinearY(.93, -69, 149.5, 157, false, center_point, true)}, false, true);
		}

		high_ground_index = 11;

		attack_path_alt = { P(138, 138),
							P(130, 145),
							P(135, 160),
							P(120, 162),
							P(95, 135),
							P(80, 133),
							P(67, 139),
							P(66, 143),
							P(67, 146),
							P(70, 149.5),
							P(75, 155) };

		high_ground_index_alt = 10;

		initial_scout_pos = P(72.5, 152.5);

		main_scout_path = { P(60, 163),
							P(65, 177),
							P(85, 170),
							P(82, 157) };

		natural_scout_path = { P(58, 155),
								P(53, 145),
								P(63, 140) };

		possible_3rds = { P(57.5, 121.5), P(91.5, 140.5)};

		enemy_natural = P(61.5, 146.5);


		Point2D entrance_point = P(95, 178);
		Point2D exit_point = P(48, 100);
		std::vector<Point2D> entrance_points = { P(72, 178),
												P(64, 156),
												P(56, 131) };
		std::vector<Point2D> exit_points = { P(60, 165),
											P(52, 144),
											P(52, 115) };;
		std::vector<Point2D> base_points = { P(69.5, 168.5),
											P(61.5, 146.5),
											P(57.5, 121.5) };;
		std::vector<std::vector<Point2D>> exfi_paths = { {P(50, 158),
															P(55, 176),
															P(75, 181)},
															{ P(48, 134),
															P(48, 107)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);



		third_base_pylon_gap = P(124, 58.5);
		natural_door_closed = P(141, 62.5);
		natural_door_open = P(138, 62.5);



		adept_harrass_protoss_consolidation = { P(82, 135), P(62, 125) };


		main_early_dead_space = P(172, 85);
		natural_front = { P(138, 62),
							P(141, 69) };


		adept_scout_shade = P(64, 129);
		adept_scout_runaway = P(63, 120);
		adept_scout_ramptop = P(72.5, 152.5);
		adept_scout_nat_path = { P(52, 146),
									P(61, 155) };
		adept_scout_base_spots = { P(58, 167),
									P(69, 177),
									P(82, 173),
									P(86, 159) };


		blink_presure_consolidation = P(85, 126);
		blink_pressure_prism_consolidation = P(91, 121);
		blink_pressure_blink_up = P(87, 149);
		blink_pressure_blink_down = P(83, 154);


		if (!swap)
		{
			blink_nat_attacK_path_line = PathManager({ new LineSegmentCurveX(-.06, 9, -205.5, 75, 85, false, center_point, true),
														new LineSegmentCurveX(.13, -19.5, 863.25, 65, 75, false, center_point, true) }, true, false);


			blink_main_attack_path_lines = { PathManager({ new LineSegmentLinearX(-.3, 178.9, 65, 83, false, center_point, true) }, true, false),
											PathManager({ new LineSegmentCurveY(-.015, 4.62, -272.74, 154, 175, false, center_point, true) }, false, true) };
		}
		else
		{
			blink_nat_attacK_path_line = PathManager({ new LineSegmentCurveY(-.06, 8.76, -185.75, 73, 83, false, center_point, true),
														new LineSegmentCurveY(.13, -18.98, 826.77, 63, 73, false, center_point, true) }, false, false);


			blink_main_attack_path_lines = { PathManager({ new LineSegmentLinearY(-.3, 180.3, 63, 81, false, center_point, true) }, false, false),
											PathManager({ new LineSegmentCurveX(-.015, 4.68, -284.04, 156, 177, false, center_point, true) }, true, true) };
		}

	}
	
}

