#pragma once
#include "locations.h"
#include "TossBot.h"

#include <iostream>
#include <string>




namespace sc2 {

void Locations::SetLightshadeLocations(Point3D start_location, BuildOrder build_order)
{
	auto convert_location = [](Point2D point, bool swap)
	{
		if (swap)
			return Point2D(184, 164) - point;
		else
			return point;
	};

	bool swap = start_location.x == 40.5 && start_location.y == 131.5;

	nexi_locations = { convert_location(Point2D(143.5, 32.5), swap),
						convert_location(Point2D(145.5, 61.5), swap),
						convert_location(Point2D(147.5, 94.5), swap),
						convert_location(Point2D(119.5, 53.5), swap),
						convert_location(Point2D(104.5, 27.5), swap),
						convert_location(Point2D(148.5, 125.5), swap),
						convert_location(Point2D(124.5, 120.5), swap) };

	pylon_locations = { convert_location(Point2D(140, 36), swap),
						convert_location(Point2D(124, 29), swap),
						convert_location(Point2D(147, 99), swap),
						convert_location(Point2D(146, 91), swap),
						convert_location(Point2D(135, 36), swap),
						convert_location(Point2D(116, 50), swap),
						convert_location(Point2D(121, 57), swap),
						convert_location(Point2D(150, 26), swap),
						convert_location(Point2D(154, 64), swap),
						convert_location(Point2D(156, 97), swap),
						convert_location(Point2D(134, 24), swap),
						convert_location(Point2D(149, 45), swap),
						convert_location(Point2D(156, 92), swap),
						convert_location(Point2D(150, 70), swap),
						convert_location(Point2D(101, 27), swap),
						convert_location(Point2D(106, 31), swap),
						convert_location(Point2D(110, 21), swap),
						convert_location(Point2D(99, 20), swap),
						convert_location(Point2D(134, 26), swap) };

	first_pylon_location_zerg = convert_location(Point2D(142, 63), swap);
	first_pylon_location_protoss = convert_location(Point2D(135, 47), swap);
	first_pylon_location_terran = convert_location(Point2D(131, 45), swap);

	gateway_locations = { convert_location(Point2D(141.5, 68.5), swap),
							convert_location(Point2D(136.5, 62.5), swap),
							convert_location(Point2D(136.5, 49.5), swap),
							convert_location(Point2D(128.5, 43.5), swap),
							convert_location(Point2D(133.5, 44.5), swap),
							convert_location(Point2D(143.5, 100.5), swap),
							convert_location(Point2D(142.5, 97.5), swap),
							convert_location(Point2D(142.5, 94.5), swap),
							convert_location(Point2D(143.5, 91.5), swap),
							convert_location(Point2D(122.5, 32.5), swap),
							convert_location(Point2D(125.5, 32.5), swap),
							convert_location(Point2D(128.5, 32.5), swap),
							convert_location(Point2D(126.5, 25.5), swap),
							convert_location(Point2D(136.5, 22.5), swap),
							convert_location(Point2D(139.5, 22.5), swap),
							convert_location(Point2D(136.5, 25.5), swap),
							convert_location(Point2D(136.5, 28.5), swap),
							convert_location(Point2D(130.5, 22.5), swap),
							convert_location(Point2D(130.5, 25.5), swap),
							convert_location(Point2D(139.5, 33.5), swap),
							convert_location(Point2D(136.5, 33.5), swap),
							convert_location(Point2D(139.5, 30.5), swap),
							convert_location(Point2D(137.5, 36.5), swap),
							convert_location(Point2D(142.5, 36.5), swap),
							convert_location(Point2D(145.5, 36.5), swap),
							convert_location(Point2D(134.5, 38.5), swap),
							convert_location(Point2D(137.5, 39.5), swap),
							convert_location(Point2D(142.5, 39.5), swap), };

	assimilator_locations = { convert_location(Point2D(150.5, 35.5), swap),
								convert_location(Point2D(139.5, 25.5), swap),
								convert_location(Point2D(152.5, 58.5), swap),
								convert_location(Point2D(148.5, 54.5), swap) };

	cyber_core_locations = { convert_location(Point2D(139.5, 64.5), swap),
								convert_location(Point2D(137.5, 46.5), swap),
								convert_location(Point2D(132.5, 47.5), swap) };

	tech_locations = { convert_location(Point2D(139.5, 33.5), swap),
						convert_location(Point2D(142.5, 36.5), swap),
						convert_location(Point2D(137.5, 36.5), swap),
						convert_location(Point2D(136.5, 33.5), swap),
						convert_location(Point2D(145.5, 36.5), swap) };

	proxy_pylon_locations = { convert_location(Point2D(33.0, 69.0), swap),
								convert_location(Point2D(74.0, 117.0), swap) };

	proxy_gateway_locations = { convert_location(Point2D(37.5, 69.5), swap),
							convert_location(Point2D(117.5, 121.5), swap) };

	proxy_robo_locations = { convert_location(Point2D(37.5, 69.5), swap),
							convert_location(Point2D(117.5, 121.5), swap) };

	attack_path = { convert_location(Point2D(74, 35), swap),
					convert_location(Point2D(50, 60), swap),
					convert_location(Point2D(44, 69), swap),
					convert_location(Point2D(44, 76), swap),
					convert_location(Point2D(48, 82), swap),
					convert_location(Point2D(52, 89), swap),
					convert_location(Point2D(49, 97), swap),
					convert_location(Point2D(44, 102), swap),
					convert_location(Point2D(41, 111), swap),
					convert_location(Point2D(44, 114), swap),
					convert_location(Point2D(43, 124), swap) };

	std::vector<Point2D> air_path = { convert_location(Point2D(26, 17), swap),
										convert_location(Point2D(102, 147), swap),
										convert_location(Point2D(24, 85), swap),
										convert_location(Point2D(58, 126), swap),
										convert_location(Point2D(63, 148), swap),
										convert_location(Point2D(24, 53), swap),
										convert_location(Point2D(77, 16), swap),
										convert_location(Point2D(24, 148), swap),
										convert_location(Point2D(24, 115), swap),
										convert_location(Point2D(59, 86), swap),
										convert_location(Point2D(48, 112), swap),
										convert_location(Point2D(83, 96), swap),
										convert_location(Point2D(91, 118), swap),
										convert_location(Point2D(49, 58), swap),
										convert_location(Point2D(78, 49), swap),
										convert_location(Point2D(57, 41), swap),
										convert_location(Point2D(35, 101), swap),
										convert_location(Point2D(62, 112), swap),
										convert_location(Point2D(32, 69), swap),
										convert_location(Point2D(34, 39), swap),
										convert_location(Point2D(77, 138), swap),
										convert_location(Point2D(37, 133), swap) };

	std::vector<std::vector<int>> links = {
		{5, 6, 19},
		{4, 12, 20},
		{5, 8, 9, 16, 18},
		{4, 10, 12, 17, 20, 21},
		{1, 3, 7, 20, 21},
		{0, 2, 13, 15, 18, 19}, // 5
		{0, 14, 15},
		{4, 8, 21},
		{2, 7, 10, 16, 21},
		{2, 10, 11, 13, 14, 16, 17, 18},
		{3, 8, 9, 16, 17, 21}, // 10
		{9, 12, 14, 17},
		{1, 3, 11, 17, 20},
		{5, 9, 14, 15, 18},
		{6, 9, 11, 13, 15},
		{0, 5, 6, 13, 14, 19}, // 15
		{2, 8, 9, 10},
		{3, 9, 10, 11, 12},
		{2, 5, 9, 13},
		{0, 5, 15},
		{1, 3, 4, 12}, // 20
		{3, 4, 7, 8, 10}
	};

	air_graph = new Graph(air_path, links);

	high_ground_index = 8;


	warp_prism_locations = { convert_location(Point2D(50, 60), swap),
							convert_location(Point2D(76, 86), swap),
							convert_location(Point2D(62, 131), swap),
							convert_location(Point2D(76, 86), swap) };

	initial_scout_pos = convert_location(Point2D(44, 114), swap);

	main_scout_path = { convert_location(Point2D(39, 121), swap),
						convert_location(Point2D(31, 126), swap),
						convert_location(Point2D(34, 141), swap),
						convert_location(Point2D(50, 141), swap),
						convert_location(Point2D(57, 134), swap),
						convert_location(Point2D(52, 122), swap), };

	natural_scout_path = { convert_location(Point2D(31, 110), swap),
							convert_location(Point2D(29, 97), swap),
							convert_location(Point2D(42, 96), swap), };

	enemy_natural = convert_location(Point2D(38, 102), swap);

	possible_3rds = { convert_location(Point2D(36.5, 69.5), swap),
					convert_location(Point2D(64.5, 110.5), swap), };
}

}


