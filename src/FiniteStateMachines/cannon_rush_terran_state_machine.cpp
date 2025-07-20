

#include "cannon_rush_terran_state_machine.h"
#include "theBigBot.h"


namespace sc2 {


#pragma region CannonRushTerranMoveAcross

void CannonRushTerranMoveAcross::TickState()
{
	mediator->SetUnitCommand(probe, A_MOVE, mediator->GetLocations().enemy_natural, CommandPriorty::low);
}

void CannonRushTerranMoveAcross::EnterState()
{
	return;
}

void CannonRushTerranMoveAcross::ExitState()
{
	return;
}

State* CannonRushTerranMoveAcross::TestTransitions()
{
	if (Distance2D(probe->pos, mediator->GetLocations().enemy_natural) < 15)
		return new CannonRushTerranFindAvaibleCorner(mediator, state_machine, probe, 0);
	return nullptr;
}

std::string CannonRushTerranMoveAcross::toString() const
{
	return "move across 1";
}

#pragma endregion

#pragma region CannonRushTerranFindAvaibleCorner

void CannonRushTerranFindAvaibleCorner::TickState()
{
	Point2D pos = mediator->GetLocations().cannon_rush_terran_positions[curr_index].initial_pylon;
	mediator->SetUnitCommand(probe, A_MOVE, pos, CommandPriorty::low);

	if (Distance2D(probe->pos, pos) < 6)
	{
		if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), pos) < 6)
		{
			curr_index++;
			if (curr_index >= mediator->GetLocations().cannon_rush_terran_positions.size())
				curr_index = 0;
		}
		else if (Distance2D(probe->pos, pos) < 1 && mediator->CanAfford(PYLON, 1))
		{
			mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, pos, CommandPriorty::low);
		}
	}
}

void CannonRushTerranFindAvaibleCorner::EnterState()
{
	return;
}

void CannonRushTerranFindAvaibleCorner::ExitState()
{
	return;
}

State* CannonRushTerranFindAvaibleCorner::TestTransitions()
{
	Point2D pos = mediator->GetLocations().cannon_rush_terran_positions[curr_index].initial_pylon;
	if (Utility::DistanceToClosest(mediator->GetUnits(IsFriendlyUnit(PYLON)), pos) < 1)
		return new CannonRushTerranScout(mediator, state_machine, probe, 0, mediator->GetLocations().main_scout_path, false);
	return nullptr;
}

std::string CannonRushTerranFindAvaibleCorner::toString() const
{
	return "find available corner";
}

#pragma endregion

#pragma region CannonRushTerranScout

void CannonRushTerranScout::TickState()
{
	if (Distance2D(probe->pos, current_target) < 3)
	{
		index++;
		if (index < main_scout_path.size())
			current_target = main_scout_path[index];
		else
			index = 0;
	}
	mediator->SetUnitCommand(probe, A_MOVE, current_target, CommandPriorty::low);
}

void CannonRushTerranScout::EnterState()
{
	current_target = main_scout_path[index];
}

void CannonRushTerranScout::ExitState()
{
	return;
}

State* CannonRushTerranScout::TestTransitions()
{
	if (mediator->GetUnits(IsFriendlyUnit(CANNON)).size() == 0)
	{
		Units pylons = mediator->GetUnits(IsFriendlyUnit(PYLON));
		Units enemies = mediator->GetUnits(Unit::Alliance::Enemy);
		bool free_pylon = false;
		for (const auto& position : mediator->GetLocations().cannon_rush_terran_positions)
		{
			if (Utility::DistanceToClosest(pylons, position.initial_pylon) < 1 && Utility::DistanceToClosest(enemies, position.initial_pylon) > 3)
			{
				free_pylon = true;
				break;
			}

		}
		if (free_pylon == false)
			return new CannonRushTerranFindAvaibleCorner(mediator, state_machine, probe, (int)pylons.size() - 1);
	}

	if (mediator->GetUnits(IsFriendlyUnit(GATEWAY)).size() == 0)
		return nullptr;

	const Unit* closest_gas = Utility::ClosestTo(mediator->GetUnits(IsGeyser()), probe->pos);
	if (Distance2D(closest_gas->pos, probe->pos) < 3)
	{
		if (gas_stolen == false && mediator->CanAfford(ASSIMILATOR, 1) && Utility::DistanceToClosest(mediator->GetUnits(IsFriendlyUnit(ASSIMILATOR)), closest_gas->pos) > 1 &&
			Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), closest_gas->pos) > 1.5)
			return new CannonRushTerranGasSteal(mediator, state_machine, probe, index, closest_gas);
	}
	return nullptr;
}

std::string CannonRushTerranScout::toString() const
{
	return "scout";
}

#pragma endregion

#pragma region CannonRushTerranGasSteal

void CannonRushTerranGasSteal::TickState()
{
	//agent->Debug()->DebugSphereOut(gas->pos, 1.5, Color(255, 0, 0));
	if (Distance2D(probe->pos, gas->pos) < 1.75)
		mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_ASSIMILATOR, gas, CommandPriorty::low);
}

void CannonRushTerranGasSteal::EnterState()
{
	mediator->SetUnitCommand(probe, A_MOVE, gas, CommandPriorty::low);
}

void CannonRushTerranGasSteal::ExitState()
{
	return;
}

State* CannonRushTerranGasSteal::TestTransitions()
{
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), gas->pos) < 1.5)
		return new CannonRushTerranScout(mediator, state_machine, probe, scouting_index, mediator->GetLocations().main_scout_path, false);

	if (mediator->GetUnits(IsFriendlyUnit(ASSIMILATOR)).size() == 0)
		return nullptr;

	const Unit* closest_gas = Utility::ClosestUnitTo(mediator->GetUnits(IsFriendlyUnit(ASSIMILATOR)), gas->pos);
	if (closest_gas->display_type != Unit::DisplayType::Placeholder && Distance2D(gas->pos, closest_gas->pos) < 1)
		return new CannonRushTerranScout(mediator, state_machine, probe, scouting_index, mediator->GetLocations().main_scout_path, true);

	return nullptr;
}

