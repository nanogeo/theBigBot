#pragma once

#include "utility.h"
#include "piecewise_path.h"
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class DefendLineArmyGroup : public ArmyGroup
{
protected:
	Units oracles;

	Point2D start;
	Point2D end;
	float leash_range = 15;

public:
	DefendLineArmyGroup(Mediator*, Point2D, Point2D, std::vector<UNIT_TYPEID>, int, int);

	void Run() override;
	std::string ToString() const override
	{
		return "Defend line army group";
	}

	void AddNewUnit(const Unit* unit) override;
	void AddUnit(const Unit* unit) override;
	void RemoveUnit(const Unit* unit) override;
};

}