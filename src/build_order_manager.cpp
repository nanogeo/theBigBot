#include "build_order_manager.h"
#include "TossBot.h"
#include "utility.h"
#include "locations.h"
#include "finish_state_machine.h"
#include "action_manager.h"
#include "army_group.h"

#include <algorithm>
#include <vector>

namespace sc2
{

void BuildOrderManager::CheckBuildOrder()
{
	if (build_order_step < build_order.size())
	{
		BuildOrderData current_step = build_order[build_order_step];
		bool(sc2::BuildOrderManager::*condition)(BuildOrderConditionArgData) = current_step.condition;
		BuildOrderConditionArgData condition_arg = current_step.condition_arg;
		if ((*this.*condition)(condition_arg))
		{
			bool(sc2::BuildOrderManager::*result)(BuildOrderResultArgData) = current_step.result;
			BuildOrderResultArgData result_arg = current_step.result_arg;
			if ((*this.*result)(result_arg))
			{
				build_order_step++;
			}
		}
	}
}

bool BuildOrderManager::TimePassed(BuildOrderConditionArgData data)
{
	return agent->Observation()->GetGameLoop() / 22.4 >= data.time;
}

bool BuildOrderManager::NumWorkers(BuildOrderConditionArgData data)
{
	return agent->Observation()->GetFoodWorkers() >= data.amount;
}

bool BuildOrderManager::HasBuilding(BuildOrderConditionArgData data)
{
	for (const auto &building : agent->Observation()->GetUnits(IsUnit(data.unitId)))
	{
		if (building->build_progress == 1)
		{
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::HasBuildingStarted(BuildOrderConditionArgData data)
{
	for (const auto &building : agent->Observation()->GetUnits(IsUnit(data.unitId)))
	{
		return true;
	}
	return false;
}

bool BuildOrderManager::IsResearching(BuildOrderConditionArgData data)
{
	for (const auto &building : agent->Observation()->GetUnits(IsUnit(data.unitId)))
	{
		if (!building->orders.empty())
			return true;
	}
	return false;
}

bool BuildOrderManager::HasGas(BuildOrderConditionArgData data)
{
	return agent->Observation()->GetVespene() >= data.amount;
}

bool BuildOrderManager::HasUnits(BuildOrderConditionArgData data)
{
	if (agent->Observation()->GetUnits(IsUnit(data.unitId)).size() >= data.amount)
	{
		return true;
	}
	return false;
}



bool BuildOrderManager::BuildBuilding(BuildOrderResultArgData data)
{
	Point2D pos = agent->GetLocation(data.unitId);
	const Unit* builder = agent->worker_manager.GetBuilder(pos);
	if (builder == NULL)
	{
		std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	agent->worker_manager.RemoveWorker(builder);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(builder, data.unitId, pos)));
	return true;
}

bool BuildOrderManager::BuildFirstPylon(BuildOrderResultArgData data)
{
	Point2D pos;
	if (current_build_order == BuildOrder::recessed_cannon_rush)
	{
		pos = agent->locations->first_pylon_cannon_rush;
	}
	else
	{
		switch (agent->enemy_race)
		{
		case Race::Zerg:
			pos = agent->locations->first_pylon_location_zerg;
			break;
		case Race::Protoss:
			pos = agent->locations->first_pylon_location_protoss;
			break;
		case Race::Terran:
			pos = agent->locations->first_pylon_location_terran;
			break;
		}
	}

	const Unit* builder = agent->worker_manager.GetBuilder(pos);
	if (builder == NULL)
	{
		std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	agent->worker_manager.RemoveWorker(builder);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(builder, data.unitId, pos)));
	return true;
}

bool BuildOrderManager::BuildBuildingMulti(BuildOrderResultArgData data)
{
	Point2D pos = agent->GetLocation(data.unitIds[0]);
	const Unit* builder = agent->worker_manager.GetBuilder(pos);
	if (builder == NULL)
	{
		std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	agent->worker_manager.RemoveWorker(builder);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuildingMulti, new ActionArgData(builder, data.unitIds, pos, 0)));
	return true;
}

bool BuildOrderManager::BuildProxyMulti(BuildOrderResultArgData data)
{
	Point2D pos = agent->GetProxyLocation(data.unitIds[0]);
	const Unit* builder = agent->worker_manager.GetBuilder(pos);
	if (builder == NULL)
	{
		std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	agent->worker_manager.RemoveWorker(builder);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionBuildProxyMulti, new ActionArgData(builder, data.unitIds, pos, 0)));
	return true;
}

bool BuildOrderManager::Scout(BuildOrderResultArgData data)
{
	Point2D pos = agent->Observation()->GetGameInfo().enemy_start_locations[0];
	const Unit* scouter = agent->worker_manager.GetBuilder(pos);
	if (scouter == NULL)
	{
		std::cout << "Error could not find builder in Scout" << std::endl;
		return false;
	}
	agent->worker_manager.RemoveWorker(scouter);
	if (agent->enemy_race == Race::Zerg)
	{
		ScoutZergStateMachine* scout_fsm = new ScoutZergStateMachine(agent, "Scout Zerg", scouter, agent->locations->initial_scout_pos, agent->locations->main_scout_path, agent->locations->natural_scout_path, agent->locations->enemy_natural, agent->locations->possible_3rds);
		agent->active_FSMs.push_back(scout_fsm);
	}
	else
	{
		ScoutTerranStateMachine* scout_fsm = new ScoutTerranStateMachine(agent, "Scout Terran", scouter, agent->locations->initial_scout_pos, agent->locations->main_scout_path, agent->locations->natural_scout_path, agent->locations->enemy_natural);
		agent->active_FSMs.push_back(scout_fsm);
	}
	return true;
}

