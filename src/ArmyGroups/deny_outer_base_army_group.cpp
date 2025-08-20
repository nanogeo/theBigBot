
#include "deny_outer_base_army_group.h"
#include "mediator.h"
#include "definitions.h"

namespace sc2 {

DenyOuterBaseArmyGroup::DenyOuterBaseArmyGroup(Mediator* mediator, ArmyTemplate<DenyOuterBaseArmyGroup>* army_template) : ArmyGroup(mediator)
{
	unit_types = army_template->unit_types;
	desired_units = army_template->desired_units;
	max_units = army_template->max_units;

	std::vector<Point2D> exposed_enemy_bases = mediator->GetExposedEnemyBases();
	for (const auto& pos : exposed_enemy_bases)
	{
		if (mediator->GetArmyGroupDenyingBase(pos) != nullptr)
			continue;
		base_pos = pos;
	}
	if (base_pos == Point2D(0, 0))
	{
		std::cerr << "Error, no exposed base found in DenyOuterBaseArmyGroup" << std::endl;
		mediator->LogMinorError();
	}
}

void DenyOuterBaseArmyGroup::SetUp()
{
	// TODO check for units mid warp in
	mediator->SetUnitsCommand(all_units, A_MOVE, Utility::MedianCenter(all_units), CommandPriority::low);
	if (Utility::GetUnitsWithin(all_units, Utility::MedianCenter(all_units), 5).size() >= desired_units)
	{
		ready = true;
		accept_new_units = false;
	}
}

void DenyOuterBaseArmyGroup::Run()
{
	mediator->SetUnitsCommand(all_units, A_ATTACK, base_pos, CommandPriority::normal);

	if (all_units.size() == 0 || Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits(TOWNHALL_TYPES)), base_pos) > 7)
		mediator->MarkArmyGroupForDeletion(this);
}

void DenyOuterBaseArmyGroup::AddNewUnit(const Unit* unit)
{
	AddUnit(unit);
}

Point2D DenyOuterBaseArmyGroup::GetTargetPos() const
{
	return base_pos;
}

}