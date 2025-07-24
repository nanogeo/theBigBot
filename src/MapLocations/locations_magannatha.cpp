
#include "locations.h"
#include "theBigBot.h"
#include "piecewise_path.h"
#include "definitions.h"

#include <iostream>
#include <string>

#define P(x, y) convert_location(Point2D(x, y), swap)

#pragma warning(push)
#pragma warning(disable : 4305)

namespace sc2 {

	void Locations::SetMagannathaLocations(Point3D start)
	{
		Point2D center_point = Point2D(0, 0);

		// mirror line: y=mx+n
		float m = 1;
		float n = 0;

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

		bool swap = start.x == 38.5 && start.y == 141.5;

		this->start_location = start;

		base_locations = { P(141.5, 38.5),
							P(117.5, 28.5),
							P(86.5, 27.5),
							P(120.5, 59.5),
							P(142.5, 85.5),
							P(83.5, 62.5),
							P(27.5, 27.5),
							P(138.5, 138.5),
							P(46.5, 46.5),
							P(102.5, 102.5),
							convert_location(Point2D(38.5, 141.5), !swap),
							convert_location(Point2D(117.5, 28.5), !swap),
							convert_location(Point2D(86.5, 27.5), !swap),
							convert_location(Point2D(120.5, 59.5), !swap),
							convert_location(Point2D(142.5, 85.5), !swap),
							convert_location(Point2D(83.5, 62.5), !swap), };

		nexi_locations = { P(141.5, 38.5),
							P(117.5, 28.5),
							P(86.5, 27.5),
							P(120.5, 59.5),
							P(142.5, 85.5),
							P(83.5, 62.5),
							P(27.5, 27.5),
							P(138.5, 138.5),
							P(46.5, 46.5),
							P(102.5, 102.5) };

		pylon_locations = { P(138, 40),
							P(138, 42),
							P(142, 23),
							P(142, 25),
							P(153, 34),
							P(151, 34),
							P(140, 55),
							P(140, 53),
							P(82, 27),
							P(112, 22),
							P(124, 23),
							P(88, 31),
							P(117, 58),
							P(89, 19),
							P(123, 63),
							P(141, 89),
							P(139, 84),
							P(149, 79),
							P(82, 59),
							P(87, 64),
							P(31, 25),
							P(26, 31),
							P(45, 50),
							P(50, 46),
							P(99, 104),
							P(103, 99) };
		
		first_pylon_location_zerg = P(111, 33);
		first_pylon_location_protoss = P(129, 41);
		first_pylon_location_terran = P(130, 49);

		gateway_locations = { P(108.5, 32.5), // z
							P(109.5, 36.5),
							P(126.5, 39.5), // p
							P(129.5, 38.5),
							P(128.5, 46.5), // t
							P(131.5, 46.5),
							P(139.5, 24.5),
							P(139.5, 27.5),
							P(144.5, 24.5),
							P(144.5, 27.5),
							P(151.5, 31.5),
							P(151.5, 36.5),
							P(147.5, 27.5),
							P(142.5, 53.5),
							P(142.5, 50.5),
							P(137.5, 53.5),
							P(137.5, 50.5),
							P(76.5, 28.5),
							P(79.5, 29.5),
							P(82.5, 29.5),
							P(85.5, 31.5),
							P(138.5, 92.5),
							P(138.5, 89.5),
							P(138.5, 86.5) };

		assimilator_locations = { P(144.5, 45.5),
									P(148.5, 41.5),
									P(124.5, 25.5),
									P(121.5, 21.5),
									P(93.5, 30.5),
									P(83.5, 20.5),
									P(117.5, 52.5),
									P(127.5, 62.5),
									P(149.5, 88.5),
									P(145.5, 92.5),
									P(84.5, 69.5),
									P(76.5, 61.5),
									P(20.5, 30.5),
									P(30.5, 20.5),
									P(135.5, 145.5),
									P(145.5, 135.5) };

		cyber_core_locations = { P(112.5, 37.5),
									P(126.5, 42.5),
									P(132.5, 50.5) };

		tech_locations = { P(137.5, 37.5),
							P(137.5, 34.5),
							P(135.5, 40.5),
							P(140.5, 42.5),
							P(137.5, 44.5) };

		proxy_pylon_locations = { P(64, 79),
								P(78, 118) };

		proxy_gateway_locations = { P(61.5, 79.5),
									P(79.5, 115.5) };

		proxy_tech_locations = { P(64.5, 81.5),
								P(80.5, 118.5) };
		
		defensive_natural_battery_locations = { P(113, 33),
												P(111, 31),
												P(107, 30) };
		
		gateway_walloff = P(129.5, 36.5);
		/*
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

		attack_path_short_line = PiecewisePath(attack_path_short);

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

		attack_path_line = PiecewisePath(attack_path);

		

		high_ground_index = 5;

		attack_path_alt = { P(81, 55),
							P(38, 63),
							P(57, 88),
							P(41, 103),
							P(41, 132) };

		high_ground_index_alt = 2;

		initial_scout_pos = P(39.5, 118.5);
		*/
		main_scout_path = { P(51, 139),
							P(42, 153),
							P(26, 148),
							P(30, 135) };

		natural_scout_path = { P(32, 110),
								P(19, 115),
								P(23, 126), };

		possible_3rds = { P(27.5, 86.5),
						P(59.5, 120.5) };


		enemy_natural = P(28.5, 117.5);

		for (const auto& pos : main_scout_path)
		{
			self_main_scout_path.push_back(convert_location(pos, true));
		}

		for (const auto& pos : natural_scout_path)
		{
			self_natural_scout_path.push_back(convert_location(pos, true));
		}
		/*
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

		blink_nat_attack_path_line = PiecewisePath(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(53, 123),
																	P(32, 126) },
																	{ P(53, 123),
																	P(50, 132),
																	P(41, 138),
																	P(34, 139) } };

		blink_main_attack_path_lines = { PiecewisePath(blink_main_attack_path[0]), PiecewisePath(blink_main_attack_path[1]) };

		std::vector<std::vector<Point2D>> blink_third_attack_path = { { P(76, 95),
																	P(59, 103),
																	P(60, 109),
																	P(65, 117),
																	P(67, 124),
																	P(68, 131) },
																	{ P(76, 95),
																	P(53, 85) } };

		blink_third_attack_path_lines = { PiecewisePath(blink_third_attack_path[0]), PiecewisePath(blink_third_attack_path[1]) };


		bad_warpin_spots = { first_pylon_location_terran };

		worker_rush_defense_group = P(161, 21.5);
		worker_rush_defense_attack = P(31, 134.5);

		warp_prism_locations = { P(90, 100),
								P(79, 135),
								P(55, 140),
								P(30, 142),
								P(31, 109),
								P(55, 78) };
								*/
		main_ramp_forcefield_top = P(127, 37);
		main_ramp_forcefield_mid = P(125, 35);
	}
	
}

#pragma warning(pop)
