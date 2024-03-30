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

	Point2D concave_origin = Point2D(0, 0);
	std::map<const Unit*, Point2D> unit_position_asignments;

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
	void FindUnitPositions(Units, float);
	void FindReadyUnits(Units, Units&, Units&);
	void MicroReadyUnits(Units, std::vector<UNIT_TYPEID>, float, int);
	std::map<const Unit*, int> MicroNonReadyUnits(Units);


	void AutoAddNewUnits(std::vector<UNIT_TYPEID>);
	void OnNewUnitCreatedListener(const Unit*);
	void AutoAddUnits(std::vector<UNIT_TYPEID>);
	void OnUnitCreatedListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);


};

}