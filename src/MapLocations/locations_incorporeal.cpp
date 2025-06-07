
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

	void Locations::SetIncorporealLocations(Point3D start)
	{
		Point2D center_point = Point2D(0, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(156, 164) - point;
			else
				return point;
		};

		bool swap = start.x == 32.5 && start.y == 139.5;

		this->start_location = start;

		base_locations = { P(123.5, 24.5),
							P(96.5, 25.5),
							P(67.5, 25.5),
							P(113.5, 48.5),
							P(124.5, 77.5),
							P(33.5, 25.5),
							P(97.5, 77.5),
							convert_location(Point2D(123.5, 24.5), !swap),
							convert_location(Point2D(96.5, 25.5), !swap),
							convert_location(Point2D(67.5, 25.5), !swap),
							convert_location(Point2D(113.5, 48.5), !swap),
							convert_location(Point2D(124.5, 77.5), !swap),
							convert_location(Point2D(33.5, 25.5), !swap),
							convert_location(Point2D(97.5, 77.5), !swap) };

		nexi_locations = { P(123.5, 24.5),
							P(96.5, 25.5),
							P(67.5, 25.5),
							P(113.5, 48.5),
							P(124.5, 77.5),
							P(33.5, 25.5),
							P(97.5, 77.5) };

		pylon_locations = { P(128, 45),
							P(128, 43),
							P(118, 13),
							P(118, 15),
							P(116, 37),
							P(63, 25),
							P(115, 52),
							P(122, 28),
							P(120, 28),
							P(118, 28),
							P(110, 33),
							P(131, 16),
							P(97, 17),
							P(87, 23),
							P(62, 30),
							P(34, 30),
							P(31, 18),
							P(110, 47),
							P(120, 82),
							P(125, 87),
							P(133, 73),
							P(93, 76),
							P(101, 82) };
		
		first_pylon_location_zerg = P(93, 32);
		first_pylon_location_protoss = P(111, 21);
		first_pylon_location_terran = P(131, 30);

		gateway_locations = { P(96.5, 34.5), // z
							P(92.5, 34.5),
							P(108.5, 22.5), // p
							P(111.5, 23.5),
							P(133.5, 30.5), // t
							//P(),
							P(125.5, 43.5),
							P(125.5, 40.5),
							P(130.5, 43.5),
							P(130.5, 40.5),
							P(120.5, 13.5),
							P(115.5, 14.5),
							P(115.5, 17.5),
							P(113.5, 35.5),
							P(113.5, 32.5),
							P(118.5, 37.5),
							P(116.5, 34.5),
							P(121.5, 37.5),
							P(126.5, 14.5),
							P(133.5, 19.5),
							P(56.5, 28.5),
							P(59.5, 30.5),
							P(62.5, 32.5),
							P(63.5, 35.5) };

		assimilator_locations = { P(119.5, 17.5),
									P(130.5, 27.5),
									P(92.5, 18.5),
									P(89.5, 22.5),
									P(74.5, 28.5),
									P(63.5, 18.5),
									P(109.5, 41.5),
									P(120.5, 51.5),
									P(129.5, 71.5),
									P(125.5, 70.5),
									P(37.5, 18.5),
									P(26.5, 28.5) };

		cyber_core_locations = { P(89.5, 33.5),
									P(108.5, 18.5),
									P(127.5, 29.5) };
		
		tech_locations = { P(124.5, 28.5),
							P(119.5, 25.5),
							P(116.5, 25.5),
							P(121.5, 30.5),
							P(124.5, 31.5) };
		
		proxy_pylon_locations = { P(59, 91),
								P(84, 138) };

		proxy_gateway_locations = { P(56.5, 89.5),
									P(85.5, 140.5) };

		proxy_tech_locations = { P(59.5, 88.5),
								P(86.5, 137.5) };

		defensive_natural_battery_locations = { P(96, 32),
												P(90, 31),
												P(98, 32) };

		gateway_walloff = P(112.5, 26.5);
		
		attack_path_short = { P(94, 33),
								P(92, 37),
								P(94, 47),
								P(99, 52),
								P(98, 60),
								P(92, 66),
								P(90, 75),
								P(94, 80),
								P(93, 93),
								P(87, 99),
								P(82, 110),
								P(76, 116),
								P(65, 126),
								P(63, 130),
								P(60, 132),
								P(57, 132),
								P(52, 134),
								P(46, 140),
								P(41, 142),
								P(28, 143) };

		attack_path_short_line = PathManager(attack_path_short);

		attack_path = { P(94, 33),
						P(92, 37),
						P(94, 47),
						P(99, 52),
						P(98, 60),
						P(92, 66),
						P(90, 75),
						P(94, 80),
						P(100, 88),
						P(106, 94),
						P(108, 105),
						P(104, 110),
						P(102, 118),
						P(104, 121),
						P(102, 126),
						P(96, 132),
						P(86, 133),
						P(75, 124),
						P(65, 126),
						P(63, 130),
						P(60, 132),
						P(57, 132),
						P(52, 134),
						P(46, 140),
						P(41, 142),
						P(28, 143) };

		attack_path_line = PathManager(attack_path);

		
		
		high_ground_index = 5;

		attack_path_alt = { P(59, 87),
							P(32, 87),
							P(42, 116),
							P(60, 138),
							P(34, 140) };

		high_ground_index_alt = 2;

		initial_scout_pos = P(48, 138);
		
		main_scout_path = { P(37, 132),
							P(27, 128),
							P(21, 138),
							P(25, 148),
							P(40, 150) };

		natural_scout_path = { P(62, 134),
								P(69, 144),
								P(56, 147), };

		possible_3rds = { P(88.5, 138.5),
						P(42.5, 115.5) };


		enemy_natural = P(59.5, 138.5);

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

		*/
		third_base_pylon_gap = P(64.5, 25);
		natural_door_closed = P(94.5, 35);
		natural_door_open = P(94.5, 39);
		

		adept_harrass_protoss_consolidation = { P(61, 114), P(79, 127) };
		
		main_early_dead_space = P(129, 42);
		natural_front = { P(95.5, 37),
							P(89, 34.5) };

		
		adept_scout_shade = P(63, 124);
		adept_scout_runaway = P(76, 115);
		adept_scout_ramptop = P(47, 139);
		adept_scout_nat_path = { P(55, 148),
									P(68, 147) };
		adept_scout_base_spots = { P(27, 125),
									P(23, 135),
									P(27, 147),
									P(42, 149) };
		

		blink_presure_consolidation = P(73, 116);
		blink_pressure_prism_consolidation = P(75, 113);
		blink_pressure_blink_up = P(38, 118);
		blink_pressure_blink_down = P(31, 121);


		std::vector<Point2D> blink_nat_attack_path = { P(93, 93),
														P(87, 99),
														P(82, 110),
														P(76, 116),
														P(65, 126),
														P(63, 130),
														P(58, 143) };

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(31, 121),
																	P(44, 146) },
																	{ P(31, 121),
																	P(28, 143) } };

		blink_main_attack_path_lines = { PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1]) };

		std::vector<std::vector<Point2D>> blink_third_attack_path = { { P(73, 116),
																	P(93, 143) },
																	{ P(61, 117),
																	P(57, 112),
																	P(47, 113),
																	P(39, 119) } };

		blink_third_attack_path_lines = { PathManager(blink_third_attack_path[0]), PathManager(blink_third_attack_path[1]) };

		
		bad_warpin_spots = {  };
		
		worker_rush_defense_group = P(130, 24.5);
		worker_rush_defense_attack = P(62, 131.5);

		warp_prism_locations = { P(72, 84),
								P(44, 100),
								P(26, 122),
								P(24, 148),
								P(60, 148),
								P(94, 148) };

		main_ramp_forcefield_top = P(47, 139);
		main_ramp_forcefield_mid = P(49, 137);
	}
	
}

#pragma warning(pop)
