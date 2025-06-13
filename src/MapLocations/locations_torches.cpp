
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

	void Locations::SetTorchesLocations(Point3D start)
	{
		Point2D center_point = Point2D(0, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(point.x, 208 - point.y);
			else
				return point;
		};

		bool swap = start.x == 124.5 && start.y == 159.5;

		this->start_location = start;

		base_locations = { P(124.5, 48.5),
							P(95.5, 50.5),
							P(90.5, 75.5),
							P(62.5, 48.5),
							P(31.5, 47.5),
							P(34.5, 74.5),
							P(57.5, 97.5),
							P(124.5, 76.5),
							P(125.5, 97.5),
							convert_location(Point2D(124.5, 48.5), !swap),
							convert_location(Point2D(95.5, 50.5), !swap),
							convert_location(Point2D(90.5, 75.5), !swap),
							convert_location(Point2D(62.5, 48.5), !swap),
							convert_location(Point2D(31.5, 47.5), !swap),
							convert_location(Point2D(34.5, 74.5), !swap),
							convert_location(Point2D(57.5, 97.5), !swap),
							convert_location(Point2D(124.5, 76.5), !swap),
							convert_location(Point2D(125.5, 97.5), !swap) };

		nexi_locations = { P(124.5, 48.5),
							P(95.5, 50.5),
							P(90.5, 75.5),
							P(62.5, 48.5),
							P(31.5, 47.5),
							P(34.5, 74.5),
							P(57.5, 97.5),
							P(124.5, 76.5),
							P(125.5, 97.5) };

		pylon_locations = { P(121, 50),
							P(91, 80),
							P(59, 51),
							P(29, 51),
							P(38, 73),
							P(54, 97),
							P(122, 80),
							P(122, 98),
							P(87, 77),
							P(111, 36),
							P(138, 45),
							P(126, 36),
							P(67, 42),
							P(29, 39),
							P(27, 80),
							P(61, 99),
							P(133, 77),
							P(129, 98),
							P(136, 45),
							P(126, 38),
							P(111, 38),
							P(121, 52) };
		
		first_pylon_location_zerg = P(90, 54);
		first_pylon_location_protoss = P(113, 51);
		first_pylon_location_terran = P(133, 54);

		gateway_locations = { P(91.5, 59.5), // z
							P(90.5, 56.5),
							P(110.5, 52.5), // p
							P(110.5, 49.5),
							P(135.5, 54.5), // t
							P(135.5, 57.5),
							P(96.5, 82.5),
							P(93.5, 82.5),
							P(90.5, 82.5),
							P(88.5, 79.5),
							P(135.5, 48.5),
							P(135.5, 41.5),
							P(128.5, 36.5),
							P(123.5, 36.5),
							P(129.5, 39.5),
							P(113.5, 36.5),
							P(113.5, 39.5),
							P(107.5, 40.5) };

		assimilator_locations = { P(121.5, 41.5),
									P(131.5, 51.5),
									P(91.5, 43.5),
									P(88.5, 47.5),
									P(88.5, 68.5),
									P(97.5, 79.5),
									P(69.5, 47.5),
									P(55.5, 47.5),
									P(24.5, 50.5),
									P(34.5, 40.5),
									P(31.5, 67.5),
									P(27.5, 71.5),
									P(50.5, 99.5),
									P(121.5, 69.5),
									P(131.5, 79.5),
									P(132.5, 99.5) };

		cyber_core_locations = { P(86.5, 55.5),
									P(113.5, 53.5),
									P(128.5, 52.5) };

		tech_locations = { P(123.5, 52.5),
							P(123.5, 55.5),
							P(118.5, 54.5),
							P(118.5, 51.5),
							P(120.5, 47.5) };
		
		proxy_pylon_locations = { P(69, 113),
								P(48, 137) };

		proxy_gateway_locations = { P(66.5, 114.5),
									P(46.5, 134.5) };

		proxy_tech_locations = { P(65.5, 110.5),
								P(45.5, 137.5) };

		defensive_natural_battery_locations = { P(87, 53),
												P(92, 54),
												P(93, 57) };

		gateway_walloff = P(114.5, 56.5);
		
		attack_path_short = { P(85, 61),
								P(82, 64),
								P(79, 72),
								P(82, 75),
								P(83.5, 82),
								P(83, 126),
								P(82, 133),
								P(79, 136),
								P(82, 144),
								P(87, 151),
								P(97, 152),
								P(100, 149),
								P(105, 147),
								P(112, 154),
								P(118, 157),
								P(128, 162) };

		attack_path_short_line = PathManager(attack_path_short);

		attack_path = { P(85, 61),
						P(82, 64),
						P(79, 72),
						P(82, 75),
						P(83.5, 82),
						P(83, 126),
						P(82, 133),
						P(79, 136),
						P(82, 144),
						P(87, 151),
						P(97, 152),
						P(100, 149),
						P(105, 147),
						P(112, 154),
						P(118, 157),
						P(128, 162) };

		attack_path_line = PathManager(attack_path);

		

		high_ground_index = 5;

		attack_path_alt = { P(34, 133),
							P(31, 160),
							P(62, 159),
							P(95, 157),
							P(124, 159) };

		high_ground_index_alt = 2;
		
		initial_scout_pos = P(111, 153);
		
		main_scout_path = { P(119, 151),
							P(136, 152),
							P(133, 170),
							P(109, 169) };

		natural_scout_path = { P(83, 159),
								P(91, 168),
								P(102, 165) };

		possible_3rds = { P(90.5, 132.5),
						P(62.5, 159.5) };


		enemy_natural = P(95.5, 157.5);

		for (const auto& pos : main_scout_path)
		{
			self_main_scout_path.push_back(convert_location(pos, true));
		}

		for (const auto& pos : natural_scout_path)
		{
			self_natural_scout_path.push_back(convert_location(pos, true));
		}
		
		Point2D entrance_point = P(137, 128);
		Point2D exit_point = P(33, 175);
		std::vector<Point2D> entrance_points = { P(131.5, 155),
												P(105, 159),
												P(71, 162) };
		std::vector<Point2D> exit_points = { P(121.5, 168.5),
											P(88.5, 161.5),
											P(54, 162) };
		std::vector<Point2D> base_points = { P(124.5, 159.5),
											P(95.5, 157.5),
											P(62.5, 159.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(108, 175),
															P(140, 175),
															P(140, 140)},
															{ P(77, 175),
															P(45, 175)} };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);

		
		third_base_pylon_gap = P(91, 78.5);
		natural_door_closed = P(88.5, 55.5);
		natural_door_open = P(88.5, 59.5);
		

		adept_harrass_protoss_consolidation = { P(85, 133), P(70, 148) };
		
		main_early_dead_space = P(132, 63);
		natural_front = { P(85.5, 55),
							P(91, 61) };


		adept_scout_shade = P(81, 139);
		adept_scout_runaway = P(80, 130);
		adept_scout_ramptop = P(111, 153);
		adept_scout_nat_path = { P(99, 167),
									P(89, 166) };
		adept_scout_base_spots = { P(132, 148),
									P(135, 161),
									P(126, 170),
									P(110, 170) };

		
		blink_presure_consolidation = P(85, 125);
		blink_pressure_prism_consolidation = P(84, 120);
		blink_pressure_blink_up = P(0, 0);
		blink_pressure_blink_down = P(0, 0);


		std::vector<Point2D> blink_nat_attack_path = { P(83.5, 82),
														P(83, 126),
														P(82, 133),
														P(79, 136),
														P(82, 144),
														P(87, 151),
														P(97, 152),
														P(100, 149),
														P(105, 147),
														P(112, 154),
														P(118, 157),
														P(128, 162) };

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { {},
																	{} };

		blink_main_attack_path_lines = {}; // PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1]) };

		std::vector<std::vector<Point2D>> blink_third_attack_path = { { P(85, 125),
																	P(93, 135) },
																	{ P(85, 125),
																	P(84, 130),
																	P(78, 138),
																	P(66, 150),
																	P(61, 157),
																	P(60, 164) } };

		blink_third_attack_path_lines = { PathManager(blink_third_attack_path[0]), PathManager(blink_third_attack_path[1]) };


		bad_warpin_spots = { first_pylon_location_terran };
		
		worker_rush_defense_group = P(132, 47.5);
		worker_rush_defense_attack = P(132, 160.5);

		warp_prism_locations = { P(81, 115),
								P(105, 139),
								P(135, 148),
								P(133, 169),
								P(87, 167),
								P(53, 166),
								P(47, 131) };

		main_ramp_forcefield_top = P(111, 55);
		main_ramp_forcefield_mid = P(109, 57);
	}
	
}

#pragma warning(pop)
