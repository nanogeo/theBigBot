
#include <iostream>

#include "build_order_manager.h"
#include "utility.h"
#include "locations.h"
#include "finite_state_machine.h"
#include "action_manager.h"
#include "army_group.h"
#include "definitions.h"
#include "mediator.h"

#include <algorithm>
#include <vector>

// temp
#include "theBigBot.h"

#define Data BuildOrderData
#define Condition BuildOrderConditionArgData
#define Result BuildOrderResultArgData

namespace sc2
{

void BuildOrderManager::CheckBuildOrder()
{
	if (!run_build_order)
		return;

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
				std::cerr << "Complete build order step " << std::to_string(build_order_step) << std::endl;
				build_order_step++;
			}
		}
	}
}

void BuildOrderManager::PauseBuildOrder()
{
	run_build_order = false;

	if (!mediator->HasActionOfType(&ActionManager::ActionContinueBuildingPylons))
		mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueBuildingPylons, new ActionArgData()));

	if (!mediator->HasActionOfType(&ActionManager::ActionContinueChronos))
		mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueChronos, new ActionArgData()));
}

void BuildOrderManager::UnpauseBuildOrder()
{
	run_build_order = true;
}

bool BuildOrderManager::TimePassed(Condition data)
{
	return mediator->GetGameLoop() / FRAME_TIME >= data.time;
}

bool BuildOrderManager::NumWorkers(Condition data)
{
	return mediator->GetNumWorkers() >= data.amount;
}

bool BuildOrderManager::HasBuilding(Condition data)
{
	for (const auto &building : mediator->GetUnits(IsFriendlyUnit(data.unitId)))
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
	if (mediator->GetUnits(Unit::Alliance::Self, IsNonPlaceholderUnit(data.unitId)).size() > 0)
		return true;
	return false;
}

bool BuildOrderManager::HasNumBuildingStarted(Condition data)
{
	if (mediator->GetUnits(Unit::Alliance::Self, IsNonPlaceholderUnit(data.unitId)).size() >= data.amount)
		return true;
	return false;
}

bool BuildOrderManager::IsResearching(Condition data)
{
	for (const auto &building : mediator->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		if (!building->orders.empty())
			return true;
	}
	return false;
}

#pragma warning(push)
#pragma warning(disable : 4100)
bool BuildOrderManager::HasGas(Condition data)
{
	return mediator->HasResources(0, 100, 0);
}
#pragma warning(pop)

bool BuildOrderManager::HasUnits(Condition data)
{
	if (mediator->GetUnits(IsFriendlyUnit(data.unitId)).size() >= data.amount)
	{
		return true;
	}
	return false;
}

bool BuildOrderManager::NOP(BuildOrderResultArgData data)
{
	return true;
}

bool BuildOrderManager::BuildBuilding(BuildOrderResultArgData data)
{
	Point2D pos = mediator->GetLocation(data.unitId);
	const Unit* builder = mediator->GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->BuildBuilding(data.unitId, pos, builder);
	return true;
}

bool BuildOrderManager::BuildFirstPylon(BuildOrderResultArgData data)
{
	Point2D pos = mediator->GetFirstPylonLocation();

	const Unit* builder = mediator->GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->BuildBuilding(PYLON, pos, builder);
	return true;
}

bool BuildOrderManager::BuildBuildingMulti(BuildOrderResultArgData data)
{
	Point2D pos = mediator->GetLocation(data.unitIds[0]);
	const Unit* builder = mediator->GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->RemoveWorker(builder);
	mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuildingMulti, new ActionArgData(builder, data.unitIds, pos, 0)));
	return true;
}

bool BuildOrderManager::BuildProxyMulti(BuildOrderResultArgData data)
{
	Point2D pos = mediator->GetProxyLocation(data.unitIds[0]);
	const Unit* builder = mediator->GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->RemoveWorker(builder);
	mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionBuildProxyMulti, new ActionArgData(builder, data.unitIds, pos, 0)));
	return true;
}

bool BuildOrderManager::Scout(BuildOrderResultArgData data)
{
	return mediator->SendScout();
}

bool BuildOrderManager::CannonRushProbe1(BuildOrderResultArgData data)
{
	return mediator->SendCannonRushProbe1();
}

bool BuildOrderManager::CutWorkers(BuildOrderResultArgData data)
{
	mediator->SetBuildWorkers(false);
	return true;
}

bool BuildOrderManager::UncutWorkers(BuildOrderResultArgData data)
{
	mediator->SetBuildWorkers(true);
	return true;
}

bool BuildOrderManager::ImmediatelySaturateGasses(BuildOrderResultArgData data)
{
	mediator->SetImmediatlySaturateGasses(true);
	return true;
}

bool BuildOrderManager::CancelImmediatelySaturateGasses(BuildOrderResultArgData data)
{
	mediator->SetImmediatlySaturateGasses(false);
	return true;
}

bool BuildOrderManager::ImmediatelySemiSaturateGasses(BuildOrderResultArgData data)
{
	mediator->SetImmediatlySemiSaturateGasses(true);
	return true;
}

bool BuildOrderManager::CancelImmediatelySemiSaturateGasses(BuildOrderResultArgData data)
{
	mediator->SetImmediatlySemiSaturateGasses(false);
	return true;
}

bool BuildOrderManager::BalanceIncome(BuildOrderResultArgData data)
{
	mediator->SetBalanceIncome(true);
	return true;
}

