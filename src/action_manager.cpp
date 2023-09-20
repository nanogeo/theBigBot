
#include "TossBot.h"
#include "action_manager.h"
#include "locations.h"

#include <fstream>
#include <chrono>

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
	for (const auto &building : agent->Observation()->GetUnits(IsUnit(buildingId)))
	{
		if (Distance2D(Point2D(building->pos), pos) < 1 && building->display_type != Unit::DisplayType::Placeholder)
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
	for (const auto &building : agent->Observation()->GetUnits(IsUnit(buildingId)))
	{
		if (Point2D(building->pos) == pos && building->display_type != Unit::DisplayType::Placeholder)
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
	for (const auto &building : agent->Observation()->GetUnits(IsUnit(buildingId)))
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
	int num_workers = agent->Observation()->GetFoodWorkers();
	int num_nexi = agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)).size();
	int num_assimilators = agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ASSIMILATOR)).size();
	if (num_workers >= std::min(num_nexi * 16 + num_assimilators * 3, 70))
	{
		agent->worker_manager.should_build_workers = false;
	}
	else
	{
		agent->worker_manager.should_build_workers = true;
	}
	return false;
}

bool ActionManager::ActionContinueBuildingPylons(ActionArgData* data)
{
	int build_pylon_actions = 0;
	for (const auto &action : active_actions)
	{
		if (action->action == &ActionManager::ActionBuildBuilding && action->action_arg->unitId == UNIT_TYPEID::PROTOSS_PYLON)
		{
			build_pylon_actions++;
		}
	}
	int pending_pylons = 0;
	for (const auto &pylon : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_PYLON)))
	{
		if (pylon->build_progress < 1)
			pending_pylons++;
	}
	int supply_used = agent->Observation()->GetFoodUsed();
	int supply_cap = agent->Observation()->GetFoodCap() - 8 * agent->extra_pylons;
	if (supply_cap >= 200)
		return false;
	supply_cap += 8 * (build_pylon_actions + pending_pylons);
	supply_used += agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)).size();
	supply_used += 2 * agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_WARPGATE)).size();
	supply_used += 2 * agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY)).size();
	supply_used += 3 * agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)).size();
	supply_used += 3 * agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_STARGATE)).size();
	if (supply_used >= supply_cap)
		agent->build_order_manager.BuildBuilding(UNIT_TYPEID::PROTOSS_PYLON);

	return false;
}

bool ActionManager::ActionContinueUpgrades(ActionArgData* data)
{
	// TODO make global upgrade tracker
	std::vector<ABILITY_ID> upgrades = { ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS, ABILITY_ID::RESEARCH_PROTOSSSHIELDS, ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR};
	for (const auto &forge : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_FORGE)))
	{
		int upgrade_value = 9;
		if (forge->orders.size() == 0)
		{
			AvailableAbilities abilities = agent->Query()->GetAbilitiesForUnit(forge);
			for (const auto &ability : abilities.abilities)
			{
				auto found = std::find(upgrades.begin(), upgrades.end(), ability.ability_id);
				if (found != upgrades.end())
				{
					int index = found - upgrades.begin();
					if (index < upgrade_value)
						upgrade_value = index;
				}
			}
		}
		if (upgrade_value < 9)
		{
			agent->Actions()->UnitCommand(forge, upgrades[upgrade_value]);
			upgrades.erase(upgrades.begin() + upgrade_value);
		}
	}
	return false;
}

bool ActionManager::ActionContinueChronos(ActionArgData* data)
{
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
		if (nexus->energy >= 50)
		{
			agent->Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, need_chrono[0]);
			need_chrono.erase(need_chrono.begin());
		}
	}
	return false;
}

