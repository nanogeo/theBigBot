#pragma once
#include "sc2api/sc2_common.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_client.h"

#include <map>

namespace sc2
{

class BlankBot : public sc2::Agent {
public:
	BlankBot() : Agent() {};
};

class ZerglingFloodBot : public sc2::Agent {
public:
	ZerglingFloodBot() : Agent() {};
	std::map<const Unit*, bool> attacks;

	virtual void OnGameStart()
	{
		//Debug()->DebugGiveAllUpgrades();
		//Debug()->DebugFastBuild();
		//Debug()->DebugGiveAllResources();
		Debug()->SendDebug();
	}

	virtual void OnStep()
	{
		if (Observation()->GetGameLoop() >= 4700 && Observation()->GetGameLoop() % 700 == 500)
		{
			Debug()->DebugCreateUnit(ZERGLING, Observation()->GetStartLocation(), 1, 10);
			Debug()->DebugGiveAllUpgrades();
			Debug()->SendDebug();
		}
		Actions()->UnitCommand(Observation()->GetUnits(IsUnit(ZERGLING)), ABILITY_ID::ATTACK, Observation()->GetGameInfo().enemy_start_locations[0]);

	}
};

class CannonRushBot : public sc2::Agent {
public:
	CannonRushBot() : Agent() {};
	const Unit* probe = nullptr;


	virtual void OnGameStart()
	{
		//Debug()->DebugGiveAllUpgrades();
		//Debug()->DebugFastBuild();
		//Debug()->DebugGiveAllResources();
		Debug()->SendDebug();
	}

	virtual void OnStep()
	{

		// abyssal reef

		Point2D cpylon = Point2D(42, 121);
		Point2D cforge = Point2D(42.5, 118.5);
		Point2D cpylon2 = Point2D(158, 35);
		std::vector<Point2D> ccannons = { Point2D(161, 35), Point2D(155, 31), Point2D(159, 30) };
		if (Observation()->GetStartLocation().x == 161.5 && Observation()->GetStartLocation().y == 21.5)
		{
			cpylon = Point2D(158, 23);
			cforge = Point2D(157.5, 25.5);
			cpylon2 = Point2D(44, 109);
			ccannons = { Point2D(41, 109), Point2D(47, 113), Point2D(43, 114) };
		}

		if (probe == nullptr && Observation()->GetUnits(IsUnit(PROBE)).size() > 0)
			probe = Observation()->GetUnits(IsUnit(PROBE))[0];

		if (Observation()->GetUnits(IsUnit(FORGE)).size() == 0 && Observation()->GetUnits(IsUnit(PYLON)).size() == 1)
		{
			const Unit* p = Utility::ClosestTo(Observation()->GetUnits(IsUnit(PROBE)), cforge);
			if (Distance2D(p->pos, cforge) > 1)
				Actions()->UnitCommand(p, ABILITY_ID::MOVE_MOVE, cforge);
			else
				Actions()->UnitCommand(p, ABILITY_ID::BUILD_FORGE, cforge);
		}

		if (Observation()->GetUnits(IsUnit(PYLON)).size() == 0)
		{
			if (Distance2D(probe->pos, cpylon) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, cpylon);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, cpylon);
		}
		else if (Observation()->GetUnits(Unit::Alliance::Self, IsUnit(PYLON)).size() == 1)
		{
			if (Distance2D(probe->pos, cpylon2) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, cpylon2);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, cpylon2);
		}
		else if (Observation()->GetUnits(IsUnit(CANNON)).size() < 3)
		{
			Point2D cannon_pos = ccannons[Observation()->GetUnits(IsUnit(CANNON)).size()];

			if (Distance2D(probe->pos, cannon_pos) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, cannon_pos);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_PHOTONCANNON, cannon_pos);
		}
	}

};

class ZealotRushBot : public sc2::Agent {
public:
	ZealotRushBot() : Agent() {};
	const Unit* probe = nullptr;


	virtual void OnGameStart()
	{
		//Debug()->DebugGiveAllUpgrades();
		//Debug()->DebugFastBuild();
		//Debug()->DebugGiveAllResources();
		Debug()->SendDebug();
	}