std::string CannonRushTerranGasSteal::toString() const
{
	return "steal gas";
}

#pragma endregion

#pragma region CannonRushTerranMoveAcross2

void CannonRushTerranMoveAcross2::TickState()
{
	mediator->SetUnitCommand(probe, A_MOVE, mediator->GetLocations().enemy_natural, CommandPriorty::low);
}

void CannonRushTerranMoveAcross2::EnterState()
{
	for (const auto& unit : mediator->GetUnits(IsFriendlyUnit(PYLON)))
	{
		if (Distance2D(unit->pos, mediator->GetLocations().start_location) > 30)
			state_machine->pylons.push_back(unit);
	}
	return;
}

void CannonRushTerranMoveAcross2::ExitState()
{
	return;
}

State* CannonRushTerranMoveAcross2::TestTransitions()
{
	if (Distance2D(probe->pos, mediator->GetLocations().enemy_natural) < 15)
		return new CannonRushTerranFindWallOffSpot(mediator, state_machine, probe, 0);
	return nullptr;
}

std::string CannonRushTerranMoveAcross2::toString() const
{
	return "move across 2";
}

#pragma endregion

#pragma region CannonRushTerranFindWallOffSpot

void CannonRushTerranFindWallOffSpot::TickState()
{
	CannonRushPosition pos = mediator->GetLocations().cannon_rush_terran_positions[index];
	mediator->SetUnitCommand(probe, A_MOVE, pos.initial_pylon, CommandPriorty::low);
	if (Utility::DistanceToClosest(mediator->GetUnits(IsFriendlyUnit(PYLON)), pos.initial_pylon) > 1)
	{
		index++;
		if (index >= mediator->GetLocations().cannon_rush_terran_positions.size())
			index = 0; // TODO maybe place another initial pylon or find a triple wall off instead?
	}


}

void CannonRushTerranFindWallOffSpot::EnterState()
{
	return;
}

void CannonRushTerranFindWallOffSpot::ExitState()
{
	return;
}

State* CannonRushTerranFindWallOffSpot::TestTransitions()
{
	CannonRushPosition pos = mediator->GetLocations().cannon_rush_terran_positions[index];
	if (Distance2D(pos.initial_pylon, probe->pos) < 6)
	{
		Units enemy_units = mediator->GetUnits(Unit::Alliance::Enemy);

		for (int i = 0; i < pos.cannon_position.size(); i++)
		{
			if (Utility::DistanceToClosest(enemy_units, pos.cannon_position[i].cannon_pos) > 8 && mediator->GetCurrentResources().mineral_cost > 200)
			{
				return new CannonRushTerranCannonFirstWallOff(mediator, state_machine, probe, pos.cannon_position[i].cannon_pos, pos.pylon_walloff_positions[i], pos.gateway_walloff_positions[i]);
			}

			if (Utility::DistanceToClosest(enemy_units, pos.cannon_position[i].cannon_pos) < 1.5)
				break;
			bool gate_wall_blocked = false;
			for (int j = 0; j < pos.gateway_walloff_positions[i].size(); j++)
			{
				Point2D building_pos = pos.gateway_walloff_positions[i][j].building_pos;
				if (floor(building_pos.x) == building_pos.x) // TODO this is a bad way of seeing if its a a pylon
				{
					// its a pylon
					if (Utility::DistanceToClosest(enemy_units, building_pos) < 1.5 + j)
					{
						gate_wall_blocked = true;
						break;
					}
				}
				else
				{
					// its a gate
					if (Utility::DistanceToClosest(enemy_units, building_pos) < 2.5 + j)
					{
						gate_wall_blocked = true;
						break;
					}
				}
			}
			if (!gate_wall_blocked)
				return new CannonRushTerranWallOff(mediator, state_machine, probe, pos.cannon_position[i].cannon_pos, pos.cannon_position[i].with_gate_walloff, pos.gateway_walloff_positions[i]);

			bool pylon_wall_blocked = false;
			for (int j = 0; j < pos.pylon_walloff_positions[i].size(); j++)
			{
				Point2D building_pos = pos.pylon_walloff_positions[i][j].building_pos;
				if (Utility::DistanceToClosest(enemy_units, building_pos) < 1.5 + j)
				{
					gate_wall_blocked = true;
					break;
				}
			}
			if (!pylon_wall_blocked)
				return new CannonRushTerranWallOff(mediator, state_machine, probe, pos.cannon_position[i].cannon_pos, pos.cannon_position[i].with_pylon_wallof, pos.pylon_walloff_positions[i]);
		}
		index++;
		if (index >= mediator->GetLocations().cannon_rush_terran_positions.size())
			index = 0; // TODO maybe place another initial pylon or find a triple wall off instead?
	}
	return nullptr;
}

std::string CannonRushTerranFindWallOffSpot::toString() const
{
	return "find walloff spot";
}

#pragma endregion

#pragma region CannonRushTerranWallOff

void CannonRushTerranWallOff::TickState()
{
	Point2D building_pos;
	Point2D move_to_pos;
	if (index == wall_pos.size())
	{
		building_pos = cannon_pos;
		move_to_pos = cannon_move_to;
		mediator->SetUnitCommand(probe, A_MOVE, move_to_pos, CommandPriorty::low);
		if (mediator->CanAfford(CANNON, 1) && Distance2D(probe->pos, move_to_pos) < .25)
			mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PHOTONCANNON, building_pos, CommandPriorty::low);
	}
	else
	{
		BuildingPlacement pos = wall_pos[index];
		building_pos = pos.building_pos;
		move_to_pos = pos.move_to_pos;
		float dist = Distance2D(probe->pos, move_to_pos);
		mediator->SetUnitCommand(probe, A_MOVE, Utility::PointBetween(probe->pos, move_to_pos, dist + .5f), CommandPriorty::low);

		if (pos.type == PYLON)
		{
			if (mediator->CanAfford(PYLON, 1) && Distance2D(probe->pos, move_to_pos) < .1)
				mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, building_pos, CommandPriorty::low);
		}
		else
		{

			if (mediator->CanAfford(GATEWAY, 1) && Distance2D(probe->pos, move_to_pos) < .1)
				mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_GATEWAY, building_pos, CommandPriorty::low);
		}
	}
	const Unit* closest_building = Utility::ClosestUnitTo(mediator->GetUnits(IsUnits({ CANNON, PYLON, GATEWAY })), building_pos);
	if (closest_building->display_type != Unit::DisplayType::Placeholder && Distance2D(building_pos, closest_building->pos) < 1)
		index++;
}