bool ActionManager::ActionContinueExpanding(ActionArgData* data)
{
	if (agent->worker_manager.far_3_mineral_patch_extras.size() > 0)
	{
		for (const auto &pylon : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
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
	for (const auto &nexus : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
	{
		for (const auto &ability : agent->Query()->GetAbilitiesForUnit(nexus).abilities)
		{
			if (ability.ability_id == ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST)
			{
				agent->Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building);
				return false;
			}
		}
	}
	return false;
}

bool ActionManager::ActionConstantChrono(ActionArgData* data)
{
	const Unit* building = data->unit;
	for (const auto &buff : building->buffs)
	{
		if (buff == BUFF_ID::CHRONOBOOSTENERGYCOST)
			return false;
	}
}

bool ActionManager::ActionWarpInAtProxy(ActionArgData* data)
{
	std::vector<Point2D> possible_spots = agent->FindProxyWarpInSpots();
	if (possible_spots.size() == 0)
		return false;
	for (const auto &warpgate : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE)))
	{
		for (const auto &ability : agent->Query()->GetAbilitiesForUnit(warpgate).abilities)
		{
			if (ability.ability_id == ABILITY_ID::TRAINWARP_ZEALOT)
			{
				if (Utility::CanAfford(UNIT_TYPEID::PROTOSS_STALKER, 1, agent->Observation()))
				{
					agent->Actions()->UnitCommand(warpgate, ABILITY_ID::TRAINWARP_STALKER, possible_spots.back());
					possible_spots.pop_back();
					break;
				}
			}
		}
		if (possible_spots.size() == 0)
			break;
	}
	return false;
}