	virtual void OnStep()
	{
		if (Observation()->GetUnits(IsUnit(NEXUS)).size() > 0 &&
			Observation()->GetUnits(IsUnit(NEXUS))[0]->orders.size() == 0 &&
			Observation()->GetUnits(IsUnit(PROBE)).size() < 13)
			Actions()->UnitCommand(Observation()->GetUnits(IsUnit(NEXUS))[0], ABILITY_ID::TRAIN_PROBE);

		Point2D rally, pylon, pylon2, swap, start;
		std::vector<Point2D> gates;

		std::string map_name = Observation()->GetGameInfo().map_name;

		if (map_name == "Incorporeal AIE") // 2025 seaon 2 maps
		{

		}
		else if (map_name == "Last Fantasy AIE")
		{

		}
		else if (map_name == "Ley Lines AIE")
		{

		}
		else if (map_name == "Magannatha AIE")
		{

		}
		else if (map_name == "Persephone AIE")
		{
			rally = Point2D(56, 142);
			pylon = Point2D(63, 121);
			start = Point2D(37.5, 1454.5);
			swap = Point2D(0, 0);
			gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
			if (Observation()->GetStartLocation().x == start.x && Observation()->GetStartLocation().y == start.y)
			{
				rally = Point2D(rally.x, 180 - rally.y);
				pylon = Point2D(pylon.x, 180 - pylon.y);
				pylon2 = Point2D(pylon2.x, 180 - pylon2.y);
				for (int i = 0; i < 4; i++)
				{
					gates[i] = Point2D(gates[i].x, 180 - gates[i].y);
				}
			}
		}
		else if (map_name == "Pylon AIE")
		{
			rally = Point2D(88, 166);
			pylon = Point2D(95, 142);
			start = Point2D(72.5, 171.5);
			swap = Point2D(248, 248);
			gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
			if (Observation()->GetStartLocation().x == start.x && Observation()->GetStartLocation().y == start.y)
			{
				rally = swap - rally;
				pylon = swap - pylon;
				pylon2 = swap - pylon2;
				for (int i = 0; i < 4; i++)
				{
					gates[i] = swap - gates[i];
				}
			}
		}
		else if (map_name == "Torches AIE")
		{
			rally = Point2D(110, 152);
			pylon = Point2D(88, 132);
			start = Point2D(124.5, 159.5);
			swap = Point2D(0, 0);
			gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
			if (Observation()->GetStartLocation().x == start.x && Observation()->GetStartLocation().y == start.y)
			{
				rally = Point2D(rally.x, 208 - rally.y);
				pylon = Point2D(pylon.x, 208 - pylon.y);
				pylon2 = Point2D(pylon2.x, 208 - pylon2.y);
				for (int i = 0; i < 4; i++)
				{
					gates[i] = Point2D(gates[i].x, 208 - gates[i].y);
				}
			}
		}
		else if (map_name == "Ultralove AIE")
		{

		}

		pylon2 = pylon + Point2D(2, 3);



		// abyssal reef
		/*rally = Point2D(56, 118);
		pylon = Point2D(77, 99);
		pylon2 = pylon + Point2D(2, 3);
		gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		swap = Point2D(200, 144);
		if (Observation()->GetStartLocation().x == 38.5 && Observation()->GetStartLocation().y == 122.5)
		{*/
		// acropolis
		/*Point2D rally = Point2D(136, 50);
		Point2D pylon = Point2D(113, 65);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(176, 172);
		if (Observation()->GetStartLocation().x == 142.5 && Observation()->GetStartLocation().y == 33.5)
		{*/
		// automation
		/*Point2D rally = Point2D(34, 50);
		Point2D pylon = Point2D(62, 65);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(184, 180);
		if (Observation()->GetStartLocation().x == 29.5 && Observation()->GetStartLocation().y == 65.5)
		{*/
		// ephemeron
		/*Point2D rally = Point2D(34, 122);
		Point2D pylon = Point2D(60, 99);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(160, 160);
		if (Observation()->GetStartLocation().x == 29.5 && Observation()->GetStartLocation().y == 138.5)
		{*/
		// interloper
		/*Point2D rally = Point2D(34, 122);
		Point2D pylon = Point2D(50, 117);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(152, 168);
		if (Observation()->GetStartLocation().x == 26.5 && Observation()->GetStartLocation().y == 137.5)
		{*/
		// thunderbird
		/*Point2D rally = Point2D(40, 118);
		Point2D pylon = Point2D(70, 95);
		Point2D pylon2 = pylon + Point2D(2, 3);
		std::vector<Point2D> gates = { pylon + Point2D(2.5, 0.5), pylon + Point2D(-0.5, 2.5), pylon + Point2D(-2.5, -0.5), pylon + Point2D(0.5, -2.5) };
		Point2D swap = Point2D(192, 156);
		if (Observation()->GetStartLocation().x == 38.5 && Observation()->GetStartLocation().y == 133.5)
		{*/
		/*	rally = swap - rally;
			pylon = swap - pylon;
			pylon2 = swap - pylon2;
			for (int i = 0; i < 4; i++)
			{
				gates[i] = swap - gates[i];
			}

		}*/


		if (probe == nullptr && Observation()->GetUnits(IsUnit(PROBE)).size() > 0)
			probe = Observation()->GetUnits(IsUnit(PROBE))[0];

		if (Observation()->GetUnits(IsUnit(NEXUS)).size() > 0 &&
			Observation()->GetUnits(IsUnit(NEXUS))[0]->orders.size() == 0 &&
			Observation()->GetUnits(IsUnit(PROBE)).size() < 14)
			Actions()->UnitCommand(Observation()->GetUnits(IsUnit(NEXUS))[0], ABILITY_ID::TRAIN_PROBE);

		if (Observation()->GetUnits(IsUnit(PYLON)).size() == 0)
		{
			if (Distance2D(probe->pos, pylon) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, pylon);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon);
		}
		else if (Observation()->GetUnits(IsUnit(GATEWAY)).size() < 4)
		{
			Point2D gate_pos = gates[Observation()->GetUnits(IsUnit(GATEWAY)).size()];

			if (Distance2D(probe->pos, gate_pos) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, gate_pos);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_GATEWAY, gate_pos);
		}
		else if (Observation()->GetUnits(IsUnit(PYLON)).size() < 2)
		{
			if (Distance2D(probe->pos, pylon2) > 1)
				Actions()->UnitCommand(probe, ABILITY_ID::MOVE_MOVE, pylon2);
			else
				Actions()->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon2);
		}

		if (Observation()->GetGameLoop() % 5 == 0)
			return;

		for (const auto& gate : Observation()->GetUnits(IsUnit(GATEWAY)))
		{
			if (gate->build_progress < 1)
				Actions()->UnitCommand(gate, ABILITY_ID::SMART, rally);

			if (gate->build_progress == 1 && gate->orders.size() == 0)
				Actions()->UnitCommand(gate, ABILITY_ID::TRAIN_ZEALOT);
		}
		for (const auto& nexus : Observation()->GetUnits(IsUnit(NEXUS)))
		{
			if (nexus->energy > 50)
			{
				bool ignore = false;
				for (const auto& gate : Observation()->GetUnits(IsUnit(GATEWAY)))
				{
					for (const auto& buff : gate->buffs)
					{
						if (buff == BUFF_ID::CHRONOBOOSTENERGYCOST)
							ignore = true;
					}
					if (ignore == false)
					{
						Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, gate);
						break;
					}
				}
			}
		}
	}
};


