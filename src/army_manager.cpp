#pragma once
#include "army_manager.h"
#include "mediator.h"
#include "theBigBot.h"


namespace sc2 {

	ArmyManager::ArmyManager(Mediator* mediator)
{
	this->mediator = mediator;
}

void ArmyManager::SetUpInitialArmies()
{
	unassigned_group = new ArmyGroup(mediator, ArmyRole::none, ALL_ARMY_UNITS);

	switch (mediator->GetEnemyRace())
	{
	case Race::Protoss:
		CreateArmyGroup(ArmyRole::defend_main, { ZEALOT, ADEPT, STALKER, SENTRY }, 2, 4);
		break;
	case Race::Zerg:
		CreateArmyGroup(ArmyRole::defend_door, { ZEALOT, ADEPT, STALKER, SENTRY }, 1, 1);
		break;
	case Race::Terran:
		CreateArmyGroup(ArmyRole::defend_main, { ADEPT, STALKER, SENTRY }, 1, 4);
		break;
	case Race::Random:
		CreateArmyGroup(ArmyRole::defend_main, { ZEALOT, ADEPT, STALKER, SENTRY }, 1, 4);
		break;
	}
}

ArmyGroup* ArmyManager::CreateArmyGroup(ArmyRole role, std::vector<UNIT_TYPEID> unit_types, int desired_units, int max_units)
{
	ArmyGroup* army;
	switch (role)
	{
	case ArmyRole::outside_control:
		army = new ArmyGroup(mediator, role, unit_types);
		break;
	case ArmyRole::attack:
		army = new ArmyGroup(mediator, mediator->GetDirectAttackLine(), mediator->GetDirectAttackPath(), role, unit_types);
		break;
	case ArmyRole::pressure:
		army = new ArmyGroup(mediator, mediator->GetIndirectAttackLine(), mediator->GetIndirectAttackPath(), role, unit_types);
		break;
	case ArmyRole::defend_door:
		army = new ArmyGroup(mediator, role, unit_types);
		break;
	case ArmyRole::defend_third:
		army = new ArmyGroup(mediator, role, unit_types);
		break;
	case ArmyRole::defend_main:
		army = new ArmyGroup(mediator, role, unit_types);
		break;
	case ArmyRole::simple_attack:
		army = new ArmyGroup(mediator, mediator->GetAltAttackPath(), role, unit_types);
		break;
	case ArmyRole::scour:
		army = new ArmyGroup(mediator, role, unit_types);
		break;
	default:
		std::cerr << "Unknown ArmyRole in CreateArmyGroup" << std::endl;
		return NULL;
	}

	army->desired_units = desired_units;
	army->max_units = max_units;
	army_groups.push_back(army);
	BalanceUnits();
	return army;
}

void ArmyManager::RunArmyGroups()
{
	for (const auto& army_group : army_groups)
	{
		switch (army_group->role)
		{
		case ArmyRole::outside_control:
			break;
		case ArmyRole::attack:
		case ArmyRole::pressure:
			switch (mediator->GetEnemyRace())
			{
			case Race::Zerg:
				army_group->AttackLine(0, 6, ZERG_PRIO);
				break;
			case Race::Protoss:
				army_group->AttackLine(0, 6, PROTOSS_PRIO);
				break;
			case Race::Terran:
				army_group->AttackLine(2, 6, TERRAN_PRIO);
				break;
			default:
				std::cerr << "Unknown enemy race in RunArmyGroup" << std::endl;
			}
			break;
		case ArmyRole::scour:
			army_group->ScourMap();
			break;
		case ArmyRole::simple_attack:
			army_group->SimpleAttack();
			break;
		case ArmyRole::defend_door:
			army_group->DefendFrontDoor(mediator->agent->locations->natural_door_open, mediator->agent->locations->natural_door_closed);
			break;
		case ArmyRole::defend_third:
			army_group->DefendThirdBase(mediator->agent->locations->third_base_pylon_gap);
			break;
		case ArmyRole::defend_main:
			
			break;
		default:
			std::cerr << "Unknown ArmyRole in RunArmyGroup" << std::endl;
		}
	}
}

void ArmyManager::FindArmyGroupForUnit(const Unit* unit)
{
	std::vector<ArmyGroup*> possibles_groups;
	for (const auto& group : army_groups)
	{
		if (std::find(group->unit_types.begin(), group->unit_types.end(), unit->unit_type) == group->unit_types.end())
			continue;
		if (group->all_units.size() >= group->max_units)
			continue;
		possibles_groups.push_back(group);
	}

	if (possibles_groups.size() == 0)
	{
		unassigned_group->AddUnit(unit);
		return;
	}

	std::sort(possibles_groups.begin(), possibles_groups.end(),
		[](const ArmyGroup* a, const ArmyGroup* b) -> bool
	{
		return ((double)(a->all_units.size()) / (double)(a->desired_units)) > ((double)(b->all_units.size()) / (double)(b->desired_units));
	});

	possibles_groups[0]->AddNewUnit(unit);
}

void ArmyManager::OnUnitDestroyed(const Unit* unit)
{
	for (const auto& group : army_groups)
	{
		if (std::find(group->all_units.begin(), group->all_units.end(), unit) != group->all_units.end() ||
			std::find(group->new_units.begin(), group->new_units.end(), unit) != group->new_units.end())
			group->RemoveUnit(unit);
	}
}

void ArmyManager::BalanceUnits()
{
	Units unassigned = unassigned_group->all_units;

	for (const auto& unit : unassigned)
	{
		unassigned_group->RemoveUnit(unit);
		FindArmyGroupForUnit(unit);
	}

	if (unassigned_group->all_units.size() > 0)
		return;

	// shuffle other groups units if necessary
}

}