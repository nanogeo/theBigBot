#pragma once

#include "utility.h"
#include "path_manager.h"
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

enum class AttackLineResult
{
	normal,
	all_units_dead,
	all_units_standing_by,
	reached_end_of_path
};

class Mediator;

template<typename T>
struct ArmyTemplate;

class AttackArmyGroup : public ArmyGroup
{
protected:
	Units warp_prisms;
	Units oracles;

	PathManager attack_path;
	std::map<const Unit*, Point2D> unit_position_asignments;
	float dispersion;
	float default_range = 6.0f;
	std::vector<std::vector<UNIT_TYPEID>> target_priority;
	bool limit_advance;
	Point2D concave_origin;
	Point2D concave_target;
	float concave_degree = 30;
	bool advancing = true;
	std::vector<Units> units_on_their_way;
	int min_reinforce_group_size;
	float unit_size = 0;
	Units basic_units;
	bool using_standby = false;
	Units standby_units;
	Point2D standby_pos;
	float attack_threshold = .25f;
	int required_units;

	float CalculateDesiredRange(Units);
	Point2D CalculateConcaveTarget();
	Point2D CalculateNewConcaveOrigin(Units, float, Point2D);
	Point2D FindLimitToAdvance(std::vector<UNIT_TYPEID>, float, bool, int);
	void FindNewConcaveOrigin();
	std::vector<Point2D> FindConcaveWithPrism(std::vector<Point2D>&);
	std::vector<std::pair<const Unit*, UnitDanger>> CalculateUnitDanger();
	std::map<const Unit*, Point2D> AssignUnitsToPositions(Units, std::vector<Point2D>);
	void OraclesDefendArmy();
	bool TestSwap(Point2D, Point2D, Point2D, Point2D) const;

	void MicroReadyUnits(Units, float, int);

public:
	AttackArmyGroup(Mediator*, PathManager, std::vector<UNIT_TYPEID>, int, int, int, int);
	AttackArmyGroup(Mediator*, ArmyTemplate<AttackArmyGroup>*);

	void SetUp() override;
	void Run() override;
	void ScourMap() override;
	std::string ToString() const override
	{
		return "Attack army group";
	}
	void AddUnit(const Unit*) override;
	void RemoveUnit(const Unit*) override;

	bool MobilizeNewUnits(Units);
	void GroupUpNewUnits();

	AttackLineResult AttackLine();

};

}