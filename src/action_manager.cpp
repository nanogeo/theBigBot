
#include "theBigBot.h"
#include "action_manager.h"
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

bool ActionManager::ActionBuildBuilding(ActionArgData* data)
{
	UNIT_TYPEID buildingId = data->unitId;
	Point2D pos = data->position;
	const Unit *builder = data->unit;
	for (const auto &building : agent->Observation()->GetUnits(IsFriendlyUnit(buildingId)))
	{
		if (Distance2D(building->pos, pos) < 1 && building->display_type != Unit::DisplayType::Placeholder)
		{
			agent->worker_manager.PlaceWorker(builder);
			// finished buildings.remove building.tag
			return true;
		}
	}
	if (Distance2D(builder->pos, pos) < Utility::BuildingSize(buildingId) + 1 && Utility::CanBuildBuilding(buildingId, agent->Observation()))
	{
		if (buildingId == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
		{
			std::vector<UNIT_TYPEID> gas_types = { UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER };
			const Unit *gas = Utility::ClosestTo(agent->Observation()->GetUnits(IsUnits(gas_types)), pos);
			agent->Actions()->UnitCommand(builder, ABILITY_ID::BUILD_ASSIMILATOR, gas);
		}
		else
		{
			agent->Actions()->UnitCommand(builder, Utility::GetBuildAbility(buildingId), pos);
		}
	}
	else if (Distance2D(builder->pos, pos) > Utility::BuildingSize(buildingId))
	{
		agent->Actions()->UnitCommand(builder, ABILITY_ID::MOVE_MOVE, pos);
	}
	return false;
}

bool ActionManager::ActionBuildBuildingMulti(ActionArgData* data)
{
	UNIT_TYPEID buildingId = data->unitIds[data->index];
	Point2D pos = data->position;
	const Unit *builder = data->unit;
	for (const auto &building : agent->Observation()->GetUnits(IsFriendlyUnit(buildingId)))
	{
		if (Distance2D(building->pos, pos) < 1 && building->display_type != Unit::DisplayType::Placeholder)
		{
			// finished buildings.remove building.tag
			data->index++;
			if (data->index < data->unitIds.size())
			{
				data->position = agent->GetLocation(data->unitIds[data->index]);
				active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuildingMulti, data));
			}
			else
			{
				agent->worker_manager.PlaceWorker(builder);
			}
			return true;
		}
	}
	if (Distance2D(builder->pos, pos) < Utility::BuildingSize(buildingId) + 1 && Utility::CanBuildBuilding(buildingId, agent->Observation()))
	{
		if (buildingId == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
		{
			std::vector<UNIT_TYPEID> gas_types = { UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER };
			const Unit *gas = Utility::ClosestTo(agent->Observation()->GetUnits(IsUnits(gas_types)), pos);
			agent->Actions()->UnitCommand(builder, ABILITY_ID::BUILD_ASSIMILATOR, gas);
		}
		else
		{
			agent->Actions()->UnitCommand(builder, Utility::GetBuildAbility(buildingId), pos);
		}
	}
	else if (Distance2D(builder->pos, pos) > Utility::BuildingSize(buildingId))
	{
		agent->Actions()->UnitCommand(builder, ABILITY_ID::GENERAL_MOVE, pos);
	}
	return false;
}

