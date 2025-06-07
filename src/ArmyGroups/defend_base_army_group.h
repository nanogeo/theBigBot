#pragma once

#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class DefendBaseArmyGroup : public ArmyGroup
{
public:
	Units probes;

	Point2D base_pos;
	//Point2D fallback_pos;
	const Unit* enemy_minerals = nullptr;
	const Unit* base_minerals = nullptr;
	//bool defending = false;
	//bool falling_back = false;
	float leash_range = 10;

	DefendBaseArmyGroup(Mediator*, Point2D, std::vector<UNIT_TYPEID>, uint16_t, uint16_t);

	void SetUp() override;
	void Run() override;
	std::string ToString() override
	{
		return "Defend base army group";
	}

	void AddNewUnit(const Unit* unit) override;
	void AddUnit(const Unit* unit) override;
	void RemoveUnit(const Unit* unit) override;
};

}