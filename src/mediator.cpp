
#include "mediator.h"
#include "utility.h"
#include "theBigBot.h"
#include "adept_base_defense_terran_state_machine.h"
#include "adept_harass_protoss_state_machine.h"
#include "blink_stalker_attack_terran_state_machine.h"
#include "cannon_rush_terran_state_machine.h"
#include "chargelot_allin_state_machine.h"
#include "immortal_drop_state_machine.h"
#include "oracle_harass_state_machine.h"
#include "scout_terran_state_machine.h"
#include "scout_zerg_state_machine.h"
#include "scout_protoss_state_machine.h"
#include "stalker_base_defense_terran_state_machine.h"
#include "worker_rush_defense_state_machine.h"




namespace sc2
{

void Mediator::SetUpManagers(bool debug)
{
	worker_manager.new_base = agent->Observation()->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS))[0];

	int id = agent->Observation()->GetPlayerID();
	auto infos = agent->Observation()->GetGameInfo().player_info;
	if (infos.size() > 0)
	{
		scouting_manager.SetEnemyRace(infos[2 - id].race_requested);
	}
	if (debug)
	{
		SetBuildOrder(BuildOrder::testing);
	}
	else
	{
		switch (scouting_manager.enemy_race)
		{
		case Race::Protoss:
			SendChat("Tag: race_protoss", ChatChannel::Team);
			SetBuildOrder(BuildOrder::pvp_openner);
			break;
		case Race::Terran:
			SendChat("Tag: race_terran", ChatChannel::Team);
			SetBuildOrder(BuildOrder::four_gate_blink);
			break;
		case Race::Zerg:
			SendChat("Tag: race_zerg", ChatChannel::Team);
			SetBuildOrder(BuildOrder::oracle_gatewayman_pvz);
			break;
		default:
			SendChat("Tag: race_random", ChatChannel::Team);
			SetBuildOrder(BuildOrder::three_gate_robo);
			break;
		}
	}

	army_manager.SetUpInitialArmies();
}

void Mediator::RunManagers()
{
	ability_manager.UpdateStalkerInfo();
	ability_manager.UpdateOracleInfo();
	fire_control_manager.UpdateInfo();

	scouting_manager.UpdateInfo();

	defense_manager.CheckForAttacks();

	if (agent->Observation()->GetGameLoop() % 20 == 0)
		defense_manager.UpdateOngoingAttacks();

	transition_manager.CheckTransitions();

	if (agent->Observation()->GetGameLoop() > 40)
		worker_manager.DistributeWorkers();
	
	if (worker_manager.new_base != nullptr)
	{
		//std::cout << "add new base\n";
		//std::cout << worker_manager.new_base->pos.x << ' ' << worker_manager.new_base->pos.y << '\n';;
		worker_manager.AddNewBase();
		if (agent->Observation()->GetGameLoop() < 10)
		{
			worker_manager.SplitWorkers();
		}
	}
	if (agent->Observation()->GetGameLoop() % 5 == 0)
	{
		worker_manager.BuildWorkers();
		unit_production_manager.RunUnitProduction();
	}


	build_order_manager.CheckBuildOrder();

	action_manager.ProcessActions();

	finite_state_machine_manager.RunStateMachines();

	army_manager.CreateNewArmyGroups();
	army_manager.RunArmyGroups();

	fire_control_manager.DoAttacks();

	unit_command_manager.ParseUnitCommands();

}

int  Mediator::GetGameLoop()
{
	return agent->Observation()->GetGameLoop();
}

float Mediator::GetCurrentTime()
{
	return agent->Observation()->GetGameLoop() / FRAME_TIME;
}

int Mediator::GetUniqueId()
{
	return agent->GetUniqueId();
}

int Mediator::GetNumWorkers()
{
	return agent->Observation()->GetFoodWorkers();
}

int Mediator::GetSupplyUsed()
{
	return agent->Observation()->GetFoodUsed();
}

int Mediator::GetSupplyCap()
{
	return agent->Observation()->GetFoodCap();
}

ImageData Mediator::GetPathingGrid()
{
	return agent->Observation()->GetGameInfo().pathing_grid;
}

bool Mediator::IsPathable(Point2D pos)
{
	return agent->Observation()->IsPathable(pos);
}

bool Mediator::HasBuildingCompleted(UNIT_TYPEID buildingId)
{
	for (const auto& building : agent->Observation()->GetUnits(IsFriendlyUnit(buildingId)))
	{
		if (building->build_progress == 1)
		{
			return true;
		}
	}
	return false;
}

bool Mediator::HasBuildingUnderConstruction(UNIT_TYPEID buildingId)
{
	for (const auto& building : agent->Observation()->GetUnits(IsFriendlyUnit(buildingId)))
	{
		if (building->build_progress < 1)
		{
			return true;
		}
	}
	return false;
}

bool Mediator::HasBuildingStarted(UNIT_TYPEID buildingId)
{
	if (agent->Observation()->GetUnits(IsFriendlyUnit(buildingId)).size() > 0)
		return true;
	return false;
}

bool Mediator::IsResearching()
{
	return false;
}

bool Mediator::HasResources(int minerals, int vespene, int supply)
{
	bool enough_minerals = (int)agent->Observation()->GetMinerals() >= minerals;
	bool enough_vespene = (int)agent->Observation()->GetVespene() >= vespene;
	bool enough_supply = (int)(agent->Observation()->GetFoodCap() - agent->Observation()->GetFoodUsed()) >= supply;
	return enough_minerals && enough_vespene && enough_supply;
}

int Mediator::GetNumUnits(UNIT_TYPEID unitId)
{
	return (int)agent->Observation()->GetUnits(IsFriendlyUnit(unitId)).size();
}

bool Mediator::CanAfford(UNIT_TYPEID unitId, int amount)
{
	return Utility::CanAfford(unitId, amount, agent->Observation());
}

bool Mediator::CanAffordAfter(UNIT_TYPEID unitId, UnitCost cost)
{
	return Utility::CanAffordAfter(unitId, cost, agent->Observation());
}

int Mediator::MaxCanAfford(UNIT_TYPEID unitId)
{
	return Utility::MaxCanAfford(unitId, agent->Observation());
}

bool Mediator::CanAffordUpgrade(UPGRADE_ID upgrade_id)
{
	return Utility::CanAffordUpgrade(upgrade_id, agent->Observation());
}

