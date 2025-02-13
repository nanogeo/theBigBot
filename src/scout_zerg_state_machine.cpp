#pragma once

#include "scout_zerg_state_machine.h"
#include "theBigBot.h"


namespace sc2 {


#pragma region ScoutZInitialMove

void ScoutZInitialMove::TickState()
{
	if (state_machine->scout->orders.size() == 0 || state_machine->scout->orders[0].target_pos != state_machine->current_target)
		agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target);
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
		return new ScoutZScoutMain(agent, state_machine);
	return NULL;
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
	agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target);
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
		return new ScoutZScoutNatural(agent, state_machine);
	return NULL;
}

std::string ScoutZScoutMain::toString()
{
	return "scout main";
}

#pragma endregion

#pragma region ScoutZScoutNatural

void ScoutZScoutNatural::TickState()
{
	if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 2)
	{
		state_machine->index++;
		if (state_machine->index < state_machine->natural_scout_path.size())
			state_machine->current_target = state_machine->natural_scout_path[state_machine->index];
	}
	agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target);
}

void ScoutZScoutNatural::EnterState()
{
	state_machine->index = 0;
	state_machine->current_target = state_machine->natural_scout_path[0];
}

void ScoutZScoutNatural::ExitState()
{
	return;
}

State* ScoutZScoutNatural::TestTransitions()
{
	if (state_machine->index >= state_machine->natural_scout_path.size())
		return new ScoutZLookFor3rd(agent, state_machine);
	return NULL;
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
	agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::GENERAL_MOVE, state_machine->current_target);
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
	if (state_machine->index >= state_machine->possible_3rds.size())
		return new ScoutZScoutMain(agent, state_machine);
	return NULL;
}

std::string ScoutZLookFor3rd::toString()
{
	return "look for 3rd";
}

#pragma endregion

ScoutZergStateMachine::ScoutZergStateMachine(TheBigBot* agent, std::string name, const Unit* scout, Point2D enemy_main, std::vector<Point2D> main_scout_path, std::vector<Point2D> natural_scout_path, Point2D enemy_natural_pos, std::vector<Point2D> possible_3rds)
{
    this->agent = agent;
    this->name = name;
    current_state = new ScoutZInitialMove(agent, this);
    this->scout = scout;
    this->enemy_main = enemy_main;
    this->main_scout_path = main_scout_path;
    this->natural_scout_path = natural_scout_path;
    this->enemy_natural_pos = enemy_natural_pos;
    this->possible_3rds = possible_3rds;

    current_state->EnterState();
}


void ScoutZergStateMachine::CheckScoutingInfo()
{
    for (const auto& unit : agent->Observation()->GetUnits())
    {
        if (unit->alliance == Unit::Alliance::Enemy && unit->display_type == Unit::DisplayType::Visible)
        {
            if (unit->unit_type.ToType() == UNIT_TYPEID::ZERG_SPAWNINGPOOL && agent->scout_info_zerg.pool_timing == 0)
            {
                //std::cout << "pool built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                agent->scout_info_zerg.pool_timing = Utility::GetTimeBuilt(unit, agent->Observation());
            }
            else if (unit->unit_type.ToType() == UNIT_TYPEID::ZERG_ROACHWARREN && agent->scout_info_zerg.roach_warren_timing == 0)
            {
                //std::cout << "roach warren built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                agent->scout_info_zerg.roach_warren_timing = Utility::GetTimeBuilt(unit, agent->Observation());
            }
            else if (unit->unit_type.ToType() == UNIT_TYPEID::ZERG_EXTRACTOR && agent->scout_info_zerg.gas_timing == 0)
            {
                //std::cout << "gas built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                agent->scout_info_zerg.gas_timing = Utility::GetTimeBuilt(unit, agent->Observation());
            }
            else if (unit->unit_type.ToType() == UNIT_TYPEID::ZERG_HATCHERY && agent->scout_info_zerg.natural_timing == 0)
            {
                if (Distance2D(unit->pos, enemy_natural_pos) < 2)
                {
                    //std::cout << "natural built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                    agent->scout_info_zerg.natural_timing = Utility::GetTimeBuilt(unit, agent->Observation());
                }
            }
            else if (unit->unit_type.ToType() == UNIT_TYPEID::ZERG_HATCHERY && agent->scout_info_zerg.third_timing == 0)
            {
                for (const auto& pos : possible_3rds)
                {
                    if (Distance2D(unit->pos, pos) < 2)
                    {
                        //std::cout << "third built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                        agent->scout_info_zerg.third_timing = Utility::GetTimeBuilt(unit, agent->Observation());
                    }
                }
            }
        }
    }
}

}