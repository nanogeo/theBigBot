#pragma once

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_unit_filters.h"

#include "utility.h"



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
	std::vector<Point2D> attack_path;
	int current_attack_index;
	int high_ground_index;

	std::map<const Unit*, float> last_time_blinked;

	int event_id;
	std::vector<UNIT_TYPEID> unit_types;

	ArmyGroup() {};
	ArmyGroup(TossBot* agent)
	{
		this->agent = agent;
	}

	ArmyGroup(TossBot*, Units, std::vector<Point2D>, int);

	void AddUnit(const Unit* unit);
	void AddNewUnit(const Unit* unit);
	void RemoveUnit(const Unit* unit);

	std::vector<Point2D> FindConcave(Point2D, Point2D, int, float, float);
	std::vector<Point2D> FindConcaveFromBack(Point2D, Point2D, int, float, float);
	bool TestSwap(Point2D, Point2D, Point2D, Point2D);
	std::map<const Unit*, Point2D> AssignUnitsToPositions(Units, std::vector<Point2D>);
	void PickUpUnits(std::map<const Unit*, int>);
	void DodgeShots();

	void ApplyPressureGrouped(Point2D, Point2D, std::map<const Unit*, Point2D>, std::map<const Unit*, Point2D>);
	void DefendFrontDoor(Point2D, Point2D);
	void DefendExpansion(Point2D, Point2D);

	void MicroUnits();

	void AutoAddUnits(std::vector<UNIT_TYPEID>);
	void OnUnitCreatedListener(const Unit*);


};

}