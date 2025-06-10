
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

	void Locations::SetUltraloveLocations(Point3D start)
	{
		Point2D center_point = Point2D(0, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(184, 184) - point;
			else
				return point;
		};

		bool swap = start.x == 141.5 && start.y == 137.5;

		this->start_location = start;

		base_locations = { P(42.5, 46.5),
							P(68.5, 42.5),
							P(99.5, 38.5),
							P(64.5, 70.5),
							P(39.5, 82.5),
							P(38.5, 106.5),
							P(144.5, 36.5),
							convert_location(Point2D(42.5, 46.5), !swap),
							convert_location(Point2D(68.5, 42.5), !swap),
							convert_location(Point2D(99.5, 38.5), !swap),
							convert_location(Point2D(64.5, 70.5), !swap),
							convert_location(Point2D(39.5, 82.5), !swap),
							convert_location(Point2D(38.5, 106.5), !swap),
							convert_location(Point2D(144.5, 36.5), !swap) };

		nexi_locations = { P(42.5, 46.5),
							P(68.5, 42.5),
							P(99.5, 38.5),
							P(64.5, 70.5),
							P(39.5, 82.5),
							P(38.5, 106.5),
							P(144.5, 36.5) };

		pylon_locations = { P(44, 50),
							P(46, 50),
							P(104, 39),
							P(99, 42),
							P(42, 67),
							P(42, 65),
							P(31, 56),
							P(33, 56),
							P(40, 35),
							P(31, 44),
							P(64, 75),
							P(67, 35),
							P(55, 72),
							P(43, 84),
							P(31, 83),
							P(40, 110),
							P(30, 103),
							P(104, 39),
							P(99, 42),
							P(146, 40),
							P(147, 29) };
		
		first_pylon_location_zerg = P(74, 46);
		first_pylon_location_protoss = P(55, 55);
		first_pylon_location_terran = P(55, 62);

		gateway_locations = { P(77.5, 46.5), // z
							P(72.5, 51.5),
							P(57.5, 53.5), // p
							P(54.5, 52.5),
							P(56.5, 59.5), // t
							P(52.5, 60.5),
							P(107.5, 39.5),
							P(104.5, 41.5),
							P(101.5, 42.5),
							P(98.5, 44.5),
							P(39.5, 65.5),
							P(44.5, 65.5),
							P(44.5, 62.5),
							P(39.5, 62.5),
							P(32.5, 58.5),
							P(35.5, 58.5),
							P(32.5, 53.5),
							P(35.5, 53.5),
							P(37.5, 36.5),
							P(42.5, 36.5),
							P(32.5, 41.5),
							P(32.5, 46.5),
							P(59.5, 76.5),
							P(62.5, 77.5),
							P(66.5, 74.5),
							P(65.5, 77.5),
							P(56.5, 76.5),
							P(107.5, 39.5),
							P(104.5, 41.5),
							P(101.5, 42.5),
							P(98.5, 44.5) };

		assimilator_locations = { P(46.5, 39.5),
									P(35.5, 49.5),
									P(61.5, 39.5),
									P(64.5, 35.5),
									P(92.5, 37.5),
									P(106.5, 36.5),
									P(68.5, 63.5),
									P(57.5, 73.5),
									P(32.5, 85.5),
									P(35.5, 89.5),
									P(31.5, 109.5),
									P(34.5, 113.5),
									P(139.5, 30.5),
									P(151.5, 38.5) };

		cyber_core_locations = { P(73.5, 48.5),
									P(57.5, 56.5),
									P(52.5, 63.5) };

		tech_locations = { P(46.5, 47.5),
							P(48.5, 50.5),
							P(45.5, 52.5),
							P(41.5, 50.5),
							P(41.5, 53.5) };

		proxy_pylon_locations = { P(116, 89),
								P(70, 126) };

		proxy_gateway_locations = { P(118.5, 87.5),
									P(71.5, 128.5) };

		proxy_tech_locations = { P(115.5, 86.5),
								P(68.5, 129.5) };
		
		defensive_natural_battery_locations = { P(72, 46),
												P(78, 44),
												P(71, 48) };

		gateway_walloff = P(53.5, 49.5);
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
		*/
		initial_scout_pos = P(127, 133);

		main_scout_path = { P(132, 125),
							P(152, 125),
							P(151, 146),
							P(133, 147) };

		natural_scout_path = { P(106, 141),
								P(115, 151),
								P(125, 145) };

		possible_3rds = { P(119.5, 113.5),
						P(84.5, 145.5) };


		enemy_natural = P(115.5, 141.5);

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

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(53, 123),
																	P(32, 126) },
																	{ P(53, 123),
																	P(50, 132),
																	P(41, 138),
																	P(34, 139) } };

		blink_main_attack_path_lines = { PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1]) };

		std::vector<std::vector<Point2D>> blink_third_attack_path = { { P(76, 95),
																	P(59, 103),
																	P(60, 109),
																	P(65, 117),
																	P(67, 124),
																	P(68, 131) },
																	{ P(76, 95),
																	P(53, 85) } };

		blink_third_attack_path_lines = { PathManager(blink_third_attack_path[0]), PathManager(blink_third_attack_path[1]) };


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
		main_ramp_forcefield_top = P(57, 51);
		main_ramp_forcefield_mid = P(59, 49);
	}
	
}

#pragma warning(pop)
