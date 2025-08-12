#pragma once

#include "utility.h"
#include "piecewise_path.h"
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

	PiecewisePath attack_path;
	std::map<const Unit*, Point2D> unit_position_asignments;
	float dispersion;
	float default_range = 6.0f;
	std::vector<std::vector<UNIT_TYPEID>> target_priority;
	bool limit_advance;
	Point2D pre_prism_limit = Point2D(0, 0);
	Point2D concave_origin;
	float concave_degree = 30;
	bool advancing = true;
	std::vector<Units> units_on_their_way;
	int min_reinforce_group_size;
	float unit_size = 0;
	Units basic_units;
	bool using_standby = false;
	Units standby_units;
	float attack_threshold = .25f;
	int required_units;

	static float CalculateDesiredRange(Units, Units, float, float);
	static Point2D CalculateConcaveTarget(Point2D, Units, Path*);
	static Point2D FindLimitToAdvance(Path*, Point2D, float);
	Point2D FindLimitToAdvance(Path*, std::vector<UNIT_TYPEID>, float, bool, int);
	static PathDirection ShouldMoveConcaveOrigin(Point2D, Point2D, Path*, float, Units, Point2D);
	static float GetAverageDistanceForUnitPosition(std::map<const Unit*, Point2D>);
	static Point2D GetNewOriginForward(Point2D, Point2D, float, Path*, Point2D);
	static Point2D GetNewOriginBackward(Point2D, Point2D, float, Path*, Units, float);
	std::vector<Point2D> FindConcaveWithPrism(int, int, float, float, float, Point2D, Point2D, std::vector<Point2D>&);
	static std::map<const Unit*, Point2D> AssignUnitsToPositions(Units, std::vector<Point2D>);
	std::vector<std::pair<const Unit*, UnitDanger>> CalculateUnitDanger(Units) const;
	static bool TestSwap(Point2D, Point2D, Point2D, Point2D);
	Units EvadeDamage(std::vector<std::pair<const Unit*, UnitDanger>>, Units, Point2D);

	void OraclesDefendArmy();

	void MicroReadyUnits(Units, float, int);

public:
	AttackArmyGroup(Mediator*, PiecewisePath, std::vector<UNIT_TYPEID>, int, int, int, int);
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

	AttackLineResult AttackLine(Units, Point2D&, float, Path*, bool,
		Point2D, Units, float, std::map<const Unit*, Point2D>&, Units, bool&, float);
	AttackLineResult AttackLine();

};

}