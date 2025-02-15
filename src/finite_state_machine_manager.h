#pragma once

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_unit_filters.h"

#include "finite_state_machine.h"

namespace sc2
{

class Mediator;

class FiniteStateMachineManager
{
public:
	Mediator* mediator;
	std::vector<StateMachine*> active_state_machines;
	std::vector<StateMachine*> state_machines_to_delete;

	FiniteStateMachineManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void RunStateMachines();
	void AddStateMachine(StateMachine*);
	void RemoveStateMachine(StateMachine*);
	void MarkStateMachineForDeletion(StateMachine*);
};

}