void CannonRushTerranWallOff::EnterState()
{
	return;
}

void CannonRushTerranWallOff::ExitState()
{
	return;
}

State* CannonRushTerranWallOff::TestTransitions()
{
	if (index > wall_pos.size())
		return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by);
	return nullptr;
}

std::string CannonRushTerranWallOff::toString() const
{
	return "walloff spot";
}

#pragma endregion

#pragma region CannonRushTerranCannonFirstWallOff

void CannonRushTerranCannonFirstWallOff::TickState()
{
	if (probe->orders.size() > 0)
		return;
	if (cannon_placed == false)
	{
		const Unit* closest_building = Utility::ClosestUnitTo(mediator->GetUnits(IsFriendlyUnit(CANNON)), cannon_pos);
		if (closest_building != nullptr && closest_building->display_type != Unit::DisplayType::Placeholder && (Distance2D(cannon_pos, closest_building->pos) < 1))
			cannon_placed = true;
		else
			mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PHOTONCANNON, cannon_pos, CommandPriorty::low);
	}
	else
	{
		if (wall.size() > 0)
		{
			if (wall[0].type == PYLON)
			{
				if (Utility::DistanceToClosest(mediator->GetUnits(IsNonPlaceholderUnit(PYLON)), wall[0].building_pos) < 1)
				{
					wall.erase(wall.begin());
				}
				else
				{
					if (mediator->CanAfford(PYLON, 1) == false)
					{
						mediator->SetBuildWorkers(false);
						mediator->SetUnitsCommand(mediator->GetUnits(IsFriendlyUnit(NEXUS)), A_CANCEL_PRODUCTION, CommandPriorty::low);
					}
					mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, wall[0].building_pos, CommandPriorty::low);
				}
			}
			else
			{
				if (Utility::DistanceToClosest(mediator->GetUnits(IsNonPlaceholderUnit(GATEWAY)), wall[0].building_pos) < 1)
				{
					wall.erase(wall.begin());
				}
				else
				{
					if (mediator->CanAfford(GATEWAY, 1) == false)
					{
						mediator->SetBuildWorkers(false);
						mediator->SetUnitsCommand(mediator->GetUnits(IsFriendlyUnit(NEXUS)), A_CANCEL_PRODUCTION, CommandPriorty::low);
					}
					mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_GATEWAY, wall[0].building_pos, CommandPriorty::low);
				}
			}
		}
		else
		{
			float dist_to_closest = Utility::DistanceToClosest(mediator->GetUnits(IsUnit(SCV)), probe->pos);
			int minerals = mediator->GetCurrentResources().mineral_cost;
			Point2D gate_pos;
			Point2D pylon_pos;
			for (const auto &pos : gateway_wall_pos)
			{
				if (pos.type == GATEWAY)
					gate_pos = pos.building_pos;
				else if (pos.type == PYLON)
					pylon_pos = pos.building_pos;
			}

			Point2D move_to = Utility::PointBetween(gate_pos, pylon_pos, 1);

			mediator->SetUnitCommand(probe, A_MOVE, move_to, CommandPriorty::low);

			if (dist_to_closest < 8 && minerals < 200)
			{
				// cancel probe
				wall = pylon_wall_pos;
				wall_set = true;
			}
			else if (minerals >= 200)
			{
				// place gateway
				wall = gateway_wall_pos;
				wall_set = true;
			}

		}
	}

}

void CannonRushTerranCannonFirstWallOff::EnterState()
{
	mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PHOTONCANNON, cannon_pos, CommandPriorty::low);
	return;
}

void CannonRushTerranCannonFirstWallOff::ExitState()
{
	return;
}

State* CannonRushTerranCannonFirstWallOff::TestTransitions()
{
	const Unit* cannon = Utility::ClosestUnitTo(mediator->GetUnits(IsFriendlyUnit(CANNON)), cannon_pos);
	if ((wall_set && wall.size() == 0) || (cannon != nullptr && cannon->build_progress > .75))
	{
		mediator->SetBuildWorkers(true);
		return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by);
	}
	return nullptr;
}

std::string CannonRushTerranCannonFirstWallOff::toString() const
{
	return "cannon first walloff spot";
}

#pragma endregion

#pragma region CannonRushTerranStandBy

void CannonRushTerranStandBy::TickState()
{
	mediator->SetUnitCommand(probe, A_MOVE, stand_by_spot, CommandPriorty::low);
}

void CannonRushTerranStandBy::EnterState()
{
	return;
}

void CannonRushTerranStandBy::ExitState()
{
	return;
}

