#pragma once
#include "locations.h"
#include "theBigBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)


namespace sc2 {

	void Locations::SetInterloperLocations(Point3D start_location, BuildOrder build_order)
	{
		Point2D center_point = Point2D(224, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(152, 168) - point;
			else
				return point;
		};

		bool swap = start_location.x == 26.5 && start_location.y == 137.5;

		this->start_location = start_location;

		nexi_locations = { P(125.5, 30.5),
							P(126.5, 56.5),
							P(93.5, 39.5),
							P(125.5, 86.5),
							P(91.5, 71.5),
							P(59.5, 28.5),
							P(65.5, 53.5),
							P(26.5, 30.5) };

		pylon_locations = { P(89, 38),
							P(124, 34),
							P(96, 32),
							P(104, 28),
							P(130, 49),
							P(135, 57),
							P(127, 19),
							P(137, 30),
							P(112, 18),
							P(127, 21),
							P(137, 28),
							P(122, 34),
							P(112, 20),
							P(104, 30),
							P(128, 79),
							P(124, 61),
							P(121, 88),
							P(126, 44) };

		std::cerr << "First pylon: " + std::to_string(pylon_locations[0].x) + ", " + std::to_string(pylon_locations[0].y) + "\n";

		first_pylon_location_zerg = P(116, 57);
		first_pylon_location_protoss = P(113, 43);
		first_pylon_location_terran = P(112, 44);

		gateway_locations = { P(114.5, 45.5),
								P(115.5, 42.5),
								P(110.5, 40.5),
								P(113.5, 61.5),
								P(113.5, 57.5),
								P(113.5, 39.5),
								P(86.5, 38.5),
								P(88.25, 35.25),
								P(89.25, 40.75),
								P(91.5, 43.5),
								P(124.5, 19.5),
								P(132.5, 20.5),
								P(127.5, 21.5),
								P(136.5, 27.5),
								P(135.5, 34.5),
								P(114.5, 18.5),
								P(114.5, 21.5),
								P(109.5, 18.5),
								P(109.5, 21.5),
								P(104.5, 25.5),
								P(107.5, 25.5),
								P(104.5, 30.5),
								P(107.5, 30.5) };

		assimilator_locations = { P(122.5, 23.5),
									P(132.5, 33.5),
									P(124.5, 49.5),
									P(133.5, 59.5),
									P(100.5, 42.5),
									P(90.5, 32.5),
									P(122.5, 79.5),
									P(132.5, 89.5),
									P(98.5, 73.5),
									P(87.5, 64.5),
									P(66.5, 26.5),
									P(52.5, 27.5),
									P(69.5, 46.5),
									P(58.5, 55.5),
									P(29.5, 23.5),
									P(19.5, 33.5) };

		cyber_core_locations = {P(109.75, 43.25),
									P(113.5, 54.5),
									P(109.75, 43.25) };

		tech_locations = { P(126.5, 34.5),
							P(123.5, 36.5),
							P(126.5, 37.5),
							P(120.5, 36.5),
							P(121.5, 31.5),
							P(118.5, 31.5),
							P(117.5, 36.5) };

		proxy_pylon_locations = { P(70, 109),
									P(45, 72) };

		proxy_gateway_locations = { P(72.5, 108.5),
									P(46.5, 69.5) };

		proxy_tech_locations = { P(70.5, 111.5),
									P(47.5, 72.5) };


		attack_path = {P(106, 121),
						P(102, 126),
						P(82, 136),
						P(72, 134.5),
						P(55, 119),
						P(49, 113),
						P(40.5, 110),
						P(35.5, 110.5),
						P(32, 119.5),
						P(34.5, 122.5),
						P(34, 132),
						P(24, 143) };

		attack_path_line = PathManager(attack_path);
		
		

		high_ground_index = 5;

		attack_path_alt = { P(70, 91),
							P(29, 81),
							P(28, 112),
							P(32, 140) };

		high_ground_index_alt = 2;

		initial_scout_pos = P(34.5, 122.5);

		main_scout_path = { P(17, 133),
							P(22, 146.5),
							P(40, 143),
							P(34.5, 132) };

		natural_scout_path = { P(18.5, 118.5),
								P(16.5, 108),
								P(30, 105.5), };

		possible_3rds = { P(58.5, 128.5),
						P(26.5, 81.5) };

		enemy_natural = P(25.5, 111.5);

		
		Point2D entrance_point = P(83, 150);
		Point2D exit_point = P(16, 90);
		std::vector<Point2D> entrance_points = { P(62, 138),
												P(30, 147),
												P(28, 121) };
		std::vector<Point2D> exit_points = { P(48, 125),
											P(17, 134),
											P(63, 110) };
		std::vector<Point2D> base_points = { P(58.5, 128.5),
											P(26.5, 137.5),
											P(25.5, 111.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(47, 151),
															P(80, 151)},
															{ P(12, 123),
															P(12, 97)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);

		
		third_base_pylon_gap = P(90.5, 38);
		natural_door_closed = P(113, 59.5);
		natural_door_open = P(110, 59.5);
		
		

		adept_harrass_protoss_consolidation = { P(56, 116), P(48, 92) };

		main_early_dead_space = P(107, 22);
		natural_front = { P(112, 63),
							P(112, 54) };


		adept_scout_shade = P(42, 105);
		adept_scout_runaway = P(48, 92);
		adept_scout_ramptop = P(34, 122);
		adept_scout_nat_path = { P(16, 110),
									P(23, 120) };
		adept_scout_base_spots = { P(17, 134),
									P(22, 145),
									P(35, 146),
									P(45, 136) };


		blink_presure_consolidation = P(60, 120);
		blink_pressure_prism_consolidation = P(63, 119);
		blink_pressure_blink_up = P(51, 140);
		blink_pressure_blink_down = P(48, 141);


		std::vector<Point2D> blink_nat_attack_path = { P(60, 120),
														P(40, 110),
														P(23, 111),
														P(18, 116) };

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(48, 141),
																	P(23, 142) },
																	{ P(48, 141),
																	P(40, 133),
																	P(28, 130),
																	P(18, 136) } };

		blink_main_attack_path_lines = { PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1]) };


		bad_warpin_spots = { first_pylon_location_terran, P(108, 42), P(112.5, 48) };
		

	}
	
}

