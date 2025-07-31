
#include "mediator.h"
#include "locations.h"

#include <fstream>
#include <chrono>
#include <algorithm>
#include <stdlib.h>

namespace sc2
{


void ActionManager::DisplayActiveActions() const
{
	std::string actions_message = "Active Actions:\n";
	for (int i = 0; i < active_actions.size(); i++)
	{
		actions_message += active_actions[i]->toString() + "\n";
		const Unit* unit = active_actions[i]->action_arg->unit;
		if (unit != nullptr)
			mediator->DebugText(active_actions[i]->toString(), unit->pos, Color(0, 255, 0), 20);
	}
	mediator->DebugText(actions_message, Point2D(.1f, 0), Color(0, 255, 0), 20);
}

const std::vector<ActionData*>& ActionManager::GetActiveActions() const
{
	return active_actions;
}

void ActionManager::ProcessActions()
{
	for (int i = 0; i < active_actions.size(); i++)
	{
		bool(sc2::ActionManager::*action)(ActionArgData*) = active_actions[i]->action;
		if ((*this.*action)(active_actions[i]->action_arg))
		{
			active_actions.erase(active_actions.begin() + i);
			i--;
		}

	}
}

void ActionManager::AddAction(ActionData* action)
{
	active_actions.push_back(action);
}

std::vector<ActionData*>::iterator ActionManager::EraseAction(ActionData* action)
{
	for (auto itr = active_actions.begin(); itr != active_actions.end();itr++)
	{
		if ((*itr) == action)
			return active_actions.erase(itr);
	}
	return active_actions.end();
}

bool ActionManager::ActionBuildBuilding(ActionArgData* data)
{
	mediator->DebugSphere(mediator->ToPoint3D(data->position), 1, Color(255, 0, 128));
	UNIT_TYPEID buildingId = data->unitId;
	Point2D pos = data->position;
	const Unit *builder = data->unit;
	if (builder->is_alive == false)
	{
		// builder died
		mediator->RebuildBuilding(data->position, data->unitId);
		return true;
	}
	for (const auto &building : mediator->GetUnits(IsFriendlyUnit(buildingId)))
	{
		if (Distance2D(building->pos, pos) < 1 && building->display_type != Unit::DisplayType::Placeholder)
		{
			mediator->PlaceWorker(builder);
			// finished buildings.remove building.tag
			return true;
		}
	}
	if (Distance2D(builder->pos, pos) < Utility::BuildingSize(buildingId))
	{
		for (const auto& unit : Utility::GetUnitsWithin(mediator->GetUnits(Unit::Alliance::Self), pos, .5f + Utility::BuildingSize(buildingId)))
		{
			if (unit->unit_type == PROBE)
				continue;
			mediator->SetUnitCommand(unit, A_MOVE, Utility::PointBetween(pos, unit->pos, MEDIUM_RANGE), CommandPriorty::high);
		}
		if (mediator->TryBuildBuilding(builder, buildingId, pos) != TryActionResult::success) // TODO handle non success cases
		{
			//mediator->LogMinorError();
		}
	}
	else if (Distance2D(builder->pos, pos) > Utility::BuildingSize(buildingId))
	{
		mediator->SetUnitCommand(builder, A_MOVE, pos, CommandPriorty::low);
	}
	return false;
}

bool ActionManager::ActionBuildBuildingWhenSafe(ActionArgData* data)
{
	mediator->DebugSphere(mediator->ToPoint3D(data->position), 1, Color(255, 128, 0));
	if (mediator->CanAfford(data->unitId, 1) && 
		Utility::DistanceToClosest(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), data->position) > MEDIUM_RANGE &&
		(data->unitId == PYLON || data->unitId == NEXUS || Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Self, IsUnit(PYLON)), data->position) < 6.5))
	{
		const Unit* builder = mediator->GetBuilder(data->position);
		if (builder == nullptr)
		{
			//std::cout << "Error could not find builder in ActionBuildBuildingWhenSafe" << std::endl;
			return false;
		}
		mediator->BuildBuilding(data->unitId, data->position, builder);
		return true;
	}
	return false;
}

