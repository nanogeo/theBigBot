#pragma once

#include "finite_state_machine.h"

namespace sc2
{

class Mediator;
class ScoutProtossStateMachine;
class TheBigBot;
class ArmyGroup;


#pragma region Scout T


class ScoutPInitialMove : public State
{
public:
    ScoutProtossStateMachine* state_machine;
    ScoutPInitialMove(TheBigBot* agent, ScoutProtossStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutPScoutPylon : public State
{
public:
    ScoutProtossStateMachine* state_machine;
    ScoutPScoutPylon(TheBigBot* agent, ScoutProtossStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutPScoutMain : public State
{
public:
    ScoutProtossStateMachine* state_machine;
    bool scouted_pylon;
    ScoutPScoutMain(TheBigBot* agent, ScoutProtossStateMachine* state_machine, bool scouted_pylon)
    {
        this->agent = agent;
        this->state_machine = state_machine;
        this->scouted_pylon = scouted_pylon;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutPScoutNatural : public State
{
public:
    ScoutProtossStateMachine* state_machine;
    ScoutPScoutNatural(TheBigBot* agent, ScoutProtossStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutPReturnToBase : public State
{
public:
    ScoutProtossStateMachine* state_machine;
    ScoutPReturnToBase(TheBigBot* agent, ScoutProtossStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

#pragma endregion


class ScoutProtossStateMachine : public StateMachine
{
public:
    const Unit* scout;
    Point2D enemy_main;
    int index = 0;
    std::vector<Point2D> main_scout_path;
    std::vector<Point2D> natural_scout_path;
    Point2D enemy_natural_pos;
    Point2D current_target;
    ScoutProtossStateMachine(TheBigBot* agent, std::string name, const Unit* scout, Point2D enemy_main, 
        std::vector<Point2D> main_scout_path, std::vector<Point2D> natural_scout_path, Point2D enemy_natural_pos) : StateMachine(agent, name)
    {
        current_state = new ScoutPInitialMove(agent, this);
        this->scout = scout;
        this->enemy_main = enemy_main;
        this->main_scout_path = main_scout_path;
        this->natural_scout_path = natural_scout_path;
        this->enemy_natural_pos = enemy_natural_pos;

        current_state->EnterState();
    }

    ~ScoutProtossStateMachine();

    void RunStateMachine()
    {
        StateMachine::RunStateMachine();
    }

};

}