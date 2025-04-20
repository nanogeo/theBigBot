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
	ImmortalDropWaitForImmortals(TheBigBot* agent, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropInitialMove : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropInitialMove(TheBigBot* agent, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropMicroDrop : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	bool first_immortal_turn = true;
	bool immortal1_has_attack_order = false;
	bool immortal2_has_attack_order = false;
	ImmortalDropMicroDrop(TheBigBot* agent, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropMicroDropCarrying1 : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	int entry_frame;
	ImmortalDropMicroDropCarrying1(TheBigBot* agent, ImmortalDropStateMachine* state_machine);
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
	ImmortalDropMicroDropCarrying2(TheBigBot* agent, ImmortalDropStateMachine* state_machine);
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
	ImmortalDropMicroDropDropped1(TheBigBot* agent, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropMicroDropDropped2 : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	bool immortal1_has_attack_order = false;
	bool immortal2_has_attack_order = false;
	ImmortalDropMicroDropDropped2(TheBigBot* agent, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropWaitForShields : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropWaitForShields(TheBigBot* agent, ImmortalDropStateMachine* state_machine)
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

class ImmortalDropLeave : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropLeave(TheBigBot* agent, ImmortalDropStateMachine* state_machine)
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


class ImmortalDropStateMachine : public StateMachine
{
public:
	const Unit* immortal1;
	const Unit* immortal2;
	const Unit* prism;
	Point2D entry_pos;
	std::vector<Point2D> prism_path;
	int path_index;
	std::vector<UNIT_TYPEID> target_priority = { UNIT_TYPEID::TERRAN_CYCLONE, UNIT_TYPEID::TERRAN_THOR, UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, UNIT_TYPEID::TERRAN_SIEGETANK,
		UNIT_TYPEID::TERRAN_MULE, UNIT_TYPEID::TERRAN_MARAUDER, UNIT_TYPEID::TERRAN_MARINE, UNIT_TYPEID::TERRAN_SCV, UNIT_TYPEID::TERRAN_WIDOWMINE, UNIT_TYPEID::TERRAN_TECHLAB, UNIT_TYPEID::TERRAN_REACTOR };
	ImmortalDropStateMachine(TheBigBot* agent, std::string name, const Unit* immortal1, const Unit* immortal2, 
		const Unit* prism, Point2D entry_pos, std::vector<Point2D> prism_path) : StateMachine(agent, name)
	{
		current_state = new ImmortalDropWaitForImmortals(agent, this);
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