bool BuildOrderManager::CannonRushProbe1(BuildOrderResultArgData data)
{
	Point2D pos = agent->Observation()->GetGameInfo().enemy_start_locations[0];
	const Unit* scouter = agent->worker_manager.GetBuilder(pos);
	if (scouter == NULL)
	{
		std::cout << "Error could not find builder in Scout" << std::endl;
		return false;
	}
	agent->worker_manager.RemoveWorker(scouter);
	if (agent->enemy_race == Race::Zerg)
	{
		ScoutZergStateMachine* scout_fsm = new ScoutZergStateMachine(agent, "Scout Zerg", scouter, agent->locations->initial_scout_pos, agent->locations->main_scout_path, agent->locations->natural_scout_path, agent->locations->enemy_natural, agent->locations->possible_3rds);
		agent->active_FSMs.push_back(scout_fsm);
	}
	else
	{
		ScoutTerranStateMachine* scout_fsm = new ScoutTerranStateMachine(agent, "Scout Terran", scouter, agent->locations->initial_scout_pos, agent->locations->main_scout_path, agent->locations->natural_scout_path, agent->locations->enemy_natural);
		agent->active_FSMs.push_back(scout_fsm);
	}
	return true;
}

bool BuildOrderManager::CutWorkers(BuildOrderResultArgData data)
{
	agent->worker_manager.should_build_workers = false;
	return true;
}

bool BuildOrderManager::UncutWorkers(BuildOrderResultArgData data)
{
	agent->worker_manager.should_build_workers = true;
	return true;
}

bool BuildOrderManager::ImmediatelySaturateGasses(BuildOrderResultArgData data)
{
	agent->immediatelySaturateGasses = true;
	return true;
}

bool BuildOrderManager::TrainStalker(BuildOrderResultArgData data)
{
	if (Utility::CanAfford(UNIT_TYPEID::PROTOSS_STALKER, 1, agent->Observation()))
	{
		for (const auto &gateway : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY)))
		{
			if (gateway->build_progress == 1 && gateway->orders.size() == 0)
			{
				if (agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)).size() > 0)
				{
					agent->Actions()->UnitCommand(gateway, ABILITY_ID::TRAIN_STALKER);
					return true;
				}
				/*for (const auto & ability : agent->Query()->GetAbilitiesForUnit(gateway).abilities)
				{
					if (ability.ability_id.ToType() == ABILITY_ID::TRAIN_STALKER)
					{
						agent->Actions()->UnitCommand(gateway, ABILITY_ID::TRAIN_STALKER);
						return true;
					}
				}*/
			}
		}
	}
	return false;
}

bool BuildOrderManager::TrainAdept(BuildOrderResultArgData data)
{
	if (Utility::CanAfford(UNIT_TYPEID::PROTOSS_ADEPT, 1, agent->Observation()))
	{
		for (const auto &gateway : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY)))
		{
			if (gateway->build_progress == 1 && gateway->orders.size() == 0)
			{
				if (agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)).size() > 0)
				{
					agent->Actions()->UnitCommand(gateway, ABILITY_ID::TRAIN_ADEPT);
					return true;
				}
				/*for (const auto & ability : agent->Query()->GetAbilitiesForUnit(gateway).abilities)
				{
					if (ability.ability_id.ToType() == ABILITY_ID::TRAIN_ADEPT)
					{
						agent->Actions()->UnitCommand(gateway, ABILITY_ID::TRAIN_ADEPT);
						return true;
					}
				}*/
			}
		}
	}
	return false;
}

