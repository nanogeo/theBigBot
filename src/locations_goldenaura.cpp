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

		bool swap = start_location.x == 46.5 && start_location.y == 119.5;

		this->start_location = start_location;

		nexi_locations = { P(170.5, 67.5),
							P(148.5, 59.5),
							P(123.5, 55.5),
							P(142.5, 89.5),
							P(168.5, 114.5),
							P(104.5, 61.5) };

		pylon_locations = { P(124, 60),
							P(144, 93),
							P(139, 89),
							P(169, 71),
							P(169, 73),
							P(174, 60),
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
								P(119.5, 57.5),
								P(121.5, 60.5),
								P(124.5, 62.5),
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

		assimilator_locations = {  };


		cyber_core_locations = { P(157.5, 72.5),
								P(156.5, 82.5),
								P(141.5, 64.5) };

		tech_locations = { P(166.5, 66.5),
							P(166.5, 69.5),
							P(166.5, 72.5),
							P(171.5, 71.5),
							P(171.5, 74.5),
							P(166.5, 75.5),
							P(163.5, 69.5) };

		proxy_pylon_locations = {  };

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
			attack_path_line = PathManager({ new LineSegmentLinearX(-1.5, 241, 97, 116, false, center_point, true),
											new LineSegmentCurveX(.08, -20, 1310.4, 114, 128, false, center_point, true),
											new LineSegmentCurveX(-.06, 16.5, 1068.68, 130, 142, false, center_point, true),
											new LineSegmentCurveX(.1, -28.92, 2154.72, 142, 149.5, false, center_point, true),
											new LineSegmentLinearX(.93, -72.86, 149.5, 157, false, center_point, true) }, true, true);
		}
		else
		{
			attack_path_line = PathManager({ new LineSegmentLinearY(-1.5, 240, 95, 114, false, center_point, true),
											new LineSegmentCurveY(.08, -19.68, 1272.72, 114, 128, false, center_point, true),
											new LineSegmentCurveY(-.06, 16.26, -1033.92, 128, 140, false, center_point, true),
											new LineSegmentCurveY(.1, -28.52, 2099.28, 140, 147.5, false, center_point, true),
											new LineSegmentLinearY(.93, -69, 147.5, 155, false, center_point, true)}, false, true);
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

		//initial_scout_pos = P(, );

		main_scout_path = { P(60, 163),
							P(65, 177),
							P(85, 170),
							P(82, 157) };

		natural_scout_path = { P(58, 155),
								P(53, 145),
								P(63, 140) };

		possible_3rds = { P(57.5, 121.5), P(91.5, 140.5)};

		enemy_natural = P(61.5, 146.5);


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



		third_base_pylon_gap = P(124, 58.5);
		natural_door_closed = P(141, 62.5);
		natural_door_open = P(138, 62.5);



		adept_harrass_protoss_consolidation = { P(82, 135), P(62, 125) };


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

