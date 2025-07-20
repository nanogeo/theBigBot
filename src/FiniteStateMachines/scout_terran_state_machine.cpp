
#include "scout_terran_state_machine.h"
#include "mediator.h"


namespace sc2 {



#pragma region ScoutTInitialMove

void ScoutTInitialMove::TickState()
{
	if (state_machine->scout->orders.size() == 0 || state_machine->scout->orders[0].target_pos != state_machine->current_target)
		mediator->SetUnitCommand(state_machine->scout, A_MOVE, state_machine->current_target, CommandPriorty::low);
}

void ScoutTInitialMove::EnterState()
{
	state_machine->current_target = state_machine->enemy_main;
}

void ScoutTInitialMove::ExitState()
{
	return;
}

State* ScoutTInitialMove::TestTransitions()
{
	if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 1)
		return new ScoutTScoutMain(mediator, state_machine);
	return nullptr;
}

std::string ScoutTInitialMove::toString() const
{
	return "initial move";
}

#pragma endregion

#pragma region ScoutTScoutMain

void ScoutTScoutMain::TickState()
{
	if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 2)
	{
		state_machine->index++;
		if (state_machine->index < state_machine->main_scout_path.size())
			state_machine->current_target = state_machine->main_scout_path[state_machine->index];
	}
	mediator->SetUnitCommand(state_machine->scout, A_MOVE, state_machine->current_target, CommandPriorty::low);
}

void ScoutTScoutMain::EnterState()
{
	state_machine->index = 0;
	state_machine->current_target = state_machine->main_scout_path[0];
}

void ScoutTScoutMain::ExitState()
{
	return;
}

State* ScoutTScoutMain::TestTransitions()
{
	if (mediator->GetFirstBarrackTiming() > 0 && mediator->GetCurrentTime() >= mediator->GetFirstBarrackTiming() + 46 + 12)
	{
		return new ScoutTScoutRax(mediator, state_machine);
	}
	if (state_machine->index >= state_machine->main_scout_path.size())
	{
		if (mediator->GetUnits(IsUnit(BARRACKS)).size() > 1 ||
			mediator->GetFirstBarrackTiming() == 0 ||
			mediator->GetUnits(IsUnit(REFINERY)).size() > 1)
		{
			state_machine->index = 0;
			state_machine->current_target = state_machine->main_scout_path[0];
			//mediator->scouting_manager.CheckTerranScoutingInfoEarly(); // TODO check if needed
		}
		else
		{
			return new ScoutTScoutNatural(mediator, state_machine);
		}
	}
	return nullptr;
}

std::string ScoutTScoutMain::toString() const
{
	return "scout main";
}

#pragma endregion

#pragma region ScoutTScoutNatural

void ScoutTScoutNatural::TickState()
{
	if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 2)
	{
		state_machine->index++;
		if (state_machine->index < state_machine->natural_scout_path.size())
			state_machine->current_target = state_machine->natural_scout_path[state_machine->index];
	}
	mediator->SetUnitCommand(state_machine->scout, A_MOVE, state_machine->current_target, CommandPriorty::low);
}

void ScoutTScoutNatural::EnterState()
{
	state_machine->index = 0;
	state_machine->current_target = state_machine->natural_scout_path[0];
}

void ScoutTScoutNatural::ExitState()
{
	return;
}

State* ScoutTScoutNatural::TestTransitions()
{
	if (mediator->GetNaturalTiming() > 0 || state_machine->index >= state_machine->natural_scout_path.size())
	{
		return new ScoutTScoutMain(mediator, state_machine);
	}
	return nullptr;
}

std::string ScoutTScoutNatural::toString() const
{
	return "scout natural";
}

#pragma endregion

#pragma region ScoutTScoutRax

void ScoutTScoutRax::TickState()
{

}

void ScoutTScoutRax::EnterState()
{
	if (mediator->GetUnits(IsUnits({ BARRACKS, BARRACKS_FLYING })).size() > 0)
	{
		rax = mediator->GetUnits(IsUnits({ BARRACKS, BARRACKS_FLYING }))[0];
		mediator->SetUnitCommand(state_machine->scout, A_MOVE, rax->pos, CommandPriorty::low);
	}
	else
	{
		for (const auto& pos : mediator->GetEnemySavedPositions())
		{
			if (pos.first->unit_type == BARRACKS || pos.first->unit_type == BARRACKS_FLYING)
			{
				rax = pos.first;
				mediator->SetUnitCommand(state_machine->scout, A_MOVE, rax->pos, CommandPriorty::low);
				return;
			}
		}
	}
	std::cerr << "No barracks found in ScoutTScoutRax::EnterState" << std::endl;
	mediator->LogMinorError();
}

void ScoutTScoutRax::ExitState()
{
	return;
}

State* ScoutTScoutRax::TestTransitions()
{
	if (rax == nullptr)
		return new ScoutTReturnToBase(mediator, state_machine);

	if (mediator->GetCurrentTime() >= mediator->GetFirstBarrackTiming() + 46 + 20 || mediator->GetUnits(IsUnit(MARINE)).size() > 0)
	{
		mediator->SetFirstBarrackProduction(FirstRaxProduction::reaper);

		for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy))
		{
			if (unit->unit_type == BARRACKS_TECH_LAB)
			{
				if (Distance2D(unit->pos, rax->pos) < 3)
				{
					mediator->SetFirstBarrackProduction(FirstRaxProduction::techlab);
					break;
				}
			}
			else if (unit->unit_type == BARRACKS_REACTOR)
			{
				if (Distance2D(unit->pos, rax->pos) < 3)
				{
					mediator->SetFirstBarrackProduction(FirstRaxProduction::reactor);
					break;
				}
			}
			else if (unit->unit_type == MARINE)
			{
				mediator->SetFirstBarrackProduction(FirstRaxProduction::marine);
				break;
			}
		}
		return new ScoutTReturnToBase(mediator, state_machine);
	}
	return nullptr;
}

std::string ScoutTScoutRax::toString() const
{
	return "scout rax";
}

#pragma endregion

#pragma region ScoutTReturnToBase

void ScoutTReturnToBase::TickState()
{
	for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy))
	{
		if (unit->unit_type == MARINE)
		{
			mediator->SetFirstBarrackProduction(FirstRaxProduction::marine);
			break;
		}
	}
	return;
}

void ScoutTReturnToBase::EnterState()
{
	mediator->SetUnitCommand(state_machine->scout, A_MOVE, mediator->GetLocations().start_location, CommandPriorty::low);
}

void ScoutTReturnToBase::ExitState()
{
	return;
}

State* ScoutTReturnToBase::TestTransitions()
{
	if (Distance2D(state_machine->scout->pos, mediator->GetLocations().start_location) <= 20)
	{
		mediator->MarkStateMachineForDeletion(state_machine);
		return nullptr;
	}
	return nullptr;
}

std::string ScoutTReturnToBase::toString() const
{
	return "return to base";
}

#pragma endregion


ScoutTerranStateMachine::~ScoutTerranStateMachine()
{
	mediator->PlaceWorker(scout);
}

const Unit* ScoutTerranStateMachine::GetScout()
{
	return scout;
}

}