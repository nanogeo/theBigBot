#pragma once
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class ScoutBasesArmyGroup : public ArmyGroup
{
public:
	std::vector<Point2D> base_locations;
	Point2D current_target;

	ScoutBasesArmyGroup(Mediator*);

	void Run() override;
	std::string ToString() override
	{
		return "Scout bases army group";
	}

	void AddNewUnit(const Unit* unit) override;

};

}