State* CannonRushTerranStandBy::TestTransitions()
{
	int scv_attacking_probe = 0;
	int scv_attacking_pylon = 0;
	int scv_attacking_cannons = 0;
	int pulled_scvs = 0;
	for (const auto& scv : mediator->GetUnits(IsUnit(SCV)))
	{
		if (Distance2D(probe->pos, scv->pos) < 3 && Utility::IsFacing(scv, probe))
		{
			scv_attacking_probe++;
		}
		else
		{
			bool scv_pulled = false;
			if (Distance2D(scv->pos, state_machine->pylons[0]->pos) < 8)
				scv_pulled = true;

			const Unit* closest_pylon = Utility::ClosestUnitTo(state_machine->pylons, scv->pos);
			if (Distance2D(closest_pylon->pos, scv->pos) < 1.5)
			{
				scv_attacking_pylon++;
				scv_pulled = true;
			}

			const Unit* closest_cannon = Utility::ClosestUnitTo(state_machine->cannons, scv->pos);
			if (Distance2D(closest_cannon->pos, scv->pos) < 1.5)
			{
				scv_attacking_cannons++;
				scv_pulled = true;
			}
			if (scv_pulled)
				pulled_scvs++;
		}
	}

	if (scv_attacking_probe > 0)
		return new CannonRushTerranStandByLoop(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by_loop);

	int num_pylons = (int)state_machine->pylons.size();
	for (const auto& pylon : state_machine->pylons)
	{
		if ((pylon->health / pylon->health_max) < (pylon->build_progress * .75))
			num_pylons--;
	}

	if (mediator->GetCurrentResources().mineral_cost >= 100 && pulled_scvs > (4 * num_pylons))
		return new CannonRushTerranExtraPylon(mediator, state_machine, probe);


	if (mediator->GetCurrentResources().mineral_cost >= 150 && state_machine->cannons[0]->build_progress > .5 && (pulled_scvs > state_machine->cannons.size() * 4))
		return new CannonRushTerranExtraCannon(mediator, state_machine, probe);

	if (mediator->GetCurrentResources().mineral_cost >= 150 && mediator->GetUnits(IsFriendlyUnit(GATEWAY)).size() == 0)
		return new CannonRushTerranBuildGateway(mediator, state_machine, probe);

	int num_gasses = (int)mediator->GetUnits(IsFriendlyUnit(ASSIMILATOR)).size() + mediator->GetNumBuildActions(ASSIMILATOR);
	
	//if (mediator->GetCurrentResources().mineral_cost >= 200 && mediator->GetUnits(IsFriendlyUnit(GATEWAY)).size() > 0 && num_gasses < 2 && mediator->GetUnits(IsFinishedUnit(CANNON)).size() > 0)
		//agent->mediator.build_order_manager.BuildBuilding(BuildOrderResultArgData(ASSIMILATOR));

	if (mediator->GetUnits(IsFinishedUnit(CYBERCORE)).size() > 0 && mediator->GetUnits(IsFriendlyUnit(STARGATE)).size() == 0 && mediator->CanAfford(STARGATE, 1))
		return new CannonRushTerranBuildStargate(mediator, state_machine, probe);

	if (pulled_scvs == 0 && mediator->GetUnits(IsFinishedUnit(CANNON)).size() > 0)
		return new CannonRushTerranStandByPhase2(mediator, state_machine, probe, stand_by_spot);

	return nullptr;
}

std::string CannonRushTerranStandBy::toString() const
{
	return "stand by";
}

#pragma endregion

#pragma region CannonRushTerranStandByLoop

void CannonRushTerranStandByLoop::TickState()
{
	mediator->SetUnitCommand(probe, A_MOVE, loop_path[index], CommandPriorty::low);
	if (Distance2D(probe->pos, loop_path[index]) < 2)
		index++;
}

void CannonRushTerranStandByLoop::EnterState()
{
	return;
}

void CannonRushTerranStandByLoop::ExitState()
{
	return;
}

State* CannonRushTerranStandByLoop::TestTransitions()
{
	int threatening_scvs = 0;
	for (const auto& scv : mediator->GetUnits(IsUnit(SCV)))
	{
		if (Distance2D(probe->pos, scv->pos) < 3 && Utility::IsFacing(scv, probe))
			threatening_scvs++;
	}
	if (threatening_scvs == 0 || index >= loop_path.size())
		return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by);
	return nullptr;
}

std::string CannonRushTerranStandByLoop::toString() const
{
	return "stand by loop";
}

#pragma endregion

#pragma region CannonRushTerranExtraPylon

void CannonRushTerranExtraPylon::TickState()
{
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), pylon_pos) < 2)
	{
		pylon_pos = FindPylonPlacement();
		mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon_pos, CommandPriorty::low);
	}
	if (probe->orders.size() == 0)
	{
		mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon_pos, CommandPriorty::low);
	}
}

void CannonRushTerranExtraPylon::EnterState()
{
	pylon_pos = FindPylonPlacement();
	mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon_pos, CommandPriorty::low);
	return;
}

void CannonRushTerranExtraPylon::ExitState()
{
	return;
}

State* CannonRushTerranExtraPylon::TestTransitions()
{
	if (pylon_pos == Point2D(0, 0) || (probe->orders.size() == 0 && mediator->GetCurrentResources().mineral_cost < 100))
		return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by);
	for (const auto& pylon : state_machine->pylons)
	{
		if (Distance2D(pylon->pos, pylon_pos) < 1 && pylon->display_type != Unit::DisplayType::Placeholder)
			return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by);
	}
	return nullptr;
}

std::string CannonRushTerranExtraPylon::toString() const
{
	return "place extra pylon";
}

Point2D CannonRushTerranExtraPylon::FindPylonPlacement()
{
	std::vector<Point2D> possible_positions;
	Point2D cannon_pos = state_machine->cannons[0]->pos;
	for (int i = -6; i <= 6; i += 2)
	{
		for (int j = -6; j <= 6; j += 2)
		{
			Point2D pos = Point2D(cannon_pos.x + i, cannon_pos.y + j);
			if (Distance2D(pos, cannon_pos) > 6)
				continue;
			if (mediator->IsBuildable(pos) && Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), pos) > 2)
				possible_positions.push_back(pos);
		}
	}
	Point2D probe_pos = probe->pos;
	sort(possible_positions.begin(), possible_positions.end(),
		[probe_pos](const Point2D& a, const Point2D& b) -> bool
	{
		return Distance2D(a, probe_pos) < Distance2D(b, probe_pos);
	});
	if (possible_positions.size() > 0)
	{
		for (const auto &pos : possible_positions)
		{
			//if (agent->Query()->Placement(ABILITY_ID::BUILD_PYLON, pos))
				return pos;
		}
	}

	return Point2D(0, 0);
}

#pragma endregion

#pragma region CannonRushTerranExtraCannon

void CannonRushTerranExtraCannon::TickState()
{
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), cannon_pos) < 2 || cannon_pos == Point2D(0, 0))
	{
		cannon_pos = FindCannonPlacement();
		mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PHOTONCANNON, cannon_pos, CommandPriorty::low);
	}
	if (probe->orders.size() == 0)
	{
		mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PHOTONCANNON, cannon_pos, CommandPriorty::low);
	}
}

