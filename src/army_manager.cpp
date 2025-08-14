
#include <iostream>

#include "attack_army_group.h"
#include "cannon_rush_defense_army_group.h"
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
	
void ArmyManager::CreateProtossArmyTemplates()
{
	std::vector<UNIT_TYPEID> pvp_basic_types = { STALKER, SENTRY, PRISM, IMMORTAL };
	std::map<UNIT_TYPEID, int> pvp_basic_req;
	pvp_basic_req[STALKER] = 10;
	pvp_basic_req[PRISM] = 1;
	pvp_basic_req[IMMORTAL] = 3;
	pvp_basic_req[SENTRY] = 2;
	bool(sc2::ArmyManager:: * condition)() = &ArmyManager::NoLossesForOneMinute;
	ArmyTemplate<AttackArmyGroup>* basic_army = new ArmyTemplate<AttackArmyGroup>(pvp_basic_req, condition, 10, pvp_basic_types, 20, 30);
	army_templates.push_back(basic_army);

}

void ArmyManager::CreateTerranArmyTemplates()
{
	std::vector<UNIT_TYPEID> base_denial_types = { STALKER };
	std::map<UNIT_TYPEID, int> base_denial_req;
	base_denial_req[STALKER] = 2;
	bool(sc2::ArmyManager:: * condition)() = &ArmyManager::EnemyHasExposedBase;
	ArmyTemplate<DenyOuterBaseArmyGroup>* base_denial = new ArmyTemplate<DenyOuterBaseArmyGroup>(base_denial_req, condition, 10, base_denial_types, 2, 2);
	army_templates.push_back(base_denial);


	std::vector<UNIT_TYPEID> observer_scout_types = { OBSERVER };
	std::map<UNIT_TYPEID, int> observer_scout_req;
	observer_scout_req[OBSERVER] = 1;
	ArmyTemplate<ObserverScoutArmyGroup>* observer_scout = new ArmyTemplate<ObserverScoutArmyGroup>(observer_scout_req, 10, observer_scout_types, 1, 1);
	army_templates.push_back(observer_scout);

	std::vector<UNIT_TYPEID> zealot_run_by_types = { ZEALOT };
	std::map<UNIT_TYPEID, int> zealot_run_by_req;
	zealot_run_by_req[ZEALOT] = 10;
	ArmyTemplate<SimpleAttackArmyGroup>* zealot_run_by = new ArmyTemplate<SimpleAttackArmyGroup>(zealot_run_by_req, 10, zealot_run_by_types, 10, 20);
	army_templates.push_back(zealot_run_by);
}

void ArmyManager::CreateZergArmyTemplates()
{
	std::vector<UNIT_TYPEID> pvz_basic_types = { STALKER, PRISM, IMMORTAL };
	std::map<UNIT_TYPEID, int> pvz_basic_req;
	pvz_basic_req[STALKER] = 10;
	pvz_basic_req[PRISM] = 1;
	pvz_basic_req[IMMORTAL] = 3;
	bool(sc2::ArmyManager:: * condition)() = &ArmyManager::NoLossesForOneMinute;
	ArmyTemplate<AttackArmyGroup>* basic_army = new ArmyTemplate<AttackArmyGroup>(pvz_basic_req, condition, 10, pvz_basic_types, 20, 30);
	army_templates.push_back(basic_army);

	std::vector<UNIT_TYPEID> stalker_oracle_types = { STALKER, ORACLE, IMMORTAL, COLOSSUS, PRISM };
	std::map<UNIT_TYPEID, int> stalker_oracle_req;
	stalker_oracle_req[STALKER] = 7;
	stalker_oracle_req[ORACLE] = 2;
	ArmyTemplate<AttackArmyGroup>* stalker_oracle = new ArmyTemplate<AttackArmyGroup>(stalker_oracle_req, 10, stalker_oracle_types, 25, 35);
	army_templates.push_back(stalker_oracle);

	std::vector<UNIT_TYPEID> zealot_run_by_types = { ZEALOT };
	std::map<UNIT_TYPEID, int> zealot_run_by_req;
	zealot_run_by_req[ZEALOT] = 10;
	ArmyTemplate<SimpleAttackArmyGroup>* zealot_run_by = new ArmyTemplate<SimpleAttackArmyGroup>(zealot_run_by_req, 10, zealot_run_by_types, 10, 20);
	army_templates.push_back(zealot_run_by);

	std::vector<UNIT_TYPEID> oracle_harass_types = { ORACLE };
	std::map<UNIT_TYPEID, int> oracle_harass_req;
	oracle_harass_req[ORACLE] = 2;
	bool(sc2::ArmyManager:: * oracle_condition)() = &ArmyManager::OracleHarassCondition;
	ArmyTemplateStateMachine<OutsideControlArmyGroup, OracleHarassStateMachine>* oracle_harass = new ArmyTemplateStateMachine<OutsideControlArmyGroup, OracleHarassStateMachine>(oracle_harass_req, oracle_condition, 20, oracle_harass_types, 2, 2);
	army_templates.push_back(oracle_harass);
}


bool ArmyManager::EnemyHasExposedBase()
{
	return FindExposedBase() != Point2D(0, 0);
}

bool ArmyManager::NoLossesForOneMinute()
{
	return mediator->GetLossesSince(mediator->GetCurrentTime() - 60).mineral_cost == 0;
}

