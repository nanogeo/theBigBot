#pragma once
#include "finite_state_machine.h"
#include "locations.h"
#include "utility.h"
#include "definitions.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <math.h>


#include "sc2api/sc2_api.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"

#include "theBigBot.h"

namespace sc2 {

void State::TickState()
{
	//std::cout << "TickState called on base State class";
}

void State::EnterState()
{
	//std::cout << "EnterState called on base State class";
}

void State::ExitState()
{
	//std::cout << "ExitState called on base State class";
}

State* State::TestTransitions()
{
	//std::cout << "TestTransitions called on base State class";
	return NULL;
}

std::string State::toString()
{
	return "State";
}

void StateMachine::CloseStateMachine()
{
	delete current_state;
	current_state = NULL;
	agent->mediator.RemoveStateMachine(this);
}



}