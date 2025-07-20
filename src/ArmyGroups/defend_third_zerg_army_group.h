#pragma once

#include "utility.h"
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class DefendThirdZergArmyGroup : public ArmyGroup
{
protected:
	Point2D pylon_gap_pos;

public:
	DefendThirdZergArmyGroup(Mediator*, Point2D, std::vector<UNIT_TYPEID>);

	void SetUp() override;
	void Run() override;
	std::string ToString() const override
	{
		return "Defend third base army group";
	}

	void AddNewUnit(const Unit* unit) override;

};

}