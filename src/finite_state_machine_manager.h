#pragma once

#include "finite_state_machine.h"

namespace sc2
{

class Mediator;

class FiniteStateMachineManager
{
private:
	Mediator* mediator;
	std::vector<StateMachine*> active_state_machines;
	std::vector<StateMachine*> state_machines_to_delete;

public:
	FiniteStateMachineManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}
	void DisplayActiveStateMachines() const;

	void RunStateMachines();
	const std::vector<StateMachine*>& GetActiveStateMachines() const;
	void AddStateMachine(StateMachine*);
	void RemoveStateMachine(StateMachine*);
	void MarkStateMachineForDeletion(StateMachine*);
};

}