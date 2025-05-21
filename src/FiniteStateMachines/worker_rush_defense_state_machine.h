#pragma once

#include "finite_state_machine.h"



namespace sc2
{

class Mediator;

class WorkerRushDefenseStateMachine;
class TheBigBot;

class WorkerRushDefenseGroupUp : public State
{
public:
	Units new_workers;
	Point2D group_pos;
	float enter_time = 0;;
	class WorkerRushDefenseStateMachine* state_machine;
	WorkerRushDefenseGroupUp(Mediator* mediator, WorkerRushDefenseStateMachine* state_machine)
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

class WorkerRushDefenseInitialMove : public State
{
public:
	class WorkerRushDefenseStateMachine* state_machine;
	WorkerRushDefenseInitialMove(Mediator* mediator, WorkerRushDefenseStateMachine* state_machine)
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

class WorkerRushDefenseDefend : public State
{
public:
	class WorkerRushDefenseStateMachine* state_machine;
	WorkerRushDefenseDefend(Mediator* mediator, WorkerRushDefenseStateMachine* state_machine)
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


class WorkerRushDefenseStateMachine : public StateMachine
{
public:
	Units workers;
	const Unit* grouping_mineral_patch;
	const Unit* attacking_mineral_patch;
	WorkerRushDefenseStateMachine(Mediator*, std::string);
};



}