bool ActionManager::ActionBuildBuildingMulti(ActionArgData* data)
{
	UNIT_TYPEID buildingId = data->unitIds[data->index];
	Point2D pos = data->position;
	const Unit *builder = data->unit;
	if (builder->is_alive == false)
	{
		// builder died
		builder = mediator->GetBuilder(pos);
		if (builder == nullptr)
		{
			return false;
		}
		mediator->RemoveWorker(builder);
		data->unit = builder;
	}
	for (const auto &building : mediator->GetUnits(IsFriendlyUnit(buildingId)))
	{
		if (Distance2D(building->pos, pos) < 1 && building->display_type != Unit::DisplayType::Placeholder)
		{
			// finished buildings.remove building.tag
			data->index++;
			if (data->index < data->unitIds.size())
			{
				data->position = mediator->GetLocation(data->unitIds[data->index]);
				active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuildingMulti, data));
			}
			else
			{
				mediator->PlaceWorker(builder);
			}
			return true;
		}
	}
	if (Distance2D(builder->pos, pos) < Utility::BuildingSize(buildingId))
	{
		for (const auto& unit : Utility::GetUnitsWithin(mediator->GetUnits(Unit::Alliance::Self), pos, .5f + Utility::BuildingSize(buildingId)))
		{
			if (unit == builder)
				continue;
			mediator->SetUnitCommand(unit, A_MOVE, Utility::PointBetween(pos, unit->pos, MEDIUM_RANGE), CommandPriorty::high);
		}
		if (mediator->TryBuildBuilding(builder, buildingId, pos) != TryActionResult::success) // TODO handle non success cases
		{
			//mediator->LogMinorError();
		}
	}
	else if (Distance2D(builder->pos, pos) > Utility::BuildingSize(buildingId))
	{
		mediator->SetUnitCommand(builder, A_MOVE, pos, CommandPriorty::low);
	}
	return false;
}

bool ActionManager::ActionBuildBuildingWhenAble(ActionArgData* data)
{
	if (mediator->HasTechForBuilding(data->unitId))
	{
		mediator->BuildBuilding(data->unitId);
		return true;
	}
	return false;
}

bool ActionManager::ActionBuildProxyMulti(ActionArgData* data) // TODO add avoidance code thats in ActionRemoveScoutToProxy
{
	UNIT_TYPEID buildingId = data->unitIds[data->index];
	Point2D pos = data->position;
	const Unit *builder = data->unit;
	if (builder->is_alive == false)
	{
		// builder died
		builder = mediator->GetBuilder(pos);
		if (builder == nullptr)
		{
			return false;
		}
		mediator->RemoveWorker(builder);
		data->unit = builder;
	}
	for (const auto &building : mediator->GetUnits(IsFriendlyUnit(buildingId)))
	{
		if (Point2D(building->pos) == pos && building->display_type != Unit::DisplayType::Placeholder)
		{
			// finished buildings.remove building.tag
			data->index++;
			if (data->index < data->unitIds.size())
			{
				data->position = mediator->GetProxyLocation(data->unitIds[data->index]);
				active_actions.push_back(new ActionData(&ActionManager::ActionBuildProxyMulti, data));
			}
			else
			{
				mediator->PlaceWorker(builder);
			}
			return true;
		}
	}
	if (Distance2D(builder->pos, pos) < Utility::BuildingSize(buildingId))
	{
		if (mediator->TryBuildBuilding(builder, buildingId, pos) != TryActionResult::success) // TODO handle non success cases
		{
			//mediator->LogMinorError();
		}
	}
	else if (Distance2D(builder->pos, pos) > Utility::BuildingSize(buildingId))
	{
		mediator->SetUnitCommand(builder, A_MOVE, pos, CommandPriorty::low);
	}
	return false;
}