bool Mediator::CheckUpgrade(UPGRADE_ID upgrade_id)
{
	return upgrade_manager.CheckUpgrade(upgrade_id);
}

bool Mediator::CanBuildBuilding(UNIT_TYPEID unitId)
{
	return Utility::CanBuildBuilding(unitId, agent->Observation());
}

bool Mediator::IsUnitOccupied(const Unit* unit)
{
	for (ArmyGroup* army_group : army_manager.army_groups)
	{
		for (const Unit* army_unit : army_group->all_units)
		{
			if (unit == army_unit)
				return true;
		}
	}
	return false;
}

int Mediator::GetUpgradeLevel(UpgradeType upgrade_type)
{
	return upgrade_manager.GetUpgradeLevel(upgrade_type);
}

const Unit* Mediator::GetMostRecentBuilding(UNIT_TYPEID type)
{
	Units possible_buildings = GetUnits(Unit::Alliance::Self, IsUnit(type));
	if (possible_buildings.size() == 0)
		return nullptr;

	for (int i = 0; i < possible_buildings.size(); i++)
	{
		if (possible_buildings.size() == 1)
			return possible_buildings[0];

		if (possible_buildings[i]->build_progress == 1)
		{
			possible_buildings.erase(possible_buildings.begin() + i);
			i--;
		}
	}
	std::sort(possible_buildings.begin(), possible_buildings.end(),
		[](const Unit* a, const Unit* b) -> bool
	{
		return a->build_progress < b->build_progress;
	});
	return possible_buildings[0];
}

float Mediator::GetLineDangerLevel(PathManager path)
{
	float danger = 0;
	for (const auto& unit : scouting_manager.enemy_unit_saved_position)
	{
		Point2D closest_point = path.FindClosestPoint(unit.second.pos);
		if (Distance2D(closest_point, unit.second.pos) < Utility::GetGroundRange(unit.first))
			danger += Utility::GetDPS(unit.first);
	}
	return danger;
}

bool Mediator::IsVisible(Point2D pos)
{
	return agent->Observation()->GetVisibility(pos) == Visibility::Visible;
}

std::vector<std::vector<UNIT_TYPEID>> Mediator::GetPrio()
{
	switch (GetEnemyRace())
	{
	case Race::Zerg:
		return ZERG_PRIO;
	case Race::Terran:
		return TERRAN_PRIO;
	case Race::Protoss:
		return PROTOSS_PRIO;
	default:
		return {};
	}
}

UnitCost Mediator::GetCurrentResources()
{
	return UnitCost(agent->Observation()->GetMinerals(), agent->Observation()->GetVespene(), 0);
}

void Mediator::CancelBuilding(const Unit* building)
{
	SetUnitCommand(building, ABILITY_ID::CANCEL, 100); // TODO check abilitiy
	RebuildBuilding(building->pos, building->unit_type);
}

void Mediator::RebuildBuilding(Point2D pos, UNIT_TYPEID type)
{
	action_manager.AddAction(new ActionData(&ActionManager::ActionBuildBuildingWhenSafe, new ActionArgData(type, pos)));
}

void Mediator::SendChat(std::string message, ChatChannel channel)
{
	agent->Actions()->SendChat(message, channel);
}

const Unit* Mediator::GetBuilder(Point2D position)
{
	return worker_manager.GetBuilder(position);
}

bool Mediator::BuildBuilding(UNIT_TYPEID buildingId)
{
	Point2D pos = GetLocation(buildingId);
	const Unit* builder = GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	worker_manager.RemoveWorker(builder);
	action_manager.AddAction(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(builder, buildingId, pos)));
	return true;
}

void Mediator::BuildBuilding(UNIT_TYPEID buildingId, Point2D position, const Unit* probe)
{
	worker_manager.RemoveWorker(probe);
	action_manager.AddAction(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(probe, buildingId, position)));
}

void Mediator::BuildBuildingMulti(std::vector<UNIT_TYPEID> buildingIds, Point2D position, const Unit* probe)
{
	worker_manager.RemoveWorker(probe);
	action_manager.AddAction(new ActionData(&ActionManager::ActionBuildBuildingMulti, new ActionArgData(probe, buildingIds, position, 0)));
}

Units Mediator::GetAllWorkersOnGas()
{
	Units workers;
	for (const auto& data : worker_manager.assimilators_reversed)
	{
		workers.push_back(data.first);
	}
	return workers;
}

int Mediator::NumFar3rdWorkers()
{
	return (int)worker_manager.far_3_mineral_patch_extras.size();
}

bool Mediator::SendScout()
{
	Point2D pos = GetNaturalLocation();
	const Unit* scouter = GetBuilder(pos);
	if (scouter == nullptr)
	{
		//std::cout << "Error could not find builder in Scout" << std::endl;
		return false;
	}
	RemoveWorker(scouter);
	switch (GetEnemyRace())
	{
	case Race::Zerg:
	{
		ScoutZergStateMachine* scout_fsm = new ScoutZergStateMachine(agent, "Scout Zerg", scouter, agent->locations->initial_scout_pos,
			agent->locations->main_scout_path, agent->locations->natural_scout_path, agent->locations->enemy_natural, agent->locations->possible_3rds);
		finite_state_machine_manager.AddStateMachine(scout_fsm);
		break;
	}
	case Race::Terran:
	{
		ScoutTerranStateMachine* scout_fsm = new ScoutTerranStateMachine(agent, "Scout Terran", scouter, agent->locations->initial_scout_pos,
			agent->locations->main_scout_path, agent->locations->natural_scout_path, agent->locations->enemy_natural);
		finite_state_machine_manager.AddStateMachine(scout_fsm);
		break;
	}
	case Race::Protoss:
	{
		ScoutProtossStateMachine* scout_fsm = new ScoutProtossStateMachine(agent, "Scout Protoss", scouter, agent->locations->initial_scout_pos,
			agent->locations->main_scout_path, agent->locations->natural_scout_path, agent->locations->enemy_natural);
		finite_state_machine_manager.AddStateMachine(scout_fsm);
		break;
	}
	default: // TODO make a dedicated random scout state machine
	{
		ScoutTerranStateMachine* scout_fsm = new ScoutTerranStateMachine(agent, "Scout Random", scouter, agent->locations->initial_scout_pos,
			agent->locations->main_scout_path, agent->locations->natural_scout_path, agent->locations->enemy_natural);
		finite_state_machine_manager.AddStateMachine(scout_fsm);
		break;
	}
	}
	return true;
}

