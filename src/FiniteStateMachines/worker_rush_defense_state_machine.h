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
	class WorkerRushDefenseStateMachine* state_machine;
	WorkerRushDefenseGroupUp(TheBigBot* agent, WorkerRushDefenseStateMachine* state_machine)
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

class WorkerRushDefenseInitialMove : public State
{
public:
	class WorkerRushDefenseStateMachine* state_machine;
	WorkerRushDefenseInitialMove(TheBigBot* agent, WorkerRushDefenseStateMachine* state_machine)
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

class WorkerRushDefenseDefend : public State
{
public:
	class WorkerRushDefenseStateMachine* state_machine;
	WorkerRushDefenseDefend(TheBigBot* agent, WorkerRushDefenseStateMachine* state_machine)
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


class WorkerRushDefenseStateMachine : public StateMachine
{
public:
	Units workers;
	const Unit* grouping_mineral_patch;
	const Unit* attacking_mineral_patch;
	WorkerRushDefenseStateMachine(TheBigBot* agent, std::string name, Units workers,
		const Unit* grouping_mineral_patch, const Unit* attacking_mineral_patch)
	{
		this->agent = agent;
		this->name = name;
		this->workers = workers;
		this->attacking_mineral_patch = attacking_mineral_patch;
		this->grouping_mineral_patch = grouping_mineral_patch;

		current_state = new WorkerRushDefenseGroupUp(agent, this);
		current_state->EnterState();
	}

};



}