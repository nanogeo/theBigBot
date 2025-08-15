#pragma once

#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;

class DefenseArmyGroup : public ArmyGroup
{
protected:
	Point2D central_pos;
	int air_harassers = 0;
	Units unassigned_units;
	std::vector<DefensiveGroup> defensive_groups;

	void AllocateUnitsToDefensiveGroups(std::vector<EnemyArmyGroup>);

public:
	DefenseArmyGroup(Mediator*);

	void SetUp() override;
	void Run() override;
	std::string ToString() const override
	{
		return "Defense army group";
	}

	void AddNewUnit(const Unit* unit) override;
	void AddUnit(const Unit* unit) override;
	void RemoveUnit(const Unit* unit) override;

};

}