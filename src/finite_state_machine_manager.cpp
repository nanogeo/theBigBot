#pragma once
#include "finite_state_machine_manager.h"

namespace sc2 {

 void FiniteStateMachineManager::RunStateMachines()
{
    for (int i = 0; i < active_state_machines.size(); i++)
    {
        if (std::find(state_machines_to_delete.begin(), state_machines_to_delete.end(), active_state_machines[i]) != state_machines_to_delete.end())
        {
            state_machines_to_delete.erase(std::remove(state_machines_to_delete.begin(), state_machines_to_delete.end(), active_state_machines[i]), state_machines_to_delete.end());
            RemoveStateMachine(active_state_machines[i]);
            i--;
        }
        else
        {
            active_state_machines[i]->RunStateMachine();
        }
    }
}

 void FiniteStateMachineManager::AddStateMachine(StateMachine* state_machine)
 {
     active_state_machines.push_back(state_machine);
 }

void FiniteStateMachineManager::RemoveStateMachine(StateMachine* state_machine)
{
	active_state_machines.erase(std::remove(active_state_machines.begin(), active_state_machines.end(), state_machine));
    delete state_machine;
}

void FiniteStateMachineManager::MarkStateMachineForDeletion(StateMachine* state_machine)
{
    state_machines_to_delete.push_back(state_machine);
}
	
}