bool BuildOrderManager::TrainOracle(BuildOrderResultArgData data)
{
	if (Utility::CanAfford(UNIT_TYPEID::PROTOSS_ORACLE, 1, agent->Observation()))
	{
		for (const auto &stargate : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_STARGATE)))
		{
			if (stargate->build_progress == 1 && stargate->orders.size() == 0)
			{
				for (const auto & ability : agent->Query()->GetAbilitiesForUnit(stargate).abilities)
				{
					if (ability.ability_id.ToType() == ABILITY_ID::TRAIN_ORACLE)
					{
						agent->Actions()->UnitCommand(stargate, ABILITY_ID::TRAIN_ORACLE);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool BuildOrderManager::TrainPrism(BuildOrderResultArgData data)
{
	if (Utility::CanAfford(UNIT_TYPEID::PROTOSS_WARPPRISM, 1, agent->Observation()))
	{
		for (const auto &robo : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
		{
			if (robo->build_progress == 1 && robo->orders.size() == 0)
			{
				agent->Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_WARPPRISM);
				return true;
				/*for (const auto & ability : agent->Query()->GetAbilitiesForUnit(robo).abilities)
				{
					if (ability.ability_id.ToType() == ABILITY_ID::TRAIN_WARPPRISM)
					{
						agent->Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_WARPPRISM);
						return true;
					}
				}*/
			}
		}
	}
	return false;
}

bool BuildOrderManager::TrainObserver(BuildOrderResultArgData data)
{
	if (Utility::CanAfford(UNIT_TYPEID::PROTOSS_OBSERVER, 1, agent->Observation()))
	{
		for (const auto &robo : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
		{
			if (robo->build_progress == 1 && robo->orders.size() == 0)
			{
				agent->Actions()->UnitCommand(robo, ABILITY_ID::TRAIN_OBSERVER);
				return true;
			}
		}
	}
	return false;
}

bool BuildOrderManager::ChronoBuilding(BuildOrderResultArgData data)
{
	for (const auto &building : agent->Observation()->GetUnits(IsUnit(data.unitId)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
		{
			for (const auto &nexus : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
			{
				if (nexus->energy >= 50 && nexus->build_progress == 1)
				{
					agent->Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building);
					return true;
				}
				/*for (const auto &ability : agent->Query()->GetAbilitiesForUnit(nexus).abilities)
				{
					if (ability.ability_id == ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST)
					{
						agent->Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building);
						return true;
					}
				}*/
			}
		}
	}
	return false;
}

bool BuildOrderManager::ResearchWarpgate(BuildOrderResultArgData data)
{
	for (const auto &cyber : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)))
	{
		if (Utility::CanAffordUpgrade(UPGRADE_ID::WARPGATERESEARCH, agent->Observation()))
		{
			agent->Actions()->UnitCommand(cyber, ABILITY_ID::RESEARCH_WARPGATE);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ResearchBlink(BuildOrderResultArgData data)
{
	for (const auto &twilight : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)))
	{
		if (Utility::CanAffordUpgrade(UPGRADE_ID::BLINKTECH, agent->Observation()) && twilight->orders.size() == 0)
		{
			agent->Actions()->UnitCommand(twilight, ABILITY_ID::RESEARCH_BLINK);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ResearchCharge(BuildOrderResultArgData data)
{
	for (const auto &twilight : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)))
	{
		if (Utility::CanAffordUpgrade(UPGRADE_ID::CHARGE, agent->Observation()) && twilight->orders.size() == 0)
		{
			agent->Actions()->UnitCommand(twilight, ABILITY_ID::RESEARCH_CHARGE);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ResearchGlaives(BuildOrderResultArgData data)
{
	for (const auto &twilight : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)))
	{
		if (Utility::CanAffordUpgrade(UPGRADE_ID::ADEPTPIERCINGATTACK, agent->Observation()) && twilight->orders.size() == 0)
		{
			agent->Actions()->UnitCommand(twilight, ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ResearchDTBlink(BuildOrderResultArgData data)
{
	for (const auto &dark_shrine : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_DARKSHRINE)))
	{
		if (Utility::CanAffordUpgrade(UPGRADE_ID::DARKTEMPLARBLINKUPGRADE, agent->Observation()) && dark_shrine->orders.size() == 0)
		{
			agent->Actions()->UnitCommand(dark_shrine, ABILITY_ID::RESEARCH_SHADOWSTRIKE);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ResearchAttackOne(BuildOrderResultArgData data)
{
	for (const auto &forge : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_FORGE)))
	{
		if (Utility::CanAffordUpgrade(UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1, agent->Observation()))
		{
			agent->Actions()->UnitCommand(forge, ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ResearchAttackTwo(BuildOrderResultArgData data)
{
	for (const auto &forge : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_FORGE)))
	{
		if (forge->orders.size() == 0 && Utility::CanAffordUpgrade(UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2, agent->Observation()))
		{
			agent->Actions()->UnitCommand(forge, ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL2);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ResearchShieldsOne(BuildOrderResultArgData data)
{
	for (const auto &forge : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_FORGE)))
	{
		if (forge->orders.size() == 0 && Utility::CanAffordUpgrade(UPGRADE_ID::PROTOSSSHIELDSLEVEL1, agent->Observation()))
		{
			agent->Actions()->UnitCommand(forge, ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL1);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ResearchAirAttackOne(BuildOrderResultArgData data)
{
	for (const auto &forge : agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)))
	{
		if (forge->orders.size() == 0 && Utility::CanAffordUpgrade(UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1, agent->Observation()))
		{
			agent->Actions()->UnitCommand(forge, ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL1);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ChronoTillFinished(BuildOrderResultArgData data)
{
	for (const auto &building : agent->Observation()->GetUnits(IsFinishedUnit(data.unitId)))
	{
		if (building->orders.size() > 0)
		{
			agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionChronoTillFinished, new ActionArgData(building, data.unitId)));
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::WarpInAtProxy(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionWarpInAtProxy, new ActionArgData()));
	return true;
}

bool BuildOrderManager::BuildProxy(BuildOrderResultArgData data)
{
	std::vector<Point2D> building_locations = agent->GetProxyLocations(data.unitId);

	Point2D pos = building_locations[0];
	const Unit* builder = agent->worker_manager.GetBuilder(pos);
	if (builder == NULL)
	{
		std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	agent->worker_manager.RemoveWorker(builder);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(builder, data.unitId, pos)));
	return true;
}

bool BuildOrderManager::ContinueBuildingPylons(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueBuildingPylons, new ActionArgData()));
	return true;
}

bool BuildOrderManager::ContinueMakingWorkers(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueMakingWorkers, new ActionArgData()));
	return true;
}

bool BuildOrderManager::ContinueUpgrades(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueUpgrades, new ActionArgData()));
	return true;
}

bool BuildOrderManager::ContinueChronos(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueChronos, new ActionArgData()));
	return true;
}

bool BuildOrderManager::ContinueExpanding(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueExpanding, new ActionArgData()));
	return true;
}

bool BuildOrderManager::TrainFromProxy(BuildOrderResultArgData data)
{
	if (data.unitId == UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)
	{
		std::vector<const Unit*> proxy_robos;
		for (const auto &robo : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
		{
			if (Utility::DistanceToClosest(agent->GetProxyLocations(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY), robo->pos) < 2)
			{
				agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionTrainFromProxyRobo, new ActionArgData(robo)));
				return true;
			}
		}
	}
	return false;
}

bool BuildOrderManager::ContinueChronoProxyRobo(BuildOrderResultArgData data)
{
	if (data.unitId == UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)
	{
		std::vector<const Unit*> proxy_robos;
		for (const auto &robo : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)))
		{
			if (Utility::DistanceToClosest(agent->GetProxyLocations(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY), robo->pos) < 2)
			{
				agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionConstantChrono, new ActionArgData(robo)));
				return true;
			}
		}
	}
	return false;
}

bool BuildOrderManager::Contain(BuildOrderResultArgData data)
{
	ArmyGroup* army = new ArmyGroup(agent, {}, agent->locations->attack_path, agent->locations->high_ground_index);
	agent->army_groups.push_back(army);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContain, new ActionArgData(army)));
	return true;
}

