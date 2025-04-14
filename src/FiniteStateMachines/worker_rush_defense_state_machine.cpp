#pragma once

#include "worker_rush_defense_state_machine.h"
#include "theBigBot.h"

namespace sc2 {


#pragma region WorkerRushDefenseGroupUp

void WorkerRushDefenseGroupUp::TickState()
{
	for (const auto& worker : state_machine->workers)
	{
		if (worker->orders.size() < 2)
			agent->mediator.SetUnitCommand(worker, ABILITY_ID::SMART, state_machine->grouping_mineral_patch, 0);
	}
}

void WorkerRushDefenseGroupUp::EnterState()
{
	Point2D pos = Utility::PointBetween(state_machine->grouping_mineral_patch->pos,
		Utility::ClosestTo(agent->mediator.GetUnits(IsUnit(NEXUS)), state_machine->grouping_mineral_patch->pos)->pos, 1);
	agent->mediator.SetUnitsCommand(state_machine->workers, ABILITY_ID::GENERAL_MOVE, pos, 0);
	agent->mediator.SetUnitsCommand(state_machine->workers, ABILITY_ID::SMART, state_machine->grouping_mineral_patch, 0, true);
}

void WorkerRushDefenseGroupUp::ExitState()
{
	return;
}

State* WorkerRushDefenseGroupUp::TestTransitions()
{
	float max_dist = 0;
	for (int i = 0; i < state_machine->workers.size(); i++)
	{
		for (int j = i + 1; j < state_machine->workers.size(); j++)
		{
			float dist = Distance2D(state_machine->workers[i]->pos, state_machine->workers[j]->pos);
			if (dist > max_dist)
				max_dist = dist;
		}
	}
	if (max_dist <= .25)
		return new WorkerRushDefenseInitialMove(agent, state_machine);
	return nullptr;
}

std::string WorkerRushDefenseGroupUp::toString()
{
	return "group up";
}

#pragma endregion


#pragma region WorkerRushDefenseInitialMove

void WorkerRushDefenseInitialMove::TickState()
{
	
}

void WorkerRushDefenseInitialMove::EnterState()
{
	agent->mediator.SetUnitsCommand(state_machine->workers, ABILITY_ID::SMART, state_machine->attacking_mineral_patch, 0);
}

void WorkerRushDefenseInitialMove::ExitState()
{
	return;
}

State* WorkerRushDefenseInitialMove::TestTransitions()
{
	
	return nullptr;
}

std::string WorkerRushDefenseInitialMove::toString()
{
	return "initial move";
}

#pragma endregion


#pragma region WorkerRushDefenseDefend

void WorkerRushDefenseDefend::TickState()
{
	for (int i = 0; i < state_machine->workers.size(); i++)
	{
		if (state_machine->workers[i]->is_alive == false)
		{
			state_machine->workers.erase(state_machine->workers.begin() + i);
			i--;
		}
		else if (state_machine->workers[i]->health + state_machine->workers[i]->shield <= 5)
		{
			agent->mediator.PlaceWorker(state_machine->workers[i]);
			state_machine->workers.erase(state_machine->workers.begin() + i);
			i--;
		}
		else if (state_machine->workers[i]->weapon_cooldown == 0)
		{
			const Unit* closest_enemy = Utility::ClosestTo(agent->mediator.GetUnits(Unit::Alliance::Enemy), state_machine->workers[i]->pos);
			if (closest_enemy != nullptr)
				agent->mediator.SetUnitCommand(state_machine->workers[i], ABILITY_ID::ATTACK, closest_enemy, 0);
		}
		else
		{
			agent->mediator.SetUnitCommand(state_machine->workers[i], ABILITY_ID::SMART, state_machine->grouping_mineral_patch, 0);
		}
	}
}

void WorkerRushDefenseDefend::EnterState()
{
	
}

void WorkerRushDefenseDefend::ExitState()
{
	return;
}

State* WorkerRushDefenseDefend::TestTransitions()
{
	// TODO transition out when enemy leaves or is vanquished
	return nullptr;
}

std::string WorkerRushDefenseDefend::toString()
{
	return "defend";
}

#pragma endregion

}