bool Mediator::SendCannonRushProbe1()
{
	/*Point2D pos = mediator->GetEnemyStartLocation();
	const Unit* scouter = mediator->GetBuilder(pos);
	if (scouter == nullptr)
	{
		//std::cout << "Error could not find builder in Scout" << std::endl;
		return false;
	}
	mediator->RemoveWorker(scouter);
	if (mediator->GetEnemyRace() == Race::Zerg)
	{
		ScoutZergStateMachine* scout_fsm = new ScoutZergStateMachine(mediator, "Scout Zerg", scouter, mediator->locations->initial_scout_pos, mediator->locations->main_scout_path, mediator->locations->natural_scout_path, mediator->locations->enemy_natural, mediator->locations->possible_3rds);
		mediator->active_FSMs.push_back(scout_fsm);
	}
	else
	{
		ScoutTerranStateMachine* scout_fsm = new ScoutTerranStateMachine(mediator, "Scout Terran", scouter, mediator->locations->initial_scout_pos, mediator->locations->main_scout_path, mediator->locations->natural_scout_path, mediator->locations->enemy_natural);
		mediator->active_FSMs.push_back(scout_fsm);
	}*/
	return true;
}

void Mediator::SetBuildWorkers(bool value)
{
	worker_manager.should_build_workers = value;
}

bool Mediator::CheckBuildWorkers()
{
	return worker_manager.should_build_workers;
}

void Mediator::SetImmediatlySaturateGasses(bool value)
{
	worker_manager.immediatelySaturateGasses = value;
}

void Mediator::SetImmediatlySemiSaturateGasses(bool value)
{
	worker_manager.immediatelySemiSaturateGasses = value;
}

void Mediator::SetBalanceIncome(bool value)
{
	worker_manager.balance_income = value;
}

void Mediator::SetBuildOrder(BuildOrder build)
{
	agent->locations = new Locations(ToPoint3D(GetStartLocation()), GetMapName());
	build_order_manager.SetBuildOrder(build);
	SendChat("Tag: " + GetMapName(), ChatChannel::Team);
}

void Mediator::PauseBuildOrder()
{
	build_order_manager.PauseBuildOrder();

}

void Mediator::UnPauseBuildOrder()
{
	build_order_manager.UnpauseBuildOrder();
}

void Mediator::SetWorkerRushDefenseBuidOrder()
{
	build_order_manager.SetWorkerRushDefense();
	if (GetUnits(Unit::Alliance::Self, IsUnit(GATEWAY)).size() > 0)
		build_order_manager.build_order_step = 1;
	else
		build_order_manager.build_order_step = 0;
}

// TODO create locations manager
Point2D Mediator::GetLocation(UNIT_TYPEID unit_type)
{
	std::vector<Point2D> possible_locations;
	int pending_buildings = 0;
	for (const auto& action : action_manager.active_actions)
	{
		if (action->action == &ActionManager::ActionBuildBuilding)
		{
			if (action->action_arg->unitId == unit_type)
			{
				pending_buildings++;
			}
		}
	}

	std::vector<UNIT_TYPEID> tech_buildings = { FORGE, TWILIGHT, ROBO, ROBO_BAY, STARGATE, FLEET_BEACON, DARK_SHRINE, TEMPLAR_ARCHIVES };
	if (std::find(tech_buildings.begin(), tech_buildings.end(), unit_type) != tech_buildings.end())
	{
		possible_locations = agent->locations->tech_locations;
	}
	else
	{
		switch (unit_type)
		{
		case PYLON:
			possible_locations = agent->locations->pylon_locations;
			break;
		case NEXUS:
			possible_locations = agent->locations->nexi_locations;
			break;
		case GATEWAY:
			possible_locations = agent->locations->gateway_locations;
			break;
		case ASSIMILATOR:
			possible_locations = agent->locations->assimilator_locations;
			break;
		case CYBERCORE:
			possible_locations = agent->locations->cyber_core_locations;
			break;
		default:
			//std::cout << "Error invalid type id in GetLocation" << std::endl;
			return Point2D(0, 0);
		}
	}

	for (const auto& point : possible_locations)
	{
		bool blocked = false;
		bool in_base = !(unit_type == PYLON);
		bool in_energy_field = (unit_type == PYLON || unit_type == ASSIMILATOR || unit_type == NEXUS);
		for (const auto& building : agent->Observation()->GetUnits(IsBuilding()))
		{
			if (Distance2D(building->pos, point) < 1)
			{
				blocked = true;
				break;
			}
			if (!in_energy_field && building->unit_type == PYLON)
			{
				if (Distance2D(Point2D(building->pos), point) < 6.5)
				{
					in_energy_field = true;
				}
			}
			if (building->unit_type == NEXUS && Distance2D(building->pos, point) < 22)
				in_base = true;
		}
		if (in_base && !blocked && in_energy_field)
			if (pending_buildings == 0)
				return point;
			else
				pending_buildings--;

	}
	std::cerr << "Error no viable point found in GetLocation for type " << UnitTypeToName(unit_type) << std::endl;
	std::cerr << "Possible locations:" << std::endl;
	for (const auto& location : possible_locations)
	{
		std::cerr << location.x << ", " << location.y << std::endl;
	}
	std::cerr << std::endl;
	return Point2D(0, 0);
}

Point2D Mediator::GetLocation(UNIT_TYPEID unit_type, int index)
{
	std::vector<Point2D> positions;
	switch (unit_type)
	{
	case NEXUS:
		positions = agent->locations->nexi_locations;
		break;
	case PYLON:
		positions = agent->locations->pylon_locations;
		break;
	case GATEWAY:
		positions = agent->locations->gateway_locations;
		break;
	case ROBO:
	case TWILIGHT:
	case STARGATE:
	case ROBO_BAY:
	case TEMPLAR_ARCHIVES:
	case DARK_SHRINE:
	case FLEET_BEACON:
	case FORGE:
		positions = agent->locations->tech_locations;
		break;
	}
	if (positions.size() - 1 > index)
		return positions[index];

	return Point2D(0, 0);
}