bool BuildOrderManager::StalkerOraclePressure(BuildOrderResultArgData data)
{
	ArmyGroup* army = new ArmyGroup(agent, {}, agent->locations->attack_path, agent->locations->high_ground_index);
	Units oracles = agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ORACLE));
	for (const auto &fsm : agent->active_FSMs)
	{
		if (dynamic_cast<OracleHarassStateMachine*>(fsm))
		{
			OracleHarassStateMachine* state_machine = dynamic_cast<OracleHarassStateMachine*>(fsm);
			for (const auto &oracle : oracles)
			{
				if (std::find(state_machine->oracles.begin(), state_machine->oracles.end(), oracle) == state_machine->oracles.end())
				{
					state_machine->AddOracle(oracle);
				}
			}
			state_machine->attached_army_group = army;
		}
	}
	for (const auto &unit : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_STALKER)))
	{
		if (agent->UnitIsOccupied(unit))
			continue;
		army->AddNewUnit(unit);
	}
	army->AutoAddUnits({ UNIT_TYPEID::PROTOSS_STALKER, UNIT_TYPEID::PROTOSS_IMMORTAL, UNIT_TYPEID::PROTOSS_WARPPRISM, UNIT_TYPEID::PROTOSS_COLOSSUS, UNIT_TYPEID::PROTOSS_CARRIER });
	agent->army_groups.push_back(army);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionStalkerOraclePressure, new ActionArgData(army)));
	return true;
}

bool BuildOrderManager::ZealotDoubleprong(BuildOrderResultArgData data)
{
	ArmyGroup* army = new ArmyGroup(agent, {}, agent->locations->attack_path_alt, agent->locations->high_ground_index_alt);
	for (const auto &unit : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ZEALOT)))
	{
		if (agent->UnitIsOccupied(unit))
			continue;
		army->AddNewUnit(unit);
	}
	army->AutoAddUnits({ UNIT_TYPEID::PROTOSS_ZEALOT });
	agent->army_groups.push_back(army);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionZealotDoubleprong, new ActionArgData(army)));
	return true;
}

bool BuildOrderManager::MicroOracles(BuildOrderResultArgData data)
{
	//StateMachine* oracle_fsm = new StateMachine(agent, new OracleDefend(agent, agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ORACLE)), agent->GetLocations(UNIT_TYPEID::PROTOSS_NEXUS)[2]), "Oracles");
	//agent->active_FSMs.push_back(oracle_fsm);
	return true;
}

bool BuildOrderManager::OracleHarass(BuildOrderResultArgData data)
{
	Units oracles = agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ORACLE));
	for (const auto &fsm : agent->active_FSMs)
	{
		if (dynamic_cast<OracleHarassStateMachine*>(fsm))
		{
			OracleHarassStateMachine* state_machine = dynamic_cast<OracleHarassStateMachine*>(fsm);
			for (const auto &oracle : oracles)
			{
				if (std::find(state_machine->oracles.begin(), state_machine->oracles.end(), oracle) == state_machine->oracles.end())
				{
					state_machine->AddOracle(oracle);
				}
			}
		}
	}
	return true;
}

bool BuildOrderManager::SpawnUnits(BuildOrderResultArgData data)
{
	agent->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ZERGLING, agent->GetLocations(UNIT_TYPEID::PROTOSS_NEXUS)[2], 2, 15);
	return true;
}

bool BuildOrderManager::ConntinueWarpingInStalkers(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueWarpingInStalkers, new ActionArgData()));
	return true;
}

bool BuildOrderManager::StopWarpingInStalkers(BuildOrderResultArgData data)
{
	for (const auto &action : agent->action_manager.active_actions)
	{
		if (action->action == &ActionManager::ActionContinueWarpingInStalkers)
		{
			agent->action_manager.active_actions.erase(std::remove(agent->action_manager.active_actions.begin(), agent->action_manager.active_actions.end(), action), agent->action_manager.active_actions.end());
			return true;
		}
	}
}

bool BuildOrderManager::ConntinueVolleyWarpingInStalkers(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueVolleyWarpingInStalkers, new ActionArgData()));
	return true;
}

bool BuildOrderManager::StopVolleyWarpingInStalkers(BuildOrderResultArgData data)
{
	for (const auto &action : agent->action_manager.active_actions)
	{
		if (action->action == &ActionManager::ActionContinueVolleyWarpingInStalkers)
		{
			agent->action_manager.active_actions.erase(std::remove(agent->action_manager.active_actions.begin(), agent->action_manager.active_actions.end(), action), agent->action_manager.active_actions.end());
			return true;
		}
	}
}

bool BuildOrderManager::ContinueVolleyWarpingInZealots(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueVolleyWarpingInZealots, new ActionArgData()));
	return true;
}

bool BuildOrderManager::StopVolleyWarpingInZealots(BuildOrderResultArgData data)
{
	for (const auto &action : agent->action_manager.active_actions)
	{
		if (action->action == &ActionManager::ActionContinueVolleyWarpingInZealots)
		{
			agent->action_manager.active_actions.erase(std::remove(agent->action_manager.active_actions.begin(), agent->action_manager.active_actions.end(), action), agent->action_manager.active_actions.end());
			return true;
		}
	}
}

bool BuildOrderManager::ContinueBuildingCarriers(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueBuildingCarriers, new ActionArgData()));
	return true;
}

