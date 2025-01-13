#pragma once
#include "locations.h"
#include "theBigBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetAbyssalReefLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(224, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(200, 144) - point;
			else
				return point;
		};

		bool swap = start_location.x == 161.5 && start_location.y == 21.5;

		this->start_location = start_location;

		nexi_locations = { P(129.5, 26.5),
							P(157.5, 50.5),
							P(141.5, 65.5),
							P(129.5, 49.5),
							P(100.5, 28.5),
							P(159.5, 99.5) };

		pylon_locations = { P(159, 55),
							P(121, 24),
							P(130, 19),
							P(161, 43),
							P(166, 48),
							P(160, 25),
							P(158, 25),
							P(32, 109),
							P(59, 132),
							P(36, 133),
							P(27, 118) };

		std::cerr << "First pylon: " + std::to_string(pylon_locations[0].x) + ", " + std::to_string(pylon_locations[0].y) + "\n";

		first_pylon_location_zerg = P(136, 39);
		first_pylon_location_protoss = P(147, 21);
		first_pylon_location_terran = P(158, 37);

		gateway_locations = { P(144.5, 22.5),
							P(147.5, 23.5),
							P(155.5, 35.5),
							P(158.5, 34.5),
							P(139.5, 37.5),
							P(133.5, 37.5),
							P(156.5, 60.5),
							P(159.5, 58.5),
							P(162.5, 56.5),
							P(30.5, 112.5),
							P(33.5, 112.5),
							P(36.5, 112.5),
							P(56.5, 131.5),
							P(53.5, 131.5),
							P(57.5, 127.5),
							P(54.5, 127.5),
							P(33.5, 131.5),
							P(39.5, 132.5),
							P(28.5, 115.5),
							P(31.5, 115.5),
							P(28.5, 120.5),
							P(28.5, 123.5) };

		assimilator_locations = { P(158.5, 14.5),
									P(168.5, 24.5),
									P(132.5, 19.5),
									P(136.5, 23.5),
									P(154.5, 43.5),
									P(164.5, 53.5),
									P(139.5, 72.5),
									P(148.5, 62.5),
									P(132.5, 42.5),
									P(136.5, 46.5),
									P(107.5, 27.5),
									P(107.5, 31.5),
									P(160.5, 106.5),
									P(162.5, 92.5) };


		cyber_core_locations = { P(144.5, 22.5),
									P(160.5, 37.5),
									P(131.5, 34.5) };

		tech_locations = { P(162.5, 25.5),
							P(165.5, 25.5),
							P(159.5, 27.5),
							P(156.5, 27.5),
							P(157.5, 22.5),
							P(154.5, 22.5) };

		proxy_pylon_locations = {P(106, 97),
								P(70, 83) };

		proxy_gateway_locations = { P(103.5, 98.5),
									P(69.5, 80.5) };

		proxy_tech_locations = { P(106.5, 99.5),
								P(66.5, 83.5) };


		attack_path = { P(81, 38),
						P(76, 41),
						P(62, 55),
						P(54, 60),
						P(46, 65),
						P(38, 75),
						P(39, 84),
						P(41, 87),
						P(55, 100),
						P(61, 104),
						P(59, 115),
						P(56, 118),
						P(40, 123), };

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

		attack_path_alt = { P(,),
							P(,), };

		high_ground_index_alt = 6;*/

		initial_scout_pos = P(55.5, 119.5);

		main_scout_path = { P(49, 129),
							P(37, 132),
							P(30, 126.5),
							P(30, 117),
							P(42.5, 113.5) };

		natural_scout_path = { P(64, 125),
								P(75, 125.5),
								P(76.5, 114), };

		possible_3rds = { P(70.5, 94.5),
						P(42.5, 93.5), };

		enemy_natural = P(70.5, 117.5);

		
		/*Point2D entrance_point = P(75, 137);
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


		third_base_pylon_gap = P(158.5, 53.5);
		natural_door_closed = P(137.5, 38.5);
		natural_door_open = P(137.5, 42);
		

		/*adept_harrass_protoss_consolidation = { P(,), P(,) };
		
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