#pragma warning(push)
#pragma warning(disable : 4100)
bool ActionManager::ActionScoutZerg(ActionArgData* data)
{
	return true;
}

bool ActionManager::ActionContinueMakingWorkers(ActionArgData* data)
{
	int extra_workers = 0;// data->index; TODO add extra workers to worker manager
	int num_workers = mediator->GetNumWorkers();
	int num_nexi = (int)mediator->GetUnits(IsFriendlyUnit(NEXUS)).size();
	int num_assimilators = (int)mediator->GetUnits(IsFriendlyUnit(ASSIMILATOR)).size();
	if (num_workers >= std::min(num_nexi * 16 + num_assimilators * 3, MAX_WORKERS) + extra_workers)
	{
		mediator->SetBuildWorkers(false);
	}
	else
	{
		mediator->SetBuildWorkers(true);
	}

	return false;
}

bool ActionManager::ActionContinueBuildingPylons(ActionArgData* data)
{
	int build_pylon_actions = 0;
	for (const auto &action : active_actions)
	{
		if (action->action == &ActionManager::ActionBuildBuilding && action->action_arg->unitId == PYLON)
		{
			build_pylon_actions++;
		}
	}

	int supply_used = mediator->GetSupplyUsed();
	int supply_cap = mediator->GetSupplyCap() + 8 * (build_pylon_actions/* - mediator->extra_pylons*/); // TODO readd extra pylons functionality

	for (const auto &building : mediator->GetUnits(Unit::Alliance::Self))
	{
		if (supply_cap >= 200)
			return false;
		if (building->unit_type == PYLON)
		{
			if (building->build_progress < 1)
				supply_cap += 8;
		}
		else if(building->unit_type == NEXUS)
		{
			supply_used += 1;
		}
		else if (building->unit_type == GATEWAY)
		{
			supply_used += 2;
		}
		else if (building->unit_type == WARP_GATE)
		{
			supply_used += 2;
		}
		else if (building->unit_type == ROBO)
		{
			supply_used += 3;
		}
		else if (building->unit_type == STARGATE)
		{
			supply_used += 4;
		}
	}
	/*
	for (const auto &pylon : mediator->Observation()->GetUnits(IsFriendlyUnit(PYLON)))
	{
		if (pylon->build_progress < 1)
			pending_pylons++;
	}
	int supply_used = mediator->Observation()->GetFoodUsed();
	int supply_cap = mediator->Observation()->GetFoodCap() - 8 * mediator->extra_pylons;
	if (supply_cap >= 200)
		return false;
	supply_cap += 8 * (build_pylon_actions + pending_pylons);
	supply_used += mediator->Observation()->GetUnits(IsFriendlyUnit(NEXUS)).size();
	supply_used += 2 * mediator->Observation()->GetUnits(IsFriendlyUnit(WARP_GATE)).size();
	supply_used += 2 * mediator->Observation()->GetUnits(IsFriendlyUnit(GATEWAY)).size();
	supply_used += 3 * mediator->Observation()->GetUnits(IsFriendlyUnit(ROBO)).size();
	supply_used += 3 * mediator->Observation()->GetUnits(IsFriendlyUnit(STARGATE)).size();
	*/
	if (supply_used >= supply_cap)
		mediator->BuildBuilding(PYLON);

	return false;
}

bool ActionManager::ActionContinueUpgrades(ActionArgData* data)
{
	// TODO make global upgrade tracker
	

	return false;
}