bool BuildOrderManager::TrainUnit(BuildOrderResultArgData data)
{
	if (mediator->GetGameLoop() % 2 == 0)
		return false;

	if (mediator->CanAfford(data.unitId, data.amount) && mediator->CanTrainUnit(data.unitId))
	{
		UNIT_TYPEID build_structure = Utility::GetBuildStructure(data.unitId);
		Units structures_ready;
		for (const auto& structure : mediator->GetUnits(IsFriendlyUnit(build_structure)))
		{
			if (structure->build_progress == 1 && structure->orders.size() == 0)
			{
				structures_ready.push_back(structure);
			}
		}
		if (structures_ready.size() >= data.amount)
		{
			for (int i = 0; i < data.amount; i++)
			{
				mediator->SetUnitCommand(structures_ready[i], Utility::GetTrainAbility(data.unitId), 0);
			}
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::TrainStalker(BuildOrderResultArgData data)
{
	if (mediator->CanAfford(STALKER, data.amount) && mediator->GetUnits(IsFinishedUnit(CYBERCORE)).size() > 0)
	{
		Units gates_ready;
		for (const auto& gateway : mediator->GetUnits(IsFriendlyUnit(GATEWAY)))
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
				mediator->SetUnitCommand(gates_ready[i], ABILITY_ID::TRAIN_STALKER, 0);
			}
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::TrainAdept(BuildOrderResultArgData data)
{
	if (mediator->CanAfford(ADEPT, data.amount) && mediator->GetUnits(IsFinishedUnit(CYBERCORE)).size() > 0)
	{
		Units gates_ready;
		for (const auto &gateway : mediator->GetUnits(IsFriendlyUnit(GATEWAY)))
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
				mediator->SetUnitCommand(gates_ready[i], ABILITY_ID::TRAIN_ADEPT, 0);
			}
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::TrainZealot(BuildOrderResultArgData data)
{
	if (mediator->CanAfford(ZEALOT, 1))
	{
		for (const auto &gateway : mediator->GetUnits(IsFriendlyUnit(GATEWAY)))
		{
			if (gateway->build_progress == 1 && gateway->orders.size() == 0)
			{
				mediator->SetUnitCommand(gateway, ABILITY_ID::TRAIN_ZEALOT, 0);
				return true;
			}
		}
	}
	return false;
}

bool BuildOrderManager::TrainSentry(BuildOrderResultArgData data)
{
	if (mediator->CanAfford(SENTRY, data.amount) && mediator->GetUnits(IsFinishedUnit(CYBERCORE)).size() > 0)
	{
		Units gates_ready;
		for (const auto& gateway : mediator->GetUnits(IsFriendlyUnit(GATEWAY)))
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
				mediator->SetUnitCommand(gates_ready[i], ABILITY_ID::TRAIN_SENTRY, 0);
			}
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::TrainOracle(BuildOrderResultArgData data)
{
	if (mediator->CanAfford(UNIT_TYPEID::PROTOSS_ORACLE, 1))
	{
		for (const auto &stargate : mediator->GetUnits(IsFriendlyUnit(STARGATE)))
		{
			if (stargate->build_progress == 1 && stargate->orders.size() == 0)
			{
				mediator->SetUnitCommand(stargate, ABILITY_ID::TRAIN_ORACLE, 0);
				return true;
			}
		}
	}
	return false;
}

bool BuildOrderManager::TrainPrism(BuildOrderResultArgData data)
{
	if (mediator->CanAfford(PRISM, 1))
	{
		for (const auto &robo : mediator->GetUnits(IsFriendlyUnit(ROBO)))
		{
			if (robo->build_progress == 1 && robo->orders.size() == 0)
			{
				mediator->SetUnitCommand(robo, ABILITY_ID::TRAIN_WARPPRISM, 0);
				return true;
			}
		}
	}
	return false;
}

bool BuildOrderManager::TrainImmortal(BuildOrderResultArgData data)
{
	if (mediator->CanAfford(IMMORTAL, 1))
	{
		for (const auto& robo : mediator->GetUnits(IsFriendlyUnit(ROBO)))
		{
			if (robo->build_progress == 1 && robo->orders.size() == 0)
			{
				mediator->SetUnitCommand(robo, ABILITY_ID::TRAIN_IMMORTAL, 0);
				return true;
			}
		}
	}
	return false;
}

bool BuildOrderManager::TrainObserver(BuildOrderResultArgData data)
{
	if (mediator->CanAfford(UNIT_TYPEID::PROTOSS_OBSERVER, 1))
	{
		for (const auto &robo : mediator->GetUnits(IsFriendlyUnit(ROBO)))
		{
			if (robo->build_progress == 1 && robo->orders.size() == 0)
			{
				mediator->SetUnitCommand(robo, ABILITY_ID::TRAIN_OBSERVER, 0);
				mediator->CreateArmyGroup(ArmyRole::observer_scout, { OBSERVER }, 1, 2);
				return true;
			}
		}
	}
	return false;
}

bool BuildOrderManager::ChronoBuilding(BuildOrderResultArgData data)
{
	for (const auto &building : mediator->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
		{
			for (const auto &nexus : mediator->GetUnits(IsFriendlyUnit(NEXUS)))
			{
				if (nexus->energy >= 50 && nexus->build_progress == 1)
				{
					mediator->SetUnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building, 0);
					return true;
				}
			}
		}
	}
	return false;
}

bool BuildOrderManager::OptionalChronoBuilding(BuildOrderResultArgData data)
{
	for (const auto& building : mediator->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		if (building->build_progress == 1 && building->orders.size() > 0 && std::find(building->buffs.begin(), building->buffs.end(), BUFF_ID::CHRONOBOOSTENERGYCOST) == building->buffs.end())
		{
			for (const auto& nexus : mediator->GetUnits(IsFriendlyUnit(NEXUS)))
			{
				if (nexus->energy >= 50 && nexus->build_progress == 1)
				{
					mediator->SetUnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST, building, 0);
					return true;
				}
			}
		}
	}
	return true;
}

bool BuildOrderManager::ResearchWarpgate(BuildOrderResultArgData data)
{
	for (const auto &cyber : mediator->GetUnits(IsFinishedUnit(CYBERCORE)))
	{
		if (cyber->orders.size() > 0 && cyber->orders[0].ability_id == ABILITY_ID::RESEARCH_WARPGATE)
			return true;
		if (mediator->CanAffordUpgrade(UPGRADE_ID::WARPGATERESEARCH))
		{
			mediator->SetUnitCommand(cyber, ABILITY_ID::RESEARCH_WARPGATE, 0);
		}
	}
	return false;
}

bool BuildOrderManager::ResearchBlink(BuildOrderResultArgData data)
{
	for (const auto &twilight : mediator->GetUnits(IsFinishedUnit(TWILIGHT)))
	{
		if (twilight->orders.size() > 0 && twilight->orders[0].ability_id == ABILITY_ID::RESEARCH_BLINK)
			return true;
		if (mediator->CanAffordUpgrade(UPGRADE_ID::BLINKTECH) && twilight->orders.size() == 0)
		{
			mediator->SetUnitCommand(twilight, ABILITY_ID::RESEARCH_BLINK, 0);
		}
	}
	return false;
}

bool BuildOrderManager::ResearchCharge(BuildOrderResultArgData data)
{
	for (const auto &twilight : mediator->GetUnits(IsFinishedUnit(TWILIGHT)))
	{
		if (twilight->orders.size() > 0 && twilight->orders[0].ability_id == ABILITY_ID::RESEARCH_CHARGE)
			return true;
		if (mediator->CanAffordUpgrade(UPGRADE_ID::CHARGE) && twilight->orders.size() == 0)
		{
			mediator->SetUnitCommand(twilight, ABILITY_ID::RESEARCH_CHARGE, 0);
		}
	}
	return false;
}

bool BuildOrderManager::ResearchGlaives(BuildOrderResultArgData data)
{
	for (const auto &twilight : mediator->GetUnits(IsFinishedUnit(TWILIGHT)))
	{
		if (twilight->orders.size() > 0 && twilight->orders[0].ability_id == ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES)
			return true;
		if (mediator->CanAffordUpgrade(UPGRADE_ID::ADEPTPIERCINGATTACK) && twilight->orders.size() == 0)
		{
			mediator->SetUnitCommand(twilight, ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES, 0);
		}
	}
	return false;
}

bool BuildOrderManager::ResearchDTBlink(BuildOrderResultArgData data)
{
	for (const auto &dark_shrine : mediator->GetUnits(IsFinishedUnit(DARK_SHRINE)))
	{
		if (dark_shrine->orders.size() > 0 && dark_shrine->orders[0].ability_id == ABILITY_ID::RESEARCH_SHADOWSTRIKE)
			return true;
		if (mediator->CanAffordUpgrade(UPGRADE_ID::DARKTEMPLARBLINKUPGRADE) && dark_shrine->orders.size() == 0)
		{
			mediator->SetUnitCommand(dark_shrine, ABILITY_ID::RESEARCH_SHADOWSTRIKE, 0);
		}
	}
	return false;
}

bool BuildOrderManager::ResearchAttackOne(BuildOrderResultArgData data)
{
	for (const auto &forge : mediator->GetUnits(IsFinishedUnit(FORGE)))
	{
		if (forge->orders.size() > 0 && forge->orders[0].ability_id == ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS)
			return true;
		if (mediator->CanAffordUpgrade(UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1))
		{
			mediator->SetUnitCommand(forge, ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1, 0);
		}
	}
	return false;
}

bool BuildOrderManager::ResearchAttackTwo(BuildOrderResultArgData data)
{
	for (const auto &forge : mediator->GetUnits(IsFinishedUnit(FORGE)))
	{
		if (forge->orders.size() == 0 && mediator->CanAffordUpgrade(UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2))
		{
			mediator->SetUnitCommand(forge, ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL2, 0);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ResearchShieldsOne(BuildOrderResultArgData data)
{
	for (const auto &forge : mediator->GetUnits(IsFinishedUnit(FORGE)))
	{
		if (forge->orders.size() == 0 && mediator->CanAffordUpgrade(UPGRADE_ID::PROTOSSSHIELDSLEVEL1))
		{
			mediator->SetUnitCommand(forge, ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL1, 0);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ResearchAirAttackOne(BuildOrderResultArgData data)
{
	for (const auto &forge : mediator->GetUnits(IsFinishedUnit(CYBERCORE)))
	{
		if (forge->orders.size() == 0 && mediator->CanAffordUpgrade(UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1))
		{
			mediator->SetUnitCommand(forge, ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL1, 0);
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::ChronoTillFinished(BuildOrderResultArgData data)
{
	for (const auto &building : mediator->GetUnits(IsFinishedUnit(data.unitId)))
	{
		if (building->orders.size() > 0)
		{
			mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionChronoTillFinished, new ActionArgData(building, data.unitId)));
			return true;
		}
	}
	return false;
}

bool BuildOrderManager::BuildProxy(BuildOrderResultArgData data)
{
	Point2D pos = mediator->GetProxyLocation(data.unitId);

	const Unit* builder = mediator->GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->BuildBuilding(data.unitId, pos, builder);
	return true;
}

bool BuildOrderManager::ContinueBuildingPylons(BuildOrderResultArgData data)
{
	if (!mediator->HasActionOfType(&ActionManager::ActionContinueBuildingPylons))
		mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueBuildingPylons, new ActionArgData()));
	return true;
}

bool BuildOrderManager::ContinueMakingWorkers(BuildOrderResultArgData data)
{
	if (!mediator->HasActionOfType(&ActionManager::ActionContinueMakingWorkers))
		mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueMakingWorkers, new ActionArgData(data.amount)));
	return true;
}

bool BuildOrderManager::ContinueUpgrades(BuildOrderResultArgData data)
{
	if (!mediator->HasActionOfType(&ActionManager::ActionContinueUpgrades))
		mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueUpgrades, new ActionArgData()));
	return true;
}

bool BuildOrderManager::ContinueChronos(BuildOrderResultArgData data)
{
	if (!mediator->HasActionOfType(&ActionManager::ActionContinueChronos))
		mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueChronos, new ActionArgData()));
	return true;
}

bool BuildOrderManager::ContinueExpanding(BuildOrderResultArgData data)
{
	if (!mediator->HasActionOfType(&ActionManager::ActionContinueExpanding))
		mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContinueExpanding, new ActionArgData()));
	return true;
}

bool BuildOrderManager::TrainFromProxy(BuildOrderResultArgData data)
{
	return mediator->TrainFromProxyRobo();
}

bool BuildOrderManager::ContinueChronoProxyRobo(BuildOrderResultArgData data)
{
	/*if (data.unitId == ROBO)
	{
		std::vector<const Unit*> proxy_robos;
		for (const auto &robo : mediator->GetUnits(IsFriendlyUnit(ROBO)))
		{
			if (Utility::DistanceToClosest(mediator->GetProxyLocations(ROBO), robo->pos) < 2)
			{
				mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionConstantChrono, new ActionArgData(robo)));
				return true;
			}
		}
	}*/
	return false;
}

bool BuildOrderManager::Contain(BuildOrderResultArgData data)
{
	/*ArmyGroup* army = new ArmyGroup(mediator, {}, mediator->locations->attack_path, mediator->locations->high_ground_index);
	mediator->army_groups.push_back(army);
	mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionContain, new ActionArgData(army)));*/
	return true;
}

bool BuildOrderManager::StalkerOraclePressure(BuildOrderResultArgData data)
{
	mediator->CreateArmyGroup(ArmyRole::pressure, { STALKER, IMMORTAL, PRISM, COLOSSUS, ORACLE, CARRIER }, 15, 25);
	return true;
}

bool BuildOrderManager::ZealotSimpleAttack(BuildOrderResultArgData data)
{
	mediator->CreateArmyGroup(ArmyRole::simple_attack, { ZEALOT }, 2, 20);
	return true;
}

bool BuildOrderManager::ZealotDoubleprong(BuildOrderResultArgData data)
{
	mediator->CreateArmyGroup(ArmyRole::simple_attack, { ZEALOT }, 12, 20);
	return true;
}

bool BuildOrderManager::ZealotDoubleprongLarge(BuildOrderResultArgData data)
{
	mediator->CreateArmyGroup(ArmyRole::simple_attack, { ZEALOT }, 15, 30);
	return true;
}

bool BuildOrderManager::MicroOracles(BuildOrderResultArgData data)
{
	//StateMachine* oracle_fsm = new StateMachine(mediator, new OracleDefend(mediator, mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_ORACLE)), mediator->GetLocations(NEXUS)[2]), "Oracles");
	//mediator->active_FSMs.push_back(oracle_fsm);
	return true;
}

bool BuildOrderManager::SpawnUnits(BuildOrderResultArgData data)
{
	//mediator->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ZERGLING, mediator->GetLocations(NEXUS)[2], 2, 15);
	return true;
}

bool BuildOrderManager::WarpInUnits(BuildOrderResultArgData data)
{
	if (mediator->WarpInUnits(data.unitId, data.amount, mediator->GetNaturalLocation()))
		return true;
	return false;
}

bool BuildOrderManager::PullOutOfGas(BuildOrderResultArgData data)
{
	if (data.amount == 0)
		mediator->PullOutOfGas();
	else
		mediator->PullOutOfGas(data.amount);
	return true;
}

bool BuildOrderManager::IncreaseExtraPylons(BuildOrderResultArgData data)
{
	//mediator->extra_pylons += data.amount;
	return true;
}

bool BuildOrderManager::StartChargelotAllIn(BuildOrderResultArgData data)
{
	mediator->StartChargelotAllInStateMachine();
	return true;
}

bool BuildOrderManager::RemoveScoutToProxy(BuildOrderResultArgData data)
{
	if (mediator->RemoveScoutToProxy(data.unitId, data.amount))
		return true;
	return BuildProxyMulti(BuildOrderResultArgData({ PYLON, data.unitId }));
}

bool BuildOrderManager::SafeRallyPoint(BuildOrderResultArgData data)
{
	for (const auto &building : mediator->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		Point2D pos = Utility::PointBetween(building->pos, mediator->GetStartLocation(), 2);
		mediator->SetUnitCommand(building, ABILITY_ID::SMART, pos, 0);
	}
	return true;
}

bool BuildOrderManager::SafeRallyPointFromRamp(BuildOrderResultArgData data)
{
	for (const auto& building : mediator->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		float dist = Distance2D(mediator->GetMainRampForcefieldLocation(), building->pos);
		Point2D pos = Utility::PointBetween(mediator->GetMainRampForcefieldLocation(), building->pos, dist + 2);
		mediator->SetUnitCommand(building, ABILITY_ID::SMART, pos, 0);
	}
	return true;
}

bool BuildOrderManager::SetRallyPointToRamp(BuildOrderResultArgData data)
{
	for (const auto& building : mediator->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		Point2D pos = Utility::PointBetween(building->pos, mediator->GetMainRampForcefieldLocation(), 2);
		mediator->SetUnitCommand(building, ABILITY_ID::SMART, pos, 0);
	}
	return true;
}

bool BuildOrderManager::DTHarass(BuildOrderResultArgData data)
{
	/*if (mediator->enemy_race == Race::Terran)
		mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionDTHarassTerran, new ActionArgData()));*/
	return true;
}

bool BuildOrderManager::UseProxyDoubleRobo(BuildOrderResultArgData data)
{
	//mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionUseProxyDoubleRobo, new ActionArgData({ IMMORTAL, PRISM, IMMORTAL, UNIT_TYPEID::PROTOSS_OBSERVER })));
	return true;
}

bool BuildOrderManager::MicroImmortalDrop(BuildOrderResultArgData data)
{
	/*const Unit* immortal1 = nullptr;
	const Unit* immortal2 = nullptr;
	for (const auto &immortal : mediator->GetUnits(IsFriendlyUnit(IMMORTAL)))
	{
		if (immortal1 == nullptr)
			immortal1 = immortal;
		else if (immortal2 == nullptr)
			immortal2 = immortal;
	}
	ImmortalDropStateMachine* immortal_drop_fsm = new ImmortalDropStateMachine(mediator, "Immortal Drop", immortal1, immortal2, mediator->GetUnits(IsFriendlyUnit(PRISM))[0], mediator->GetEnemyStartLocation(), mediator->locations->immortal_drop_prism_locations);
	mediator->active_FSMs.push_back(immortal_drop_fsm);*/
	return true;
}

bool BuildOrderManager::ProxyDoubleRoboAllIn(BuildOrderResultArgData data)
{
	/*Units already_occupied;
	for (const auto &fsm : mediator->active_FSMs)
	{
		if (dynamic_cast<ImmortalDropStateMachine*>(fsm))
		{
			already_occupied.push_back(((ImmortalDropStateMachine*)fsm)->prism);
			already_occupied.push_back(((ImmortalDropStateMachine*)fsm)->immortal1);
			already_occupied.push_back(((ImmortalDropStateMachine*)fsm)->immortal2);
		}
	}
	Units available_units;
	for (const auto &unit : mediator->GetUnits(IsUnits({ STALKER, UNIT_TYPEID::PROTOSS_OBSERVER, IMMORTAL })))
	{
		if (std::find(already_occupied.begin(), already_occupied.end(), unit) == already_occupied.end())
			available_units.push_back(unit);
	}
	ArmyGroup* army = new ArmyGroup(mediator, available_units, mediator->locations->attack_path_alt, mediator->locations->high_ground_index_alt);
	mediator->army_groups.push_back(army);
	mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionAllIn, new ActionArgData(army)));*/
	return true;
}

bool BuildOrderManager::DefendThirdBase(BuildOrderResultArgData data)
{
	mediator->DefendThirdBaseZerg();
	mediator->CreateArmyGroup(ArmyRole::defend_outer, { ORACLE, VOID_RAY, CARRIER, TEMPEST }, 0, 10);
	return true;
}

bool BuildOrderManager::SetDoorGuard(BuildOrderResultArgData data)
{
	mediator->CreateArmyGroup(ArmyRole::defend_door, { ADEPT, STALKER, ZEALOT }, 1, 1);
	return true;
}

bool BuildOrderManager::AdeptDefendBaseTerran(BuildOrderResultArgData data)
{
	if (mediator->GetUnits(IsFriendlyUnit(ADEPT)).size() > 0)
	{
		mediator->CreateAdeptBaseDefenseTerranFSM();
		return true;
	}
	return false;
}

bool BuildOrderManager::StalkerDefendBaseTerran(BuildOrderResultArgData data)
{
	if (mediator->GetUnits(IsFriendlyUnit(STALKER)).size() > 0)
	{
		//mediator->CreateAdeptBaseDefenseTerranFSM();
		return true;
	}
	return false;
}

bool BuildOrderManager::StartFourGateBlinkPressure(BuildOrderResultArgData data)
{
	mediator->CreateFourGateBlinkFSM();
	return true;
}

bool BuildOrderManager::SendCannonRushTerranProbe1(BuildOrderResultArgData data)
{
	/*Point2D pos = mediator->GetEnemyStartLocation();
	const Unit* cannoneer = mediator->GetBuilder(pos);
	if (cannoneer == nullptr)
	{
		//std::cout << "Error could not find builder in SendCannonRushTerranProbe1" << std::endl;
		return false;
	}
	mediator->RemoveWorker(cannoneer);
	CannonRushTerran* cannon_fsm = new CannonRushTerran(mediator, "cannon rush terran 1", cannoneer, 1);
	mediator->finite_state_machine_manager.active_state_machines.push_back(cannon_fsm);*/
	return true;
}

bool BuildOrderManager::SendCannonRushTerranProbe2(BuildOrderResultArgData data)
{
	/*Point2D pos = mediator->GetEnemyStartLocation();
	const Unit* cannoneer = mediator->GetBuilder(pos);
	if (cannoneer == nullptr)
	{
		//std::cout << "Error could not find builder in SendCannonRushTerranProbe1" << std::endl;
		return false;
	}
	mediator->RemoveWorker(cannoneer);
	CannonRushTerran* cannon_fsm = new CannonRushTerran(mediator, "cannon rush terran 2", cannoneer, 2);
	mediator->finite_state_machine_manager.active_state_machines.push_back(cannon_fsm);*/
	return true;
}

bool BuildOrderManager::CannonRushAttack(BuildOrderResultArgData data)
{
	/*if (mediator->GetUnits(IsFriendlyUnit(ZEALOT)).size() == 0)
		return false;
	const Unit* zealot = mediator->GetUnits(IsFriendlyUnit(ZEALOT))[0];
	
	CannonRushTerran* cannon_fsm = new CannonRushTerran(mediator, "cannon rush terran 3", zealot, 3);
	mediator->finite_state_machine_manager.active_state_machines.push_back(cannon_fsm);*/
	return true;
}

bool BuildOrderManager::SendAllInAttack(BuildOrderResultArgData data)
{
	mediator->CreateArmyGroup(ArmyRole::attack, { ZEALOT, ADEPT, SENTRY, STALKER, HIGH_TEMPLAR, ARCHON, IMMORTAL, PRISM, COLOSSUS, DISRUPTOR, VOID_RAY, TEMPEST, CARRIER }, 15, 30);
	return true;
}

bool BuildOrderManager::SendAdeptHarassProtoss(BuildOrderResultArgData data)
{
	mediator->CreateAdeptHarassProtossFSM();
	return true;
}

bool BuildOrderManager::CheckForEarlyPool(BuildOrderResultArgData data)
{
	if (mediator->GetThirdTiming() != 0 && mediator->GetThirdTiming() < 180)
	{
		// 3 hatch before pool
		SetChargeAllInInterrupt();
		build_order_step = 0;
		std::cerr << "Chargelot all in interrupt. build order step now " << std::to_string(build_order_step) << std::endl;
		mediator->SendChat("Tag:scout_triple_hatch", ChatChannel::Team);
	}
	else if (mediator->GetCurrentTime() >= 90 && mediator->GetSpawningPoolTiming() == 0)
	{
		// late pool
		SetChargeAllInInterrupt();
		build_order_step = 0;
		std::cerr << "Chargelot all in interrupt. build order step now " << std::to_string(build_order_step) << std::endl;
		mediator->SendChat("Tag:scout_late_pool", ChatChannel::Team);
	}
	else if (mediator->GetSpawningPoolTiming() == 0)
	{
		return false;
	}
	else if (mediator->GetSpawningPoolTiming() < 25)
	{
		// 12 pool
		SetEarlyPoolInterrupt(); // TODO new interrupt for 12 pool
		build_order_step = 0;
		std::cerr << "Early pool interrupt. build order step now " << std::to_string(build_order_step) << std::endl;
		mediator->SendChat("Tag:scout_12_pool", ChatChannel::Team);
	}
	else if (mediator->GetSpawningPoolTiming() < 45)
	{
		// pool first
		SetEarlyPoolInterrupt();
		build_order_step = 0;
		std::cerr << "Early pool interrupt. build order step now " << std::to_string(build_order_step) << std::endl;
		mediator->SendChat("Tag:scout_pool_first", ChatChannel::Team);
	}
	else if (mediator->GetFirstGasTiming() == 0)
	{
		// gasless
		SetChargeAllInInterrupt();
		build_order_step = 0;
		std::cerr << "Chargelot all in interrupt. build order step now " << std::to_string(build_order_step) << std::endl;
		mediator->SendChat("Tag:scout_gasless", ChatChannel::Team);
	}
	else if (mediator->GetSpawningPoolTiming() >= 45)
	{
		// hatch first
		mediator->SendChat("Tag:scout_hatch_pool", ChatChannel::Team);
		return true;
	}
	return false;
}

bool BuildOrderManager::BuildNaturalDefensiveBuilding(BuildOrderResultArgData data)
{
	Point2D pos = mediator->GetNaturalDefensiveLocation(data.unitId);
	const Unit* builder = mediator->GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->RemoveWorker(builder);
	mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(builder, data.unitId, pos)));
	return true;
}

