#pragma once

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_unit_filters.h"

#include "utility.h"
#include "path_manager.h"
#include "fire_control.h"



namespace sc2
{

class TossBot;

struct UnitDanger
{
	int unit_prio;
	float damage_value;
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
	const Unit* prism;
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
	TossBot* agent;
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

	std::map<const Unit*, bool> attack_status;
	Units possibly_confused_units;

	std::vector<Point2D> attack_path;
	int current_attack_index;
	int high_ground_index;
	PathManager attack_path_line;

	bool advancing = true;
	Point2D concave_origin = Point2D(0, 0);
	std::map<const Unit*, Point2D> unit_position_asignments;

	std::map<const Unit*, int> prism_cargo;
	std::map<const Unit*, PrismCargo> units_in_cargo;

	bool using_standby = false;
	Point2D standby_pos;
	Units standby_units;

	std::map<const Unit*, float> last_time_blinked;

	int event_id;
	std::vector<UNIT_TYPEID> unit_types;

	PersistentFireControl persistent_fire_control;

	ArmyGroup() : persistent_fire_control() {};
	ArmyGroup(TossBot* agent);
	ArmyGroup(TossBot*, Units, std::vector<Point2D>, int);
	ArmyGroup(TossBot*, Units, PathManager);

	void AddUnit(const Unit* unit);
	void AddNewUnit(const Unit* unit);
	void RemoveUnit(const Unit* unit);

	std::vector<Point2D> FindConcave(Point2D, Point2D, int, float, float, float);
	std::vector<Point2D> FindConcaveWithPrism(Point2D, Point2D, int, int, float, float, float, std::vector<Point2D>&);
	std::vector<Point2D> FindConcaveFromBack(Point2D, Point2D, int, float, float);
	bool TestSwap(Point2D, Point2D, Point2D, Point2D);
	std::map<const Unit*, Point2D> AssignUnitsToPositions(Units, std::vector<Point2D>);
	void PickUpUnits(std::map<const Unit*, int>);
	void DodgeShots();

	void FindStalkerPositions(std::map<const Unit*, Point2D>&, std::map<const Unit*, Point2D>&, float, float);
	
	void MicroStalkerAttack();
	void ApplyPressureGrouped(Point2D, Point2D, std::map<const Unit*, Point2D>, std::map<const Unit*, Point2D>);
	void DefendFrontDoor(Point2D, Point2D);
	void DefendExpansion(Point2D, Point2D);
	void CannonRushPressure();


	void MicroUnits();

	void AttackLine(float);
	void FindUnitPositions(Units, Units, float, float);
	void FindReadyUnits(Units, Units&, Units&);
	void MicroReadyUnits(Units, std::vector<UNIT_TYPEID>, float, int);
	std::vector<std::pair<const Unit*, UnitDanger>> MicroNonReadyUnits(Units);
	void MicroWarpPrisms(std::vector<std::pair<const Unit*, UnitDanger>>);


	void AutoAddNewUnits(std::vector<UNIT_TYPEID>);
	void OnNewUnitCreatedListener(const Unit*);
	void AutoAddUnits(std::vector<UNIT_TYPEID>);
	void OnUnitCreatedListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);


};

}