#pragma once

#include "finite_state_machine.h"


namespace sc2
{

class Mediator;
class ChargelotAllInStateMachine;
class ArmyGroup;


class ChargeAllInMovingToWarpinSpot : public State 
{
private:
    ChargelotAllInStateMachine* state_machine;
public:
    ChargeAllInMovingToWarpinSpot(Mediator* mediator, ChargelotAllInStateMachine* state_machine)
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

class ChargeAllInWarpingIn : public State 
{
private:
    ChargelotAllInStateMachine* state_machine;
    float time_arrived;
    bool done_warp_in = false;
public:
    ChargeAllInWarpingIn(Mediator* mediator, ChargelotAllInStateMachine* state_machine, float time_arrived)
    {
        this->mediator = mediator;
        this->state_machine = state_machine;
        this->time_arrived = time_arrived;
    }
    virtual std::string toString() const override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ChargelotAllInStateMachine : public StateMachine
{
    friend ChargeAllInMovingToWarpinSpot;
    friend ChargeAllInWarpingIn;
private:
    Units zealots;
    const Unit* prism = nullptr;
    float last_warp_in_time;
    std::vector<Point2D> prism_spots;
    uint16_t prism_spots_index;
    Point2D next_warp_in_location = Point2D(0, 0);
    bool first_warp_in_done = false;
public:
    ChargelotAllInStateMachine(Mediator* mediator, std::string name, std::vector<Point2D> prism_locations,
        float last_warp_in_time) : StateMachine(mediator, name)
    {
        current_state = new ChargeAllInMovingToWarpinSpot(mediator, this);
        prism_spots = prism_locations;
        this->last_warp_in_time = last_warp_in_time;
        prism_spots_index = 0;
        current_state->EnterState();
    }
    void RunStateMachine() override;

    bool AddUnit(const Unit* unit) override;
    void RemoveUnit(const Unit* unit) override;
};


}