class Proxy4RaxBot : public sc2::Agent {
public:
	Proxy4RaxBot() : Agent() {};
	Units scvs;


	virtual void OnStep()
	{
		// abyssal reef

		Point2D rally, rally2, raxmid, depot1, depot2, swap, start;
		std::vector<Point2D> rax;

		std::string map_name = Observation()->GetGameInfo().map_name;

		if (map_name == "Abyssal Reef AIE")
		{
			rally = Point2D(70, 118);
			rally2 = Point2D(38.5, 122);
			raxmid = Point2D(77, 99);
			depot1 = Point2D(155, 20);
			depot2 = Point2D(74, 96);
			rax = { raxmid + Point2D(3.5, 0.5), raxmid + Point2D(-1.5, 2.5), raxmid + Point2D(-3.5, -0.5), raxmid + Point2D(1.5, -2.5) };
			swap = Point2D(200, 144);
			start = Point2D(38.5, 122.5);
		}
		else if (map_name == "Pylon AIE") // 2025 seaon 2 maps
		{
			rally = Point2D(97, 175);
			rally2 = Point2D(69, 174);
			raxmid = Point2D(96, 138);
			depot1 = Point2D(174, 84);
			depot2 = Point2D(96, 134);
			rax = { raxmid + Point2D(3.5, 0.5), raxmid + Point2D(-1.5, 2.5), raxmid + Point2D(-3.5, -0.5), raxmid + Point2D(1.5, -2.5) };
			swap = Point2D(248, 248);
			start = Point2D(72.5, 171.5);
		}

		if (Observation()->GetStartLocation().x == start.x && Observation()->GetStartLocation().y == start.y)
		{
			rally = swap - rally;
			rally2 = swap - rally2;
			depot1 = swap - depot1;
			depot2 = swap - depot2;
			for (int i = 0; i < 4; i++)
			{
				rax[i] = swap - rax[i];
			}

		}


		if (scvs.size() == 0 && Observation()->GetUnits(IsUnit(SCV)).size() > 0)
		{
			for (const auto& scv : Observation()->GetUnits(IsUnit(SCV)))
			{
				scvs.push_back(scv);
				if (scvs.size() == 5)
					break;
			}
		}

		if (Observation()->GetGameLoop() < 10 &&
			Observation()->GetUnits(IsUnit(COMMAND_CENTER))[0]->orders.size() == 0)
			Actions()->UnitCommand(Observation()->GetUnits(IsUnit(COMMAND_CENTER))[0], ABILITY_ID::TRAIN_SCV);

		if (Observation()->GetUnits(IsUnit(SUPPLY_DEPOT)).size() == 0 && Observation()->GetGameLoop() > 250)
		{
			if (Distance2D(scvs[0]->pos, depot1) > 1)
				Actions()->UnitCommand(scvs[0], ABILITY_ID::MOVE_MOVE, depot1);
			else
				Actions()->UnitCommand(scvs[0], ABILITY_ID::BUILD_SUPPLYDEPOT, depot1);
		}
		else if (Observation()->GetUnits(IsUnit(SUPPLY_DEPOT)).size() == 1 && Observation()->GetGameLoop() > 250)
		{
			if (scvs[0] && scvs[0]->orders.size() == 0)
			{
				Actions()->UnitCommand(scvs[0], ABILITY_ID::SMART, Utility::ClosestTo(Observation()->GetUnits(IsUnits(MINERAL_PATCH)), scvs[0]->pos));
			}
		}

		if (Observation()->GetGameLoop() > 100 && Observation()->GetUnits(IsUnit(BARRACKS)).size() < 1)
		{
			Point2D pos = rax[0];

			if (Distance2D(scvs[1]->pos, pos) > 1)
				Actions()->UnitCommand(scvs[1], ABILITY_ID::MOVE_MOVE, pos);
			else
				Actions()->UnitCommand(scvs[1], ABILITY_ID::BUILD_BARRACKS, pos);
		}
		else if (Observation()->GetGameLoop() > 1600 && Observation()->GetUnits(IsUnit(BARRACKS)).size() < 4)
		{
			Point2D pos = rax[3];

			if (Distance2D(scvs[1]->pos, pos) > 1)
				Actions()->UnitCommand(scvs[1], ABILITY_ID::MOVE_MOVE, pos);
			else
				Actions()->UnitCommand(scvs[1], ABILITY_ID::BUILD_BARRACKS, pos);
		}

		if (Observation()->GetGameLoop() > 300 && Observation()->GetUnits(IsUnit(BARRACKS)).size() < 2)
		{
			Point2D pos = rax[1];

			if (Distance2D(scvs[2]->pos, pos) > 1)
				Actions()->UnitCommand(scvs[2], ABILITY_ID::MOVE_MOVE, pos);
			else
				Actions()->UnitCommand(scvs[2], ABILITY_ID::BUILD_BARRACKS, pos);
		}
		else if (Observation()->GetGameLoop() > 1900 && Observation()->GetUnits(IsUnit(SUPPLY_DEPOT)).size() < 2)
		{
			if (Distance2D(scvs[2]->pos, depot2) > 1)
				Actions()->UnitCommand(scvs[2], ABILITY_ID::MOVE_MOVE, depot2);
			else
				Actions()->UnitCommand(scvs[2], ABILITY_ID::BUILD_SUPPLYDEPOT, depot2);
		}

		if (Observation()->GetGameLoop() > 560 && Observation()->GetUnits(IsUnit(BARRACKS)).size() < 3)
		{
			Point2D pos = rax[2];

			if (Distance2D(scvs[3]->pos, pos) > 1)
				Actions()->UnitCommand(scvs[3], ABILITY_ID::MOVE_MOVE, pos);
			else
				Actions()->UnitCommand(scvs[3], ABILITY_ID::BUILD_BARRACKS, pos);
		}


		if (Observation()->GetGameLoop() % 5 == 0)
			return;

		for (const auto& gate : Observation()->GetUnits(IsUnit(BARRACKS)))
		{
			if (gate->build_progress == 1 && gate->orders.size() == 0)
				Actions()->UnitCommand(gate, ABILITY_ID::TRAIN_MARINE);
		}
		for (const auto& marine : Observation()->GetUnits(IsUnit(MARINE)))
		{
			if (marine->orders.size() == 0)
			{
				Actions()->UnitCommand(marine, ABILITY_ID::ATTACK, rally);
				Actions()->UnitCommand(marine, ABILITY_ID::ATTACK, rally2, true);
			}
		}
	}
};

