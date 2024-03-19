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
							P(150, 99),
							P(123, 58),
							P(121, 64),
							P(153, 134),
							P(148, 46),
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

		gateway_locations = { P(142.5, 61.5),
							P(139.5, 61.5),
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
							P(132.5, 39.5),
							P(135.5, 39.5),
							P(130.5, 45.5),
							P(133.5, 45.5),
							P(145.5, 30.5),
							P(148.5, 30.5),
							P(151.5, 30.5),
							P(161.5, 44.5),
							P(161.5, 41.5) };

		assimilator_locations = {  };


		cyber_core_locations = { P(143.5, 58.5),
								P(136.5, 58.5),
								P(149.5, 77.5) };

		tech_locations = { P(150.5, 48.5),
							P(150.5, 49.5),
							P(145.5, 41.5),
							P(145.5, 44.5),
							P(145.5, 17.5),
							P(145.5, 50.5),
							P(142.5, 44.5),
							P(142.5, 47.5) };

		proxy_pylon_locations = {  };

		proxy_gateway_locations = {  };

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

		//initial_scout_pos = P(, );

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


		/*Point2D entrance_point = P(, );
		Point2D exit_point = P(, );
		std::vector<Point2D> entrance_points = { P(, ),
												 P(, ),
												 P(, ) };
		std::vector<Point2D> exit_points = { P(, ),
											P(, ),
											P(, ) };;
		std::vector<Point2D> base_points = { P(, ),
											P(, ),
											P(, ) };;
		std::vector<std::vector<Point2D>> exfi_paths = { {P(, ),
															P(, ),
															P(, )},
															{ P(, ),
															P(, )} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);*/



		third_base_pylon_gap = P(152, 104.5);
		natural_door_closed = P(146.5, 73.5);
		natural_door_open = P(146.5, 76);


		adept_harrass_protoss_consolidation = { P(71, 125), P(54, 105) };


		main_early_dead_space = P(, );
		natural_front = { P(,),
							P(,) };

		base_defense_front = P(, );
		base_defense_main = P(, );
		base_defense_nat = P(, );
		base_defense_center = P(, );

		adept_scout_shade = P(, );
		adept_scout_runaway = P(, );
		adept_scout_ramptop = P(, );
		adept_scout_nat_path = { P(,),
									P(,) };
		adept_scout_base_spots = { P(,),
									P(,) };

		blink_presure_consolidation = P(, );
		blink_pressure_prism_consolidation = P(, );
		blink_pressure_blink_up = P(, );
		blink_pressure_blink_down = P(, );

		blink_nat_attacK_path_line = PathManager({ new  swap, center_point, true),
													new  swap, center_point, true),
													new  swap, center_point, true),
													new  swap, center_point, true), }, false, !swap);

		blink_main_attack_path_lines = { PathManager({ new  swap, center_point, true) }, false, !swap),
										PathManager({ new  swap, center_point, true) }, false, swap) };
	}
	
}

