#pragma once

#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class DefendMainRampArmyGroup : public ArmyGroup
{
public:
	Units sentries;

	Point2D ramp_top;
	Point2D forcefield_pos;
	Point2D walloff_pos;

	DefendMainRampArmyGroup(Mediator*, Point2D, Point2D);

	void Run() override;
	std::string ToString() override
	{
		return "Defend main ramp army group";
	}

	void AddNewUnit(const Unit* unit) override;
	void AddUnit(const Unit* unit) override;
	void RemoveUnit(const Unit* unit) override;

};
}