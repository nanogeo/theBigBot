#pragma once
#include "locations.h"
#include "TossBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetSiteDeltaLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(100, 100);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(200, 200) - point;
			else
				return point;
		};

		bool swap = start_location.x == 50.5 && start_location.y == 157.5;

		this->start_location = start_location;

		nexi_locations = { P(149.5, 42.5),
							P(154.5, 69.5),
							P(153.5, 101.5),
							P(124.5, 65.5),
							P(107.5, 42.5),
							P(82.5, 59.5),
							P(156.5, 133.5),
							P(153.5, 159.5) };

		pylon_locations = { P(152, 106),
							P(148, 46),
							P(154, 73),
							P(150, 99),
							P(123, 58),
							P(121, 64),
							P(153, 134),
							P(106, 46),
							P(152,156 ),
							P(86, 59),
							P(148, 48),
							P(132, 42),
							P(148, 35),
							P(158, 43),
							P(159, 63),
							P(161, 75) };

		first_pylon_location_zerg = P(149, 72);
		first_pylon_location_protoss = P(141, 57);
		first_pylon_location_terran = P(139, 59);

		gateway_locations = { P(139.75, 61.25),
							P(142.5, 61.5),
							P(144.5, 72.5),
							P(148.5, 74.5),
							P(148.5, 110.5),
							P(148.5, 107.5),
							P(149.5, 104.5),
							P(149.5, 101.5),
							P(119.5, 54.5),
							P(117.5, 57.5),
							P(117.5, 60.5),
							P(118.5, 63.5),
							P(138.5, 55.5),
							P(135.5, 55.5),
							P(132.5, 39.5),
							P(135.5, 39.5),
							P(130.5, 45.5),
							P(133.5, 45.5),
							P(145.5, 30.5),
							P(148.5, 30.5),
							P(151.5, 30.5),
							P(161.5, 44.5),
							P(161.5, 41.5) };

		assimilator_locations = { P(145.5, 35.5),
									P(156.5, 45.5),
									P(161.5, 72.5),
									P(158.5, 76.5),
									P(155.5, 108.5),
									P(155.5, 94.5),
									P(131.5, 68.5),
									P(120.5, 58.5),
									P(114.5, 45.5),
									P(103.5, 35.5),
									P(86.5, 52.5),
									P(75.5, 62.5),
									P(163.5, 136.5),
									P(152.5, 126.5),
									P(160.5, 156.5),
									P(149.5, 166.5) };


		cyber_core_locations = { P(136.75, 58.25),
								P(143.5, 58.5),
								P(149.5, 77.5) };

		tech_locations = { P(150.5, 46.5),
							P(150.5, 49.5),
							P(145.5, 41.5),
							P(145.5, 44.5),
							P(145.5, 47.5),
							P(145.5, 50.5),
							P(142.5, 44.5),
							P(142.5, 47.5) };

		proxy_pylon_locations = { P(101, 134) };

		proxy_gateway_locations = { P(103.5, 134.5) };

		proxy_tech_locations = {  };


		attack_path = { P(87, 66),
						P(60, 89),
						P(57, 92),
						P(53, 99),
						P(55, 105),
						P(59, 109),
						P(61.5, 115),
						P(60, 118.5),
						P(56, 122.5),
						P(52, 126),
						P(50, 130),
						P(52, 135),
						P(57, 141) };

		attack_path_line = PathManager({new LineSegmentLinearY(-1.15, 163, 66, 92, swap, center_point, true),
										new LineSegmentCurveY(.07, -13.965, 749.504, 92, 106, swap, center_point, true),
										new LineSegmentLinearY(1, -50.25, 106, 110, swap, center_point, true),
										new LineSegmentCurveY(-.1, 22.86, -1244.85, 110, 120, swap, center_point, true),
										new LineSegmentLinearY(-1, 178.4, 120, 127, swap, center_point, true),
										new LineSegmentCurveY(.11, -28.732, 1926.15, 127, 134.5, swap, center_point, true),
										new LineSegmentLinearY(.85, -62.7, 134.5, 142, swap, center_point, true)}, false, swap);

		high_ground_index = 12;

		attack_path_alt = { P(135, 115),
							P(107, 142),
							P(96, 147),
							P(87, 143),
							P(75, 130),
							P(68, 120),
							P(60, 118.5),
							P(56, 122.5),
							P(52, 126),
							P(50, 130),
							P(52, 135),
							P(57, 141) };

		high_ground_index_alt = 11;

		initial_scout_pos = P(55, 138.5);

		main_scout_path = { P(52, 148),
							P(45, 150),
							P(43, 163),
							P(58, 166),
							P(65, 155) };

		natural_scout_path = { P(45, 140),
								P(36, 128),
								P(46, 121) };

		possible_3rds = { P(75.5, 134.5), P(46.5, 98.5)};

		enemy_natural = P(45.5, 130.5);


		Point2D entrance_point = P(75, 170);
		Point2D exit_point = P(35, 75);
		std::vector<Point2D> entrance_points = { P(56, 168),
												P(46, 142),
												P(43, 109) };
		std::vector<Point2D> exit_points = { P(40, 153),
											P(38, 123),
											P(43, 89) };
		std::vector<Point2D> base_points = { P(50.5, 157.5),
											P(45.5, 130.5),
											P(46.5, 98.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(35, 143),
															P(38, 172),
															P(60, 172)},
															{ P(35, 113),
															P(35, 82)} };


		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);



		third_base_pylon_gap = P(152, 104.5);
		natural_door_closed = P(146.5, 73.5);
		natural_door_open = P(146.5, 76);


		adept_harrass_protoss_consolidation = { P(71, 125), P(54, 105) };


		main_early_dead_space = P(133, 39);
		natural_front = { P(142.5, 75.5),
							P(146, 79) };


		adept_scout_shade = P(56, 107);
		adept_scout_runaway = P(56, 96);
		adept_scout_ramptop = P(55, 138.5);
		adept_scout_nat_path = { P(36, 130),
								P(46, 139) };
		adept_scout_base_spots = { P(41, 157),
									P(50, 167),
									P(63, 161),
									P(68, 149) };


		blink_presure_consolidation = P(81, 125);
		blink_pressure_prism_consolidation = P(88, 121);
		blink_pressure_blink_up = P(72, 142);
		blink_pressure_blink_down = P(66, 146);


		blink_nat_attacK_path_line = PathManager({ new LineSegmentLinearX(.52, 82.88, 70, 81, swap, center_point, true),
													new LineSegmentCurveX(.05, -6.5, 329.25, 50, 70, swap, center_point, true)}, true, swap);

		blink_main_attack_path_lines = { PathManager({ new LineSegmentLinearX(.26, 128.84, 55, 66, swap, center_point, true) }, true, swap),
										PathManager({ new LineSegmentCurveY(-.02, 5.84, -360.32, 146, 167, swap, center_point, true) }, false, !swap) };
	}
	
}

