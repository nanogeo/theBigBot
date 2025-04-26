
#include "locations.h"
#include "theBigBot.h"
#include "path_manager.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)

#pragma warning(push)
#pragma warning(disable : 4305)

namespace sc2 {

	void Locations::SetAbyssalReefLocations(Point3D start)
	{
		Point2D center_point = Point2D(224, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(200, 144) - point;
			else
				return point;
		};

		bool swap = start.x == 38.5 && start.y == 122.5;

		this->start_location = start;

		nexi_locations = { P(161.5, 21.5),
							P(129.5, 26.5),
							P(157.5, 50.5),
							P(141.5, 65.5),
							P(129.5, 49.5),
							P(100.5, 28.5),
							P(159.5, 99.5) };

		pylon_locations = { P(159, 55),
							P(160, 25),
							P(121, 24),
							P(130, 19),
							P(161, 43),
							P(166, 48),
							P(158, 25),
							P(168, 35),
							P(141, 12),
							P(164, 11),
							P(173, 26),
							P(153, 51),
							P(171, 26),
							P(164, 13),
							P(156, 25),
							P(126, 32) };

		std::cerr << "First pylon: " + std::to_string(pylon_locations[0].x) + ", " + std::to_string(pylon_locations[0].y) + "\n";

		first_pylon_location_zerg = P(134, 35);
		first_pylon_location_protoss = P(147, 21);
		first_pylon_location_terran = P(158, 37);
	
		gateway_locations = { P(144.5, 22.5),
							P(149.5, 20.5),
							P(155.5, 35.5),
							P(158.5, 34.5),
							P(155.5, 32.5),
							P(161.5, 34.5),
							P(131.5, 34.5),
							P(139.5, 37.5),
							P(156.5, 60.5),
							P(159.5, 58.5),
							P(162.5, 56.5),
							P(169.5, 31.5),
							P(166.5, 31.5),
							P(163.5, 31.5),
							P(143.5, 12.5),
							P(146.5, 12.5),
							P(142.5, 16.5),
							P(145.5, 16.5),
							P(166.5, 12.5),
							P(160.5, 11.5),
							P(171.5, 28.5),
							P(168.5, 28.5),
							P(171.5, 23.5),
							P(171.5, 20.5) };

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


		cyber_core_locations = { P(147.5, 23.5),
									P(160.5, 37.5),
									P(135.75, 37.25) };

		tech_locations = { P(162.5, 25.5),
							P(165.5, 25.5),
							P(159.5, 27.5),
							P(156.5, 27.5),
							P(157.5, 22.5),
							P(154.5, 22.5) };

		proxy_pylon_locations = {P(70, 83),
									P(106, 97) };

		proxy_gateway_locations = { P(69.5, 80.5), 
									P(103.5, 98.5) };

		proxy_tech_locations = { P(66.5, 83.5),
								P(106.5, 99.5) };

		defensive_natural_battery_locations = { P(139, 35),
												P(134, 35),
												P(131, 33),
												P(135, 31) };

		attack_path_short = { P(131.4, 37.5),
								P(127.8, 40.5),
								P(119.3, 44.8),
								P(113, 45),
								P(106, 55.6),
								P(103.5, 61.6),
								P(105.7, 66.1),
								P(109, 70.3),
								P(103.9, 79.5),
								P(100.4, 80.5),
								P(96.5, 82.4),
								P(94, 88.4),
								P(87, 99),
								P(80.7, 99.2),
								P(72.2, 103.5),
								P(68.6, 106.5),
								P(58.8, 115.1),
								P(54.8, 118.7),
								P(44.5, 121.3),
								P(34.7, 119), };


		attack_path_short_line = PathManager(attack_path_short);

		attack_path_special = { P(131.4, 37.5),
								P(127.8, 40.5),
								P(119.3, 44.8),
								P(113, 45),
								P(106, 55.6),
								P(103.5, 61.6),
								P(105.7, 66.1),
								P(109, 70.3),
								P(103.9, 79.5),
								P(100.4, 80.5),
								P(96.5, 82.4),
								P(94, 92.4),
								P(87.3, 98.1),
								P(80.3, 96.6),
								P(75.1, 90.3),
								P(79.4, 85.8),
								P(78.7, 79.9),
								P(76, 77.7),
								P(67.8, 77.9),
								P(52.8, 90.3),
								P(52.5, 98.2),
								P(61.8, 105.3), };


		attack_path_special_line = PathManager(attack_path_special);

		attack_path = { P(121, 47.3),
						P(115.6, 47.3),
						P(109.3, 47),
						P(104.9, 42.7),
						P(81, 38),
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
						P(40, 123),
						P(33, 125) };

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
		natural_door_closed = P(137.5, 38);
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


		blink_presure_consolidation = P(56, 86);
		blink_pressure_prism_consolidation = P(56, 82);
		blink_pressure_blink_up = P(41, 103);
		blink_pressure_blink_down = P(41, 106);


		std::vector<Point2D> blink_nat_attack_path = { P(65, 78),
														P(54, 88),
														P(52, 94),
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

#pragma warning(pop)
