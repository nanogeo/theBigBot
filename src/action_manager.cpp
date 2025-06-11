
#include "mediator.h"
#include "locations.h"

#include <fstream>
#include <chrono>
#include <algorithm>
#include <stdlib.h>

namespace sc2
{

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
		if (mediator->TryBuildBuilding(builder, buildingId, pos) != TryActionResult::success) // TODO handle non success cases
		{
			std::cerr << "Failed to build building " << std::endl;
			//mediator->LogMinorError();
		}
	}
	else if (Distance2D(builder->pos, pos) > Utility::BuildingSize(buildingId))
	{
		mediator->SetUnitCommand(builder, ABILITY_ID::GENERAL_MOVE, pos, 0);
	}
	return false;
}

bool ActionManager::ActionBuildBuildingWhenSafe(ActionArgData* data)
{
	mediator->DebugSphere(mediator->ToPoint3D(data->position), 1, Color(255, 128, 0));
	if (mediator->CanAfford(data->unitId, 1) && 
		Utility::DistanceToClosest(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), data->position) > 10 &&
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
		if (mediator->TryBuildBuilding(builder, buildingId, pos) != TryActionResult::success) // TODO handle non success cases
		{
			std::cerr << "Failed to build building " << std::endl;
			//mediator->LogMinorError();
		}
	}
	else if (Distance2D(builder->pos, pos) > Utility::BuildingSize(buildingId))
	{
		mediator->SetUnitCommand(builder, ABILITY_ID::GENERAL_MOVE, pos, 0);
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
			std::cerr << "Failed to build building " << std::endl;
			//mediator->LogMinorError();
		}
	}
	else if (Distance2D(builder->pos, pos) > Utility::BuildingSize(buildingId))
	{
		mediator->SetUnitCommand(builder, ABILITY_ID::GENERAL_MOVE, pos, 0);
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
#ifdef DEBUG_TIMING
	unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
#endif

	int extra_workers = 0;// data->index; TODO add extra workers to worker manager
	int num_workers = mediator->GetNumWorkers();
	int num_nexi = (int)mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)).size();
	int num_assimilators = (int)mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ASSIMILATOR)).size();
	if (num_workers >= std::min(num_nexi * 16 + num_assimilators * 3, 70) + extra_workers)
	{
		mediator->SetBuildWorkers(false);
	}
	else
	{
		mediator->SetBuildWorkers(true);
	}
#ifdef DEBUG_TIMING
	unsigned long long end_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	std::ofstream make_workers;
	make_workers.open("make_workers.txt", std::ios_base::app);

	make_workers << end_time - start_time << "\n";
	make_workers.close();
#endif

	return false;
}

