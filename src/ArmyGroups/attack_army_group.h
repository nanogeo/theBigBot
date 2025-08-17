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

	void OraclesDefendArmy(Units, Path*, Units);

public:
	AttackArmyGroup(Mediator* mediator) : ArmyGroup(mediator) {};

	std::string ToString() const override
	{
		return "Attack army group";
	}
	AttackLineResult AttackLine(Units, Point2D&, float, Path*, bool,
		Point2D, Units, float, std::map<const Unit*, Point2D>&, Units, bool&, float);


};

}