Point2D ArmyManager::FindExposedBase() const
{
	Units enemy_bases = mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ COMMAND_CENTER, ORBITAL, PLANETARY, NEXUS, HATCHERY, LAIR, HIVE }));
	for (const auto& base : enemy_bases)
	{
		if (Distance2D(base->pos, mediator->GetEnemyStartLocation()) < VERY_CLOSE_RANGE ||
			Distance2D(base->pos, mediator->GetEnemyNaturalLocation()) < VERY_CLOSE_RANGE)
			continue;

		bool at_base = false;
		for (const auto& base_pos : mediator->GetAllBases())
		{
			if (Distance2D(base->pos, base_pos) < VERY_CLOSE_RANGE)
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
			if (Distance2D(base->pos, third_pos) < VERY_CLOSE_RANGE)
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

bool ArmyManager::OracleHarassCondition()
{
	GameState game_state = mediator->GetGameState();
	switch (game_state.game_state_worker)
	{
	case GameStateWorker::even:
	case GameStateWorker::slightly_less:
		if (game_state.good_worker_intel && mediator->GetWorstOngoingAttackValue() > OGA_LOSING_SLIGHTLY)
			return true;
		break;
	case GameStateWorker::much_less:
		if (mediator->GetWorstOngoingAttackValue() > OGA_LOSING_SLIGHTLY)
			return true;
		break;
	}
	return false;
}

ArmyManager::ArmyManager(Mediator* mediator)
{
	this->mediator = mediator;
}

void ArmyManager::DisplayArmyGroups() const
{
	std::string army_info = "Armies:\n";
	std::vector<ArmyGroup*> groups = { unassigned_group };
	groups.insert(groups.begin() + 1, army_groups.begin(), army_groups.end());
	for (int i = 0; i < groups.size(); i++)
	{
		army_info += groups[i]->ToString() + "\n";
		if (groups[i]->all_units.size() + groups[i]->new_units.size() > 0)
		{
			army_info += "  Units: " + std::to_string(groups[i]->desired_units) + "/" + std::to_string(groups[i]->max_units) + "\n    ";
			std::map<UNIT_TYPEID, int> unit_totals;
			for (const auto& unit : groups[i]->all_units)
			{
				if (unit_totals.count(unit->unit_type) > 0)
					unit_totals[unit->unit_type] += 1;
				else
					unit_totals[unit->unit_type] = 1;
			}
			for (const auto& unit : groups[i]->new_units)
			{
				if (unit_totals.count(unit->unit_type) > 0)
					unit_totals[unit->unit_type] += 1;
				else
					unit_totals[unit->unit_type] = 1;
			}

			int num_per_line = 0;
			for (const auto& type : ALL_ARMY_UNITS)
			{
				if (unit_totals.count(type) > 0)
				{
					if (num_per_line > 3)
					{
						army_info += "\n    ";
						num_per_line = 0;
					}
					army_info += UnitTypeToName(type);
					army_info += "-" + std::to_string(unit_totals[type]) + ", ";
					num_per_line++;
				}
			}
			army_info += "\n";
		}
	}
	mediator->DebugText(army_info, Point2D(.8f, .3f), Color(255, 255, 255), 20);
}


void ArmyManager::SetUpInitialArmies()
{
	unassigned_group = new DefenseArmyGroup(mediator);

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

void ArmyManager::CreateNewArmyGroups()
{
	std::map<UNIT_TYPEID, int> extra_units;

	for (const auto& unit : unassigned_group->all_units)
	{
		if (unit->unit_type == ORACLE && unit->energy < ENERGY_COST_PULSAR_BEAM) // ignore oracles with little energy TODO same with sentries/templar
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
			if (unit->unit_type == ORACLE && unit->energy < ENERGY_COST_PULSAR_BEAM) // ignore oracles with little energy TODO same with sentries/templar
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
			new_state_machine->SetAttachedArmyGroup(new_army);
			if (dynamic_cast<OutsideControlArmyGroup*>(new_army))
			{
				dynamic_cast<OutsideControlArmyGroup*>(new_army)->SetStateMachine(new_state_machine);
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
	if (army == nullptr)
		mediator->LogCallStack();
	army_groups.push_back(army);
	BalanceUnits();
	std::cerr << "Delete army group " << army->ToString() << std::endl;
}

const std::vector<ArmyGroup*>& ArmyManager::GetArmyGroups() const
{
	return army_groups;
}

void ArmyManager::RunArmyGroups()
{
	if (scouring_map)
		unassigned_group->ScourMap();
	else
		unassigned_group->Run();

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

void ArmyManager::FindArmyGroupForUnit(const Unit* unit) const
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
		{
			group->RemoveUnit(unit);
			BalanceUnits();
		}
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

void ArmyManager::DeleteArmyGroup(ArmyGroup* army)
{
	Units unassigned_units;
	unassigned_units.insert(unassigned_units.end(), army->all_units.begin(), army->all_units.end());
	unassigned_units.insert(unassigned_units.end(), army->new_units.begin(), army->new_units.end());
	army_groups.erase(std::remove(army_groups.begin(), army_groups.end(), army), army_groups.end());
	std::cerr << "Delete army group " << army->ToString() << std::endl;
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