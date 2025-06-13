
#include <iostream>

#include "attack_army_group.h"
#include "cannon_rush_defense_army_group.h"
#include "defend_base_army_group.h"
#include "defend_line_army_group.h"
#include "defend_main_ramp_army_group.h"
#include "defend_third_zerg_army_group.h"
#include "deny_outer_base_army_group.h"
#include "door_guard_army_group.h"
#include "observer_scout_army_group.h"
#include "outside_control_army_group.h"
#include "scout_bases_army_group.h"
#include "simple_attack_army_group.h"

#include "oracle_harass_state_machine.h"

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
	unassigned_group = new ArmyGroup(mediator, { ALL_ARMY_UNITS });

	AddArmyGroup(new DefendBaseArmyGroup(mediator, mediator->GetStartLocation(), { ZEALOT, ADEPT, STALKER, SENTRY, IMMORTAL, COLOSSUS }, 0, 2));

	switch (mediator->GetEnemyRace())
	{
	case Race::Protoss:
		CreateProtossArmyTemplates();
		break;
	case Race::Zerg:
		mediator->SetDoorGuard();
		CreateZergArmyTemplates();
		break;
	case Race::Terran:
		CreateTerranArmyTemplates();
		break;
	case Race::Random:
		break;
	}
}

void ArmyManager::CreateProtossArmyTemplates()
{

}

void ArmyManager::CreateTerranArmyTemplates()
{
	std::vector<UNIT_TYPEID> base_denial_types = { STALKER };
	std::map<UNIT_TYPEID, uint16_t> base_denial_req;
	base_denial_req[STALKER] = 2;
	bool(sc2::ArmyManager:: * condition)() = &ArmyManager::EnemyHasExposedBase;
	ArmyTemplate<DenyOuterBaseArmyGroup>* base_denial = new ArmyTemplate<DenyOuterBaseArmyGroup>(base_denial_req, condition, 10, base_denial_types, 2, 2);
	army_templates.push_back(base_denial);


	std::vector<UNIT_TYPEID> observer_scout_types = { STALKER };
	std::map<UNIT_TYPEID, uint16_t> observer_scout_req;
	observer_scout_req[OBSERVER] = 1;
	ArmyTemplate<ObserverScoutArmyGroup>* observer_scout = new ArmyTemplate<ObserverScoutArmyGroup>(observer_scout_req, 10, observer_scout_types, 1, 1);
	army_templates.push_back(observer_scout);
}

void ArmyManager::CreateZergArmyTemplates()
{
	std::vector<UNIT_TYPEID> stalker_oracle_types = { STALKER, ORACLE };
	std::map<UNIT_TYPEID, uint16_t> stalker_oracle_req;
	stalker_oracle_req[STALKER] = 7;
	stalker_oracle_req[ORACLE] = 2;
	ArmyTemplate<AttackArmyGroup>* stalker_oracle = new ArmyTemplate<AttackArmyGroup>(stalker_oracle_req, 10, stalker_oracle_types, 25, 35);
	army_templates.push_back(stalker_oracle);

	std::vector<UNIT_TYPEID> zealot_run_by_types = { ZEALOT };
	std::map<UNIT_TYPEID, uint16_t> zealot_run_by_req;
	zealot_run_by_req[ZEALOT] = 10;
	ArmyTemplate<SimpleAttackArmyGroup>* zealot_run_by = new ArmyTemplate<SimpleAttackArmyGroup>(zealot_run_by_req, 10, zealot_run_by_types, 10, 20);
	army_templates.push_back(zealot_run_by);

	std::vector<UNIT_TYPEID> oracle_harass_types = { ORACLE };
	std::map<UNIT_TYPEID, uint16_t> oracle_harass_req;
	oracle_harass_req[ORACLE] = 2;
	ArmyTemplateStateMachine<OutsideControlArmyGroup, OracleHarassStateMachine>* oracle_harass = new ArmyTemplateStateMachine<OutsideControlArmyGroup, OracleHarassStateMachine>(oracle_harass_req, 20, oracle_harass_types, 2, 2);
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
	IArmyTemplate* template_to_create = nullptr;
	for (const auto& army_template : army_templates)
	{
		if (army_template->condition != nullptr)
		{
			bool(sc2::ArmyManager:: * condition)() = army_template->condition;
			if ((*this.*condition)() == false)
				continue;
		}

		bool all_req_units = true;
		for (const auto& type : army_template->required_units)
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
			if (template_to_create == nullptr || army_template->priority < template_to_create->priority)
			{
				template_to_create = army_template;
			}
		}
	}

	if (template_to_create != nullptr)
	{
		ArmyGroup* new_army = template_to_create->CreateArmyGroup(mediator);

		if (dynamic_cast<IArmyTemplateStateMachine*>(template_to_create))
		{
			StateMachine* new_state_machine = dynamic_cast<IArmyTemplateStateMachine*>(template_to_create)->CreateStateMachine(mediator);
			mediator->AddStateMachine(new_state_machine);
			new_state_machine->attached_army_group = new_army;
			if (dynamic_cast<OutsideControlArmyGroup*>(new_army))
			{
				dynamic_cast<OutsideControlArmyGroup*>(new_army)->state_machine = new_state_machine;
			}
			else
			{
				std::cerr << "Incorrect ArmyGroup type created with StateMachine in ArmyManager::CreateNewArmyGroups" << std::endl;
				mediator->LogMinorError();
			}
		}

		AddArmyGroup(new_army);
	}
}

