
#include "scout_bases_army_group.h"
#include "mediator.h"
#include "definitions.h"

namespace sc2 {

ScoutBasesArmyGroup::ScoutBasesArmyGroup(Mediator* mediator) : ArmyGroup(mediator)
{
	this->unit_types = unit_types;
	desired_units = 1;
	max_units = 1;

	this->base_locations = mediator->GetAllBases();
	unit_types = { ZEALOT, ADEPT, STALKER }; // TODO maybe add flying units phoenix/oracle?
}

void ScoutBasesArmyGroup::Run()
{
	if (all_units.size() == 0)
		return;

	if (current_target == Point2D(0, 0) && base_locations.size() > 0)
		current_target = Utility::ClosestTo(base_locations, all_units[0]->pos);

	if (Distance2D(all_units[0]->pos, current_target) > 7)
	{
		if (Utility::DistanceToClosest(mediator->GetUnits(IsUnits(TOWNHALL_TYPES)), current_target) < 1)
		{
			base_locations.erase(std::remove(base_locations.begin(), base_locations.end(), current_target), base_locations.end());
			if (base_locations.size() == 0)
			{
				base_locations = mediator->GetAllBases();
			}
			current_target = Utility::ClosestTo(base_locations, all_units[0]->pos);
		}
		mediator->SetUnitCommand(all_units[0], A_MOVE, current_target, CommandPriority::low);
	}
	else
	{
		if (base_locations.size() == 0)
		{
			base_locations = mediator->GetAllBases();
		}
		current_target = Utility::ClosestTo(base_locations, all_units[0]->pos);
		base_locations.erase(std::remove(base_locations.begin(), base_locations.end(), current_target), base_locations.end());
	}

}

void ScoutBasesArmyGroup::AddNewUnit(const Unit* unit)
{
	AddUnit(unit);
}

}