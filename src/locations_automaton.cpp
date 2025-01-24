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
				return Point2D(184, 180) - point;
			else
				return point;
		};

		bool swap = start_location.x == 29.5 && start_location.y == 65.5;

		this->start_location = start_location;

		nexi_locations = {P(148.5, 145.5),
							P(127.5, 114.5),
							P(120.5, 153.5),
							P(85.5, 153.5),
							P(54.5, 151.5),
							P(150.5, 74.5) };

		pylon_locations = { P(151, 116),
							P(126, 110),
							P(141, 96),
							P(132, 108),
							P(147, 153),
							P(157, 145),
							P(119, 161),
							P(129, 153),
							P(151, 118),
							P(157, 96),
							P(138, 114),
							P(140, 114),
							P(157, 98),
							P(118, 149),
							P(143, 146),
							P(144, 145),
							P(161, 109),
							P(140, 98) };

		std::cerr << "First pylon: " + std::to_string(pylon_locations[0].x) + ", " + std::to_string(pylon_locations[0].y) + "\n";

		first_pylon_location_zerg = P(139, 137);
		first_pylon_location_protoss = P(155, 127);
		first_pylon_location_terran = P(137, 126);

		gateway_locations = { P(153.5, 129.5),
							P(156.5, 129.5),
							P(136.5, 123.5),
							P(142.5, 127.5),
							P(139.5, 124.5),
							P(142.5, 124.5),
							P(139.5, 124.5),
							P(138.5, 133.5),
							P(136.5, 136.5),
							P(123.5, 108.5),
							P(126.25, 107.25),
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
									P(144.5, 152.5),
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
						P(60.5, 79),
						P(60, 54),
						P(49, 44.5),
						P(45, 44),
						P(36, 48),
						P(33, 51),
						P(34, 69) };

		attack_path_line = PathManager(attack_path);


		high_ground_index = 5;

		attack_path_alt = { P(98, 30),
							P(64, 28),
							P(38, 38),
							P(33, 71) };

		high_ground_index_alt = 2;

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

		Point2D entrance_point = P(24, 97);
		Point2D exit_point = P(91, 22);
		std::vector<Point2D> entrance_points = { P(27, 76),
												P(27, 39),
												P(53, 30) };
		std::vector<Point2D> exit_points = { P(25, 59),
											P(38, 27),
											P(68, 22) };
		std::vector<Point2D> base_points = { P(29.5, 65.5),
											P(35.5, 34.5),
											P(63.5, 26.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(20, 46),
															P(20, 86) },
															{ P(47, 19),
															P(83, 19) } };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);

		
		third_base_pylon_gap = P(126, 111.5);
		natural_door_closed = P(136, 138.5);
		natural_door_open = P(133, 138.5);
		

		adept_harrass_protoss_consolidation = { P(61, 53), P(66, 28) };

		main_early_dead_space = P(140, 100);
		natural_front = { P(136, 140),
							P(138, 133) };


		adept_scout_shade = P(52, 45);
		adept_scout_runaway = P(62, 54);
		adept_scout_ramptop = P(33.5, 51);
		adept_scout_nat_path = { P(35, 27),
									P(26, 36) };
		adept_scout_base_spots = { P(21, 62),
									P(25, 78),
									P(40, 80),
									P(43, 58) };


		blink_presure_consolidation = P(63, 59);
		blink_pressure_prism_consolidation = P(66, 61);
		blink_pressure_blink_up = P(52, 75);
		blink_pressure_blink_down = P(47, 76);


		std::vector<Point2D> blink_nat_attack_path = { P(63, 59),
														P(31, 33) };

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(47, 76),
																	P(40, 63),
																	P(31, 57),
																	P(23, 57) },
																	{ P(47, 76),
																	P(33, 78),
																	P(26, 70),
																	P(26, 62) } };

		blink_main_attack_path_lines = { PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1]) };


		bad_warpin_spots = { first_pylon_location_terran };

	}
	
}

