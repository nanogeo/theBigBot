#pragma once

#include "finite_state_machine.h"

namespace sc2
{

class Mediator;

#pragma region BlinkStalkerAttackTerran

class BlinkStalkerAttackTerran;
class TheBigBot;
class ArmyGroup;

class BlinkStalkerAttackTerranMoveAcross : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	BlinkStalkerAttackTerranMoveAcross(TheBigBot* agent, BlinkStalkerAttackTerran* state_machine)
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

class BlinkStalkerAttackTerranWarpIn : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	BlinkStalkerAttackTerranWarpIn(TheBigBot* agent, BlinkStalkerAttackTerran* state_machine)
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

class BlinkStalkerAttackTerranConsolidate : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	BlinkStalkerAttackTerranConsolidate(TheBigBot* agent, BlinkStalkerAttackTerran* state_machine)
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

class BlinkStalkerAttackTerranAttack : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	const Unit* target = nullptr;
	BlinkStalkerAttackTerranAttack(TheBigBot* agent, BlinkStalkerAttackTerran* state_machine)
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

class BlinkStalkerAttackTerranSnipeUnit : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	const Unit* target;
	float enter_time = 0;
	BlinkStalkerAttackTerranSnipeUnit(TheBigBot* agent, BlinkStalkerAttackTerran* state_machine, const Unit* target)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->target = target;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class BlinkStalkerAttackTerranBlinkUp : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	Units stalkers_to_blink;
	BlinkStalkerAttackTerranBlinkUp(TheBigBot* agent, BlinkStalkerAttackTerran* state_machine)
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

class BlinkStalkerAttackTerranLeaveHighground : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	Units stalkers_to_blink;
	BlinkStalkerAttackTerranLeaveHighground(TheBigBot* agent, BlinkStalkerAttackTerran* state_machine)
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


class BlinkStalkerAttackTerran : public StateMachine
{
public:
	const Unit* prism = nullptr;
	Units attacking_stalkers;
	Units standby_stalkers;
	Units moving_to_standby_stalkers;
	bool attacking_main = false;
	Point2D consolidation_pos;
	Point2D prism_consolidation_pos;
	Point2D blink_up_pos;
	Point2D blink_down_pos;
	bool warping_in = false;


	BlinkStalkerAttackTerran(TheBigBot* agent, std::string name, Point2D consolidation_pos, 
		Point2D prism_consolidation_pos, Point2D blink_up_pos, Point2D blink_down_pos);


	virtual void RunStateMachine() override;
	bool AddUnit(const Unit*) override;
	void RemoveUnit(const Unit*) override;

	~BlinkStalkerAttackTerran();
};


}