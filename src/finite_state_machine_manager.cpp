#pragma once
#include "finite_state_machine_manager.h"

namespace sc2 {

 void FiniteStateMachineManager::RunStateMachines()
{
    for (auto &state_machine : active_state_machines)
    {
        state_machine->RunStateMachine();
    }
}

 void FiniteStateMachineManager::AddStateMachine(StateMachine* state_machine)
 {
     active_state_machines.push_back(state_machine);
 }

void FiniteStateMachineManager::RemoveStateMachine(StateMachine* state_machine)
{
	active_state_machines.erase(std::remove(active_state_machines.begin(), active_state_machines.end(), state_machine), active_state_machines.end());
}
	
}