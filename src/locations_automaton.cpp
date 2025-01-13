#pragma once
#include "locations.h"
#include "theBigBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetAutomatonLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(224, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(184, 180) + point;
			else
				return point;
		};

		bool swap = start_location.x == 154.5 && start_location.y == 114.5;

		this->start_location = start_location;

		nexi_locations = {P(148.5, 145.5),
							P(127.5, 114.5),
							P(120.5, 153.5),
							P(85.5, 153.5),
							P(54.5, 151.5),
							P(150.5, 74.5) };

		pylon_locations = { P(126, 110),
							P(132, 108),
							P(147, 153),
							P(151, 116),
							P(157, 145),
							P(119, 161),
							P(129, 153),
							P(151, 118),
							P(157, 96),
							P(141, 96),
							P(138, 114) };

		std::cerr << "First pylon: " + std::to_string(pylon_locations[0].x) + ", " + std::to_string(pylon_locations[0].y) + "\n";

		first_pylon_location_zerg = P(139, 137);
		first_pylon_location_protoss = P(137, 126);
		first_pylon_location_terran = P(155, 127);

		gateway_locations = { P(153.5, 129.5),
							P(156.5, 129.5),
							P(136.5, 123.5),
							P(139.5, 124.5),
							P(136.5, 136.5),
							P(138.5, 133.5),
							P(126.5, 107.5),
							P(123.5, 108.5),
							P(123.5, 111.5),
							P(123.5, 114.5),
							P(154.5, 96.5),
							P(154.5, 99.5),
							P(159.5, 99.5),
							P(138.5, 99.5),
							P(144.5, 96.5),
							P(144.5, 99.5),
							P(144.5, 99.5),
							P(138.5, 116.5),
							P(141.5, 116.5),
							P(138.5, 111.5),
							P(138.5, 108.5),
							P(138.5, 119.5) };

		assimilator_locations = { P(161.5, 117.5),
									P(157.5, 121.5),
									P(144.5, 145.5),
									P(155.5, 142.5),
									P(129.5, 107.5),
									P(129.5, 121.5),
									P(116.5, 160.5),
									P(127.5, 150.5),
									P(78.5, 154.5),
									P(92.5, 154.5),
									P(58.5, 158.5),
									P(47.5, 148.5),
									P(146.5, 81.5),
									P(157.5, 71.5) };


		cyber_core_locations = { P(152.5, 126.5),
									P(139.5, 127.5),
									P(136.5, 140.5) };

		tech_locations = { P(150.5, 113.5),
							P(150.5, 110.5),
							P(148.5, 116.5),
							P(148.5, 119.5),
							P(153.5, 118.5),
							P(153.5, 121.5),
							P(147.5, 113.5) };

		proxy_pylon_locations = {P(96, 50),
								P(89, 81) };

		proxy_gateway_locations = { P(93.5, 49.5),
									P(89.5, 83.5) };

		proxy_tech_locations = { P(96.5, 52.5),
								P(93.5, 79.5) };


		attack_path = { P(68, 132),
						P(62, 126),
						P(48, 116),
						P(45, 112),
						P(50, 99),
						P(53, 95),
						P(59, 85),
						P(60, 79),
						P(60, 54),
						P(49, 44),
						P(45, 44),
						P(36, 48),
						P(33, 51),
						P(34, 69) };

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

		initial_scout_pos = P(33.5, 50.5);

		main_scout_path = { P(22, 60.5),
							P(22, 76.5),
							P(39.5, 73),
							P(38.5, 59.5) };

		natural_scout_path = { P(26, 38.5),
								P(34, 26),
								P(42.5, 34) };

		possible_3rds = { P(56.5, 65.5),
						P(63.5, 26.5), };

		enemy_natural = P(35.5, 34.5);

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

		/*adept_harrass_protoss_consolidation = {P(,), P(,)};
		
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