Point2D Mediator::GetProxyLocation(UNIT_TYPEID unit_type)
{
	std::vector<Point2D> possible_locations;

	std::vector<UNIT_TYPEID> tech_buildings = { FORGE, TWILIGHT, ROBO, ROBO_BAY, STARGATE, FLEET_BEACON, DARK_SHRINE, TEMPLAR_ARCHIVES };
	if (std::find(tech_buildings.begin(), tech_buildings.end(), unit_type) != tech_buildings.end())
	{
		possible_locations = agent->locations->proxy_tech_locations;
	}
	else
	{
		switch (unit_type)
		{
		case PYLON:
			possible_locations = agent->locations->proxy_pylon_locations;
			break;
		case NEXUS:
			possible_locations = agent->locations->nexi_locations;
			break;
		case GATEWAY:
			possible_locations = agent->locations->proxy_gateway_locations;
			break;
		default:
			//std::cout << "Error invalid type id in GetLocation" << std::endl;
			return Point2D(0, 0);
		}
	}

	for (const auto& point : possible_locations)
	{
		bool blocked = false;
		bool in_energy_field = (unit_type == PYLON || unit_type == ASSIMILATOR || unit_type == NEXUS);
		for (const auto& building : agent->Observation()->GetUnits(IsBuilding()))
		{
			if (Point2D(building->pos) == point)
			{
				blocked = true;
				break;
			}
			if (!in_energy_field && building->unit_type == PYLON)
			{
				if (Distance2D(Point2D(building->pos), point) < 6.5)
				{
					in_energy_field = true;
				}
			}
		}
		if (!blocked && in_energy_field)
			return point;

	}
	std::cerr << "Error no viable point found in GetProxyLocation for type " << UnitTypeToName(unit_type) << std::endl;
	return Point2D(0, 0);
}

std::vector<Point2D> Mediator::GetProxyLocations(UNIT_TYPEID unit_type)
{
	return agent->GetProxyLocations(unit_type);
}

Point2D Mediator::GetNaturalDefensiveLocation(UNIT_TYPEID unit_type)
{
	if (unit_type == BATTERY)
	{
		std::vector<Point2D> possible_locations = agent->locations->defensive_natural_battery_locations;
		for (const auto& point : possible_locations)
		{
			bool blocked = false;
			bool in_energy_field = (unit_type == PYLON || unit_type == ASSIMILATOR || unit_type == NEXUS);
			for (const auto& building : agent->Observation()->GetUnits(IsBuilding()))
			{
				if (Point2D(building->pos) == point)
				{
					blocked = true;
					break;
				}
				if (!in_energy_field && building->unit_type == PYLON)
				{
					if (Distance2D(Point2D(building->pos), point) < 6.5)
					{
						in_energy_field = true;
					}
				}
			}
			for (const auto& action : action_manager.active_actions)
			{
				if (action->action == &ActionManager::ActionBuildBuilding)
				{
					if (Distance2D(point, action->action_arg->position) < 2)
					{
						blocked = true;
						break;
					}
				}
			}
			if (!blocked && in_energy_field)
				return point;
		}
		std::cerr << "Error no viable point found in GetNaturalDefensiveLocation for type " << UnitTypeToName(unit_type) << std::endl;
		return Point2D(0, 0);
	}
	if (unit_type == PYLON)
		return Utility::PointBetween(agent->locations->nexi_locations[1], agent->locations->natural_door_closed, 4);
	
	return Point2D(0, 0);
}

Point2D Mediator::GetFirstPylonLocation()
{
	switch (GetEnemyRace())
	{
	case Race::Protoss:
		return agent->locations->first_pylon_location_protoss;
		break;
	case Race::Zerg:
		return agent->locations->first_pylon_location_zerg;
		break;
	case Race::Terran:
		return agent->locations->first_pylon_location_terran;
		break;
	default:
		return agent->locations->first_pylon_location_protoss;
		break;
	}
}

Point2D Mediator::FindLocation(UNIT_TYPEID unit_type, Point2D location)
{

	std::vector<Point2D> spots;
	Point2D enemy_main = GetEnemyStartLocation();
	Units buildings = GetUnits(Unit::Alliance::Self, IsBuilding());
	Units pylons = GetUnits(Unit::Alliance::Self, IsFinishedUnit(UNIT_TYPEID::PROTOSS_PYLON));

	std::sort(pylons.begin(), pylons.end(), [location](const Unit*& a, const Unit*& b) -> bool
	{
		return Distance2D(a->pos, location) < Distance2D(b->pos, location);
	});

	std::vector<Point2D> possible_locations;
	for (const auto& pylon : pylons)
	{
		for (float i = -7; i <= 6; i += 1)
		{
			for (float j = -7; j <= 6; j += 1)
			{
				Point2D curr_pos = pylon->pos + Point2D(i, j);
				if (Distance2D(location, enemy_main) < Distance2D(curr_pos, enemy_main))
					continue;
				bool blocked = false;
				for (const auto& building : buildings)
				{
					float size = 1.5;
					if (building->unit_type == NEXUS)
						size = 2.5;
					else if (building->unit_type == BATTERY || building->unit_type == CANNON ||
						building->unit_type == PYLON || building->unit_type == DARK_SHRINE)
						size = 1;
					if (((curr_pos.x + 1 > building->pos.x - size && curr_pos.x - 1 < building->pos.x + size ) ||
						(curr_pos.x - 1 > building->pos.x - size && curr_pos.x - 1 < building->pos.x + size)) &&
						((curr_pos.y + 1 > building->pos.y - size && curr_pos.y + 1 < building->pos.y + size) ||
							(curr_pos.y - 1 > building->pos.y - size && curr_pos.y - 1 < building->pos.y + size)))
					{
						blocked = true;
						break;
					}
				}
				if (!blocked)
					possible_locations.push_back(curr_pos);
			}
		}
	}

	if (possible_locations.size() == 0)
	{
		std::cerr << "Error no viable point found in GetProxyLocation for type " << UnitTypeToName(unit_type) << std::endl;
		return Point2D(0, 0);
	}

	std::sort(possible_locations.begin(), possible_locations.end(), [location, enemy_main](const Point2D& a, const Point2D& b) -> bool
	{
		return Distance2D(a, location) + Distance2D(a, enemy_main) < Distance2D(b, location) + Distance2D(b, enemy_main);
	});

	if (possible_locations.size() > 10)
	{
		std::sort(possible_locations.begin(), possible_locations.begin() + 10, [location](const Point2D& a, const Point2D& b) -> bool
		{
			return Distance2D(a, location) < Distance2D(b, location);
		});
	}
	else if (possible_locations.size() > 5)
	{
		std::sort(possible_locations.begin(), possible_locations.begin() + 5, [location](const Point2D& a, const Point2D& b) -> bool
		{
			return Distance2D(a, location) < Distance2D(b, location);
		});
	}

	return possible_locations[0];
}

