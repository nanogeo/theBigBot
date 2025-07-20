#pragma once
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class ScoutBasesArmyGroup : public ArmyGroup
{
protected:
	std::vector<Point2D> base_locations;
	Point2D current_target;

public:
	ScoutBasesArmyGroup(Mediator*);

	void Run() override;
	std::string ToString() const override
	{
		return "Scout bases army group";
	}

	void AddNewUnit(const Unit* unit) override;

};

}