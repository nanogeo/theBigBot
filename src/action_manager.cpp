
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
	UNIT_TYPEID buildingId = data->unitId;
	Point2D pos = data->position;
	const Unit *builder = data->unit;
	if (builder->is_alive == false)
	{
		// builder died
		mediator->RebuildBuilding(data->position, data->unitId);
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
	if (Distance2D(builder->pos, pos) < Utility::BuildingSize(buildingId) + 1 && mediator->CanBuildBuilding(buildingId))
	{
		if (buildingId == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
		{
			std::vector<UNIT_TYPEID> gas_types = { UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER };
			const Unit *gas = Utility::ClosestTo(mediator->GetUnits(IsUnits(gas_types)), pos);
			mediator->SetUnitCommand(builder, ABILITY_ID::BUILD_ASSIMILATOR, gas, 0);
		}
		else
		{
			mediator->SetUnitCommand(builder, Utility::GetBuildAbility(buildingId), pos, 0);
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
	if (Utility::DistanceToClosest(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), data->position) > 10 &&
		(data->unitId == PYLON || data->unitId == NEXUS || Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Self, IsUnit(PYLON)), data->position) < 6.5)) // TODO add CanAfford?
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
	if (Distance2D(builder->pos, pos) < Utility::BuildingSize(buildingId) + 1 && mediator->CanBuildBuilding(buildingId))
	{
		if (buildingId == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
		{
			std::vector<UNIT_TYPEID> gas_types = { UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER };
			const Unit *gas = Utility::ClosestTo(mediator->GetUnits(IsUnits(gas_types)), pos);
			mediator->SetUnitCommand(builder, ABILITY_ID::BUILD_ASSIMILATOR, gas, 0);
		}
		else
		{
			mediator->SetUnitCommand(builder, Utility::GetBuildAbility(buildingId), pos, 0);
		}
	}
	else if (Distance2D(builder->pos, pos) > Utility::BuildingSize(buildingId))
	{
		mediator->SetUnitCommand(builder, ABILITY_ID::GENERAL_MOVE, pos, 0);
	}
	return false;
}

bool ActionManager::ActionBuildProxyMulti(ActionArgData* data)
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
	if (Distance2D(builder->pos, pos) < Utility::BuildingSize(buildingId) + 1 && mediator->CanBuildBuilding(buildingId))
	{
		if (buildingId == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
		{
			std::vector<UNIT_TYPEID> gas_types = { UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER };
			const Unit *gas = Utility::ClosestTo(mediator->GetUnits(IsUnits(gas_types)), pos);
			mediator->SetUnitCommand(builder, ABILITY_ID::BUILD_ASSIMILATOR, gas, 0);
		}
		else
		{
			mediator->SetUnitCommand(builder, Utility::GetBuildAbility(buildingId), pos, 0);
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
		if (num_prisms == 0 && mediator->CanAfford(UNIT_TYPEID::PROTOSS_WARPPRISM, 1))
			mediator->SetUnitCommand(robo, ABILITY_ID::TRAIN_WARPPRISM, 0);
		else if (num_obs == 0 && mediator->CanAfford(UNIT_TYPEID::PROTOSS_OBSERVER, 1))
			mediator->SetUnitCommand(robo, ABILITY_ID::TRAIN_OBSERVER, 0);
		else if (mediator->CanAfford(UNIT_TYPEID::PROTOSS_IMMORTAL, 1))
			mediator->SetUnitCommand(robo, ABILITY_ID::TRAIN_IMMORTAL, 0);
	}
	return false;
}

bool ActionManager::ActionZealotDoubleprong(ActionArgData* data)
{
	ArmyGroup* army = data->army_group;

	for (const auto& unit : army->new_units)
	{
		if (unit->orders.size() == 0)
		{
			mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, army->attack_path[0], 0);
		}
		if (Distance2D(unit->pos, army->attack_path[0]) < 2)
		{
			army->AddUnit(unit);
		}
	}
	if (army->zealots.size() > 10)
	{
		for (const auto& unit : army->zealots)
		{
			if (unit->orders.size() == 0)
			{
				for (const auto& point : army->attack_path)
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, point, 0, true);
				}
			}
		}
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
	else if (Distance2D(scout->pos, data->position) < 1 && !pylon_placed && mediator->GetGameLoop() / FRAME_TIME >= data->index)
	{
		mediator->SetUnitCommand(scout, ABILITY_ID::BUILD_PYLON, data->position, 0);
	}
	else if (pylon_placed)
	{
		if (data->unitId == UNIT_TYPEID::PROTOSS_PYLON)
		{
			mediator->PlaceWorker(data->unit);
			return true;
		}
		if (pylon_finished)
		{
			std::vector<Point2D> building_locations = mediator->GetProxyLocations(data->unitId);
			Point2D pos = building_locations[0];
			active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(scout, data->unitId, pos)));
			return true;
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
				if (mediator->CanAfford(UNIT_TYPEID::PROTOSS_IMMORTAL, 1))
					mediator->SetUnitCommand(robo, ABILITY_ID::TRAIN_IMMORTAL, 0);
			}
			else if (mediator->CanAfford(data->unitIds[0], 1))
			{
				mediator->SetUnitCommand(robo, Utility::GetTrainAbility(data->unitIds[0]), 0);
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

bool ActionManager::ActionAllInAttack(ActionArgData* data)
{
	ArmyGroup* army = data->army_group;
	Units stalkers = army->stalkers;

	for (int i = 0; i < army->new_units.size(); i++)
	{
		const Unit* unit = army->new_units[i];
		if (unit->orders.size() == 0 || unit->orders[0].ability_id == ABILITY_ID::BUILD_INTERCEPTORS)
		{
			if (army->stalkers.size() > 0)
			{
				if (unit->unit_type == PRISM)
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(army->stalkers), 0, true);
				else
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, Utility::MedianCenter(army->stalkers), 0, true);
			}
			else
			{
				if (unit->unit_type == PRISM)
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, army->attack_path[0], 0, true);
				else
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, army->attack_path[0], 0, true);
			}
		}
		if ((army->stalkers.size() > 0 && Distance2D(unit->pos, Utility::MedianCenter(army->stalkers)) < 5) || (army->stalkers.size() == 0 && Distance2D(unit->pos, army->attack_path[0]) < 2))
		{
			army->AddUnit(unit);
			i--;
		}
	}

	/*for (int i = 0; i < army->new_units.size(); i++)
	{
		const Unit* unit = army->new_units[i];
		if (unit->unit_type == UNIT_TYPEID::PROTOSS_WARPPRISM)
		{
			army->AddUnit(unit);
			i--;
			continue;
		}
		if (unit->orders.size() == 0)
		{
			Point2D closest = Utility::ClosestTo(army->attack_path, unit->pos);

			for (int i = find(army->attack_path.begin(), army->attack_path.end(), closest) - army->attack_path.begin(); i < army->attack_path.size(); i++)
			{
				mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, army->attack_path[i], 0, true);
			}
		}
		if ((army->stalkers.size() > 0 && Distance2D(unit->pos, Utility::MedianCenter(army->stalkers)) < 5) || (army->stalkers.size() == 0)
		{
			army->AddUnit(unit);
			i--;
		}
	}*/

	/*for (const auto& pos : mediator->locations->attack_path_line.GetPoints())
	{
		mediator->Debug()->DebugSphereOut(mediator->ToPoint3D(pos), .5, Color(255, 255, 255));
	}*/

	army->AttackLine(0, 6, PROTOSS_PRIO);

	/*if (army->stalkers.size() == 0)
		return false;

	Units enemies = mediator->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
	Point2D stalkers_center = Utility::MedianCenter(army->stalkers);
	Point2D stalker_line_pos = mediator->locations->attack_path_line.FindClosestPoint(stalkers_center);

	Units close_enemies = Utility::NClosestUnits(enemies, stalkers_center, 5);
	// remove far enemies
	for (int i = 0; i < close_enemies.size(); i++)
	{
		if (Distance2D(stalkers_center, close_enemies[i]->pos) > 12)
		{
			close_enemies.erase(close_enemies.begin() + i);
			i--;
		}
	}

	Point2D concave_target = mediator->locations->attack_path_line.GetPointFrom(stalker_line_pos, 8, true);
	float max_range = 7;
	if (close_enemies.size() > 0)
	{
		concave_target = Utility::MedianCenter(close_enemies);
		max_range = std::max(Utility::GetMaxRange(close_enemies) + 2, 6.0f);
	}

	mediator->Debug()->DebugSphereOut(mediator->ToPoint3D(concave_target), 1, Color(255, 255, 0));


	Point2D retreat_concave_origin = mediator->locations->attack_path_line.GetPointFrom(concave_target, max_range, false);
	if (retreat_concave_origin == Point2D(0, 0))
		retreat_concave_origin = mediator->locations->attack_path_line.GetPointFrom(stalker_line_pos, 2 * .625, false);

	Point2D attack_concave_origin = mediator->locations->attack_path_line.GetPointFrom(stalker_line_pos, 2 * .625, true);


	std::vector<Point2D> attack_concave_positions = army->FindConcaveFromBack(attack_concave_origin, (2 * attack_concave_origin) - concave_target, army->stalkers.size(), .625, 0);
	std::vector<Point2D> retreat_concave_positions = army->FindConcave(retreat_concave_origin, (2 * retreat_concave_origin) - concave_target, army->stalkers.size(), .625, 0, 30);

	std::map<const Unit*, Point2D> attack_unit_positions = army->AssignUnitsToPositions(army->stalkers, attack_concave_positions);
	std::map<const Unit*, Point2D> retreat_unit_positions = army->AssignUnitsToPositions(army->stalkers, retreat_concave_positions);

	for (const auto& pos : attack_concave_positions)
	{
		mediator->Debug()->DebugSphereOut(mediator->ToPoint3D(pos), .625, Color(255, 0, 0));
	}
	for (const auto& pos : retreat_concave_positions)
	{
		mediator->Debug()->DebugSphereOut(mediator->ToPoint3D(pos), .625, Color(0, 255, 0));
	}

	army->ApplyPressureGrouped(concave_target, (2 * retreat_concave_origin) - concave_target, retreat_unit_positions, attack_unit_positions);



	if (army->current_attack_index > 2 && Distance2D(Utility::Center(stalkers), retreat_point) < 3)
		army->current_attack_index--;
	if (army->current_attack_index < army->attack_path.size() - 1 && Distance2D(Utility::MedianCenter(stalkers), attack_point) < 3)
	{
		//if (obs_in_position)
			army->current_attack_index++;
		//else
		//	army->current_attack_index = std::min(army->current_attack_index + 1, army->high_ground_index - 1);
	}*/

	return false;
}

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


bool ActionManager::ActionAttackLine(ActionArgData* data)
{
	ArmyGroup* army = data->army_group;
	army->AttackLine(0, 6, ZERG_PRIO);
	return false;
}

}