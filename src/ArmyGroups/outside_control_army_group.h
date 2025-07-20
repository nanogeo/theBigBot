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
protected:
	StateMachine* state_machine = nullptr;

public:
	OutsideControlArmyGroup(Mediator*, StateMachine*, std::vector<UNIT_TYPEID>, int, int);
	OutsideControlArmyGroup(Mediator*, ArmyTemplate<OutsideControlArmyGroup>*);

	void Run() override;
	std::string ToString() const override
	{
		return "Outside control army group";
	}

	void SetStateMachine(StateMachine*);
	StateMachine* GetStateMachine() const;

	void AddNewUnit(const Unit*) override;
	void RemoveUnit(const Unit*) override;
};

}