bool ActionManager::ActionBuildProxyMulti(ActionArgData* data)
{
	UNIT_TYPEID buildingId = data->unitIds[data->index];
	Point2D pos = data->position;
	const Unit *builder = data->unit;
	for (const auto &building : agent->Observation()->GetUnits(IsFriendlyUnit(buildingId)))
	{
		if (Point2D(building->pos) == pos && building->display_type != Unit::DisplayType::Placeholder)
		{
			// finished buildings.remove building.tag
			data->index++;
			if (data->index < data->unitIds.size())
			{
				data->position = agent->GetProxyLocation(data->unitIds[data->index]);
				active_actions.push_back(new ActionData(&ActionManager::ActionBuildProxyMulti, data));
			}
			else
			{
				agent->worker_manager.PlaceWorker(builder);
			}
			return true;
		}
	}
	if (Distance2D(builder->pos, pos) < Utility::BuildingSize(buildingId) + 1 && Utility::CanBuildBuilding(buildingId, agent->Observation()))
	{
		if (buildingId == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
		{
			std::vector<UNIT_TYPEID> gas_types = { UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER, UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER };
			const Unit *gas = Utility::ClosestTo(agent->Observation()->GetUnits(IsUnits(gas_types)), pos);
			agent->Actions()->UnitCommand(builder, ABILITY_ID::BUILD_ASSIMILATOR, gas);
		}
		else
		{
			agent->Actions()->UnitCommand(builder, Utility::GetBuildAbility(buildingId), pos);
		}
	}
	else if (Distance2D(builder->pos, pos) > Utility::BuildingSize(buildingId))
	{
		agent->Actions()->UnitCommand(builder, ABILITY_ID::GENERAL_MOVE, pos);
	}
	return false;
}

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

	int extra_workers = data->index;
	int num_workers = agent->Observation()->GetFoodWorkers();
	int num_nexi = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)).size();
	int num_assimilators = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ASSIMILATOR)).size();
	if (num_workers >= std::min(num_nexi * 16 + num_assimilators * 3, 70) + extra_workers)
	{
		agent->worker_manager.should_build_workers = false;
	}
	else
	{
		agent->worker_manager.should_build_workers = true;
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

	int supply_used = agent->Observation()->GetFoodUsed();
	int supply_cap = agent->Observation()->GetFoodCap() + 8 * (build_pylon_actions - agent->extra_pylons);

	for (const auto &building : agent->Observation()->GetUnits(Unit::Alliance::Self))
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
	for (const auto &pylon : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_PYLON)))
	{
		if (pylon->build_progress < 1)
			pending_pylons++;
	}
	int supply_used = agent->Observation()->GetFoodUsed();
	int supply_cap = agent->Observation()->GetFoodCap() - 8 * agent->extra_pylons;
	if (supply_cap >= 200)
		return false;
	supply_cap += 8 * (build_pylon_actions + pending_pylons);
	supply_used += agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)).size();
	supply_used += 2 * agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_WARPGATE)).size();
	supply_used += 2 * agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_GATEWAY)).size();
	supply_used += 3 * agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)).size();
	supply_used += 3 * agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_STARGATE)).size();
	*/
	if (supply_used >= supply_cap)
		agent->build_order_manager.BuildBuilding(UNIT_TYPEID::PROTOSS_PYLON);

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
	if (agent->upgrade_shields < 3)
		upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSSHIELDS);
	if (agent->upgrade_ground_weapon < 3)
		upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS);
	if (agent->upgrade_ground_armor < 3)
		upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR);

	Units idle_forges;
	for (const auto &forge : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_FORGE)))
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
			agent->Actions()->UnitCommand(forge, upgrades[0]);
			upgrades.erase(upgrades.begin());
		}
	}

	std::vector<ABILITY_ID> air_upgrades = {};
	if (agent->upgrade_air_weapon < 3)
		air_upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONS);
	if (agent->upgrade_air_armor < 3)
		air_upgrades.push_back(ABILITY_ID::RESEARCH_PROTOSSAIRARMOR);

	Units idle_cybers;
	for (const auto &cyber : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)))
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
			agent->Actions()->UnitCommand(cyber, air_upgrades[0]);
			air_upgrades.erase(air_upgrades.begin());
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
	for (const auto &building : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_ROBOTICSBAY)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}
	for (const auto &building : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}
	for (const auto &building : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_STARGATE)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}
	for (const auto &building : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_FORGE)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
			need_chrono.push_back(building);
	}

	for (const auto &nexus : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
	{
		if (need_chrono.size() == 0)
			break;
		if (nexus->energy >= 50 && nexus->build_progress == 1)
		{
			agent->Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, need_chrono[0]);
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

	if (agent->worker_manager.far_3_mineral_patch_extras.size() > 0)
	{
		for (const auto &pylon : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
		{
			if (pylon->build_progress < 1)
				return false;
		}
		for (const auto &action : active_actions)
		{
			if (action->action == &ActionManager::ActionBuildBuilding && action->action_arg->unitId == UNIT_TYPEID::PROTOSS_NEXUS)
			{
				return false;
			}
		}

		agent->build_order_manager.BuildBuilding(UNIT_TYPEID::PROTOSS_NEXUS);
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
}

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
	for (const auto &nexus : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
	{
		if (nexus->energy >= 50 && nexus->build_progress == 1)
		{
			agent->Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building);
			return false;
		}
		/*for (const auto &ability : agent->Query()->GetAbilitiesForUnit(nexus).abilities)
		{
			if (ability.ability_id == ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST)
			{
				agent->Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building);
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
}

bool ActionManager::ActionWarpInAtProxy(ActionArgData* data)
{
	std::vector<Point2D> possible_spots = agent->FindProxyWarpInSpots();
	if (possible_spots.size() == 0)
		return false;
	for (const auto &warpgate : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE)))
	{
		if (agent->warpgate_status[warpgate].frame_ready == 0)
		{
			agent->Actions()->UnitCommand(warpgate, ABILITY_ID::TRAINWARP_STALKER, possible_spots.back());
			agent->warpgate_status[warpgate].used = true;
			agent->warpgate_status[warpgate].frame_ready = agent->Observation()->GetGameLoop() + round(23 * 22.4);
			possible_spots.pop_back();
		}
		if (possible_spots.size() == 0)
			break;
	}
	return false;
}

bool ActionManager::ActionTrainFromProxyRobo(ActionArgData* data)
{
	const Unit * robo = data->unit;
	int num_prisms = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_WARPPRISM)).size();
	int num_obs = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_OBSERVER)).size();

	if (robo->build_progress == 1 && robo->orders.size() == 0)
	{
		if (num_prisms == 0 && Utility::CanAfford(UNIT_TYPEID::PROTOSS_WARPPRISM, 1, agent->Observation()))
			agent->Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_WARPPRISM);
		else if (num_obs == 0 && Utility::CanAfford(UNIT_TYPEID::PROTOSS_OBSERVER, 1, agent->Observation()))
			agent->Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_OBSERVER);
		else if (Utility::CanAfford(UNIT_TYPEID::PROTOSS_IMMORTAL, 1, agent->Observation()))
			agent->Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_IMMORTAL);
	}
	return false;
}

bool ActionManager::ActionContain(ActionArgData* data)
{
	ArmyGroup* army = data->army_group;
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
			agent->ObserveAttackPath(observers, retreat_point, attack_point);
		}
		if (prisms.size() > 0)
		{
			agent->StalkerAttackTowardsWithPrism(stalkers, prisms, retreat_point, attack_point, obs_in_position);
			if (immortals.size() > 0)
				agent->ImmortalAttackTowardsWithPrism(immortals, prisms, retreat_point, attack_point, obs_in_position);
		}
		else
		{
			agent->StalkerAttackTowards(stalkers, retreat_point, attack_point, obs_in_position);
			if (immortals.size() > 0)
				agent->ImmortalAttackTowards(immortals, retreat_point, attack_point, obs_in_position);
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

	return false;
}

bool ActionManager::ActionStalkerOraclePressure(ActionArgData* data)
{
#ifdef DEBUG_TIMING
	unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();


	unsigned long long new_units = 0;
	unsigned long long critical_points = 0;
	unsigned long long close_targets = 0;
	unsigned long long concave_origins = 0;
	unsigned long long positions = 0;
	unsigned long long apply_pressure = 0;
#endif

	float unit_size = .625;
	float unit_dispersion = 0;

	ArmyGroup* army = data->army_group;


	for (int i = 0; i < army->new_units.size(); i++)
	{
		const Unit* unit = army->new_units[i];
		if (unit->orders.size() == 0 || unit->orders[0].ability_id == ABILITY_ID::BUILD_INTERCEPTORS)
		{
			for (const auto &point : army->attack_path)
			{
				agent->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, point, true);
			}
		}
		if ((army->stalkers.size() > 0 && Distance2D(unit->pos, Utility::MedianCenter(army->stalkers)) < 5) || (army->stalkers.size() == 0 && Distance2D(unit->pos, army->attack_path[0]) < 2))
		{
			army->AddUnit(unit);
			i--;
		}
	}

#ifdef DEBUG_TIMING
	new_units = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
#endif

	////agent->Debug()->DebugSphereOut(Point3D(fallback_point.x, fallback_point.y, agent->Observation()->TerrainHeight(fallback_point)), 3, Color(255, 0, 0));
	////agent->Debug()->DebugSphereOut(Point3D(attack_point.x, attack_point.y, agent->Observation()->TerrainHeight(attack_point)), 3, Color(0, 255, 0));

	for (const auto &pos : agent->locations->attack_path_line.GetPoints())
	{
		//agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .5, Color(255, 255, 255));
	}

	army->AttackLine(0, 6, ZERG_PRIO);

	return false;
}

bool ActionManager::ActionZealotDoubleprong(ActionArgData* data)
{
	ArmyGroup* army = data->army_group;

	for (const auto &unit : army->new_units)
	{
		if (unit->orders.size() == 0)
		{
			agent->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, army->attack_path[0]);
		}
		if (Distance2D(unit->pos, army->attack_path[0]) < 2)
		{
			army->AddUnit(unit);
		}
	}
	if (army->zealots.size() > 10)
	{
		for (const auto &unit : army->zealots)
		{
			if (unit->orders.size() == 0)
			{
				for (const auto &point : army->attack_path)
				{
					agent->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, point, true);
				}
			}
		}
	}
	return false;
}

bool ActionManager::ActionContinueWarpingInStalkers(ActionArgData* data)
{
	if (agent->Observation()->GetGameLoop() % 2 == 0)
		return false;
	Units gates_ready;
	Units gates = agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
	for (const auto &warpgate : gates)
	{
		if (agent->warpgate_status[warpgate].frame_ready == 0)
		{
			gates_ready.push_back(warpgate);
		}
	}

	if (gates_ready.size() > 0)
	{
		int max_warpins = std::min(int(gates_ready.size()), Utility::MaxCanAfford(UNIT_TYPEID::PROTOSS_STALKER, gates_ready.size(), agent->Observation()));
		if (max_warpins == 0)
			return false;
		std::vector<Point2D> spots = agent->FindWarpInSpots(agent->Observation()->GetGameInfo().enemy_start_locations[0], max_warpins);
		//std::cout << "spots " << spots.size() << "\n";
		for (int i = 0; i < spots.size(); i++)
		{
			//std::cout << "warp in at " << spots[i].x << ", " << spots[i].y << "\n";
			agent->Actions()->UnitCommand(gates_ready[i], ABILITY_ID::TRAINWARP_STALKER, spots[i]);
			agent->warpgate_status[gates_ready[i]].used = true;
			agent->warpgate_status[gates_ready[i]].frame_ready = agent->Observation()->GetGameLoop() + round(23 * 22.4);
		}
	}
	return false;
}

bool ActionManager::ActionContinueVolleyWarpingInStalkers(ActionArgData* data)
{
	bool all_gates_ready = true;
	Units gates = agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
	for (const auto &warpgate : gates)
	{
		if (agent->warpgate_status[warpgate].frame_ready > 0)
		{
			all_gates_ready = false;
			break;
		}
	}
	if (all_gates_ready && Utility::CanAfford(UNIT_TYPEID::PROTOSS_STALKER, gates.size(), agent->Observation()))
	{
		//std::cout << "warp in stalkers/n";
		//std::cout << "all gates ready " << all_gates_ready << "\n";

		std::vector<Point2D> spots = agent->FindWarpInSpots(agent->Observation()->GetGameInfo().enemy_start_locations[0], gates.size());
		//std::cout << "spots " << spots.size() << "\n";
		if (spots.size() >= gates.size())
		{
			for (int i = 0; i < gates.size(); i++)
			{
				//std::cout << "warp in at " << spots[i].x << ", " << spots[i].y << "\n";
				agent->Actions()->UnitCommand(gates[i], ABILITY_ID::TRAINWARP_STALKER, spots[i]);
				agent->warpgate_status[gates[i]].used = true;
				agent->warpgate_status[gates[i]].frame_ready = agent->Observation()->GetGameLoop() + round(23 * 22.4);
			}
		}
	}
	return false;
}

bool ActionManager::ActionContinueVolleyWarpingInZealots(ActionArgData* data)
{
#ifdef DEBUG_TIMING
	unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
#endif

	bool all_gates_ready = true;
	//if (Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_GATEWAY)).size() > 0)
	//    return false;
	Units gates = agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
	for (const auto &warpgate : gates)
	{
		if (agent->warpgate_status[warpgate].frame_ready > 0)
		{
			all_gates_ready = false;
			break;
		}
	}
#ifdef DEBUG_TIMING
	unsigned long long get_abilities = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
#endif

	if (gates.size() > 0 && all_gates_ready && Utility::CanAfford(UNIT_TYPEID::PROTOSS_ZEALOT, gates.size(), agent->Observation()))
	{
		std::vector<Point2D> spots = agent->FindWarpInSpots(agent->Observation()->GetGameInfo().enemy_start_locations[0], gates.size());
		if (spots.size() >= gates.size())
		{
			for (int i = 0; i < gates.size(); i++)
			{
				Point3D pos = Point3D(gates[i]->pos.x, gates[i]->pos.y, agent->Observation()->TerrainHeight(gates[i]->pos));
				//agent->Debug()->DebugSphereOut(pos, 1, Color(255, 0, 255));
				agent->Actions()->UnitCommand(gates[i], ABILITY_ID::TRAINWARP_ZEALOT, spots[i]);
				agent->warpgate_status[gates[i]].used = true;
				agent->warpgate_status[gates[i]].frame_ready = agent->Observation()->GetGameLoop() + round(20 * 22.4);

			}
		}
	}
#ifdef DEBUG_TIMING
	unsigned long long end_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	std::ofstream zealot_warp;
	zealot_warp.open("zealot_warp.txt", std::ios_base::app);

	zealot_warp << get_abilities - start_time << ", ";
	zealot_warp << end_time - get_abilities << "\n";
	zealot_warp.close();
#endif
	return false;
}

bool ActionManager::ActionContinueBuildingCarriers(ActionArgData* data)
{
	for (const auto &stargate : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_STARGATE)))
	{
		if (stargate->orders.size() == 0 && Utility::CanAfford(UNIT_TYPEID::PROTOSS_CARRIER, 1, agent->Observation()))
		{
			agent->Actions()->UnitCommand(stargate, ABILITY_ID::TRAIN_CARRIER);
		}
	}
	return false;
}

bool ActionManager::ActionPullOutOfGas(ActionArgData* data)
{
	Units workers;
	for (const auto &data : agent->worker_manager.assimilators_reversed)
	{
		workers.push_back(data.first);
	}

	for (const auto &worker : workers)
	{
		agent->worker_manager.RemoveWorker(worker);
		agent->worker_manager.PlaceWorker(worker);
	}
	if (agent->worker_manager.assimilators_reversed.size() == 0)
		return true;
}

bool ActionManager::ActionRemoveScoutToProxy(ActionArgData* data)
{
	bool pylon_placed = false;
	bool pylon_finished = false;
	for (const auto &pylon : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_PYLON)))
	{
		if (Distance2D(pylon->pos, data->position) < 1)
		{
			pylon_placed = true;
			if (pylon->build_progress == 1)
				pylon_finished = true;
			break;
		}
	}
	int build_time = data->index;
	const Unit* scout = data->unit;

	if (Distance2D(scout->pos, data->position) > 1 && !pylon_placed)
	{
		agent->Actions()->UnitCommand(scout, ABILITY_ID::MOVE_MOVE, data->position);
	}
	else if (Distance2D(scout->pos, data->position) < 1 && !pylon_placed && agent->Observation()->GetGameLoop() / 22.4 >= data->index)
	{
		agent->Actions()->UnitCommand(scout, ABILITY_ID::BUILD_PYLON, data->position);
	}
	else if (pylon_placed)
	{
		if (data->unitId == UNIT_TYPEID::PROTOSS_PYLON)
		{
			agent->worker_manager.PlaceWorker(data->unit);
			return true;
		}
		if (pylon_finished)
		{
			std::vector<Point2D> building_locations = agent->GetProxyLocations(data->unitId);
			Point2D pos = building_locations[0];
			active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(scout, data->unitId, pos)));
			return true;
		}
	}
	return false;
}

bool ActionManager::ActionDTHarassTerran(ActionArgData* data)
{
	for (const auto &unit : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_DARKTEMPLAR)))
	{
		// if outside -> move into enemy main
		if ((unit->pos.z + .1 < agent->Observation()->GetStartLocation().z || unit->pos.z - .1 > agent->Observation()->GetStartLocation().z) && unit->orders.size() == 0)
		{
			agent->Actions()->UnitCommand(unit, ABILITY_ID::MOVE_MOVE, agent->locations->initial_scout_pos);
			agent->Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_SHADOWSTRIDE, Utility::PointBetween(agent->locations->initial_scout_pos, agent->Observation()->GetGameInfo().enemy_start_locations[0], 7), true);
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
	}
	return false;
}

bool ActionManager::ActionUseProxyDoubleRobo(ActionArgData* data)
{
	for (const auto &robo : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
	{
		if (robo->build_progress < 1)
			continue;
		if (robo->orders.size() == 0)
		{
			if (data->unitIds.size() == 0)
			{
				if (Utility::CanAfford(UNIT_TYPEID::PROTOSS_IMMORTAL, 1, agent->Observation()))
					agent->Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_IMMORTAL);
			}
			else if (Utility::CanAfford(data->unitIds[0], 1, agent->Observation()))
			{
				agent->Actions()->UnitCommand(robo, Utility::GetTrainAbility(data->unitIds[0]));
				data->unitIds.erase(data->unitIds.begin());
			}
		}
		else if (robo->orders[0].ability_id == ABILITY_ID::TRAIN_IMMORTAL)
		{
			if (Utility::HasBuff(robo, BUFF_ID::CHRONOBOOSTENERGYCOST))
				continue;

			for (const auto &nexus : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
			{
				if (nexus->energy >= 50 && nexus->build_progress == 1)
					agent->Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, robo);
				/*for (const auto &ability : agent->Query()->GetAbilitiesForUnit(nexus).abilities)
				{
					if (ability.ability_id == ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST)
					{
						agent->Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, robo);
					}
				}*/
			}
		}
	}
	return false;
}