//old lightshade
/*
auto convert_location = [](Point2D point, bool swap)
		{
			if (swap)
				return Point2D(184, 164) - point;
			else
				return point;
		};

		bool swap = Observation()->GetStartLocation().x == 40.5 && Observation()->GetStartLocation().y == 131.5;

		std::vector<Point2D> nexi = { convert_location(Point2D(143.5, 32.5), swap),
									convert_location(Point2D(145.5, 61.5), swap),
									convert_location(Point2D(147.5, 94.5), swap),
									convert_location(Point2D(119.5, 53.5), swap),
									convert_location(Point2D(104.5, 27.5), swap),
									convert_location(Point2D(148.5, 125.5), swap),
									convert_location(Point2D(124.5, 120.5), swap) };

		std::vector<Point2D> pylons = { convert_location(Point2D(142, 63), swap),
										convert_location(Point2D(140, 36), swap),
										convert_location(Point2D(132, 36), swap),
										convert_location(Point2D(154, 64), swap),
										convert_location(Point2D(149, 26), swap),
										convert_location(Point2D(140, 34), swap),
										convert_location(Point2D(140, 38), swap),
										convert_location(Point2D(143, 95), swap),
										convert_location(Point2D(136, 94), swap) };

		std::vector<Point2D> gateways = { convert_location(Point2D(141.5, 68.5), swap),
										convert_location(Point2D(139.5, 64.5), swap),
										convert_location(Point2D(137.5, 34.5), swap),
										convert_location(Point2D(137.5, 37.5), swap),
										convert_location(Point2D(134.5, 34.5), swap),
										convert_location(Point2D(134.5, 37.5), swap),
										convert_location(Point2D(143.5, 100.5), swap),
										convert_location(Point2D(140.5, 98.5), swap),
										convert_location(Point2D(137.5, 96.5), swap),
										convert_location(Point2D(134.5, 40.5), swap),
										convert_location(Point2D(137.5, 40.5), swap),
										convert_location(Point2D(137.5, 31.5), swap),
										convert_location(Point2D(134.5, 31.5), swap),
										convert_location(Point2D(131.5, 33.5), swap),
										convert_location(Point2D(131.5, 30.5), swap) };

		std::vector<Point2D> assimilators = { convert_location(Point2D(150.5, 35.5), swap),
											convert_location(Point2D(139.5, 25.5), swap),
											convert_location(Point2D(152.5, 58.5), swap),
											convert_location(Point2D(148.5, 54.5), swap) };

		std::vector<Point2D> cybernetics_core = { convert_location(Point2D(136.5, 62.5), swap) };
		std::vector<Point2D> twilight = { convert_location(Point2D(142.5, 36.5), swap) };
		std::vector<Point2D> robo = { convert_location(Point2D(145.5, 36.5), swap)};
		std::vector<Point2D> stargate = { convert_location(Point2D(142.5, 39.5), swap) };

		std::vector<Point2D> proxy_pylons = { convert_location(Point2D(33.0, 69.0), swap),
											convert_location(Point2D(74.0, 117.0), swap) };

		std::vector<Point2D> proxy_gateways = { convert_location(Point2D(0, 0), swap) };

		std::vector<Point2D> proxy_robos = { convert_location(Point2D(37.5, 69.5), swap),
											convert_location(Point2D(117.5, 121.5), swap) };

		std::vector<Point2D> attack_path = { convert_location(Point2D(74, 35), swap),
											convert_location(Point2D(50, 60), swap),
											convert_location(Point2D(44, 69), swap),
											convert_location(Point2D(44, 76), swap),
											convert_location(Point2D(48, 82), swap),
											convert_location(Point2D(52, 89), swap),
											convert_location(Point2D(49, 97), swap),
											convert_location(Point2D(44, 102), swap),
											convert_location(Point2D(41, 111), swap),
											convert_location(Point2D(44, 114), swap),
											convert_location(Point2D(43, 124), swap) };

		std::vector<Point2D> air_path = {
								convert_location(Point2D(26, 17), swap),
								convert_location(Point2D(102, 147), swap),
								convert_location(Point2D(24, 85), swap),
								convert_location(Point2D(58, 126), swap),
								convert_location(Point2D(63, 148), swap),
								convert_location(Point2D(24, 53), swap),
								convert_location(Point2D(77, 16), swap),
								convert_location(Point2D(24, 148), swap),
								convert_location(Point2D(24, 115), swap),
								convert_location(Point2D(59, 86), swap),
								convert_location(Point2D(48, 112), swap),
								convert_location(Point2D(83, 96), swap),
								convert_location(Point2D(91, 118), swap),
								convert_location(Point2D(49, 58), swap),
								convert_location(Point2D(78, 49), swap),
								convert_location(Point2D(57, 41), swap),
								convert_location(Point2D(35, 101), swap),
								convert_location(Point2D(62, 112), swap),
								convert_location(Point2D(32, 69), swap),
								convert_location(Point2D(34, 39), swap),
								convert_location(Point2D(77, 138), swap),
								convert_location(Point2D(37, 133), swap) };

		std::vector<std::vector<int>> links = {
			{5, 6, 19},
			{4, 12, 20},
			{5, 8, 9, 16, 18},
			{4, 10, 12, 17, 20, 21},
			{1, 3, 7, 20, 21},
			{0, 2, 13, 15, 18, 19}, // 5
			{0, 14, 15},
			{4, 8, 21},
			{2, 7, 10, 16, 21},
			{2, 10, 11, 13, 14, 16, 17, 18},
			{3, 8, 9, 16, 17, 21}, // 10
			{9, 12, 14, 17},
			{1, 3, 11, 17, 20},
			{5, 9, 14, 15, 18},
			{6, 9, 11, 13, 15},
			{0, 5, 6, 13, 14, 19}, // 15
			{2, 8, 9, 10},
			{3, 9, 10, 11, 12},
			{2, 5, 9, 13},
			{0, 5, 15},
			{1, 3, 4, 12}, // 20
			{3, 4, 7, 8, 10}
		};

		Graph* air_graph = new Graph(air_path, links);

		int high_ground_index = 8;

		locations = Locations(nexi, pylons, gateways, cybernetics_core, twilight, robo, stargate, assimilators,
			proxy_pylons, proxy_gateways, proxy_robos, attack_path, high_ground_index, air_graph);
	}*/