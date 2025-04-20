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
	ArmyRole role;
	StateMachine* state_machine = nullptr;

	Units all_units;
	Units zealots;
	Units stalkers;
	Units adepts;
	Units sentries;
	Units high_templar;
	Units dark_templar;
	Units archons;
	Units immortals;
	Units collossi;
	Units disrupter;
	Units observers;
	Units warp_prisms;
	Units phoenixes;
	Units void_rays;
	Units oracles;
	Units carriers;
	Units tempests;

	Units new_units;

	Units possibly_confused_units;

	std::vector<Point2D> attack_path;
	int current_attack_index = 0;
	int high_ground_index = 0;
	PathManager attack_path_line;
	LineSegment* defense_line = nullptr;
	Point2D target_pos;

	bool advancing = true;
	Point2D concave_origin = Point2D(0, 0);
	std::map<const Unit*, Point2D> unit_position_asignments;

	std::map<const Unit*, int> prism_cargo;
	std::map<const Unit*, PrismCargo> units_in_cargo;

	bool using_standby = false;
	Point2D standby_pos;
	Units standby_units;


	int event_id;
	std::vector<UNIT_TYPEID> unit_types;
	int desired_units = 0;
	int max_units = 100;
	bool ready = false;

	// oracles
	std::map<const Unit*, float> time_last_attacked;
	std::map<const Unit*, bool> has_attacked;
	std::map<const Unit*, uint64_t> target;


	ArmyGroup(Mediator*, PathManager, std::vector<Point2D> attack_path, ArmyRole, std::vector<UNIT_TYPEID>);
	ArmyGroup(Mediator*, std::vector<Point2D>, ArmyRole, std::vector<UNIT_TYPEID>);
	ArmyGroup(Mediator*, ArmyRole, std::vector<UNIT_TYPEID>);
	ArmyGroup(Mediator*, Point2D, ArmyRole, std::vector<UNIT_TYPEID>);
	ArmyGroup(Mediator*, Point2D, Point2D, ArmyRole, std::vector<UNIT_TYPEID>);

	~ArmyGroup();

	void AddUnit(const Unit* unit);
	void AddNewUnit(const Unit* unit);
	void RemoveUnit(const Unit* unit);
	Units GetExtraUnits();

	

	std::vector<Point2D> FindConcave(Point2D, Point2D, int, float, float, float);
	std::vector<Point2D> FindConcaveWithPrism(Point2D, Point2D, int, int, float, float, float, std::vector<Point2D>&);
	std::vector<Point2D> FindConcaveFromBack(Point2D, Point2D, int, float, float);
	bool TestSwap(Point2D, Point2D, Point2D, Point2D);
	std::map<const Unit*, Point2D> AssignUnitsToPositions(Units, std::vector<Point2D>);
	void PickUpUnits(std::map<const Unit*, int>);
	void DodgeShots();

	void FindStalkerPositions(std::map<const Unit*, Point2D>&, std::map<const Unit*, Point2D>&, float, float);
	
	void DefendFrontDoor(Point2D, Point2D);
	void DefendThirdBase(Point2D);
	void DefendLine();
	void CannonRushPressure();
	void ScourMap();
	void SimpleAttack();
	void DefendLocation();
	void ObserverScout();
	void OutsideControl();

	int AttackLine(float, float, std::vector<std::vector<UNIT_TYPEID>>, bool = false);
	int AttackLine(Units, float, float, std::vector<std::vector<UNIT_TYPEID>>, bool = false);
	void OraclesDefendArmy(Units);

	bool FindUnitPositions(Units, Units, float, float, Point2D);
	void FindReadyUnits(Units, Units&, Units&);
	void MicroReadyUnits(Units, std::vector<std::vector<UNIT_TYPEID>>, float, int);
	std::vector<std::pair<const Unit*, UnitDanger>> MicroNonReadyUnits(Units);
	void MicroWarpPrisms(std::vector<std::pair<const Unit*, UnitDanger>>);
	Point2D FindLimitToAdvance();

	void AutoAddNewUnits(std::vector<UNIT_TYPEID>);
	void OnNewUnitCreatedListener(const Unit*);
	void AutoAddUnits(std::vector<UNIT_TYPEID>);

	void OnUnitCreatedListener(const Unit*);
	void OnUnitDamagedListener(const Unit*, float, float);
	void OnUnitDestroyedListener(const Unit*);


};

}