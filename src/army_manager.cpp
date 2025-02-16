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
		CreateArmyGroup(ArmyRole::defend_base, { ZEALOT, ADEPT, STALKER, SENTRY }, 0, 3);
		break;
	case Race::Zerg:
		CreateArmyGroup(ArmyRole::defend_door, { ZEALOT, ADEPT, STALKER, SENTRY }, 1, 1);
		CreateArmyGroup(ArmyRole::defend_base, { ZEALOT, ADEPT, STALKER, SENTRY }, 0, 3);
		break;
	case Race::Terran:
		CreateArmyGroup(ArmyRole::defend_base, { ADEPT, STALKER, SENTRY }, 0, 3);
		break;
	case Race::Random:
		CreateArmyGroup(ArmyRole::defend_base, { ZEALOT, ADEPT, STALKER, SENTRY }, 0, 3);
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
	case ArmyRole::defend_base:
		army = new ArmyGroup(mediator, mediator->GetMostRecentBuilding(NEXUS)->pos, role, unit_types);
		break;
	case ArmyRole::observer_scout:
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
	for (int i = 0; i < army_groups.size(); i++)
	{
		if (std::find(army_groups_to_delete.begin(), army_groups_to_delete.end(), army_groups[i]) != army_groups_to_delete.end())
		{
			army_groups_to_delete.erase(std::remove(army_groups_to_delete.begin(), army_groups_to_delete.end(), army_groups[i]), army_groups_to_delete.end());
			DeleteArmyGroup(army_groups[i]);
			army_groups.erase(army_groups.begin() + i);
			i--;
			BalanceUnits();
			continue;
		}
		switch (army_groups[i]->role)
		{
		case ArmyRole::outside_control:
			army_groups[i]->OutsideControl();
			break;
		case ArmyRole::attack:
		case ArmyRole::pressure:
			switch (mediator->GetEnemyRace())
			{
			case Race::Zerg:
				army_groups[i]->AttackLine(0, 6, ZERG_PRIO);
				break;
			case Race::Protoss:
				army_groups[i]->AttackLine(0, 6, PROTOSS_PRIO);
				break;
			case Race::Terran:
				army_groups[i]->AttackLine(2, 6, TERRAN_PRIO);
				break;
			default:
				std::cerr << "Unknown enemy race in RunArmyGroup" << std::endl;
			}
			break;
		case ArmyRole::scour:
			army_groups[i]->ScourMap();
			break;
		case ArmyRole::simple_attack:
			army_groups[i]->SimpleAttack();
			break;
		case ArmyRole::defend_door:
			army_groups[i]->DefendFrontDoor(mediator->agent->locations->natural_door_open, mediator->agent->locations->natural_door_closed);
			break;
		case ArmyRole::defend_third:
			army_groups[i]->DefendThirdBase(mediator->agent->locations->third_base_pylon_gap);
			break;
		case ArmyRole::defend_main:
			
			break;
		case ArmyRole::defend_base:
			army_groups[i]->DefendLocation();
			break;
		case ArmyRole::observer_scout:
			army_groups[i]->ObserverScout();
			break;
		default:
			std::cerr << "Unknown ArmyRole in RunArmyGroup" << std::endl;
			break;
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
		if (a->desired_units == 0)
		{
			if (b->desired_units > b->all_units.size() + b->new_units.size())
				return false;
			return ((double)(a->all_units.size() + a->new_units.size()) / (double)(a->max_units)) < ((double)(b->all_units.size() + b->new_units.size()) / (double)(b->max_units));
		}
		if (b->desired_units == 0)
		{
			if (a->desired_units > a->all_units.size() + a->new_units.size())
				return true;
			return ((double)(a->all_units.size() + a->new_units.size()) / (double)(a->max_units)) < ((double)(b->all_units.size() + b->new_units.size()) / (double)(b->max_units));
		}

		return ((double)(a->all_units.size() + a->new_units.size()) / (double)(a->desired_units)) < ((double)(b->all_units.size() + b->new_units.size()) / (double)(b->desired_units));
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


	// shuffle other groups units if necessary
	for (const auto& group : army_groups)
	{
		Units extra = group->GetExtraUnits();
		for (const auto& unit : extra)
		{
			group->RemoveUnit(unit);
			FindArmyGroupForUnit(unit);
		}
	}
}

void ArmyManager::ScourMap()
{
	for (const auto& army_group : army_groups)
	{
		army_group->role = ArmyRole::scour;
	}
}

void ArmyManager::NexusStarted()
{
	CreateArmyGroup(ArmyRole::defend_base, { ZEALOT, ADEPT, STALKER, SENTRY }, 0, 3);
}

void ArmyManager::RemoveArmyGroupWithRole(ArmyRole role)
{
	for (int i = 0; i < army_groups.size(); i++)
	{
		if (army_groups[i]->role == role)
		{
			DeleteArmyGroup(army_groups[i]);
			i--;
		}
	}
	BalanceUnits();
}

void ArmyManager::DeleteArmyGroup(ArmyGroup* army)
{
	Units unassigned_units;
	unassigned_units.insert(unassigned_units.end(), army->all_units.begin(), army->all_units.end());
	unassigned_units.insert(unassigned_units.end(), army->new_units.begin(), army->new_units.end());
	delete army;

	for (const auto& unit : unassigned_units)
	{
		unassigned_group->AddUnit(unit);
	}
}

void ArmyManager::MarkArmyGroupForDeletion(ArmyGroup* army_group)
{
	army_groups_to_delete.push_back(army_group);
}

}