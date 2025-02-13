#pragma once
#include <string>

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_unit_filters.h"


#include "utility.h"
#include "locations.h"
#include "path_manager.h"

namespace sc2 {

class TheBigBot;

class State
{
public:
    TheBigBot* agent;
    State() {};
    virtual std::string toString();
    virtual void TickState();
    virtual void EnterState();
    virtual void ExitState();
    virtual State* TestTransitions();
};


class StateMachine
{
public:
    TheBigBot* agent;
    State* current_state;
    std::string name;
    StateMachine() {}
    StateMachine(TheBigBot* agent, State* starting_state, std::string name) {
        this->agent = agent;
        current_state = starting_state;
        this->name = name;
        current_state->EnterState();
    }
    virtual void RunStateMachine()
    {
        State* new_state = current_state->TestTransitions();
        if (new_state != NULL)
        {
            current_state->ExitState();
            delete current_state;
            current_state = new_state;
            current_state->EnterState();
        }
		if (current_state != NULL)
	        current_state->TickState();
    }

    virtual void StartStateMachine()
    {
        return;
    }

    State* GetState()
    {
        return current_state;
    }

    std::string toString()
    {
        return name + " - " + current_state->toString();
    }

    void CloseStateMachine();

};




}

