#pragma once

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_unit_filters.h"



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

	std::map<const Unit*, bool> attack_status;
	std::vector<Point2D> attack_path;
	int current_attack_index;
	int high_ground_index;


	ArmyGroup() {};
	ArmyGroup(TossBot* agent)
	{
		this->agent = agent;
	}

	ArmyGroup(TossBot* agent, Units all_units, std::vector<Point2D> path, int index)
	{
		this->agent = agent;
		this->all_units = all_units;

		for (const auto &unit : all_units)
		{
			if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ZEALOT)
				zealots.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_STALKER)
				stalkers.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ADEPT)
				adepts.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_SENTRY)
				sentries.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_HIGHTEMPLAR)
				high_templar.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_DARKTEMPLAR)
				dark_templar.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ARCHON)
				archons.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_IMMORTAL)
				immortals.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_COLOSSUS)
				collossi.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_DISRUPTOR)
				disrupter.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_OBSERVER)
				observers.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_WARPPRISM)
				warp_prisms.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_PHOENIX)
				phoenixes.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_VOIDRAY)
				void_rays.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ORACLE)
				oracles.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_CARRIER)
				carriers.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_TEMPEST)
				tempests.push_back(unit);
		}
		attack_path = path;
		current_attack_index = 3;
		high_ground_index = index;
	}

	void AddUnit(const Unit* unit);

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

	void AutoAddStalkers();
	void OnStalkerCreatedListener(const Unit*);


};

}