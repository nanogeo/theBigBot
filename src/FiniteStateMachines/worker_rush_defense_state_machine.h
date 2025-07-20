#pragma once

#include "finite_state_machine.h"



namespace sc2
{

class Mediator;

class WorkerRushDefenseStateMachine;
class TheBigBot;

class WorkerRushDefenseGroupUp : public State
{
private:
	Units new_workers;
	Point2D group_pos;
	float enter_time = 0;;
	WorkerRushDefenseStateMachine* state_machine;
public:
	WorkerRushDefenseGroupUp(Mediator* mediator, WorkerRushDefenseStateMachine* state_machine)
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

class WorkerRushDefenseInitialMove : public State
{
private:
	WorkerRushDefenseStateMachine* state_machine;
public:
	WorkerRushDefenseInitialMove(Mediator* mediator, WorkerRushDefenseStateMachine* state_machine)
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

class WorkerRushDefenseDefend : public State
{
private:
	WorkerRushDefenseStateMachine* state_machine;
public:
	WorkerRushDefenseDefend(Mediator* mediator, WorkerRushDefenseStateMachine* state_machine)
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


class WorkerRushDefenseStateMachine : public StateMachine
{
	friend WorkerRushDefenseGroupUp;
	friend WorkerRushDefenseInitialMove;
	friend WorkerRushDefenseDefend;
private:
	Units workers;
	const Unit* grouping_mineral_patch;
	const Unit* attacking_mineral_patch;
public:
	WorkerRushDefenseStateMachine(Mediator*, std::string);
};



}