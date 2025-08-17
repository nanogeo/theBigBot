#pragma once

#include "utility.h"
#include "definitions.h"
#include "attack_army_group.h"



namespace sc2
{

class Mediator;

template<typename T>
struct ArmyTemplate;

class PiecewiseAttackArmyGroup : public AttackArmyGroup
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

public:
	PiecewiseAttackArmyGroup(Mediator*, PiecewisePath, std::vector<UNIT_TYPEID>, int, int, int, int);
	PiecewiseAttackArmyGroup(Mediator*, ArmyTemplate<PiecewiseAttackArmyGroup>*);

	void SetUp() override;
	void Run() override;
	void ScourMap() override;
	std::string ToString() const override
	{
		return "Piecewise attack army group";
	}
	void AddUnit(const Unit*) override;
	void RemoveUnit(const Unit*) override;

	bool MobilizeNewUnits(Units);
	void GroupUpNewUnits();

	AttackLineResult AttackLine();

};

}