#pragma once

#include "finite_state_machine.h"

namespace sc2
{

class Mediator;
class ScoutTerranStateMachine;
class ArmyGroup;


#pragma region Scout T


class ScoutTInitialMove : public State
{
public:
    ScoutTerranStateMachine* state_machine;
    ScoutTInitialMove(Mediator* mediator, ScoutTerranStateMachine* state_machine)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutTScoutMain : public State
{
public:
    ScoutTerranStateMachine* state_machine;
    ScoutTScoutMain(Mediator* mediator, ScoutTerranStateMachine* state_machine)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutTScoutNatural : public State
{
public:
    ScoutTerranStateMachine* state_machine;
    ScoutTScoutNatural(Mediator* mediator, ScoutTerranStateMachine* state_machine)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutTScoutRax : public State
{
public:
    const Unit* rax = nullptr;
    ScoutTerranStateMachine* state_machine;
    ScoutTScoutRax(Mediator* mediator, ScoutTerranStateMachine* state_machine)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutTReturnToBase : public State
{
public:
    ScoutTerranStateMachine* state_machine;
    ScoutTReturnToBase(Mediator* mediator, ScoutTerranStateMachine* state_machine)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

#pragma endregion


class ScoutTerranStateMachine : public StateMachine
{
public:
    const Unit* scout;
    Point2D enemy_main;
    int index = 0;
    std::vector<Point2D> main_scout_path;
    std::vector<Point2D> natural_scout_path;
    Point2D enemy_natural_pos;
    Point2D current_target;
    ScoutTerranStateMachine(Mediator* mediator, std::string name, const Unit* scout, Point2D enemy_main, 
        std::vector<Point2D> main_scout_path, std::vector<Point2D> natural_scout_path, Point2D enemy_natural_pos) : StateMachine(mediator, name)
    {
        current_state = new ScoutTInitialMove(mediator, this);
        this->scout = scout;
        this->enemy_main = enemy_main;
        this->main_scout_path = main_scout_path;
        this->natural_scout_path = natural_scout_path;
        this->enemy_natural_pos = enemy_natural_pos;

        current_state->EnterState();
    }

    ~ScoutTerranStateMachine();

};

}