bool ActionManager::ActionContinueSpendingNexusEnergy(ActionArgData* data)
{
	std::map<const Unit*, float> nexi_with_energy;
	float total = 0;
	for (const auto& nexus : mediator->GetUnits(Unit::Alliance::Self, IsFinishedUnit(NEXUS)))
	{
		if (nexus->energy >= 200)
		{
			nexi_with_energy[nexus] = 4;
			total += 4;
		}
		else if (nexus->energy >= 150)
		{
			nexi_with_energy[nexus] = 3;
			total += 3;
		}
		else if (nexus->energy >= 100)
		{
			nexi_with_energy[nexus] = 2;
			total += 2;
		}
		else if (nexus->energy >= 50)
		{
			nexi_with_energy[nexus] = 1;
			total += 1;
		}
	}

	// chrono
	Units need_chrono;
	for (const auto &building : mediator->GetUnits(IsFinishedUnit(ROBO_BAY)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}
	for (const auto &building : mediator->GetUnits(IsFinishedUnit(ROBO)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}
	for (const auto &building : mediator->GetUnits(IsFinishedUnit(STARGATE)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}
	for (const auto &building : mediator->GetUnits(IsFinishedUnit(FORGE)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}

	for (auto &nexus : nexi_with_energy)
	{
		if (need_chrono.size() == 0 || total <= 1)
			break;
		if (nexus.second >= 1)
		{
			nexus.second--;
			total--;
			mediator->SetUnitCommand(nexus.first, A_CHRONO, need_chrono[0], CommandPriorty::low);
			need_chrono.erase(need_chrono.begin());
		}
	}

	// energy recharge
	Units energy_units = mediator->GetUnits(Unit::Alliance::Self, IsUnits({ ORACLE, SENTRY }));
	if (mediator->IsEnergyRechargeOffCooldown() &&
		energy_units.size() > 0)
	{
		bool recharge_used = false;
		for (const auto& unit : energy_units)
		{
			if (unit->energy < 100)
			{
				for (const auto& nexus : nexi_with_energy)
				{
					if (Distance2D(nexus.first->pos, unit->pos) < RANGE_ENERGY_RECHARGE)
					{
						for (const auto& ability : mediator->GetAbilitiesForUnit(nexus.first).abilities)
						{
							if (strcmp(AbilityTypeToName(ability.ability_id), "UNKNOWN") == 0)
							{
								mediator->SetUnitCommand(nexus.first, ability.ability_id, unit, CommandPriorty::low);
								recharge_used = true;
								break;
							}
						}
					}
					if (recharge_used)
						break;
				}
			}
			if (recharge_used)
				break;
		}
	}

	// recall TODO


	return false;
}

bool ActionManager::ActionContinueExpanding(ActionArgData* data)
{
	GameState game_state = mediator->GetGameState();
	switch (game_state.game_state_worker)
	{
	case GameStateWorker::slightly_more:
	case GameStateWorker::much_more:
		// dont expand if we have more workers
		return false;
	case GameStateWorker::even:
		if (game_state.good_worker_intel || mediator->GetOngoingAttacks().size() > 0)
			return false;
		break;
	case GameStateWorker::slightly_less:
		if (mediator->GetOngoingAttacks().size() > 0)
			return false;
		break;
	case GameStateWorker::much_less:
		break;
	}

	if (mediator->NumFar3rdWorkers() > 0)
	{
		if (mediator->HasBuildingUnderConstruction(NEXUS))
			return false;

		for (const auto &action : active_actions)
		{
			if (action->action == &ActionManager::ActionBuildBuilding && action->action_arg->unitId == NEXUS)
			{
				return false;
			}
		}
		if (mediator->GetUnits(IsFriendlyUnit(ASSIMILATOR)).size() < 2 * mediator->GetUnits(IsFriendlyUnit(NEXUS)).size())
		{
			for (const auto& action : active_actions)
			{
				if (action->action == &ActionManager::ActionBuildBuilding && action->action_arg->unitId == ASSIMILATOR)
				{
					return false;
				}
			}
			mediator->BuildBuilding(ASSIMILATOR);
		}
		else
		{
			mediator->BuildBuilding(NEXUS);
		}
		return false;
	}

	return false;
}

#pragma warning(pop)

bool ActionManager::ActionChronoTillFinished(ActionArgData* data)
{
	if (mediator->GetGameLoop() % 2 == 0) // TODO avoid double chronoing a building
		return false;

	const Unit* building = data->unit;
	if (building->orders.size() == 0 || Utility::GetOrderTimeLeft(building->orders[0]) < LONG_RANGE)
	{
		return true;
	}
	for (const auto &buff : building->buffs)
	{
		if (buff == BUFF_ID::CHRONOBOOSTENERGYCOST)
			return false;
	}
	for (const auto &nexus : mediator->GetUnits(IsFriendlyUnit(NEXUS)))
	{
		if (nexus->energy >= ENERGY_COST_CHRONO && nexus->build_progress == 1)
		{
			mediator->SetUnitCommand(nexus, A_CHRONO, building, CommandPriorty::low);
			return false;
		}
		/*for (const auto &ability : mediator->Query()->GetAbilitiesForUnit(nexus).abilities)
		{
			if (ability.ability_id == A_CHRONO)
			{
				mediator->SetUnitCommand(nexus, A_CHRONO, building, 0);
				return false;
			}
		}*/
	}
	return false;
}

bool ActionManager::ActionConstantChrono(ActionArgData* data)
{
	const Unit* building = data->unit;
	for (const auto &buff : building->buffs)
	{
		if (buff == BUFF_ID::CHRONOBOOSTENERGYCOST)
		{
			return false;
		}
	}

	return false;
}

bool ActionManager::ActionTrainFromProxyRobo(ActionArgData* data)
{
	const Unit * robo = data->unit;
	int num_prisms = (int)mediator->GetUnits(IsFriendlyUnit(PRISM)).size();
	int num_obs = (int)mediator->GetUnits(IsFriendlyUnit(OBSERVER)).size();

	if (robo->build_progress == 1 && robo->orders.size() == 0)
	{
		if (num_prisms == 0 && mediator->TryTrainUnit(robo, PRISM) == TryActionResult::success)
			return false;
		else if (num_obs == 0 && mediator->TryTrainUnit(robo, OBSERVER) == TryActionResult::success)
			return false;
		else if (mediator->TryTrainUnit(robo, IMMORTAL) == TryActionResult::success)
			return false;
	}
	return false;
}

#pragma warning(push)
#pragma warning(disable : 4100)
bool ActionManager::ActionPullOutOfGas(ActionArgData* data)
{
	if (mediator->PullOutOfGas())
		return true;
	return false;
}
#pragma warning(pop)

bool ActionManager::ActionRemoveScoutToProxy(ActionArgData* data)
{
	bool pylon_placed = false;
	bool pylon_finished = false;
	for (const auto &pylon : mediator->GetUnits(IsFriendlyUnit(PYLON)))
	{
		if (Distance2D(pylon->pos, data->position) < 1)
		{
			pylon_placed = true;
			if (pylon->build_progress == 1)
				pylon_finished = true;
			break;
		}
	}
	//int build_time = data->index;
	const Unit* scout = data->unit;

	
	if (Distance2D(scout->pos, data->position) > 1 && !pylon_placed)
	{
		mediator->SetUnitCommand(scout, A_MOVE, data->position, CommandPriorty::low);
	}
	else if (Distance2D(scout->pos, data->position) < 5 && !pylon_placed)
	{
		if (mediator->GetGameLoop() / FRAME_TIME >= data->index)
		{
			mediator->SetUnitCommand(scout, ABILITY_ID::BUILD_PYLON, data->position, CommandPriorty::low);
		}
		else if (Utility::DistanceToClosest(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), scout->pos) < VERY_CLOSE_RANGE)
		{
			const Unit* enemy = Utility::ClosestTo(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), scout->pos);
			if (Utility::GetUnitsThatCanAttack(mediator->GetUnits(Unit::Alliance::Enemy), scout, 0).size() > 0)
			{
				const Unit* base_minerals = Utility::ClosestTo(mediator->GetUnits(IsUnits(MINERAL_PATCH)), mediator->GetStartLocation());
				if (base_minerals == nullptr)
				{
					std::cerr << "Error could not find minerals close to " << std::to_string(mediator->GetStartLocation().x) << ", " << std::to_string(mediator->GetStartLocation().y) <<
						" in ActionManager::ActionRemoveScoutToProxy" << std::endl;
					mediator->LogMinorError();
				}
				else
				{
					mediator->SetUnitCommand(scout, A_SMART, base_minerals, CommandPriorty::normal);
				}
			}
			else
			{
				Point2D run_away_pos = Point2D(0, 0);
				float angle = 15;
				while (mediator->IsPathable(run_away_pos) == false)
				{
					run_away_pos = Utility::RunAwayCircle(enemy->pos, scout->pos, 2, angle);
					angle += 15;
				}
				mediator->DebugSphere(mediator->ToPoint3D(run_away_pos), .5, Color(0, 0, 255));
				mediator->SetUnitCommand(scout, A_MOVE, run_away_pos, CommandPriorty::normal);
			}
		}
	}
	else if (pylon_placed)
	{
		if (data->unitId == PYLON)
		{
			mediator->PlaceWorker(data->unit);
			return true;
		}
		if (pylon_finished && mediator->CanBuildBuilding(data->unitId))
		{
			std::vector<Point2D> building_locations = mediator->GetProxyLocations(data->unitId);
			Point2D pos = building_locations[0];
			active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(scout, data->unitId, pos)));
			return true;
		}
		else if (Utility::DistanceToClosest(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), scout->pos) < 3)
		{
			const Unit* enemy = Utility::ClosestTo(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), scout->pos);
			if (Utility::GetUnitsThatCanAttack(mediator->GetUnits(Unit::Alliance::Enemy), scout, 0).size() > 0)
			{
				const Unit* base_minerals = Utility::ClosestTo(mediator->GetUnits(IsUnits(MINERAL_PATCH)), mediator->GetStartLocation());
				if (base_minerals == nullptr)
				{
					std::cerr << "Error could not find minerals close to " << std::to_string(mediator->GetStartLocation().x) << ", " << std::to_string(mediator->GetStartLocation().y) <<
						" in ActionManager::ActionRemoveScoutToProxy" << std::endl;
					mediator->LogMinorError();
				}
				else
				{
					mediator->SetUnitCommand(scout, A_SMART, base_minerals, CommandPriorty::normal);
				}
			}
			else
			{
				Point2D run_away_pos = Point2D(0, 0);
				float angle = 15;
				while (mediator->IsPathable(run_away_pos) == false || Distance2D(run_away_pos, data->position) < 1)
				{
					run_away_pos = Utility::RunAwayCircle(enemy->pos, scout->pos, 2, angle);
					angle += 15;
				}
				mediator->DebugSphere(mediator->ToPoint3D(run_away_pos), .5, Color(0, 0, 255));
				mediator->SetUnitCommand(scout, A_MOVE, run_away_pos, CommandPriorty::normal);
			}
		}
	}
	return false;
}

