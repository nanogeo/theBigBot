#pragma once

#include "finite_state_machine.h"


namespace sc2
{

class Mediator;
class AdeptHarassProtoss;
class ArmyGroup;

#pragma region AdeptHarassProtoss

class AdeptHarassProtossMoveAcross : public State
{
private:
	class AdeptHarassProtoss* state_machine;
public:
	AdeptHarassProtossMoveAcross(Mediator* mediator, AdeptHarassProtoss* state_machine)
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

class AdeptHarassProtossConsolidate : public State
{
private:
	class AdeptHarassProtoss* state_machine;
public:
	AdeptHarassProtossConsolidate(Mediator* mediator, AdeptHarassProtoss* state_machine)
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

class AdeptHarassProtossShadeIntoBase : public State
{private:
	class AdeptHarassProtoss* state_machine;
public:
	AdeptHarassProtossShadeIntoBase(Mediator* mediator, AdeptHarassProtoss* state_machine)
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

class AdeptHarassProtossShadeToOtherSide : public State
{
private:
	class AdeptHarassProtoss* state_machine;
public:
	AdeptHarassProtossShadeToOtherSide(Mediator* mediator, AdeptHarassProtoss* state_machine)
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

class AdeptHarassProtossKillProbes : public State
{
private:
	class AdeptHarassProtoss* state_machine;
public:
	AdeptHarassProtossKillProbes(Mediator* mediator, AdeptHarassProtoss* state_machine)
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



#pragma endregion


class AdeptHarassProtoss : public StateMachine
{
	friend AdeptHarassProtossMoveAcross;
	friend AdeptHarassProtossConsolidate;
	friend AdeptHarassProtossShadeIntoBase;
	friend AdeptHarassProtossShadeToOtherSide;
	friend AdeptHarassProtossKillProbes;
private:
	Units adepts;
	std::vector<Point2D> consolidation_points;
	uint32_t frame_shade_used = 0;
	bool attack_status;
	Units shades;
	const Unit* target = nullptr;
	int index = 0;
	int event_id;

public:
	AdeptHarassProtoss(Mediator* mediator, std::string name, Units adepts, std::vector<Point2D> consolidation_points);

	~AdeptHarassProtoss();

	void OnUnitCreatedListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);
};
}