bool BuildOrderManager::BuildMainDefensiveBuilding(BuildOrderResultArgData data)
{
	Point2D pos = mediator->FindBuildLocationNearWithinNexusRange(data.unitId, mediator->GetFirstPylonLocation());
	const Unit* builder = mediator->GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->RemoveWorker(builder);
	mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionBuildBuilding, new ActionArgData(builder, data.unitId, pos)));
	return true;
}

bool BuildOrderManager::ReturnToOracleGatewaymanPvZ(BuildOrderResultArgData data)
{
	SetOracleGatewaymanPvZ();
	build_order_step = 21;
	std::cerr << "Return to oracle gatewayman. build order step now " << std::to_string(build_order_step) << std::endl;
	return true;
}

bool BuildOrderManager::ReturnTo4GateBlink(BuildOrderResultArgData data)
{
	Set4GateBlink();
	build_order_step = 23;
	std::cerr << "Return to 4 gate blink. build order step now " << std::to_string(build_order_step) << std::endl;
	return true;
}

bool BuildOrderManager::SetUnitProduction(BuildOrderResultArgData data)
{
	mediator->SetUnitProduction(data.unitId);
	return true;
}

bool BuildOrderManager::CancelWarpgateUnitProduction(BuildOrderResultArgData data)
{
	mediator->CancelWarpgateUnitProduction();
	return true;
}