#pragma warning(push)
#pragma warning(disable : 4100)
bool ActionManager::ActionDTHarassTerran(ActionArgData* data)
{
	/*for (const auto &unit : mediator->GetUnits(IsFriendlyUnit(DARK_TEMPLAR)))
	{
		// if outside -> move into enemy main
		if ((unit->pos.z + .1 < mediator->ToPoint3D(mediator->GetStartLocation()).z || unit->pos.z - .1 > mediator->ToPoint3D(mediator->GetStartLocation()).z) && unit->orders.size() == 0)
		{
			mediator->SetUnitCommand(unit, A_MOVE, mediator->locations->initial_scout_pos, 0);
			mediator->SetUnitCommand(unit, ABILITY_ID::EFFECT_SHADOWSTRIDE, Utility::PointBetween(mediator->locations->initial_scout_pos, mediator->GetEnemyStartLocation(), 7), 0, true);
			continue;
		}
		// avoid scans
		// avoid ravens
		// spread out

		// target prio
		// repairing scvs
		// tech labs bulding on starports
		// tech labs on idle starports
		// active starports with tech labs
		// main orbital
		// mules
		// scvs
		// army units
	}*/
	return false;
}
#pragma warning(pop)

bool ActionManager::ActionUseProxyDoubleRobo(ActionArgData* data)
{
	for (const auto &robo : mediator->GetUnits(IsFriendlyUnit(ROBO)))
	{
		if (robo->build_progress < 1)
			continue;
		if (robo->orders.size() == 0)
		{
			if (data->unitIds.size() == 0)
			{
				mediator->TryTrainUnit(robo, IMMORTAL);
			}
			else if (mediator->CanAfford(data->unitIds[0], 1))
			{
				if (mediator->TryTrainUnit(robo, data->unitIds[0]) == TryActionResult::success)
					data->unitIds.erase(data->unitIds.begin());
			}
		}
		else if (robo->orders[0].ability_id == ABILITY_ID::TRAIN_IMMORTAL)
		{
			if (Utility::HasBuff(robo, BUFF_ID::CHRONOBOOSTENERGYCOST))
				continue;

			for (const auto &nexus : mediator->GetUnits(IsFriendlyUnit(NEXUS)))
			{
				if (nexus->energy >= 50 && nexus->build_progress == 1)
					mediator->SetUnitCommand(nexus, A_CHRONO, robo, CommandPriorty::low);
				/*for (const auto &ability : mediator->Query()->GetAbilitiesForUnit(nexus).abilities)
				{
					if (ability.ability_id == A_CHRONO)
					{
						mediator->SetUnitCommand(nexus, A_CHRONO, robo, 0);
					}
				}*/
			}
		}
	}
	return false;
}

