
#include "piecewise_attack_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "definitions.h"

#include "theBigBot.h"

namespace sc2 {


PiecewiseAttackArmyGroup::PiecewiseAttackArmyGroup(Mediator* mediator, PiecewisePath attack_path, std::vector<UNIT_TYPEID> unit_types,
	int desired_units, int max_units, int required_units, int min_reinforce_group_size, bool limit_advance) : AttackArmyGroup(mediator)
{
	this->unit_types = unit_types;
	this->desired_units = desired_units;
	this->max_units = max_units;
	this->required_units = required_units;
	this->min_reinforce_group_size = min_reinforce_group_size;

	this->attack_path = attack_path;
	dispersion = 0;
	switch (mediator->GetEnemyRace())
	{
	case Race::Protoss:
		target_priority = PROTOSS_PRIO;
		break;
	case Race::Terran:
		target_priority = TERRAN_PRIO;
		break;
	case Race::Zerg:
		target_priority = ZERG_PRIO;
		break;
	default:
		std::cerr << "Unknown enemy race in AttackArmyGroup" << std::endl;
		break;
	}
	this->limit_advance = limit_advance;
	pre_prism_limit = mediator->GetLocations().adept_scout_ramptop;
}

PiecewiseAttackArmyGroup::PiecewiseAttackArmyGroup(Mediator* mediator, ArmyTemplate<PiecewiseAttackArmyGroup>* army_template) : AttackArmyGroup(mediator)
{
	unit_types = army_template->unit_types;
	desired_units = army_template->desired_units;
	max_units = army_template->max_units;

	required_units = 0;
	for (const auto& temp : army_template->required_units)
	{
		required_units += temp.second;
	}
	this->min_reinforce_group_size = 5;
	dispersion = 0;
	switch (mediator->GetEnemyRace())
	{
	case Race::Protoss:
		target_priority = PROTOSS_PRIO;
		break;
	case Race::Terran:
		dispersion = .2f;
		target_priority = TERRAN_PRIO;
		break;
	case Race::Zerg:
		target_priority = ZERG_PRIO;
		break;
	default:
		std::cerr << "Unknown enemy race in AttackArmyGroup" << std::endl;
		break;
	}
	limit_advance = false;
	attack_path = mediator->GetDirectAttackLine();
}
	
void PiecewiseAttackArmyGroup::SetUp()
{
	// TODO check for units mid warp in
	if (all_units.size() == 0)
		return;

	mediator->SetUnitsCommand(all_units, A_MOVE, Utility::MedianCenter(all_units), CommandPriority::low);
	if (Utility::GetUnitsWithin(all_units, Utility::MedianCenter(all_units), 5).size() >= required_units)
	{
		ready = true;
	}
}

void PiecewiseAttackArmyGroup::Run()
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

void PiecewiseAttackArmyGroup::ScourMap()
{
	for (auto itr = units_on_their_way.begin(); itr != units_on_their_way.end();)
	{
		for (const auto& unit : *itr)
		{
			AddUnit(unit);
		}
		itr = units_on_their_way.erase(itr);
	}
	ArmyGroup::ScourMap();
}

void PiecewiseAttackArmyGroup::AddUnit(const Unit* unit)
{
	if (unit->unit_type == STALKER || unit->unit_type == SENTRY || unit->unit_type == ADEPT ||
		unit->unit_type == ARCHON || unit->unit_type == IMMORTAL)
	{
		if (std::find(basic_units.begin(), basic_units.end(), unit) == basic_units.end())
			basic_units.push_back(unit);
	}
	else if (unit->unit_type == ORACLE)
	{
		if (std::find(oracles.begin(), oracles.end(), unit) == oracles.end())
			oracles.push_back(unit);
	}
	else if (unit->unit_type == PRISM || unit->unit_type == PRISM_SIEGED)
	{
		if (std::find(warp_prisms.begin(), warp_prisms.end(), unit) == warp_prisms.end())
			warp_prisms.push_back(unit);
	}

	ArmyGroup::AddUnit(unit);
}

void PiecewiseAttackArmyGroup::RemoveUnit(const Unit* unit)
{
	for (auto itr = units_on_their_way.begin(); itr != units_on_their_way.end();)
	{
		itr->erase(std::remove(itr->begin(), itr->end(), unit), itr->end());
		if (itr->size() == 0)
			itr = units_on_their_way.erase(itr);
		else
			itr++;
	}
	basic_units.erase(std::remove(basic_units.begin(), basic_units.end(), unit), basic_units.end());
	oracles.erase(std::remove(oracles.begin(), oracles.end(), unit), oracles.end());
	warp_prisms.erase(std::remove(warp_prisms.begin(), warp_prisms.end(), unit), warp_prisms.end());

	ArmyGroup::RemoveUnit(unit);
}

// returns true when group is close enough to add into main army
bool PiecewiseAttackArmyGroup::MobilizeNewUnits(Units units)
{
	if (Distance2D(Utility::MedianCenter(units), Utility::MedianCenter(all_units)) < 5)
		return true;
	for (const auto& unit : units)
	{
		if (mediator->GetAttackStatus(unit))
			continue;
		if (unit->weapon_cooldown == 0)
			mediator->AddUnitToAttackers(unit);

		mediator->SetUnitCommand(unit, A_MOVE, Utility::MedianCenter(all_units), CommandPriority::low);
		// TODO make sure units stay grouped up
	}
	return false;
}

void PiecewiseAttackArmyGroup::GroupUpNewUnits()
{
	// TODO check for enemies in range
	mediator->SetUnitsCommand(new_units, A_MOVE, Utility::MedianCenter(new_units), CommandPriority::low);

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


AttackLineResult PiecewiseAttackArmyGroup::AttackLine()
{
	if (all_units.size() == 0 && new_units.size() == 0 && basic_units.size() == 0 && standby_units.size() == 0)
	{
		return AttackLineResult::all_units_dead;
	}
	if (basic_units.size() == 0)
	{
		if (units_on_their_way.size() > 0)
		{
			for (const auto& unit : units_on_their_way[0])
			{
				AddUnit(unit);
			}
			units_on_their_way.erase(units_on_their_way.begin());
			concave_origin = Point2D(0, 0);
		}
		else if (new_units.size() > 0)
		{
			for (const auto& unit : new_units)
			{
				AddUnit(unit);
			}
		}
		else
		{
			return AttackLineResult::all_units_dead;
		}
		// TODO add a case for standby units
	}

	// micro special units
	// oracles
	if (oracles.size())
		OraclesDefendArmy(oracles, &attack_path, basic_units);

	return AttackArmyGroup::AttackLine(basic_units, concave_origin, default_range, &attack_path, limit_advance, 
		pre_prism_limit, warp_prisms, dispersion, unit_position_asignments, oracles, advancing, attack_threshold);
	
}

}