bool BuildOrderManager::CancelStargateUnitProduction(BuildOrderResultArgData data)
{
	mediator->CancelStargateUnitProduction();
	return true;
}

bool BuildOrderManager::SetWarpInAtProxy(BuildOrderResultArgData data)
{
	mediator->SetWarpInAtProxy(data.amount);
	return true;
}

bool BuildOrderManager::AddToNaturalDefense(BuildOrderResultArgData data)
{
	mediator->AddToDefense(1, data.amount);
	return true;
}

bool BuildOrderManager::CheckTankCount(BuildOrderResultArgData data)
{
	if (mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({SIEGE_TANK, SIEGE_TANK_SIEGED})).size() > 1)
	{
		SetChargeTransition();
		build_order_step = 0;
		std::cerr << "Charge transition. build order step now " << std::to_string(build_order_step) << std::endl;
		return false;
	}
	if (mediator->GetGameLoop() / FRAME_TIME > 310)
	{
		return true;
	}
	return false;
}

bool BuildOrderManager::CheckForProxyRax(BuildOrderResultArgData data)
{
	switch (mediator->scouting_manager.CheckTerranScoutingInfoEarly())
	{
	case 0:
		mediator->SendChat("Tag:scout_no_proxy_rax", ChatChannel::Team);
		return true;
	case 1:
		// minor proxy
		SetMinorProxyRaxResponse();
		build_order_step = 0;
		std::cerr << "Minor proxy rax. build order step now " << std::to_string(build_order_step) << std::endl;
		mediator->SendChat("Tag:scout_minor/possible_proxy_rax", ChatChannel::Team);
		break;
	case 2:
		// major proxy
		SetMajorProxyRaxResponse();
		build_order_step = 0;
		std::cerr << "Major proxy rax. build orde step now " << std::to_string(build_order_step) << std::endl;
		mediator->SendChat("Tag:scout_major_proxy_rax", ChatChannel::Team);
		break;
	}
	return false;
}

