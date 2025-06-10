#pragma once

#include "finite_state_machine.h"


namespace sc2
{

class Mediator;
class ChargelotAllInStateMachine;
class ArmyGroup;


#pragma region Chargelot Allin


class ChargeAllInMovingToWarpinSpot : public State {
public:
    ChargelotAllInStateMachine* state_machine;
    ChargeAllInMovingToWarpinSpot(Mediator* mediator, ChargelotAllInStateMachine* state_machine)
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

class ChargeAllInWarpingIn : public State {
public:
    ChargelotAllInStateMachine* state_machine;
    ChargeAllInWarpingIn(Mediator* mediator, ChargelotAllInStateMachine* state_machine)
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

class ChargelotAllInStateMachine : public StateMachine
{
public:
    Units zealots;
    const Unit* prism = nullptr;
    float last_warp_in_time;
    std::vector<Point2D> prism_spots;
    uint16_t prism_spots_index;
    Point2D next_warp_in_location = Point2D(0, 0);
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