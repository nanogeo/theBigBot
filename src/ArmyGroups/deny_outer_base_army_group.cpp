
#include "deny_outer_base_army_group.h"
#include "mediator.h"
#include "definitions.h"

namespace sc2 {

DenyOuterBaseArmyGroup::DenyOuterBaseArmyGroup(Mediator* mediator) : ArmyGroup(mediator)
{
	unit_types = { STALKER }; // TODO add other units?

	this->base_pos = Point2D(0, 0);// TODO find this in here somehow;
}

DenyOuterBaseArmyGroup::DenyOuterBaseArmyGroup(Mediator* mediator, ArmyTemplate<DenyOuterBaseArmyGroup>* army_template) : ArmyGroup(mediator)
{
	unit_types = { STALKER }; // TODO add other units?

	this->base_pos = Point2D(0, 0);// TODO find this in here somehow;
}

void DenyOuterBaseArmyGroup::SetUp()
{
	// TODO check for units mid warp in
	mediator->SetUnitsCommand(all_units, A_MOVE, Utility::MedianCenter(all_units), CommandPriorty::low);
	if (Utility::GetUnitsWithin(all_units, Utility::MedianCenter(all_units), 5).size() >= desired_units)
	{
		ready = true;
		accept_new_units = false;
	}
}

void DenyOuterBaseArmyGroup::Run()
{
	mediator->SetUnitsCommand(all_units, A_ATTACK, base_pos, CommandPriorty::normal);

	if (all_units.size() == 0 || Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits(TOWNHALL_TYPES)), base_pos) > 7)
		mediator->MarkArmyGroupForDeletion(this);
}

void DenyOuterBaseArmyGroup::AddNewUnit(const Unit* unit)
{
	AddUnit(unit);
}

}