void CannonRushTerranExtraCannon::EnterState()
{
	cannon_pos = FindCannonPlacement();
	mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PHOTONCANNON, cannon_pos, CommandPriorty::low);
	return;
}

void CannonRushTerranExtraCannon::ExitState()
{
	return;
}

State* CannonRushTerranExtraCannon::TestTransitions()
{
	if (cannon_pos == Point2D(0, 0) || (probe->orders.size() == 0 && mediator->GetCurrentResources().mineral_cost < 150))
		return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by);
	for (const auto& cannon : state_machine->cannons)
	{
		if (Distance2D(cannon->pos, cannon_pos) < 1 && cannon->display_type != Unit::DisplayType::Placeholder)
			return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by);
	}
	return nullptr;
}

std::string CannonRushTerranExtraCannon::toString() const
{
	return "place extra cannon";
}

Point2D CannonRushTerranExtraCannon::FindCannonPlacement()
{
	std::vector<Point2D> possible_positions = state_machine->cannon_places;

	Point2D probe_pos = probe->pos;
	sort(possible_positions.begin(), possible_positions.end(),
		[probe_pos](const Point2D& a, const Point2D& b) -> bool
	{
		return Distance2D(a, probe_pos) < Distance2D(b, probe_pos);
	});
	if (possible_positions.size() > 0)
	{
		for (const auto &pos : possible_positions)
		{
			//if (agent->Query()->Placement(ABILITY_ID::BUILD_PHOTONCANNON, pos))
				return pos;
		}
	}

	return Point2D(0, 0);
}

#pragma endregion

#pragma region CannonRushTerranBuildGateway

void CannonRushTerranBuildGateway::TickState()
{
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), gate_pos) < 2 || gate_pos == Point2D(0, 0))
	{
		gate_pos = FindGatewayPlacement();
		mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_GATEWAY, gate_pos, CommandPriorty::low);
	}
	if (probe->orders.size() == 0)
	{
		mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_GATEWAY, gate_pos, CommandPriorty::low);
	}
}

void CannonRushTerranBuildGateway::EnterState()
{
	gate_pos = FindGatewayPlacement();
	mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_GATEWAY, gate_pos, CommandPriorty::low);
	return;
}

void CannonRushTerranBuildGateway::ExitState()
{
	return;
}

State* CannonRushTerranBuildGateway::TestTransitions()
{
	if (gate_pos == Point2D(0, 0) || (probe->orders.size() == 0 && mediator->GetCurrentResources().mineral_cost < 150))
		return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by);
	for (const auto& gate : mediator->GetUnits(IsFriendlyUnit(GATEWAY)))
	{
		if (Distance2D(gate->pos, gate_pos) < 1 && gate->display_type != Unit::DisplayType::Placeholder)
			return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by);
	}
	return nullptr;
}

std::string CannonRushTerranBuildGateway::toString() const
{
	return "place gateway";
}

Point2D CannonRushTerranBuildGateway::FindGatewayPlacement()
{
	std::vector<Point2D> possible_positions = state_machine->gateway_places;

	Point2D enemy_base = mediator->GetEnemyStartLocation();
	sort(possible_positions.begin(), possible_positions.end(),
		[enemy_base](const Point2D& a, const Point2D& b) -> bool
	{
		return Distance2D(a, enemy_base) > Distance2D(b, enemy_base);
	});
	if (possible_positions.size() > 0)
	{
		for (const auto &pos : possible_positions)
		{
			//if (agent->Query()->Placement(ABILITY_ID::BUILD_GATEWAY, pos))
				return pos;
		}
	}

	return Point2D(0, 0);
}

#pragma endregion

#pragma region CannonRushTerranBuildStargate

void CannonRushTerranBuildStargate::TickState()
{
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), stargate_pos) < 2 || stargate_pos == Point2D(0, 0))
	{
		stargate_pos = FindStargatePlacement();
		mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_STARGATE, stargate_pos, CommandPriorty::low);
	}
	if (probe->orders.size() == 0)
	{
		mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_STARGATE, stargate_pos, CommandPriorty::low);
	}
}

void CannonRushTerranBuildStargate::EnterState()
{
	stargate_pos = FindStargatePlacement();
	mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_STARGATE, stargate_pos, CommandPriorty::low);
	return;
}

void CannonRushTerranBuildStargate::ExitState()
{
	return;
}

State* CannonRushTerranBuildStargate::TestTransitions()
{
	if (stargate_pos == Point2D(0, 0) || (probe->orders.size() == 0 && mediator->CanAfford(STARGATE, 1) == false))
		return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by);
	for (const auto& stargate : mediator->GetUnits(IsFriendlyUnit(STARGATE)))
	{
		if (Distance2D(stargate->pos, stargate_pos) < 1 && stargate->display_type != Unit::DisplayType::Placeholder)
			return new CannonRushTerranStandBy(mediator, state_machine, probe, mediator->GetLocations().cannon_rush_terran_stand_by); //change
	}
	return nullptr;
}

std::string CannonRushTerranBuildStargate::toString() const
{
	return "place stargate";
}

Point2D CannonRushTerranBuildStargate::FindStargatePlacement()
{
	std::vector<Point2D> possible_positions = state_machine->gateway_places;

	Point2D enemy_base = mediator->GetEnemyStartLocation();
	sort(possible_positions.begin(), possible_positions.end(),
		[enemy_base](const Point2D& a, const Point2D& b) -> bool
	{
		return Distance2D(a, enemy_base) > Distance2D(b, enemy_base);
	});
	if (possible_positions.size() > 0)
	{
		for (const auto &pos : possible_positions)
		{
			//if (agent->Query()->Placement(ABILITY_ID::BUILD_GATEWAY, pos))
				return pos;
		}
	}

	return Point2D(0, 0);
}

#pragma endregion

#pragma region CannonRushTerranStandByPhase2