void Mediator::ContinueBuildingPylons()
{
	action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueBuildingPylons, new ActionArgData()));
}

void Mediator::ContinueMakingWorkers()
{
	action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueMakingWorkers, new ActionArgData()));
}

void Mediator::ContinueUpgrades()
{
	action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueUpgrades, new ActionArgData()));
}

void Mediator::ContinueChronos()
{
	action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueChronos, new ActionArgData()));
}

void Mediator::ContinueExpanding()
{
	action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueExpanding, new ActionArgData()));
}

bool Mediator::TrainFromProxyRobo()
{
	std::vector<const Unit*> proxy_robos;
	for (const auto& robo : agent->Observation()->GetUnits(IsFriendlyUnit(ROBO)))
	{
		if (Utility::DistanceToClosest(agent->GetProxyLocations(ROBO), robo->pos) < 2)
		{
			action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionTrainFromProxyRobo, new ActionArgData(robo)));
			return true;
		}
	}
	return false;
}

void Mediator::AddAction(bool(sc2::ActionManager::* action)(ActionArgData*), ActionArgData* data)
{
	if (!HasActionOfType(action))
		action_manager.active_actions.push_back(new ActionData(action, data));

}

int Mediator::GetNumBuildActions(UNIT_TYPEID unit_type)
{
	int actions = 0;
	for (const auto& action : action_manager.active_actions)
	{
		if (action->action == &ActionManager::ActionBuildBuilding)
		{
			if (action->action_arg->unitId == unit_type)
			{
				actions++;
			}
		}
	}
	return actions;
}

bool Mediator::HasActionOfType(bool(sc2::ActionManager::* type)(ActionArgData*))
{
	for (const auto& action : action_manager.active_actions)
	{
		if (action->action == type)
			return true;
	}
	return false;
}

