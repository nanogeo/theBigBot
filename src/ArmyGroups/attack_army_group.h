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
public:
	Units warp_prisms;
	Units oracles;

	PathManager attack_path;
	std::map<const Unit*, Point2D> unit_position_asignments;
	float dispersion;
	float default_range;
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
	bool using_standby;
	Units standby_units;
	Point2D standby_pos;
	float attack_threshold = .25f;
	uint16_t required_units;

	AttackArmyGroup(Mediator*, PathManager, std::vector<UNIT_TYPEID>, uint16_t, uint16_t, uint16_t, uint16_t);
	AttackArmyGroup(Mediator*, ArmyTemplate<AttackArmyGroup>*);

	void SetUp() override;
	void Run() override;
	std::string ToString() override
	{
		return "Attack army group";
	}
	void AddUnit(const Unit*) override;
	void RemoveUnit(const Unit*) override;

	bool MobilizeNewUnits(Units);
	void GroupUpNewUnits();

	AttackLineResult AttackLine();

	float CalculateDesiredRange(Units, float);
	Point2D CalculateConcaveTarget();
	Point2D CalculateNewConcaveOrigin(Units, Point2D, float, Point2D);
	Point2D FindLimitToAdvance(std::vector<UNIT_TYPEID>, float, bool, uint16_t);
	void FindNewConcaveOrigin();
	std::vector<Point2D> FindConcaveWithPrism(Point2D, float, std::vector<Point2D>&);
	std::vector<std::pair<const Unit*, UnitDanger>> CalculateUnitDanger();
	std::map<const Unit*, Point2D> AssignUnitsToPositions(Units, std::vector<Point2D>);
	void OraclesDefendArmy(Units);
	bool TestSwap(Point2D, Point2D, Point2D, Point2D);

	void MicroReadyUnits(Units, float, int);
};

}