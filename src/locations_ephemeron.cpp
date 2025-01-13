#pragma once
#include "locations.h"
#include "theBigBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetEphemeronLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(224, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(160, 160) - point;
			else
				return point;
		};

		bool swap = start_location.x == 130.5 && start_location.y == 21.5;

		this->start_location = start_location;

		nexi_locations = {P(130.5, 48.5),
							P(98.5, 25.5),
							P(68.5, 48.5),
							P(65.5, 18.5),
							P(28.5, 28.5),
							P(131.5, 86.5) };

		pylon_locations = { P(94, 26),
							P(98, 18),
							P(136, 55),
							P(107, 23),
							P(138, 44),
							P(129, 25),
							P(127, 25),
							P(112, 22),
							P(132, 9),
							P(143, 20) };

		std::cerr << "First pylon: " + std::to_string(pylon_locations[0].x) + ", " + std::to_string(pylon_locations[0].y) + "\n";

		first_pylon_location_zerg = P(123, 48);
		first_pylon_location_protoss = P(121, 35);
		first_pylon_location_terran = P(116, 37);

		gateway_locations = { P(118.5, 37.5),
							P(118.5, 34.5),
							P(122.5, 37.5),
							P(123.5, 34.5),
							P(120.5, 52.5),
							P(120.5, 48.5),
							P(94.5, 28.5),
							P(91.5, 28.5),
							P(88.5, 27.5),
							P(97.5, 29.5),
							P(113.5, 19.5),
							P(116.5, 19.5),
							P(113.5, 24.5),
							P(116.5, 24.5),
							P(129.5, 10.5),
							P(134.5, 10.5),
							P(126.5, 11.5),
							P(141.5, 22.5),
							P(141.5, 17.5) };

		assimilator_locations = { P(137.5, 24.5),
									P(127.5, 14.5),
									P(133.5, 55.5),
									P(137.5, 52.5),
									P(95.5, 18.5),
									P(105.5, 28.5),
									P(61.5, 51.5),
									P(71.5, 41.5),
									P(62.5, 11.5),
									P(72.5, 21.5),
									P(21.5, 31.5),
									P(31.5, 21.5),
									P(128.5, 79.5),
									P(138.5, 89.5) };

		cyber_core_locations = {P(120.5, 32.5),
									P(120.5, 45.5),
									P(115.5, 34.5) };

		tech_locations = { P(131.5, 25.5),
							P(128.5, 27.5),
							P(125.5, 27.5),
							P(126.5, 22.5),
							P(123.5, 22.5),
							P(131.5, 28.5) };

		proxy_pylon_locations = {P(54, 76),
								P(68, 113) };

		proxy_gateway_locations = { P(51.5, 74.5),
									P(70.5, 112.5) };

		proxy_tech_locations = { P(51.5, 77.5),
								P(70.5, 115.5) };


		attack_path = { P(110, 108),
						P(106, 105),
						P(94, 101),
						P(90, 104),
						P(73, 122),
						P(70, 126),
						P(58, 124),
						P(52, 117),
						P(43, 110),
						P(36, 111),
						P(35, 120),
						P(34, 122),
						P(32, 136),
						P(27, 141) };

		attack_path_line = PathManager({}, true, swap);

		for (int i = 0; i < attack_path.size() - 3; i += 2)
		{
			// line between potins i , i+1
			if (abs(attack_path[i].x - attack_path[i + 1].x) > abs(attack_path[i].y - attack_path[i + 1].y))
			{
				double slope = (attack_path[i].y - attack_path[i + 1].y) / (attack_path[i].x - attack_path[i + 1].x);

				double line_x_a = slope;
				double line_x_b = attack_path[i].y - (slope * attack_path[i].x);

				attack_path_line.segments.push_back(new LineSegmentLinearX(line_x_a, line_x_b, attack_path[i].x, attack_path[i + 1].x, false, Point2D(0, 0), false));
			}
			else
			{
				double slope = (attack_path[i].x - attack_path[i + 1].x) / (attack_path[i].y - attack_path[i + 1].y);

				double line_y_a = slope;
				double line_y_b = attack_path[i].x - (slope * attack_path[i].y);

				attack_path_line.segments.push_back(new LineSegmentLinearY(line_y_a, line_y_b, attack_path[i].y, attack_path[i + 1].y, false, Point2D(0, 0), false));
			}

			// curve between attack_path i+1, i+2
			LineSegment* curve = attack_path_line.FitLineSegment(attack_path[i + 1], attack_path[i + 2], attack_path[i], attack_path[i + 3]);
			attack_path_line.segments.push_back(curve);
		}

		/*high_ground_index = 5;

		attack_path_alt = { P(, ),
							P(, ), };

		high_ground_index_alt = 6;*/

		initial_scout_pos = P(34, 122);

		main_scout_path = { P(19, 135.5),
							P(28, 148.5),
							P(41.5, 140),
							P(39, 128) };

		natural_scout_path = { P(22.5, 118.5),
								P(20.5, 107.5),
								P(32, 104), };

		possible_3rds = { P(61.5, 134.5),
						P(28.5, 73.5) };

		enemy_natural = P(29.5, 111.5);

		/*
		Point2D entrance_point = P(75, 137);
		Point2D exit_point = P(28, 35);
		std::vector<Point2D> entrance_points = { P(50, 129),
												P(40, 102),
												P(38, 71) };
		std::vector<Point2D> exit_points = { P(37, 116),
											P(33, 85),
											P(39, 53) };
		std::vector<Point2D> base_points = { P(46.5, 119.5),
											P(39.5, 91.5),
											P(69.5, 100.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(28, 106),
															P(30, 132),
															P(67, 137)},
															{ P(28, 76),
															P(28, 43)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);



		third_base_pylon_gap = P(151.5, 102);
		natural_door_closed = P(173.5, 91.5);
		natural_door_open = P(171, 91.5);
		*/

		/*adept_harrass_protoss_consolidation = { P(, ), P(, ) };
		
		main_early_dead_space = P(160, 125);
		natural_front = { P(173, 86),
							P(171, 92) };


		adept_scout_shade = P(54, 84);
		adept_scout_runaway = P(53, 73);
		adept_scout_ramptop = P(43.5, 105);
		adept_scout_nat_path = { P(30, 89),
									P(40, 96) };
		adept_scout_base_spots = { P(35, 115),
									P(44, 129),
									P(60, 125),
									P(60, 111) };


		blink_presure_consolidation = P(72, 86);
		blink_pressure_prism_consolidation = P(75, 83);
		blink_pressure_blink_up = P(67.5, 107);
		blink_pressure_blink_down = P(64, 111);


		blink_nat_attacK_path_line = PathManager({ new LineSegmentLinearX(-.069, 91.04, 57, 72, swap, center_point, false),
													new LineSegmentCurveX(.04, -4.696, 224.828, 52, 57, swap, center_point, false),
													new LineSegmentLinearX(-.4, 109.596, 36, 52, swap, center_point, false) }, true, swap);

		blink_main_attack_path_lines = { PathManager({ new LineSegmentLinearX(-.1, 117.4, 45, 64, swap, center_point, false) }, true, swap),
										PathManager({ new LineSegmentCurveY(-.04, 8.88, -428.84, 111, 127, swap, center_point, false) }, false, true) };
*/

	}
	
}

