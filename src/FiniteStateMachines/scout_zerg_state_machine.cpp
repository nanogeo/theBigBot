
#include "scout_zerg_state_machine.h"
#include "mediator.h"


namespace sc2 {


#pragma region ScoutZInitialMove

void ScoutZInitialMove::TickState()
{
	mediator->SetUnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target, 0);
}

void ScoutZInitialMove::EnterState()
{
	state_machine->current_target = state_machine->enemy_main;
}

void ScoutZInitialMove::ExitState()
{
	return;
}

State* ScoutZInitialMove::TestTransitions()
{
	if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 1)
		return new ScoutZScoutMain(mediator, state_machine);
	return nullptr;
}

std::string ScoutZInitialMove::toString()
{
	return "initial move";
}

#pragma endregion

#pragma region ScoutZScoutMain

void ScoutZScoutMain::TickState()
{
	if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 2)
	{
		state_machine->index++;
		if (state_machine->index < state_machine->main_scout_path.size())
			state_machine->current_target = state_machine->main_scout_path[state_machine->index];
	}
	mediator->SetUnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target, 0);
}

void ScoutZScoutMain::EnterState()
{
	state_machine->index = 0;
	state_machine->current_target = state_machine->main_scout_path[0];
}

void ScoutZScoutMain::ExitState()
{
	return;
}

State* ScoutZScoutMain::TestTransitions()
{
	if (state_machine->index >= state_machine->main_scout_path.size())
	{
		if (mediator->scouting_manager.natural_timing == 0)
		{
			return new ScoutZScoutNatural(mediator, state_machine);
		}
		else if (mediator->scouting_manager.third_timing == 0 &&
					((mediator->scouting_manager.spawning_pool_timing > 0 && mediator->scouting_manager.first_gas_timing > 0) ||
					mediator->GetCurrentTime() > 120))
		{
			return new ScoutZLookFor3rd(mediator, state_machine);
		}
		else
		{
			state_machine->index = 0;
			state_machine->current_target = state_machine->main_scout_path[state_machine->index];
		}
	}


	if (mediator->scouting_manager.spawning_pool_timing > 0 &&
		mediator->GetCurrentTime() > mediator->scouting_manager.spawning_pool_timing + 60)
	{
		mediator->MarkStateMachineForDeletion(state_machine);
		return nullptr;
	}
	return nullptr;
}

std::string ScoutZScoutMain::toString()
{
	return "scout main";
}

#pragma endregion

#pragma region ScoutZScoutNatural

void ScoutZScoutNatural::TickState()
{
	mediator->SetUnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target, 0);
}

void ScoutZScoutNatural::EnterState()
{
	state_machine->index = 0;
	state_machine->current_target = mediator->GetEnemyNaturalLocation();
}

void ScoutZScoutNatural::ExitState()
{
	return;
}

State* ScoutZScoutNatural::TestTransitions()
{
	if (mediator->scouting_manager.natural_timing > 0 || Distance2D(state_machine->scout->pos, state_machine->current_target) < 2)
		return new ScoutZScoutMain(mediator, state_machine);
	return nullptr;
}

std::string ScoutZScoutNatural::toString()
{
	return "scout natural";
}

#pragma endregion

#pragma region ScoutZLookFor3rd

void ScoutZLookFor3rd::TickState()
{
	if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 8)
	{
		state_machine->index++;
		if (state_machine->index < state_machine->possible_3rds.size())
			state_machine->current_target = state_machine->possible_3rds[state_machine->index];
	}
	mediator->SetUnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target, 0);
}

void ScoutZLookFor3rd::EnterState()
{
	state_machine->index = 0;
	state_machine->current_target = state_machine->possible_3rds[0];
}

void ScoutZLookFor3rd::ExitState()
{
	return;
}

State* ScoutZLookFor3rd::TestTransitions()
{
	if (state_machine->index >= state_machine->possible_3rds.size() || mediator->scouting_manager.third_timing > 0)
		return new ScoutZScoutMain(mediator, state_machine);
	return nullptr;
}

std::string ScoutZLookFor3rd::toString()
{
	return "look for 3rd";
}

#pragma endregion

ScoutZergStateMachine::ScoutZergStateMachine(Mediator* mediator, std::string name, const Unit* scout, 
	Point2D enemy_main, std::vector<Point2D> main_scout_path, 
	Point2D enemy_natural_pos, std::vector<Point2D> possible_3rds) : StateMachine(mediator, name)
{
    current_state = new ScoutZInitialMove(mediator, this);
    this->scout = scout;
    this->enemy_main = enemy_main;
    this->main_scout_path = main_scout_path;
    this->enemy_natural_pos = enemy_natural_pos;
    this->possible_3rds = possible_3rds;

    current_state->EnterState();
}

ScoutZergStateMachine::~ScoutZergStateMachine()
{
	if (scout != nullptr && scout->is_alive)
		mediator->worker_manager.PlaceWorker(scout);
}


}