bool BuildOrderManager::WarpInUnits(BuildOrderResultArgData data)
{
	int warp_ins = data.amount;
	UnitTypeID type = data.unitId;
	AbilityID warp_ability = Utility::UnitToWarpInAbility(type);
	int gates_ready = 0;
	Units gates;
	for (const auto &warpgate : agent->warpgate_status)
	{
		if (warpgate.second.frame_ready == 0)
			gates.push_back(warpgate.first);
		/*for (const auto &ability : agent->Query()->GetAbilitiesForUnit(warpgate).abilities)
		{
			if (ability.ability_id == warp_ability)
			{
				gates_ready++;
				break;
			}
		}
		if (gates_ready >= warp_ins)
		{
			break;
		}*/
	}
	if (gates.size() > 0 && gates_ready >= warp_ins && Utility::CanAfford(type, warp_ins, agent->Observation()))
	{
		std::vector<Point2D> spots = agent->FindWarpInSpots(agent->Observation()->GetGameInfo().enemy_start_locations[0], gates.size());
		if (spots.size() >= warp_ins)
		{
			for (int i = 0; i < warp_ins; i++)
			{
				agent->Actions()->UnitCommand(gates[i], warp_ability, spots[i]);
			}
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::PullOutOfGas(BuildOrderResultArgData data)
{
	agent->worker_manager.removed_gas_miners += data.amount;
	Units workers;
	for (const auto &data : agent->worker_manager.assimilators)
	{
		if (data.second.workers[2] != NULL)
			workers.push_back(data.second.workers[2]);
	}
	for (const auto &data : agent->worker_manager.assimilators)
	{
		if (data.second.workers[1] != NULL)
			workers.push_back(data.second.workers[1]);
	}
	for (const auto &data : agent->worker_manager.assimilators)
	{
		if (data.second.workers[0] != NULL)
			workers.push_back(data.second.workers[0]);
	}
	int num_removed = 0;
	for (const auto &worker : workers)
	{
		if (num_removed >= data.amount)
			break;
		agent->worker_manager.RemoveWorker(worker);
		agent->worker_manager.PlaceWorker(worker);
		num_removed++;
	}
	return true;
}

bool BuildOrderManager::IncreaseExtraPylons(BuildOrderResultArgData data)
{
	agent->extra_pylons += data.amount;
	return true;
}

bool BuildOrderManager::MicroChargelotAllin(BuildOrderResultArgData data)
{
	if (agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_WARPPRISM)).size() > 0)
	{
		const Unit* prism = agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_WARPPRISM))[0];
		ChargelotAllInStateMachine* chargelotFSM = new ChargelotAllInStateMachine(agent, "Chargelot allin", agent->locations->warp_prism_locations, agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ZEALOT)), prism, agent->Observation()->GetGameLoop() / 22.4);
		agent->active_FSMs.push_back(chargelotFSM);
		return true;
	}
	return false;
}

bool BuildOrderManager::RemoveScoutToProxy(BuildOrderResultArgData data)
{
	for (const auto &fsm : agent->active_FSMs)
	{
		const Unit* scout = NULL;
		if (fsm->name == "Scout Zerg")
		{
			scout = ((ScoutZergStateMachine*)fsm)->scout;
		}
		else if (fsm->name == "Scout Terran")
		{
			scout = ((ScoutTerranStateMachine*)fsm)->scout;
		}
		if (scout != NULL)
		{
			agent->active_FSMs.erase(std::remove(agent->active_FSMs.begin(), agent->active_FSMs.end(), fsm), agent->active_FSMs.end());
			Point2D pos = agent->locations->proxy_pylon_locations[0];
			agent->Actions()->UnitCommand(scout, ABILITY_ID::MOVE_MOVE, pos);
			agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionRemoveScoutToProxy, new ActionArgData(scout, data.unitId, pos, data.amount)));
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::SafeRallyPoint(BuildOrderResultArgData data)
{
	for (const auto &building : agent->Observation()->GetUnits(IsUnit(data.unitId)))
	{
		Point2D pos = Utility::PointBetween(building->pos, agent->Observation()->GetStartLocation(), 2);
		agent->Actions()->UnitCommand(building, ABILITY_ID::SMART, pos);

		/*for (const auto & ability : agent->Query()->GetAbilitiesForUnit(building).abilities)
		{
			Point2D pos = Utility::PointBetween(building->pos, agent->Observation()->GetStartLocation(), 2);
			agent->Actions()->UnitCommand(building, ABILITY_ID::SMART, pos);
		}*/
	}
	return true;
}

bool BuildOrderManager::DTHarass(BuildOrderResultArgData data)
{
	if (agent->enemy_race == Race::Terran)
		agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionDTHarassTerran, new ActionArgData()));
	return true;
}

bool BuildOrderManager::UseProxyDoubleRobo(BuildOrderResultArgData data)
{
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionUseProxyDoubleRobo, new ActionArgData({ UNIT_TYPEID::PROTOSS_IMMORTAL, UNIT_TYPEID::PROTOSS_WARPPRISM, UNIT_TYPEID::PROTOSS_IMMORTAL, UNIT_TYPEID::PROTOSS_OBSERVER })));
	return true;
}

bool BuildOrderManager::MicroImmortalDrop(BuildOrderResultArgData data)
{
	const Unit* immortal1 = NULL;
	const Unit* immortal2 = NULL;
	for (const auto &immortal : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_IMMORTAL)))
	{
		if (immortal1 == NULL)
			immortal1 = immortal;
		else if (immortal2 == NULL)
			immortal2 = immortal;
	}
	ImmortalDropStateMachine* immortal_drop_fsm = new ImmortalDropStateMachine(agent, "Immortal Drop", immortal1, immortal2, agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_WARPPRISM))[0], agent->Observation()->GetGameInfo().enemy_start_locations[0], agent->locations->immortal_drop_prism_locations);
	agent->active_FSMs.push_back(immortal_drop_fsm);
	return true;
}

bool BuildOrderManager::ProxyDoubleRoboAllIn(BuildOrderResultArgData data)
{
	Units already_occupied;
	for (const auto &fsm : agent->active_FSMs)
	{
		if (dynamic_cast<ImmortalDropStateMachine*>(fsm))
		{
			already_occupied.push_back(((ImmortalDropStateMachine*)fsm)->prism);
			already_occupied.push_back(((ImmortalDropStateMachine*)fsm)->immortal1);
			already_occupied.push_back(((ImmortalDropStateMachine*)fsm)->immortal2);
		}
	}
	Units available_units;
	for (const auto &unit : agent->Observation()->GetUnits(IsUnits({ UNIT_TYPEID::PROTOSS_STALKER, UNIT_TYPEID::PROTOSS_OBSERVER, UNIT_TYPEID::PROTOSS_IMMORTAL })))
	{
		if (std::find(already_occupied.begin(), already_occupied.end(), unit) == already_occupied.end())
			available_units.push_back(unit);
	}
	ArmyGroup* army = new ArmyGroup(agent, available_units, agent->locations->attack_path_alt, agent->locations->high_ground_index_alt);
	agent->army_groups.push_back(army);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionAllIn, new ActionArgData(army)));
	return true;
}

