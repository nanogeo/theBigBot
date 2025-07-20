#pragma once

#include "finite_state_machine.h"


namespace sc2
{

class Mediator;
class ScoutZergStateMachine;
class ArmyGroup;

#pragma region Scout Z


class ScoutZInitialMove : public State
{
private:
    ScoutZergStateMachine* state_machine;
public:
    ScoutZInitialMove(Mediator* mediator, ScoutZergStateMachine* state_machine)
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

class ScoutZScoutMain : public State
{
private:
    ScoutZergStateMachine* state_machine;
public:
    ScoutZScoutMain(Mediator* mediator, ScoutZergStateMachine* state_machine)
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

class ScoutZScoutNatural : public State
{
private:
    ScoutZergStateMachine* state_machine;
public:
    ScoutZScoutNatural(Mediator* mediator, ScoutZergStateMachine* state_machine)
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

class ScoutZLookFor3rd : public State
{
private:
    ScoutZergStateMachine* state_machine;
public:
    ScoutZLookFor3rd(Mediator* mediator, ScoutZergStateMachine* state_machine)
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


class ScoutZergStateMachine : public StateMachine
{
    friend ScoutZInitialMove;
    friend ScoutZScoutMain;
    friend ScoutZScoutNatural;
    friend ScoutZLookFor3rd;
private:
    const Unit* scout;
    Point2D enemy_main;
    int index;
    std::vector<Point2D> main_scout_path;
    std::vector<Point2D> natural_scout_path;
    std::vector<Point2D> possible_3rds;
    Point2D enemy_natural_pos;
    Point2D current_target;
public:
    ScoutZergStateMachine(Mediator* mediator, std::string name, const Unit* scout, Point2D enemy_main, 
        std::vector<Point2D> main_scout_path, Point2D enemy_natural_pos, std::vector<Point2D> possible_3rds);

    ~ScoutZergStateMachine();
    const Unit* GetScout();
};


}