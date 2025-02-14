#pragma once

#include "army_group.h"
#include "definitions.h"


namespace sc2
{

class Mediator;

class ArmyManager
{
public:
	Mediator* mediator;
	ArmyGroup* unassigned_group;
	std::vector<ArmyGroup*> army_groups;

	ArmyManager(Mediator* mediator);

	void SetUpInitialArmies();

	ArmyGroup* CreateArmyGroup(ArmyRole, std::vector<UNIT_TYPEID>, int, int);
	void RunArmyGroups();

	void FindArmyGroupForUnit(const Unit*);
	void OnUnitDestroyed(const Unit*);
	void BalanceUnits();

	void ScourMap();

	void NexusStarted();
	void RemoveArmyGroupWithRole(ArmyRole);
	void DeleteArmyGroup(ArmyGroup*);
};

}