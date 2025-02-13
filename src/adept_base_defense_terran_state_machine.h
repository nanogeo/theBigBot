#pragma once

#include "finite_state_machine.h"



namespace sc2
{

class Mediator;

class AdeptBaseDefenseTerran;
class TheBigBot;
class ArmyGroup;

class AdeptBaseDefenseTerranClearBase : public State
{
public:
	class AdeptBaseDefenseTerran* state_machine;
	bool checked_dead_space = false;
	AdeptBaseDefenseTerranClearBase(TheBigBot* agent, AdeptBaseDefenseTerran* state_machine)
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

class AdeptBaseDefenseTerranDefendFront : public State
{
public:
	class AdeptBaseDefenseTerran* state_machine;
	bool forward = true;
	AdeptBaseDefenseTerranDefendFront(TheBigBot* agent, AdeptBaseDefenseTerran* state_machine)
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

class AdeptBaseDefenseTerranMoveAcross : public State
{
public:
	class AdeptBaseDefenseTerran* state_machine;
	AdeptBaseDefenseTerranMoveAcross(TheBigBot* agent, AdeptBaseDefenseTerran* state_machine)
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

class AdeptBaseDefenseTerranScoutBase : public State
{
public:
	class AdeptBaseDefenseTerran* state_machine;
	Point2D shade_target;
	Point2D adept_scout_shade;
	Point2D adept_scout_runaway;
	Point2D adept_scout_ramptop;
	bool shields_regening = false;
	std::vector<Point2D> adept_scout_nat_path;
	std::vector<Point2D> adept_scout_base_spots;
	int base_spots_index;
	AdeptBaseDefenseTerranScoutBase(TheBigBot* agent, AdeptBaseDefenseTerran* state_machine, Point2D adept_scout_shade, Point2D adept_scout_runaway,
		Point2D adept_scout_ramptop, std::vector<Point2D> adept_scout_nat_path, std::vector<Point2D> adept_scout_base_spots)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->adept_scout_shade = adept_scout_shade;
		this->adept_scout_runaway = adept_scout_runaway;
		this->adept_scout_ramptop = adept_scout_ramptop;
		this->adept_scout_nat_path = adept_scout_nat_path;
		this->adept_scout_base_spots = adept_scout_base_spots;
		shade_target = adept_scout_ramptop;
		base_spots_index = 1;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
	void UpdateShadeTarget();
};



class AdeptBaseDefenseTerran : public StateMachine
{
public:
	const Unit* adept;
	int frame_shade_used = 0;
	bool attack_status = false;
	const Unit* shade = NULL;
	Point2D dead_space_spot;
	const Unit* target = NULL;
	std::vector<Point2D> front_of_base;
	int event_id;
	AdeptBaseDefenseTerran(TheBigBot* agent, std::string name, const Unit* adept, Point2D dead_space_spot, std::vector<Point2D> front_of_base);
	~AdeptBaseDefenseTerran();

	void OnUnitCreatedListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);
};



}