bool ActionManager::ActionContinueBuildingPylons(ActionArgData* data)
{
#ifdef DEBUG_TIMING
	unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
#endif

	int build_pylon_actions = 0;
	for (const auto &action : active_actions)
	{
		if (action->action == &ActionManager::ActionBuildBuilding && action->action_arg->unitId == UNIT_TYPEID::PROTOSS_PYLON)
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
		if (building->unit_type == UNIT_TYPEID::PROTOSS_PYLON)
		{
			if (building->build_progress < 1)
				supply_cap += 8;
		}
		else if(building->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
		{
			supply_used += 1;
		}
		else if (building->unit_type == UNIT_TYPEID::PROTOSS_GATEWAY)
		{
			supply_used += 2;
		}
		else if (building->unit_type == UNIT_TYPEID::PROTOSS_WARPGATE)
		{
			supply_used += 2;
		}
		else if (building->unit_type == UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)
		{
			supply_used += 3;
		}
		else if (building->unit_type == UNIT_TYPEID::PROTOSS_STARGATE)
		{
			supply_used += 4;
		}
	}
	/*
	for (const auto &pylon : mediator->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_PYLON)))
	{
		if (pylon->build_progress < 1)
			pending_pylons++;
	}
	int supply_used = mediator->Observation()->GetFoodUsed();
	int supply_cap = mediator->Observation()->GetFoodCap() - 8 * mediator->extra_pylons;
	if (supply_cap >= 200)
		return false;
	supply_cap += 8 * (build_pylon_actions + pending_pylons);
	supply_used += mediator->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)).size();
	supply_used += 2 * mediator->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_WARPGATE)).size();
	supply_used += 2 * mediator->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_GATEWAY)).size();
	supply_used += 3 * mediator->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)).size();
	supply_used += 3 * mediator->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_STARGATE)).size();
	*/
	if (supply_used >= supply_cap)
		mediator->build_order_manager.BuildBuilding(UNIT_TYPEID::PROTOSS_PYLON);

#ifdef DEBUG_TIMING
	unsigned long long end_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	std::ofstream build_pylons;
	build_pylons.open("build_pylons.txt", std::ios_base::app);

	build_pylons << end_time - start_time << "\n";
	build_pylons.close();
#endif

	return false;
}

bool ActionManager::ActionContinueUpgrades(ActionArgData* data)
{
#ifdef DEBUG_TIMING
	unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
#endif

	// TODO make global upgrade tracker
	std::vector<ABILITY_ID> upgrades = {};
	if (mediator->GetEnemyRace() == Race::Zerg)
	{
		if (mediator->GetUpgradeLevel(UpgradeType::shields) < 3)
			upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSSHIELDS);
		if (mediator->GetUpgradeLevel(UpgradeType::ground_weapons) < 3)
			upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS);
		if (mediator->GetUpgradeLevel(UpgradeType::ground_armor) < 3)
			upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR);
	}
	else if (mediator->GetEnemyRace() == Race::Terran)
	{
		if (mediator->GetUpgradeLevel(UpgradeType::ground_armor) < 3)
			upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR);
		if (mediator->GetUpgradeLevel(UpgradeType::ground_weapons) < 3)
			upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS);
		if (mediator->GetUpgradeLevel(UpgradeType::shields) < 3)
			upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSSHIELDS);
	}
	else
	{
		if (mediator->GetUpgradeLevel(UpgradeType::ground_weapons) < 3)
			upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS);
		if (mediator->GetUpgradeLevel(UpgradeType::ground_armor) < 3)
			upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR);
		if (mediator->GetUpgradeLevel(UpgradeType::shields) < 3)
			upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSSHIELDS);
	}

	Units idle_forges;
	for (const auto &forge : mediator->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_FORGE)))
	{
		if (forge->orders.size() > 0)
			upgrades.erase(std::remove(upgrades.begin(), upgrades.end(), forge->orders[0].ability_id), upgrades.end());
		else
			idle_forges.push_back(forge);
	}
	if (idle_forges.size() > 0 && upgrades.size() > 0)
	{
		for (const auto &forge : idle_forges)
		{
			mediator->SetUnitCommand(forge, upgrades[0], 0);
			upgrades.erase(upgrades.begin());
			if (upgrades.size() == 0)
				break;
		}
	}

	if (mediator->GetStargateProduction() == UNIT_TYPEID::INVALID)
		return false;

	std::vector<ABILITY_ID> air_upgrades = {};
	if (mediator->GetUpgradeLevel(UpgradeType::air_weapons) < 3)
		air_upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONS);
	if (mediator->GetUpgradeLevel(UpgradeType::air_armor) < 3)
		air_upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSAIRARMOR);

	Units idle_cybers;
	for (const auto &cyber : mediator->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)))
	{
		if (cyber->orders.size() > 0)
			air_upgrades.erase(std::remove(air_upgrades.begin(), air_upgrades.end(), cyber->orders[0].ability_id), air_upgrades.end());
		else
			idle_cybers.push_back(cyber);
	}
	if (idle_cybers.size() > 0 && air_upgrades.size() > 0)
	{
		for (const auto &cyber : idle_cybers)
		{
			mediator->SetUnitCommand(cyber, air_upgrades[0], 0);
			air_upgrades.erase(air_upgrades.begin());
			if (air_upgrades.size() == 0)
				break;
		}
	}

#ifdef DEBUG_TIMING
	unsigned long long end_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	std::ofstream get_upgrades;
	get_upgrades.open("get_upgrades.txt", std::ios_base::app);

	get_upgrades << end_time - start_time << "\n";
	get_upgrades.close();
#endif

	return false;
}

