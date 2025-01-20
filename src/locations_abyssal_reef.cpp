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

		bool swap = start_location.x == 38.5 && start_location.y == 122.5;

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

		attack_path_line = PathManager(attack_path);

		

		high_ground_index = 5;

		attack_path_alt = { P(101, 112),
							P(74, 96),
							P(70, 116),
							P(41, 122) };

		high_ground_index_alt = 2;

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

		
		Point2D entrance_point = P(107, 129);
		Point2D exit_point = P(32, 67);
		std::vector<Point2D> entrance_points = { P(78, 117),
												P(43, 132),
												P(44, 103) };
		std::vector<Point2D> exit_points = { P(66, 126),
											P(29, 120),
											P(33, 89) };
		std::vector<Point2D> base_points = { P(70.5, 117.5),
											P(38.5, 122.5),
											P(42.5, 93.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(61, 136),
															P(86, 136)},
															{ P(28, 106),
															P(28, 77)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);


		third_base_pylon_gap = P(158.5, 53.5);
		natural_door_closed = P(137.5, 38.5);
		natural_door_open = P(137.5, 42);
		

		adept_harrass_protoss_consolidation = { P(52, 98), P(80, 103) };
		
		main_early_dead_space = P(163, 35);

		natural_front = { P(131, 37),
							P(139, 38) };


		adept_scout_shade = P(59, 103);
		adept_scout_runaway = P(55,91 );
		adept_scout_ramptop = P(56, 118);
		adept_scout_nat_path = { P(66, 124),
									P(78, 120) };

		adept_scout_base_spots = { P(52, 130),
									P(35, 129),
									P(30, 117),
									P(40, 111) };


		blink_presure_consolidation = P(52, 94);
		blink_pressure_prism_consolidation = P(50, 92);
		blink_pressure_blink_up = P(41, 103);
		blink_pressure_blink_down = P(41, 106);


		std::vector<Point2D> blink_nat_attack_path = {P(52, 94),
														P(62, 105),
														P(69, 115),
														P(71, 122) };

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(41, 106),
																	P(53, 128) },
																	{ P(41, 106),
																	P(35, 113),
																	P(36, 123),
																	P(39, 129) } };

		blink_main_attack_path_lines = { PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1])};


		bad_warpin_spots = { first_pylon_location_terran };

	}

}

