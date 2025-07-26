
#include "mediator.h"
#include "transition_manager.h"
#include "definitions.h"
#include "utility.h"



namespace sc2 {

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
}

void TransitionManager::AddTransitionsFor4GateBlinkPvT()
{
	// charge, colo, immortal
	
	int num_tanks = mediator->GetEnemyUnitCount(SIEGE_TANK) + mediator->GetEnemyUnitCount(SIEGE_TANK_SIEGED);
	int num_marines = mediator->GetEnemyUnitCount(MARINE);
	int num_marauders = mediator->GetEnemyUnitCount(MARAUDER);

	if (num_tanks > 2)
	{
		// defending with tanks -> charge
		
	}
	else if (num_marines > 10) // TODO adjust number 
	{
		// lots of marines -> colo

	}
	else if (num_marauders > 10) // TODO adjust number 
	{
		// lots of marauders -> immortal

	}
}

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
	if ((mediator->GetCurrentTime() > 300 && Utility::DistanceToClosest(mediator->GetUnits(IsUnits({NEXUS, COMMAND_CENTER, ORBITAL, PLANETARY, HATCHERY, LAIR, HIVE})), mediator->GetEnemyStartLocation()) > 15)
		|| (mediator->GetCurrentTime() > 600 && mediator->GetArmySupply() > 30)
		|| mediator->GetCurrentTime() > 900)
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
		mediator->GetCurrentTime() > 60 &&
		mediator->GetNumBuildActions(PYLON) == 0)
		return true;
	return false;
}

bool TransitionManager::FixEarlySupplyBlockRemoveCondition() const
{
	if (!mediator->HasActionOfType(&ActionManager::ActionContinueBuildingPylons))
		return true;
	return false;
}

void TransitionManager::FixEarlySupplyBlockEnterAction()
{
	mediator->AddUniqueAction(&ActionManager::ActionContinueBuildingPylons, new ActionArgData());
}

bool TransitionManager::NullRemoveCondition() const
{
	return false;
}

bool TransitionManager::PvZAddColossusCondition() const
{
	if (mediator->GetEnemyUnitCount(ZERGLING) > 30 || mediator->GetEnemyUnitCount(HYDRA) > 5)
		return true;
	return false;
}

void TransitionManager::PvZAddColossusEnterAction()
{
	if (mediator->GetNumUnits(ROBO) == 0)
		mediator->BuildBuilding(ROBO);
	if (mediator->GetNumUnits(ROBO_BAY) == 0)
		mediator->BuildBuildingWhenAble(ROBO_BAY);
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
	if (mediator->GetNumUnits(ROBO) == 0)
		mediator->BuildBuilding(ROBO);
	mediator->IncreaseUnitAmountInTargetComposition(IMMORTAL, 5);
	mediator->IncreaseUnitAmountInTargetComposition(PRISM, 1);
}

}