#pragma warning(push)
#pragma warning(disable : 4100)
bool ActionManager::ActionAllIn(ActionArgData* data)
{
	/*ArmyGroup* army = data->army_group;
	Point2D retreat_point = army->attack_path[army->current_attack_index - 2];

	Point2D attack_point = army->attack_path[army->current_attack_index];

	Units prisms = army->warp_prisms;
	Units stalkers = army->stalkers;
	Units observers = army->observers;
	Units immortals = army->immortals;


	bool obs_in_position = false;
	if (stalkers.size() > 0)
	{
		if (observers.size() > 0)
		{
			if (Utility::DistanceToClosest(observers, attack_point) < 10)
				obs_in_position = true;
			mediator->ObserveAttackPath(observers, retreat_point, attack_point);
		}
		if (prisms.size() > 0)
		{
			mediator->StalkerAttackTowardsWithPrism(stalkers, prisms, retreat_point, attack_point, obs_in_position);
			if (immortals.size() > 0)
				mediator->ImmortalAttackTowardsWithPrism(immortals, prisms, retreat_point, attack_point, obs_in_position);
		}
		else
		{
			mediator->StalkerAttackTowards(stalkers, retreat_point, attack_point, obs_in_position);
			if (immortals.size() > 0)
				mediator->ImmortalAttackTowards(immortals, retreat_point, attack_point, obs_in_position);
		}
	}

	if (army->current_attack_index > 2 && Distance2D(Utility::Center(stalkers), retreat_point) < 3)
		army->current_attack_index--;
	if (army->current_attack_index < army->attack_path.size() - 1 && Distance2D(Utility::MedianCenter(stalkers), attack_point) < 3)
	{
		if (obs_in_position)
			army->current_attack_index++;
		else
			army->current_attack_index = std::min(army->current_attack_index + 1, army->high_ground_index - 1);
	}
	*/
	return false;
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4100)
bool ActionManager::ActionScourMap(ActionArgData* data)
{
	ImageData raw_map = mediator->GetPathingGrid();
	for (const auto& unit : mediator->GetUnits(IsFightingUnit(Unit::Alliance::Self)))
	{
		if (unit->orders.size() == 0)
		{
			std::srand((unsigned int)(unit->tag + mediator->GetGameLoop()));
			int x = std::rand() % raw_map.width;
			int y = std::rand() % raw_map.height;
			Point2D pos = Point2D((float)x, (float)y);
			while (!unit->is_flying && !mediator->IsPathable(pos))
			{
				x = std::rand() % raw_map.width;
				y = std::rand() % raw_map.height;
				pos = Point2D((float)x, (float)y);
			}
			mediator->SetUnitCommand(unit, A_ATTACK, pos, CommandPriorty::low);
		}
	}
	return false;
}
#pragma warning(pop)

