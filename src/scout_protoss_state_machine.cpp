
#include "scout_protoss_state_machine.h"
#include "theBigBot.h"


namespace sc2 {



#pragma region ScoutPInitialMove

void ScoutPInitialMove::TickState()
{
	if (state_machine->scout->orders.size() == 0 || state_machine->scout->orders[0].target_pos != state_machine->current_target)
		agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target);
}

void ScoutPInitialMove::EnterState()
{
	state_machine->current_target = state_machine->enemy_main;
}

void ScoutPInitialMove::ExitState()
{
	return;
}

State* ScoutPInitialMove::TestTransitions()
{
	if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 1)
	{
		if (agent->mediator.GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON)).size() > 0)
			return new ScoutPScoutPylon(agent, state_machine);
		else
			return new ScoutPScoutMain(agent, state_machine, false);
	}
	return NULL;
}

std::string ScoutPInitialMove::toString()
{
	return "initial move";
}

#pragma endregion

#pragma region ScoutPScoutPylon

void ScoutPScoutPylon::TickState()
{
	if (state_machine->scout->orders.size() == 0)
	{
		Units pylons = agent->mediator.GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON));
		if (pylons.size() > 0)
			agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, pylons[0]->pos);
	}
}

void ScoutPScoutPylon::EnterState()
{
	Units pylons = agent->mediator.GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON));
	if (pylons.size() > 0)
		agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, pylons[0]->pos);
}

void ScoutPScoutPylon::ExitState()
{
	return;
}

State* ScoutPScoutPylon::TestTransitions()
{
	Units pylons = agent->mediator.GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON));
	if (pylons.size() == 0)
		return new ScoutPScoutMain(agent, state_machine, false);
	if (pylons.size() > 0 && Distance2D(state_machine->scout->pos, pylons[0]->pos) < 2)
		return new ScoutPScoutMain(agent, state_machine, true);
	return NULL;
}

std::string ScoutPScoutPylon::toString()
{
	return "scout pylon";
}

#pragma endregion

#pragma region ScoutPScoutMain

void ScoutPScoutMain::TickState()
{
	if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 2)
	{
		state_machine->index++;
		if (state_machine->index < state_machine->main_scout_path.size())
			state_machine->current_target = state_machine->main_scout_path[state_machine->index];
	}
	agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target);
}

void ScoutPScoutMain::EnterState()
{
	state_machine->index = 0;
	state_machine->current_target = state_machine->main_scout_path[0];
}

void ScoutPScoutMain::ExitState()
{
	return;
}

State* ScoutPScoutMain::TestTransitions()
{
	if (scouted_pylon == false && agent->mediator.GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON)).size() > 0)
		return new ScoutPScoutPylon(agent, state_machine);
	if (state_machine->index >= state_machine->main_scout_path.size())
		return new ScoutPScoutNatural(agent, state_machine);
	if (agent->mediator.GetCurrentTime() > 120)
		return new ScoutPReturnToBase(agent, state_machine);
	return NULL;
}

std::string ScoutPScoutMain::toString()
{
	return "scout main";
}

#pragma endregion

#pragma region ScoutPScoutNatural

void ScoutPScoutNatural::TickState()
{
	if (state_machine->scout->orders.size() == 0 && Distance2D(state_machine->scout->pos, agent->mediator.GetEnemyNaturalLocation()) > 6)
		agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, agent->mediator.GetEnemyNaturalLocation());

}

void ScoutPScoutNatural::EnterState()
{
	agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, agent->mediator.GetEnemyNaturalLocation());
}

void ScoutPScoutNatural::ExitState()
{
	return;
}

State* ScoutPScoutNatural::TestTransitions()
{
	if (Distance2D(state_machine->scout->pos, agent->mediator.GetEnemyNaturalLocation()) < 8)
	{
		return new ScoutPScoutMain(agent, state_machine, agent->mediator.GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON)).size() != 0);
	}
	return NULL;
}

std::string ScoutPScoutNatural::toString()
{
	return "scout natural";
}

#pragma endregion


#pragma region ScoutPReturnToBase

void ScoutPReturnToBase::TickState()
{
	return;
}

void ScoutPReturnToBase::EnterState()
{
	agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, agent->locations->start_location);
}

void ScoutPReturnToBase::ExitState()
{
	return;
}

State* ScoutPReturnToBase::TestTransitions()
{
	if (Distance2D(state_machine->scout->pos, agent->locations->start_location) <= 20)
	{
		agent->mediator.MarkStateMachineForDeletion(state_machine);
		return NULL;
	}
	return NULL;
}

std::string ScoutPReturnToBase::toString()
{
	return "return to base";
}

#pragma endregion


ScoutProtossStateMachine::~ScoutProtossStateMachine()
{
	
}


}