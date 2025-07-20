
#include "scout_protoss_state_machine.h"
#include "theBigBot.h"


namespace sc2 {



#pragma region ScoutPInitialMove

void ScoutPInitialMove::TickState()
{
	if (state_machine->scout->orders.size() == 0 || state_machine->scout->orders[0].target_pos != state_machine->current_target)
		mediator->SetUnitCommand(state_machine->scout, A_MOVE, state_machine->current_target, CommandPriorty::low);
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
		if (mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON)).size() > 0 &&
			Utility::DistanceToClosest(mediator->GetUnits(IsUnit(PYLON)), mediator->GetEnemyStartLocation()) < 25)
			return new ScoutPScoutPylon(mediator, state_machine);
		else
			return new ScoutPScoutMain(mediator, state_machine, false);
	}
	return nullptr;
}

std::string ScoutPInitialMove::toString() const
{
	return "initial move";
}

#pragma endregion

#pragma region ScoutPScoutPylon

void ScoutPScoutPylon::TickState()
{
	if (state_machine->scout->orders.size() == 0)
	{
		Units pylons = mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON));
		if (pylons.size() > 0)
			mediator->SetUnitCommand(state_machine->scout, A_MOVE, pylons[0]->pos, CommandPriorty::low);
	}
}

void ScoutPScoutPylon::EnterState()
{
	Units pylons = mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON));
	if (pylons.size() > 0)
		mediator->SetUnitCommand(state_machine->scout, A_MOVE, pylons[0]->pos, CommandPriorty::low);
}

void ScoutPScoutPylon::ExitState()
{
	return;
}

State* ScoutPScoutPylon::TestTransitions()
{
	Units pylons = mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON));
	if (pylons.size() == 0)
		return new ScoutPScoutMain(mediator, state_machine, false);
	if (pylons.size() > 0 && Distance2D(state_machine->scout->pos, pylons[0]->pos) < 2)
		return new ScoutPScoutMain(mediator, state_machine, true);
	return nullptr;
}

std::string ScoutPScoutPylon::toString() const
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
	mediator->SetUnitCommand(state_machine->scout, A_MOVE, state_machine->current_target, CommandPriorty::low);
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
	if (scouted_pylon == false && mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON)).size() > 0 &&
		Utility::DistanceToClosest(mediator->GetUnits(IsUnit(PYLON)), mediator->GetEnemyStartLocation()) < 25)
		return new ScoutPScoutPylon(mediator, state_machine);
	if (state_machine->index >= state_machine->main_scout_path.size())
		return new ScoutPScoutNatural(mediator, state_machine);
	if (mediator->GetCurrentTime() > 120)
		return new ScoutPReturnToBase(mediator, state_machine);
	return nullptr;
}

std::string ScoutPScoutMain::toString() const
{
	return "scout main";
}

#pragma endregion

#pragma region ScoutPScoutNatural

void ScoutPScoutNatural::TickState()
{
	if (state_machine->scout->orders.size() == 0 && Distance2D(state_machine->scout->pos, mediator->GetEnemyNaturalLocation()) > 6)
		mediator->SetUnitCommand(state_machine->scout, A_MOVE, mediator->GetEnemyNaturalLocation(), CommandPriorty::low);

}

void ScoutPScoutNatural::EnterState()
{
	mediator->SetUnitCommand(state_machine->scout, A_MOVE, mediator->GetEnemyNaturalLocation(), CommandPriorty::low);
}

void ScoutPScoutNatural::ExitState()
{
	return;
}

State* ScoutPScoutNatural::TestTransitions()
{
	if (Distance2D(state_machine->scout->pos, mediator->GetEnemyNaturalLocation()) < 8)
	{
		return new ScoutPScoutMain(mediator, state_machine, mediator->GetUnits(Unit::Alliance::Enemy, IsUnit(PYLON)).size() != 0);
	}
	return nullptr;
}

std::string ScoutPScoutNatural::toString() const
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
	mediator->SetUnitCommand(state_machine->scout, A_MOVE, mediator->GetStartLocation(), CommandPriorty::low);
}

void ScoutPReturnToBase::ExitState()
{
	return;
}

State* ScoutPReturnToBase::TestTransitions()
{
	if (Distance2D(state_machine->scout->pos, mediator->GetStartLocation()) <= 20)
	{
		mediator->MarkStateMachineForDeletion(state_machine);
		return nullptr;
	}
	return nullptr;
}

std::string ScoutPReturnToBase::toString() const
{
	return "return to base";
}

#pragma endregion


ScoutProtossStateMachine::~ScoutProtossStateMachine()
{
	
}

const Unit* ScoutProtossStateMachine::GetScout()
{
	return scout;
}

}