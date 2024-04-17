#pragma once
#include "locations.h"
#include "theBigBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetHardLeadLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(84, 82);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(168, 164) - point;
			else
				return point;
		};

		bool swap = start_location.x == 37.5 && start_location.y == 129.5;

		this->start_location = start_location;

		nexi_locations = { P(130.5, 34.5),
							P(102.5, 30.5),
							P(107.5, 57.5),
							P(72.5, 29.5),
							P(74.5, 54.5),
							P(132.5, 74.5),
							P(32.5, 29.5) };

		pylon_locations = { P(129, 38),
							P(66, 34),
							P(63, 32),
							P(109, 62),
							P(104, 59),
							P(99, 32),
							P(78, 53),
							P(73, 58),
							P(36, 28),
							P(101, 23),
							P(111, 28),
							P(129, 74),
							P(129, 40),
							P(129, 27),
							P(145, 40),
							P(134, 50),
							P(124, 34),
							P(117, 43),
							P(139, 35),
							P(132, 48) };

		first_pylon_location_zerg = P(99, 37);
		first_pylon_location_protoss = P(117, 43);
		first_pylon_location_terran = P(122, 50);

		gateway_locations = { P(112.5, 41.5),
								P(118.5, 39.5),
								P(119.5, 49.5),
								P(99.5, 41.5),
								P(95.5, 39.5),
								P(60.5, 32.5),
								P(63.5, 34.5),
								P(66.5, 36.5),
								P(67.5, 39.5),
								P(106.25, 61.75),
								P(111.5, 63.75),
								P(114.5, 62.75),
								P(108.5, 64.5),
								P(129.5, 24.5),
								P(133.5, 26.5),
								P(141.5, 37.5),
								P(141.5, 40.5),
								P(141.5, 43.5),
								P(137.5, 47.5),
								P(134.5, 47.5),
								P(124.5, 49.5),
								P(127.5, 50.5),
								P(127.5, 47.5) };

		assimilator_locations = { P(126.5, 27.5),
									P(137.5, 37.5),
									P(98.5, 23.5),
									P(95.5, 27.5),
									P(68.5, 22.5),
									P(65.5, 26.5),
									P(103.5, 50.5),
									P(114.5, 59.5),
									P(76.5, 47.5),
									P(67.5, 55.5),
									P(134.5, 67.5),
									P(130.5, 67.5),
									P(30.5, 22.5),
									P(34.5, 22.5) };


		cyber_core_locations = { P(115.5, 39.5),
								P(122.75, 52.25),
								P(94.5, 36.5) };

		tech_locations = { P(126.5, 32.5),
							P(126.5, 35.5),
							P(126.5, 38.5),
							P(126.5, 41.5),
							P(131.5, 38.5),
							P(131.5, 41.5),
							P(123.5, 36.5),
							P(123.5, 39.5) };

		proxy_pylon_locations = { P(108, 118) };

		proxy_gateway_locations = { P(110.5, 118.5) };

		proxy_tech_locations = {  };


		attack_path = { P(130, 103),
						P(108, 125),
						P(103, 130),
						P(96, 130),
						P(90, 125),
						P(86, 121),
						P(83, 119),
						P(79, 118.5),
						P(75, 120),
						P(67, 128),
						P(63, 130.5),
						P(60, 129),
						P(54, 124),
						P(49, 121.5),
						P(43, 125) };

		attack_path_line = PathManager({new LineSegmentLinearX(-1, 233, 105, 130, swap, center_point, true),
										new LineSegmentCurveX(-.08, 15.824, -651.497, 92, 105, swap, center_point, true),
										new LineSegmentLinearX(1, 35.191, 86, 92, swap, center_point, true),
										new LineSegmentCurveX(.08, -12.8, 630.3, 74, 86, swap, center_point, true),
										new LineSegmentLinearX(-1, 195.2, 66, 74, swap, center_point, true),
										new LineSegmentCurveX(-.16, 20.16, -504.38, 60, 66, swap, center_point, true),
										new LineSegmentLinearX(.88, 76.4, 53, 60, swap, center_point, true),
										new LineSegmentCurveX(.1, -9.8, 361.5, 43, 53, swap, center_point, true)}, true, swap);

		high_ground_index = 12;

		attack_path_alt = { P(70, 64),
							P(55, 65),
							P(50, 90),
							P(62, 105),
							P(75, 120),
							P(67, 128),
							P(63, 130.5),
							P(60, 129),
							P(54, 124),
							P(49, 121.5),
							P(43, 125) };

		high_ground_index_alt = 8;

		initial_scout_pos = P(56, 125.5);

		main_scout_path = { P(42, 118),
							P(28, 123),
							P(32, 137),
							P(43, 139),
							P(45, 125) };

		natural_scout_path = { P(57, 140),
								P(72, 143),
								P(72, 129) };

		possible_3rds = { P(60.5, 106.5), P(95.5, 134.5)};

		enemy_natural = P(65.5, 133.5);


		Point2D entrance_point = P(20, 105);
		Point2D exit_point = P(120, 145);
		std::vector<Point2D> entrance_points = { P(27, 125),
												P(53, 135),
												P(84, 136) };
		std::vector<Point2D> exit_points = { P(43, 141),
											P(76, 139),
											P(105, 140) };
		std::vector<Point2D> base_points = { P(37.5, 129.5),
											P(65.5, 133.5),
											P(95.5, 134.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(50, 145),
															P(25, 142),
															P(20, 120)},
															{ P(81, 145),
															P(110, 145)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);



		third_base_pylon_gap = P(109, 60.5);
		natural_door_closed = P(97.5, 40.5);
		natural_door_open = P(97.5, 42.5);


		adept_harrass_protoss_consolidation = { P(90, 125), P(68, 107) };


		main_early_dead_space = P(134, 50);
		natural_front = { P(92, 39),
							P(97, 44) };


		adept_scout_shade = P(83, 121);
		adept_scout_runaway = P(95, 124);
		adept_scout_ramptop = P(56, 125.5);
		adept_scout_nat_path = { P(67, 142),
								P(56, 134) };
		adept_scout_base_spots = { P(36, 115),
									P(26, 124),
									P(33, 139),
									P(46, 138) };


		blink_presure_consolidation = P(69, 104);
		blink_pressure_prism_consolidation = P(67, 95);
		blink_pressure_blink_up = P(50, 108);
		blink_pressure_blink_down = P(47, 113);



		blink_nat_attacK_path_line = PathManager({ new LineSegmentLinearY(.535, 13.36, 104, 112, swap, center_point, true), 
													new LineSegmentCurveY(-.05, 11.78, -618.842, 112, 135, swap, center_point, true) }, false, !swap);

		blink_main_attack_path_lines = { PathManager({ new LineSegmentLinearY(.1, 35.7, 113, 135, swap, center_point, true) }, false, !swap),
										PathManager({ new LineSegmentCurveX(.03, -2.82, 179.27, 27, 47, swap, center_point, true) }, true, !swap) };
	}
	
}

