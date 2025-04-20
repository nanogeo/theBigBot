
#include <iostream>

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
		CreateArmyGroup(ArmyRole::defend_base, { ZEALOT, ADEPT, STALKER, SENTRY, IMMORTAL, COLOSSUS }, 0, 3);
		CreateProtossArmyTemplates();
		break;
	case Race::Zerg:
		CreateArmyGroup(ArmyRole::defend_door, { ZEALOT, ADEPT, STALKER, SENTRY }, 1, 1);
		CreateArmyGroup(ArmyRole::defend_base, { ZEALOT, ADEPT, STALKER, SENTRY, IMMORTAL, COLOSSUS }, 0, 3);
		CreateZergArmyTemplates();
		break;
	case Race::Terran:
		CreateArmyGroup(ArmyRole::defend_base, { ADEPT, STALKER, SENTRY, IMMORTAL, COLOSSUS }, 0, 3);
		CreateTerranArmyTemplates();
		break;
	case Race::Random:
		CreateArmyGroup(ArmyRole::defend_base, { ZEALOT, ADEPT, STALKER, SENTRY, IMMORTAL, COLOSSUS }, 0, 3);
		break;
	}
}

void ArmyManager::CreateProtossArmyTemplates()
{

}

void ArmyManager::CreateTerranArmyTemplates()
{

}

void ArmyManager::CreateZergArmyTemplates()
{
	std::map<UNIT_TYPEID, int> stalker_oracle_req;
	stalker_oracle_req[STALKER] = 7;
	stalker_oracle_req[ORACLE] = 2;
	stalker_oracle_req[CARRIER] = 0;
	ArmyTemplate stalker_oracle = ArmyTemplate(stalker_oracle_req, 10, ArmyRole::pressure, 15, 25);
	army_templates.push_back(stalker_oracle);

	std::map<UNIT_TYPEID, int> zealot_double_prong_req;
	zealot_double_prong_req[ZEALOT] = 10;
	ArmyTemplate zealot_double_prong = ArmyTemplate(zealot_double_prong_req, 10, ArmyRole::simple_attack, 12, 20);
	army_templates.push_back(zealot_double_prong);

	std::map<UNIT_TYPEID, int> oracle_harass_req;
	oracle_harass_req[ORACLE] = 2;
	ArmyTemplate oracle_harass = ArmyTemplate(oracle_harass_req, 20, ArmyRole::oracle_harass, 2, 2);
	army_templates.push_back(oracle_harass);
}

void ArmyManager::CreateNewArmyGroups()
{
	std::map<UNIT_TYPEID, int> extra_units;

	for (const auto& unit : unassigned_group->all_units)
	{
		if (unit->unit_type == ORACLE && unit->energy < 20) // ignore oracles with little energy TODO same with sentries/templar
			continue;
		if (extra_units.find(unit->unit_type) != extra_units.end())
		{
			extra_units[unit->unit_type]++;
		}
		else
		{
			extra_units[unit->unit_type] = 1;
		}
	}


	for (const auto& group : army_groups)
	{
		Units extras = group->GetExtraUnits();
		for (const auto& unit : extras)
		{
			if (unit->unit_type == ORACLE && unit->energy < 20) // ignore oracles with little energy TODO same with sentries/templar
				continue;
			if (extra_units.find(unit->unit_type) != extra_units.end())
			{
				extra_units[unit->unit_type]++;
			}
			else
			{
				extra_units[unit->unit_type] = 1;
			}
		}
	}
	const ArmyTemplate* template_to_create = nullptr;
	for (const auto& army_template : army_templates)
	{
		bool all_req_units = true;
		for (const auto& type : army_template.required_units)
		{
			if (type.second > 0 && (extra_units.find(type.first) == extra_units.end() || extra_units[type.first] < type.second))
			{
				// not all required units found
				all_req_units = false;
				continue;
			}
		}
		if (all_req_units)
		{
			if (template_to_create == nullptr || army_template.priority < template_to_create->priority)
			{
				template_to_create = &army_template;
			}
		}
	}

	if (template_to_create != nullptr)
	{
		// create new army with template
		std::vector<UNIT_TYPEID> unit_types;
		for (const auto& type : template_to_create->required_units)
		{
			unit_types.push_back(type.first);
		}
		CreateArmyGroup(template_to_create->role, unit_types, template_to_create->desired_units, template_to_create->max_units);
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
	case ArmyRole::defend_outer:
		army = new ArmyGroup(mediator, mediator->GetLocation(NEXUS, 2), mediator->GetNaturalLocation(), role, unit_types);
		break;
	case ArmyRole::observer_scout:
		army = new ArmyGroup(mediator, role, unit_types);
		break;
	case ArmyRole::oracle_harass:
		army = new ArmyGroup(mediator, ArmyRole::outside_control, unit_types);
		mediator->StartOracleHarassStateMachine(army);
		break;
	default:
		std::cerr << "Unknown ArmyRole in CreateArmyGroup" << std::endl;
		return nullptr;
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
				army_groups[i]->AttackLine(0, 8, PROTOSS_PRIO);
				break;
			case Race::Terran:
				army_groups[i]->AttackLine(.2f, 7, TERRAN_PRIO);
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
		case ArmyRole::defend_outer:
			army_groups[i]->DefendLine();
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
		if (army_group->state_machine)
		{
			mediator->RemoveStateMachine(army_group->state_machine);
			army_group->state_machine = nullptr;
		}
	}
}

void ArmyManager::NexusStarted()
{
	CreateArmyGroup(ArmyRole::defend_base, { ZEALOT, ADEPT, STALKER, SENTRY, IMMORTAL, COLOSSUS, VOID_RAY, CARRIER, TEMPEST }, 0, 3);
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

void ArmyManager::RemoveDefenseGroupAt(Point2D pos)
{
	for (int i = 0; i < army_groups.size(); i++)
	{
		if (army_groups[i]->role == ArmyRole::defend_base && army_groups[i]->target_pos == pos)
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
	army_groups.erase(std::remove(army_groups.begin(), army_groups.end(), army), army_groups.end());
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