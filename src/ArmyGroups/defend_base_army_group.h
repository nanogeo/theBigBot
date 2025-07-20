#pragma once

#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class DefendBaseArmyGroup : public ArmyGroup
{
protected:
	Units probes;

	Point2D base_pos;
	//Point2D fallback_pos;
	const Unit* enemy_minerals = nullptr;
	const Unit* base_minerals = nullptr;
	//bool defending = false;
	//bool falling_back = false;
	float leash_range = 10;

public:
	DefendBaseArmyGroup(Mediator*, Point2D, std::vector<UNIT_TYPEID>, int, int);

	void SetUp() override;
	void Run() override;
	std::string ToString() const override
	{
		return "Defend base army group";
	}

	void AddNewUnit(const Unit* unit) override;
	void AddUnit(const Unit* unit) override;
	void RemoveUnit(const Unit* unit) override;

	Point2D GetBasePos() const;
};

}