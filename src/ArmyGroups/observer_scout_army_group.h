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
protected:
	Point2D main_pos;
	Point2D natural_pos;
	Point2D current_target;

public:
	ObserverScoutArmyGroup(Mediator*);
	ObserverScoutArmyGroup(Mediator*, ArmyTemplate<ObserverScoutArmyGroup>*);

	void Run() override;
	std::string ToString() const override
	{
		return "Observer scout army group";
	}

	void AddNewUnit(const Unit* unit) override;

};

}