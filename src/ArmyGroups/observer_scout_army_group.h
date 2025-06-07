#pragma once
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;

template<typename T>
struct ArmyTemplate;

class ObserverScoutArmyGroup : public ArmyGroup
{
public:
	Point2D main_pos;
	Point2D natural_pos;
	Point2D current_target;

	ObserverScoutArmyGroup(Mediator*);
	ObserverScoutArmyGroup(Mediator*, ArmyTemplate<ObserverScoutArmyGroup>*);

	void Run() override;
	std::string ToString() override
	{
		return "Observer scout army group";
	}

	void AddNewUnit(const Unit* unit) override;

};

}