void Mediator::CancelAllBuildActions()
{
	for (auto itr = action_manager.active_actions.begin(); itr != action_manager.active_actions.end();)
	{
		if ((*itr)->action == &ActionManager::ActionBuildBuilding)
		{
			if ((*itr)->action_arg->unit)
				PlaceWorker((*itr)->action_arg->unit);

			itr = action_manager.active_actions.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void Mediator::CancelAllActionsOfType(bool(sc2::ActionManager::* type)(ActionArgData*))
{
	for (auto itr = action_manager.active_actions.begin(); itr != action_manager.active_actions.end();)
	{
		if ((*itr)->action == type)
		{
			if ((*itr)->action_arg->unit && (*itr)->action_arg->unit->unit_type == PROBE)
				PlaceWorker((*itr)->action_arg->unit);

			itr = action_manager.active_actions.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

UnitCost Mediator::CalculateCostOfCurrentBuildActions()
{
	UnitCost total;
	for (const auto& action : action_manager.active_actions)
	{
		if (action->action == &ActionManager::ActionBuildBuilding)
		{
			total += Utility::GetCost(action->action_arg->unitId);
		}
	}
	return total;
}

ScoutInfoTerran Mediator::GetScoutInfoTerran()
{
	return agent->scout_info_terran;
}

ScoutInfoZerg Mediator::GetScoutInfoZerg()
{
	return agent->scout_info_zerg;
}

int Mediator::GetEnemyUnitCount(UNIT_TYPEID type)
{
	return scouting_manager.GetEnemyUnitCount(type);
}

StateMachine* Mediator::GetStateMachineByName(std::string name)
{
	for (auto& fsm : finite_state_machine_manager.active_state_machines)
	{
		if (fsm->name == name)
			return fsm;
	}
	return nullptr;
}

void Mediator::RemoveStateMachine(StateMachine* state_machine)
{
	finite_state_machine_manager.RemoveStateMachine(state_machine);
}

void Mediator::CreateFourGateBlinkFSM()
{
	ArmyGroup* army = CreateArmyGroup(ArmyRole::outside_control, { STALKER, PRISM }, 15, 25);
	army->standby_pos = agent->locations->blink_presure_consolidation;
	army->attack_path = GetStalkerAttackPath();
	army->attack_path_line = GetStalkerAttackLine();

	BlinkStalkerAttackTerran* blink_fsm = new BlinkStalkerAttackTerran(agent, "4 gate blink pressure", agent->locations->blink_presure_consolidation,
		agent->locations->blink_pressure_prism_consolidation, agent->locations->blink_pressure_blink_up, agent->locations->blink_pressure_blink_down);
	finite_state_machine_manager.active_state_machines.push_back(blink_fsm);
	army->state_machine = blink_fsm;
	blink_fsm->attached_army_group = army;
}

void Mediator::CreateAdeptHarassProtossFSM()
{
	AdeptHarassProtoss* adept_fsm = new AdeptHarassProtoss(agent, "adept harass protoss", GetUnits(IsFriendlyUnit(ADEPT)), agent->locations->adept_harrass_protoss_consolidation);
	finite_state_machine_manager.active_state_machines.push_back(adept_fsm);

	ArmyGroup* adept_army = army_manager.CreateArmyGroup(ArmyRole::outside_control, { ADEPT }, 2, 2);
	adept_army->state_machine = adept_fsm;
	adept_fsm->attached_army_group = adept_army;
}

void Mediator::StartOracleHarassStateMachine(ArmyGroup* army)
{
	OracleHarassStateMachine* oracle_fsm = new OracleHarassStateMachine(agent, {}, "Oracle harass");
	finite_state_machine_manager.active_state_machines.push_back(oracle_fsm);

	army->state_machine = oracle_fsm;
	oracle_fsm->attached_army_group = army;
}

bool Mediator::RemoveScoutToProxy(UNIT_TYPEID unitId, int amount)
{
	const Unit* scout = nullptr;
	Race enemy_race = GetEnemyRace();
	StateMachine* state_machine = nullptr;
	switch (enemy_race)
	{
	case Race::Zerg:
		state_machine = GetStateMachineByName("Scout Zerg");
		if (state_machine != nullptr)
		{
			scout = ((ScoutZergStateMachine*)state_machine)->scout;
		}
		else
		{
			state_machine = GetStateMachineByName("Scout Random");
			if (state_machine != nullptr)
				scout = ((ScoutTerranStateMachine*)state_machine)->scout;
		}
		break;
	case Race::Terran:
		state_machine = GetStateMachineByName("Scout Terran");
		if (state_machine != nullptr)
		{
			scout = ((ScoutTerranStateMachine*)state_machine)->scout;
		}
		else
		{
			state_machine = GetStateMachineByName("Scout Random");
			if (state_machine != nullptr)
				scout = ((ScoutTerranStateMachine*)state_machine)->scout;
		}
		break;
	case Race::Protoss:
		state_machine = GetStateMachineByName("Scout Protoss");
		if (state_machine != nullptr)
		{
			scout = ((ScoutProtossStateMachine*)state_machine)->scout;
		}
		else
		{
			state_machine = GetStateMachineByName("Scout Random");
			if (state_machine != nullptr)
				scout = ((ScoutTerranStateMachine*)state_machine)->scout;
		}
		break;
	case Race::Random:
		state_machine = GetStateMachineByName("Scout Random");
		if (state_machine != nullptr)
			scout = ((ScoutTerranStateMachine*)state_machine)->scout;
		break;
	}

	if (scout != nullptr && scout->is_alive)
	{
		RemoveStateMachine(state_machine);

		Point2D pos = GetProxyLocations(PYLON)[0];
		SetUnitCommand(scout, ABILITY_ID::GENERAL_MOVE, pos, 0);
		action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionRemoveScoutToProxy, new ActionArgData(scout, unitId, pos, amount)));
		return true;
	}
	return false;
}

void Mediator::CreateAdeptBaseDefenseTerranFSM()
{
	AdeptBaseDefenseTerran* adept_defense_fsm = new AdeptBaseDefenseTerran(agent, "Adept base defense", 
		agent->locations->main_early_dead_space, agent->locations->natural_front);
	finite_state_machine_manager.active_state_machines.push_back(adept_defense_fsm);

	ArmyGroup* adept_harass = army_manager.CreateArmyGroup(ArmyRole::outside_control, { ADEPT }, 1, 1);
	adept_harass->state_machine = adept_defense_fsm;
	adept_defense_fsm->attached_army_group = adept_harass;
}

void Mediator::CreateWorkerRushDefenseFSM()
{
	Units attackers = Utility::GetUnitsWithin(GetUnits(Unit::Alliance::Enemy, IsUnits({ PROBE, SCV, DRONE })),
		GetStartLocation(), 20.0f);

	WorkerRushDefenseStateMachine* worker_rush_defense_fsm =
		new WorkerRushDefenseStateMachine(agent, "Worker rush defense");

	finite_state_machine_manager.active_state_machines.push_back(worker_rush_defense_fsm);
}

void Mediator::MarkStateMachineForDeletion(StateMachine* state_machine)
{
	finite_state_machine_manager.MarkStateMachineForDeletion(state_machine);
	if (state_machine->attached_army_group != nullptr)
		army_manager.MarkArmyGroupForDeletion(state_machine->attached_army_group);
}

void Mediator::MarkArmyGroupForDeletion(ArmyGroup* army_group)
{
	army_manager.MarkArmyGroupForDeletion(army_group);
	if (army_group->state_machine != nullptr)
		finite_state_machine_manager.MarkStateMachineForDeletion(army_group->state_machine);
}

void Mediator::DefendThirdBaseZerg()
{
	army_manager.CreateArmyGroup(ArmyRole::defend_third, { ADEPT }, 1, 1);
}

void Mediator::AddToDefense(int base, int amount)
{
	Point2D base_location = agent->locations->nexi_locations[base];
	for (auto& army_group : army_manager.army_groups)
	{
		if (army_group->target_pos == base_location)
		{
			army_group->desired_units += amount;
			army_group->max_units += amount;
		}
	}
	army_manager.BalanceUnits();
}

void Mediator::AddToDefense(Point2D base_location, int amount)
{
	for (auto& army_group : army_manager.army_groups)
	{
		if (army_group->target_pos == base_location)
		{
			army_group->desired_units += amount;
			army_group->max_units += amount;
		}
	}
	army_manager.BalanceUnits();
}

void Mediator::BuildDefensiveBuilding(UNIT_TYPEID type, Point2D location)
{
	Point2D pos = Point2D(0, 0);
	if (location == GetNaturalLocation())
	{
		pos = GetNaturalDefensiveLocation(type);
	}
	if (pos == Point2D(0, 0))
		pos = FindLocation(BATTERY, location);

	const Unit* builder = GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return;
	}
	worker_manager.RemoveWorker(builder);

	action_manager.AddAction(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(builder, type, pos)));
}

std::vector<OngoingAttack> Mediator::GetOngoingAttacks()
{
	return defense_manager.ongoing_attacks;
}

float Mediator::GetWorstOngoingAttackValue()
{
	float min = 0;
	for (const auto& attack : defense_manager.ongoing_attacks)
	{
		if (attack.status < min)
			min = attack.status;
	}
	return min;
}

void Mediator::PlaceWorker(const Unit* worker)
{
	worker_manager.PlaceWorker(worker);
}

RemoveWorkerResult Mediator::RemoveWorker(const Unit* worker)
{
	return worker_manager.RemoveWorker(worker);
}

void Mediator::PullOutOfGas()
{
	worker_manager.PullOutOfGas();
}

UnitCost Mediator::CalculateIncome()
{
	return worker_manager.CalculateIncome();
}

void Mediator::SetUnitProduction(UNIT_TYPEID unit_type)
{
	switch (unit_type)
	{
	case ZEALOT:
	case STALKER:
	case SENTRY:
	case ADEPT:
	case HIGH_TEMPLAR:
	case DARK_TEMPLAR:
		unit_production_manager.SetWarpgateProduction(unit_type);
		break;
	case IMMORTAL:
	case COLOSSUS:
	case DISRUPTOR:
	case OBSERVER:
	case PRISM:
		unit_production_manager.SetRoboProduction(unit_type);
		break;
	case PHOENIX:
	case VOID_RAY:
	case ORACLE:
	case CARRIER:
	case TEMPEST:
		unit_production_manager.SetStargateProduction(unit_type);
		break;
	}
}

UNIT_TYPEID Mediator::GetWarpgateProduction()
{
	return unit_production_manager.warpgate_production;
}

UNIT_TYPEID Mediator::GetRoboProduction()
{
	return unit_production_manager.robo_production;
}

UNIT_TYPEID Mediator::GetStargateProduction()
{
	return unit_production_manager.stargate_production;
}

void Mediator::CancelWarpgateUnitProduction()
{
	unit_production_manager.SetWarpgateProduction(UNIT_TYPEID::INVALID);
}

void Mediator::CancelRoboUnitProduction()
{
	unit_production_manager.SetRoboProduction(UNIT_TYPEID::INVALID);
}

void Mediator::CancelStargateUnitProduction()
{
	unit_production_manager.SetStargateProduction(UNIT_TYPEID::INVALID);
}

void Mediator::SetWarpInAtProxy(bool status)
{
	unit_production_manager.SetWarpInAtProxy(status);
}

UnitCost Mediator::CalculateCostOfProduction()
{
	return unit_production_manager.CalculateCostOfProduction();
}

ArmyGroup* Mediator::CreateArmyGroup(ArmyRole role, std::vector<UNIT_TYPEID> unit_types, int desired_units, int max_units)
{
	return army_manager.CreateArmyGroup(role, unit_types, desired_units, max_units);
}

ArmyGroup* Mediator::GetArmyGroupDefendingBase(Point2D pos)
{
	for (const auto& army_group : army_manager.army_groups)
	{
		if (army_group->role == ArmyRole::defend_base && army_group->target_pos == pos)
			return army_group;
	}
	return nullptr;
}

void Mediator::ScourMap()
{
	army_manager.ScourMap();
}

Point2D Mediator::GetStartLocation()
{
	return agent->Observation()->GetStartLocation();
}

Point2D Mediator::GetNaturalLocation()
{
	return agent->locations->nexi_locations[1];
}

Point2D Mediator::GetEnemyStartLocation()
{
	return agent->Observation()->GetGameInfo().enemy_start_locations[0];
}

Point2D Mediator::GetEnemyNaturalLocation()
{
	return agent->locations->enemy_natural;
}

PathManager Mediator::GetDirectAttackLine()
{
	return agent->locations->attack_path_short_line;
}

std::vector<Point2D> Mediator::GetDirectAttackPath()
{
	return agent->locations->attack_path_short;
}

PathManager Mediator::GetStalkerAttackLine()
{
	if (GetMapName() == "Abyssal Reef AIE" || GetMapName() == "Interloper AIE")
		return agent->locations->attack_path_special_line;
	else
		return agent->locations->attack_path_short_line;
}

std::vector<Point2D> Mediator::GetStalkerAttackPath()
{
	if (GetMapName() == "Abyssal Reef AIE")
		return agent->locations->attack_path_special;
	else
		return agent->locations->attack_path_short;
}

PathManager Mediator::GetIndirectAttackLine()
{
	return agent->locations->attack_path_line;
}

std::vector<Point2D> Mediator::GetIndirectAttackPath()
{
	return agent->locations->attack_path;
}

std::vector<Point2D> Mediator::GetAltAttackPath()
{
	return agent->locations->attack_path_alt;
}

std::vector<Point2D> Mediator::GetBadWarpInSpots()
{
	return agent->locations->bad_warpin_spots;
}

const Unit* Mediator::GetWorkerRushDefenseAttackingMineralPatch()
{
	return Utility::ClosestTo(GetUnits(Unit::Alliance::Neutral), agent->locations->worker_rush_defense_attack);
}

const Unit* Mediator::GetWorkerRushDefenseGroupingMineralPatch()
{
	return Utility::ClosestTo(GetUnits(Unit::Alliance::Neutral), agent->locations->worker_rush_defense_group);
}

std::vector<Point2D> Mediator::GetEmptyBases()
{
	return agent->locations->base_locations;
}

std::string Mediator::GetMapName()
{
	return agent->Observation()->GetGameInfo().map_name;
}

Race Mediator::GetEnemyRace()
{
	return scouting_manager.enemy_race;
}

Point3D Mediator::ToPoint3D(Point2D point)
{
	return agent->ToPoint3D(point);
}
	
Units Mediator::GetUnits(Unit::Alliance alliance, Filter filter)
{
	return agent->Observation()->GetUnits(alliance, filter);
}

Units Mediator::GetUnits(Unit::Alliance alliance)
{
	return agent->Observation()->GetUnits(alliance);
}

Units Mediator::GetUnits(Filter filter)
{
	return agent->Observation()->GetUnits(filter);
}

const Unit* Mediator::GetUnit(Tag tag)
{
	return agent->Observation()->GetUnit(tag);
}

AvailableAbilities Mediator::GetAbilitiesForUnit(const Unit* unit)
{
	// Expensive, use very sparingly
	std::cerr << "Call to GetAbilitiesForUnit" << std::endl;
	return agent->Query()->GetAbilitiesForUnit(unit);
}

Point2D Mediator::GetUnitPosition(const Unit* unit)
{
	if (scouting_manager.enemy_unit_saved_position.count(unit) != 0)
		return scouting_manager.enemy_unit_saved_position[unit].pos;
	return Point2D(0, 0);
}

bool Mediator::IsStalkerBlinkOffCooldown(const Unit* unit)
{
	return ability_manager.IsStalkerBlinkOffCooldown(unit);
}

void Mediator::SetStalkerOrder(const Unit* unit)
{
	ability_manager.SetStalkerOrder(unit);
}

bool Mediator::IsOracleBeamActive(const Unit* unit)
{
	return ability_manager.IsOracleBeamOn(unit);
}

void Mediator::SetOracleOrder(const Unit* unit, ABILITY_ID ability)
{
	ability_manager.SetOracleOrder(unit, ability);
}

bool Mediator::IsOracleCasting(const Unit* unit)
{
	return ability_manager.IsOracleCasting(unit);
}

bool Mediator::IsBatteryOverchargeOffCooldown()
{
	return ability_manager.NexusBatteryOverchargeOffCooldown();
}

void Mediator::SetBatteryOverchargeCooldown()
{
	ability_manager.SetBatteryOverchargeCooldown(GetCurrentTime());
}

bool Mediator::IsNexusRecallOffCooldown()
{
	return ability_manager.NexusRecallOffCooldown();
}

void Mediator::SetNexusRecallCooldown()
{
	ability_manager.SetNexusRecallCooldown(GetCurrentTime());
}

void Mediator::AddUnitToAttackers(const Unit* unit)
{
	fire_control_manager.AddUnit(unit);
}

void Mediator::AddUnitsToAttackers(const Units units)
{
	for (const auto& unit : units)
	{
		fire_control_manager.AddUnit(unit);
	}
}

void Mediator::ConfirmAttack(const Unit* attacker, const Unit* target)
{
	fire_control_manager.ConfirmAttack(attacker, target);
}

void Mediator::CancelAttack(const Unit* unit)
{
	fire_control_manager.CancelAttack(unit);
}

bool Mediator::GetAttackStatus(const Unit* unit)
{
	return fire_control_manager.GetAttackStatus(unit);
}

// TODO make these boolean if the command is invalid
void Mediator::SetUnitCommand(const Unit* unit, AbilityID ability, int prio, bool queued_command)
{
	unit_command_manager.SetUnitCommand(unit, ability, prio, queued_command);
}

void Mediator::SetUnitCommand(const Unit* unit, AbilityID ability, const Point2D& point, int prio, bool queued_command)
{
	unit_command_manager.SetUnitCommand(unit, ability, point, prio, queued_command);
}

void Mediator::SetUnitCommand(const Unit* unit, AbilityID ability, const Unit* target, int prio, bool queued_command)
{
	unit_command_manager.SetUnitCommand(unit, ability, target, prio, queued_command);
}

void Mediator::SetUnitsCommand(const Units& units, AbilityID ability, int prio, bool queued_command)
{
	unit_command_manager.SetUnitsCommand(units, ability, prio, queued_command);
}

void Mediator::SetUnitsCommand(const Units& units, AbilityID ability, const Point2D& point, int prio, bool queued_command)
{
	unit_command_manager.SetUnitsCommand(units, ability, point, prio, queued_command);
}

void Mediator::SetUnitsCommand(const Units& units, AbilityID ability, const Unit* target, int prio, bool queued_command)
{
	unit_command_manager.SetUnitsCommand(units, ability, target, prio, queued_command);
}

void Mediator::OnBuildingConstructionComplete(const Unit* building)
{
	if (building->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
	{
		worker_manager.SetNewBase(building);
	}
	else if (building->unit_type == UNIT_TYPEID::PROTOSS_ASSIMILATOR)
	{
		worker_manager.AddAssimilator(building);
		if (worker_manager.immediatelySaturateGasses)
			worker_manager.SaturateGas(building);
		else if (worker_manager.immediatelySemiSaturateGasses)
			worker_manager.SemiSaturateGas(building);
	}
	unit_production_manager.OnBuildingConstructionComplete(building);
}

#pragma warning(push)
#pragma warning(disable : 4100)
void Mediator::OnNeutralUnitCreated(const Unit* unit)
{

}
#pragma warning(pop)

void Mediator::OnUnitCreated(const Unit* unit)
{
	if (unit->unit_type == PROBE)
	{
		worker_manager.PlaceWorker(unit);
	}
	else if (unit->unit_type == NEXUS)
	{
		army_manager.NexusStarted();
	}
	else
	{
		// dont need this check?
		if (std::find(ALL_ARMY_UNITS.begin(), ALL_ARMY_UNITS.end(), unit->unit_type) != ALL_ARMY_UNITS.end())
		{
			army_manager.FindArmyGroupForUnit(unit);
		}
		ability_manager.OnUnitCreated(unit);
	}
}

void Mediator::OnUnitEnterVision(const Unit*)
{

}

#pragma warning(push)
#pragma warning(disable : 4100)
void Mediator::OnUnitDamaged(const Unit* unit, float health_damage, float shields_damage)
{
	if (unit->is_building && unit->build_progress < 1)
	{
		if (Utility::DangerLevelAt(unit, unit->pos, agent->Observation()) >= unit->health + unit->shield)
			CancelBuilding(unit);
	}
}
#pragma warning(pop)

void Mediator::OnUnitDestroyed(const Unit* unit)
{
	worker_manager.OnUnitDestroyed(unit);
	ability_manager.OnUnitDestroyed(unit);

	if (std::find(ALL_ARMY_UNITS.begin(), ALL_ARMY_UNITS.end(), unit->unit_type) != ALL_ARMY_UNITS.end())
	{
		army_manager.OnUnitDestroyed(unit);
	}
	scouting_manager.OnUnitDestroyed(unit);

	if (unit->unit_type == NEXUS)
	{
		army_manager.RemoveDefenseGroupAt(unit->pos);
		defense_manager.RemoveOngoingAttackAt(unit->pos);
	}

	if (unit->is_building)
		RebuildBuilding(unit->pos, unit->unit_type);
}
void Mediator::OnUpgradeCompleted(UPGRADE_ID upgrade)
{
	upgrade_manager.OnUpgradeCompleted(upgrade);
	ability_manager.OnUpgradeCompleted(upgrade);
}



void Mediator::AddListenerToOnUnitDamagedEvent(int id, std::function<void(const Unit*, float, float)> func)
{
	agent->AddListenerToOnUnitDamagedEvent(id, func);
}

void Mediator::RemoveListenerToOnUnitDamagedEvent(int id)
{
	agent->RemoveListenerToOnUnitDamagedEvent(id);
}

void Mediator::AddListenerToOnUnitDestroyedEvent(int id, std::function<void(const Unit*)> func)
{
	agent->AddListenerToOnUnitDestroyedEvent(id, func);
}

void Mediator::RemoveListenerToOnUnitDestroyedEvent(int id)
{
	agent->RemoveListenerToOnUnitDestroyedEvent(id);
}

void Mediator::AddListenerToOnUnitCreatedEvent(int id, std::function<void(const Unit*)> func)
{
	agent->AddListenerToOnUnitCreatedEvent(id, func);
}

void Mediator::RemoveListenerToOnUnitCreatedEvent(int id)
{
	agent->RemoveListenerToOnUnitCreatedEvent(id);
}

void Mediator::AddListenerToOnUnitEntersVisionEvent(int id, std::function<void(const Unit*)> func)
{
	agent->AddListenerToOnUnitEntersVisionEvent(id, func);
}

void Mediator::RemoveListenerToOnUnitEntersVisionEvent(int id)
{
	agent->RemoveListenerToOnUnitEntersVisionEvent(id);
}



}