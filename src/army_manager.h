#pragma once

#include "army_group.h"
#include "definitions.h"
#include "deny_outer_base_army_group.h"
#include "simple_attack_army_group.h"
#include "outside_control_army_group.h"
#include "attack_army_group.h"
#include "defense_army_group.h"


namespace sc2
{
struct IArmyTemplate;

template<typename T>
struct ArmyTemplate;

class Mediator;

class ArmyManager
{
private:
	Mediator* mediator;
	DefenseArmyGroup* unassigned_group = nullptr;
	std::vector<ArmyGroup*> army_groups;
	std::vector<ArmyGroup*> army_groups_to_delete;
	bool scouring_map = false;

	std::vector<IArmyTemplate*> army_templates;

	void CreateProtossArmyTemplates();
	void CreateTerranArmyTemplates();
	void CreateZergArmyTemplates();

	bool EnemyHasExposedBase();
	bool NoLossesForOneMinute();
	Point2D FindExposedBase() const;
	bool OracleHarassCondition();
public:
	ArmyManager(Mediator* mediator);

	void DisplayArmyGroups() const;
	void SetUpInitialArmies();

	void CreateNewArmyGroups();

	void AddArmyGroup(ArmyGroup*);
	const std::vector<ArmyGroup*>& GetArmyGroups() const;

	void RunArmyGroups();

	void FindArmyGroupForUnit(const Unit*) const;
	void OnUnitDestroyed(const Unit*);
	void BalanceUnits();

	void ScourMap();

	template<typename T>
	void RemoveArmyGroupOfType();
	void DeleteArmyGroup(ArmyGroup*);
	void MarkArmyGroupForDeletion(ArmyGroup*);

	void LogDebugInfo();
};

struct IArmyTemplate
{
	std::map<UNIT_TYPEID, int> required_units;
	bool(sc2::ArmyManager::* condition)() = nullptr;
	int priority = 0; // TODO change to enum
	virtual ArmyGroup* CreateArmyGroup(Mediator* mediator) = 0;
};

template<typename T>
struct ArmyTemplate : public IArmyTemplate
{
	std::vector<UNIT_TYPEID> unit_types;
	int desired_units = 0;
	int max_units = 0;
	ArmyTemplate() {};
	ArmyTemplate(std::map<UNIT_TYPEID, int> required_units, int priority, std::vector<UNIT_TYPEID> unit_types,
		int desired_units, int max_units)
	{
		this->required_units = required_units;
		this->priority = priority;
		this->unit_types = unit_types;
		this->desired_units = desired_units;
		this->max_units = max_units;
	}
	ArmyTemplate(std::map<UNIT_TYPEID, int> required_units, bool(sc2::ArmyManager::* condition)(),
		int priority, std::vector<UNIT_TYPEID> unit_types, int desired_units, int max_units)
	{
		this->required_units = required_units;
		this->priority = priority;
		this->condition = condition;
		this->unit_types = unit_types;
		this->desired_units = desired_units;
		this->max_units = max_units;
	}
	ArmyGroup* CreateArmyGroup(Mediator* mediator) override
	{
		return new T(mediator, this);
	}
};

struct IArmyTemplateStateMachine : ArmyTemplate<OutsideControlArmyGroup>
{
	virtual StateMachine* CreateStateMachine(Mediator* mediator) = 0;
};


template<typename T, typename U>
struct ArmyTemplateStateMachine : public IArmyTemplateStateMachine
{
	ArmyTemplateStateMachine(std::map<UNIT_TYPEID, int> required_units, int priority, std::vector<UNIT_TYPEID> unit_types,
		int desired_units, int max_units)
	{
		this->required_units = required_units;
		this->priority = priority;
		this->unit_types = unit_types;
		this->desired_units = desired_units;
		this->max_units = max_units;
	}
	ArmyTemplateStateMachine(std::map<UNIT_TYPEID, int> required_units, bool(sc2::ArmyManager::* condition)(), int priority,
		std::vector<UNIT_TYPEID> unit_types, int desired_units, int max_units)
	{
		this->required_units = required_units;
		this->priority = priority;
		this->condition = condition;
		this->unit_types = unit_types;
		this->desired_units = desired_units;
		this->max_units = max_units;
	}
	ArmyGroup* CreateArmyGroup(Mediator* mediator) override
	{
		return new T(mediator, this);
	}
	StateMachine* CreateStateMachine(Mediator* mediator)
	{
		return new U(mediator, this);
	}
};

}