bool BuildOrderManager::CheckProtossOpening(BuildOrderResultArgData data)
{
	build_order_step = 0;
	if (mediator->scouting_manager.enemy_unit_counts[FORGE] > 0)
	{
		// cannon rush
		mediator->SendChat("Tag:scout_cannon_rush", ChatChannel::Team);
		mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionCheckBaseForCannons, new ActionArgData(0)));
		Set2GateProxyRobo();
		return false;
	}
	switch (mediator->scouting_manager.enemy_unit_counts[GATEWAY])
	{
	case 0:
		// proxy
		mediator->SendChat("Tag:scout_proxy_gate", ChatChannel::Team);
		SetProxyGateResponse();
		break;
	case 1:
		// 1 gate expand
		mediator->SendChat("Tag:scout_1_gate_expand", ChatChannel::Team);
		Set2GateProxyRobo();
		break;
	case 2:
		// 2 gate
		mediator->SendChat("Tag:scout_2_gate", ChatChannel::Team);
		Set2GateProxyRobo();
		break;
	}
	return false;
}

bool BuildOrderManager::DoubleCheckProxyGate(BuildOrderResultArgData data)
{
	if (mediator->scouting_manager.enemy_unit_counts[FORGE] > 0)
	{
		// cannon rush
		mediator->SendChat("Tag:scout_cannon_rush", ChatChannel::Team);
		mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionCheckBaseForCannons, new ActionArgData(0)));
		build_order_step = 3;
		SetReturnTo2GateProxyRobo();
		RemoveScoutToProxy(BuildOrderResultArgData(ROBO, 0));
		SetRallyPointToRamp(BuildOrderResultArgData(GATEWAY));
		return true;
	}
	switch (mediator->scouting_manager.enemy_unit_counts[GATEWAY])
	{
	case 0:
		// proxy
		return true;
	case 1:
		// 1 gate expand
		mediator->SendChat("Tag:scout_1_gate_expand", ChatChannel::Team);
		build_order_step = 3;
		SetReturnTo2GateProxyRobo();
		RemoveScoutToProxy(BuildOrderResultArgData(ROBO, 0));
		SetRallyPointToRamp(BuildOrderResultArgData(GATEWAY));
		break;
	case 2:
		// 2 gate
		mediator->SendChat("Tag:scout_2_gate", ChatChannel::Team);
		build_order_step = 3;
		SetReturnTo2GateProxyRobo();
		RemoveScoutToProxy(BuildOrderResultArgData(ROBO, 0));
		SetRallyPointToRamp(BuildOrderResultArgData(GATEWAY));
		break;
	}
	return false;
}

bool BuildOrderManager::ScoutBases(BuildOrderResultArgData data)
{
	mediator->CreateArmyGroup(ArmyRole::scout_bases, { ADEPT, STALKER }, 1, 1);
	return true;
}

bool BuildOrderManager::WallOffRamp(BuildOrderResultArgData data)
{
	Point2D pos = mediator->GetWallOffLocation(data.unitId);
	const Unit* builder = mediator->GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->BuildBuilding(data.unitId, pos, builder);
	return true;
}

