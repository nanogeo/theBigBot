#pragma once
#include "locations.h"
#include "theBigBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetGresvanLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(100, 82);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(200, 164) - point;
			else
				return point;
		};

		bool swap = start_location.x == 38.5 && start_location.y == 121.5;

		this->start_location = start_location;

		nexi_locations = { P(161.5, 42.5),
							P(163.5, 74.5),
							P(137.5, 64.5),
							P(163.5, 99.5),
							P(134.5, 91.5),
							P(119.5, 39.5),
							P(145.5, 124.5),
							P(117.5, 124.5) };

		pylon_locations = { P(160, 46),
							P(164, 104),
							P(160, 98),
							P(133, 63),
							P(144, 121),
							P(118, 43),
							P(136, 95),
							P(160, 48),
							P(118, 121),
							P(141, 44),
							P(155, 37),
							P(173, 42),
							P(167, 67),
							P(168, 79) };

		first_pylon_location_zerg = P(156, 76);
		first_pylon_location_protoss = P(167, 55);
		first_pylon_location_terran = P(147, 56);

		gateway_locations = { P(165.5, 59.5),
							P(171.5, 56.5),
							P(148.5, 58.5),
							P(153.5, 80.5),
							P(153.5, 76.5),
							P(164.5, 109.5),
							P(163.5, 106.5),
							P(161.5, 103.5),
							P(159.5, 100.5),
							P(132.5, 60.5),
							P(130.5, 63.5),
							P(132.5, 66.5),
							P(149.5, 55.5),
							P(151.5, 58.5),
							P(144.5, 51.5),
							P(142.5, 46.5),
							P(145.5, 46.5),
							P(142.5, 41.5),
							P(145.5, 41.5),
							P(154.5, 34.5),
							P(151.5, 34.5),
							P(170.5, 37.5),
							P(171.5, 44.5) };

		assimilator_locations = { P(157.5, 35.5),
									P(154.5, 39.5),
									P(170.5, 77.5),
									P(166.5, 81.5),
									P(144.5, 67.5),
									P(134.5, 57.5),
									P(164.5, 92.5),
									P(160.5, 92.5),
									P(136.5, 84.5),
									P(127.5, 93.5),
									P(126.5, 42.5),
									P(115.5, 32.5),
									P(149.5, 131.5),
									P(152.5, 127.5),
									P(124.5, 121.5),
									P(113.5, 131.5) };


		cyber_core_locations = { P(164.5, 56.5),
								P(144.5, 54.5),
								P(152.5, 73.5) };

		tech_locations = { P(162.5, 46.5),
							P(162.5, 49.5),
							P(157.5, 42.5),
							P(157.5, 45.5),
							P(157.5, 48.5),
							P(157.5, 51.5),
							P(154.5, 45.5),
							P(154.5, 48.5) };

		proxy_pylon_locations = { P(84, 61) };

		proxy_gateway_locations = { P(86.5, 61.5) };

		proxy_tech_locations = {  };


		attack_path = { P(80, 50),
						P(50, 85),
						P(45, 90),
						P(43, 95),
						P(42.5, 100),
						P(36, 106) };

		attack_path_line = PathManager({new LineSegmentLinearY(-.86, 123, 50, 90, swap, center_point, false),
										new LineSegmentCurveY(.07, -13.538, 697.062, 90, 97, swap, center_point, false),
										new LineSegmentCurveY(-.075, 14.505, -658.817, 97, 105, swap, center_point, false), 
										new LineSegmentLinearY(.2, 16.333, 105, 127, swap, center_point, false) }, false, !swap);

		high_ground_index = 5;

		attack_path_alt = { P(105, 127),
							P(83, 120),
							P(65, 98),
							P(45, 90),
							P(43, 95),
							P(42.5, 100),
							P(36, 106),
							P(50, 85),
							P(45, 90),
							P(43, 95),
							P(42.5, 100),
							P(36, 106) };

		high_ground_index_alt = 12;

		initial_scout_pos = P(37, 105);

		main_scout_path = { P(30, 110),
							P(28, 126),
							P(40, 132),
							P(53, 120),
							P(48, 108) };

		natural_scout_path = { P(33, 98),
								P(27, 89),
								P(35, 80) };

		possible_3rds = { P(62.5, 99.5), P(36.5, 64.5) };

		enemy_natural = P(36.5, 89.5);


		Point2D entrance_point = P(65, 136);
		Point2D exit_point = P(25, 35);
		std::vector<Point2D> entrance_points = { P(50, 130),
												P(30, 100),
												P(34, 75) };
		std::vector<Point2D> exit_points = { P(27, 117),
											P(34, 78),
											P(34, 58) };
		std::vector<Point2D> base_points = { P(38.5, 121.5),
											P(36.5, 89.5),
											P(62.5, 99.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(23, 107),
															P(23, 136),
															P(50, 136)},
															{ P(23, 77),
															P(23, 48)} };


		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);



		third_base_pylon_gap = P(134.5, 63);
		natural_door_closed = P(153, 78.5);
		natural_door_open = P(151, 78.5);


		adept_harrass_protoss_consolidation = { P(55, 71), P(62, 95) };

		main_early_dead_space = P(144, 41);
		natural_front = { P(151, 73),
		P(153, 82) };


		adept_scout_shade = P(55, 77);
		adept_scout_runaway = P(60, 68);
		adept_scout_ramptop = P(37, 105);
		adept_scout_nat_path = { P(27, 86),
									P(35, 98) };
		adept_scout_base_spots = { P(28, 115),
									P(34, 130),
									P(50, 129),
									P(54, 111) };


		blink_presure_consolidation = P(69, 96);
		blink_pressure_prism_consolidation = P(73, 100);
		blink_pressure_blink_up = P(57.5, 105.5);
		blink_pressure_blink_down = P(55, 108);

		blink_nat_attacK_path_line = PathManager({ new LineSegmentLinearX(.585, 55.635, 57, 69, swap, center_point, true),
													new LineSegmentCurveX(.03, -2.8745, 155.355, 32, 57, swap, center_point, true) }, true, swap);

		blink_main_attack_path_lines = { PathManager({ new LineSegmentLinearX(-.3, 124.5, 28, 55, swap, center_point, true) }, true, swap),
										PathManager({ new LineSegmentCurveY(-.02, 4.32, -178.28, 108, 130, swap, center_point, true) }, false, !swap) };

	}
	
}

