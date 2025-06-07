#pragma once

#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;

template<typename T>
struct ArmyTemplate;

class DenyOuterBaseArmyGroup : public ArmyGroup
{
public:
	Point2D base_pos;

	DenyOuterBaseArmyGroup(Mediator*);
	DenyOuterBaseArmyGroup(Mediator*, ArmyTemplate<DenyOuterBaseArmyGroup>*);

	void SetUp() override;
	void Run() override;
	std::string ToString() override
	{
		return "Deny outer base army group";
	}

	void AddNewUnit(const Unit* unit) override;

};

}
