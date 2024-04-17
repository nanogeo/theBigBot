#include "build_order_manager.h"
#include "theBigBot.h"
#include "utility.h"
#include "locations.h"
#include "finite_state_machine.h"
#include "action_manager.h"
#include "army_group.h"
#include "definitions.h"

#include <algorithm>
#include <vector>


#define Data BuildOrderData
#define Condition BuildOrderConditionArgData
#define Result BuildOrderResultArgData

namespace sc2
{

void BuildOrderManager::CheckBuildOrder()
{
	if (build_order_step < build_order.size())
	{
		Data current_step = build_order[build_order_step];
		bool(sc2::BuildOrderManager::*condition)(Condition) = current_step.condition;
		Condition condition_arg = current_step.condition_arg;
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

bool BuildOrderManager::TimePassed(Condition data)
{
	return agent->Observation()->GetGameLoop() / 22.4 >= data.time;
}

bool BuildOrderManager::NumWorkers(Condition data)
{
	return agent->Observation()->GetFoodWorkers() >= data.amount;
}

bool BuildOrderManager::HasBuilding(Condition data)
{
	for (const auto &building : agent->Observation()->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		if (building->build_progress == 1)
		{
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::HasBuildingStarted(Condition data)
{
	for (const auto &building : agent->Observation()->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		return true;
	}
	return false;
}

bool BuildOrderManager::IsResearching(Condition data)
{
	for (const auto &building : agent->Observation()->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		if (!building->orders.empty())
			return true;
	}
	return false;
}

bool BuildOrderManager::HasGas(Condition data)
{
	return agent->Observation()->GetVespene() >= data.amount;
}

bool BuildOrderManager::HasUnits(Condition data)
{
	if (agent->Observation()->GetUnits(IsFriendlyUnit(data.unitId)).size() >= data.amount)
	{
		return true;
	}
	return false;
}

bool BuildOrderManager::ReadyToScour(Condition data)
{
	if (Utility::DistanceToClosest(agent->Observation()->GetUnits(IsUnits({ NEXUS, COMMAND_CENTER, ORBITAL, UNIT_TYPEID::ZERG_HATCHERY, UNIT_TYPEID::ZERG_LAIR })), agent->Observation()->GetGameInfo().enemy_start_locations[0]) > 15
		|| agent->Observation()->GetGameLoop() / 22.4 >= data.time)
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
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
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
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
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
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
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
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
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
		//std::cout << "Error could not find builder in Scout" << std::endl;
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
		//std::cout << "Error could not find builder in Scout" << std::endl;
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

bool BuildOrderManager::CancelImmediatelySaturateGasses(BuildOrderResultArgData data)
{
	agent->immediatelySaturateGasses = false;
	return true;
}

bool BuildOrderManager::ImmediatelySemiSaturateGasses(BuildOrderResultArgData data)
{
	agent->immediatelySemiSaturateGasses = true;
	return true;
}

bool BuildOrderManager::CancelImmediatelySemiSaturateGasses(BuildOrderResultArgData data)
{
	agent->immediatelySemiSaturateGasses = false;
	return true;
}

bool BuildOrderManager::TrainStalker(BuildOrderResultArgData data)
{
	if (Utility::CanAfford(STALKER, 1, agent->Observation()))
	{
		for (const auto &gateway : agent->Observation()->GetUnits(IsFriendlyUnit(GATEWAY)))
		{
			if (gateway->build_progress == 1 && gateway->orders.size() == 0)
			{
				if (agent->Observation()->GetUnits(IsFinishedUnit(CYBERCORE)).size() > 0)
				{
					agent->Actions()->UnitCommand(gateway, ABILITY_ID::TRAIN_STALKER);
					return true;
				}
			}
		}
	}
	return false;
}

bool BuildOrderManager::TrainAdept(BuildOrderResultArgData data)
{
	if (Utility::CanAfford(ADEPT, data.amount, agent->Observation()) && agent->Observation()->GetUnits(IsFinishedUnit(CYBERCORE)).size() > 0)
	{
		Units gates_ready;
		for (const auto &gateway : agent->Observation()->GetUnits(IsFriendlyUnit(GATEWAY)))
		{
			if (gateway->build_progress == 1 && gateway->orders.size() == 0)
			{
				gates_ready.push_back(gateway);
			}
		}
		if (gates_ready.size() >= data.amount)
		{
			for (int i = 0; i < data.amount; i++)
			{
				agent->Actions()->UnitCommand(gates_ready[i], ABILITY_ID::TRAIN_ADEPT);
			}
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::TrainZealot(BuildOrderResultArgData data)
{
	if (Utility::CanAfford(ZEALOT, 1, agent->Observation()))
	{
		for (const auto &gateway : agent->Observation()->GetUnits(IsFriendlyUnit(GATEWAY)))
		{
			if (gateway->build_progress == 1 && gateway->orders.size() == 0)
			{
				agent->Actions()->UnitCommand(gateway, ABILITY_ID::TRAIN_ZEALOT);
				return true;
			}
		}
	}
	return false;
}

bool BuildOrderManager::TrainOracle(BuildOrderResultArgData data)
{
	if (Utility::CanAfford(UNIT_TYPEID::PROTOSS_ORACLE, 1, agent->Observation()))
	{
		for (const auto &stargate : agent->Observation()->GetUnits(IsFriendlyUnit(STARGATE)))
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
	if (Utility::CanAfford(PRISM, 1, agent->Observation()))
	{
		for (const auto &robo : agent->Observation()->GetUnits(IsFriendlyUnit(ROBO)))
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
		for (const auto &robo : agent->Observation()->GetUnits(IsFriendlyUnit(ROBO)))
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
	for (const auto &building : agent->Observation()->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
		{
			for (const auto &nexus : agent->Observation()->GetUnits(IsFriendlyUnit(NEXUS)))
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
	for (const auto &cyber : agent->Observation()->GetUnits(IsFinishedUnit(CYBERCORE)))
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
	for (const auto &twilight : agent->Observation()->GetUnits(IsFinishedUnit(TWILIGHT)))
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
	for (const auto &twilight : agent->Observation()->GetUnits(IsFinishedUnit(TWILIGHT)))
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
	for (const auto &twilight : agent->Observation()->GetUnits(IsFinishedUnit(TWILIGHT)))
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
	for (const auto &dark_shrine : agent->Observation()->GetUnits(IsFinishedUnit(DARK_SHRINE)))
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
	for (const auto &forge : agent->Observation()->GetUnits(IsFinishedUnit(FORGE)))
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
	for (const auto &forge : agent->Observation()->GetUnits(IsFinishedUnit(FORGE)))
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
	for (const auto &forge : agent->Observation()->GetUnits(IsFinishedUnit(FORGE)))
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
	for (const auto &forge : agent->Observation()->GetUnits(IsFinishedUnit(CYBERCORE)))
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
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
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
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueMakingWorkers, new ActionArgData(data.amount)));
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
	if (data.unitId == ROBO)
	{
		std::vector<const Unit*> proxy_robos;
		for (const auto &robo : agent->Observation()->GetUnits(IsFriendlyUnit(ROBO)))
		{
			if (Utility::DistanceToClosest(agent->GetProxyLocations(ROBO), robo->pos) < 2)
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
	if (data.unitId == ROBO)
	{
		std::vector<const Unit*> proxy_robos;
		for (const auto &robo : agent->Observation()->GetUnits(IsFriendlyUnit(ROBO)))
		{
			if (Utility::DistanceToClosest(agent->GetProxyLocations(ROBO), robo->pos) < 2)
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
	army->attack_path_line = agent->locations->attack_path_line;
	Units oracles = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ORACLE));
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
	for (const auto &unit : agent->Observation()->GetUnits(IsFriendlyUnit(STALKER)))
	{
		if (agent->UnitIsOccupied(unit))
			continue;
		army->AddNewUnit(unit);
	}
	army->AutoAddNewUnits({ STALKER, IMMORTAL, PRISM, COLOSSUS, CARRIER });
	agent->army_groups.push_back(army);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionStalkerOraclePressure, new ActionArgData(army)));
	return true;
}

bool BuildOrderManager::ZealotDoubleprong(BuildOrderResultArgData data)
{
	ArmyGroup* army = new ArmyGroup(agent, {}, agent->locations->attack_path_alt, agent->locations->high_ground_index_alt);
	for (const auto &unit : agent->Observation()->GetUnits(IsFriendlyUnit(ZEALOT)))
	{
		if (agent->UnitIsOccupied(unit))
			continue;
		army->AddNewUnit(unit);
	}
	army->AutoAddUnits({ ZEALOT });
	agent->army_groups.push_back(army);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionZealotDoubleprong, new ActionArgData(army)));
	return true;
}

bool BuildOrderManager::MicroOracles(BuildOrderResultArgData data)
{
	//StateMachine* oracle_fsm = new StateMachine(agent, new OracleDefend(agent, agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ORACLE)), agent->GetLocations(NEXUS)[2]), "Oracles");
	//agent->active_FSMs.push_back(oracle_fsm);
	return true;
}

bool BuildOrderManager::OracleHarass(BuildOrderResultArgData data)
{
	Units oracles = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ORACLE));
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
	//agent->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ZERGLING, agent->GetLocations(NEXUS)[2], 2, 15);
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
	if (agent->Observation()->GetUnits(IsFriendlyUnit(PRISM)).size() > 0)
	{
		const Unit* prism = agent->Observation()->GetUnits(IsFriendlyUnit(PRISM))[0];
		ChargelotAllInStateMachine* chargelotFSM = new ChargelotAllInStateMachine(agent, "Chargelot allin", agent->locations->warp_prism_locations, agent->Observation()->GetUnits(IsFriendlyUnit(ZEALOT)), prism, agent->Observation()->GetGameLoop() / 22.4);
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
	for (const auto &building : agent->Observation()->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		Point2D pos = Utility::PointBetween(building->pos, agent->Observation()->GetStartLocation(), 2);
		agent->Actions()->UnitCommand(building, ABILITY_ID::SMART, pos);
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
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionUseProxyDoubleRobo, new ActionArgData({ IMMORTAL, PRISM, IMMORTAL, UNIT_TYPEID::PROTOSS_OBSERVER })));
	return true;
}

bool BuildOrderManager::MicroImmortalDrop(BuildOrderResultArgData data)
{
	const Unit* immortal1 = NULL;
	const Unit* immortal2 = NULL;
	for (const auto &immortal : agent->Observation()->GetUnits(IsFriendlyUnit(IMMORTAL)))
	{
		if (immortal1 == NULL)
			immortal1 = immortal;
		else if (immortal2 == NULL)
			immortal2 = immortal;
	}
	ImmortalDropStateMachine* immortal_drop_fsm = new ImmortalDropStateMachine(agent, "Immortal Drop", immortal1, immortal2, agent->Observation()->GetUnits(IsFriendlyUnit(PRISM))[0], agent->Observation()->GetGameInfo().enemy_start_locations[0], agent->locations->immortal_drop_prism_locations);
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
	for (const auto &unit : agent->Observation()->GetUnits(IsUnits({ STALKER, UNIT_TYPEID::PROTOSS_OBSERVER, IMMORTAL })))
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
	for (const auto &unit : agent->Observation()->GetUnits(IsFriendlyUnit(ADEPT)))
	{
		if (agent->UnitIsOccupied(unit))
			continue;
		defenders->AddUnit(unit);
		break;
	}
	for (const auto &unit : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ORACLE)))
	{
		defenders->AddUnit(unit);
	}
	agent->army_groups.push_back(defenders);
	defenders->DefendExpansion(agent->GetLocations(NEXUS)[2], agent->locations->third_base_pylon_gap);
	return true;
}

bool BuildOrderManager::SetDoorGuard(BuildOrderResultArgData data)
{
	ArmyGroup* defenders = new ArmyGroup(agent);
	for (const auto &unit : agent->Observation()->GetUnits(IsFriendlyUnit(ADEPT)))
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

bool BuildOrderManager::AdeptDefendBaseTerran(BuildOrderResultArgData data)
{
	if (agent->Observation()->GetUnits(IsFriendlyUnit(ADEPT)).size() > 0)
	{
		const Unit* adept = agent->Observation()->GetUnits(IsFriendlyUnit(ADEPT))[0];
		AdeptBaseDefenseTerran* adept_defense_fsm = new AdeptBaseDefenseTerran(agent, "Adept base defense", adept, agent->locations->main_early_dead_space, agent->locations->natural_front);
		agent->active_FSMs.push_back(adept_defense_fsm);
		return true;
	}
	return false;
}

bool BuildOrderManager::StalkerDefendBaseTerran(BuildOrderResultArgData data)
{
	if (agent->Observation()->GetUnits(IsFriendlyUnit(STALKER)).size() > 0)
	{
		const Unit* stalker = agent->Observation()->GetUnits(IsFriendlyUnit(STALKER))[0];
		StalkerBaseDefenseTerran* stalker_defense_fsm = new StalkerBaseDefenseTerran(agent, "Stalker base defense", stalker, agent->locations->natural_front);
		agent->active_FSMs.push_back(stalker_defense_fsm);
		return true;
	}
	return false;
}

bool BuildOrderManager::StartFourGateBlinkPressure(BuildOrderResultArgData data)
{
	ArmyGroup* army = new ArmyGroup(agent);
	army->using_standby = true;
	army->standby_pos = agent->locations->blink_presure_consolidation;
	for (const auto& fsm : agent->active_FSMs)
	{
		if (fsm->name == "Stalker base defense")
		{
			agent->active_FSMs.erase(std::remove(agent->active_FSMs.begin(), agent->active_FSMs.end(), fsm), agent->active_FSMs.end());
			break;
		}
	}
	for (const auto &unit : agent->Observation()->GetUnits(IsUnits({ PRISM, STALKER })))
	{
		if (agent->UnitIsOccupied(unit))
			continue;
		army->AddUnit(unit);
	}
	BlinkStalkerAttackTerran* blink_fsm = new BlinkStalkerAttackTerran(agent, "4 gate blink pressure", army, agent->locations->blink_presure_consolidation, 
		agent->locations->blink_pressure_prism_consolidation, agent->locations->blink_pressure_blink_up, agent->locations->blink_pressure_blink_down);
	agent->active_FSMs.push_back(blink_fsm);
	return true;
}

bool BuildOrderManager::SendCannonRushTerranProbe1(BuildOrderResultArgData data)
{
	Point2D pos = agent->Observation()->GetGameInfo().enemy_start_locations[0];
	const Unit* cannoneer = agent->worker_manager.GetBuilder(pos);
	if (cannoneer == NULL)
	{
		//std::cout << "Error could not find builder in SendCannonRushTerranProbe1" << std::endl;
		return false;
	}
	agent->worker_manager.RemoveWorker(cannoneer);
	CannonRushTerran* cannon_fsm = new CannonRushTerran(agent, "cannon rush terran 1", cannoneer, 1);
	agent->active_FSMs.push_back(cannon_fsm);
	return true;
}

bool BuildOrderManager::SendCannonRushTerranProbe2(BuildOrderResultArgData data)
{
	Point2D pos = agent->Observation()->GetGameInfo().enemy_start_locations[0];
	const Unit* cannoneer = agent->worker_manager.GetBuilder(pos);
	if (cannoneer == NULL)
	{
		//std::cout << "Error could not find builder in SendCannonRushTerranProbe1" << std::endl;
		return false;
	}
	agent->worker_manager.RemoveWorker(cannoneer);
	CannonRushTerran* cannon_fsm = new CannonRushTerran(agent, "cannon rush terran 2", cannoneer, 2);
	agent->active_FSMs.push_back(cannon_fsm);
	return true;
}

bool BuildOrderManager::CannonRushAttack(BuildOrderResultArgData data)
{
	if (agent->Observation()->GetUnits(IsFriendlyUnit(ZEALOT)).size() == 0)
		return false;
	const Unit* zealot = agent->Observation()->GetUnits(IsFriendlyUnit(ZEALOT))[0];
	
	CannonRushTerran* cannon_fsm = new CannonRushTerran(agent, "cannon rush terran 3", zealot, 3);
	agent->active_FSMs.push_back(cannon_fsm);
	return true;
}

bool BuildOrderManager::SendAllInAttack(BuildOrderResultArgData data)
{
	ArmyGroup* army = new ArmyGroup(agent, {}, agent->locations->attack_path, agent->locations->high_ground_index);
	army->attack_path_line = agent->locations->attack_path_line;

	for (const auto& unit : agent->Observation()->GetUnits(IsFriendlyUnit(STALKER)))
	{
		if (agent->UnitIsOccupied(unit))
			continue;
		army->AddNewUnit(unit);
	}
	army->AutoAddNewUnits({ ZEALOT, ADEPT, SENTRY, STALKER, HIGH_TEMPLAR, ARCHON, IMMORTAL, PRISM, COLOSSUS, DISRUPTOR, VOID_RAY, TEMPEST, CARRIER });
	agent->army_groups.push_back(army);
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionAllInAttack, new ActionArgData(army)));
	return true;
}

bool BuildOrderManager::SendAdeptHarassProtoss(BuildOrderResultArgData data)
{
	AdeptHarassProtoss* adept_fsm = new AdeptHarassProtoss(agent, "adept harass protoss", agent->Observation()->GetUnits(IsFriendlyUnit(ADEPT)), agent->locations->adept_harrass_protoss_consolidation);
	agent->active_FSMs.push_back(adept_fsm);
	return true;
}

bool BuildOrderManager::ScourMap(BuildOrderResultArgData data)
{
	agent->active_FSMs = {};
	std::vector<ActionData*>* actions = &(agent->action_manager.active_actions);
	for (int i = 0; i < actions->size(); i++)
	{
		if ((*actions)[i]->toString() == "Stalker Oracle pressure" || (*actions)[i]->toString() == "Zealot double prong" || (*actions)[i]->toString() == "All in attack")
		{
			actions->erase(std::remove(actions->begin(), actions->end(), (*actions)[i]), actions->end());
			i--;
		}

	}
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionScourMap, new ActionArgData()));
	return true;
}


bool BuildOrderManager::SpawnArmy(BuildOrderResultArgData data)
{
	//agent->Debug()->DebugCreateUnit(STALKER, agent->locations->attack_path[0], 2, 10);
	////agent->Debug()->DebugCreateUnit(PRISM, agent->locations->attack_path[0], 2, 1);
	////agent->Debug()->DebugCreateUnit(MARINE, agent->locations->attack_path[2], 2, 18);
	////agent->Debug()->DebugCreateUnit(MARAUDER, agent->locations->attack_path[2], 2, 6);
	////agent->Debug()->DebugCreateUnit(SIEGE_TANK_SIEGED, agent->locations->attack_path[2], 2, 1);
	//agent->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_RAVAGER, agent->locations->attack_path[2], 1, 6);
	//agent->Debug()->DebugGiveAllUpgrades();
	return true;
}

bool BuildOrderManager::AttackLine(BuildOrderResultArgData data)
{
	ArmyGroup* army = new ArmyGroup(agent, agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Self)), agent->locations->attack_path_line);
	army->standby_pos = agent->locations->attack_path[0];
	army->using_standby = true;
	agent->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionAttackLine, new ActionArgData(army)));
	return true;
}



