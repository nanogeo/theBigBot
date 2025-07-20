
#include "defend_third_zerg_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {


DefendThirdZergArmyGroup::DefendThirdZergArmyGroup(Mediator* mediator, Point2D pylon_gap_pos, std::vector<UNIT_TYPEID> unit_types) : ArmyGroup(mediator)
{
	for (const auto& type : unit_types)
	{
		this->unit_types.push_back(type);
	}
	desired_units = 1;
	max_units = 1;

	this->pylon_gap_pos = pylon_gap_pos;
}
	
void DefendThirdZergArmyGroup::SetUp()
{
	mediator->SetUnitsCommand(all_units, A_MOVE, pylon_gap_pos, CommandPriorty::low);
	if (Utility::GetUnitsWithin(all_units, Utility::MedianCenter(all_units), 10).size() >= desired_units)
	{
		ready = true;
		accept_new_units = false;
	}
}

void DefendThirdZergArmyGroup::Run()
{
	for (const auto& unit : all_units)
	{
		if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Self, IsUnit(PROBE)), unit->pos) < 2)
		{
			mediator->SetUnitCommand(unit, A_STOP, CommandPriorty::low);
		}
		else
		{
			if (Distance2D(unit->pos, pylon_gap_pos) > 1 && unit->orders.size() == 0)
			{
				mediator->SetUnitCommand(unit, A_MOVE, pylon_gap_pos, CommandPriorty::low);
				mediator->SetUnitCommand(unit, A_HOLD_POSITION, CommandPriorty::low, true);
			}
		}
	}
	// TODO use fire control to find the best target
}

void DefendThirdZergArmyGroup::AddNewUnit(const Unit* unit)
{
	AddUnit(unit);
}

}