bool ActionManager::ActionAllIn(ActionArgData* data)
{
	ArmyGroup* army = data->army_group;
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
			agent->ObserveAttackPath(observers, retreat_point, attack_point);
		}
		if (prisms.size() > 0)
		{
			agent->StalkerAttackTowardsWithPrism(stalkers, prisms, retreat_point, attack_point, obs_in_position);
			if (immortals.size() > 0)
				agent->ImmortalAttackTowardsWithPrism(immortals, prisms, retreat_point, attack_point, obs_in_position);
		}
		else
		{
			agent->StalkerAttackTowards(stalkers, retreat_point, attack_point, obs_in_position);
			if (immortals.size() > 0)
				agent->ImmortalAttackTowards(immortals, retreat_point, attack_point, obs_in_position);
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

	return false;
}

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
					agent->Actions()->UnitCommand(unit, ABILITY_ID::MOVE_MOVE, Utility::MedianCenter(army->stalkers), true);
				else
					agent->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, Utility::MedianCenter(army->stalkers), true);
			}
			else
			{
				if (unit->unit_type == PRISM)
					agent->Actions()->UnitCommand(unit, ABILITY_ID::MOVE_MOVE, army->attack_path[0], true);
				else
					agent->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, army->attack_path[0], true);
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
				agent->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, army->attack_path[i], true);
			}
		}
		if ((army->stalkers.size() > 0 && Distance2D(unit->pos, Utility::MedianCenter(army->stalkers)) < 5) || (army->stalkers.size() == 0)
		{
			army->AddUnit(unit);
			i--;
		}
	}*/

	for (const auto& pos : agent->locations->attack_path_line.GetPoints())
	{
		//agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .5, Color(255, 255, 255));
	}

	army->AttackLine(0, 6, PROTOSS_PRIO);

	/*if (army->stalkers.size() == 0)
		return false;

	Units enemies = agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
	Point2D stalkers_center = Utility::MedianCenter(army->stalkers);
	Point2D stalker_line_pos = agent->locations->attack_path_line.FindClosestPoint(stalkers_center);

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

	Point2D concave_target = agent->locations->attack_path_line.GetPointFrom(stalker_line_pos, 8, true);
	float max_range = 7;
	if (close_enemies.size() > 0)
	{
		concave_target = Utility::MedianCenter(close_enemies);
		max_range = std::max(Utility::GetMaxRange(close_enemies) + 2, 6.0f);
	}

	//agent->Debug()->DebugSphereOut(agent->ToPoint3D(concave_target), 1, Color(255, 255, 0));


	Point2D retreat_concave_origin = agent->locations->attack_path_line.GetPointFrom(concave_target, max_range, false);
	if (retreat_concave_origin == Point2D(0, 0))
		retreat_concave_origin = agent->locations->attack_path_line.GetPointFrom(stalker_line_pos, 2 * .625, false);

	Point2D attack_concave_origin = agent->locations->attack_path_line.GetPointFrom(stalker_line_pos, 2 * .625, true);


	std::vector<Point2D> attack_concave_positions = army->FindConcaveFromBack(attack_concave_origin, (2 * attack_concave_origin) - concave_target, army->stalkers.size(), .625, 0);
	std::vector<Point2D> retreat_concave_positions = army->FindConcave(retreat_concave_origin, (2 * retreat_concave_origin) - concave_target, army->stalkers.size(), .625, 0, 30);

	std::map<const Unit*, Point2D> attack_unit_positions = army->AssignUnitsToPositions(army->stalkers, attack_concave_positions);
	std::map<const Unit*, Point2D> retreat_unit_positions = army->AssignUnitsToPositions(army->stalkers, retreat_concave_positions);

	for (const auto& pos : attack_concave_positions)
	{
		//agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .625, Color(255, 0, 0));
	}
	for (const auto& pos : retreat_concave_positions)
	{
		//agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .625, Color(0, 255, 0));
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

bool ActionManager::ActionScourMap(ActionArgData* data)
{
	ImageData raw_map = agent->Observation()->GetGameInfo().pathing_grid;
	for (const auto& unit : agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Self)))
	{
		if (unit->orders.size() == 0)
		{
			std::srand(unit->tag + agent->Observation()->GetGameLoop());
			int x = std::rand() % raw_map.width;
			int y = std::rand() % raw_map.height;
			Point2D pos = Point2D(x, y);
			while (!unit->is_flying && !agent->Observation()->IsPathable(pos))
			{
				x = std::rand() % raw_map.width;
				y = std::rand() % raw_map.height;
				pos = Point2D(x, y);
			}
			agent->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, pos);
		}
	}
	return false;
}


bool ActionManager::ActionAttackLine(ActionArgData* data)
{
	ArmyGroup* army = data->army_group;
	army->AttackLine(0, 6, ZERG_PRIO);
	return false;
}

}