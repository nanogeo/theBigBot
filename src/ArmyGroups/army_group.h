#pragma once

#include "utility.h"
#include "path_manager.h"
#include "finite_state_machine.h"
#include "definitions.h"



namespace sc2
{

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
public:
	Mediator* mediator = nullptr;

	Units all_units;
	Units new_units;

	bool accept_new_units = true;
	std::vector<UNIT_TYPEID> unit_types;
	uint16_t desired_units = 0;
	uint16_t max_units = 100;
	bool ready = false;

	// oracles TODO move to interface of other subclass
	std::map<const Unit*, float> time_last_attacked;
	std::map<const Unit*, bool> has_attacked;
	std::map<const Unit*, uint64_t> target;

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
	virtual std::string ToString()
	{
		return "Army group";
	}

	virtual void AddUnit(const Unit* unit);
	virtual void AddNewUnit(const Unit* unit);
	virtual void RemoveUnit(const Unit* unit);
	Units GetExtraUnits();

	
	/*
	std::vector<Point2D> FindConcave(Point2D, Point2D, int, float, float, float);
	std::vector<Point2D> FindConcaveWithPrism(Point2D, Point2D, int, int, float, float, float, std::vector<Point2D>&);
	std::vector<Point2D> FindConcaveFromBack(Point2D, Point2D, int, float, float);
	bool TestSwap(Point2D, Point2D, Point2D, Point2D);
	std::map<const Unit*, Point2D> AssignUnitsToPositions(Units, std::vector<Point2D>);
	void PickUpUnits(std::map<const Unit*, int>);
	void DodgeShots();

	void FindStalkerPositions(std::map<const Unit*, Point2D>&, std::map<const Unit*, Point2D>&, float, float);

	int AttackLine(float, float, std::vector<std::vector<UNIT_TYPEID>>, bool = false);
	int AttackLine(Units, float, float, std::vector<std::vector<UNIT_TYPEID>>, bool = false);
	void OraclesDefendArmy(Units);

	bool FindUnitPositions(Units, Units, float, float, Point2D);
	void FindReadyUnits(Units, Units&, Units&);
	std::vector<std::pair<const Unit*, UnitDanger>> MicroNonReadyUnits(Units);
	void MicroWarpPrisms(std::vector<std::pair<const Unit*, UnitDanger>>);
	Point2D FindLimitToAdvance();
	*/

	void OnUnitDamagedListener(const Unit*, float, float);
	void OnUnitDestroyedListener(const Unit*);


};

}