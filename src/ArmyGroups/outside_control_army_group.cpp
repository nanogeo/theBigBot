
#include "outside_control_army_group.h"
#include "army_manager.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {


	OutsideControlArmyGroup::OutsideControlArmyGroup(Mediator* mediator, StateMachine* state_machine, std::vector<UNIT_TYPEID> unit_types,
		uint16_t desired_units, uint16_t max_units) : ArmyGroup(mediator)
	{
		this->unit_types = unit_types;

		this->desired_units = desired_units;
		this->max_units = max_units;
		this->state_machine = state_machine;
	}

	OutsideControlArmyGroup::OutsideControlArmyGroup(Mediator* mediator, ArmyTemplate<OutsideControlArmyGroup>* army_template) : ArmyGroup(mediator)
	{
		unit_types = army_template->unit_types;
		desired_units = army_template->desired_units;
		max_units = army_template->max_units;
		state_machine = nullptr;
	}
	
	void OutsideControlArmyGroup::Run()
	{
		for (int i = 0; i < new_units.size(); i++)
		{
			if (state_machine->AddUnit(new_units[i]))
			{
				AddUnit(new_units[i]);
				i--;
			}
		}
		// should state machine be run here instead of from state machine manager?
	}

	void OutsideControlArmyGroup::AddNewUnit(const Unit* unit)
	{
		if (state_machine && state_machine->AddUnit(unit))
			AddUnit(unit);
		else
			ArmyGroup::AddNewUnit(unit);
	}

	void OutsideControlArmyGroup::RemoveUnit(const Unit* unit)
	{
		ArmyGroup::RemoveUnit(unit);
		if (state_machine)
			state_machine->RemoveUnit(unit);
	}

}