void CannonRushTerranStandByPhase2::TickState()
{
	// build from stargate
	if (next_unit == UNIT_TYPEID::BEACON_PROTOSS)
	{
		if (state_machine->stargates.size() > 0 && state_machine->stargates[0]->build_progress == 1)
		{
			if (mediator->CanAfford(VOID_RAY, 1))
			{
				mediator->SetUnitCommand(state_machine->stargates[0], ABILITY_ID::TRAIN_VOIDRAY, CommandPriorty::low);
				next_unit = FLEET_BEACON;
				return;
			}
		}
	}
	else if (next_unit == TEMPEST)
	{
		if (state_machine->stargates.size() > 0 && mediator->GetUnits(IsFinishedUnit(FLEET_BEACON)).size() > 0)
		{
			for (const auto& stargate : state_machine->stargates)
			{
				if (stargate->build_progress == 1 && stargate->orders.size() == 0)
				{
					if (mediator->CanAfford(TEMPEST, 1))
					{
						mediator->SetUnitCommand(state_machine->stargates[0], ABILITY_ID::TRAIN_TEMPEST, CommandPriorty::low);
						return;
					}
				}
			}
		}
	}

	if (next_unit == STARGATE && mediator->GetUnits(IsFriendlyUnit(STARGATE)).size() > 0 &&
		mediator->GetUnits(IsFriendlyUnit(STARGATE))[0]->display_type != Unit::DisplayType::Placeholder)
		next_unit = UNIT_TYPEID::BEACON_PROTOSS;
	else if (next_unit == FLEET_BEACON && mediator->GetUnits(IsFriendlyUnit(FLEET_BEACON)).size() > 0 &&
		mediator->GetUnits(IsFriendlyUnit(FLEET_BEACON))[0]->display_type != Unit::DisplayType::Placeholder)
		next_unit = TEMPEST;

	if (probe->orders.size() > 0)
		probe_busy = false;

	if (probe->orders.size() > 0)
	{
		/*if (probe->orders[0].ability_id == ABILITY_ID::BUILD_FLEETBEACON)
			next_unit = TEMPEST;
		else if(probe->orders[0].ability_id == ABILITY_ID::BUILD_STARGATE)
			next_unit = UNIT_TYPEID::BEACON_PROTOSS;*/
		return;
	}

	/*if ((next_unit == UNIT_TYPEID::BEACON_PROTOSS || next_unit == FLEET_BEACON) && mediator->GetUnits(IsFinishedUnit(STARGATE)).size() > 0)
	{
		if (Utility::CanAfford(FLEET_BEACON, 1, agent->Observation()))
		{
			// build fleet beacon
			Point2D pos = FindBuildingPlacement();
			if (pos != Point2D(0, 0))
			{
				mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_FLEETBEACON, pos);
				return;
			}
			else
			{
				// build pylon
				Point2D pylon_pos = FindPylonPlacement();
				if (pylon_pos != Point2D(0, 0))
				{
					if (state_machine->pylons[state_machine->pylons.size() - 1]->build_progress == 1)
						mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon_pos);
					return;
				}
			}
		}
	}
	else */if (next_unit == STARGATE && mediator->GetUnits(IsFinishedUnit(CYBERCORE)).size() > 0)
	{
		if (mediator->CanAfford(STARGATE, 1))
		{
			// build stargate
			Point2D pos = FindBuildingPlacement();
			if (pos != Point2D(0, 0))
			{
				mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_STARGATE, pos, CommandPriorty::low);
				return;
			}
			else
			{
				// build pylon
				Point2D pylon_pos = FindPylonPlacement();
				if (pylon_pos != Point2D(0, 0))
				{
					if (state_machine->pylons[state_machine->pylons.size() - 1]->build_progress == 1)
						mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon_pos, CommandPriorty::low);
					return;
				}
			}
		}
	}

	// build extra cannons / batteries
	int extra_minerals = mediator->GetCurrentResources().mineral_cost - Utility::GetCost(next_unit).mineral_cost;
	if (extra_minerals < 100)
	{
		if (probe->orders.size() == 0)
			mediator->SetUnitCommand(probe, A_MOVE, stand_by_spot, CommandPriorty::low);
		return;
	}

	int num_pylons = (int)state_machine->pylons.size();
	int num_cannons = (int)state_machine->cannons.size();
	int num_batteries = (int)state_machine->batteries.size();
	int num_other = (int)(state_machine->gateways.size() + state_machine->stargates.size());

	if (num_pylons <= (num_cannons + num_batteries + num_other) / 4 || state_machine->cannon_places.size() < 5 || state_machine->gateway_places.size() < 5)
	{
		// build pylon
		Point2D pylon_pos = FindPylonPlacement();
		if (pylon_pos != Point2D(0, 0))
		{
			if (state_machine->pylons[state_machine->pylons.size() - 1]->build_progress == 1)
				mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon_pos, CommandPriorty::low);
			return;
		}

	}
	else
	{
		int total_battery_energy = 0;
		for (const auto& battery : state_machine->batteries)
		{
			if (battery->build_progress < 1)
				total_battery_energy += 50;
			else
				total_battery_energy += (int)battery->energy;
		}
		if (mediator->GetUnits(IsFinishedUnit(CYBERCORE)).size() > 0 && total_battery_energy < num_cannons * 100)
		{
			//build battery
			Point2D pos = FindBatteryPlacement();
			if (pos != Point2D(0, 0))
			{
				mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_SHIELDBATTERY, pos, CommandPriorty::low);
				return;
			}
			else
			{
				// build pylon
				Point2D pylon_pos = FindPylonPlacement();
				if (pylon_pos != Point2D(0, 0))
				{
					if (state_machine->pylons[state_machine->pylons.size() - 1]->build_progress == 1)
						mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon_pos, CommandPriorty::low);
					return;
				}
			}
		}
		else if (extra_minerals >= 150)
		{
			// build cannon
			Point2D pos = FindCannonPlacement();
			if (pos != Point2D(0, 0))
			{
				mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PHOTONCANNON, pos, CommandPriorty::low);
				return;
			}
			else
			{
				// build pylon
				Point2D pylon_pos = FindPylonPlacement();
				if (pylon_pos != Point2D(0, 0))
				{
					if (state_machine->pylons[state_machine->pylons.size() - 1]->build_progress == 1)
						mediator->SetUnitCommand(probe, ABILITY_ID::BUILD_PYLON, pylon_pos, CommandPriorty::low);
					return;
				}
			}
		}
	}
	mediator->SetUnitCommand(probe, A_MOVE, stand_by_spot, CommandPriorty::low);
}

