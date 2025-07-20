#pragma once

#include "utility.h"
#include "path_manager.h"
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;

template<typename T>
struct ArmyTemplate;

class SimpleAttackArmyGroup : public ArmyGroup
{
protected:
	std::vector<Point2D> attack_path;
	int current_attack_index = 0;

public:
	SimpleAttackArmyGroup(Mediator*, std::vector<Point2D>, std::vector<UNIT_TYPEID>, int, int);
	SimpleAttackArmyGroup(Mediator*, ArmyTemplate<SimpleAttackArmyGroup>*);

	void SetUp() override;
	void Run() override;
	std::string ToString() const override
	{
		return "Simple attack army group";
	}

	void AddNewUnit(const Unit* unit) override;

};

}