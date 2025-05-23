#pragma once

#include "army_group.h"
#include "definitions.h"


namespace sc2
{

struct ArmyTemplate;

class Mediator;

class ArmyManager
{
public:
	Mediator* mediator;
	ArmyGroup* unassigned_group = nullptr;
	std::vector<ArmyGroup*> army_groups;
	std::vector<ArmyGroup*> army_groups_to_delete;

	std::vector<ArmyTemplate> army_templates;

	ArmyManager(Mediator* mediator);

	void SetUpInitialArmies();
	void CreateProtossArmyTemplates();
	void CreateTerranArmyTemplates();
	void CreateZergArmyTemplates();

	void CreateNewArmyGroups();

	ArmyGroup* CreateArmyGroup(ArmyRole, std::vector<UNIT_TYPEID>, int, int);
	void RunArmyGroups();

	void FindArmyGroupForUnit(const Unit*);
	void OnUnitDestroyed(const Unit*);
	void BalanceUnits();

	void ScourMap();

	void NexusStarted();
	void RemoveArmyGroupWithRole(ArmyRole);
	void RemoveDefenseGroupAt(Point2D);
	void DeleteArmyGroup(ArmyGroup*);
	void MarkArmyGroupForDeletion(ArmyGroup*);

	bool EnemyHasExposedBase();
	Point2D FindExposedBase();
};

struct ArmyTemplate
{
	std::map<UNIT_TYPEID, int> required_units;
	bool(sc2::ArmyManager::* condition)() = nullptr;
	int priority;
	ArmyRole role;
	int desired_units;
	int max_units;
	ArmyTemplate(std::map<UNIT_TYPEID, int> required_units, int priority, ArmyRole role, int desired_units, int max_units)
	{
		this->required_units = required_units;
		this->priority = priority;
		this->role = role;
		this->desired_units = desired_units;
		this->max_units = max_units;
	}
	ArmyTemplate(std::map<UNIT_TYPEID, int> required_units, int priority, ArmyRole role, int desired_units, int max_units, bool(sc2::ArmyManager::* condition)())
	{
		this->required_units = required_units;
		this->priority = priority;
		this->role = role;
		this->desired_units = desired_units;
		this->max_units = max_units;
		this->condition = condition;
	}
};

}