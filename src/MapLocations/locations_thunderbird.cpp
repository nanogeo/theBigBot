
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

	void Locations::SetThunderbirdLocations(Point3D start)
	{
		Point2D center_point = Point2D(224, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(192, 156) - point;
			else
				return point;
		};

		bool swap = start.x == 38.5 && start.y == 133.5;

		this->start_location = start;

		nexi_locations = { P(153.5, 22.5),
							P(151.5, 52.5),
							P(124.5, 29.5),
							P(134.5, 68.5),
							P(154.5, 93.5),
							P(93.5, 25.5),
							P(60.5, 19.5),
							P(80.5, 55.5) };

		pylon_locations = { P(152, 26),
							P(120, 28),
							P(134, 20),
							P(158, 59),
							P(144, 12),
							P(129, 70),
							P(159, 12),
							P(127, 22),
							P(147, 56),
							P(156, 46),
							P(165, 25),
							P(150, 26),
							P(136, 20),
							P(163, 25),
							P(124, 33),
							P(144, 14),
							P(159, 14) };

		std::cerr << "First pylon: " + std::to_string(pylon_locations[0].x) + ", " + std::to_string(pylon_locations[0].y) + "\n";

		first_pylon_location_zerg = P(143, 50);
		first_pylon_location_protoss = P(156, 35);
		first_pylon_location_terran = P(138, 33);

		gateway_locations = { P(155.5, 37.5),
							P(158.5, 36.5),
							P(136.25, 30.25),
							P(140.75, 34.25),
							P(136.5, 27.5),
							P(139.5, 27.5),
							P(139.5, 30.5),
							P(140.5, 46.5),
							P(140.5, 49.5),
							P(117.5, 22.5),
							P(119.5, 25.5),
							P(120.5, 30.5),
							P(121.5, 33.5),
							P(134.5, 17.5),
							P(137.5, 17.5),
							P(134.5, 22.5),
							P(137.5, 22.5),
							P(141.5, 12.5),
							P(141.5, 15.5),
							P(147.5, 10.5),
							P(144.5, 14.5),
							P(156.5, 12.5),
							P(161.5, 15.5),
							P(163.5, 21.5),
							P(163.5, 29.5) };

		assimilator_locations = { P(149.5, 15.5),
									P(160.5, 25.5),
									P(158.5, 56.5),
									P(154.5, 59.5),
									P(131.5, 32.5),
									P(120.5, 22.5),
									P(132.5, 61.5),
									P(141.5, 72.5),
									P(161.5, 97.5),
									P(157.5, 100.5),
									P(97.5, 18.5),
									P(100.5, 22.5),
									P(56.5, 12.5),
									P(53.5, 16.5),
									P(76.5, 48.5),
									P(73.5, 52.5 ) };

		cyber_core_locations = { P(153.5, 34.5),
									P(140.25, 34.25),
									P(140.5, 53.5) };

		tech_locations = { P(154.5, 26.5),
							P(151.5, 28.5),
							P(148.5, 28.5),
							P(149.5, 23.5),
							P(146.5, 23.5),
							P(154.5, 29.5),
							P(145.5, 28.5) };

		proxy_pylon_locations = { P(89, 120),
								P(70, 80) };

		proxy_gateway_locations = { P(87.5, 122.5),
									P(67.5, 78.5) };

		proxy_tech_locations = { P(87.5, 125.5),
								P(70.5, 82.5) };

		defensive_natural_battery_locations = { P(143, 53),
												P(143, 55),
												P(145, 55),
												P(147, 50) };

		attack_path_short = { P(142, 49),
								P(137.5, 51),
								P(133, 54),
								P(128, 59),
								P(118, 61.5),
								P(112, 60.5),
								P(98, 59),
								P(94, 61),
								P(89, 72),
								P(103, 84),
								P(98, 95),
								P(94, 97),
								P(80, 95.5),
								P(74, 94.5),
								P(64, 97),
								P(59, 102),
								P(54.5, 105),
								P(50, 107),
								P(43, 115),
								P(38.5, 119.5),
								P(39, 131),
								P(42, 140) };

		attack_path_short_line = PathManager(attack_path_short);

		attack_path = { P(132.4, 53.8),
						P(128.1, 58),
						P(140, 85),
						P(143, 89),
						P(144.7, 100.7),
						P(142.4, 104.5),
						P(130, 103),
						P(126, 100),
						P(98.5, 96.5),
						P(93.5, 96),
						P(64.5, 99),
						P(59, 103),
						P(53, 106.5),
						P(49.5, 106),
						P(43, 115),
						P(38.5, 119),
						P(36, 128),
						P(44, 140) };

		attack_path_line = PathManager(attack_path);

		

		high_ground_index = 5;

		attack_path_alt = { P(81, 55),
							P(38, 63),
							P(57, 88),
							P(41, 103),
							P(41, 132) };

		high_ground_index_alt = 2;

		initial_scout_pos = P(39.5, 118.5);

		main_scout_path = { P(28, 132),
							P(37, 144),
							P(51, 136),
							P(47, 123) };

		natural_scout_path = { P(31, 109),
								P(33, 97),
								P(47, 98), };

		possible_3rds = { P(67.5, 126.5),
						P(57.5, 87.5) };


		enemy_natural = P(40.5, 103.5);

		
		Point2D entrance_point = P(92, 142);
		Point2D exit_point = P(33, 79);
		std::vector<Point2D> entrance_points = { P(71, 136),
												P(43, 143),
												P(36, 112) };
		std::vector<Point2D> exit_points = { P(57, 124),
											P(28, 131),
											P(34, 96) };
		std::vector<Point2D> base_points = { P(67.5, 126.5),
											P(38.5, 133.5),
											P(40.5, 103.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(58, 145),
															P(85, 145)},
															{ P(28, 114),
															P(28, 88)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);

		
		third_base_pylon_gap = P(121.5, 28.5);
		natural_door_closed = P(140, 51.5);
		natural_door_open = P(137, 51.5);
		

		adept_harrass_protoss_consolidation = { P(68, 95), P(64, 113) };
		
		main_early_dead_space = P(135, 17);
		natural_front = { P(139, 54),
							P(140, 46) };


		adept_scout_shade = P(55, 105);
		adept_scout_runaway = P(73, 93);
		adept_scout_ramptop = P(39, 119);
		adept_scout_nat_path = { P(36, 93),
									P(31, 108) };
		adept_scout_base_spots = { P(28, 131),
									P(36, 142),
									P(50, 139),
									P(52, 127) };


		blink_presure_consolidation = P(76, 95);
		blink_pressure_prism_consolidation = P(79, 93);
		blink_pressure_blink_up = P(56, 120);
		blink_pressure_blink_down = P(53, 123);


		std::vector<Point2D> blink_nat_attack_path = { P(98, 96),
														P(83, 96),
														P(64, 99),
														P(58, 103),
														P(46, 105),
														P(35, 104) };

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(53, 123),
																	P(32, 126) },
																	{ P(53, 123),
																	P(50, 132),
																	P(41, 138),
																	P(34, 139) } };

		blink_main_attack_path_lines = { PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1]) };
		

		bad_warpin_spots = { first_pylon_location_terran };

	}
	
}

#pragma warning(pop)
