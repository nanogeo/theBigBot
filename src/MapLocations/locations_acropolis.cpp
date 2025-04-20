
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

	void Locations::SetAcropolisLocations(Point3D start)
	{
		Point2D center_point = Point2D(88, 86);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(176, 172) - point;
			else
				return point;
		};

		bool swap = start.x == 142.5 && start.y == 33.5;

		this->start_location = start;


		nexi_locations = { P(33.5, 138.5),
							P(31.5, 113.5),
							P(32.5, 85.5),
							P(29.5, 53.5),
							P(58.5, 111.5),
							P(73.5, 138.5),
							P(47.5, 28.5),
							P(68.5, 42.5) };

		pylon_locations = { P(35, 135),
							P(34, 81),
							P(34, 89),
							P(62, 112),
							P(30, 125),
							P(29, 145),
							P(47, 146),
							P(23, 113),
							P(27, 120),
							P(27, 79),
							P(27, 92),
							P(37, 135) };

		std::cerr << "First pylon: " + std::to_string(pylon_locations[0].x) + ", " + std::to_string(pylon_locations[0].y) + "\n";

		first_pylon_location_zerg = P(34, 108);
		first_pylon_location_protoss = P(45, 125);
		first_pylon_location_terran = P(49, 134);

		gateway_locations = { P(43.5, 122.5),
							 P(45.5, 127.5),
							 P(51.5, 131.5),
							 P(48.5, 131.5),
							 P(46.5, 134.5),
							 P(49.5, 138.5),
							 P(38.5, 106.5),
							 P(34.5, 105.5),
							 P(34.75, 77.25),
							 P(36.75, 80.25),
							 P(36.5, 83.5),
							 P(36.5, 86.5),
							 P(63.5, 115.5),
							 P(64.5, 112.5),
							 P(62.5, 109.5),
							 P(59.5, 107.5),
							 P(27.5, 126.5),
							 P(27.5, 129.5),
							 P(30.5, 127.5),
							 P(30.5, 130.5),
							 P(26.5, 145.5),
							 P(29.5, 147.5),
							 P(49.5, 142.5),
							 P(46.5, 142.5),
							 P(43.5, 142.5),
							 P(43.5, 146.5) };

		assimilator_locations = { P(26.5, 135.5),
									P(37.5, 145.5),
									P(24.5, 110.5),
									P(28.5, 106.5),
									P(25.5, 81.5),
									P(30.5, 78.5),
									P(26.5, 60.5),
									P(22.5, 56.5),
									P(62.5, 118.5),
									P(51.5, 108.5),
									P(66.5, 135.5),
									P(77.5, 145.5),
									P(40.5, 31.5),
									P(51.5, 21.5),
									P(61.5, 46.5),
									P(70.5, 35.5) };


		cyber_core_locations = { P(31.5, 105.5),
									P(42.5, 125.5),
									P(51.5, 134.5) };

		tech_locations = {	P(32.5, 134.5),
							P(29.5, 134.5),
							P(35.5, 132.5),
							P(38.5, 132.5),
							P(32.5, 131.5),
							P(37.5, 137.5),
							P(40.5, 137.5),
							P(41.5, 132.5) };

		proxy_pylon_locations = {P(124, 109),
								P(93, 57) };

		proxy_gateway_locations = { P(124.5, 106.5),
									P(92.5, 54.5) };

		proxy_tech_locations = { P(127.5, 111.5),
								P(88.5, 56.5) };

		defensive_natural_battery_locations = { P(38, 109),
												P(32, 108),
												P(36, 112),
												P(31, 110) };

		attack_path_short = { P(36, 104.5),
								P(37, 100.5),
								P(44, 97.5),
								P(47, 100.5),
								P(69.5, 98.5),
								P(74, 94),
								P(84, 87),//
								P(92, 85),//
								P(102, 78),
								P(106.5, 73.5),
								P(129, 71.5),
								P(132, 74.5),
								P(138.5, 72.5),
								P(140, 67.5),
								P(139, 53),
								P(135.5, 49.5),
								P(138, 37),
								P(147, 30) };

		attack_path_short_line = PathManager(attack_path_short);

		attack_path = { P(69.6, 98.8),
						P(75, 95),
						P(83, 95.6),
						P(85.3, 100.4),
						P(88.3, 106),
						P(91.6, 108.3),
						P(105, 105),
						P(112, 99),
						P(121.3, 97.7),
						P(126, 101.5),
						P(135, 97.5),
						P(137.5, 93.25),
						P(139.25, 72.75),
						P(140.25, 67.5),
						P(138, 52.25),
						P(135.5, 49.75),
						P(140, 36),
						P(147, 28) };

		attack_path_line = PathManager(attack_path);

		
		high_ground_index = 5;

		attack_path_alt = { P(96, 38),
							P(113, 59),
							P(142, 59),
							P(139, 34) };

		high_ground_index_alt = 2;
		
		initial_scout_pos = P(136, 50);

		main_scout_path = { P(152, 39),
							P(152, 29),
							P(140, 23),
							P(132, 32),
							P(133, 44) };

		natural_scout_path = { P(150, 52),
								P(154, 63),
								P(142, 68), };

		possible_3rds = { P(143.5, 86.5),
						P(117.5, 60.5), };

		enemy_natural = P(144.5, 59.5);

		Point2D entrance_point = P(114, 22);
		Point2D exit_point = P(152, 110);
		std::vector<Point2D> entrance_points = { P(138, 25),
												P(147, 48),
												P(148, 78) };
		std::vector<Point2D> exit_points = { P(152, 36),
											P(149, 66),
											P(146, 94) };
		std::vector<Point2D> base_points = { P(142.5, 33.5),
											P(144.5, 59),
											P(143.5, 86.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(155, 48),
															P(155, 22),
															P(126, 22)},
															{ P(155, 74),
															P(155, 104)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);

		third_base_pylon_gap = P(34, 82.5);
		natural_door_closed = P(36.5, 106);
		natural_door_open = P(36.5, 102);

		
		adept_harrass_protoss_consolidation = { P(138, 82), P(119, 69) };
		
		main_early_dead_space = P(49, 142);
		natural_front = { P(40, 103),
							P(33, 101) };


		adept_scout_shade = P(138, 73);
		adept_scout_runaway = P(137, 85);
		adept_scout_ramptop = P(136, 50);
		adept_scout_nat_path = { P(154, 59),
									P(141, 53) };
		adept_scout_base_spots = { P(151, 40),
									P(150, 28),
									P(137, 25),
									P(126, 37) };


		blink_presure_consolidation = P(135, 96);
		blink_pressure_prism_consolidation = P(132, 100);
		blink_pressure_blink_up = P(120.5, 56);
		blink_pressure_blink_down = P(127.5, 50);


		std::vector<Point2D> blink_nat_attack_path = { P(130.3, 126.5),
														P(134.8, 122.4),
														P(138.5, 110.5),
														P(134.7, 106.8),
														P(133, 100.4),
														P(136.7, 93.6),
														P(139, 73),
														P(140, 68),
														P(138, 52.25),
														P(135.5, 49.75),
														P(140, 36),
														P(147, 28) };

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(127.5, 50),
																	P(149, 38) },
																	{ P(127.5, 50),
																	P(129, 37),
																	P(139, 30),
																	P(148, 30) } };

		blink_main_attack_path_lines = { PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1]) };


		bad_warpin_spots = { first_pylon_location_terran, P(56.5, 132) };

	}
	
}

#pragma warning(pop)