bool ActionManager::ActionTrainFromProxyRobo(ActionArgData* data)
{
	const Unit * robo = data->unit;
	int num_prisms = agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_WARPPRISM)).size();
	int num_obs = agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_OBSERVER)).size();

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
	unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();


	unsigned long long new_units = 0;
	unsigned long long closest_units = 0;
	unsigned long long concaves = 0;
	unsigned long long positions = 0;
	unsigned long long debug = 0;
	unsigned long long apply_pressure = 0;
	unsigned long long update_index = 0;

	ArmyGroup* army = data->army_group;

	Point2D fallback_point = army->attack_path[army->current_attack_index - 2];

	Point2D attack_point = army->attack_path[army->current_attack_index];

	for (int i = 0; i < army->new_units.size(); i++)
	{
		const Unit* unit = army->new_units[i];
		agent->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, fallback_point);
		if (Distance2D(unit->pos, fallback_point) < 5)
		{
			army->AddUnit(unit);
			i--;
		}
	}

	new_units = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	agent->Debug()->DebugSphereOut(Point3D(fallback_point.x, fallback_point.y, agent->Observation()->TerrainHeight(fallback_point)), 3, Color(255, 0, 0));
	agent->Debug()->DebugSphereOut(Point3D(attack_point.x, attack_point.y, agent->Observation()->TerrainHeight(attack_point)), 3, Color(0, 255, 0));

	if (army->stalkers.size() == 0)
		return false;

	const Unit* closest_enemy = Utility::ClosestTo(agent->Observation()->GetUnits(Unit::Alliance::Enemy), Utility::MedianCenter(army->stalkers));
	const Unit* closest_unit_to_enemies = Utility::ClosestTo(army->stalkers, closest_enemy->pos);
	const Unit* furthest_unit_from_enemies = Utility::NthClosestTo(army->stalkers, closest_enemy->pos, round(army->stalkers.size() / 2));

	closest_units  = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	agent->Debug()->DebugSphereOut(closest_unit_to_enemies->pos, .625, Color(255, 0, 255));
	agent->Debug()->DebugSphereOut(furthest_unit_from_enemies->pos, .625, Color(0, 255, 255));

	float unit_size = .625;
	float unit_dispersion = 0;
	Point2D attacking_concave_origin = Utility::PointBetween(Utility::ClosestPointOnLine(furthest_unit_from_enemies->pos, attack_point, fallback_point), attack_point, 2 * unit_size + unit_dispersion);
	//Point2D retreating_concave_origin = Utility::PointBetween(Utility::ClosestPointOnLine(closest_unit_to_enemies->pos, attack_point, fallback_point), fallback_point, unit_size + unit_dispersion);
	Point2D retreating_concave_origin = Utility::PointBetween(Utility::ClosestPointOnLine(closest_enemy->pos, attack_point, fallback_point), fallback_point, Utility::RealGroundRange(closest_enemy, closest_unit_to_enemies) + 2);

	agent->Debug()->DebugSphereOut(Point3D(attacking_concave_origin.x, attacking_concave_origin.y, agent->Observation()->TerrainHeight(attacking_concave_origin)), .625, Color(0, 255, 128));
	agent->Debug()->DebugSphereOut(Point3D(retreating_concave_origin.x, retreating_concave_origin.y, agent->Observation()->TerrainHeight(retreating_concave_origin)), .625, Color(255, 0, 128));

	std::vector<Point2D> attacking_concave_positions = army->FindConcaveFromBack(attacking_concave_origin, fallback_point, army->stalkers.size(), .625, unit_dispersion);
	std::vector<Point2D> retreating_concave_positions = army->FindConcave(retreating_concave_origin, fallback_point, army->stalkers.size(), .625, unit_dispersion);

	concaves  = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	std::map<const Unit*, Point2D> attacking_unit_positions = army->AssignUnitsToPositions(army->stalkers, attacking_concave_positions);
	std::map<const Unit*, Point2D> retreating_unit_positions = army->AssignUnitsToPositions(army->stalkers, retreating_concave_positions);

	positions  = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	for (const auto &pos : attacking_concave_positions)
	{
		agent->Debug()->DebugSphereOut(Point3D(pos.x, pos.y, agent->Observation()->TerrainHeight(pos)), .625, Color(255, 0, 0));
	}
	for (const auto &pos : retreating_concave_positions)
	{
		agent->Debug()->DebugSphereOut(Point3D(pos.x, pos.y, agent->Observation()->TerrainHeight(pos)), .625, Color(0, 255, 0));
	}

	for (const auto &unit : retreating_unit_positions)
	{
		agent->Debug()->DebugLineOut(unit.first->pos + Point3D(0, 0, .2), Point3D(unit.second.x, unit.second.y, agent->Observation()->TerrainHeight(unit.second) + .2), Color(0, 0, 0));
		//Actions()->UnitCommand(unit.first, ABILITY_ID::MOVE_MOVE, unit.second);
	}
	debug  = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	//agent->Actions()->UnitCommand(army->warp_prisms[0], ABILITY_ID::MOVE_MOVE, Utility::PointBetween(Utility::MedianCenter(army->stalkers), fallback_point, 3));
	//agent->Actions()->UnitCommand(army->warp_prisms[0], ABILITY_ID::UNLOADALLAT_WARPPRISM, army->warp_prisms[0]);
	army->ApplyPressureGrouped(attack_point, fallback_point, retreating_unit_positions, attacking_unit_positions);

	apply_pressure = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	Units enemy_attacking_units = agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
	//Actions()->UnitCommand(enemy_attacking_units, ABILITY_ID::ATTACK, fallback_point);

	if (army->current_attack_index > 2 && Distance2D(Utility::Center(army->stalkers), fallback_point) < 5)
		army->current_attack_index--;
	if (army->current_attack_index < army->attack_path.size() - 1 && Distance2D(Utility::MedianCenter(army->stalkers), attack_point) < 5)
	{
		if (true) //obs_in_position
			army->current_attack_index++;
		else
			army->current_attack_index = std::min(army->current_attack_index + 1, army->high_ground_index - 1);
	}
	update_index = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	std::ofstream action_time;
	action_time.open("action_time.txt", std::ios_base::app);
	action_time << new_units - start_time << ", ";
	action_time << closest_units - new_units << ", ";
	action_time << concaves - closest_units << ", ";
	action_time << positions - concaves << ", ";
	action_time << debug - positions << ", ";
	action_time << apply_pressure - debug << ", ";
	action_time << update_index - apply_pressure << "\n";

	action_time.close();

	return false;
}

