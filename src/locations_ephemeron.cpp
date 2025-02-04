#pragma once
#include "locations.h"
#include "theBigBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetEphemeronLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(224, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(160, 160) - point;
			else
				return point;
		};

		bool swap = start_location.x == 29.5 && start_location.y == 138.5;

		this->start_location = start_location;

		nexi_locations = { P(130.5, 21.5),
							P(130.5, 48.5),
							P(98.5, 25.5),
							P(68.5, 48.5),
							P(65.5, 18.5),
							P(28.5, 28.5),
							P(131.5, 86.5) };

		pylon_locations = { P(127, 25),
							P(94, 25),
							P(98, 18),
							P(112, 22),
							P(136, 55),
							P(107, 23),
							P(138, 44),
							P(129, 25),
							P(132, 9),
							P(143, 20),
							P(114, 22),
							P(141, 20),
							P(132, 11) };

		std::cerr << "First pylon: " + std::to_string(pylon_locations[0].x) + ", " + std::to_string(pylon_locations[0].y) + "\n";

		first_pylon_location_zerg = P(123, 48);
		first_pylon_location_protoss = P(121, 35);
		first_pylon_location_terran = P(116, 37);

		gateway_locations = { P(118.5, 37.5),
							P(118.5, 34.5),
							P(115.5, 31.5),
							P(118.5, 31.5),
							P(122.5, 37.5),
							P(123.5, 34.5),
							P(120.5, 52.5),
							P(120.5, 48.5),
							P(88.75, 27.75),
							P(94.75, 27.75),
							P(91.5, 27.75),
							P(97.5, 29.5),
							P(113.5, 19.5),
							P(116.5, 19.5),
							P(113.5, 24.5),
							P(116.5, 24.5),
							P(129.5, 10.5),
							P(134.5, 10.5),
							P(126.5, 11.5),
							P(141.5, 22.5),
							P(141.5, 17.5) };

		assimilator_locations = { P(137.5, 24.5),
									P(127.5, 14.5),
									P(133.5, 55.5),
									P(137.5, 52.5),
									P(95.5, 18.5),
									P(105.5, 28.5),
									P(61.5, 51.5),
									P(71.5, 41.5),
									P(62.5, 11.5),
									P(72.5, 21.5),
									P(21.5, 31.5),
									P(31.5, 21.5),
									P(128.5, 79.5),
									P(138.5, 89.5) };

		cyber_core_locations = { P(115.5, 34.5),
									P(120.5, 32.5),
									P(120.5, 45.5) };

		tech_locations = { P(131.5, 25.5),
							P(128.5, 27.5),
							P(125.5, 27.5),
							P(126.5, 22.5),
							P(123.5, 22.5),
							P(131.5, 28.5) };

		proxy_pylon_locations = { P(68, 113),
									P(54, 76) };

		proxy_gateway_locations = { P(70.75, 112.75),
									P(51.5, 74.5) };

		proxy_tech_locations = { P(70.5, 115.5),
									P(51.5, 77.5) };

		defensive_natural_battery_locations = { P(123, 52),
												P(125, 52) };


		attack_path_short = { P(123, 50),
								P(118.5, 50.1),
								P(107, 53.5),
								P(103.5, 56.5),
								P(102, 66),
								P(100, 70.5),
								P(96, 75.5),
								P(90.5, 75),
								P(82.5, 77.5),
								P(77.5, 82.5),
								P(69.5, 85),
								P(64, 84.5),
								P(60, 89.5),
								P(58, 94),
								P(56.5, 103.5),
								P(53, 106.5),
								P(41.5, 109.9),
								P(37, 110),
								P(31.5, 119),
								P(35, 123),
								P(32.5, 136),
								P(26, 142.5) };

		attack_path_short_line = PathManager(attack_path_short);

		attack_path = { P(110, 108),
						P(106, 105),
						P(94, 101),
						P(90, 104),
						P(73, 122),
						P(70, 126),
						P(58, 124),
						P(52, 117),
						P(43, 110),
						P(36, 111),
						P(35, 120),
						P(34, 122),
						P(32, 136),
						P(27, 141) };

		attack_path_line = PathManager(attack_path);


		high_ground_index = 5;

		attack_path_alt = { P(30, 31),
							P(34, 75),
							P(31, 111),
							P(29, 138), };

		high_ground_index_alt = 2;

		initial_scout_pos = P(34, 122);

		main_scout_path = { P(19, 135.5),
							P(28, 148.5),
							P(41.5, 140),
							P(39, 128) };

		natural_scout_path = { P(22.5, 118.5),
								P(20.5, 107.5),
								P(32, 104), };

		possible_3rds = { P(61.5, 134.5),
						P(28.5, 73.5) };

		enemy_natural = P(29.5, 111.5);

		Point2D entrance_point = P(86, 148);
		Point2D exit_point = P(21, 86);
		std::vector<Point2D> entrance_points = { P(64, 143),
												P(35, 147),
												P(25, 120) };
		std::vector<Point2D> exit_points = { P(51, 130),
											P(20, 135),
											P(25, 104) };
		std::vector<Point2D> base_points = { P(61.5, 134.5),
											P(29.5, 138.5),
											P(29.5, 111.5) }; 
		std::vector<std::vector<Point2D>> exfi_paths = { {P(48, 150),
															P(80, 150)},
															{ P(17, 124),
															P(17, 96)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);

		
		third_base_pylon_gap = P(95.5, 25);
		natural_door_closed = P(120, 50.5);
		natural_door_open = P(116, 50.5);
		


		adept_harrass_protoss_consolidation = { P(57, 121), P(60, 94) };

		main_early_dead_space = P(114, 19);
		natural_front = { P(120, 53),
							P(120, 46) };


		adept_scout_shade = P(46, 112);
		adept_scout_runaway = P(60, 122);
		adept_scout_ramptop = P(34, 122);
		adept_scout_nat_path = { P(21, 107),
									P(23, 117) };
		adept_scout_base_spots = { P(19, 136),
									P(25, 146),
									P(40, 146),
									P(45, 131) };


		blink_presure_consolidation = P(61, 126);
		blink_pressure_prism_consolidation = P(65, 125);
		blink_pressure_blink_up = P(53, 128);
		blink_pressure_blink_down = P(47, 130);


		std::vector<Point2D> blink_nat_attack_path = { P(61, 126),
														P(47, 113),
														P(37, 111),
														P(25, 113) };

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(47, 130),
																	P(23, 130) },
																	{ P(47, 130),
																	P(41, 138),
																	P(30, 143),
																	P(20, 143 ) } };

		blink_main_attack_path_lines = { PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1]) };


		bad_warpin_spots = { first_pylon_location_terran, P(113.5, 36), P(118, 40) };

	}
	
}