bool BuildOrderManager::DefendMainRamp(BuildOrderResultArgData data)
{
	mediator->CreateArmyGroup(ArmyRole::defend_main_ramp, {ADEPT, STALKER, SENTRY}, 10, 20);
	mediator->MarkArmyGroupForDeletion(mediator->GetArmyGroupDefendingBase(mediator->GetStartLocation()));
	return true;
}

bool BuildOrderManager::SackUnit(BuildOrderResultArgData data)
{
	Units units = mediator->GetUnits(Unit::Alliance::Self, IsUnit(data.unitId));
	const Unit* furthest_from_base = Utility::FurthestFrom(units, mediator->GetStartLocation());
	if (furthest_from_base != nullptr && Distance2D(furthest_from_base->pos, mediator->GetStartLocation()) > 30)
	{
		if (data.unitId == PROBE)
			mediator->RemoveWorker(furthest_from_base);
		mediator->SetUnitCommand(furthest_from_base, ABILITY_ID::MOVE_MOVE, mediator->GetEnemyStartLocation(), 10);
	}
	return true;
}

bool BuildOrderManager::SpawnArmy(BuildOrderResultArgData data)
{
	//mediator->agent->Debug()->DebugCreateUnit(ORACLE, mediator->agent->locations->attack_path[0], 2, 1);
	//mediator->agent->Debug()->DebugCreateUnit(ADEPT, mediator->agent->locations->attack_path[0], 2, 1);
	//mediator->agent->Debug()->DebugCreateUnit(MARINE, mediator->agent->locations->attack_path[2], 1, 12);
	//mediator->agent->Debug()->DebugCreateUnit(MARAUDER, mediator->agent->locations->attack_path[2], 1, 6);
	//mediator->agent->Debug()->DebugCreateUnit(ZERGLING, mediator->agent->locations->attack_path[2], 1, 3);
	//mediator->agent->Debug()->DebugCreateUnit(ROACH, mediator->agent->locations->attack_path[2], 1, 2);
	//mediator->agent->Debug()->DebugCreateUnit(SIEGE_TANK_SIEGED, mediator->agent->locations->attack_path[2], 1, 1);
	//mediator->agent->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_RAVAGER, mediator->agent->locations->attack_path[2], 1, 6);
	// 
	mediator->agent->Debug()->DebugCreateUnit(ORACLE, mediator->GetStartLocation(), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(ADEPT, mediator->GetStartLocation(), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(ZERGLING, Utility::PointBetween(mediator->GetStartLocation(), mediator->GetEnemyStartLocation(), 30), 2, 40);
	mediator->agent->Debug()->DebugCreateUnit(ROACH, Utility::PointBetween(mediator->GetStartLocation(), mediator->GetEnemyStartLocation(), 30), 2, 0);
	//mediator->agent->Debug()->DebugGiveAllUpgrades();
	mediator->agent->Debug()->DebugShowMap();
	mediator->agent->Debug()->DebugEnemyControl();
	return true;
}

bool BuildOrderManager::AttackLine(BuildOrderResultArgData data)
{
	Units eUnits = mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
	Units fUnits = mediator->GetUnits(IsFightingUnit(Unit::Alliance::Self));
	
	mediator->defense_manager.JudgeFight(eUnits, fUnits, 0.0f, 300.0f, true);
	//ArmyGroup* army = mediator->CreateArmyGroup(ArmyRole::simple_attack, { ADEPT, STALKER, PRISM }, 3, 20);
	//army->standby_pos = mediator->agent->locations->attack_path[0];
	//army->using_standby = false;
	return true;
}



bool BuildOrderManager::RemoveProbe(BuildOrderResultArgData data)
{
	const Unit* builder = mediator->GetBuilder(Point2D(0, 0));
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in RemoveProbe" << std::endl;
		return false;
	}
	mediator->RemoveWorker(builder);
	return true;
}


void BuildOrderManager::SetBuildOrder(BuildOrder build)
{
	current_build_order = build;
	switch (build)
	{
	case BuildOrder::blank:
		SetBlank();
		break;
	case BuildOrder::testing:
		SetTesting();
		break;
	case BuildOrder::oracle_gatewayman_pvz:
		SetOracleGatewaymanPvZ();
		break;
	case BuildOrder::four_gate_blink:
		Set4GateBlink();
		break;
	case BuildOrder::three_gate_robo:
		SetThreeGateRobo();
		break;
	case BuildOrder::blink_proxy_robo_pressure:
		SetBlinkProxyRoboPressureBuild();
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
	case BuildOrder::pvp_openner:
		SetPvPOpenner();
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


void BuildOrderManager::SetEarlyPoolInterrupt()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(100.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(110.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(112.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainAdept,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(138.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(139.0f),			&BuildOrderManager::BuildBuilding,						Result(STARGATE)),
					//Data(&BuildOrderManager::HasUnits,				Condition(ADEPT, 1),		&BuildOrderManager::SetDoorGuard,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(154.0f),			&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(156.0f),			&BuildOrderManager::TrainAdept,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::SetUnitProduction,					Result(ORACLE)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::ChronoBuilding,						Result(STARGATE)),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(180.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(180.0f),			&BuildOrderManager::ReturnToOracleGatewaymanPvZ,		Result()),
	};
}

void BuildOrderManager::SetChargeAllInInterrupt()
{
	build_order = { 
					Data(&BuildOrderManager::TimePassed,			Condition(97.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(101.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(127.0f),				&BuildOrderManager::BuildBuilding,					Result(TWILIGHT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::ResearchWarpgate,				Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::ChronoBuilding,					Result(CYBERCORE)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainUnit,						Result(ADEPT, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(152.0f),				&BuildOrderManager::PullOutOfGas,					Result(2)),
					Data(&BuildOrderManager::TimePassed,			Condition(156.0f),				&BuildOrderManager::BuildBuilding,					Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(160.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::HasGas,				Condition(100),					&BuildOrderManager::PullOutOfGas,					Result(4)),
					Data(&BuildOrderManager::TimePassed,			Condition(172.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ResearchCharge,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ChronoTillFinished,				Result(TWILIGHT)),
					Data(&BuildOrderManager::NumWorkers,			Condition(32),					&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(189.0f),				&BuildOrderManager::BuildBuildingMulti,				Result({GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::HasNumBuildingStarted,	Condition(GATEWAY, 8),			&BuildOrderManager::TrainUnit,						Result(PRISM, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::OptionalChronoBuilding,			Result(ROBO)),
					//Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::IncreaseExtraPylons,			Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::ContinueBuildingPylons,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(225.0f),				&BuildOrderManager::WarpInUnits,					Result(ZEALOT, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(245.0f),				&BuildOrderManager::WarpInUnits,					Result(ZEALOT, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(245.0f),				&BuildOrderManager::StartChargelotAllIn,			Result()),
	};
}

void BuildOrderManager::SetChargeTransition()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::CancelWarpgateUnitProduction,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::ResearchCharge,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::ChronoTillFinished,					Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::BuildBuilding,						Result(FORGE)),

					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::ContinueExpanding,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::ContinueUpgrades,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::ContinueChronos,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(320.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(360.0f),			&BuildOrderManager::SetUnitProduction,					Result(ZEALOT)),
					Data(&BuildOrderManager::TimePassed,			Condition(360.0f),			&BuildOrderManager::SetWarpInAtProxy,					Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(400.0f),			&BuildOrderManager::ZealotDoubleprong,					Result()),
					Data(&BuildOrderManager::HasUnits,				Condition(ZEALOT, 12),		&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::HasUnits,				Condition(ZEALOT, 12),		&BuildOrderManager::SetWarpInAtProxy,					Result(1)),
	};
}

void BuildOrderManager::SetMinorProxyRaxResponse()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(101.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(105.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainAdept,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(115.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::AdeptDefendBaseTerran,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(148.0f),			&BuildOrderManager::BuildBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::AddToNaturalDefense,				Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(183.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::ResearchBlink,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::ChronoBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(180.0f),			&BuildOrderManager::ReturnTo4GateBlink,					Result()),
	};
}

void BuildOrderManager::SetMajorProxyRaxResponse()
{
	build_order = { Data(&BuildOrderManager::NumWorkers,			Condition(19),				&BuildOrderManager::CutWorkers,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(101.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(120.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(122.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(122.0f),			&BuildOrderManager::AddToNaturalDefense,				Result(5)),
					Data(&BuildOrderManager::TimePassed,			Condition(133.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(134.0f),			&BuildOrderManager::UncutWorkers,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(152.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::NumWorkers,			Condition(22),				&BuildOrderManager::CutWorkers,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(160.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(165.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(170.0f),			&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(179.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(187.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(192.0f),			&BuildOrderManager::UncutWorkers,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(208.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::CancelImmediatelySaturateGasses,	Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::ImmediatelySemiSaturateGasses,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(217.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(235.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),			&BuildOrderManager::BuildBuilding,						Result(TWILIGHT)),
					//Data(&BuildOrderManager::TimePassed,			Condition(248.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(250.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(260.0f),			&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(260.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(260.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(260.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(276.0f),			&BuildOrderManager::ResearchBlink,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(276.0f),			&BuildOrderManager::ChronoTillFinished,					Result(TWILIGHT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::TrainObserver,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::HasUnits,				Condition(OBSERVER, 1),		&BuildOrderManager::TrainPrism,							Result()),
					Data(&BuildOrderManager::HasUnits,				Condition(OBSERVER, 1),		&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::HasUnits,				Condition(PRISM, 1),		&BuildOrderManager::AddToNaturalDefense,				Result(-4)),
					Data(&BuildOrderManager::HasUnits,				Condition(PRISM, 1),		&BuildOrderManager::StartFourGateBlinkPressure,			Result()),

					//Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(360.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({FORGE, FORGE})),
					Data(&BuildOrderManager::TimePassed,			Condition(360.0f),			&BuildOrderManager::ResearchCharge,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(390.0f),			&BuildOrderManager::ContinueExpanding,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(390.0f),			&BuildOrderManager::ContinueUpgrades,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(390.0f),			&BuildOrderManager::ContinueChronos,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(390.0f),			&BuildOrderManager::SetUnitProduction,					Result(ZEALOT)),
					Data(&BuildOrderManager::TimePassed,			Condition(410.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(460.0f),			&BuildOrderManager::ZealotDoubleprong,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(480.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
	};
}

void BuildOrderManager::SetWorkerRushDefense()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::CancelImmediatelySemiSaturateGasses,Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::CancelImmediatelySaturateGasses,	Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::PullOutOfGas,						Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::CutWorkers,							Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::SetUnitProduction,					Result(ZEALOT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::OptionalChronoBuilding,				Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(60.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({ GATEWAY, GATEWAY })),
					Data(&BuildOrderManager::TimePassed,			Condition(60.0f),			&BuildOrderManager::ContinueChronos,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(120.0f),			&BuildOrderManager::ZealotSimpleAttack,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(120.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
	};
}


void BuildOrderManager::SetProxyGateResponse()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::WallOffRamp,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(73.0f),			&BuildOrderManager::BuildBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(80.0f),			&BuildOrderManager::DoubleCheckProxyGate,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(94.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(98.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(SENTRY, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::SafeRallyPointFromRamp,				Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(120.0f),			&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(127.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(142.0f),			&BuildOrderManager::TrainUnit,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(142.0f),			&BuildOrderManager::DefendMainRamp,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::TrainUnit,							Result(SENTRY, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
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
					Data(&BuildOrderManager::TimePassed,			Condition(75.0f),			&BuildOrderManager::CheckForEarlyPool,					Result()),

					Data(&BuildOrderManager::TimePassed,			Condition(95.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(102.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(123.0f),			&BuildOrderManager::BuildBuilding,						Result(STARGATE)),
					Data(&BuildOrderManager::TimePassed,			Condition(124.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(STARGATE),		&BuildOrderManager::TrainAdept,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(149.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(156.0f),			&BuildOrderManager::TrainAdept,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::SetUnitProduction,					Result(ORACLE)),
					Data(&BuildOrderManager::TimePassed,			Condition(173.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(186.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(190.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(191.0f),			&BuildOrderManager::ChronoBuilding,						Result(STARGATE)),

					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::DefendThirdBase,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({NEXUS, PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(230.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({PYLON, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({TWILIGHT, FORGE})),

					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),		&BuildOrderManager::ResearchBlink,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),		&BuildOrderManager::ChronoTillFinished,					Result(TWILIGHT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(FORGE),			&BuildOrderManager::ResearchAttackOne,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(FORGE),			&BuildOrderManager::ChronoBuilding,						Result(FORGE)),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::CancelStargateUnitProduction,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(305.0f),			&BuildOrderManager::ChronoBuilding,						Result(FORGE)),

					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({NEXUS, PYLON })),

					Data(&BuildOrderManager::TimePassed,			Condition(330.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),		&BuildOrderManager::ResearchCharge,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),		&BuildOrderManager::ChronoTillFinished,					Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					//Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					//Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::ContinueUpgrades,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::ContinueChronos,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::ContinueExpanding,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(370.0f),			&BuildOrderManager::BalanceIncome,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(420.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(420.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(500.0f),			&BuildOrderManager::SetUnitProduction,					Result(ZEALOT)),
					Data(&BuildOrderManager::TimePassed,			Condition(500.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({FLEET_BEACON, STARGATE})),
					Data(&BuildOrderManager::HasBuilding,			Condition(FLEET_BEACON),	&BuildOrderManager::SetUnitProduction,					Result(CARRIER)),
					Data(&BuildOrderManager::TimePassed,			Condition(600.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
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
					Data(&BuildOrderManager::TimePassed,			Condition(122.0f),			&BuildOrderManager::RemoveScoutToProxy,					Result(GATEWAY, 0)),
					Data(&BuildOrderManager::TimePassed,			Condition(126.0f),			&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::SendAdeptHarassProtoss,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::SendAllInAttack,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(174.0f),			&BuildOrderManager::TrainPrism,							Result(PRISM)),
					Data(&BuildOrderManager::TimePassed,			Condition(174.0f),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(186.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::SetWarpInAtProxy,					Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
	};
}

void BuildOrderManager::SetPvPOpenner()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(8.5f),			&BuildOrderManager::BuildFirstPylon,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::Scout,								Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(27.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(36.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(41.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(41.0f),			&BuildOrderManager::ImmediatelySemiSaturateGasses,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(41.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(55.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(63.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::CheckProtossOpening,				Result(ASSIMILATOR)),
	};
}

void BuildOrderManager::Set2GateProxyRobo()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::BuildProxyMulti,					Result({PYLON, ROBO})),
					Data(&BuildOrderManager::TimePassed,			Condition(73.0f),			&BuildOrderManager::BuildBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(75.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					//Data(&BuildOrderManager::TimePassed,			Condition(80.0f),			&BuildOrderManager::RemoveScoutToProxy,					Result(ROBO, 0)), // TODO swap this back for better opponents
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(146.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					//Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::PrepRampWallOff,					Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(153.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(153.0f),			&BuildOrderManager::SendAllInAttack,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::TrainImmortal,						Result(IMMORTAL, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(173.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(177.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(185.0f),			&BuildOrderManager::TrainSentry,						Result(SENTRY, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(196.0f),			&BuildOrderManager::TrainPrism,							Result(PRISM, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(196.0f),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::ContinueChronos,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::SetUnitProduction,					Result(IMMORTAL)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::SetWarpInAtProxy,					Result(STALKER)),
	};
}

void BuildOrderManager::SetReturnTo2GateProxyRobo()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::BuildProxyMulti,					Result({PYLON, ROBO})),
					Data(&BuildOrderManager::TimePassed,			Condition(73.0f),			&BuildOrderManager::BuildBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(75.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),			&BuildOrderManager::NOP,								Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(125.0f),			&BuildOrderManager::SackUnit,							Result(PROBE)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(146.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(153.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(153.0f),			&BuildOrderManager::SendAllInAttack,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::TrainImmortal,						Result(IMMORTAL, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(173.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(177.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(185.0f),			&BuildOrderManager::TrainSentry,						Result(SENTRY, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(196.0f),			&BuildOrderManager::TrainPrism,							Result(PRISM, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(196.0f),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::ContinueChronos,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::SetUnitProduction,					Result(IMMORTAL)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::SetWarpInAtProxy,					Result(STALKER)),
	};
}

void BuildOrderManager::Set1GateExpand()
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
					Data(&BuildOrderManager::TimePassed,			Condition(122.0f),			&BuildOrderManager::RemoveScoutToProxy,					Result(GATEWAY, 0)),
					Data(&BuildOrderManager::TimePassed,			Condition(126.0f),			&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::SendAdeptHarassProtoss,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::SendAllInAttack,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(174.0f),			&BuildOrderManager::TrainPrism,							Result(PRISM)),
					Data(&BuildOrderManager::TimePassed,			Condition(174.0f),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(186.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::SetWarpInAtProxy,					Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
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
					Data(&BuildOrderManager::TimePassed,			Condition(100.0f),			&BuildOrderManager::CheckForProxyRax,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(101.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainAdept,							Result(ADEPT, 1)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::AdeptDefendBaseTerran,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(148.0f),			&BuildOrderManager::BuildBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::TrainStalker,						Result(STALKER, 1)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::AddToNaturalDefense,				Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(183.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::ResearchBlink,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::ChronoBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(217.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::TrainObserver,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(223.0f),			&BuildOrderManager::ChronoBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(225.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(227.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(235.0f),			&BuildOrderManager::BuildProxy,							Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(243.0f),			&BuildOrderManager::TrainPrism,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(243.0f),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(245.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(250.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(253.0f),			&BuildOrderManager::ChronoBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(260.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::HasUnits,				Condition(PRISM, 1),		&BuildOrderManager::StartFourGateBlinkPressure,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::BuildBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::SetWarpInAtProxy,					Result(1)),
					//Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::ScoutBases,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::CheckTankCount,						Result()),

					Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(360.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({FORGE, FORGE})),
					Data(&BuildOrderManager::TimePassed,			Condition(400.0f),			&BuildOrderManager::ContinueExpanding,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(400.0f),			&BuildOrderManager::ContinueUpgrades,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(400.0f),			&BuildOrderManager::ContinueChronos,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(410.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(420.0f),			&BuildOrderManager::ResearchCharge,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(500.0f),			&BuildOrderManager::SetUnitProduction,					Result(ZEALOT)),
					Data(&BuildOrderManager::TimePassed,			Condition(500.0f),			&BuildOrderManager::ZealotDoubleprong,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(720.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
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
	build_order = { Data(&BuildOrderManager::TimePassed,		Condition(6.5f),				&BuildOrderManager::BuildBuilding,				Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,		Condition(17.0f),				&BuildOrderManager::BuildBuilding,				Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,		Condition(40.5f),				&BuildOrderManager::BuildBuilding,				Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,		Condition(60.5f),				&BuildOrderManager::BuildBuilding,				Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,		Condition(70.0f),				&BuildOrderManager::BuildBuilding,				Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,		Condition(82.0f),				&BuildOrderManager::BuildBuilding,				Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,		Condition(85.0f),				&BuildOrderManager::BuildBuilding,				Result(PYLON)),
					Data(&BuildOrderManager::NumWorkers,		Condition(21),					&BuildOrderManager::CutWorkers,					Result()),
					Data(&BuildOrderManager::HasBuilding,		Condition(CYBERCORE),			&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,		Condition(CYBERCORE),			&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,		Condition(CYBERCORE),			&BuildOrderManager::ChronoBuilding,				Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,		Condition(CYBERCORE),			&BuildOrderManager::ChronoBuilding,				Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,		Condition(117.0f),				&BuildOrderManager::ResearchWarpgate,			Result()),
					Data(&BuildOrderManager::TimePassed,		Condition(117.0f),				&BuildOrderManager::Contain,					Result()),
					Data(&BuildOrderManager::TimePassed,		Condition(136.0f),				&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,		Condition(136.0f),				&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,		Condition(137.0f),				&BuildOrderManager::BuildBuilding,				Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,		Condition(152.0f),				&BuildOrderManager::BuildBuilding,				Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,		Condition(159.0f),				&BuildOrderManager::BuildProxy,					Result(PYLON)) ,
					Data(&BuildOrderManager::TimePassed,		Condition(166.0f),				&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,		Condition(171.0f),				&BuildOrderManager::TrainStalker,				Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,		Condition(176.5f),				&BuildOrderManager::BuildBuilding,				Result(NEXUS)),
					Data(&BuildOrderManager::HasBuilding,		Condition(TWILIGHT),			&BuildOrderManager::ResearchBlink,				Result(UPGRADE_ID::BLINKTECH)),
					Data(&BuildOrderManager::IsResearching,	Condition(TWILIGHT),			&BuildOrderManager::ChronoTillFinished,				Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,		Condition(185.0f),				&BuildOrderManager::UncutWorkers,				Result()),
					Data(&BuildOrderManager::TimePassed,		Condition(185.0f),				&BuildOrderManager::SetWarpInAtProxy,			Result(1)),
					Data(&BuildOrderManager::TimePassed,		Condition(185.0f),				&BuildOrderManager::SetUnitProduction,			Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,		Condition(205.0f),				&BuildOrderManager::BuildProxy,					Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,		Condition(205.0f),				&BuildOrderManager::ContinueBuildingPylons,		Result()),
					Data(&BuildOrderManager::TimePassed,		Condition(205.0f),				&BuildOrderManager::ContinueMakingWorkers,		Result(0)),
					Data(&BuildOrderManager::HasBuilding,		Condition(ROBO),				&BuildOrderManager::TrainFromProxy,				Result(ROBO)),
					Data(&BuildOrderManager::HasBuilding,		Condition(ROBO),				&BuildOrderManager::ContinueChronoProxyRobo,	Result(ROBO))
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
					Data(&BuildOrderManager::TimePassed,			Condition(255.0f),				&BuildOrderManager::StartChargelotAllIn,			Result()),
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
					Data(&BuildOrderManager::TimePassed,			Condition(255.0f),				&BuildOrderManager::StartChargelotAllIn,			Result()),
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
					//(self.has_unit, UNIT_TYPEID.WARPPRISM, self.adept_pressure, None),
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
					Data(&BuildOrderManager::TimePassed,			Condition(230.0f),				&BuildOrderManager::SetWarpInAtProxy,				Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(230.0f),				&BuildOrderManager::SetUnitProduction,				Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),				&BuildOrderManager::ContinueMakingWorkers,			Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(250.0f),				&BuildOrderManager::BuildBuilding,					Result({ASSIMILATOR})),
					Data(&BuildOrderManager::TimePassed,			Condition(280.0f),				&BuildOrderManager::BuildBuilding,					Result({ASSIMILATOR})),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),				&BuildOrderManager::ProxyDoubleRoboAllIn,			Result()),
	};
}


}