class TwelvePoolBot : public sc2::Agent {
public:
	TwelvePoolBot() : Agent() {};
	const Unit* drone = nullptr;
	bool made_pool = false;

	virtual void OnStep()
	{
		if (!made_pool && Observation()->GetUnits(IsUnit(SPAWNING_POOL)).size() == 0 && Observation()->GetMinerals() >= 200)
		{
			drone = Observation()->GetUnits(IsUnit(DRONE))[0];
			Point2D point1 = drone->pos;
			Point2D point2 = Observation()->GetStartLocation();
			float dist = 10;
			float total_dist = Distance2D(point1, point2);
			Actions()->UnitCommand(drone, ABILITY_ID::BUILD_SPAWNINGPOOL, Point2D(point1.x + dist * (point2.x - point1.x) / total_dist, point1.y + dist * (point2.y - point1.y) / total_dist));
		}
		if (Observation()->GetUnits(IsUnit(SPAWNING_POOL)).size() == 0)
			return;
		for (const auto& larva : Observation()->GetUnits(IsUnit(LARVA)))
		{
			if (Observation()->GetFoodCap() - Observation()->GetFoodUsed() <= 2 || Observation()->GetUnits(IsUnit(OVERLORD)).size() == 1)
			{
				bool overlord_morphing = false;
				for (const auto& egg : Observation()->GetUnits(IsUnit(EGG)))
				{
					if (egg->orders.size() > 0 && egg->orders[0].ability_id == ABILITY_ID::TRAIN_OVERLORD)
					{
						overlord_morphing = true;
						break;
					}
				}
				if (!overlord_morphing)
					Actions()->UnitCommand(larva, ABILITY_ID::TRAIN_OVERLORD);
			}
			Actions()->UnitCommand(larva, ABILITY_ID::TRAIN_ZERGLING);
		}
		for (const auto& zergling: Observation()->GetUnits(IsUnit(ZERGLING)))
		{
			Actions()->UnitCommand(zergling, ABILITY_ID::ATTACK, Observation()->GetGameInfo().enemy_start_locations[0]);
		}
		
	}
};

}