
#include "mediator.h"
#include "transition_manager.h"
#include "definitions.h"
#include "utility.h"



namespace sc2 {

bool TransitionManager::WorkerRushTransitionCondition() const
{
	// if > 2? enemy workers are within 20? of base
	return Utility::GetUnitsWithin(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ PROBE, SCV, DRONE })), mediator->GetStartLocation(), VERY_LONG_RANGE).size() > 2 ||
		Utility::GetUnitsWithin(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ PROBE, SCV, DRONE })), mediator->GetNaturalLocation(), VERY_LONG_RANGE).size() > 2;
	
}

bool TransitionManager::WorkerRushTransitionRemoveCondition() const
{
	return mediator->GetCurrentTime() > 180;
}
void TransitionManager::WorkerRushTransitionEnterAction()
{
	// start worker rush defense state machine
	mediator->CreateWorkerRushDefenseFSM();
	// change to worker rush defense build order
	if (mediator->GetCurrentTime() < 120)
	{
		mediator->SetWorkerRushDefenseBuidOrder();
		// cancel current actions
		mediator->CancelAllActionsOfType(&ActionManager::ActionBuildBuilding);
		mediator->CancelAllActionsOfType(&ActionManager::ActionBuildBuildingMulti);
		mediator->CancelAllActionsOfType(&ActionManager::ActionContinueMakingWorkers);
		// unsaturate gasses
	}
}

bool TransitionManager::ScourTransitionCondition() const
{
	UnitCost enemy_losses_last_minute = mediator->GetEnemyLossesSince(mediator->GetCurrentTime() - 60);
	if ((mediator->GetCurrentTime() > 300 && Utility::DistanceToClosest(mediator->GetUnits(IsUnits({NEXUS, COMMAND_CENTER, ORBITAL, PLANETARY, HATCHERY, LAIR, HIVE})), mediator->GetEnemyStartLocation()) > 15)
		|| (mediator->GetCurrentTime() > 600 && mediator->GetArmySupply() > 30 && enemy_losses_last_minute.mineral_cost + enemy_losses_last_minute.vespene_cost < 250)
		|| mediator->GetCurrentTime() > 900 && enemy_losses_last_minute.mineral_cost + enemy_losses_last_minute.vespene_cost < 250)
	{
		return true;
	}
	return false;
}

bool TransitionManager::ScourTransitionRemoveCondition() const
{
	return false;
}

void TransitionManager::ScourTransitionEnterAction()
{
	mediator->ScourMap();
}

bool TransitionManager::FixEarlySupplyBlockCondition() const
{
	if (mediator->GetSupplyCap() - mediator->GetSupplyUsed() <= 1 &&
		mediator->GetCurrentTime() > 180 &&
		mediator->GetNumBuildActions(PYLON) == 0 &&
		mediator->GetUnits(Unit::Alliance::Self, IsNotFinishedUnit(PYLON)).size() == 0)
		return true;
	return false;
}

bool TransitionManager::FixEarlySupplyBlockRemoveCondition() const
{
	if (mediator->HasActionOfType(&ActionManager::ActionContinueBuildingPylons))
		return true;
	return false;
}

void TransitionManager::FixEarlySupplyBlockEnterAction()
{
	mediator->TagWithTimestamp("transition_fix_supply_block");
	mediator->AddUniqueAction(&ActionManager::ActionContinueBuildingPylons, new ActionArgData());
}

bool TransitionManager::NullRemoveCondition() const
{
	return false;
}

bool TransitionManager::PvZAddZealotCondition() const
{
	UnitCost available_resources = mediator->GetAvailableResources();
	if (available_resources.mineral_cost > 1000 && available_resources.vespene_cost < 200)
		return true;

	std::map<UNIT_TYPEID, int> target_comp = mediator->GetTargetUnitComp();
	int units_needed = 0;
	int units_filled = 0;
	for (const auto& target : target_comp)
	{
		units_filled += mediator->GetNumUnits(target.first);
		units_needed += target.second;
	}
	return (float)(units_filled) / units_needed > .8f && mediator->GetAvailableResources().mineral_cost > 500;
}

void TransitionManager::PvZAddZealotEnterAction()
{
	mediator->TagWithTimestamp("transition_charge");
	mediator->AddRequiredUpgrade(U_CHARGE);
	mediator->IncreaseUnitAmountInTargetComposition(ZEALOT, 15);
	if (mediator->GetNumUnits(FORGE) + mediator->GetNumBuildActions(FORGE) < 2)
		mediator->BuildBuilding(FORGE);
}

bool TransitionManager::PvZAddColossusCondition() const
{
	if (mediator->GetEnemyUnitCount(ZERGLING) > 30 || mediator->GetEnemyUnitCount(HYDRA) > 5)
		return true;
	return false;
}

void TransitionManager::PvZAddColossusEnterAction()
{
	mediator->TagWithTimestamp("transition_colossus");
	if (mediator->GetNumUnits(ROBO) == 0)
		mediator->BuildBuilding(ROBO);
	if (mediator->GetNumUnits(ROBO_BAY) == 0)
		mediator->BuildBuildingWhenAble(ROBO_BAY);
	mediator->AddRequiredUpgrade(U_THERMAL_LANCE);
	mediator->IncreaseUnitAmountInTargetComposition(COLOSSUS, 3);
	mediator->IncreaseUnitAmountInTargetComposition(PRISM, 1);
}