void CannonRushTerranStandByPhase2::EnterState()
{
	int num_gasses = (int)mediator->GetUnits(IsFriendlyUnit(ASSIMILATOR)).size() + mediator->GetNumBuildActions(ASSIMILATOR);
	
	for (int i = 0; i < 2 - num_gasses; i++)
	{
		//agent->mediator.build_order_manager.BuildBuilding(BuildOrderResultArgData(ASSIMILATOR));
	}

	if (mediator->GetUnits(IsFriendlyUnit(STARGATE)).size() == 0)
		next_unit = STARGATE;
	else if (mediator->GetUnits(IsFinishedUnit(STARGATE)).size() == 0)
		next_unit = UNIT_TYPEID::BEACON_PROTOSS; // represents a void ray + fleet beacon
	else if (mediator->GetUnits(IsFriendlyUnit(FLEET_BEACON)).size() == 0)
		next_unit = FLEET_BEACON;
	else
		next_unit = TEMPEST;

	return;
}

void CannonRushTerranStandByPhase2::ExitState()
{
	return;
}

State* CannonRushTerranStandByPhase2::TestTransitions()
{
	return nullptr;
}

std::string CannonRushTerranStandByPhase2::toString() const
{
	return "stand by phase 2";
}

Point2D CannonRushTerranStandByPhase2::FindBuildingPlacement()
{
	std::vector<Point2D> possible_positions = state_machine->gateway_places;

	Point2D enemy_base = mediator->GetEnemyStartLocation();
	sort(possible_positions.begin(), possible_positions.end(),
		[enemy_base](const Point2D& a, const Point2D& b) -> bool
	{
		return Distance2D(a, enemy_base) > Distance2D(b, enemy_base);
	});
	if (possible_positions.size() > 0)
	{
		for (const auto &pos : possible_positions)
		{
			//if (agent->Query()->Placement(ABILITY_ID::BUILD_GATEWAY, pos))
				return pos;
		}
	}

	return Point2D(0, 0);
}

Point2D CannonRushTerranStandByPhase2::FindBatteryPlacement()
{
	std::vector<Point2D> possible_positions = state_machine->cannon_places;
	std::vector<std::tuple<Point2D, int>> buildings_in_range;

	for (const auto& pos : possible_positions)
	{
		int in_range = 0;
		for (const auto& building : state_machine->cannons)
		{
			if (Distance2D(pos, building->pos) <= 6)
				in_range++;
		}
		buildings_in_range.push_back(std::make_tuple(pos, in_range));
	}

	sort(possible_positions.begin(), possible_positions.end(),
		[buildings_in_range](const Point2D& a, const Point2D& b) -> bool
	{
		int near_a = 0;
		int near_b = 0;
		for (const auto& tup : buildings_in_range)
		{
			if (std::get<0>(tup) == a)
				near_a = std::get<1>(tup);
			else if (std::get<0>(tup) == b)
				near_b = std::get<1>(tup);
		}
		return near_a > near_b;
	});

	if (possible_positions.size() > 0)
	{
		for (const auto &pos : possible_positions)
		{
			//if (Utility::DistanceToClosest(state_machine->pylons, pos) < 6.5 && agent->Query()->Placement(ABILITY_ID::BUILD_SHIELDBATTERY, pos))
				return pos;
		}
	}

	return Point2D(0, 0);
}

Point2D CannonRushTerranStandByPhase2::FindCannonPlacement()
{
	std::vector<Point2D> possible_positions = state_machine->cannon_places;


	Units cannons = state_machine->cannons;
	sort(possible_positions.begin(), possible_positions.end(),
		[cannons](const Point2D& a, const Point2D& b) -> bool
	{
		float aa = Utility::DistanceToClosest(cannons, a);
		float bb = Utility::DistanceToClosest(cannons, b);
		if (aa <= 6)
		{
			if (bb <= 6)
				return aa > bb;
			else
				return true;
		}
		else
		{
			if (bb <= 6)
				return false;
			return aa < bb;
		}
	});
	if (possible_positions.size() > 0)
	{
		for (const auto &pos : possible_positions)
		{
			//if (Utility::DistanceToClosest(state_machine->pylons, pos) < 6.5 && agent->Query()->Placement(ABILITY_ID::BUILD_PHOTONCANNON, pos))
				return pos;
		}
	}

	return Point2D(0, 0);
}

Point2D CannonRushTerranStandByPhase2::FindPylonPlacement()
{
	std::vector<Point2D> possible_positions = state_machine->cannon_places;


	Units pylons = state_machine->pylons;
	Point2D enemy_base = mediator->GetEnemyStartLocation();
	sort(possible_positions.begin(), possible_positions.end(),
		[pylons, enemy_base](const Point2D& a, const Point2D& b) -> bool
	{
		float aa = Utility::DistanceToClosest(pylons, a) - (Distance2D(a, enemy_base) / 2);
		float bb = Utility::DistanceToClosest(pylons, b) - (Distance2D(b, enemy_base) / 2);
		return aa > bb;
	});
	if (possible_positions.size() > 0)
	{
		for (const auto &pos : possible_positions)
		{
			//if (agent->Query()->Placement(ABILITY_ID::BUILD_PYLON, pos))
				return pos;
		}
	}

	return Point2D(0, 0);
}

#pragma endregion

#pragma region CannonRushTerranUnitMicro

#pragma warning(push)
#pragma warning(disable : 4100)
CannonRushTerranUnitMicro::CannonRushTerranUnitMicro(Mediator* mediator, CannonRushTerran* state_machine, const Unit* zealot)
{
	//army = new ArmyGroup();
	this->mediator = mediator;
	this->state_machine = state_machine;
}
#pragma warning(pop)

void CannonRushTerranUnitMicro::TickState()
{
	//army->CannonRushPressure();
}

