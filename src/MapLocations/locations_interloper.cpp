
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

	void Locations::SetInterloperLocations(Point3D start)
	{
		Point2D center_point = Point2D(224, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(152, 168) - point;
			else
				return point;
		};

		bool swap = start.x == 26.5 && start.y == 137.5;
		
		this->start_location = start;

		base_locations = { P(125.5, 30.5),
							P(126.5, 56.5),
							P(93.5, 39.5),
							P(125.5, 86.5),
							P(91.5, 71.5),
							P(59.5, 28.5),
							P(65.5, 53.5),
							P(26.5, 30.5),
							convert_location(Point2D(125.5, 30.5), !swap),
							convert_location(Point2D(126.5, 56.5), !swap),
							convert_location(Point2D(93.5, 39.5), !swap),
							convert_location(Point2D(125.5, 86.5), !swap),
							convert_location(Point2D(91.5, 71.5), !swap),
							convert_location(Point2D(59.5, 28.5), !swap),
							convert_location(Point2D(65.5, 53.5), !swap),
							convert_location(Point2D(26.5, 30.5), !swap) };

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
							P(129, 19),
							P(135, 30),
							P(122, 34),
							P(112, 20),
							P(106, 28),
							P(128, 79),
							P(124, 61),
							P(121, 88),
							P(126, 44) };
		
		first_pylon_location_zerg = P(116, 57);
		first_pylon_location_protoss = P(113, 43);
		first_pylon_location_terran = P(112, 44);

		gateway_locations = { P(114.5, 45.5),
								P(110.5, 38.5),
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

		cyber_core_locations = {P(115.5, 42.5),
									P(113.5, 54.5),
									P(109.75, 43.25) };

		tech_locations = { P(126.5, 34.5),
							P(123.5, 36.5),
							P(126.5, 37.5),
							P(120.5, 36.5),
							P(121.5, 31.5),
							P(118.5, 31.5),
							P(117.5, 36.5) };

		proxy_pylon_locations = { P(77, 105),
									P(45, 72) };

		proxy_gateway_locations = { P(79.5, 104.5),
									P(46.5, 69.5) };

		proxy_tech_locations = { P(77.5, 107.5),
									P(47.5, 72.5) };

		defensive_natural_battery_locations = { P(116, 61),
												P(118, 61),
												P(118, 56),
												P(120, 60) };

		gateway_walloff = P(118.5, 41.5);

		attack_path_short = {	P(117, 57),
								P(113, 59),
								P(107, 64),
								P(108, 71),
								P(105.5, 78),
								P(100.5, 80.5),
								P(80.5, 81.5),
								P(71.5, 86.5),
								P(51.5, 87.5),
								P(46.5, 90),
								P(44, 97),
								P(45, 104),
								P(39, 109),
								P(35, 111),
								P(32, 120),
								P(35, 123),
								P(31, 135.5),
								P(22, 141.5) };

		attack_path_short_line = PathManager(attack_path_short);

		attack_path = { P(106.5, 64.7),
						P(107.6, 72.1),
						P(110.4, 80.4),
						P(115.1, 83.1),
						P(121.1, 99.1),
						P(121, 103.3),
						P(106, 121),
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
		
		attack_path_special = { P(117, 57),
								P(113, 59),
								P(107, 64),
								P(108, 71),
								P(105.5, 78),
								P(100.5, 80.5),
								P(94.5, 89.5),
								P(94, 96),
								P(91.5, 103),
								P(85.5, 107.5),
								P(69, 115.5),
								P(65.5, 118.5),
								P(56, 119),
								P(47.5, 112.5),
								P(40.5, 110),
								P(35, 111),
								P(32, 120),
								P(35, 123),
								P(31, 135.5),
								P(22, 141.5) };

		attack_path_special_line = PathManager(attack_path_special);

		high_ground_index = 5;

		attack_path_alt = { P(70, 91),
							P(29, 81),
							P(28, 112),
							P(32, 140) };

		high_ground_index_alt = 2;

		initial_scout_pos = P(34.5, 122.5);

		main_scout_path = { P(34.5, 132),
							P(40, 143),
							P(22, 146.5),
							P(17, 133) };

		natural_scout_path = { P(18.5, 118.5),
								P(16.5, 108),
								P(30, 105.5), };

		possible_3rds = { P(58.5, 128.5),
						P(26.5, 81.5) };

		enemy_natural = P(25.5, 111.5);

		for (const auto& pos : main_scout_path)
		{
			self_main_scout_path.push_back(convert_location(pos, true));
		}

		for (const auto& pos : natural_scout_path)
		{
			self_natural_scout_path.push_back(convert_location(pos, true));
		}
		
		Point2D entrance_point = P(83, 150);
		Point2D exit_point = P(16, 90);
		std::vector<Point2D> entrance_points = { P(62, 138),
												P(30, 147),
												P(28, 121) };
		std::vector<Point2D> exit_points = { P(48, 125),
											P(17, 134),
											P(16, 108) };
		std::vector<Point2D> base_points = { P(58.5, 128.5),
											P(26.5, 137.5),
											P(25.5, 111.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(47, 151),
															P(80, 151)},
															{ P(12, 123),
															P(12, 97)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);

		std::vector<Point2D> oracle_points = { P(83, 150),
												P(16, 90),
												P(62, 138),
												P(30, 147),
												P(28, 121),
												P(48, 125),
												P(17, 134),
												P(16, 108),
												P(47, 151),
												P(80, 151),
												P(12, 123),
												P(12, 97),
												P(12, 151) };

		std::vector<std::vector<int>> oracle_links = {
			{ 9 },
			{ 11 },
			{ 5, 8, 9 },
			{ 5, 6, 8, 12 },
			{ 6, 7, 10 },
			{ 2, 3, 8 },
			{ 4, 10, 12 },
			{ 4, 10, 11 },
			{ 2, 3, 5, 9, 12 },
			{ 0, 2, 8 },
			{ 4, 6, 7, 11, 12 },
			{ 1, 7, 10 },
			{ 3, 6, 8, 10 }
		};

		//oracle_pathing = Graph(oracle_points, oracle, links);
		
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


		std::vector<Point2D> blink_nat_attack_path = { P(80, 111),
														P(67, 117),
														P(54, 119),
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

		std::vector<std::vector<Point2D>> blink_third_attack_path = { { P(60, 120),
																	P(55, 133) },
																	{ P(60, 120),
																	P(49, 113),
																	P(45, 102),
																	P(44, 96),
																	P(36, 82),
																	P(24, 84) } };

		blink_third_attack_path_lines = { PathManager(blink_third_attack_path[0]), PathManager(blink_third_attack_path[1]) };

		std::vector<Point2D> rocks = { Point2D(75, 67),
										Point2D(101, 111),
										Point2D(51, 57),
										Point2D(73, 103),
										Point2D(77, 101),
										Point2D(81, 47),
										Point2D(79, 65),
										Point2D(48, 61),
										Point2D(104, 107),
										Point2D(71, 121) };

		bad_warpin_spots = { first_pylon_location_terran, P(108, 42), P(112.5, 48), 
							Point2D(75, 67), Point2D(101, 111), Point2D(51, 57), Point2D(73, 103), Point2D(77, 101), 
							Point2D(81, 47), Point2D(79, 65), Point2D(48, 61), Point2D(104, 107), Point2D(71, 121) };

		worker_rush_defense_group = P(133, 29.5);
		worker_rush_defense_attack = P(19, 138.5);

		warp_prism_locations = { P(85, 105),
								P(80, 140),
								P(46, 145),
								P(18, 146),
								P(17, 115),
								P(41, 85) };

		main_ramp_forcefield_top = P(117, 45);
		main_ramp_forcefield_mid = P(19, 47);
	}
	
}

#pragma warning(pop)