bool TransitionManager::PvZAddImmortalCondition() const
{
	if (mediator->GetEnemyUnitCount(ROACH) > 20)
		return true;
	return false;
}

void TransitionManager::PvZAddImmortalEnterAction()
{
	mediator->TagWithTimestamp("transition_immortal");
	if (mediator->GetNumUnits(ROBO) == 0)
		mediator->BuildBuilding(ROBO);
	mediator->IncreaseUnitAmountInTargetComposition(IMMORTAL, 5);
	mediator->IncreaseUnitAmountInTargetComposition(PRISM, 1);
}

bool TransitionManager::PvTAddZealotCondition() const
{
	int tanks = mediator->GetEnemyUnitCount(SIEGE_TANK) + mediator->GetEnemyUnitCount(SIEGE_TANK_SIEGED);
	if (tanks >= 3)
		return true;

	UnitCost available_resources = mediator->GetAvailableResources();
	if (available_resources.mineral_cost > 1000 && available_resources.vespene_cost < 200)
		return true;

	std::map<UNIT_TYPEID, int> target_comp = mediator->GetTargetUnitComp();
	int units_needed = 0;
	int units_filled = 0;
	for (const auto& target : target_comp)
	{
		units_filled += mediator->GetNumUnits(target.first);
		units_needed += target.second;
	}
	return (float)(units_filled) / units_needed > .8f && available_resources.mineral_cost > 500;
}

void TransitionManager::PvTAddZealotEnterAction()
{
	mediator->TagWithTimestamp("transition_charge");
	mediator->AddRequiredUpgrade(U_CHARGE);
	mediator->IncreaseUnitAmountInTargetComposition(ZEALOT, 15);
	if (mediator->GetNumUnits(FORGE) + mediator->GetNumBuildActions(FORGE) < 2)
		mediator->BuildBuilding(FORGE);
}

bool TransitionManager::PvTAddColossusCondition() const
{
	if (mediator->GetEnemyUnitCount(MARINE) > 20)
		return true;
	return false;
}

void TransitionManager::PvTAddColossusEnterAction()
{
	mediator->TagWithTimestamp("transition_colossus");
	if (mediator->GetNumUnits(ROBO) == 0)
		mediator->BuildBuilding(ROBO);
	if (mediator->GetNumUnits(ROBO_BAY) == 0)
		mediator->BuildBuildingWhenAble(ROBO_BAY);
	mediator->AddRequiredUpgrade(U_THERMAL_LANCE);
	mediator->IncreaseUnitAmountInTargetComposition(COLOSSUS, 3); // transition 4 gate blink army to normal attack army when a colo is added
}

TransitionManager::TransitionManager(Mediator* mediator)
{
	this->mediator = mediator;
	possible_transitions.push_back(TransitionTemplate(&TransitionManager::WorkerRushTransitionCondition,
		&TransitionManager::WorkerRushTransitionRemoveCondition, &TransitionManager::WorkerRushTransitionEnterAction));

	possible_transitions.push_back(TransitionTemplate(&TransitionManager::ScourTransitionCondition,
		&TransitionManager::ScourTransitionRemoveCondition, &TransitionManager::ScourTransitionEnterAction));

	possible_transitions.push_back(TransitionTemplate(&TransitionManager::FixEarlySupplyBlockCondition,
		&TransitionManager::FixEarlySupplyBlockRemoveCondition, &TransitionManager::FixEarlySupplyBlockEnterAction));
}

void TransitionManager::CheckTransitions()
{
	for (auto itr = possible_transitions.begin(); itr != possible_transitions.end();)
	{
		bool(sc2::TransitionManager:: * remove_condition)() const = itr->remove_condition;
		bool(sc2::TransitionManager:: * condition)() const = itr->condition;
		if ((*this.*remove_condition)())
		{
			itr = possible_transitions.erase(itr);
		}
		else if ((*this.*condition)())
		{
			void(sc2::TransitionManager:: * enter_action)() = itr->enter_action;
			active_transitions.push_back(*itr);
			(*this.*enter_action)();
			itr = possible_transitions.erase(itr);
		}
		else
		{
			itr++;
		}
	}

}

void TransitionManager::AddZergTransitions()
{
	// charge, colo, immortal, phoenix
	// lots of bases and defending with queen, ling, hydra & not roaches -> charge
	// defending with mostly ling, hydra -> colo
	// defending with mostly roaches -> immortal
	// harassing with muta -> phoenix

	possible_transitions.push_back(TransitionTemplate(&TransitionManager::PvZAddColossusCondition,
		&TransitionManager::NullRemoveCondition, &TransitionManager::PvZAddColossusEnterAction));

	possible_transitions.push_back(TransitionTemplate(&TransitionManager::PvZAddImmortalCondition,
		&TransitionManager::NullRemoveCondition, &TransitionManager::PvZAddImmortalEnterAction));

	possible_transitions.push_back(TransitionTemplate(&TransitionManager::PvZAddZealotCondition,
		&TransitionManager::NullRemoveCondition, &TransitionManager::PvZAddZealotEnterAction));
}

void TransitionManager::AddTerranTransitions()
{
	possible_transitions.push_back(TransitionTemplate(&TransitionManager::PvTAddColossusCondition,
		&TransitionManager::NullRemoveCondition, &TransitionManager::PvTAddColossusEnterAction));

	possible_transitions.push_back(TransitionTemplate(&TransitionManager::PvTAddZealotCondition,
		&TransitionManager::NullRemoveCondition, &TransitionManager::PvTAddZealotEnterAction));
}


}