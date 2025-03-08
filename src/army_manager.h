#pragma once

#include "army_group.h"
#include "definitions.h"


namespace sc2
{

struct ArmyTemplate
{
	std::map<UNIT_TYPEID, int> required_units;
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
		this->required_units = required_units;
	}
};

class Mediator;

class ArmyManager
{
public:
	Mediator* mediator;
	ArmyGroup* unassigned_group;
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
	void DeleteArmyGroup(ArmyGroup*);
	void MarkArmyGroupForDeletion(ArmyGroup*);
};

}