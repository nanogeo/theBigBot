#pragma once

#include "utility.h"
#include "path_manager.h"
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class DefendLineArmyGroup : public ArmyGroup
{
public:
	Units oracles;

	Point2D start;
	Point2D end;
	float leash_range = 15;

	DefendLineArmyGroup(Mediator*, Point2D, Point2D, std::vector<UNIT_TYPEID>, uint16_t, uint16_t);

	void Run() override;
	std::string ToString() override
	{
		return "Defend line army group";
	}

	void AddNewUnit(const Unit* unit) override;
	void AddUnit(const Unit* unit) override;
	void RemoveUnit(const Unit* unit) override;
};

}