bool ActionManager::ActionCheckBaseForCannons(ActionArgData* data)
{
	if (data->unit == nullptr || data->unit->is_alive == false)
	{
		data->unit = mediator->GetBuilder(mediator->GetStartLocation());
		if (data->unit == nullptr)
			return false;
		mediator->RemoveWorker(data->unit);
	}

	std::vector<Point2D> path = mediator->GetSelfMainScoutPath();
	if (data->index >= path.size())
	{
		mediator->PlaceWorker(data->unit);
		return true;
	}

	Point2D pos = path[data->index];

	if (Distance2D(data->unit->pos, pos) < 2)
		data->index++;
	else
		mediator->SetUnitCommand(data->unit, A_MOVE, pos, CommandPriorty::low);

	return false;
}

bool ActionManager::ActionCheckNaturalForCannons(ActionArgData* data)
{
	if (data->unit == nullptr || data->unit->is_alive == false)
	{
		data->unit = mediator->GetBuilder(mediator->GetNaturalLocation());
		if (data->unit == nullptr)
			return false;
		mediator->RemoveWorker(data->unit);
	}

	std::vector<Point2D> path = mediator->GetSelfNaturalScoutPath();
	if (data->index >= path.size())
	{
		mediator->PlaceWorker(data->unit);
		return true;
	}

	Point2D pos = path[data->index];

	if (Distance2D(data->unit->pos, pos) < 2)
		data->index++;
	else
		mediator->SetUnitCommand(data->unit, A_MOVE, pos, CommandPriorty::low);

	return false;
}