bool ActionManager::ActionContinueWarpingInStalkers(ActionArgData* data)
{
	bool all_gates_ready = true;
	Units gates = agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
	for (const auto &warpgate : gates)
	{
		bool gate_ready = false;
		for (const auto &ability : agent->Query()->GetAbilitiesForUnit(warpgate).abilities)
		{
			if (ability.ability_id == ABILITY_ID::TRAINWARP_STALKER)
			{
				gate_ready = true;
				break;
			}
		}
		if (!gate_ready)
		{
			all_gates_ready = false;
			break;
		}
	}
	if (all_gates_ready && Utility::CanAfford(UNIT_TYPEID::PROTOSS_STALKER, gates.size(), agent->Observation()))
	{
		std::cout << "warp in stalkers/n";
		std::cout << "all gates ready " << all_gates_ready << "\n";

		std::vector<Point2D> spots = agent->FindWarpInSpots(agent->Observation()->GetGameInfo().enemy_start_locations[0]);
		std::cout << "spots " << spots.size() << "\n";
		if (spots.size() >= gates.size())
		{
			for (int i = 0; i < gates.size(); i++)
			{
				std::cout << "warp in at " << spots[i].x << ", " << spots[i].y << "\n";
				agent->Actions()->UnitCommand(gates[i], ABILITY_ID::TRAINWARP_STALKER, spots[i]);
			}
		}
	}
	return false;
}

bool ActionManager::ActionContinueWarpingInZealots(ActionArgData* data)
{
	bool all_gates_ready = true;
	//if (Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_GATEWAY)).size() > 0)
	//    return false;
	Units gates = agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
	for (const auto &warpgate : gates)
	{
		bool gate_ready = false;
		for (const auto &ability : agent->Query()->GetAbilitiesForUnit(warpgate).abilities)
		{
			if (ability.ability_id == ABILITY_ID::TRAINWARP_ZEALOT)
			{
				gate_ready = true;
				break;
			}
		}
		if (!gate_ready)
		{
			all_gates_ready = false;
			break;
		}
	}
	if (gates.size() > 0 && all_gates_ready && Utility::CanAfford(UNIT_TYPEID::PROTOSS_ZEALOT, gates.size(), agent->Observation()))
	{
		std::vector<Point2D> spots = agent->FindWarpInSpots(agent->Observation()->GetGameInfo().enemy_start_locations[0]);
		if (spots.size() >= gates.size())
		{
			for (int i = 0; i < gates.size(); i++)
			{
				Point3D pos = Point3D(gates[i]->pos.x, gates[i]->pos.y, agent->Observation()->TerrainHeight(gates[i]->pos));
				agent->Debug()->DebugSphereOut(pos, 1, Color(255, 0, 255));
				agent->Actions()->UnitCommand(gates[i], ABILITY_ID::TRAINWARP_ZEALOT, spots[i]);
			}
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
	for (const auto &pylon : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_PYLON)))
	{
		if (Distance2D(pylon->pos, data->position) < 1)
		{
			pylon_placed = true;
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
		std::vector<Point2D> building_locations = agent->GetProxyLocations(data->unitId);
		Point2D pos = building_locations[0];
		active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(scout, data->unitId, pos)));
		return true;
	}
	return false;
}

bool ActionManager::ActionDTHarassTerran(ActionArgData* data)
{
	for (const auto &unit : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_DARKTEMPLAR)))
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
	for (const auto &robo : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
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

			for (const auto &nexus : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
			{
				for (const auto &ability : agent->Query()->GetAbilitiesForUnit(nexus).abilities)
				{
					if (ability.ability_id == ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST)
					{
						agent->Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, robo);
					}
				}
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


}