void CannonRushTerranUnitMicro::EnterState()
{
	//army->AutoAddUnits({ ZEALOT, STALKER, ADEPT, VOID_RAY,
	//	ORACLE, TEMPEST, CARRIER });
	return;
}

void CannonRushTerranUnitMicro::ExitState()
{
	return;
}

State* CannonRushTerranUnitMicro::TestTransitions()
{
	return nullptr;
}

std::string CannonRushTerranUnitMicro::toString() const
{
	return "micro units";
}

#pragma endregion


#pragma region CannonRushTerran

CannonRushTerran::CannonRushTerran(Mediator* mediator, std::string name, const Unit* probe, int variation) : StateMachine(mediator, name)
{
	this->probe = probe;
	if (variation == 1)
		current_state = new CannonRushTerranMoveAcross(mediator, this, probe);
	else if (variation == 2)
		current_state = new CannonRushTerranMoveAcross2(mediator, this, probe);
	else
		current_state = new CannonRushTerranUnitMicro(mediator, this, probe);


	event_id = mediator->GetUniqueId();
	std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
		this->OnUnitCreatedListener(unit);
	};
	mediator->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	current_state->EnterState();
}

CannonRushTerran::~CannonRushTerran()
{
	mediator->RemoveListenerToOnUnitCreatedEvent(event_id);
	mediator->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

void CannonRushTerran::RunStateMachine()
{
	StateMachine::RunStateMachine();

	/*for (const auto& pos : cannon_places)
	{
		agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), 1, Color(255, 0, 0));
	}
	for (const auto &pos : gateway_places)
	{
		agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), 1.5, Color(0, 255, 0));
	}*/
}

void CannonRushTerran::OnUnitCreatedListener(const Unit* unit)
{
	if (unit->display_type == Unit::DisplayType::Placeholder)
		return;
	if (unit->unit_type == PYLON && Distance2D(unit->pos, mediator->GetLocations().start_location) > 30)
	{
		pylons.push_back(unit);
		for (float i = -7.0f; i <= 7.0f; i++)
		{
			for (float j = -7.0f; j <= 7.0f; j++)
			{
				Point2D pos = unit->pos + Point2D(i, j);
				//if (Distance2D(pos, unit->pos) < 7.5 && std::find(cannon_places.begin(), cannon_places.end(), pos) == cannon_places.end() && agent->Query()->Placement(ABILITY_ID::BUILD_PYLON, pos))
				{
					cannon_places.push_back(pos);
				}
			}
		}
		for (float i = -6.0f; i <= 5.0f; i++)
		{
			for (float j = -6.0f; j <= 5.0f; j++)
			{
				Point2D pos = unit->pos + Point2D(i, j) + Point2D(.5, .5);
				//if (Distance2D(pos, unit->pos) < 6.5 && std::find(gateway_places.begin(), gateway_places.end(), pos) == gateway_places.end() && agent->Query()->Placement(ABILITY_ID::BUILD_BARRACKS, pos))
				{
					gateway_places.push_back(pos);
				}
			}
		}
		SmallBuildingBlock(unit->pos);
	}
	else if (unit->unit_type == CANNON && Distance2D(unit->pos, mediator->GetLocations().start_location) > 30)
	{
		cannons.push_back(unit);
		SmallBuildingBlock(unit->pos);
	}
	else if (unit->unit_type == BATTERY && Distance2D(unit->pos, mediator->GetLocations().start_location) > 30)
	{
		batteries.push_back(unit);
		SmallBuildingBlock(unit->pos);
	}
	else if (unit->unit_type == GATEWAY)
	{
		gateways.push_back(unit);
		BigBuildingBlock(unit->pos);
	}
	else if (unit->unit_type == STARGATE)
	{
		stargates.push_back(unit);
		BigBuildingBlock(unit->pos);
	}
	else if (unit->unit_type == FLEET_BEACON)
	{
		BigBuildingBlock(unit->pos);
	}
}

void CannonRushTerran::OnUnitDestroyedListener(const Unit* unit)
{
	auto pylon = std::find(pylons.begin(), pylons.end(), unit);
	if (pylon != pylons.end())
	{
		pylons.erase(pylon);
		return;
	}

	auto cannon = std::find(cannons.begin(), cannons.end(), unit);
	if (cannon != cannons.end())
	{
		cannons.erase(cannon);
		return;
	}
}

void CannonRushTerran::SmallBuildingBlock(Point2D building_pos)
{
	for (float i = -1.0f; i <= 1.0f; i += 1)
	{
		for (float j = -1.0f; j <= 1.0f; j += 1)
		{
			Point2D pos = building_pos + Point2D(i, j);
			auto pos_it = std::find(cannon_places.begin(), cannon_places.end(), pos);
			if (pos_it != cannon_places.end())
			{
				cannon_places.erase(pos_it);
			}
		}
	}
	for (float i = -2.0f; i <= 1.0f; i += 1)
	{
		for (float j = -2.0f; j <= 1.0f; j += 1)
		{
			Point2D pos = building_pos + Point2D(i, j) + Point2D(.5, .5);
			auto pos_it = std::find(gateway_places.begin(), gateway_places.end(), pos);
			if (pos_it != gateway_places.end())
			{
				gateway_places.erase(pos_it);
			}
		}
	}
}

void CannonRushTerran::BigBuildingBlock(Point2D building_pos)
{
	for (float i = -2.0f; i <= 1.0f; i += 1)
	{
		for (float j = -2.0f; j <= 1.0f; j += 1)
		{
			Point2D pos = building_pos + Point2D(i, j) + Point2D(.5, .5);
			auto pos_it = std::find(cannon_places.begin(), cannon_places.end(), pos);
			if (pos_it != cannon_places.end())
			{
				cannon_places.erase(pos_it);
			}
		}
	}
	for (float i = -2.0f; i <= 2.0f; i += 1)
	{
		for (float j = -2.0f; j <= 2.0f; j += 1)
		{
			Point2D pos = building_pos + Point2D(i, j);
			auto pos_it = std::find(gateway_places.begin(), gateway_places.end(), pos);
			if (pos_it != gateway_places.end())
			{
				gateway_places.erase(pos_it);
			}
		}
	}
}

#pragma endregion



}