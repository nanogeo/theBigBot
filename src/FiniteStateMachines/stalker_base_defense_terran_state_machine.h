#pragma once

#include "finite_state_machine.h"


namespace sc2
{

class Mediator;
class StalkerBaseDefenseTerran;
class TheBigBot;
class ArmyGroup;


#pragma region StalkerBaseDefenseTerran

class StalkerBaseDefenseTerranDefendFront : public State
{
public:
	class StalkerBaseDefenseTerran* state_machine;
	bool forward = true;
	StalkerBaseDefenseTerranDefendFront(TheBigBot* agent, StalkerBaseDefenseTerran* state_machine)
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

class StalkerBaseDefenseTerranMoveAcross : public State
{
public:
	class StalkerBaseDefenseTerran* state_machine;
	StalkerBaseDefenseTerranMoveAcross(TheBigBot* agent, StalkerBaseDefenseTerran* state_machine)
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

class ScoutBaseDefenseTerranHarrassFront : public State
{
public:
	class StalkerBaseDefenseTerran* state_machine;
	Point2D attack_pos;
	Point2D retreat_pos;
	bool shields_regening = false;
	ScoutBaseDefenseTerranHarrassFront(TheBigBot* agent, StalkerBaseDefenseTerran* state_machine, Point2D adept_scout_shade, Point2D adept_scout_runaway)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->attack_pos = adept_scout_shade;
		this->retreat_pos = adept_scout_runaway;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};


#pragma endregion


class StalkerBaseDefenseTerran : public StateMachine
{
public:
	const Unit* stalker;
	bool attack_status = false;
	const Unit* target = nullptr;
	std::vector<Point2D> front_of_base;
	int event_id;
	StalkerBaseDefenseTerran(TheBigBot* agent, std::string name, const Unit* stalker, std::vector<Point2D> front_of_base);
	~StalkerBaseDefenseTerran();

	void OnUnitDestroyedListener(const Unit*);
};

}