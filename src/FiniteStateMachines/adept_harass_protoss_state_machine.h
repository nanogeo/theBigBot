#pragma once

#include "finite_state_machine.h"


namespace sc2
{

class Mediator;
class AdeptHarassProtoss;
class TheBigBot;
class ArmyGroup;

#pragma region AdeptHarassProtoss

class AdeptHarassProtossMoveAcross : public State
{
public:
	class AdeptHarassProtoss* state_machine;
	AdeptHarassProtossMoveAcross(TheBigBot* agent, AdeptHarassProtoss* state_machine)
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

class AdeptHarassProtossConsolidate : public State
{
public:
	class AdeptHarassProtoss* state_machine;
	AdeptHarassProtossConsolidate(TheBigBot* agent, AdeptHarassProtoss* state_machine)
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

class AdeptHarassProtossShadeIntoBase : public State
{
public:
	class AdeptHarassProtoss* state_machine;
	AdeptHarassProtossShadeIntoBase(TheBigBot* agent, AdeptHarassProtoss* state_machine)
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

class AdeptHarassProtossShadeToOtherSide : public State
{
public:
	class AdeptHarassProtoss* state_machine;
	AdeptHarassProtossShadeToOtherSide(TheBigBot* agent, AdeptHarassProtoss* state_machine)
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

class AdeptHarassProtossKillProbes : public State
{
public:
	class AdeptHarassProtoss* state_machine;
	AdeptHarassProtossKillProbes(TheBigBot* agent, AdeptHarassProtoss* state_machine)
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


class AdeptHarassProtoss : public StateMachine
{
public:
	Units adepts;
	std::vector<Point2D> consolidation_points;
	uint32_t frame_shade_used = 0;
	bool attack_status;
	Units shades;
	const Unit* target = nullptr;
	int index = 0;
	int event_id;

	AdeptHarassProtoss(TheBigBot* agent, std::string name, Units adepts, std::vector<Point2D> consolidation_points);

	~AdeptHarassProtoss();

	void OnUnitCreatedListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);
};
}