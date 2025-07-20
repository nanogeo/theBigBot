#pragma once

#include "utility.h"
#include "path_manager.h"
#include "finite_state_machine.h"
#include "definitions.h"



namespace sc2
{

class ArmyManager;
class Mediator;

struct UnitDanger
{
	// 1: lethal damage, 2: damage value* > 40, 3: damage > 0
	// TODO make enum
	int unit_prio = 4;
	float damage_value = 0;
	UnitDanger() {}
	UnitDanger(int unit_prio, float damage_value)
	{
		this->unit_prio = unit_prio;
		this->damage_value = damage_value;
	}
	// is this higher prio than rhs
	bool operator<(const UnitDanger& rhs) const
	{
		if (unit_prio == rhs.unit_prio)
			return damage_value > rhs.damage_value;
		return unit_prio < rhs.unit_prio;
	}
	// is this lower prio than rhs
	bool operator>(const UnitDanger& rhs) const
	{
		if (unit_prio == rhs.unit_prio)
			return damage_value < rhs.damage_value;
		return unit_prio > rhs.unit_prio;
	}
};

struct PrismCargo
{
	const Unit* prism = nullptr;
	bool confirmed = false;
	PrismCargo() {};
	PrismCargo(const Unit* prism)
	{
		this->prism = prism;
	}
};

class ArmyGroup
{
	friend ArmyManager;
protected:
	Mediator* mediator = nullptr;

	Units all_units;
	Units new_units;

	bool accept_new_units = true;
	std::vector<UNIT_TYPEID> unit_types;
	int desired_units = 0;
	int max_units = 100;
	bool ready = false;

	// oracles TODO move to interface of other subclass
	std::map<const Unit*, float> time_last_attacked;
	std::map<const Unit*, bool> has_attacked;
	std::map<const Unit*, uint64_t> target;

public:
	ArmyGroup(Mediator* mediator)
	{
		this->mediator = mediator;
	}
	ArmyGroup(Mediator* mediator, std::vector<UNIT_TYPEID> unit_types)
	{
		this->mediator = mediator;
		this->unit_types = unit_types;
	}
	~ArmyGroup();

	virtual void SetUp() 
	{
		ready = true;
	};
	virtual void Run() {};
	virtual void ScourMap();
	virtual std::string ToString() const
	{
		return "Army group";
	}

	virtual void AddUnit(const Unit* unit);
	virtual void AddNewUnit(const Unit* unit);
	virtual void RemoveUnit(const Unit* unit);
	Units GetExtraUnits() const;
	int GetDesiredUnits() const;
	void AddDesiredUnits(int);
	void AddMaxUnits(int);

	void OnUnitDamagedListener(const Unit*, float, float);
	void OnUnitDestroyedListener(const Unit*);


};

}