bool BuildOrderManager::RemoveProbe(BuildOrderResultArgData data)
{
	const Unit* builder = agent->worker_manager.GetBuilder(Point2D(0, 0));
	if (builder == NULL)
	{
		//std::cout << "Error could not find builder in RemoveProbe" << std::endl;
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
	case BuildOrder::cannon_rush_terran:
		SetCannonRushTerran();
		break;
	case BuildOrder::three_gate_robo:
		SetThreeGateRobo();
		break;
	default:
		//std::cout << "Error invalid build order in SetBuildOrder" << std::endl;
		break;
	}
}

void BuildOrderManager::SetBlank()
{
	build_order = {};
}


void BuildOrderManager::SetTesting()
{
	build_order = { Data(&BuildOrderManager::TimePassed,		Condition(1.0f),				&BuildOrderManager::CutWorkers,							Result()),
					Data(&BuildOrderManager::TimePassed,		Condition(2.0f),				&BuildOrderManager::SpawnArmy,							Result()),
					Data(&BuildOrderManager::TimePassed,		Condition(3.0f),				&BuildOrderManager::AttackLine,							Result()),

	};
}

// finished
void BuildOrderManager::SetOracleGatewaymanPvZ()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(6.5f),			&BuildOrderManager::BuildFirstPylon,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(34.0f),			&BuildOrderManager::Scout,								Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(33.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(48.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(48.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(68.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({NEXUS, CYBERCORE})),
					Data(&BuildOrderManager::TimePassed,			Condition(95.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(102.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(123.0f),			&BuildOrderManager::BuildBuilding,						Result(STARGATE)),
					Data(&BuildOrderManager::TimePassed,			Condition(124.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(STARGATE),		&BuildOrderManager::TrainAdept,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(149.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(156.0f),			&BuildOrderManager::TrainAdept,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(156.0f),			&BuildOrderManager::SetDoorGuard,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::TrainOracle,						Result(STARGATE)),
					Data(&BuildOrderManager::TimePassed,			Condition(173.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(186.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(191.0f),			&BuildOrderManager::ChronoBuilding,						Result(STARGATE)),
					Data(&BuildOrderManager::TimePassed,			Condition(202.0f),			&BuildOrderManager::TrainOracle,						Result(STARGATE)),
					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::DefendThirdBase,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({NEXUS, PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(230.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(236.0f),			&BuildOrderManager::TrainOracle,						Result(STARGATE)),
					Data(&BuildOrderManager::TimePassed,			Condition(236.0f),			&BuildOrderManager::OracleHarass,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({PYLON, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({TWILIGHT, FORGE})),

					//Data(&BuildOrderManager::TimePassed,			Condition(250.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),		&BuildOrderManager::ResearchBlink,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),		&BuildOrderManager::ChronoTillFinished,					Result(TWILIGHT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(FORGE),			&BuildOrderManager::ResearchAttackOne,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(FORGE),			&BuildOrderManager::ChronoBuilding,						Result(FORGE)),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::ConntinueVolleyWarpingInStalkers,	Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(305.0f),			&BuildOrderManager::ChronoBuilding,						Result(FORGE)),
					Data(&BuildOrderManager::HasUnits,				Condition(STALKER, 7),		&BuildOrderManager::StalkerOraclePressure,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({NEXUS, PYLON })),

					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),		&BuildOrderManager::ResearchCharge,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),		&BuildOrderManager::ChronoTillFinished,					Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(390.0f),			&BuildOrderManager::StopVolleyWarpingInStalkers,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(390.0f),			&BuildOrderManager::ContinueVolleyWarpingInZealots,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(390.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(390.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::HasBuilding,			Condition(FORGE),			&BuildOrderManager::ResearchShieldsOne,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(400.0f),			&BuildOrderManager::ContinueUpgrades,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(400.0f),			&BuildOrderManager::ContinueChronos,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(400.0f),			&BuildOrderManager::ContinueExpanding,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(410.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({FLEET_BEACON, STARGATE, STARGATE})),
					Data(&BuildOrderManager::HasBuilding,			Condition(FLEET_BEACON),	&BuildOrderManager::ContinueBuildingCarriers,			Result()),
					Data(&BuildOrderManager::HasUnits,				Condition(ZEALOT, 12),		&BuildOrderManager::ZealotDoubleprong,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(390.0f),			&BuildOrderManager::StopVolleyWarpingInZealots,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(390.0f),			&BuildOrderManager::ConntinueVolleyWarpingInStalkers,	Result()),
					Data(&BuildOrderManager::ReadyToScour,			Condition(600.0f),			&BuildOrderManager::ScourMap,							Result()),
	};
}

void BuildOrderManager::SetThreeGateRobo()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(8.5f),			&BuildOrderManager::BuildFirstPylon,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(27.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(33.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(34.0f),			&BuildOrderManager::Scout,								Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(41.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(41.0f),			&BuildOrderManager::ImmediatelySemiSaturateGasses,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(41.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(55.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(63.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(73.0f),			&BuildOrderManager::BuildBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(88.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainAdept,							Result(ADEPT, 2)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::SafeRallyPoint,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(126.0f),			&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(127.0f),			&BuildOrderManager::RemoveScoutToProxy,					Result(GATEWAY, 0)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::SendAdeptHarassProtoss,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::SendAllInAttack,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.1f),			&BuildOrderManager::TrainStalker,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::TrainPrism,							Result(PRISM)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(186.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::WarpInAtProxy,						Result()),
					Data(&BuildOrderManager::ReadyToScour,			Condition(480.0f),			&BuildOrderManager::ScourMap,							Result()),
	};
}

void BuildOrderManager::Set4GateBlink()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(8.5f),			&BuildOrderManager::BuildFirstPylon,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(27.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(33.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(34.0f),			&BuildOrderManager::Scout,								Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(35.0f),			&BuildOrderManager::SafeRallyPoint,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(41.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(73.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({CYBERCORE, NEXUS, PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(101.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainAdept,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::AdeptDefendBaseTerran,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(148.0f),			&BuildOrderManager::BuildBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::StalkerDefendBaseTerran,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(183.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::ResearchBlink,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::ChronoBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(217.0f),			&BuildOrderManager::ConntinueWarpingInStalkers,			Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::TrainObserver,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(223.0f),			&BuildOrderManager::ChronoBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(225.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(227.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(243.0f),			&BuildOrderManager::TrainPrism,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(243.0f),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(245.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(250.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(253.0f),			&BuildOrderManager::ChronoBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(260.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::HasUnits,				Condition(PRISM, 1),		&BuildOrderManager::StartFourGateBlinkPressure,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::BuildProxy,							Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::BuildBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::HasUnits,				Condition(STALKER, 8),		&BuildOrderManager::StopWarpingInStalkers,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::WarpInAtProxy,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(360.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({FORGE, FORGE})),
					Data(&BuildOrderManager::TimePassed,			Condition(400.0f),			&BuildOrderManager::ContinueExpanding,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(400.0f),			&BuildOrderManager::ContinueUpgrades,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(400.0f),			&BuildOrderManager::ContinueChronos,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(410.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::ReadyToScour,			Condition(900.0f),			&BuildOrderManager::ScourMap,							Result()),
	};
}

// unfinished

void BuildOrderManager::SetCannonRushTerran()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(8.5f),			&BuildOrderManager::BuildFirstPylon,					Result({PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(16.0f),			&BuildOrderManager::SendCannonRushTerranProbe1,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(27.0f),			&BuildOrderManager::BuildBuilding,						Result({FORGE})),
					Data(&BuildOrderManager::TimePassed,			Condition(35.0f),			&BuildOrderManager::SendCannonRushTerranProbe2,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(35.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result(2)),
					Data(&BuildOrderManager::TimePassed,			Condition(35.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::TrainZealot,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::BuildBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::HasUnits,				Condition(ZEALOT, 1),		&BuildOrderManager::CannonRushAttack,					Result()),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(STARGATE),		&BuildOrderManager::TrainStalker,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::BuildBuilding,						Result(FLEET_BEACON)),
					Data(&BuildOrderManager::HasBuilding,			Condition(FLEET_BEACON),	&BuildOrderManager::ContinueChronos,					Result(STARGATE)),

	};
}

void BuildOrderManager::SetBlinkProxyRoboPressureBuild()
{
	build_order = { Data(&BuildOrderManager::TimePassed,		Condition(6.5f),				&BuildOrderManager::BuildBuilding,			Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,		Condition(17.0f),				&BuildOrderManager::BuildBuilding,			Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,		Condition(40.5f),				&BuildOrderManager::BuildBuilding,			Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,		Condition(60.5f),				&BuildOrderManager::BuildBuilding,			Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,		Condition(70.0f),				&BuildOrderManager::BuildBuilding,			Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,		Condition(82.0f),				&BuildOrderManager::BuildBuilding,			Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,		Condition(85.0f),				&BuildOrderManager::BuildBuilding,			Result(PYLON)),
					Data(&BuildOrderManager::NumWorkers,		Condition(21),					&BuildOrderManager::CutWorkers,				Result()),
					Data(&BuildOrderManager::HasBuilding,		Condition(CYBERCORE),			&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,		Condition(CYBERCORE),			&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,		Condition(CYBERCORE),			&BuildOrderManager::ChronoBuilding,			Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,		Condition(CYBERCORE),			&BuildOrderManager::ChronoBuilding,			Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,		Condition(117.0f),				&BuildOrderManager::ResearchWarpgate,			Result()),
					Data(&BuildOrderManager::TimePassed,		Condition(117.0f),				&BuildOrderManager::Contain,					Result()),
					Data(&BuildOrderManager::TimePassed,		Condition(136.0f),				&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,		Condition(136.0f),				&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,		Condition(137.0f),				&BuildOrderManager::BuildBuilding,			Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,		Condition(152.0f),				&BuildOrderManager::BuildBuilding,			Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,		Condition(159.0f),				&BuildOrderManager::BuildProxy,				Result(PYLON)) ,
					Data(&BuildOrderManager::TimePassed,		Condition(166.0f),				&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,		Condition(171.0f),				&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,		Condition(176.5f),				&BuildOrderManager::BuildBuilding,			Result(NEXUS)),
					Data(&BuildOrderManager::HasBuilding,		Condition(TWILIGHT),			&BuildOrderManager::ResearchBlink,			Result(UPGRADE_ID::BLINKTECH)),
					Data(&BuildOrderManager::IsResearching,	Condition(TWILIGHT),			&BuildOrderManager::ChronoTillFinished,		Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,		Condition(185.0f),				&BuildOrderManager::UncutWorkers,				Result()),
					Data(&BuildOrderManager::TimePassed,		Condition(185.0f),				&BuildOrderManager::WarpInAtProxy,			Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,		Condition(205.0f),				&BuildOrderManager::BuildProxy,				Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,		Condition(205.0f),				&BuildOrderManager::ContinueBuildingPylons,   Result()),
					Data(&BuildOrderManager::TimePassed,		Condition(205.0f),				&BuildOrderManager::ContinueMakingWorkers,	Result(0)),
					Data(&BuildOrderManager::HasBuilding,		Condition(ROBO),				&BuildOrderManager::TrainFromProxy,			Result(ROBO)),
					Data(&BuildOrderManager::HasBuilding,		Condition(ROBO),				&BuildOrderManager::ContinueChronoProxyRobo,  Result(ROBO))
	};
}

void BuildOrderManager::SetChargelotAllin()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(6.5f),				&BuildOrderManager::BuildFirstPylon,				Result({PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(16.0f),				&BuildOrderManager::BuildBuilding,					Result({GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(30.0f),				&BuildOrderManager::ChronoBuilding,					Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(34.0f),				&BuildOrderManager::Scout,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(39.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(63.0f),				&BuildOrderManager::ChronoBuilding,					Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(69.0f),				&BuildOrderManager::BuildBuildingMulti,				Result({CYBERCORE, NEXUS})),
					Data(&BuildOrderManager::TimePassed,			Condition(97.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(101.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(124.0f),				&BuildOrderManager::BuildBuilding,					Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(128.0f),				&BuildOrderManager::TrainStalker,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),				&BuildOrderManager::ResearchWarpgate,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(158.0f),				&BuildOrderManager::BuildBuilding,					Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(160.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(172.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::HasGas,				Condition(100),					&BuildOrderManager::PullOutOfGas,					Result(6)),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ResearchCharge,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ChronoTillFinished,				Result(TWILIGHT)),
					Data(&BuildOrderManager::NumWorkers,			Condition(30),					&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(189.0f),				&BuildOrderManager::BuildBuildingMulti,				Result({GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(215.0f),				&BuildOrderManager::TrainPrism,						Result(PRISM)),
					Data(&BuildOrderManager::TimePassed,			Condition(215.0f),				&BuildOrderManager::ChronoBuilding,					Result(ROBO)),
					//Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::IncreaseExtraPylons,			Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::ContinueBuildingPylons,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(235.0f),				&BuildOrderManager::WarpInUnits,					Result(ZEALOT, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(255.0f),				&BuildOrderManager::WarpInUnits,					Result(ZEALOT, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(255.0f),				&BuildOrderManager::MicroChargelotAllin,			Result()),
	};
}

void BuildOrderManager::SetChargelotAllinOld()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(6.5f),				&BuildOrderManager::BuildFirstPylon,				Result({PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(16.0f),				&BuildOrderManager::BuildBuilding,					Result({GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(34.0f),				&BuildOrderManager::Scout,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(39.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(39.0f),				&BuildOrderManager::ImmediatelySaturateGasses,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(68.0f),				&BuildOrderManager::BuildBuildingMulti,				Result({NEXUS, CYBERCORE})),
					Data(&BuildOrderManager::TimePassed,			Condition(97.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(102.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(110.0f),				&BuildOrderManager::ChronoBuilding,					Result(NEXUS)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainStalker,					Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::ResearchWarpgate,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),				&BuildOrderManager::ChronoBuilding,					Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(133.0f),				&BuildOrderManager::BuildBuilding,					Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(149.0f),				&BuildOrderManager::PullOutOfGas,					Result(2)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),				&BuildOrderManager::BuildBuilding,					Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(165.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(172.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::HasGas,				Condition(100),					&BuildOrderManager::PullOutOfGas,					Result(4)),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ResearchCharge,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ChronoTillFinished,				Result(TWILIGHT)),
					Data(&BuildOrderManager::NumWorkers,			Condition(30),					&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(189.0f),				&BuildOrderManager::BuildBuildingMulti,				Result({GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(215.0f),				&BuildOrderManager::TrainPrism,						Result(PRISM)),
					Data(&BuildOrderManager::TimePassed,			Condition(215.0f),				&BuildOrderManager::ChronoBuilding,					Result(ROBO)),
					//Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::IncreaseExtraPylons,			Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::ContinueBuildingPylons,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(235.0f),				&BuildOrderManager::WarpInUnits,					Result(ZEALOT, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(255.0f),				&BuildOrderManager::WarpInUnits,					Result(ZEALOT, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(255.0f),				&BuildOrderManager::MicroChargelotAllin,			Result()),
	};
}

void BuildOrderManager::Set4GateAdept()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(6.5f),				&BuildOrderManager::BuildFirstPylon,				Result({PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(16.0f),				&BuildOrderManager::BuildBuilding,					Result({GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(34.0f),				&BuildOrderManager::Scout,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(39.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(68.0f),				&BuildOrderManager::BuildBuildingMulti,				Result({NEXUS, CYBERCORE})),
					Data(&BuildOrderManager::TimePassed,			Condition(94.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(102.0f),				&BuildOrderManager::ChronoBuilding,					Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(102.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainStalker,					Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::ResearchWarpgate,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),				&BuildOrderManager::ChronoBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),				&BuildOrderManager::BuildBuilding,					Result(TWILIGHT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainStalker,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(158.0f),				&BuildOrderManager::BuildBuilding,					Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(170.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::NumWorkers,			Condition(30),					&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ResearchGlaives,				Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ChronoBuilding,					Result(TWILIGHT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainAdept,						Result(ADEPT, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(191.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(198.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),				&BuildOrderManager::TrainPrism,						Result(PRISM)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),				&BuildOrderManager::ChronoBuilding,					Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(211.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),				&BuildOrderManager::UncutWorkers,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(230.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					//(&BuildOrderManager::TimePassed, 230, self.continue_warping_in_adepts, None),
					Data(&BuildOrderManager::NumWorkers,			Condition(30),					&BuildOrderManager::CutWorkers,						Result()),
					//(self.has_unit, UnitTypeId.WARPPRISM, self.adept_pressure, None),
					Data(&BuildOrderManager::TimePassed,			Condition(250.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(250.0f),				&BuildOrderManager::ContinueBuildingPylons,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(250.0f),				&BuildOrderManager::ContinueMakingWorkers,			Result(0)),
	};
}

void BuildOrderManager::SetFastestDTsPvT()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(10.0f),				&BuildOrderManager::BuildFirstPylon,				Result({PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(27.0f),				&BuildOrderManager::BuildBuilding,					Result({GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(34.0f),				&BuildOrderManager::Scout,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(34.0f),				&BuildOrderManager::SafeRallyPoint,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(39.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(39.0f),				&BuildOrderManager::ImmediatelySaturateGasses,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(39.0f),				&BuildOrderManager::ChronoBuilding,					Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(47.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(73.0f),				&BuildOrderManager::BuildBuilding,					Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(85.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(105.0f),				&BuildOrderManager::RemoveScoutToProxy,				Result(GATEWAY, 151)),
					Data(&BuildOrderManager::TimePassed,			Condition(110.0f),				&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(112.0f),				&BuildOrderManager::BuildBuilding,					Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(115.0f),				&BuildOrderManager::ResearchWarpgate,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(116.0f),				&BuildOrderManager::TrainStalker,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(116.0f),				&BuildOrderManager::ChronoBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(126.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(136.0f),				&BuildOrderManager::Contain,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(136.0f),				&BuildOrderManager::TrainStalker,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(149.0f),				&BuildOrderManager::BuildBuilding,					Result(DARK_SHRINE)),
					Data(&BuildOrderManager::TimePassed,			Condition(166.0f),				&BuildOrderManager::TrainStalker,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(177.0f),				&BuildOrderManager::TrainStalker,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(190.0f),				&BuildOrderManager::BuildBuilding,					Result(NEXUS)),
					Data(&BuildOrderManager::HasBuilding,			Condition(DARK_SHRINE),			&BuildOrderManager::ResearchDTBlink,				Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(DARK_SHRINE),			&BuildOrderManager::ChronoTillFinished,				Result(DARK_SHRINE)),
					Data(&BuildOrderManager::TimePassed,			Condition(225.0f),				&BuildOrderManager::WarpInUnits,					Result(STALKER, 3)),
					Data(&BuildOrderManager::TimePassed,			Condition(227.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(245.0f),				&BuildOrderManager::WarpInUnits,					Result(STALKER, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),				&BuildOrderManager::UncutWorkers,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(298.0f),				&BuildOrderManager::WarpInUnits,					Result(DARK_TEMPLAR, 3)),
					Data(&BuildOrderManager::TimePassed,			Condition(298.0f),				&BuildOrderManager::DTHarass,						Result()),
	};
}

void BuildOrderManager::SetProxyDoubleRobo()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(10.0f),				&BuildOrderManager::BuildFirstPylon,				Result({PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(25.0f),				&BuildOrderManager::BuildBuilding,					Result({ASSIMILATOR})),
					Data(&BuildOrderManager::TimePassed,			Condition(30.0f),				&BuildOrderManager::ChronoBuilding,					Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(35.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(45.0f),				&BuildOrderManager::SafeRallyPoint,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(68.0f),				&BuildOrderManager::BuildProxyMulti,				Result({NEXUS, PYLON, ROBO, ROBO})),
					Data(&BuildOrderManager::TimePassed,			Condition(82.0f),				&BuildOrderManager::BuildBuilding,					Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(93.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainAdept,						Result(ADEPT, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::ChronoBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(146.0f),				&BuildOrderManager::TrainStalker,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),				&BuildOrderManager::ResearchWarpgate,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),				&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),				&BuildOrderManager::UseProxyDoubleRobo,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(185.0f),				&BuildOrderManager::BuildBuilding,					Result({PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),				&BuildOrderManager::UncutWorkers,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					//Data(&BuildOrderManager::TimePassed,			Condition(205.0f),				&BuildOrderManager::BuildBuildingMulti,				Result({GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(228.0f),				&BuildOrderManager::ContinueBuildingPylons,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(228.0f),				&BuildOrderManager::MicroImmortalDrop,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(230.0f),				&BuildOrderManager::WarpInAtProxy,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),				&BuildOrderManager::ContinueMakingWorkers,			Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(250.0f),				&BuildOrderManager::BuildBuilding,					Result({ASSIMILATOR})),
					Data(&BuildOrderManager::TimePassed,			Condition(280.0f),				&BuildOrderManager::BuildBuilding,					Result({ASSIMILATOR})),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),				&BuildOrderManager::ProxyDoubleRoboAllIn,			Result()),
	};
}


}