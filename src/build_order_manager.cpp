
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


void BuildOrderManager::DisplayBuildOrder() const
{
	std::string build_order_message = "";
	for (int i = build_order_step; i < build_order_step + 5; i++)
	{
		if (i >= build_order.size())
			break;
		build_order_message += build_order[i].toString() + "\n";
	}
	if (run_build_order)
		mediator->DebugText(build_order_message, Point2D(.7f, .15f), Color(0, 255, 0), 20);
	else
		mediator->DebugText(build_order_message, Point2D(.7f, .15f), Color(255, 0, 0), 20);
}

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
	mediator->AddUniqueAction(&ActionManager::ActionContinueBuildingPylons, new ActionArgData());
	mediator->AddUniqueAction(&ActionManager::ActionContinueSpendingNexusEnergy, new ActionArgData());
}

void BuildOrderManager::UnpauseBuildOrder()
{
	run_build_order = true;
}

bool BuildOrderManager::GetBuildOrderStatus() const
{
	return run_build_order;
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
	case BuildOrder::one_gate_expand_with_ramp:
		Set1GateExpandWithRamp();
		break;
	default:
		//std::cout << "Error invalid build order in SetBuildOrder" << std::endl;
		break;
	}
}

void BuildOrderManager::SetWorkerRushDefense()
{
	if (mediator->GetUnits(Unit::Alliance::Self, IsUnit(GATEWAY)).size() > 0)
		build_order_step = 1;
	else
		build_order_step = 0;

	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::CancelImmediatelySemiSaturateGasses,Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::CancelImmediatelySaturateGasses,	Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::PullOutOfGas,						Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),			&BuildOrderManager::CutWorkers,							Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::SetUnitProduction,					Result(ZEALOT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::OptionalChronoBuilding,				Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(60.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({ GATEWAY, GATEWAY })),
					Data(&BuildOrderManager::TimePassed,			Condition(60.0f),			&BuildOrderManager::ContinueSpendingNexusEnergy,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(120.0f),			&BuildOrderManager::ZealotSimpleAttack,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(120.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
	};
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
	if (data.amount == 0)
	{
		if (mediator->GetUnits(Unit::Alliance::Self, IsNonPlaceholderUnit(data.unitId)).size() > 0)
			return true;
	}
	else
	{
		if (mediator->GetUnits(Unit::Alliance::Self, IsNonPlaceholderUnit(data.unitId)).size() >= data.amount)
			return true;
	}
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

bool BuildOrderManager::HasMinerals(Condition data)
{
	return mediator->HasResources(data.amount, 0, 0);
}

bool BuildOrderManager::HasGas(Condition data)
{
	return mediator->HasResources(0, data.amount, 0);
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

	if (mediator->GetEnemyRace() == Race::Zerg)
	{
		mediator->ChangeAreaControl(mediator->GetNaturalLocation(), LONG_RANGE, mediator->ToPoint3D(mediator->GetNaturalLocation()).z, NodeControl::friendly_control);
	}

	const Unit* builder = mediator->GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->BuildBuilding(PYLON, pos, builder);
	return true;
}

bool BuildOrderManager::BuildLowGroundFirstPylon(BuildOrderResultArgData data)
{
	Point2D pos = mediator->GetLocations().first_pylon_location_zerg;

	mediator->ChangeAreaControl(mediator->GetNaturalLocation(), LONG_RANGE, mediator->ToPoint3D(mediator->GetNaturalLocation()).z, NodeControl::friendly_control);

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
	mediator->AddAction(&ActionManager::ActionBuildBuildingMulti, new ActionArgData(builder, data.unitIds, pos, 0));
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
	mediator->AddAction(&ActionManager::ActionBuildProxyMulti, new ActionArgData(builder, data.unitIds, pos, 0));
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
	mediator->CancelAllActionsOfType(&ActionManager::ActionContinueMakingWorkers);
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

	for (const auto& structure : mediator->GetUnits(IsFriendlyUnit(Utility::GetBuildStructure(data.unitId))))
	{
		if (structure->build_progress == 1 && structure->orders.size() == 0)
		{
			TryActionResult result = mediator->TryTrainUnit(structure, data.unitId);
			if (result == TryActionResult::success)
				return true;
			if (result == TryActionResult::low_tech || result == TryActionResult::cannot_afford)
				return false;
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
					mediator->SetUnitCommand(nexus, A_CHRONO, building, CommandPriorty::high);
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
					mediator->SetUnitCommand(nexus, A_CHRONO, building, CommandPriorty::low);
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
		if (mediator->TryResearchUpgrade(cyber, UPGRADE_ID::WARPGATERESEARCH) == TryActionResult::success)
			return true;
	}
	return false;
}

bool BuildOrderManager::ResearchBlink(BuildOrderResultArgData data)
{
	for (const auto &twilight : mediator->GetUnits(IsFinishedUnit(TWILIGHT)))
	{
		if (mediator->TryResearchUpgrade(twilight, UPGRADE_ID::BLINKTECH) == TryActionResult::success)
			return true;
	}
	return false;
}

bool BuildOrderManager::ResearchCharge(BuildOrderResultArgData data)
{
	for (const auto &twilight : mediator->GetUnits(IsFinishedUnit(TWILIGHT)))
	{
		if (mediator->TryResearchUpgrade(twilight, UPGRADE_ID::CHARGE) == TryActionResult::success)
			return true;
	}
	return false;
}

bool BuildOrderManager::ResearchGlaives(BuildOrderResultArgData data)
{
	for (const auto &twilight : mediator->GetUnits(IsFinishedUnit(TWILIGHT)))
	{
		if (mediator->TryResearchUpgrade(twilight, UPGRADE_ID::ADEPTPIERCINGATTACK) == TryActionResult::success)
			return true;
	}
	return false;
}

bool BuildOrderManager::ResearchDTBlink(BuildOrderResultArgData data)
{
	for (const auto &dark_shrine : mediator->GetUnits(IsFinishedUnit(DARK_SHRINE)))
	{
		if (mediator->TryResearchUpgrade(dark_shrine, UPGRADE_ID::DARKTEMPLARBLINKUPGRADE) == TryActionResult::success)
			return true;
	}
	return false;
}

bool BuildOrderManager::ResearchAttackOne(BuildOrderResultArgData data)
{
	for (const auto &forge : mediator->GetUnits(IsFinishedUnit(FORGE)))
	{
		if (mediator->TryResearchUpgrade(forge, UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1) == TryActionResult::success)
			return true;
	}
	return false;
}

bool BuildOrderManager::ChronoTillFinished(BuildOrderResultArgData data)
{
	for (const auto &building : mediator->GetUnits(IsFinishedUnit(data.unitId)))
	{
		if (building->orders.size() > 0)
		{
			mediator->AddAction(&ActionManager::ActionChronoTillFinished, new ActionArgData(building, data.unitId));
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
	mediator->AddUniqueAction(&ActionManager::ActionContinueBuildingPylons, new ActionArgData());
	return true;
}

bool BuildOrderManager::ContinueMakingWorkers(BuildOrderResultArgData data)
{
	mediator->AddUniqueAction(&ActionManager::ActionContinueMakingWorkers, new ActionArgData(data.amount));
	return true;
}

bool BuildOrderManager::ContinueSpendingNexusEnergy(BuildOrderResultArgData data)
{
	mediator->AddUniqueAction(&ActionManager::ActionContinueSpendingNexusEnergy, new ActionArgData());
	return true;
}

bool BuildOrderManager::ContinueExpanding(BuildOrderResultArgData data)
{
	mediator->AddUniqueAction(&ActionManager::ActionContinueExpanding, new ActionArgData());
	return true;
}

bool BuildOrderManager::ContinueMacro(BuildOrderResultArgData data)
{
	mediator->AddUniqueAction(&ActionManager::ActionContinueBuildingPylons, new ActionArgData());
	mediator->AddUniqueAction(&ActionManager::ActionContinueMakingWorkers, new ActionArgData());
	mediator->AddUniqueAction(&ActionManager::ActionContinueSpendingNexusEnergy, new ActionArgData());
	mediator->AddUniqueAction(&ActionManager::ActionContinueExpanding, new ActionArgData());
	mediator->SetBalanceIncome(true);
	mediator->StartUpgradeManager();
	mediator->AddRequiredUpgrade(U_WARPGATE);
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
	mediator->CreateAttack({ STALKER, IMMORTAL, PRISM, COLOSSUS, ORACLE, CARRIER }, 15, 25, 6, 5);
	return true;
}

bool BuildOrderManager::ZealotSimpleAttack(BuildOrderResultArgData data)
{
	mediator->CreateSimpleAttack({ ZEALOT }, 2, 20);
	return true;
}

bool BuildOrderManager::ZealotDoubleprong(BuildOrderResultArgData data)
{
	mediator->CreateSimpleAttack({ ZEALOT }, 10, 20);
	return true;
}

bool BuildOrderManager::ZealotDoubleprongLarge(BuildOrderResultArgData data)
{
	mediator->CreateSimpleAttack({ ZEALOT }, 15, 30);
	return true;
}

bool BuildOrderManager::MicroOracles(BuildOrderResultArgData data)
{
	//StateMachine* oracle_fsm = new StateMachine(mediator, new OracleDefend(mediator, mediator->GetUnits(IsFriendlyUnit(ORACLE)), mediator->GetLocations(NEXUS)[2]), "Oracles");
	//mediator->active_FSMs.push_back(oracle_fsm);
	return true;
}

bool BuildOrderManager::SpawnUnits(BuildOrderResultArgData data)
{
	//mediator->Debug()->DebugCreateUnit(ZERGLING, mediator->GetLocations(NEXUS)[2], 2, 15);
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
		mediator->SetUnitCommand(building, A_SMART, pos, CommandPriorty::low);
	}
	return true;
}

bool BuildOrderManager::SafeRallyPointFromRamp(BuildOrderResultArgData data)
{
	for (const auto& building : mediator->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		float dist = Distance2D(mediator->GetLocations().main_ramp_forcefield_top, building->pos);
		Point2D pos = Utility::PointBetween(mediator->GetLocations().main_ramp_forcefield_top, building->pos, dist + 2);
		mediator->SetUnitCommand(building, A_SMART, pos, CommandPriorty::low);
	}
	return true;
}

bool BuildOrderManager::SetRallyPointToRamp(BuildOrderResultArgData data)
{
	for (const auto& building : mediator->GetUnits(IsFriendlyUnit(data.unitId)))
	{
		Point2D pos = Utility::PointBetween(building->pos, mediator->GetLocations().main_ramp_forcefield_top, 2);
		mediator->SetUnitCommand(building, A_SMART, pos, CommandPriorty::low);
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
	//mediator->action_manager.active_actions.push_back(new ActionData(&ActionManager::ActionUseProxyDoubleRobo, new ActionArgData({ IMMORTAL, PRISM, IMMORTAL, OBSERVER })));
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
	for (const auto &unit : mediator->GetUnits(IsUnits({ STALKER, OBSERVER, IMMORTAL })))
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
	return true;
}

bool BuildOrderManager::SetDoorGuard(BuildOrderResultArgData data)
{
	mediator->SetDoorGuard();
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
	mediator->CreateAttack({ ZEALOT, ADEPT, SENTRY, STALKER, HIGH_TEMPLAR, ARCHON, IMMORTAL, PRISM, COLOSSUS, DISRUPTOR, VOID_RAY, TEMPEST, CARRIER }, 15, 30, 1, 1);
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
	else if (mediator->GetSpawningPoolTiming() < 45)
	{
		if (mediator->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS)).size() == 1)
		{
			mediator->CancelAllActionsOfType(&ActionManager::ActionBuildBuildingMulti);
		}
		if (mediator->GetSpawningPoolTiming() < 20  || mediator->GetEnemyUnitCount(ZERGLING) > 0)
		{
			// 12 pool
			Set12PoolInterrupt();
			build_order_step = 0;
			std::cerr << "12 pool interrupt. build order step now " << std::to_string(build_order_step) << std::endl;
			mediator->SendChat("Tag:scout_12_pool", ChatChannel::Team);
			Point2D pos = mediator->GetLocation(CYBERCORE);
			const Unit* builder = mediator->GetBuilder(pos);
			mediator->RemoveWorker(builder);
			mediator->AddAction(&ActionManager::ActionBuildBuildingMulti, new ActionArgData(builder, { CYBERCORE, GATEWAY }, pos, 0));
		}
		else
		{
			// pool first
			SetEarlyPoolInterrupt();
			build_order_step = 0;
			std::cerr << "Early pool interrupt. build order step now " << std::to_string(build_order_step) << std::endl;
			mediator->SendChat("Tag:scout_pool_first", ChatChannel::Team);
			Point2D pos = mediator->GetLocation(CYBERCORE);
			const Unit* builder = mediator->GetBuilder(pos);
			mediator->RemoveWorker(builder);
			mediator->AddAction(&ActionManager::ActionBuildBuildingMulti, new ActionArgData(builder, { CYBERCORE, NEXUS }, pos, 0));
		}
	}
	else if (mediator->GetRoachWarrenTiming() != 0)
	{
		// 2 base roach
		mediator->SendChat("Tag:scout_2_base_roach", ChatChannel::Team);
		return true;
	}
	else if (mediator->GetCurrentTime() >= 90 && mediator->GetFirstGasTiming() == 0)
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

bool BuildOrderManager::DoubleCheckEarlyPool(BuildOrderResultArgData data)
{
	if (mediator->GetRoachWarrenTiming() > 0)
	{
		if (mediator->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS)).size() == 1)
		{
			mediator->CancelAllActionsOfType(&ActionManager::ActionBuildBuildingMulti);
		}
		build_order_step = 0;
		SetRoachRushInterrupt();
		return false;
	}
	else if (mediator->GetCurrentTime() > 100)
	{
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
	mediator->AddAction(&ActionManager::ActionBuildBuilding, new ActionArgData(builder, data.unitId, pos));
	return true;
}

bool BuildOrderManager::BuildMainDefensiveBuilding(BuildOrderResultArgData data)
{
	Point2D pos = mediator->FindBuildLocationNearWithinNexusRange(data.unitId, mediator->GetNaturalLocation());
	const Unit* builder = mediator->GetBuilder(pos);
	if (builder == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->RemoveWorker(builder);
	mediator->AddAction(&ActionManager::ActionBuildBuilding, new ActionArgData(builder, data.unitId, pos));
	return true;
}

bool BuildOrderManager::ReturnToOracleGatewaymanPvZ(BuildOrderResultArgData data)
{
	SetOracleGatewaymanPvZ();
	build_order_step = 20;
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

bool BuildOrderManager::CheckForProxyRax(BuildOrderResultArgData data)
{
	int missing_scvs = 19;
	for (const auto& unit : mediator->GetEnemySavedPositions())
	{
		if (unit.first->unit_type != SCV)
			continue;

		if (Distance2D(unit.second.pos, mediator->GetEnemyStartLocation()) < 25 ||
			Distance2D(unit.second.pos, mediator->GetEnemyNaturalLocation()) < 25 ||
			Distance2D(unit.second.pos, mediator->GetStartLocation()) < 25)
			missing_scvs--;
	}

	if (mediator->GetFirstBarrackTiming() == 0 || Utility::DistanceToClosest(mediator->GetUnits(IsUnit(BARRACKS)), mediator->GetEnemyStartLocation()) > 25)
	{
		// proxy rax
		switch (mediator->GetEnemyUnitCount(REFINERY))
		{
		case 0:
			// proxy marine
			if (missing_scvs >= 2)
			{
				// major proxy rax marine
				build_order_step = 0;
				SetMajorProxyRaxResponse();
				mediator->SendChat("Tag:scout_major_proxy_rax_marine", ChatChannel::Team);
				return false;
			}
			else if (missing_scvs == 1)
			{
				// minor proxy rax marine
				build_order_step = 0;
				SetMinorProxyRaxResponse();
				mediator->SendChat("Tag:scout_minor_proxy_rax_marine", ChatChannel::Team);
				return false;
			}
			else
			{
				// cc first
				mediator->SendChat("Tag:scout_cc_first", ChatChannel::Team);
			}
			break;
		case 1:
		case 2:
			if (missing_scvs > 2)
			{
				// proxy reaper
				build_order_step = 0;
				SetMajorProxyRaxResponse();
				mediator->SendChat("Tag:scout_proxy_reaper", ChatChannel::Team);
				return false;
			}
			else if (missing_scvs == 2)
			{
				// proxy marauder
				build_order_step = 0;
				SetMajorProxyRaxResponse();
				mediator->SendChat("Tag:scout_proxy_marauder", ChatChannel::Team);
				return false;
			}
			else if (missing_scvs == 1)
			{
				// proxy rax + factory
				build_order_step = 0;
				SetChargeAllInInterruptTerran();
				mediator->SendChat("Tag:scout_minor_proxy_rax/factory", ChatChannel::Team);
				return false;
			}
			else
			{
				// late cc first
				mediator->SendChat("Tag:scout_late_cc_first?", ChatChannel::Team);
			}
			break;
		}
	}
	else if (mediator->GetFirstBarrackTiming() > 50)
	{
		switch (mediator->GetEnemyUnitCount(REFINERY))
		{
		case 0:
			// gasless
			if (missing_scvs >= 2)
			{
				// hidden major proxy rax
				build_order_step = 0;
				SetMinorProxyRaxResponse();
				mediator->SendChat("Tag:scout_hidden_major_proxy_rax_marine", ChatChannel::Team);
				return false;
			}
			else if (missing_scvs >= 1)
			{
				// hidden proxy rax
				build_order_step = 0;
				SetMinorProxyRaxResponse();
				mediator->SendChat("Tag:scout_hidden_minor_proxy_rax_marine", ChatChannel::Team);
				return false;
			}
			else
			{
				// gasless expand with scout
				mediator->SendChat("Tag:scout_gasless_1_rax_expand", ChatChannel::Team);
			}
			break;
		case 1:
			// 1 rax expand
			if (missing_scvs > 1)
			{
				// hidden proxy reaper expand
				build_order_step = 0;
				SetMinorProxyRaxResponse();
				mediator->SendChat("Tag:scout_hidden_proxy_reaper", ChatChannel::Team);
				return false;
			}
			else
			{
				// 1 rax expand with/out scout
				mediator->SendChat("Tag:scout_1_rax_expand", ChatChannel::Team);
			}
			break;
		case 2:
			// double gas
			mediator->SendChat("Tag:scout_double_gas", ChatChannel::Team);
			// TODO add stargate transition
			break;
		}
	}
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(BUNKER)), mediator->GetNaturalLocation()) > LONG_RANGE)
		mediator->CancelUnit(ZEALOT);
	return true;
}

bool BuildOrderManager::CheckProtossOpening(BuildOrderResultArgData data)
{
	build_order_step = 0;
	if (mediator->GetEnemyRace() != Race::Protoss)
	{
		Set2GateProxyRobo();
		return false;
	}
	if (mediator->GetEnemyUnitCount(FORGE) > 0)
	{
		// cannon rush
		mediator->SendChat("Tag:scout_cannon_rush", ChatChannel::Team);
		mediator->StartCannonRushDefense();
		build_order_step = 0;
		SetCannonRushResponse();
		return false;
	}
	if (mediator->GetFirstGateTiming() == 0 || Utility::DistanceToClosest(mediator->GetUnits(IsUnit(GATEWAY)), mediator->GetEnemyStartLocation()) > 25)
	{
		// proxy
		mediator->SendChat("Tag:scout_proxy_gate", ChatChannel::Team);
		SetProxyGateResponse();
		return false;
	}
	switch (mediator->GetEnemyUnitCount(GATEWAY))
	{
	case 1:
		// 1 gate expand
		mediator->SendChat("Tag:scout_1_gate_expand", ChatChannel::Team);
		Set2GateProxyRobo();
		break;
	case 2:
		// 2 gate
		mediator->SendChat("Tag:scout_2_gate", ChatChannel::Team);
		Set1GateExpand();
		break;
	default:
		// anything else
		mediator->SendChat("Tag:unknown_opener", ChatChannel::Team);
		Set2GateProxyRobo();
		break;
	}
	return false;
}

bool BuildOrderManager::CheckProtossOpening2(BuildOrderResultArgData data)
{
	if (mediator->GetEnemyRace() != Race::Protoss)
	{
		Set2GateProxyRobo();
		return false;
	}
	if (mediator->GetEnemyUnitCount(FORGE) > 0)
	{
		// cannon rush
		mediator->SendChat("Tag:scout_cannon_rush", ChatChannel::Team);
		mediator->StartCannonRushDefense();
		build_order_step = 0;
		SetCannonRushResponse();
		return false;
	}
	if (mediator->GetFirstGateTiming() == 0 || Utility::DistanceToClosest(mediator->GetUnits(IsUnit(GATEWAY)), mediator->GetEnemyStartLocation()) > 25)
	{
		// proxy
		mediator->SendChat("Tag:scout_proxy_gate", ChatChannel::Team);
		SetProxyGateResponse();
		return false;
	}
	switch (mediator->GetEnemyUnitCount(GATEWAY))
	{
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
	default:
		// anything else
		mediator->SendChat("Tag:unknown_opener", ChatChannel::Team);
		Set2GateProxyRobo();
		break;
	}
	return false;
}

bool BuildOrderManager::DoubleCheckProxyGate(BuildOrderResultArgData data)
{
	if (mediator->GetEnemyUnitCount(FORGE) > 0)
	{
		// cannon rush
		mediator->SendChat("Tag:scout_cannon_rush", ChatChannel::Team);
		mediator->AddUniqueAction(&ActionManager::ActionCheckBaseForCannons, new ActionArgData(0));
		build_order_step = 3;
		SetReturnTo2GateProxyRobo();
		RemoveScoutToProxy(BuildOrderResultArgData(ROBO, 0));
		SetRallyPointToRamp(BuildOrderResultArgData(GATEWAY));
		return true;
	}
	if (mediator->GetFirstGateTiming() != 0 && 
		Utility::DistanceToClosest(mediator->GetUnits(IsUnit(GATEWAY)), mediator->GetEnemyStartLocation()) > 25)
	{
		// proxy
		return true;
	}
	switch (mediator->GetEnemyUnitCount(GATEWAY))
	{
	case 0:
		if (mediator->GetNaturalTiming() != 0)
		{
			// nexus first
			mediator->SendChat("Tag:scout_nexus_first", ChatChannel::Team);
			build_order_step = 3;
			SetReturnTo2GateProxyRobo();
			RemoveScoutToProxy(BuildOrderResultArgData(ROBO, 0));
			SetRallyPointToRamp(BuildOrderResultArgData(GATEWAY));
		}
		else
		{
			// proxy
			return true;
		}
		break;
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
	mediator->ScoutBases();
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
	if(mediator->GetEnemyRace() == Race::Terran)
		mediator->DefendMainRamp(mediator->GetLocations().main_ramp_forcefield_mid);
	else
		mediator->DefendMainRamp(mediator->GetLocations().main_ramp_forcefield_top);

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
		mediator->SetUnitCommand(furthest_from_base, A_MOVE, mediator->GetEnemyStartLocation(), CommandPriorty::max);
	}
	return true;
}

bool BuildOrderManager::PrepRampWallOff(BuildOrderResultArgData data)
{
	mediator->CreatePvPRampWallOffFSM();
	return true;
}

bool BuildOrderManager::CancelBuilding(BuildOrderResultArgData data)
{
	for (const auto& building : mediator->GetUnits(Unit::Alliance::Self, IsUnit(data.unitId)))
	{
		if (building->build_progress < 1)
			mediator->SetUnitCommand(building, A_CANCEL_BUILDING, CommandPriorty::low);
	}
	return true;
}

bool BuildOrderManager::StopTempUnitProduction(BuildOrderResultArgData data)
{
	mediator->SetAllowProductionInterrupt(false);
	return true;
}

bool BuildOrderManager::CheckForBunkerRush(BuildOrderResultArgData data)
{
	const Unit* worker = mediator->GetBuilder(mediator->GetNaturalLocation());
	if (worker == nullptr)
	{
		//std::cout << "Error could not find builder in BuildBuilding" << std::endl;
		return false;
	}
	mediator->RemoveWorker(worker);
	mediator->AddUniqueAction(&ActionManager::ActionCheckForBunkerRush, new ActionArgData(worker));
	return true;
}

bool BuildOrderManager::EnergyRechargeUnit(BuildOrderResultArgData data)
{
	if (mediator->GetUnits(Unit::Alliance::Self, IsUnit(data.unitId)).size() == 0 || mediator->IsEnergyRechargeOffCooldown() == false)
		return false;

	for (const auto& unit : mediator->GetUnits(Unit::Alliance::Self, IsUnit(data.unitId)))
	{
		if (unit->energy < 100)
		{
			for (const auto& nexus : mediator->GetUnits(IsFriendlyUnit(NEXUS)))
			{
				if (nexus->energy >= 50 && nexus->build_progress == 1 && Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS)), unit->pos) < RANGE_ENERGY_RECHARGE)
				{
					for (const auto& ability : mediator->GetAbilitiesForUnit(nexus).abilities)
					{
						if (strcmp(AbilityTypeToName(ability.ability_id), "UNKNOWN") == 0)
						{
							mediator->SetUnitCommand(nexus, ability.ability_id, unit, CommandPriorty::low);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool BuildOrderManager::AddOracleGatewaymanPvZTransitions(BuildOrderResultArgData data)
{
	mediator->SetAutomaticUnitProduction(true);
	mediator->IncreaseUnitAmountInTargetComposition(ORACLE, 3);
	mediator->IncreaseUnitAmountInTargetComposition(STALKER, 30);
	mediator->AddZergTransitions();
	return true;
}

bool BuildOrderManager::AddRoboDefensePvZTransitions(BuildOrderResultArgData data)
{
	mediator->SetAutomaticUnitProduction(true);
	mediator->IncreaseUnitAmountInTargetComposition(PRISM, 1);
	mediator->IncreaseUnitAmountInTargetComposition(OBSERVER, 1);
	mediator->IncreaseUnitAmountInTargetComposition(IMMORTAL, 4);
	mediator->IncreaseUnitAmountInTargetComposition(STALKER, 30);
	mediator->AddZergTransitions();
	return true;
}

bool BuildOrderManager::Add4GateBlinkPvTTransitions(BuildOrderResultArgData data)
{
	mediator->SetAutomaticUnitProduction(true);
	mediator->IncreaseUnitAmountInTargetComposition(PRISM, 1);
	mediator->IncreaseUnitAmountInTargetComposition(STALKER, 30);
	mediator->AddTerranTransitions();
	return true;
}

bool BuildOrderManager::Add1GateExpandPvPTransitions(BuildOrderResultArgData data)
{
	mediator->SetAutomaticUnitProduction(true);
	mediator->IncreaseUnitAmountInTargetComposition(PRISM, 1);
	mediator->IncreaseUnitAmountInTargetComposition(IMMORTAL, 5);
	mediator->IncreaseUnitAmountInTargetComposition(STALKER, 20);
	mediator->IncreaseUnitAmountInTargetComposition(SENTRY, 3);
	mediator->AddProtossTransitions();
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
	//mediator->agent->Debug()->DebugCreateUnit(RAVAGER, mediator->agent->locations->attack_path[2], 1, 6);
	// 
	//mediator->agent->Debug()->DebugCreateUnit(ORACLE, mediator->GetStartLocation(), 1, 1);
	//mediator->agent->Debug()->DebugCreateUnit(ADEPT, mediator->GetStartLocation(), 1, 1);
	//mediator->agent->Debug()->DebugCreateUnit(ZERGLING, Utility::PointBetween(mediator->GetStartLocation(), mediator->GetEnemyStartLocation(), 30), 2, 40);
	//mediator->agent->Debug()->DebugCreateUnit(ROACH, Utility::PointBetween(mediator->GetStartLocation(), mediator->GetEnemyStartLocation(), 30), 2, 0);
	////mediator->agent->Debug()->DebugGiveAllUpgrades();
	//mediator->agent->Debug()->DebugShowMap();
	//mediator->agent->Debug()->DebugEnemyControl();
	/*mediator->agent->Debug()->DebugCreateUnit(PROBE, mediator->GetStartLocation(), 1, 40);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(25, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(27, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(29, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(31, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(33, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(35, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(37, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(39, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(45, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(41, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(31.5, 74.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(28.5, 74.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(43.5, 74.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(PYLON, Point2D(43, 77), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(37.5, 74.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(46.5, 74.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(22.5, 74.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(25.5, 74.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(24.5, 79.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(34.5, 74.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(40.5, 74.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(GATEWAY, Point2D(30.5, 79.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(CYBERCORE, Point2D(33.5, 79.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(ASSIMILATOR, Point2D(26.5, 58.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(ASSIMILATOR, Point2D(22.5, 62.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(NEXUS, Point2D(35.5, 34.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(ASSIMILATOR, Point2D(28.5, 37.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(ASSIMILATOR, Point2D(39.5, 27.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(NEXUS, Point2D(56.5, 65.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(ASSIMILATOR, Point2D(54.5, 72.5), 1, 1);
	mediator->agent->Debug()->DebugCreateUnit(ASSIMILATOR, Point2D(54.5, 58.5), 1, 1);*/
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



void BuildOrderManager::SetBlank()
{
	build_order = {};
}


void BuildOrderManager::SetTesting()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(1.0f),			&BuildOrderManager::CutWorkers,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(2.0f),			&BuildOrderManager::SpawnArmy,							Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(SENTRY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)), // 1 STALKER 1 sentry, 5 adept

	};
}

#pragma region responses

void BuildOrderManager::SetEarlyPoolInterrupt()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(90.0f),			&BuildOrderManager::DoubleCheckEarlyPool,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(100.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(110.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(112.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(CYBERCORE),		&BuildOrderManager::SafeRallyPoint,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(138.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(139.0f),			&BuildOrderManager::BuildBuilding,						Result(STARGATE)),
					//Data(&BuildOrderManager::HasUnits,				Condition(ADEPT),		&BuildOrderManager::SetDoorGuard,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(154.0f),			&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(156.0f),			&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::SetUnitProduction,					Result(ORACLE)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::ChronoBuilding,						Result(STARGATE)),
					Data(&BuildOrderManager::TimePassed,			Condition(180.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(180.0f),			&BuildOrderManager::ReturnToOracleGatewaymanPvZ,		Result()),
	};
}

void BuildOrderManager::SetRoachRushInterrupt()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(90.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(90.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(90.0f),			&BuildOrderManager::CancelBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(90.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(92.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(107.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(CYBERCORE),		&BuildOrderManager::SafeRallyPoint,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),			&BuildOrderManager::SetUnitProduction,					Result(IMMORTAL)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::ContinueMacro,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::AddRoboDefensePvZTransitions,		Result()),
	};
}

void BuildOrderManager::Set12PoolInterrupt()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(60.0f),			&BuildOrderManager::CancelBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(60.0f),			&BuildOrderManager::TrainUnit,							Result(ZEALOT)),
					Data(&BuildOrderManager::TimePassed,			Condition(60.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result()),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(CYBERCORE),		&BuildOrderManager::SafeRallyPoint,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(CYBERCORE),		&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ZEALOT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(125.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(138.0f),			&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::BuildBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(146.0f),			&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::BalanceIncome,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::ContinueSpendingNexusEnergy,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(160.0f),			&BuildOrderManager::BuildBuilding,						Result(STARGATE)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(STARGATE),		&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::SetUnitProduction,					Result(ORACLE)),
					Data(&BuildOrderManager::HasUnits,				Condition(ORACLE, 1),		&BuildOrderManager::ContinueMacro,						Result()),
					Data(&BuildOrderManager::HasUnits,				Condition(ORACLE, 1),		&BuildOrderManager::AddOracleGatewaymanPvZTransitions,	Result()),

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
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainUnit,						Result(ADEPT)),
					Data(&BuildOrderManager::TimePassed,			Condition(152.0f),				&BuildOrderManager::PullOutOfGas,					Result(2)),
					Data(&BuildOrderManager::TimePassed,			Condition(156.0f),				&BuildOrderManager::BuildBuilding,					Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(160.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::HasGas,				Condition(100),					&BuildOrderManager::PullOutOfGas,					Result(4)),
					Data(&BuildOrderManager::TimePassed,			Condition(172.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ResearchCharge,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ChronoTillFinished,				Result(TWILIGHT)),
					Data(&BuildOrderManager::NumWorkers,			Condition(32),					&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(189.0f),				&BuildOrderManager::BuildBuildingMulti,				Result({GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::HasNumBuildingStarted,	Condition(GATEWAY, 8),			&BuildOrderManager::TrainUnit,						Result(PRISM)),
					Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::OptionalChronoBuilding,			Result(ROBO)),
					//Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::IncreaseExtraPylons,			Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::ContinueBuildingPylons,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(225.0f),				&BuildOrderManager::WarpInUnits,					Result(ZEALOT, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(245.0f),				&BuildOrderManager::WarpInUnits,					Result(ZEALOT, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(245.0f),				&BuildOrderManager::StartChargelotAllIn,			Result()),
	};
}

void BuildOrderManager::SetChargeAllInInterruptTerran()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(100.0f),				&BuildOrderManager::ImmediatelySaturateGasses,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(100.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainUnit,						Result(ADEPT)),
					Data(&BuildOrderManager::TimePassed,			Condition(128.0f),				&BuildOrderManager::BuildBuilding,					Result(TWILIGHT)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(TWILIGHT),			&BuildOrderManager::ResearchWarpgate,				Result()),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(TWILIGHT),			&BuildOrderManager::ChronoBuilding,					Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(156.0f),				&BuildOrderManager::BuildBuilding,					Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(160.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),				&BuildOrderManager::NOP,							Result()),
					Data(&BuildOrderManager::HasGas,				Condition(100),					&BuildOrderManager::PullOutOfGas,					Result(6)),
					Data(&BuildOrderManager::TimePassed,			Condition(172.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ResearchCharge,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ChronoTillFinished,				Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(189.0f),				&BuildOrderManager::BuildBuildingMulti,				Result({GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::NumWorkers,			Condition(32),					&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::HasNumBuildingStarted,	Condition(GATEWAY, 8),			&BuildOrderManager::TrainUnit,						Result(PRISM)),
					Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::OptionalChronoBuilding,			Result(ROBO)),
					//Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::IncreaseExtraPylons,			Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(220.0f),				&BuildOrderManager::ContinueBuildingPylons,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(225.0f),				&BuildOrderManager::WarpInUnits,					Result(ZEALOT, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(245.0f),				&BuildOrderManager::WarpInUnits,					Result(ZEALOT, 2)),
					Data(&BuildOrderManager::TimePassed,			Condition(245.0f),				&BuildOrderManager::StartChargelotAllIn,			Result()),
	};
}

void BuildOrderManager::SetMinorProxyRaxResponse()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(101.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(105.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(115.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::AdeptDefendBaseTerran,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(148.0f),			&BuildOrderManager::BuildBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
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
					Data(&BuildOrderManager::TimePassed,			Condition(101.0f),			&BuildOrderManager::StopTempUnitProduction,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(101.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(101.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(101.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(125.0f),			&BuildOrderManager::DefendMainRamp,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(133.0f),			&BuildOrderManager::TrainUnit,							Result(SENTRY)),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(152.0f),			&BuildOrderManager::TrainUnit,							Result(SENTRY)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(156.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(157.0f),			&BuildOrderManager::CancelBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(160.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(165.0f),			&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
	};
}


void BuildOrderManager::SetProxyGateResponse()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::WallOffRamp,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::StopTempUnitProduction,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(73.0f),			&BuildOrderManager::BuildBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(80.0f),			&BuildOrderManager::DoubleCheckProxyGate,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(94.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(98.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(SENTRY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::SafeRallyPointFromRamp,				Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(120.0f),			&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(127.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(142.0f),			&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::TimePassed,			Condition(142.0f),			&BuildOrderManager::DefendMainRamp,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::TrainUnit,							Result(SENTRY)),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
	};
}

void BuildOrderManager::SetCannonRushResponse()
{
	build_order = { Data(&BuildOrderManager::HasMinerals,			Condition(120),				&BuildOrderManager::BuildBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(72.0f),			&BuildOrderManager::BuildProxyMulti,					Result({PYLON, ROBO})),
					Data(&BuildOrderManager::TimePassed,			Condition(75.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					//Data(&BuildOrderManager::HasBuildingStarted,	Condition(CYBERCORE),		&BuildOrderManager::RemoveScoutToProxy,					Result(ROBO, 0)),
					Data(&BuildOrderManager::HasMinerals,			Condition(110),				&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),			&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::HasUnits,				Condition(STALKER, 2),		&BuildOrderManager::SendAllInAttack,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::SetUnitProduction,					Result(IMMORTAL)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::OptionalChronoBuilding,				Result(ROBO)),
					Data(&BuildOrderManager::HasUnits,				Condition(IMMORTAL, 1),		&BuildOrderManager::SetUnitProduction,					Result(PRISM)),
					Data(&BuildOrderManager::HasUnits,				Condition(IMMORTAL, 1),		&BuildOrderManager::ContinueSpendingNexusEnergy,		Result()),
					Data(&BuildOrderManager::HasUnits,				Condition(PRISM, 1),		&BuildOrderManager::SetUnitProduction,					Result(IMMORTAL)),
					Data(&BuildOrderManager::HasUnits,				Condition(PRISM, 1),		&BuildOrderManager::SetWarpInAtProxy,					Result(STALKER)),
	};
}
#pragma endregion

// finished
void BuildOrderManager::SetOracleGatewaymanPvZ()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(6.5f),			&BuildOrderManager::BuildFirstPylon,					Result(PYLON)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(PYLON),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(GATEWAY),			&BuildOrderManager::Scout,								Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(33.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(48.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(48.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(68.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({NEXUS, CYBERCORE})),
					Data(&BuildOrderManager::TimePassed,			Condition(68.0f),			&BuildOrderManager::CheckForEarlyPool,					Result()),

					Data(&BuildOrderManager::TimePassed,			Condition(95.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(102.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(123.0f),			&BuildOrderManager::BuildBuilding,						Result(STARGATE)), // 10
					Data(&BuildOrderManager::TimePassed,			Condition(124.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(STARGATE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(149.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(156.0f),			&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::SetUnitProduction,					Result(ORACLE)),
					Data(&BuildOrderManager::TimePassed,			Condition(173.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(186.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(190.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)), // 20

					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::DefendThirdBase,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({NEXUS, PYLON})),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),			&BuildOrderManager::EnergyRechargeUnit,					Result(ORACLE)),
					Data(&BuildOrderManager::TimePassed,			Condition(230.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({PYLON, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({TWILIGHT, FORGE})),

					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),		&BuildOrderManager::ResearchBlink,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),		&BuildOrderManager::ChronoTillFinished,					Result(TWILIGHT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(FORGE),			&BuildOrderManager::ResearchAttackOne,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::ContinueSpendingNexusEnergy,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(270.0f),			&BuildOrderManager::CancelStargateUnitProduction,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::ContinueMacro,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::AddOracleGatewaymanPvZTransitions,	Result()),


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
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::SafeRallyPoint,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(122.0f),			&BuildOrderManager::RemoveScoutToProxy,					Result(GATEWAY, 0)),
					Data(&BuildOrderManager::TimePassed,			Condition(126.0f),			&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(145.0f),			&BuildOrderManager::SendAdeptHarassProtoss,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::SendAllInAttack,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(174.0f),			&BuildOrderManager::TrainUnit,							Result(PRISM)),
					Data(&BuildOrderManager::TimePassed,			Condition(174.0f),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(186.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::SetWarpInAtProxy,					Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
	};
}

void BuildOrderManager::Set1GateExpandWithRamp()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(5.5f),			&BuildOrderManager::BuildLowGroundFirstPylon,			Result(PYLON)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(PYLON),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(GATEWAY),			&BuildOrderManager::Scout,								Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(36.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::TimePassed,			Condition(41.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					//Data(&BuildOrderManager::TimePassed,			Condition(63.0f),			&BuildOrderManager::ChronoBuilding,						Result(NEXUS)),
					//Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::CheckProtossOpening2,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(68.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({NEXUS, CYBERCORE})),
					Data(&BuildOrderManager::HasUnits,				Condition(PROBE, 20),		&BuildOrderManager::CutWorkers,							Result()),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(CYBERCORE),		&BuildOrderManager::UncutWorkers,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(93.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(100.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),			&BuildOrderManager::BuildBuilding,						Result(STARGATE)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(STARGATE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					//Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::CheckForExpansion,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ContinueMakingWorkers,				Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ContinueSpendingNexusEnergy,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::SetUnitProduction,					Result(VOID_RAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::ChronoBuilding,						Result(STARGATE)),
					Data(&BuildOrderManager::TimePassed,			Condition(195.0f),			&BuildOrderManager::BuildMainDefensiveBuilding,			Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(230.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(240.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(250.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
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
					Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::CheckProtossOpening,				Result()),
	};
}

void BuildOrderManager::Set2GateProxyRobo()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::BuildProxyMulti,					Result({PYLON, ROBO})),
					Data(&BuildOrderManager::TimePassed,			Condition(73.0f),			&BuildOrderManager::BuildBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(75.0f),			&BuildOrderManager::ImmediatelySaturateGasses,			Result()),
					//Data(&BuildOrderManager::TimePassed,			Condition(80.0f),			&BuildOrderManager::RemoveScoutToProxy,					Result(ROBO, 0)), // TODO swap this back for better opponents
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(146.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::PrepRampWallOff,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(153.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(153.0f),			&BuildOrderManager::SendAllInAttack,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::TrainUnit,							Result(IMMORTAL)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(173.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(177.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(185.0f),			&BuildOrderManager::TrainUnit,							Result(SENTRY)),
					Data(&BuildOrderManager::TimePassed,			Condition(196.0f),			&BuildOrderManager::TrainUnit,							Result(PRISM)),
					Data(&BuildOrderManager::TimePassed,			Condition(196.0f),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::ContinueSpendingNexusEnergy,		Result()),
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
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(ROBO),			&BuildOrderManager::NOP,								Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(125.0f),			&BuildOrderManager::SackUnit,							Result(PROBE)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(146.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(153.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(153.0f),			&BuildOrderManager::SendAllInAttack,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::TrainUnit,							Result(IMMORTAL)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(173.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(177.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(185.0f),			&BuildOrderManager::TrainUnit,							Result(SENTRY)),
					Data(&BuildOrderManager::TimePassed,			Condition(196.0f),			&BuildOrderManager::TrainUnit,							Result(PRISM)),
					Data(&BuildOrderManager::TimePassed,			Condition(196.0f),			&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::ContinueBuildingPylons,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::ContinueSpendingNexusEnergy,		Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::SetUnitProduction,					Result(IMMORTAL)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(200.0f),			&BuildOrderManager::SetWarpInAtProxy,					Result(STALKER)),
	};
}

void BuildOrderManager::Set1GateExpand()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(65.0f),			&BuildOrderManager::CancelImmediatelySemiSaturateGasses,Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(73.0f),			&BuildOrderManager::BuildBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(84.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(98.0f),			&BuildOrderManager::BuildBuilding,						Result(NEXUS)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::SafeRallyPoint,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(126.0f),			&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::SafeRallyPoint,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(133.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(150.0f),			&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(SENTRY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::TrainUnit,							Result(IMMORTAL)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(190.0f),			&BuildOrderManager::EnergyRechargeUnit,					Result(SENTRY)),
					Data(&BuildOrderManager::TimePassed,			Condition(190.0f),			&BuildOrderManager::CutWorkers,							Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(192.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::TimePassed,			Condition(198.0f),			&BuildOrderManager::BuildNaturalDefensiveBuilding,		Result(BATTERY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::TrainUnit,							Result(IMMORTAL)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::ImmediatelySemiSaturateGasses,		Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ContinueMacro,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(180.0f),			&BuildOrderManager::Add1GateExpandPvPTransitions,		Result()),
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
					Data(&BuildOrderManager::TimePassed,			Condition(73.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({CYBERCORE, NEXUS})),
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::TrainUnit,							Result(ZEALOT)),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(NEXUS, 2),		&BuildOrderManager::NOP,								Result()),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(NEXUS, 2),		&BuildOrderManager::CheckForBunkerRush,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(84.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(100.0f),			&BuildOrderManager::CheckForProxyRax,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(101.0f),			&BuildOrderManager::BuildBuilding,						Result(ASSIMILATOR)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::TrainUnit,							Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ResearchWarpgate,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::AdeptDefendBaseTerran,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),			&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(148.0f),			&BuildOrderManager::BuildBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(155.0f),			&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(175.0f),			&BuildOrderManager::BuildBuilding,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(183.0f),			&BuildOrderManager::BuildBuildingMulti,					Result({GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::ResearchBlink,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(203.0f),			&BuildOrderManager::ChronoBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),			&BuildOrderManager::BuildBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(210.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(217.0f),			&BuildOrderManager::SetUnitProduction,					Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),			&BuildOrderManager::TrainUnit,							Result(OBSERVER)),
					Data(&BuildOrderManager::TimePassed,			Condition(223.0f),			&BuildOrderManager::ChronoBuilding,						Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(225.0f),			&BuildOrderManager::BuildBuilding,						Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(227.0f),			&BuildOrderManager::ContinueMakingWorkers,				Result(0)),
					Data(&BuildOrderManager::TimePassed,			Condition(235.0f),			&BuildOrderManager::BuildProxy,							Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(243.0f),			&BuildOrderManager::TrainUnit,							Result(PRISM)),
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
					Data(&BuildOrderManager::TimePassed,			Condition(290.0f),			&BuildOrderManager::ContinueMacro,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(300.0f),			&BuildOrderManager::Add4GateBlinkPvTTransitions,		Result()),
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
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::TrainUnit,							Result(ZEALOT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(GATEWAY),			&BuildOrderManager::BuildBuilding,						Result(CYBERCORE)),
					Data(&BuildOrderManager::HasUnits,				Condition(ZEALOT, 1),		&BuildOrderManager::CannonRushAttack,					Result()),
					Data(&BuildOrderManager::HasBuildingStarted,	Condition(STARGATE),		&BuildOrderManager::TrainUnit,							Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),		&BuildOrderManager::ChronoBuilding,						Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(STARGATE),		&BuildOrderManager::BuildBuilding,						Result(FLEET_BEACON)),
					Data(&BuildOrderManager::HasBuilding,			Condition(FLEET_BEACON),	&BuildOrderManager::ContinueSpendingNexusEnergy,		Result()),
		
	};
}

void BuildOrderManager::SetBlinkProxyRoboPressureBuild()
{
	build_order = { Data(&BuildOrderManager::TimePassed,			Condition(6.5f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(17.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(40.5f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(60.5f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(70.0f),				&BuildOrderManager::BuildBuilding,					Result(CYBERCORE)),
					Data(&BuildOrderManager::TimePassed,			Condition(82.0f),				&BuildOrderManager::BuildBuilding,					Result(ASSIMILATOR)),
					Data(&BuildOrderManager::TimePassed,			Condition(85.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::NumWorkers,			Condition(21),					&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::ChronoBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::ChronoBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(117.0f),				&BuildOrderManager::ResearchWarpgate,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(117.0f),				&BuildOrderManager::Contain,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(136.0f),				&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(136.0f),				&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(137.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::TimePassed,			Condition(152.0f),				&BuildOrderManager::BuildBuilding,					Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(159.0f),				&BuildOrderManager::BuildProxy,						Result(PYLON)) ,
					Data(&BuildOrderManager::TimePassed,			Condition(166.0f),				&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(171.0f),				&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(176.5f),				&BuildOrderManager::BuildBuilding,					Result(NEXUS)),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ResearchBlink,					Result(UPGRADE_ID::BLINKTECH)),
					Data(&BuildOrderManager::IsResearching,			Condition(TWILIGHT),			&BuildOrderManager::ChronoTillFinished,				Result(TWILIGHT)),
					Data(&BuildOrderManager::TimePassed,			Condition(185.0f),				&BuildOrderManager::UncutWorkers,					Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(185.0f),				&BuildOrderManager::SetWarpInAtProxy,				Result(1)),
					Data(&BuildOrderManager::TimePassed,			Condition(185.0f),				&BuildOrderManager::SetUnitProduction,				Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),				&BuildOrderManager::BuildProxy,						Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),				&BuildOrderManager::ContinueBuildingPylons,			Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(205.0f),				&BuildOrderManager::ContinueMakingWorkers,			Result(0)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),				&BuildOrderManager::TrainFromProxy,					Result(ROBO)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),				&BuildOrderManager::ContinueChronoProxyRobo,		Result(ROBO))
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
					Data(&BuildOrderManager::TimePassed,			Condition(128.0f),				&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(135.0f),				&BuildOrderManager::ResearchWarpgate,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(158.0f),				&BuildOrderManager::BuildBuilding,					Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(160.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(172.0f),				&BuildOrderManager::BuildBuilding,					Result(PYLON)),
					Data(&BuildOrderManager::HasGas,				Condition(100),					&BuildOrderManager::PullOutOfGas,					Result(6)),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ResearchCharge,					Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ChronoTillFinished,				Result(TWILIGHT)),
					Data(&BuildOrderManager::NumWorkers,			Condition(30),					&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(189.0f),				&BuildOrderManager::BuildBuildingMulti,				Result({GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY, GATEWAY})),
					Data(&BuildOrderManager::TimePassed,			Condition(215.0f),				&BuildOrderManager::TrainUnit,						Result(PRISM)),
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
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainUnit,						Result(STALKER)),
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
					Data(&BuildOrderManager::TimePassed,			Condition(215.0f),				&BuildOrderManager::TrainUnit,						Result(PRISM)),
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
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::ResearchWarpgate,				Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),				&BuildOrderManager::ChronoBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(130.0f),				&BuildOrderManager::BuildBuilding,					Result(TWILIGHT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(158.0f),				&BuildOrderManager::BuildBuilding,					Result(ROBO)),
					Data(&BuildOrderManager::TimePassed,			Condition(170.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::NumWorkers,			Condition(30),					&BuildOrderManager::CutWorkers,						Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ResearchGlaives,				Result()),
					Data(&BuildOrderManager::HasBuilding,			Condition(TWILIGHT),			&BuildOrderManager::ChronoBuilding,					Result(TWILIGHT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainUnit,						Result(ADEPT)),
					Data(&BuildOrderManager::TimePassed,			Condition(191.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(198.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::HasBuilding,			Condition(ROBO),				&BuildOrderManager::TrainUnit,						Result(PRISM)),
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
					Data(&BuildOrderManager::TimePassed,			Condition(116.0f),				&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(116.0f),				&BuildOrderManager::ChronoBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(126.0f),				&BuildOrderManager::BuildBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(136.0f),				&BuildOrderManager::Contain,						Result()),
					Data(&BuildOrderManager::TimePassed,			Condition(136.0f),				&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(149.0f),				&BuildOrderManager::BuildBuilding,					Result(DARK_SHRINE)),
					Data(&BuildOrderManager::TimePassed,			Condition(166.0f),				&BuildOrderManager::TrainUnit,						Result(STALKER)),
					Data(&BuildOrderManager::TimePassed,			Condition(177.0f),				&BuildOrderManager::TrainUnit,						Result(STALKER)),
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
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::TrainUnit,						Result(ADEPT)),
					Data(&BuildOrderManager::HasBuilding,			Condition(CYBERCORE),			&BuildOrderManager::ChronoBuilding,					Result(GATEWAY)),
					Data(&BuildOrderManager::TimePassed,			Condition(146.0f),				&BuildOrderManager::TrainUnit,						Result(STALKER)),
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