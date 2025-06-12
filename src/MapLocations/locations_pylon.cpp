
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

	void Locations::SetPylonLocations(Point3D start)
	{
		Point2D center_point = Point2D(0, 0);

		auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(248, 248) - point;
			else
				return point;
		};

		bool swap = start.x == 72.5 && start.y == 171.5;

		this->start_location = start;

		base_locations = { P(175.5, 76.5),
							P(149.5, 74.5),
							P(154.5, 104.5),
							P(119.5, 69.5),
							P(90.5, 94.5),
							P(70.5, 68.5),
							P(177.5, 123.5),
							convert_location(Point2D(175.5, 76.5), !swap),
							convert_location(Point2D(149.5, 74.5), !swap),
							convert_location(Point2D(154.5, 104.5), !swap),
							convert_location(Point2D(119.5, 69.5), !swap),
							convert_location(Point2D(90.5, 94.5), !swap),
							convert_location(Point2D(70.5, 68.5), !swap),
							convert_location(Point2D(177.5, 123.5), !swap) };

		nexi_locations = { P(175.5, 76.5),
							P(149.5, 74.5),
							P(154.5, 104.5),
							P(119.5, 69.5),
							P(90.5, 94.5),
							P(70.5, 68.5),
							P(177.5, 123.5) };

		pylon_locations = { P(174, 80),
							P(155, 109),
							P(116, 70),
							P(177, 64),
							P(143, 69),
							P(178, 127),
							P(164, 105),
							P(69, 72),
							P(88, 98),
							P(163, 76),
							P(178, 92),
							P(187, 76),
							P(172, 80),
							P(165, 76),
							P(179, 64),
							P(175, 90),
							P(156, 68),
							P(123, 73),
							P(94, 92),
							P(65, 61),
							P(182, 117) };
		
		first_pylon_location_zerg = P(144, 82);
		first_pylon_location_protoss = P(163, 87);
		first_pylon_location_terran = P(164, 95);

		gateway_locations = { P(142.5, 85.5), // z
							P(141.5, 81.5),
							P(160.5, 85.5), // p
							P(163.5, 84.5),
							P(161.5, 93.5), // t
							P(166.5, 93.5),
							P(157.5, 110.5),
							P(160.5, 110.5),
							P(154.5, 111.5),
							P(152.5, 108.5),
							P(164.5, 107.5),
							P(164.5, 78.5),
							P(164.5, 73.5),
							P(167.5, 74.5),
							P(174.5, 64.5),
							P(171.5, 64.5),
							P(182.5, 66.5),
							P(177.5, 66.5),
							P(185.5, 73.5),
							P(185.5, 78.5),
							P(180.5, 91.5),
							P(181.5, 88.5),
							P(175.5, 92.5),
							P(177.5, 89.5) };

		assimilator_locations = { P(182.5, 79.5),
									P(171.5, 69.5),
									P(145.5, 67.5),
									P(142.5, 71.5),
									P(150.5, 97.5),
									P(161.5, 107.5),
									P(115.5, 62.5),
									P(112.5, 66.5),
									P(92.5, 87.5),
									P(73.5, 61.5),
									P(63.5, 71.5),
									P(173.5, 116.5),
									P(184.5, 126.5) };

		cyber_core_locations = { P(138.5, 79.5),
									P(160.5, 88.5),
									P(166.75, 96.25) };

		tech_locations = { P(176.5, 80.5),
							P(173.5, 82.5),
							P(170.5, 82.5),
							P(171.5, 77.5),
							P(176.5, 83.5) };

		proxy_pylon_locations = { P(137, 150),
								P(100, 123) };

		proxy_gateway_locations = { P(139.5, 150.5),
									P(103.5, 123.5) };

		proxy_tech_locations = { P(137.5, 153.5),
								P(102.5, 119.5) };
		
		defensive_natural_battery_locations = { P(144, 79),
												P(146, 81),
												P(141, 78) };

		gateway_walloff = P(164.5, 81.5);
		
		attack_path_short = { P(138, 86),
								P(128, 98),
								P(123, 103),
								P(117, 109),
								P(109, 124),
								P(105, 134),
								P(104, 149),
								P(109, 155),
								P(109, 164),
								P(104, 169),
								P(92, 170),
								P(86, 164),
								P(76, 166),
								P(69, 176) };

		attack_path_short_line = PathManager(attack_path_short);

		attack_path = { P(140, 94),
						P(145, 100),
						P(145, 113),
						P(140, 121),
						P(131, 139),
						P(125, 145),
						P(120, 150),
						P(114, 156),
						P(109, 164),
						P(104, 169),
						P(92, 170),
						P(86, 164),
						P(76, 166),
						P(69, 176) };

		attack_path_line = PathManager(attack_path);

		

		high_ground_index = 5;

		attack_path_alt = { P(157, 153),
							P(177, 179),
							P(128, 178),
							P(98, 173),
							P(72, 171) };

		high_ground_index_alt = 2;
		
		initial_scout_pos = P(87, 165);

		main_scout_path = { P(78, 154),
							P(61, 168),
							P(67, 182),
							P(80, 177) };

		natural_scout_path = { P(109, 174),
								P(100, 185),
								P(89, 177), };

		possible_3rds = { P(93.5, 143.5),
						P(128.5, 178.5) };

		enemy_natural = P(98.5, 173.5);

		for (const auto& pos : main_scout_path)
		{
			self_main_scout_path.push_back(convert_location(pos, true));
		}

		for (const auto& pos : natural_scout_path)
		{
			self_natural_scout_path.push_back(convert_location(pos, true));
		}
		
		Point2D entrance_point = P(155, 188);
		Point2D exit_point = P(61, 142);
		std::vector<Point2D> entrance_points = { P(135.5, 183),
												P(105.5, 178),
												P(75.5, 180) };
		std::vector<Point2D> exit_points = { P(120.5, 178.5),
											P(90, 174),
											P(64, 169) };
		std::vector<Point2D> base_points = { P(128.5, 178.5),
											P(98.5, 173.5),
											P(72.5, 171.5) };
		std::vector<std::vector<Point2D>> exfi_paths = { {P(113, 190),
															P(147, 189)},
															{ P(84, 190),
															P(59, 190),
															P(59, 153) } };

		oracle_path = OraclePath(entrance_point, exit_point, entrance_points, exit_points, base_points, exfi_paths);

		
		third_base_pylon_gap = P(155, 107.5);
		natural_door_closed = P(142.5, 83.5);
		natural_door_open = P(138.5, 83.5);
		

		adept_harrass_protoss_consolidation = { P(104, 148), P(124, 167) };
		
		main_early_dead_space = P(176, 93);
		natural_front = { P(143, 86),
							P(137, 80) };


		adept_scout_shade = P(110, 156);
		adept_scout_runaway = P(107, 142);
		adept_scout_ramptop = P(87, 165);
		adept_scout_nat_path = { P(94, 183),
									P(106, 182) };
		adept_scout_base_spots = { P(77, 153),
									P(65, 161),
									P(63, 176),
									P(78, 181) };

		
		blink_presure_consolidation = P(105, 131);
		blink_pressure_prism_consolidation = P(108, 127);
		blink_pressure_blink_up = P(83, 143);
		blink_pressure_blink_down = P(81, 151);


		std::vector<Point2D> blink_nat_attack_path = { P(123, 103),
														P(117, 109),
														P(109, 124),
														P(105, 134),
														P(104, 149),
														P(109, 155),
														P(109, 164),
														P(97, 177) };

		blink_nat_attack_path_line = PathManager(blink_nat_attack_path);


		std::vector<std::vector<Point2D>> blink_main_attack_path = { { P(81, 151),
																	P(86, 165) },
																	{ P(81, 151),
																	P(70, 161),
																	P(68, 170),
																	P(71, 176) } };

		blink_main_attack_path_lines = { PathManager(blink_main_attack_path[0]), PathManager(blink_main_attack_path[1]) };

		std::vector<std::vector<Point2D>> blink_third_attack_path = { { P(105, 131),
																	P(90, 147) },
																	{ P(105, 131),
																	P(102, 135),
																	P(104, 149),
																	P(132, 179) } };

		blink_third_attack_path_lines = { PathManager(blink_third_attack_path[0]), PathManager(blink_third_attack_path[1]) };


		bad_warpin_spots = { first_pylon_location_terran };
		
		worker_rush_defense_group = P(183, 75.5);
		worker_rush_defense_attack = P(65, 172.5);

		warp_prism_locations = { P(109, 123),
								P(77, 151),
								P(61, 176),
								P(99, 186),
								P(137, 186),
								P(135, 152) };
								
		main_ramp_forcefield_top = P(163, 83);
		main_ramp_forcefield_mid = P(161, 81);
	}
	
}

#pragma warning(pop)
