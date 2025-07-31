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
protected:
	Point2D base_pos = Point2D(0, 0);

public:
	DenyOuterBaseArmyGroup(Mediator*, ArmyTemplate<DenyOuterBaseArmyGroup>*);

	void SetUp() override;
	void Run() override;
	std::string ToString() const override
	{
		return "Deny outer base army group";
	}

	void AddNewUnit(const Unit* unit) override;
	Point2D GetTargetPos() const;
};

}