void ArmyManager::AddArmyGroup(ArmyGroup* army)
{
	army_groups.push_back(army);
	BalanceUnits();
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
		if (scouring_map)
		{
			army_groups[i]->ScourMap();
		}
		else if (army_groups[i]->ready)
		{
			army_groups[i]->Run();
		}
		else
		{
			army_groups[i]->SetUp();
		}
	}
}

void ArmyManager::FindArmyGroupForUnit(const Unit* unit)
{
	std::vector<ArmyGroup*> possibles_groups;
	for (const auto& group : army_groups)
	{
		if (group->accept_new_units == false)
			continue;
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
			return ((double)(a->all_units.size() + a->new_units.size() + 1) / (double)(a->max_units)) < ((double)(b->all_units.size() + b->new_units.size() + 1) / (double)(b->max_units));
		}
		if (b->desired_units == 0)
		{
			if (a->desired_units > a->all_units.size() + a->new_units.size())
				return true;
			return ((double)(a->all_units.size() + a->new_units.size() + 1) / (double)(a->max_units)) < ((double)(b->all_units.size() + b->new_units.size() + 1) / (double)(b->max_units));
		}

		return ((double)(a->all_units.size() + a->new_units.size() + 1) / (double)(a->desired_units)) < ((double)(b->all_units.size() + b->new_units.size() + 1) / (double)(b->desired_units));
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
	scouring_map = true;
}

void ArmyManager::NexusStarted(Point2D pos) // TODO move to mediator
{
	AddArmyGroup(new DefendBaseArmyGroup(mediator, pos, { ZEALOT, ADEPT, STALKER, SENTRY, IMMORTAL, COLOSSUS }, 0, 3));
}

template<typename T>
void ArmyManager::RemoveArmyGroupOfType()
{
	for (int i = 0; i < army_groups.size(); i++)
	{
		if (dynamic_cast<T*>(army_groups[i]))
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
		if (dynamic_cast<DefendBaseArmyGroup*>(army_groups[i]) && dynamic_cast<DefendBaseArmyGroup*>(army_groups[i])->base_pos == pos)
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

bool ArmyManager::EnemyHasExposedBase()
{
	return FindExposedBase() != Point2D(0, 0);
}

Point2D ArmyManager::FindExposedBase()
{
	Units enemy_bases = mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ COMMAND_CENTER, ORBITAL, PLANETARY, NEXUS, HATCHERY, LAIR, HIVE }));
	for (const auto& base : enemy_bases)
	{
		if (Distance2D(base->pos, mediator->GetEnemyStartLocation()) < 2 ||
			Distance2D(base->pos, mediator->GetEnemyNaturalLocation()) < 2)
			continue;

		bool at_base = false;
		for (const auto& base_pos : mediator->GetAllBases())
		{
			if (Distance2D(base->pos, base_pos) < 2)
			{
				at_base = true;
				break;
			}
		}
		if (!at_base)
			continue;

		bool close_to_third = false;
		for (const auto& third_pos : mediator->GetPossibleEnemyThirdBaseLocations())
		{
			if (Distance2D(base->pos, third_pos) < 2)
			{
				close_to_third = true;
				break;
			}
		}
		if (!close_to_third)
			return base->pos;
	}
	return Point2D(0, 0);
}


}