bool ActionManager::ActionContinueChronos(ActionArgData* data)
{
#ifdef DEBUG_TIMING
	unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
#endif

	Units need_chrono;
	for (const auto &building : mediator->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_ROBOTICSBAY)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}
	for (const auto &building : mediator->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}
	for (const auto &building : mediator->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_STARGATE)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}
	for (const auto &building : mediator->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_FORGE)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}

	for (const auto &nexus : mediator->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
	{
		if (need_chrono.size() == 0)
			break;
		if (nexus->energy >= 50 && nexus->build_progress == 1)
		{
			mediator->SetUnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, need_chrono[0], 0);
			need_chrono.erase(need_chrono.begin());
		}
	}

#ifdef DEBUG_TIMING
	unsigned long long end_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	std::ofstream chronoing;
	chronoing.open("chronoing.txt", std::ios_base::app);

	chronoing << end_time - start_time << "\n";
	chronoing.close();
#endif

	return false;
}

bool ActionManager::ActionContinueExpanding(ActionArgData* data)
{
#ifdef DEBUG_TIMING
	unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
#endif

	if (mediator->NumFar3rdWorkers() > 0)
	{
		for (const auto &nexus : mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
		{
			if (nexus->build_progress < 1)
				return false;
		}
		for (const auto &action : active_actions)
		{
			if (action->action == &ActionManager::ActionBuildBuilding && action->action_arg->unitId == UNIT_TYPEID::PROTOSS_NEXUS)
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
			mediator->build_order_manager.BuildBuilding(ASSIMILATOR);
		}
		else
		{
			mediator->build_order_manager.BuildBuilding(UNIT_TYPEID::PROTOSS_NEXUS);
		}
		return false;
	}

#ifdef DEBUG_TIMING
	unsigned long long end_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	std::ofstream expanding;
	expanding.open("expanding.txt", std::ios_base::app);

	expanding << end_time - start_time << "\n";
	expanding.close();
#endif
	return false;
}

#pragma warning(pop)

bool ActionManager::ActionChronoTillFinished(ActionArgData* data)
{
	if (mediator->GetGameLoop() % 2 == 0) // TODO avoid double chronoing a building
		return false;

	const Unit* building = data->unit;
	if (building->orders.size() == 0)
	{
		return true;
	}
	for (const auto &buff : building->buffs)
	{
		if (buff == BUFF_ID::CHRONOBOOSTENERGYCOST)
			return false;
	}
	for (const auto &nexus : mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
	{
		if (nexus->energy >= 50 && nexus->build_progress == 1)
		{
			mediator->SetUnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building, 0);
			return false;
		}
		/*for (const auto &ability : mediator->Query()->GetAbilitiesForUnit(nexus).abilities)
		{
			if (ability.ability_id == ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST)
			{
				mediator->SetUnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building, 0);
				return false;
			}
		}*/
	}
	return false;
}

bool ActionManager::ActionConstantChrono(ActionArgData* data)
{
#ifdef DEBUG_TIMING
	unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
#endif

	const Unit* building = data->unit;
	for (const auto &buff : building->buffs)
	{
		if (buff == BUFF_ID::CHRONOBOOSTENERGYCOST)
		{

#ifdef DEBUG_TIMING
			unsigned long long end_time = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				).count();

			std::ofstream constant_chrono;
			constant_chrono.open("constant_chrono.txt", std::ios_base::app);

			constant_chrono << end_time - start_time << "\n";
			constant_chrono.close();
#endif
			return false;
		}
	}

#ifdef DEBUG_TIMING
	unsigned long long end_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	std::ofstream constant_chrono;
	constant_chrono.open("constant_chrono.txt", std::ios_base::app);

	constant_chrono << end_time - start_time << "\n";
	constant_chrono.close();
#endif
	return false;
}

bool ActionManager::ActionTrainFromProxyRobo(ActionArgData* data)
{
	const Unit * robo = data->unit;
	int num_prisms = (int)mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_WARPPRISM)).size();
	int num_obs = (int)mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_OBSERVER)).size();

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
	Units workers = mediator->GetAllWorkersOnGas();

	for (const auto &worker : workers)
	{
		mediator->RemoveWorker(worker);
		mediator->PlaceWorker(worker);
	}
	if (mediator->GetAllWorkersOnGas().size() == 0)
		return true;
	return false;
}
#pragma warning(pop)

