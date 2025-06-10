#pragma once

#include "finite_state_machine.h"



namespace sc2
{

class Mediator;

class PvPMainRampWallOffStateMachine;
class TheBigBot;

class PvPMainRampWallOffWaitForEnemies : public State
{
public:
	Units new_workers;
	Point2D group_pos;
	float enter_time = 0;;
	class PvPMainRampWallOffStateMachine* state_machine;
	PvPMainRampWallOffWaitForEnemies(Mediator* mediator, PvPMainRampWallOffStateMachine* state_machine)
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

class PvPMainRampWallOffBuildBuilding : public State
{
public:
	class PvPMainRampWallOffStateMachine* state_machine;
	PvPMainRampWallOffBuildBuilding(Mediator* mediator, PvPMainRampWallOffStateMachine* state_machine)
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

class PvPMainRampWallOffWaitForSafety : public State
{
public:
	const Unit* wall;
	Point2D fallback;
	class PvPMainRampWallOffStateMachine* state_machine;
	PvPMainRampWallOffWaitForSafety(Mediator* mediator, PvPMainRampWallOffStateMachine* state_machine, const Unit* wall, Point2D fallback)
	{
		this->mediator = mediator;
		this->state_machine = state_machine;
		this->wall = wall;
		this->fallback = fallback;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};


class PvPMainRampWallOffStateMachine : public StateMachine
{
public:
	const Unit* probe;
	Point2D wall_off_pos;
	PvPMainRampWallOffStateMachine(TheBigBot*, std::string, const Unit*, Point2D);
};



}