bool ActionManager::ActionCheckForBunkerRush(ActionArgData* data)
{
	if (mediator->GetCurrentTime() > 120 && Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ BUNKER, SCV })), mediator->GetNaturalLocation()) > 15)
	{
		if (data->unit != nullptr && data->unit->is_alive)
			mediator->PlaceWorker(data->unit);
		return true;
	}
	if (data->unit == nullptr || data->unit->is_alive == false)
	{
		data->unit = mediator->GetBuilder(mediator->GetNaturalLocation());
		if (data->unit == nullptr)
			return false;
		mediator->RemoveWorker(data->unit);
	}

	const Unit* target = nullptr;
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(BUNKER)), mediator->GetNaturalLocation()) < 15)
	{
		const Unit* scv = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(SCV)), data->unit->pos);
		if (scv == nullptr || Distance2D(scv->pos, data->unit->pos) > MEDIUM_RANGE)
			target = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(BUNKER)), data->unit->pos);
		else
			target = scv;
	}

	if (target)
	{
		if (data->unit->weapon_cooldown == 0)
			mediator->SetUnitCommand(data->unit, A_ATTACK, target, CommandPriorty::low);
		else
			mediator->SetUnitCommand(data->unit, A_MOVE, target, CommandPriorty::low);
	}
	else
	{
		if (data->unit->weapon_cooldown == 0 && Utility::GetUnitsInRange(mediator->GetUnits(Unit::Alliance::Enemy), data->unit, 0).size() > 0)
		{
			mediator->SetUnitCommand(data->unit, A_ATTACK, Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), data->unit->pos), CommandPriorty::low);
		}
		else if (data->unit->orders.size() == 0 || data->unit->orders[0].ability_id == A_ATTACK || Distance2D(data->unit->pos, mediator->GetNaturalLocation()) > LONG_RANGE)
		{
			Point2D pos = Utility::FurthestFrom(mediator->GetLocations().natural_front, data->unit->pos);
			mediator->SetUnitCommand(data->unit, A_MOVE, pos, CommandPriorty::low);
		}
	}

	return false;
}

}