bool BuildOrderManager::DefendThirdBase(BuildOrderResultArgData data)
{
	ArmyGroup* defenders = new ArmyGroup(agent);
	for (const auto &unit : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ADEPT)))
	{
		if (agent->UnitIsOccupied(unit))
			continue;
		defenders->AddUnit(unit);
		break;
	}
	for (const auto &unit : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ORACLE)))
	{
		defenders->AddUnit(unit);
	}
	agent->army_groups.push_back(defenders);
	defenders->DefendExpansion(agent->GetLocations(UNIT_TYPEID::PROTOSS_NEXUS)[2], agent->locations->third_base_pylon_gap);
	return true;
}

bool BuildOrderManager::SetDoorGuard(BuildOrderResultArgData data)
{
	ArmyGroup* defenders = new ArmyGroup(agent);
	for (const auto &unit : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_ADEPT)))
	{
		if (agent->UnitIsOccupied(unit))
			continue;
		defenders->AddUnit(unit);
		break;
	}
	agent->army_groups.push_back(defenders);
	defenders->DefendFrontDoor(agent->locations->natural_door_open, agent->locations->natural_door_closed);
	return true;
}

bool BuildOrderManager::RemoveProbe(BuildOrderResultArgData data)
{

	const Unit* builder = agent->worker_manager.GetBuilder(Point2D(0, 0));
	if (builder == NULL)
	{
		std::cout << "Error could not find builder in RemoveProbe" << std::endl;
		return false;
	}
	agent->worker_manager.RemoveWorker(builder);
	return true;
}


void BuildOrderManager::SetBuildOrder(BuildOrder build)
{
	agent->locations = new Locations(agent->Observation()->GetStartLocation(), build, agent->Observation()->GetGameInfo().map_name);
	current_build_order = build;
	switch (build)
	{
	case BuildOrder::blank:
		SetBlank();
		break;
	case BuildOrder::testing:
		SetTesting();
		break;
	case BuildOrder::blink_proxy_robo_pressure:
		SetBlinkProxyRoboPressureBuild();
		break;
	case BuildOrder::oracle_gatewayman_pvz:
		SetOracleGatewaymanPvZ();
		break;
	case BuildOrder::four_gate_blink:
		Set4GateBlink();
		break;
	case BuildOrder::chargelot_allin:
		SetChargelotAllin();
		break;
	case BuildOrder::chargelot_allin_old:
		SetChargelotAllinOld();
		break;
	case BuildOrder::four_gate_adept_pressure:
		Set4GateAdept();
		break;
	case BuildOrder::fastest_dts:
		SetFastestDTsPvT();
		break;
	case BuildOrder::proxy_double_robo:
		SetProxyDoubleRobo();
		break;
	default:
		std::cout << "Error invalid build order in SetBuildOrder" << std::endl;
	}
}

void BuildOrderManager::SetBlank()
{
	build_order = {};
}


void BuildOrderManager::SetTesting()
{
	build_order = { BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(1.0f),										&BuildOrderManager::CutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(3.0f),										&BuildOrderManager::DefendThirdBase,		BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(4.0f),										&BuildOrderManager::OracleHarass,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(5.0f),										&BuildOrderManager::StalkerOraclePressure,	BuildOrderResultArgData()),

	};
}

void BuildOrderManager::SetBlinkProxyRoboPressureBuild()
{
	build_order = { BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(6.5f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(17.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(40.5f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(60.5f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(70.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(82.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(85.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::NumWorkers,		BuildOrderConditionArgData(21),											&BuildOrderManager::CutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(117.0f),										&BuildOrderManager::ResearchWarpgate,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(117.0f),										&BuildOrderManager::Contain,					BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(136.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(136.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(137.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(152.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(159.0f),										&BuildOrderManager::BuildProxy,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)) ,
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(166.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(171.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(176.5f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ResearchBlink,			BuildOrderResultArgData(UPGRADE_ID::BLINKTECH)),
					BuildOrderData(&BuildOrderManager::IsResearching,	BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ChronoTillFinished,		BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(185.0f),										&BuildOrderManager::UncutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(185.0f),										&BuildOrderManager::WarpInAtProxy,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(205.0f),										&BuildOrderManager::BuildProxy,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(205.0f),										&BuildOrderManager::ContinueBuildingPylons,   BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(205.0f),										&BuildOrderManager::ContinueMakingWorkers,	BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY),		&BuildOrderManager::TrainFromProxy,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY),		&BuildOrderManager::ContinueChronoProxyRobo,  BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY))
	};
}

void BuildOrderManager::SetOracleGatewaymanPvZ()
{
	build_order = { BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(6.5f),										&BuildOrderManager::BuildFirstPylon,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(17.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(34.0f),										&BuildOrderManager::Scout,						BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(33.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(48.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(48.0f),										&BuildOrderManager::ImmediatelySaturateGasses,	BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(68.0f),										&BuildOrderManager::BuildBuildingMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE})),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(95.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(102.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(123.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STARGATE)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(124.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::HasBuildingStarted,	BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_STARGATE),				&BuildOrderManager::TrainAdept,					BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ADEPT)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(130.0f),										&BuildOrderManager::ResearchWarpgate,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(130.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(149.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(156.0f),										&BuildOrderManager::TrainAdept,					BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ADEPT)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(156.0f),										&BuildOrderManager::SetDoorGuard,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_STARGATE),				&BuildOrderManager::TrainOracle,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STARGATE)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(173.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(186.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(191.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STARGATE)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(202.0f),										&BuildOrderManager::TrainOracle,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STARGATE)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(203.0f),										&BuildOrderManager::DefendThirdBase,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(205.0f),										&BuildOrderManager::BuildBuildingMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_PYLON})),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(230.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(236.0f),										&BuildOrderManager::TrainOracle,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STARGATE)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(236.0f),										&BuildOrderManager::OracleHarass,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(240.0f),										&BuildOrderManager::BuildBuildingMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(240.0f),										&BuildOrderManager::BuildBuildingMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, UNIT_TYPEID::PROTOSS_FORGE})),
					//BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(250.0f),										&BuildOrderManager::BuildBuildingMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ResearchBlink,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ChronoTillFinished,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_FORGE),					&BuildOrderManager::ResearchAttackOne,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_FORGE),					&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_FORGE)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(270.0f),										&BuildOrderManager::ContinueBuildingPylons,		BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(270.0f),										&BuildOrderManager::ContinueMakingWorkers,		BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(270.0f),										&BuildOrderManager::ConntinueVolleyWarpingInStalkers,	BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(300.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(305.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_FORGE)),
					BuildOrderData(&BuildOrderManager::HasUnits,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_STALKER, 7),			&BuildOrderManager::StalkerOraclePressure,		BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(300.0f),										&BuildOrderManager::BuildBuildingMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_PYLON })),
					
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ResearchCharge,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ChronoTillFinished,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(370.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(370.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(370.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(370.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(390.0f),										&BuildOrderManager::StopVolleyWarpingInStalkers,		BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(390.0f),										&BuildOrderManager::ContinueVolleyWarpingInZealots,	BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(390.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(390.0f),										&BuildOrderManager::BuildBuildingMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_FORGE),					&BuildOrderManager::ResearchShieldsOne,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(400.0f),										&BuildOrderManager::ContinueUpgrades,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(400.0f),										&BuildOrderManager::ContinueChronos,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(400.0f),										&BuildOrderManager::ContinueExpanding,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(410.0f),										&BuildOrderManager::BuildBuildingMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_FLEETBEACON, UNIT_TYPEID::PROTOSS_STARGATE, UNIT_TYPEID::PROTOSS_STARGATE})),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_FLEETBEACON),			&BuildOrderManager::ContinueBuildingCarriers,	BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasUnits,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_ZEALOT, 12),			&BuildOrderManager::ZealotDoubleprong,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(390.0f),										&BuildOrderManager::StopVolleyWarpingInZealots,		BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(390.0f),										&BuildOrderManager::ConntinueVolleyWarpingInStalkers,	BuildOrderResultArgData()),
	};
}

