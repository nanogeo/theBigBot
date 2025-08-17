
#include "blink_fsm_army_group.h"
#include "blink_stalker_attack_terran_state_machine.h"
#include "mediator.h"
#include "utility.h"
#include "definitions.h"

#include "theBigBot.h"

namespace sc2 {


BlinkFSMArmyGroup::BlinkFSMArmyGroup(Mediator* mediator, BlinkStalkerAttackTerran* state_machine, int desired_units, int max_units, int required_units, int min_reinfore_group_size) :
	PiecewiseAttackArmyGroup(mediator, mediator->GetDirectAttackLine(), { STALKER, PRISM, COLOSSUS, IMMORTAL }, desired_units, max_units, required_units, min_reinfore_group_size)
{
	this->state_machine = state_machine;
	main_attack_path = mediator->GetLocations().blink_main_attack_path_lines;
	natural_attack_path = mediator->GetLocations().blink_nat_attack_path_line;
	third_attack_path = mediator->GetLocations().blink_third_attack_path_lines;
	dispersion = .2f;
	default_range = 7;
}
	
void BlinkFSMArmyGroup::SetUp()
{
		
}

void BlinkFSMArmyGroup::Run()
{
	// handled by state machine
}

void BlinkFSMArmyGroup::ScourMap()
{
	ConvertToRegularAttackArmyGroup();
}

void BlinkFSMArmyGroup::AddUnit(const Unit* unit)
{
	PiecewiseAttackArmyGroup::AddUnit(unit);

	if (state_machine->AddUnit(unit) == false)
	{
		ConvertToRegularAttackArmyGroup();
	}
}

void BlinkFSMArmyGroup::RemoveUnit(const Unit* unit)
{
	state_machine->RemoveUnit(unit);
	PiecewiseAttackArmyGroup::RemoveUnit(unit);
}

// returns true when group is close enough to add into main army
bool BlinkFSMArmyGroup::MobilizeNewUnits(Units units)
{
	if (Distance2D(Utility::MedianCenter(units), Utility::MedianCenter(all_units)) < 5)
		return true;
	for (const auto& unit : units)
	{
		if (mediator->GetAttackStatus(unit))
			continue;
		if (unit->weapon_cooldown == 0)
			mediator->AddUnitToAttackers(unit);

		mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(all_units), CommandPriorty::low);
		// TODO make sure units stay grouped up
	}
	return false;
}

void BlinkFSMArmyGroup::GroupUpNewUnits()
{
	// TODO check for enemies in range
	mediator->SetUnitsCommand(new_units, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(new_units), CommandPriorty::low);

	Units group = Utility::GetUnitsWithin(new_units, Utility::MedianCenter(new_units), 5);
	if (group.size() >= min_reinforce_group_size)
	{
		units_on_their_way.push_back(group);
		for (const auto& unit : group)
		{
			new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());
		}
	}
}

Point2D BlinkFSMArmyGroup::GetConcaveOrigin() const
{
	return concave_origin;
}

void BlinkFSMArmyGroup::ResetConcaveOrigin()
{
	concave_origin = Point2D(0, 0);
}

void BlinkFSMArmyGroup::SetAttackPath(PiecewisePath path)
{
	attack_path = path;
}

const PiecewisePath& BlinkFSMArmyGroup::GetAttackPath() const
{
	return attack_path;
}

void BlinkFSMArmyGroup::SetUseStandby(bool value)
{
	using_standby = value;
}

AttackLineResult BlinkFSMArmyGroup::AttackLine(Units units)
{
	if (units.size() == 0)
	{
		return AttackLineResult::all_units_dead;
	}

	return AttackArmyGroup::AttackLine(units, concave_origin, default_range, &attack_path, limit_advance, Point2D(0, 0), {}, dispersion, unit_position_asignments, {}, advancing, attack_threshold);
}

AttackLineResult BlinkFSMArmyGroup::AttackLine(Units units, const Unit* prism)
{
	if (units.size() == 0)
	{
		return AttackLineResult::all_units_dead;
	}

	return AttackArmyGroup::AttackLine(units, concave_origin, default_range, &attack_path, limit_advance, Point2D(0, 0), { prism }, dispersion, unit_position_asignments, {}, advancing, attack_threshold);
}

void BlinkFSMArmyGroup::ConvertToRegularAttackArmyGroup()
{
	mediator->MarkStateMachineForDeletion(state_machine);
	mediator->MarkArmyGroupForDeletion(this);
	mediator->DeleteFourGateBlinkFSM();
}

}