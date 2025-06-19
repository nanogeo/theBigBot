#pragma once

#include "finite_state_machine.h"


namespace sc2
{

class Mediator;
class ImmortalDropStateMachine;
class TheBigBot;
class ArmyGroup;


#pragma region ImmortalDrop


class ImmortalDropWaitForImmortals : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropWaitForImmortals(Mediator* mediator, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropInitialMove : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropInitialMove(Mediator* mediator, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropMicroDrop : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	bool first_immortal_turn = true;
	bool immortal1_has_attack_order = false;
	bool immortal2_has_attack_order = false;
	ImmortalDropMicroDrop(Mediator* mediator, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropMicroDropCarrying1 : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	int entry_frame;
	ImmortalDropMicroDropCarrying1(Mediator* mediator, ImmortalDropStateMachine* state_machine);
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ImmortalDropMicroDropCarrying2 : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	uint32_t entry_frame;
	ImmortalDropMicroDropCarrying2(Mediator* mediator, ImmortalDropStateMachine* state_machine);
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ImmortalDropMicroDropDropped1 : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	bool immortal1_has_attack_order = false;
	bool immortal2_has_attack_order = false;
	ImmortalDropMicroDropDropped1(Mediator* mediator, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropMicroDropDropped2 : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	bool immortal1_has_attack_order = false;
	bool immortal2_has_attack_order = false;
	ImmortalDropMicroDropDropped2(Mediator* mediator, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropWaitForShields : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropWaitForShields(Mediator* mediator, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropLeave : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropLeave(Mediator* mediator, ImmortalDropStateMachine* state_machine)
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


class ImmortalDropStateMachine : public StateMachine
{
public:
	const Unit* immortal1;
	const Unit* immortal2;
	const Unit* prism;
	Point2D entry_pos;
	std::vector<Point2D> prism_path;
	int path_index;
	std::vector<UNIT_TYPEID> target_priority = { CYCLONE, THOR_AOE, SIEGE_TANK_SIEGED, SIEGE_TANK,
		MULE, MARAUDER, MARINE, SCV, WIDOW_MINE, TECH_LAB, REACTOR };
	ImmortalDropStateMachine(Mediator* mediator, std::string name, const Unit* immortal1, const Unit* immortal2, 
		const Unit* prism, Point2D entry_pos, std::vector<Point2D> prism_path) : StateMachine(mediator, name)
	{
		current_state = new ImmortalDropWaitForImmortals(mediator, this);
		this->immortal1 = immortal1;
		this->immortal2 = immortal2;
		this->prism = prism;
		this->prism_path = prism_path;
		this->entry_pos = entry_pos;
		path_index = 0;
		current_state->EnterState();
	}

	Point2D UpdatePrismPathing();

};

}