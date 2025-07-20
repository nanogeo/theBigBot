#pragma once

#include "finite_state_machine.h"



namespace sc2
{

class Mediator;

class AdeptBaseDefenseTerran;
class Mediator;
class ArmyGroup;

class AdeptBaseDefenseTerranClearBase : public State
{
private:
	class AdeptBaseDefenseTerran* state_machine;
	bool checked_dead_space = false;
public:
	AdeptBaseDefenseTerranClearBase(Mediator* mediator, AdeptBaseDefenseTerran* state_machine)
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

class AdeptBaseDefenseTerranDefendFront : public State
{
private:
	class AdeptBaseDefenseTerran* state_machine;
	bool forward = true;
public:
	AdeptBaseDefenseTerranDefendFront(Mediator* mediator, AdeptBaseDefenseTerran* state_machine)
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

class AdeptBaseDefenseTerranMoveAcross : public State
{
private:
	class AdeptBaseDefenseTerran* state_machine;
public:
	AdeptBaseDefenseTerranMoveAcross(Mediator* mediator, AdeptBaseDefenseTerran* state_machine)
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

class AdeptBaseDefenseTerranScoutBase : public State
{
private:
	class AdeptBaseDefenseTerran* state_machine;
	Point2D shade_target;
	Point2D adept_scout_shade;
	Point2D adept_scout_runaway;
	Point2D adept_scout_ramptop;
	bool shields_regening = false;
	std::vector<Point2D> adept_scout_nat_path;
	std::vector<Point2D> adept_scout_base_spots;
	int base_spots_index;
public:
	AdeptBaseDefenseTerranScoutBase(Mediator* mediator, AdeptBaseDefenseTerran* state_machine, Point2D adept_scout_shade, Point2D adept_scout_runaway,
		Point2D adept_scout_ramptop, std::vector<Point2D> adept_scout_nat_path, std::vector<Point2D> adept_scout_base_spots)
	{
		this->mediator = mediator;
		this->state_machine = state_machine;
		this->adept_scout_shade = adept_scout_shade;
		this->adept_scout_runaway = adept_scout_runaway;
		this->adept_scout_ramptop = adept_scout_ramptop;
		this->adept_scout_nat_path = adept_scout_nat_path;
		this->adept_scout_base_spots = adept_scout_base_spots;
		shade_target = adept_scout_ramptop;
		base_spots_index = 1;
	}
	virtual std::string toString() const override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
	void UpdateShadeTarget();
};



class AdeptBaseDefenseTerran : public StateMachine
{
	friend AdeptBaseDefenseTerranClearBase;
	friend AdeptBaseDefenseTerranDefendFront;
	friend AdeptBaseDefenseTerranMoveAcross;
	friend AdeptBaseDefenseTerranScoutBase;
private:
	const Unit* adept = nullptr;
	uint32_t frame_shade_used = 0;
	bool attack_status = false;
	const Unit* shade = nullptr;
	Point2D dead_space_spot;
	const Unit* target = nullptr;
	std::vector<Point2D> front_of_base;
	int event_id;
public:
	AdeptBaseDefenseTerran(Mediator* mediator, std::string name, Point2D dead_space_spot, std::vector<Point2D> front_of_base);
	~AdeptBaseDefenseTerran();

	bool AddUnit(const Unit*) override;

	void OnUnitCreatedListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);
};



}