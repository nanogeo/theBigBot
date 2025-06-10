
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

	void Locations::SetPersephoneLocations(Point3D start)
	{
		Point2D center_point = Point2D(0, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(point.x, 180 - point.y);
			else
				return point;
		};

		bool swap = start.x == 37.5 && start.y == 145.5;

		this->start_location = start;

		base_locations = { P(37.5, 34.5),
							P(65.5, 30.5),
							P(60.5, 56.5),
							P(92.5, 28.5),
							P(130.5, 29.5),
							P(112.5, 60.5),
							P(30.5, 61.5),
							P(132.5, 90.5),
							convert_location(Point2D(37.5, 34.5), !swap),
							convert_location(Point2D(65.5, 30.5), !swap),
							convert_location(Point2D(60.5, 56.5), !swap),
							convert_location(Point2D(92.5, 28.5), !swap),
							convert_location(Point2D(130.5, 29.5), !swap),
							convert_location(Point2D(112.5, 60.5), !swap),
							convert_location(Point2D(30.5, 61.5), !swap),
							convert_location(Point2D(132.5, 90.5), !swap) };

		nexi_locations = { P(37.5, 34.5),
							P(65.5, 30.5),
							P(60.5, 56.5),
							P(92.5, 28.5),
							P(130.5, 29.5),
							P(112.5, 60.5),
							P(30.5, 61.5),
							P(132.5, 90.5) };

		pylon_locations = { P(39, 38),
							P(60, 61),
							P(96, 28),
							P(110, 64),
							P(132, 33),
							P(34, 65),
							P(51, 57),
							P(35, 48),
							P(37, 22),
							P(41, 38),
							P(25, 34),
							P(88, 21),
							P(135, 23),
							P(113, 57),
							P(131, 87),
							P(131, 94),
							P(28, 55),
							P(27, 34),
							P(37, 24),
							P(35, 46),
							P(61, 24),
							P(70, 32) };
		
		first_pylon_location_zerg = P(69, 36);
		first_pylon_location_protoss = P(53, 43);
		first_pylon_location_terran = P(49, 54);

		gateway_locations = { P(70.5, 41.5), // z
							P(71.5, 37.5),
							P(55.5, 41.5), // p
							P(52.5, 40.5),
							P(46.25, 54.25), // t
							P(46.5, 51.5),
							P(54.5, 61.5),
							P(57.5, 61.5),
							P(62.5, 60.5),
							P(51.5, 61.5),
							P(39.5, 22.5),
							P(33.5, 24.5),
							P(26.5, 31.5),
							P(25.5, 37.5),
							P(31.5, 45.5),
							P(37.5, 47.5),
							P(40.5, 48.5) };

		assimilator_locations = { P(41.5, 27.5),
									P(30.5, 37.5),
									P(69.5, 23.5),
									P(72.5, 27.5),
									P(63.5, 49.5),
									P(53.5, 58.5),
									P(85.5, 31.5),
									P(96.5, 21.5),
									P(126.5, 22.5),
									P(137.5, 32.5),
									P(114.5, 53.5),
									P(114.5, 67.5),
									P(25.5, 55.5),
									P(23.5, 59.5),
									P(135.5, 83.5),
									P(135.5, 97.5) };

		cyber_core_locations = { P(74.5, 35.5),
									P(55.5, 44.5),
									P(49.25, 51.25) };

		tech_locations = { P(36.5, 38.5),
							P(39.5, 40.5),
							P(42.5, 40.5),
							P(41.5, 35.5),
							P(44.5, 35.5) };

		proxy_pylon_locations = { P(56, 105),
								P(109, 145) };

		proxy_gateway_locations = { P(53.5, 104.5),
									P(109.5, 142.5) };

		proxy_tech_locations = { P(53.5, 108.5),
								P(113.5, 143.5) };
		
		defensive_natural_battery_locations = { P(69, 38),
												P(71, 35),
												P(68, 41) };

		gateway_walloff = P(51.5, 37.5);
		
		attack_path_short = { P(74, 40),
								P(96, 68),
								P(98, 79),
								P(93, 85),
								P(93, 96),
								P(98, 101),
								P(96, 113),
								P(71, 144),
								P(61, 147),
								P(54, 140),
								P(43, 139),
								P(34, 148) };

		attack_path_short_line = PathManager(attack_path_short);

		attack_path = { P(74, 40),
						P(96, 68),
						P(98, 79),
						P(93, 85),
						P(93, 96),
						P(106, 114),
						P(108, 124),
						P(102, 130),
						P(99, 138),
						P(97, 144),
						P(88, 144),
						P(82, 138),
						P(75, 139),
						P(71, 144),
						P(61, 147),
						P(54, 140),
						P(43, 139),
						P(34, 148) };

		attack_path_line = PathManager(attack_path);

		

		high_ground_index = 5;

		attack_path_alt = { P(30, 118),
							P(60, 123),
							P(65, 149),
							P(37, 145) };

		high_ground_index_alt = 2;
		
		initial_scout_pos = P(55, 141);

		main_scout_path = { P(46, 132),
							P(24, 145),
							P(39, 158),
							P(47, 143) };

		natural_scout_path = { P(74, 148),
								P(69, 159),
								P(57, 153), };
		
		possible_3rds = { P(92.5, 151.5),
						P(60.5, 123.5) };


		enemy_natural = P(65.5, 149.5);
		
		for (const auto& pos : main_scout_path)
		{
			self_main_scout_path.push_back(convert_location(pos, true));
		}

		for (const auto& pos : natural_scout_path)
		{
			self_natural_scout_path.push_back(convert_location(pos, true));
		}
		
		Point2D entrance_point = P(115, 160);
		Point2D exit_point = P(21, 123);
		std::vector<Point2D> entrance_points = { P(95.5, 158.5),
												P(72.5, 153),
												P(41, 153) };
		std::vector<Point2D> exit_points = { P(84, 148.5),
											P(57.5, 150),
											P(29, 142.5) };
		std::vector<Point2D> base_points = { P(37.5, 145.5),
											P(65.5, 149.5),
											P(92.5, 151.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(80, 160),
															P(110, 160)},
															{ P(52, 160),
															P(21, 160),
															P(21, 132)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);

		
		third_base_pylon_gap = P(60, 59.5);
		natural_door_closed = P(71.5, 39.5);
		natural_door_open = P(75.5, 39.5);
		
		
		adept_harrass_protoss_consolidation = { P(70, 126), P(88, 143) };
		
		main_early_dead_space = P(45, 53);
		natural_front = { P(71, 42),
							P(76, 37) };


		adept_scout_shade = P(76, 136);
		adept_scout_runaway = P(85, 125);
		adept_scout_ramptop = P(55, 141);
		adept_scout_nat_path = { P(59, 156),
									P(72, 157) };
		adept_scout_base_spots = { P(46, 131),
									P(36, 136),
									P(27, 147),
									P(41, 156) };


		blink_presure_consolidation = P(87, 123);
		blink_pressure_prism_consolidation = P(90, 119);
		blink_pressure_blink_up = P(51, 121);
		blink_pressure_blink_down = P(48, 126);


		std::vector<Point2D> blink_nat_attack_path = { P(93, 96),
														P(98, 101),
														P(96, 113),
														P(62, 153) };

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(48, 126),
																	P(33, 146) },
																	{ P(48, 126),
																	P(51, 143) } };

		blink_main_attack_path_lines = { PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1]) };

		std::vector<std::vector<Point2D>> blink_third_attack_path = { { P(87, 123),
																	P(96, 130),
																	P(98, 140),
																	P(89, 153) },
																	{ P(87, 123),
																	P(82, 126),
																	P(71, 127) ,
																	P(57, 122)  } };

		blink_third_attack_path_lines = { PathManager(blink_third_attack_path[0]), PathManager(blink_third_attack_path[1]) };


		bad_warpin_spots = { first_pylon_location_terran };
		
		worker_rush_defense_group = P(37, 27.5);
		worker_rush_defense_attack = P(37, 152.5);

		warp_prism_locations = { P(61, 97),
								P(46, 126),
								P(26, 149),
								P(66, 158),
								P(100, 157),
								P(100, 127) };
		
		main_ramp_forcefield_top = P(55, 39);
		main_ramp_forcefield_mid = P(57, 37);
	}
	
}

#pragma warning(pop)
