#pragma once

#include "finite_state_machine.h"

namespace sc2
{

class Mediator;
class ScoutProtossStateMachine;
class ArmyGroup;


#pragma region Scout T


class ScoutPInitialMove : public State
{
private:
    ScoutProtossStateMachine* state_machine;
public:
    ScoutPInitialMove(Mediator* mediator, ScoutProtossStateMachine* state_machine)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
    }
    virtual std::string toString() const override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutPScoutPylon : public State
{
private:
    ScoutProtossStateMachine* state_machine;
public:
    ScoutPScoutPylon(Mediator* mediator, ScoutProtossStateMachine* state_machine)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
    }
    virtual std::string toString() const override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutPScoutMain : public State
{
private:
    ScoutProtossStateMachine* state_machine;
    bool scouted_pylon;
public:
    ScoutPScoutMain(Mediator* mediator, ScoutProtossStateMachine* state_machine, bool scouted_pylon)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
        this->scouted_pylon = scouted_pylon;
    }
    virtual std::string toString() const override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutPScoutNatural : public State
{
private:
    ScoutProtossStateMachine* state_machine;
public:
    ScoutPScoutNatural(Mediator* mediator, ScoutProtossStateMachine* state_machine)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
    }
    virtual std::string toString() const override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutPReturnToBase : public State
{
private:
    ScoutProtossStateMachine* state_machine;
public:
    ScoutPReturnToBase(Mediator* mediator, ScoutProtossStateMachine* state_machine)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
    }
    virtual std::string toString() const override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

#pragma endregion


class ScoutProtossStateMachine : public StateMachine
{
    friend ScoutPInitialMove;
    friend ScoutPScoutPylon;
    friend ScoutPScoutMain;
    friend ScoutPScoutNatural;
    friend ScoutPReturnToBase;
private:
    const Unit* scout;
    Point2D enemy_main;
    int index = 0;
    std::vector<Point2D> main_scout_path;
    std::vector<Point2D> natural_scout_path;
    Point2D enemy_natural_pos;
    Point2D current_target;
public:
    ScoutProtossStateMachine(Mediator* mediator, std::string name, const Unit* scout, Point2D enemy_main, 
        std::vector<Point2D> main_scout_path, std::vector<Point2D> natural_scout_path, Point2D enemy_natural_pos) : StateMachine(mediator, name)
    {
        current_state = new ScoutPInitialMove(mediator, this);
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

    const Unit* GetScout();

};

}