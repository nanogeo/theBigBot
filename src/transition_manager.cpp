
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
}

void TransitionManager::CheckTransitions()
{
	for (auto itr = possible_transitions.begin(); itr != possible_transitions.end();)
	{
		bool(sc2::TransitionManager:: * remove_condition)() = itr->remove_condition;
		bool(sc2::TransitionManager:: * condition)() = itr->condition;
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

void TransitionManager::AddTransitionsForOracleGatewaymanPvZ()
{
	// charge, colo, immortal, phoenix
	// lots of bases and defending with queen, ling, hydra & not roaches -> charge
	// defending with mostly ling, hydra -> colo
	// defending with mostly roaches -> immortal
	// harassing with muta -> phoenix
	int num_queens = mediator->GetEnemyUnitCount(QUEEN);
	int num_zerglings = mediator->GetEnemyUnitCount(ZERGLING);
	int num_hydralisks = mediator->GetEnemyUnitCount(HYDRA);
	int num_roaches = mediator->GetEnemyUnitCount(ROACH);


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

bool TransitionManager::WorkerRushTransitionCondition()
{
	// if > 4? enemy workers are within 20? of base
	return Utility::GetUnitsWithin(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ PROBE, SCV, DRONE })), 
		mediator->GetStartLocation(), 40).size() > 2;
	
}

bool TransitionManager::WorkerRushTransitionRemoveCondition()
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

bool TransitionManager::ScourTransitionCondition()
{
	if ((mediator->GetCurrentTime() > 300 && Utility::DistanceToClosest(mediator->GetUnits(IsUnits({NEXUS, COMMAND_CENTER, ORBITAL, PLANETARY, HATCHERY, LAIR, HIVE})), mediator->GetEnemyStartLocation()) > 15)
		|| (mediator->GetCurrentTime() > 600 && mediator->GetArmySupply() > 30)
		|| mediator->GetCurrentTime() > 900)
	{
		return true;
	}
	return false;
}

bool TransitionManager::ScourTransitionRemoveCondition()
{
	return false;
}

void TransitionManager::ScourTransitionEnterAction()
{
	mediator->ScourMap();
}
	
}