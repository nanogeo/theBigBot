
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

	void Locations::SetLeyLinesLocations(Point3D start)
	{
		Point2D center_point = Point2D(0, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(198, 174) - point;
			else
				return point;
		};

		bool swap = start.x == 155.5 && start.y == 133.5;

		this->start_location = start;

		base_locations = { P(40.5, 69.5),
							P(67.5, 55.5),
							P(72.5, 85.5),
							P(43.5, 96.5),
							P(44.5, 123.5),
							P(93.5, 41.5),
							P(93.5, 66.5),
							P(120.5, 36.5),
							convert_location(Point2D(40.5, 69.5), !swap),
							convert_location(Point2D(67.5, 55.5), !swap),
							convert_location(Point2D(72.5, 85.5), !swap),
							convert_location(Point2D(43.5, 96.5), !swap),
							convert_location(Point2D(44.5, 123.5), !swap),
							convert_location(Point2D(93.5, 41.5), !swap),
							convert_location(Point2D(93.5, 66.5), !swap),
							convert_location(Point2D(120.5, 36.5), !swap) };

		nexi_locations = { P(42.5, 40.5),
							P(40.5, 69.5),
							P(67.5, 55.5),
							P(72.5, 85.5),
							P(43.5, 96.5),
							P(44.5, 123.5),
							P(93.5, 41.5),
							P(93.5, 66.5),
							P(120.5, 36.5) };

		pylon_locations = { P(44, 44),
							P(46, 44),
							P(60, 36),
							P(58, 36),
							P(35, 32),
							P(30, 49),
							P(72, 54),
							P(69, 48),
							P(32, 67),
							P(56, 36),
							P(58, 44),
							P(69, 87),
							P(32, 49),
							P(76, 83),
							P(43, 100),
							P(35, 93),
							P(40, 130),
							P(48, 123),
							P(95, 34),
							P(97, 44),
							P(124, 39),
							P(117, 29),
							P(97, 67),
							P(90, 69) };
		
		first_pylon_location_zerg = P(50, 70);
		first_pylon_location_protoss = P(37, 53);
		first_pylon_location_terran = P(56, 49);

		gateway_locations = { P(52.5, 66.5), // z
							P(49.5, 72.5),
							P(38.5, 55.5), // p
							P(39.5, 52.5),
							P(57.5, 46.5), // t
							P(54.5, 46.5),
							P(59.5, 38.5),
							P(56.5, 38.5),
							P(59.5, 33.5),
							P(56.5, 33.5),
							P(38.5, 29.5),
							P(32.5, 35.5),
							P(30.5, 51.5),
							P(32.5, 45.5),
							P(74.5, 49.5),
							P(74.5, 52.5),
							P(71.5, 56.5),
							P(74.5, 55.5) };

		assimilator_locations = { P(46.5, 33.5),
									P(49.5, 37.5),
									P(38.5, 62.5),
									P(38.5, 76.5),
									P(60.5, 58.5),
									P(71.5, 48.5),
									P(70.5, 92.5),
									P(47.5, 89.5),
									P(36.5, 99.5),
									P(37.5, 120.5),
									P(48.5, 130.5),
									P(97.5, 34.5),
									P(100.5, 38.5),
									P(90.5, 59.5),
									P(113.5, 35.5),
									P(127.5, 34.5) };

		cyber_core_locations = { P(52.5, 70.5),
									P(35.5, 55.5),
									P(53.5, 50.5) };

		tech_locations = { P(41.5, 44.5),
							P(38.5, 44.5),
							P(44.5, 46.5),
							P(46.5, 41.5),
							P(49.5, 41.5),
							P(48.5, 44.5) };
		
		proxy_pylon_locations = { P(108, 119),
								P(121, 81) };

		proxy_gateway_locations = { P(105.5, 120.5),
									P(116.5, 83.5) };

		proxy_tech_locations = { P(108.5, 121.5),
								P(121.5, 85.5) };

		defensive_natural_battery_locations = { P(50, 67),
												P(48, 70),
												P(50, 65) };

		gateway_walloff = P(42.5, 51.5);
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

		initial_scout_pos = P(39.5, 118.5);
		*/
		main_scout_path = { P(147, 128),
							P(145, 144),
							P(163, 142),
							P(166, 119) };

		natural_scout_path = { P(149, 104),
								P(164, 96),
								P(165, 110), };

		possible_3rds = { P(67.5, 55.5),
						P(72.5, 85.5) };

		enemy_natural = P(40.5, 69.5);

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
		main_ramp_forcefield_top = P(42, 55);
		main_ramp_forcefield_mid = P(44, 57);
	}
	
}

#pragma warning(pop)
