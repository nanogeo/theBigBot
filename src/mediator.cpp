
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2api/sc2_types.h"

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
#include "stalker_base_defense_terran_state_machine.h"




namespace sc2
{
	void Mediator::SetUpManagers(BuildOrder curr_build_order)
	{
		worker_manager.new_base = agent->Observation()->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS))[0];
		SetBuildOrder(curr_build_order);
		army_manager.SetUpInitialArmies();
	}

	void Mediator::RunManagers()
	{
		worker_manager.DistributeWorkers();
		if (worker_manager.new_base != NULL)
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
			worker_manager.BuildWorkers();

		build_order_manager.CheckBuildOrder();

		action_manager.ProcessActions();

		finite_state_machine_manager.RunStateMachines();

		army_manager.RunArmyGroups();

		unit_command_manager.ParseUnitCommands();
	}

	int  Mediator::GetGameLoop()
	{
		return agent->Observation()->GetGameLoop();
	}

	double Mediator::GetCurrentTime()
	{
		return agent->Observation()->GetGameLoop() / 22.4;
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

	bool Mediator::HasBuildingStarted(UNIT_TYPEID buildingId)
	{

		for (const auto& building : agent->Observation()->GetUnits(IsFriendlyUnit(buildingId)))
		{
			return true;
		}
		return false;
	}

	bool Mediator::IsResearching()
	{
		return false;
	}

	bool Mediator::HasResources(int minerals, int vespene, int supply)
	{
		bool enough_minerals = agent->Observation()->GetMinerals() >= minerals;
		bool enough_vespene = agent->Observation()->GetVespene() >= vespene;
		bool enough_supply = agent->Observation()->GetFoodCap() - agent->Observation()->GetFoodUsed() >= supply;
		return enough_minerals && enough_vespene && enough_supply;
	}

	int Mediator::GetNumUnits(UNIT_TYPEID unitId)
	{
		return agent->Observation()->GetUnits(IsFriendlyUnit(unitId)).size();
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

	const Unit* Mediator::GetBuilder(Point2D position)
	{
		return worker_manager.GetBuilder(position);
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
		return worker_manager.far_3_mineral_patch_extras.size();
	}

	bool Mediator::SendScout()
	{
		/*Point2D pos = mediator->GetEnemyStartLocation();
		const Unit* scouter = mediator->GetBuilder(pos);
		if (scouter == NULL)
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

	bool Mediator::SendCannonRushProbe1()
	{
		/*Point2D pos = mediator->GetEnemyStartLocation();
		const Unit* scouter = mediator->GetBuilder(pos);
		if (scouter == NULL)
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

	void Mediator::SetImmediatlySaturateGasses(bool value)
	{
		worker_manager.immediatelySaturateGasses = value;// TODO move to worker manager
	}

	void Mediator::SetImmediatlySemiSaturateGasses(bool value)
	{
		worker_manager.immediatelySemiSaturateGasses = value; // TODO move to worker manager
	}

	void Mediator::SetBuildOrder(BuildOrder build)
	{
		agent->locations = new Locations(ToPoint3D(GetStartLocation()), build, GetMapName());
		build_order_manager.SetBuildOrder(build);
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

		if (build_order_manager.current_build_order == BuildOrder::recessed_cannon_rush)
		{

			std::vector<UNIT_TYPEID> tech_buildings = { FORGE, TWILIGHT, ROBO, ROBO_BAY, STARGATE, FLEET_BEACON, DARK_SHRINE, TEMPLAR_ARCHIVES };
			if (std::find(tech_buildings.begin(), tech_buildings.end(), unit_type) != tech_buildings.end())
			{
				possible_locations = agent->locations->tech_locations_cannon_rush;
			}
			else
			{
				switch (unit_type)
				{
				case PYLON:
					possible_locations = agent->locations->pylon_locations_cannon_rush;
					break;
				case NEXUS:
					possible_locations = agent->locations->nexi_locations;
					break;
				case GATEWAY:
					possible_locations = agent->locations->gateway_locations_cannon_rush;
					break;
				case ASSIMILATOR:
					possible_locations = agent->locations->assimilator_locations;
					break;
				case CANNON:
					possible_locations = agent->locations->cannon_locations_cannon_rush;
					break;
				case BATTERY:
					possible_locations = agent->locations->shield_battery_locations_cannon_rush;
					break;
				default:
					//std::cout << "Error invalid type id in GetLocation" << std::endl;
					return Point2D(0, 0);
				}
			}
		}
		else
		{
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
		std::cerr << "Possible locations:" << std::endl;
		return Point2D(0, 0);
	}

	std::vector<Point2D> Mediator::GetProxyLocations(UNIT_TYPEID unit_type)
	{
		return agent->GetProxyLocations(unit_type);
	}

	Point2D Mediator::GetNaturalDefensiveLocation(UNIT_TYPEID unit_type)
	{
		return agent->locations->defensive_natural_battery_locations[0];
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
		case Race::Random:
			return agent->locations->first_pylon_location_protoss;
			break;
		}
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

	ScoutInfoTerran Mediator::GetScoutInfoTerran()
	{
		return agent->scout_info_terran;
	}

	ScoutInfoZerg Mediator::GetScoutInfoZerg()
	{
		return agent->scout_info_zerg;
	}

	StateMachine* Mediator::GetStateMachineByName(std::string name)
	{
		for (auto& fsm : finite_state_machine_manager.active_state_machines)
		{
			if (fsm->name == name)
				return fsm;
		}
		return NULL;
	}

	void Mediator::RemoveStateMachine(StateMachine* state_machine)
	{
		finite_state_machine_manager.RemoveStateMachine(state_machine);
	}

	void Mediator::CreateFourGateBlinkFSM()
	{
		ArmyGroup* army = CreateArmyGroup(ArmyRole::outside_control, { STALKER, PRISM }, 15, 25);
		army->using_standby = true;
		army->standby_pos = agent->locations->blink_presure_consolidation;
		for (const auto& fsm : finite_state_machine_manager.active_state_machines)
		{
			if (fsm->name == "Stalker base defense")
			{
				finite_state_machine_manager.active_state_machines.erase(std::remove(finite_state_machine_manager.active_state_machines.begin(), 
					finite_state_machine_manager.active_state_machines.end(), fsm), finite_state_machine_manager.active_state_machines.end());
				break;
			}
		}
		for (const auto& unit : GetUnits(IsUnits({ PRISM, STALKER })))
		{
			if (IsUnitOccupied(unit))
				continue;
			army->AddUnit(unit);
		}
		BlinkStalkerAttackTerran* blink_fsm = new BlinkStalkerAttackTerran(agent, "4 gate blink pressure", army, agent->locations->blink_presure_consolidation,
			agent->locations->blink_pressure_prism_consolidation, agent->locations->blink_pressure_blink_up, agent->locations->blink_pressure_blink_down);
		finite_state_machine_manager.active_state_machines.push_back(blink_fsm);
	}

	void Mediator::CreateAdeptHarassProtossFSM()
	{
		AdeptHarassProtoss* adept_fsm = new AdeptHarassProtoss(agent, "adept harass protoss", GetUnits(IsFriendlyUnit(ADEPT)), agent->locations->adept_harrass_protoss_consolidation);
		finite_state_machine_manager.active_state_machines.push_back(adept_fsm);
	}

	void Mediator::AddOraclesToOracleHarassFSM()
	{
		Units oracles = GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ORACLE));
		for (const auto& fsm : finite_state_machine_manager.active_state_machines)
		{
			if (dynamic_cast<OracleHarassStateMachine*>(fsm))
			{
				OracleHarassStateMachine* state_machine = dynamic_cast<OracleHarassStateMachine*>(fsm);
				for (const auto& oracle : oracles)
				{
					if (std::find(state_machine->oracles.begin(), state_machine->oracles.end(), oracle) == state_machine->oracles.end())
					{
						state_machine->AddOracle(oracle);
					}
				}
			}
		}
	}

	bool Mediator::RemoveScoutToProxy(UNIT_TYPEID unitId, int amount)
	{
		const Unit* scout = NULL;
		Race enemy_race = GetEnemyRace();
		StateMachine* state_machine;
		switch (enemy_race)
		{
		case Race::Zerg:
			state_machine = GetStateMachineByName("Scout Zerg");
			scout = ((ScoutZergStateMachine*)state_machine)->scout;
			break;
		case Race::Terran:
			state_machine = GetStateMachineByName("Scout Terran");
			scout = ((ScoutTerranStateMachine*)state_machine)->scout;
			break;
		}

		if (scout != NULL && scout->is_alive)
		{
			RemoveStateMachine(state_machine);

			Point2D pos = GetProxyLocations(PYLON)[0];
			SetUnitCommand(scout, ABILITY_ID::GENERAL_MOVE, pos);
			action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionRemoveScoutToProxy, new ActionArgData(scout, unitId, pos, amount)));
			return true;
		}
		return false;
	}

	void Mediator::CreateAdeptBaseDefenseTerranFSM()
	{
		const Unit* adept = GetUnits(IsFriendlyUnit(ADEPT))[0];
		AdeptBaseDefenseTerran* adept_defense_fsm = new AdeptBaseDefenseTerran(agent, "Adept base defense", adept, 
			agent->locations->main_early_dead_space, agent->locations->natural_front);
		finite_state_machine_manager.active_state_machines.push_back(adept_defense_fsm);
	}

	void Mediator::DefendThirdBaseZerg()
	{
		army_manager.CreateArmyGroup(ArmyRole::defend_third, { ADEPT }, 1, 1);

		StateMachine* oracle_fsm = new OracleHarassStateMachine(agent, agent->Observation()->GetUnits(Unit::Alliance::Self, IsUnit(ORACLE)), 
			agent->locations->third_base_pylon_gap, agent->locations->natural_door_closed, "Oracles");
		finite_state_machine_manager.active_state_machines.push_back(oracle_fsm);
	}

	void Mediator::PlaceWorker(const Unit* worker)
	{
		worker_manager.PlaceWorker(worker);
	}

	void Mediator::RemoveWorker(const Unit* worker)
	{
		worker_manager.RemoveWorker(worker);
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

	void Mediator::SetWarpInAtProxy(bool status)
	{
		unit_production_manager.SetWarpInAtProxy(status);
	}

	ArmyGroup* Mediator::CreateArmyGroup(ArmyRole role, std::vector<UNIT_TYPEID> unit_types, int desired_units, int max_units)
	{
		return army_manager.CreateArmyGroup(role, unit_types, desired_units, max_units);
	}

	Point2D Mediator::GetStartLocation()
	{
		return agent->Observation()->GetStartLocation();
	}

	Point2D Mediator::GetEnemyStartLocation()
	{
		return agent->Observation()->GetGameInfo().enemy_start_locations[0];
	}

	PathManager Mediator::GetDirectAttackLine()
	{
		return agent->locations->attack_path_short_line;
	}

	std::vector<Point2D> Mediator::GetDirectAttackPath()
	{
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

	std::string Mediator::GetMapName()
	{
		return agent->Observation()->GetGameInfo().map_name;
	}

	Race Mediator::GetEnemyRace()
	{
		return agent->enemy_race;
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

	// TODO make these boolean if the command is invalid
	void Mediator::SetUnitCommand(const Unit* unit, AbilityID ability, bool queued_command)
	{
		unit_command_manager.SetUnitCommand(unit, ability, queued_command);
	}

	void Mediator::SetUnitCommand(const Unit* unit, AbilityID ability, const Point2D& point, bool queued_command)
	{
		unit_command_manager.SetUnitCommand(unit, ability, point, queued_command);
	}

	void Mediator::SetUnitCommand(const Unit* unit, AbilityID ability, const Unit* target, bool queued_command)
	{
		unit_command_manager.SetUnitCommand(unit, ability, target, queued_command);
	}

	void Mediator::SetUnitsCommand(const Units& units, AbilityID ability, bool queued_command)
	{
		unit_command_manager.SetUnitsCommand(units, ability, queued_command);
	}

	void Mediator::SetUnitsCommand(const Units& units, AbilityID ability, const Point2D& point, bool queued_command)
	{
		unit_command_manager.SetUnitsCommand(units, ability, point, queued_command);
	}

	void Mediator::SetUnitsCommand(const Units& units, AbilityID ability, const Unit* target, bool queued_command)
	{
		unit_command_manager.SetUnitsCommand(units, ability, target, queued_command);
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
	void Mediator::OnNeutralUnitCreated(const Unit* unit)
	{

	}
	void Mediator::OnUnitCreated(const Unit* unit)
	{
		if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE)
		{
			worker_manager.PlaceWorker(unit);
		}
		else
		{
			// dont need this check?
			if (std::find(ALL_ARMY_UNITS.begin(), ALL_ARMY_UNITS.end(), unit->unit_type) != ALL_ARMY_UNITS.end())
			{
				army_manager.FindArmyGroupForUnit(unit);
			}
		}
	}
	void Mediator::OnUnitEnterVision(const Unit*)
	{

	}
	void Mediator::OnUnitDamaged(const Unit* unit, float health_damage, float shields_damage)
	{
		
	}
	void Mediator::OnUnitDestroyed(const Unit* unit)
	{
		if (unit->mineral_contents > 0)
			worker_manager.RemoveSpentMineralPatch(unit);

		if (std::find(ALL_ARMY_UNITS.begin(), ALL_ARMY_UNITS.end(), unit->unit_type) != ALL_ARMY_UNITS.end())
		{
			army_manager.OnUnitDestroyed(unit);
		}
	}
	void Mediator::OnUpgradeCompleted(UPGRADE_ID upgrade)
	{
		upgrade_manager.OnUpgradeCompleted(upgrade);
	}
}