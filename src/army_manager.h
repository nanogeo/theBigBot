#pragma once

#include "army_group.h"
#include "definitions.h"
#include "deny_outer_base_army_group.h"
#include "simple_attack_army_group.h"
#include "outside_control_army_group.h"
#include "attack_army_group.h"


namespace sc2
{
struct IArmyTemplate;

template<typename T>
struct ArmyTemplate;

class Mediator;

class ArmyManager
{
public:
	Mediator* mediator;
	ArmyGroup* unassigned_group = nullptr;
	std::vector<ArmyGroup*> army_groups;
	std::vector<ArmyGroup*> army_groups_to_delete;
	bool scouring_map = false;

	std::vector<IArmyTemplate*> army_templates;

	ArmyManager(Mediator* mediator);

	void SetUpInitialArmies();
	void CreateProtossArmyTemplates();
	void CreateTerranArmyTemplates();
	void CreateZergArmyTemplates();

	void CreateNewArmyGroups();

	void AddArmyGroup(ArmyGroup*);

	void RunArmyGroups();

	void FindArmyGroupForUnit(const Unit*);
	void OnUnitDestroyed(const Unit*);
	void BalanceUnits();

	void ScourMap();

	void NexusStarted(Point2D);
	template<typename T>
	void RemoveArmyGroupOfType();
	void RemoveDefenseGroupAt(Point2D);
	void DeleteArmyGroup(ArmyGroup*);
	void MarkArmyGroupForDeletion(ArmyGroup*);

	bool EnemyHasExposedBase();
	Point2D FindExposedBase();
	bool NoOngoingAttacks();

};

struct IArmyTemplate
{
	std::map<UNIT_TYPEID, uint16_t> required_units;
	bool(sc2::ArmyManager::* condition)() = nullptr;
	uint16_t priority = 0;
	virtual ArmyGroup* CreateArmyGroup(Mediator* mediator) = 0;
};

template<typename T>
struct ArmyTemplate : public IArmyTemplate
{
	std::vector<UNIT_TYPEID> unit_types;
	uint16_t desired_units = 0;
	uint16_t max_units = 0;
	ArmyTemplate() {};
	ArmyTemplate(std::map<UNIT_TYPEID, uint16_t> required_units, uint16_t priority, std::vector<UNIT_TYPEID> unit_types, 
		uint16_t desired_units, uint16_t max_units)
	{
		this->required_units = required_units;
		this->priority = priority;
		this->unit_types = unit_types;
		this->desired_units = desired_units;
		this->max_units = max_units;
	}
	ArmyTemplate(std::map<UNIT_TYPEID, uint16_t> required_units, bool(sc2::ArmyManager::* condition)(), 
		uint16_t priority, std::vector<UNIT_TYPEID> unit_types, uint16_t desired_units, uint16_t max_units)
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
	ArmyTemplateStateMachine(std::map<UNIT_TYPEID, uint16_t> required_units, uint16_t priority, std::vector<UNIT_TYPEID> unit_types,
		uint16_t desired_units, uint16_t max_units)
	{
		this->required_units = required_units;
		this->priority = priority;
		this->unit_types = unit_types;
		this->desired_units = desired_units;
		this->max_units = max_units;
	}
	ArmyTemplateStateMachine(std::map<UNIT_TYPEID, uint16_t> required_units, bool(sc2::ArmyManager::* condition)(), uint16_t priority, 
		std::vector<UNIT_TYPEID> unit_types, uint16_t desired_units, uint16_t max_units)
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