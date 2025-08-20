#pragma once

#include "worker_rush_defense_state_machine.h"
#include "theBigBot.h"

namespace sc2 {


#pragma region WorkerRushDefenseGroupUp

void WorkerRushDefenseGroupUp::TickState()
{
	int worker_diff = 1 + (int)Utility::GetUnitsWithin(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ PROBE, SCV, DRONE })),
		mediator->GetStartLocation(), 20).size() - (int)(state_machine->workers.size() + new_workers.size());

	for (int i = 0; i < worker_diff; i++)
	{
		const Unit* worker = mediator->GetBuilder(mediator->GetStartLocation());
		if (worker == nullptr)
			break;
		if (mediator->RemoveWorker(worker) == RemoveWorkerResult::NOT_FOUND)
		{
			std::cerr << "Worker not found when trying to remove in WorkerRushDefenseGroupUp::TickState" << std::endl;
			mediator->LogMinorError();
		}
		new_workers.push_back(worker);

		mediator->SetUnitCommand(worker, A_MOVE, group_pos, CommandPriority::low);

	}

	for (auto itr = new_workers.begin(); itr != new_workers.end();)
	{
		if (Distance2D((*itr)->pos, group_pos) < 1)
		{
			state_machine->workers.push_back(*itr);
			itr = new_workers.erase(itr);
		}
		else
		{
			mediator->SetUnitCommand(*itr, A_MOVE, group_pos, CommandPriority::low);
			itr++;
		}
	}

	for (const auto &worker : state_machine->workers)
	{
		mediator->SetUnitCommand(worker, A_SMART, state_machine->grouping_mineral_patch, CommandPriority::low);
	}
}

void WorkerRushDefenseGroupUp::EnterState()
{
	group_pos = Utility::PointBetween(state_machine->grouping_mineral_patch->pos,
		Utility::ClosestTo(mediator->GetUnits(IsUnit(NEXUS)), state_machine->grouping_mineral_patch->pos)->pos, 2.5);

	mediator->SetUnitsCommand(state_machine->workers, A_MOVE, group_pos, CommandPriority::low);

	enter_time = mediator->GetCurrentTime();
}

void WorkerRushDefenseGroupUp::ExitState()
{
	return;
}

State* WorkerRushDefenseGroupUp::TestTransitions()
{
	if (state_machine->workers.size() < 2 || new_workers.size() > 0)
		return nullptr;
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
	if (max_dist <= .3 || mediator->GetCurrentTime() > enter_time + 4)
		return new WorkerRushDefenseInitialMove(mediator, state_machine);
	return nullptr;
}

std::string WorkerRushDefenseGroupUp::toString() const
{
	return "group up";
}

#pragma endregion


#pragma region WorkerRushDefenseInitialMove

void WorkerRushDefenseInitialMove::TickState()
{
	for (const auto& worker : state_machine->workers)
	{
		mediator->SetUnitCommand(worker, A_SMART, state_machine->attacking_mineral_patch, CommandPriority::low);
	}
}

void WorkerRushDefenseInitialMove::EnterState()
{
	for (const auto& worker : state_machine->workers)
	{
		mediator->SetUnitCommand(worker, A_SMART, state_machine->attacking_mineral_patch, CommandPriority::low);
	}
}

void WorkerRushDefenseInitialMove::ExitState()
{
	return; 
}

State* WorkerRushDefenseInitialMove::TestTransitions()
{
	for (const auto& worker : state_machine->workers)
	{
		if (Utility::GetUnitsInRange(mediator->GetUnits(Unit::Alliance::Enemy), worker, 0).size() > 0)
			return new WorkerRushDefenseDefend(mediator, state_machine);
	}
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({SCV, DRONE, PROBE})), mediator->GetStartLocation()) < 
		Distance2D(Utility::MedianCenter(state_machine->workers), mediator->GetStartLocation()))
		return new WorkerRushDefenseDefend(mediator, state_machine);

	return nullptr;
}

std::string WorkerRushDefenseInitialMove::toString() const
{
	return "initial move";
}

#pragma endregion


#pragma region WorkerRushDefenseDefend

void WorkerRushDefenseDefend::TickState()
{
	int worker_diff = 1 + (int)Utility::GetUnitsWithin(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ PROBE, SCV, DRONE })),
		mediator->GetStartLocation(), 20).size() - (int)state_machine->workers.size();

	if (worker_diff > 0)
	{
		for (int i = 0; i < worker_diff; i++)
		{
			const Unit* worker = mediator->GetBuilder(mediator->GetStartLocation());
			if (worker == nullptr)
				break;
			if (mediator->RemoveWorker(worker) == RemoveWorkerResult::NOT_FOUND)
			{
				std::cerr << "Worker not found when trying to remove in WorkerRushDefenseDefend::TickState" << std::endl;
				mediator->LogMinorError();
			}
			state_machine->workers.push_back(worker);
		}
	}
	else if (worker_diff < 0)
	{
		for (int i = 0; i < -1 * worker_diff; i++)
		{
			const Unit* worker = Utility::GetMostDamagedUnit(state_machine->workers);
			if (worker)
			{
				mediator->PlaceWorker(worker);
				state_machine->workers.erase(std::remove(state_machine->workers.begin(), state_machine->workers.end(), worker), state_machine->workers.end());
			}

		}
	}


	for (int i = 0; i < state_machine->workers.size(); i++)
	{
		if (state_machine->workers[i]->is_alive == false)
		{
			state_machine->workers.erase(state_machine->workers.begin() + i);
			i--;
		}
		else if (state_machine->workers[i]->health + state_machine->workers[i]->shield <= 5)
		{
			mediator->PlaceWorker(state_machine->workers[i]);
			state_machine->workers.erase(state_machine->workers.begin() + i);
			i--;
		}
		else if (state_machine->workers[i]->weapon_cooldown == 0)
		{
			const Unit* closest_enemy = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), state_machine->workers[i]->pos);
			if (closest_enemy != nullptr)
				mediator->SetUnitCommand(state_machine->workers[i], A_ATTACK, mediator->GetEnemyNaturalLocation(), CommandPriority::low);
		}
		else
		{
			mediator->SetUnitCommand(state_machine->workers[i], A_SMART, state_machine->grouping_mineral_patch, CommandPriority::low);
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
	if (state_machine->workers.size() <= 1)
		mediator->MarkStateMachineForDeletion(state_machine);
	return nullptr;
}

std::string WorkerRushDefenseDefend::toString() const
{
	return "defend";
}

#pragma endregion


WorkerRushDefenseStateMachine::WorkerRushDefenseStateMachine(Mediator* mediator, std::string name) : StateMachine(mediator, name)
{
	this->attacking_mineral_patch = mediator->GetWorkerRushDefenseAttackingMineralPatch();
	this->grouping_mineral_patch = mediator->GetWorkerRushDefenseGroupingMineralPatch();

	current_state = new WorkerRushDefenseGroupUp(mediator, this);
	current_state->EnterState();
}

}