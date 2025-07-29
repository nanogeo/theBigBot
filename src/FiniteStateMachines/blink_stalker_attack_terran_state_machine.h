#pragma once

#include "finite_state_machine.h"
#include "blink_fsm_army_group.h"

namespace sc2
{

class Mediator;
class BlinkStalkerAttackTerran;
class ArmyGroup;

enum BlinkAttackLocation
{
	main,
	natural,
	natural_defensive,
	third_1,
	third_2
};

class BlinkStalkerAttackTerranMoveAcross : public State
{
private:
	class BlinkStalkerAttackTerran* state_machine;
public:
	BlinkStalkerAttackTerranMoveAcross(Mediator* mediator, BlinkStalkerAttackTerran* state_machine)
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

class BlinkStalkerAttackTerranConsolidate : public State
{
private:
	class BlinkStalkerAttackTerran* state_machine;
public:
	BlinkStalkerAttackTerranConsolidate(Mediator* mediator, BlinkStalkerAttackTerran* state_machine)
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

class BlinkStalkerAttackTerranAttackMain : public State
{
private:
	class BlinkStalkerAttackTerran* state_machine;
	const Unit* target = nullptr;
public:
	BlinkStalkerAttackTerranAttackMain(Mediator* mediator, BlinkStalkerAttackTerran* state_machine)
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

class BlinkStalkerAttackTerranAttack : public State
{
private:
	class BlinkStalkerAttackTerran* state_machine;
	const Unit* target = nullptr;
public:
	BlinkStalkerAttackTerranAttack(Mediator* mediator, BlinkStalkerAttackTerran* state_machine)
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

class BlinkStalkerAttackTerranSnipeUnit : public State
{
private:
	class BlinkStalkerAttackTerran* state_machine;
	const Unit* target;
	float enter_time = 0;
public:
	BlinkStalkerAttackTerranSnipeUnit(Mediator* mediator, BlinkStalkerAttackTerran* state_machine, const Unit* target)
	{
		this->mediator = mediator;
		this->state_machine = state_machine;
		this->target = target;
	}
	virtual std::string toString() const override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class BlinkStalkerAttackTerranBlinkUp : public State
{
private:
	class BlinkStalkerAttackTerran* state_machine;
	Units stalkers_to_blink;
public:
	BlinkStalkerAttackTerranBlinkUp(Mediator* mediator, BlinkStalkerAttackTerran* state_machine)
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

class BlinkStalkerAttackTerranLeaveHighground : public State
{
private:
	class BlinkStalkerAttackTerran* state_machine;
public:
	BlinkStalkerAttackTerranLeaveHighground(Mediator* mediator, BlinkStalkerAttackTerran* state_machine)
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



class BlinkStalkerAttackTerran : public StateMachine
{
	friend BlinkStalkerAttackTerranMoveAcross;
	friend BlinkStalkerAttackTerranConsolidate;
	friend BlinkStalkerAttackTerranAttack;
	friend BlinkStalkerAttackTerranAttackMain;
	friend BlinkStalkerAttackTerranSnipeUnit;
	friend BlinkStalkerAttackTerranBlinkUp;
	friend BlinkStalkerAttackTerranLeaveHighground;
private:
	BlinkFSMArmyGroup* attached_army_group;
	const Unit* prism = nullptr;
	Units attacking_stalkers;
	Units standby_stalkers;
	Units moving_to_standby_stalkers;
	BlinkAttackLocation attack_location = BlinkAttackLocation::natural;
	Point2D consolidation_pos;
	Point2D default_consolidation_pos;
	Point2D prism_consolidation_pos;
	Point2D blink_up_pos;
	Point2D blink_down_pos;
	bool warping_in = false;

	void GroupUpStandbyStalkers();
public:
	BlinkStalkerAttackTerran(Mediator* mediator, std::string name, Point2D consolidation_pos,
		Point2D prism_consolidation_pos, Point2D blink_up_pos, Point2D blink_down_pos);


	virtual void RunStateMachine() override;
	bool AddUnit(const Unit*) override;
	void RemoveUnit(const Unit*) override;
	void SetConsolidationPos(Point2D);
	void ResetConsolidationPos();

	void SetAttachedArmyGroup(BlinkFSMArmyGroup*);


	~BlinkStalkerAttackTerran();
};


}