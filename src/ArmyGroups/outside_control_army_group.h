#pragma once

#include "definitions.h"
#include "army_group.h"
#include "finite_state_machine.h"



namespace sc2
{

class Mediator;

template<typename T>
struct ArmyTemplate;

class OutsideControlArmyGroup : public ArmyGroup
{
public:
	StateMachine* state_machine = nullptr;
	OutsideControlArmyGroup(Mediator*, StateMachine*, std::vector<UNIT_TYPEID>, uint16_t, uint16_t);
	OutsideControlArmyGroup(Mediator*, ArmyTemplate<OutsideControlArmyGroup>*);

	void Run() override;
	std::string ToString() override
	{
		return "Outside control army group";
	}

	void AddNewUnit(const Unit*) override;
	void RemoveUnit(const Unit*) override;
};

}