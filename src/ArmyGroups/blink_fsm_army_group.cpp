
#include "blink_fsm_army_group.h"
#include "blink_stalker_attack_terran_state_machine.h"
#include "mediator.h"
#include "utility.h"
#include "definitions.h"

#include "theBigBot.h"

namespace sc2 {


	BlinkFSMArmyGroup::BlinkFSMArmyGroup(Mediator* mediator, BlinkStalkerAttackTerran* state_machine, uint16_t desired_units, uint16_t max_units, uint16_t required_units, uint16_t min_reinfore_group_size) :
		AttackArmyGroup(mediator, mediator->GetDirectAttackLine(), { STALKER, PRISM }, desired_units, max_units, required_units, min_reinfore_group_size)
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
		// TODO check for units mid warp in
		mediator->SetUnitsCommand(all_units, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(all_units), 0);
		if (Utility::GetUnitsWithin(all_units, Utility::MedianCenter(all_units), 5).size() >= desired_units)
		{
			ready = true;
		}
	}

	void BlinkFSMArmyGroup::Run()
	{
		GroupUpNewUnits();

		for (auto itr = units_on_their_way.begin(); itr != units_on_their_way.end();)
		{
			if (MobilizeNewUnits(*itr))
			{
				for (const auto& unit : *itr)
				{
					AddUnit(unit);
				}
				itr = units_on_their_way.erase(itr);
			}
			else
			{
				itr++;
			}
		}

		if (AttackLine() == AttackLineResult::all_units_dead)
			mediator->MarkArmyGroupForDeletion(this);
	}

	void BlinkFSMArmyGroup::AddUnit(const Unit* unit)
	{
		if (unit->unit_type == STALKER || unit->unit_type == SENTRY || unit->unit_type == ADEPT || 
			unit->unit_type == ARCHON || unit->unit_type == IMMORTAL)
			basic_units.push_back(unit);

		ArmyGroup::AddUnit(unit); // Intentionally skipping AttackArmyGroup::AddUnit
	}

	void BlinkFSMArmyGroup::RemoveUnit(const Unit* unit)
	{
		basic_units.erase(std::remove(basic_units.begin(), basic_units.end(), unit), basic_units.end());
		state_machine->RemoveUnit(unit);

		ArmyGroup::RemoveUnit(unit); // Intentionally skipping AttackArmyGroup::RemoveUnit
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

			mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(all_units), 0);
			// TODO make sure units stay grouped up
		}
		return false;
	}

	void BlinkFSMArmyGroup::GroupUpNewUnits()
	{
		// TODO check for enemies in range
		mediator->SetUnitsCommand(new_units, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(new_units), 0);

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

}