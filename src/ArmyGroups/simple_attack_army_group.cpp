
#include "simple_attack_army_group.h"
#include "army_manager.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {

	SimpleAttackArmyGroup::SimpleAttackArmyGroup(Mediator* mediator, std::vector<Point2D> attack_path,
		std::vector<UNIT_TYPEID> unit_types, uint16_t desired_units, uint16_t max_units) : ArmyGroup(mediator)
	{
		this->unit_types = unit_types;

		this->attack_path = attack_path;
		this->desired_units = desired_units;
		this->max_units = max_units;
	}

	SimpleAttackArmyGroup::SimpleAttackArmyGroup(Mediator* mediator, ArmyTemplate<SimpleAttackArmyGroup>* army_template) : ArmyGroup(mediator)
	{
		unit_types = army_template->unit_types;
		desired_units = army_template->desired_units;
		max_units = army_template->max_units;

		attack_path = mediator->GetIndirectAttackPath();
	}

	void SimpleAttackArmyGroup::SetUp()
	{
		// TODO check for units mid warp in
		mediator->SetUnitsCommand(all_units, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(all_units), 0);
		if (Utility::GetUnitsWithin(all_units, Utility::MedianCenter(all_units), 10).size() >= desired_units)
		{
			ready = true;
			accept_new_units = false;
		}
	}

	void SimpleAttackArmyGroup::Run()
	{
		for (const auto& unit : all_units)
		{
			if (unit->orders.size() == 0)
			{
				for (const auto& point : attack_path)
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK, point, 0, true);
				}
			}
		}
	}

	void SimpleAttackArmyGroup::AddNewUnit(const Unit* unit)
	{
		// TODO check for units mid warp in
		AddUnit(unit);
	}

}