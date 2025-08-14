
#include "defense_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {

DefenseArmyGroup::DefenseArmyGroup(Mediator* mediator) : ArmyGroup(mediator)
{
	unit_types = ALL_ARMY_UNITS;
	central_pos = mediator->GetCentralBasePos();
}
	
void DefenseArmyGroup::SetUp()
{
	ready = true;
}

void DefenseArmyGroup::Run()
{
	central_pos = mediator->GetCentralBasePos();
	for (const auto& unit : all_units)
	{
		if (Distance2D(unit->pos, central_pos) > 10)
			mediator->SetUnitCommand(unit, A_MOVE, central_pos, CommandPriorty::low);
	}

	// update ongoing attacks
	// update incoming attacks
	std::vector<EnemyArmyGroup> incoming_army_groups = mediator->GetIncomingEnemyArmyGroups();
	// find air harassers

	// group units to defend ongoing attacks
	// group units to defend incoming attacks
	
	// position units to defend against air harassers
}

void DefenseArmyGroup::AddNewUnit(const Unit* unit)
{
	AddUnit(unit);
}

void DefenseArmyGroup::AddUnit(const Unit* unit)
{
	ArmyGroup::AddUnit(unit);
}

void DefenseArmyGroup::RemoveUnit(const Unit* unit)
{
	ArmyGroup::RemoveUnit(unit);
}

}