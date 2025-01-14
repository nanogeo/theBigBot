#pragma once
#include "locations.h"
#include "theBigBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetAcropolisLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(88, 86);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(176, 172) - point;
			else
				return point;
		};

		bool swap = start_location.x == 33.5 && start_location.y == 138.5;

		this->start_location = start_location;



		nexi_locations = {P(31.5, 113.5),
							P(32.5, 85.5),
							P(29.5, 53.5),
							P(58.5, 111.5),
							P(73.5, 138.5),
							P(47.5, 28.5),
							P(68.5, 42.5) };

		pylon_locations = { P(34, 81), 
							P(34, 89),
							P(62, 112),
							P(30, 125),
							P(35, 135),
							P(29, 145),
							P(47, 146),
							P(23, 113),
							P(27, 120),
							P(27, 79),
							P(27, 92),
							P(37, 135) };

		std::cerr << "First pylon: " + std::to_string(pylon_locations[0].x) + ", " + std::to_string(pylon_locations[0].y) + "\n";

		first_pylon_location_zerg = P(34, 108);
		first_pylon_location_protoss = P(45, 125);
		first_pylon_location_terran = P(49, 134);

		gateway_locations = { P(43.5, 122.5),
							 P(45.5, 127.5),
							 P(51.5, 131.5),
							 P(38.5, 106.5),
							 P(34.5, 105.5),
							 P(34.5, 77.5),
							 P(36.5, 80.5),
							 P(36.5, 83.5),
							 P(36.5, 86.5),
							 P(63.5, 115.5),
							 P(64.5, 112.5),
							 P(62.5, 109.5),
							 P(59.5, 107.5),
							 P(27.5, 126.5),
							 P(27.5, 129.5),
							 P(30.5, 127.5),
							 P(30.5, 130.5),
							 P(26.5, 145.5),
							 P(29.5, 147.5),
							 P(49.5, 142.5),
							 P(46.5, 142.5),
							 P(43.5, 142.5),
							 P(43.5, 146.5) };

		assimilator_locations = { P(26.5, 135.5),
									P(37.5, 145.5),
									P(25.5, 110.5),
									P(28.5, 106.5),
									P(25.5, 81.5),
									P(30.5, 78.5),
									P(26.5, 60.5),
									P(22.5, 56.5),
									P(62.5, 118.5),
									P(51.5, 108.5),
									P(66.5, 135.5),
									P(77.5, 145.5),
									P(40.5, 31.5),
									P(51.5, 21.5),
									P(61.5, 46.5),
									P(70.5, 35.5) };


		cyber_core_locations = { P(31.5, 105.5),
									P(42.5, 125.5),
									P(51.5, 134.5) };

		tech_locations = {	P(32.5, 134.5),
							P(29.5, 134.5),
							P(35.5, 132.5),
							P(38.5, 132.5),
							P(32.5, 131.5),
							P(37.5, 137.5),
							P(40.5, 137.5),
							P(41.5, 132.5) };

		proxy_pylon_locations = {P(124, 109),
								P(93, 57) };

		proxy_gateway_locations = { P(124.5, 106.5),
									P(92.5, 54.5) };

		proxy_tech_locations = { P(127.5, 111.5),
								P(88.5, 56.5) };
		

		attack_path = { P(120.5,122),
						P(123.5,125.25),
						P(130,125.5),
						P(135,121),
						P(137.5,109.75),
						P(134,106),
						P(135,97.5),
						P(137.5,93.25),
						P(139.25,72.75),
						P(140.25,67.5),
						P(138,52.25),
						P(135.5,49.75),
						P(140,36),
						P(147,28) };

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
		/*
		high_ground_index = 5;

		attack_path_alt = { P(112, 130),
							P(90, 120),
							P(81, 107),
							P(62, 90),
							P(48, 90),
							P(48, 100),
							P(42, 106) };

		high_ground_index_alt = 6;
		*/
		initial_scout_pos = P(136, 50);

		main_scout_path = { P(152, 39),
							P(152, 29),
							P(140, 23),
							P(132, 32),
							P(133, 44) };

		natural_scout_path = { P(150, 52),
								P(154, 63),
								P(142, 138), };

		possible_3rds = { P(143.5, 86.5),
						P(117.5, 60.5), };

		enemy_natural = P(144.5, 59.5);

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

		*/

		third_base_pylon_gap = P(34, 82.5);
		natural_door_closed = P(36.5, 106);
		natural_door_open = P(36.5, 102);

		
		adept_harrass_protoss_consolidation = { P(140, 80), P(123, 69) };
		/*
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

