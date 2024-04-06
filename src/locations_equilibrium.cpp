#pragma once
#include "locations.h"
#include "TossBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetEquilibriumLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(224, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(224 - point.x, point.y);
			else
				return point;
		};

		bool swap = start_location.x == 46.5 && start_location.y == 119.5;

		this->start_location = start_location;

		nexi_locations = { P(177.5, 119.5),
							P(184.5, 91.5),
							P(154.5, 100.5),
							P(182.5, 61.5),
							P(157.5, 59.5),
							P(156.5, 34.5),
							P(128.5, 37.5),
							P(137.5, 122.5) };

		pylon_locations = { P(174, 116),
							P(181, 58),
							P(179, 64),
							P(150, 102),
							P(153, 97),
							P(183, 88),
							P(172, 116),
							P(163, 128),
							P(189, 98),
							P(182, 126),
							P(190, 86),
							P(190, 67),
							P(191, 58) };

		first_pylon_location_zerg = P(177, 91);
		first_pylon_location_protoss = P(185, 109);
		first_pylon_location_terran = P(161, 110);

		gateway_locations = { P(183.5, 104.5),
							P(172.5, 93.5),
							P(174.5, 89.5),
							P(159.5, 112.5),
							P(179.5, 52.5),
							P(178.5, 55.5),
							P(178.5, 58.5),
							P(178.5, 91.5),
							P(148.5, 104.5),
							P(147.5, 101.5),
							P(148.5, 98.5),
							P(150.5, 95.5),
							P(155.5, 96.5),
							P(164.5, 107.5),
							P(164.5, 110.5),
							P(164.5, 113.5),
							P(160.5, 124.5),
							P(163.5, 124.5),
							P(166.5, 124.5),
							P(166.5, 129.5),
							P(182.5, 129.5),
							P(179.5, 129.5),
							P(187.5, 124.5),
							P(189.5, 105.5),
							P(189.5, 108.5),
							P(189.5, 111.5) };

		assimilator_locations = { P(184.5, 116.5),
									P(173.5, 126.5),
									P(191.5, 88.5),
									P(187.5, 84.5),
									P(161.5, 97.5),
									P(150.5, 107.5),
									P(184.5, 68.5),
									P(184.5, 54.5),
									P(144.5, 119.5),
									P(133.5, 129.5),
									P(152.5, 27.5),
									P(149.5, 31.5),
									P(124.5, 30.5),
									P(121.5, 34.5) };


		cyber_core_locations = { P(182.5, 107.5),
									P(176.5, 86.5),
									P(160.5, 115.5) };

		tech_locations = { P(176.5, 115.5),
							P(179.5, 115.5),
							P(173.5, 118.5),
							P(170.5, 118.5),
							P(173.5, 121.5),
							P(173.5, 113.5),
							P(170.5, 113.5),
							P(176.5, 112.5) };

		proxy_pylon_locations = { P(84, 57),
								P(79, 115) };

		proxy_gateway_locations = { P(86.5, 56.5),
									P(81.5, 116.5) };

		proxy_tech_locations = { P(83.5, 54.5),
								P(81.5, 113.5) };


		attack_path = { P(105, 83),
						P(57, 87),
						P(52, 88),
						P(50, 93),
						P(48, 100),
						P(42, 106) };

		attack_path_line = PathManager({new LineSegmentLinearX(-.09, 92.8, 52.5, 105, swap, center_point, false),
										new LineSegmentCurveX(.864, -90.8928, 2478.55, 49.4, 52.5, swap, center_point, false),
										new LineSegmentCurveX(-.07, 5.11, 15.3085, 42, 49.4, swap, center_point, false)}, true, swap);

		high_ground_index = 5;

		attack_path_alt = { P(112, 130),
							P(90, 120),
							P(81, 107),
							P(62, 90),
							P(48, 90),
							P(48, 100),
							P(42, 106) };

		high_ground_index_alt = 6;

		initial_scout_pos = P(43.5, 105);

		main_scout_path = { P(38, 107),
							P(36, 116),
							P(38, 125),
							P(53, 130),
							P(60, 117),
							P(54, 110) };

		natural_scout_path = { P(35, 99),
								P(30, 89),
								P(38, 81), };

		possible_3rds = { P(69.5, 100.5),
						P(41.5, 61.5), };

		enemy_natural = P(39.5, 91.5);


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


		adept_harrass_protoss_consolidation = { P(67, 93), P(55, 74) };

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
													new LineSegmentCurveX(.04, -4.696, 224.828, 45, 57, swap, center_point, false) }, true, swap);

		blink_main_attack_path_lines = { PathManager({ new LineSegmentLinearX(-.1, 117.4, 45, 64, swap, center_point, false) }, true, swap),
										PathManager({ new LineSegmentCurveY(-.04, 8.88, -428.84, 111, 127, swap, center_point, false) }, false, true) };


	}
	
}