void BuildOrderManager::Set4GateBlink()
{
	build_order = { BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(8.5f),										&BuildOrderManager::BuildFirstPylon,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(27.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(33.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(34.0f),										&BuildOrderManager::Scout,						BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(35.0f),										&BuildOrderManager::SafeRallyPoint,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(41.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(73.0f),										&BuildOrderManager::BuildBuildingMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_PYLON})),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(101.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::TrainAdept,					BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ADEPT)),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::ResearchWarpgate,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(135.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(135.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(148.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(155.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(175.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(183.0f),										&BuildOrderManager::BuildBuildingMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(203.0f),										&BuildOrderManager::ResearchBlink,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(203.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(205.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(210.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(217.0f),										&BuildOrderManager::ConntinueWarpingInStalkers,	BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,			BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY),		&BuildOrderManager::TrainObserver,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(223.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(225.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(227.0f),										&BuildOrderManager::ContinueMakingWorkers,		BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(243.0f),										&BuildOrderManager::TrainPrism,					BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(243.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(245.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(250.0f),										&BuildOrderManager::BuildBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(253.0f),										&BuildOrderManager::ChronoBuilding,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(260.0f),										&BuildOrderManager::ContinueBuildingPylons,		BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,			BuildOrderConditionArgData(270.0f),										&BuildOrderManager::StopWarpingInStalkers,		BuildOrderResultArgData()),
	};
}

void BuildOrderManager::SetChargelotAllin()
{
	build_order = { BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(6.5f),										&BuildOrderManager::BuildFirstPylon,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(16.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(30.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(34.0f),										&BuildOrderManager::Scout,					BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(39.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(63.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(69.0f),										&BuildOrderManager::BuildBuildingMulti,		BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, UNIT_TYPEID::PROTOSS_NEXUS})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(97.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(101.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(124.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(128.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(135.0f),										&BuildOrderManager::ResearchWarpgate,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(158.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(160.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(172.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::HasGas,			BuildOrderConditionArgData(100),										&BuildOrderManager::PullOutOfGas,				BuildOrderResultArgData(6)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ResearchCharge,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ChronoTillFinished,		BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::NumWorkers,		BuildOrderConditionArgData(30),											&BuildOrderManager::CutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(189.0f),										&BuildOrderManager::BuildBuildingMulti,		BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(215.0f),										&BuildOrderManager::TrainPrism,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_WARPPRISM)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(215.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
					//BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(220.0f),										&BuildOrderManager::IncreaseExtraPylons,		BuildOrderResultArgData(1)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(220.0f),										&BuildOrderManager::ContinueBuildingPylons,	BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(235.0f),										&BuildOrderManager::WarpInUnits,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ZEALOT, 2)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(255.0f),										&BuildOrderManager::WarpInUnits,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ZEALOT, 2)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(255.0f),										&BuildOrderManager::MicroChargelotAllin,		BuildOrderResultArgData()),
	};
}

void BuildOrderManager::SetChargelotAllinOld()
{
	build_order = { BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(6.5f),										&BuildOrderManager::BuildFirstPylon,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(16.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(34.0f),										&BuildOrderManager::Scout,					BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(39.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(39.0f),										&BuildOrderManager::ImmediatelySaturateGasses,BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(68.0f),										&BuildOrderManager::BuildBuildingMulti,		BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(97.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(102.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(110.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::ResearchWarpgate,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(130.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(133.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(149.0f),										&BuildOrderManager::PullOutOfGas,				BuildOrderResultArgData(2)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(155.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(165.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(172.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::HasGas,			BuildOrderConditionArgData(100),										&BuildOrderManager::PullOutOfGas,				BuildOrderResultArgData(4)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ResearchCharge,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ChronoTillFinished,		BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::NumWorkers,		BuildOrderConditionArgData(30),											&BuildOrderManager::CutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(189.0f),										&BuildOrderManager::BuildBuildingMulti,		BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(215.0f),										&BuildOrderManager::TrainPrism,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_WARPPRISM)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(215.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
					//BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(220.0f),										&BuildOrderManager::IncreaseExtraPylons,		BuildOrderResultArgData(1)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(220.0f),										&BuildOrderManager::ContinueBuildingPylons,   BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(235.0f),										&BuildOrderManager::WarpInUnits,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ZEALOT, 2)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(255.0f),										&BuildOrderManager::WarpInUnits,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ZEALOT, 2)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(255.0f),										&BuildOrderManager::MicroChargelotAllin,		BuildOrderResultArgData()),
	};
}

void BuildOrderManager::Set4GateAdept()
{
	build_order = { BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(6.5f),										&BuildOrderManager::BuildFirstPylon,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(16.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(34.0f),										&BuildOrderManager::Scout,					BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(39.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(68.0f),										&BuildOrderManager::BuildBuildingMulti,		BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(94.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(102.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(102.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::ResearchWarpgate,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(130.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(130.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(158.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(170.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::NumWorkers,		BuildOrderConditionArgData(30),											&BuildOrderManager::CutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ResearchGlaives,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL),		&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::TrainAdept,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ADEPT)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(191.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(198.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY),		&BuildOrderManager::TrainPrism,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_WARPPRISM)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY),		&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(211.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(205.0f),										&BuildOrderManager::UncutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(230.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
		//(&BuildOrderManager::TimePassed, 230, self.continue_warping_in_adepts, None),
		BuildOrderData(&BuildOrderManager::NumWorkers,		BuildOrderConditionArgData(30),											&BuildOrderManager::CutWorkers,				BuildOrderResultArgData()),
		//(self.has_unit, UnitTypeId.WARPPRISM, self.adept_pressure, None),
		BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(250.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
		BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(250.0f),										&BuildOrderManager::ContinueBuildingPylons,   BuildOrderResultArgData()),
		BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(250.0f),										&BuildOrderManager::ContinueMakingWorkers,	BuildOrderResultArgData()),
	};
}

void BuildOrderManager::SetFastestDTsPvT()
{
	build_order = { BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(10.0f),										&BuildOrderManager::BuildFirstPylon,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(27.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(34.0f),										&BuildOrderManager::Scout,					BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(34.0f),										&BuildOrderManager::SafeRallyPoint,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(39.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(39.0f),										&BuildOrderManager::ImmediatelySaturateGasses,BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(39.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(47.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(73.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(85.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(105.0f),										&BuildOrderManager::RemoveScoutToProxy,		BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY, 151)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(110.0f),										&BuildOrderManager::CutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(112.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(115.0f),										&BuildOrderManager::ResearchWarpgate,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(116.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(116.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(126.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(136.0f),										&BuildOrderManager::Contain,					BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(136.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(149.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_DARKSHRINE)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(166.0f),										&BuildOrderManager::TrainStalker,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(177.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(190.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_DARKSHRINE),			&BuildOrderManager::ResearchDTBlink,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_DARKSHRINE),			&BuildOrderManager::ChronoTillFinished,		BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_DARKSHRINE)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(225.0f),										&BuildOrderManager::WarpInUnits,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER, 3)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(227.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_PYLON)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(240.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(245.0f),										&BuildOrderManager::WarpInUnits,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER, 2)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(270.0f),										&BuildOrderManager::UncutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(298.0f),										&BuildOrderManager::WarpInUnits,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_DARKTEMPLAR, 3)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(298.0f),										&BuildOrderManager::DTHarass,					BuildOrderResultArgData()),
	};
}

void BuildOrderManager::SetProxyDoubleRobo()
{
	build_order = { BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(10.0f),										&BuildOrderManager::BuildFirstPylon,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(25.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_ASSIMILATOR})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(30.0f),										&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_NEXUS)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(35.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(45.0f),										&BuildOrderManager::SafeRallyPoint,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(68.0f),										&BuildOrderManager::BuildProxyMulti,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_NEXUS, UNIT_TYPEID::PROTOSS_PYLON, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(82.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(93.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ASSIMILATOR)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::TrainAdept,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_ADEPT)),
					BuildOrderData(&BuildOrderManager::HasBuilding,		BuildOrderConditionArgData(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE),		&BuildOrderManager::ChronoBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(146.0f),										&BuildOrderManager::TrainStalker,				BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(155.0f),										&BuildOrderManager::ResearchWarpgate,			BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(155.0f),										&BuildOrderManager::CutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(155.0f),										&BuildOrderManager::UseProxyDoubleRobo,		BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(185.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_PYLON})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(200.0f),										&BuildOrderManager::UncutWorkers,				BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(205.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_GATEWAY)),
					//BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(205.0f),										&BuildOrderManager::BuildBuildingMulti,		BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(228.0f),										&BuildOrderManager::ContinueBuildingPylons,	BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(228.0f),										&BuildOrderManager::MicroImmortalDrop,		BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(230.0f),										&BuildOrderManager::WarpInAtProxy,			BuildOrderResultArgData(UNIT_TYPEID::PROTOSS_STALKER)),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(240.0f),										&BuildOrderManager::ContinueMakingWorkers,	BuildOrderResultArgData()),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(250.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_ASSIMILATOR})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(280.0f),										&BuildOrderManager::BuildBuilding,			BuildOrderResultArgData({UNIT_TYPEID::PROTOSS_ASSIMILATOR})),
					BuildOrderData(&BuildOrderManager::TimePassed,		BuildOrderConditionArgData(300.0f),										&BuildOrderManager::ProxyDoubleRoboAllIn,		BuildOrderResultArgData()),
	};
}


}