bool ActionManager::ActionRemoveScoutToProxy(ActionArgData* data)
{
	bool pylon_placed = false;
	bool pylon_finished = false;
	for (const auto &pylon : mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_PYLON)))
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
		mediator->SetUnitCommand(scout, ABILITY_ID::GENERAL_MOVE, data->position, 0);
	}
	else if (Distance2D(scout->pos, data->position) < 5 && !pylon_placed)
	{
		if (mediator->GetGameLoop() / FRAME_TIME >= data->index)
		{
			mediator->SetUnitCommand(scout, ABILITY_ID::BUILD_PYLON, data->position, 0);
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
					mediator->SetUnitCommand(scout, ABILITY_ID::SMART, base_minerals, 1);
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
				mediator->SetUnitCommand(scout, ABILITY_ID::GENERAL_MOVE, run_away_pos, 1);
			}
		}
	}
	else if (pylon_placed)
	{
		if (data->unitId == UNIT_TYPEID::PROTOSS_PYLON)
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
					mediator->SetUnitCommand(scout, ABILITY_ID::SMART, base_minerals, 1);
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
				mediator->SetUnitCommand(scout, ABILITY_ID::GENERAL_MOVE, run_away_pos, 1);
			}
		}
	}
	return false;
}

#pragma warning(push)
#pragma warning(disable : 4100)
bool ActionManager::ActionDTHarassTerran(ActionArgData* data)
{
	/*for (const auto &unit : mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_DARKTEMPLAR)))
	{
		// if outside -> move into enemy main
		if ((unit->pos.z + .1 < mediator->ToPoint3D(mediator->GetStartLocation()).z || unit->pos.z - .1 > mediator->ToPoint3D(mediator->GetStartLocation()).z) && unit->orders.size() == 0)
		{
			mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, mediator->locations->initial_scout_pos, 0);
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
	for (const auto &robo : mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
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

			for (const auto &nexus : mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
			{
				if (nexus->energy >= 50 && nexus->build_progress == 1)
					mediator->SetUnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, robo, 0);
				/*for (const auto &ability : mediator->Query()->GetAbilitiesForUnit(nexus).abilities)
				{
					if (ability.ability_id == ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST)
					{
						mediator->SetUnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, robo, 0);
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
			mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK, pos, 0);
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
		mediator->SetUnitCommand(data->unit, ABILITY_ID::GENERAL_MOVE, pos, 0);

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
		mediator->SetUnitCommand(data->unit, ABILITY_ID::GENERAL_MOVE, pos, 0);

	return false;
}

bool ActionManager::ActionCheckForBunkerRush(ActionArgData* data)
{
	if (mediator->GetCurrentTime() > 120 && Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), mediator->GetNaturalLocation()) > 15)
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
		if (scv == nullptr || Distance2D(scv->pos, data->unit->pos) > 10)
			target = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(BUNKER)), data->unit->pos);
		else
			target = scv;
	}

	if (target)
	{
		if (data->unit->weapon_cooldown == 0)
			mediator->SetUnitCommand(data->unit, ABILITY_ID::ATTACK, target, 0);
		else
			mediator->SetUnitCommand(data->unit, ABILITY_ID::GENERAL_MOVE, target, 0);
	}
	else
	{
		if (data->unit->weapon_cooldown == 0 && Utility::GetUnitsInRange(mediator->GetUnits(Unit::Alliance::Enemy), data->unit, 0).size() > 0)
		{
			mediator->SetUnitCommand(data->unit, ABILITY_ID::ATTACK, Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), data->unit->pos), 0);
		}
		else if (data->unit->orders.size() == 0 || data->unit->orders[0].ability_id == ABILITY_ID::ATTACK || Distance2D(data->unit->pos, mediator->GetNaturalLocation()) > 15)
		{
			Point2D pos = Utility::FurthestFrom(mediator->GetLocations().natural_front, data->unit->pos);
			mediator->SetUnitCommand(data->unit, ABILITY_ID::GENERAL_MOVE, pos, 0);
		}
	}

	return false;
}

}