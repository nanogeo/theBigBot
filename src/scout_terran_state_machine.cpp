#pragma once

#include "scout_terran_state_machine.h"
#include "theBigBot.h"


namespace sc2 {



#pragma region ScoutTInitialMove

void ScoutTInitialMove::TickState()
{
	if (state_machine->scout->orders.size() == 0 || state_machine->scout->orders[0].target_pos != state_machine->current_target)
		agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target);
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
		return new ScoutTScoutMain(agent, state_machine);
	return NULL;
}

std::string ScoutTInitialMove::toString()
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
	agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target);
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
	if (agent->scout_info_terran.barrackes_timing > 0 && agent->Observation()->GetGameLoop() / 22.4 >= agent->scout_info_terran.barrackes_timing + 46 + 12)
	{
		return new ScoutTScoutRax(agent, state_machine);
	}
	if (state_machine->index >= state_machine->main_scout_path.size())
	{
		if (agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_BARRACKS)).size() > 1)
		{
			state_machine->index = 0;
			state_machine->current_target = state_machine->main_scout_path[0];
		}
		else if (agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_REFINERY)).size() <= 1)
		{
			return new ScoutTScoutNatural(agent, state_machine);
		}
		else if (agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_REFINERY)).size() > 1)
		{
			state_machine->index = 0;
			state_machine->current_target = state_machine->main_scout_path[0];
		}
	}
	return NULL;
}

std::string ScoutTScoutMain::toString()
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
	agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target);
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
	if (agent->scout_info_terran.natural_timing > 0 || state_machine->index >= state_machine->natural_scout_path.size())
	{
		return new ScoutTScoutMain(agent, state_machine);
	}
	return NULL;
}

std::string ScoutTScoutNatural::toString()
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
	agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_BARRACKS))[0]->pos);
	rax = agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_BARRACKS))[0];
}

void ScoutTScoutRax::ExitState()
{
	return;
}

State* ScoutTScoutRax::TestTransitions()
{
	if (agent->Observation()->GetGameLoop() / 22.4 >= agent->scout_info_terran.barrackes_timing + 46 + 20 || agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_MARINE)).size() > 0)
	{
		agent->scout_info_terran.first_rax_production = FirstRaxProduction::reaper;

		for (const auto& unit : agent->Observation()->GetUnits(Unit::Alliance::Enemy))
		{
			if (unit->unit_type == UNIT_TYPEID::TERRAN_BARRACKSTECHLAB)
			{
				if (Distance2D(unit->pos, rax->pos) < 3)
				{
					agent->scout_info_terran.first_rax_production = FirstRaxProduction::techlab;
					break;
				}
			}
			else if (unit->unit_type == UNIT_TYPEID::TERRAN_BARRACKSREACTOR)
			{
				if (Distance2D(unit->pos, rax->pos) < 3)
				{
					agent->scout_info_terran.first_rax_production = FirstRaxProduction::reactor;
					break;
				}
			}
			else if (unit->unit_type == UNIT_TYPEID::TERRAN_MARINE)
			{
				agent->scout_info_terran.first_rax_production = FirstRaxProduction::marine;
				break;
			}
		}
		return new ScoutTReturnToBase(agent, state_machine);
	}
	return NULL;
}

std::string ScoutTScoutRax::toString()
{
	return "scout rax";
}

#pragma endregion

#pragma region ScoutTReturnToBase

void ScoutTReturnToBase::TickState()
{
	for (const auto& unit : agent->Observation()->GetUnits(Unit::Alliance::Enemy))
	{
		if (unit->unit_type == UNIT_TYPEID::TERRAN_MARINE)
		{
			agent->scout_info_terran.first_rax_production = FirstRaxProduction::marine;
			break;
		}
	}
	return;
}

void ScoutTReturnToBase::EnterState()
{
	agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, agent->locations->start_location);
}

void ScoutTReturnToBase::ExitState()
{
	return;
}

State* ScoutTReturnToBase::TestTransitions()
{
	if (Distance2D(state_machine->scout->pos, agent->locations->start_location) <= 20)
		state_machine->CloseStateMachine();
	return NULL;
}

std::string ScoutTReturnToBase::toString()
{
	return "return to base";
}

#pragma endregion

void ScoutTerranStateMachine::CloseStateMachine()
{
	delete current_state;
	current_state = NULL;
	current_state = NULL;
	agent->mediator.worker_manager.PlaceWorker(scout);
	agent->mediator.RemoveStateMachine(this);
}

void ScoutTerranStateMachine::CheckScoutingInfo()
{
	for (const auto& unit : agent->Observation()->GetUnits())
	{
		if (unit->alliance == Unit::Alliance::Enemy && unit->display_type == Unit::DisplayType::Visible)
		{
			if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_BARRACKS && agent->scout_info_terran.barrackes_timing == 0)
			{
				//std::cout << "barracks built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
				agent->scout_info_terran.barrackes_timing = Utility::GetTimeBuilt(unit, agent->Observation());
			}
			else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_FACTORY && agent->scout_info_terran.factory_timing == 0)
			{
				//std::cout << "factory built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
				agent->scout_info_terran.factory_timing = Utility::GetTimeBuilt(unit, agent->Observation());
			}
			else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_COMMANDCENTER && agent->scout_info_terran.natural_timing == 0 && unit->build_progress < 1)
			{
				//std::cout << "natural built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
				agent->scout_info_terran.natural_timing = Utility::GetTimeBuilt(unit, agent->Observation());
			}
			else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_REFINERY)
			{
				if (agent->scout_info_terran.gas_timing == 0)
				{
					//std::cout << "gas built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
					agent->scout_info_terran.gas_timing = Utility::GetTimeBuilt(unit, agent->Observation());
					agent->scout_info_terran.gas_pos = unit->pos;
				}
				else if (agent->scout_info_terran.second_gas_timing == 0 && unit->pos != agent->scout_info_terran.gas_pos)
				{
					//std::cout << "second gas built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
					agent->scout_info_terran.second_gas_timing = Utility::GetTimeBuilt(unit, agent->Observation());
				}
			}
		}
	}
}

}