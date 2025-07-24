#pragma once
#include <string>

#include "utility.h"
#include "locations.h"
#include "piecewise_path.h"

namespace sc2 {

class Mediator;
class ArmyGroup;

class State
{
protected:
    Mediator* mediator = nullptr;
public:
    ~State()
    {
        ExitState();
    }
    virtual std::string toString() const;
    virtual void TickState();
    virtual void EnterState();
    virtual void ExitState();
    virtual State* TestTransitions();
};


class StateMachine
{
protected:
    Mediator* mediator = nullptr;
    State* current_state = nullptr;
    ArmyGroup* attached_army_group = nullptr;
public:
    std::string name;
    StateMachine(Mediator* mediator, std::string name)
    {
        this->mediator = mediator;
        this->name = name;
    }
    StateMachine(Mediator* mediator, State* starting_state, std::string name)
    {
        this->mediator = mediator;
        current_state = starting_state;
        this->name = name;
        current_state->EnterState();
    }
    virtual ~StateMachine()
    {
        delete current_state;
    }
    virtual void RunStateMachine()
    {
        State* new_state = current_state->TestTransitions();
        if (new_state != nullptr)
        {
            current_state->ExitState();
            delete current_state;
            current_state = new_state;
            std::cerr << "Change state " << new_state->toString() << std::endl;
            current_state->EnterState();
        }
		if (current_state != nullptr)
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

    std::string toString() const
    {
        return name + " - " + current_state->toString();
    }

    void SetAttachedArmyGroup(ArmyGroup*);
    ArmyGroup* GetAttachedArmyGroup() const;

    virtual bool AddUnit(const Unit*) { return false; }
    virtual void RemoveUnit(const Unit*) {};
};




}

