#pragma once

#include "definitions.h"
#include "attack_army_group.h"
#include "point_path.h"



namespace sc2
{

class Mediator;

struct DefensiveGroup
{
	Point2D defensive_pos;
	Units friendly_units;
	Units new_units;
	Units basic_units;
	Units prisms;
	Units oracles;
	Units enemy_units;
	float status = 0;
	Point2D origin = Point2D(0, 0);
	float normal_range = 7;
	PointPath* path;
	std::map<const Unit*, Point2D> unit_position_assignments;
	bool advancing = true;

	DefensiveGroup(Point2D defensive_pos, Units friendly_units, Units enemy_units, PointPath* path)
	{
		this->defensive_pos = defensive_pos;
		this->enemy_units = enemy_units;
		this->path = path;
		for (const auto& unit : friendly_units)
		{
			this->friendly_units.push_back(unit);
			new_units.push_back(unit);
			if (unit->unit_type == ORACLE)
				oracles.push_back(unit);
			else if (unit->unit_type == PRISM)
				prisms.push_back(unit);
		}
	}
	void AddNewUnit(const Unit* unit)
	{
		friendly_units.push_back(unit);
		new_units.push_back(unit);
	}
	void AddUnit(const Unit* unit)
	{
		new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());

		if (unit->unit_type == ORACLE)
			oracles.push_back(unit);
		else if (unit->unit_type == PRISM || unit->unit_type == PRISM_SIEGED)
			prisms.push_back(unit);
		else
			basic_units.push_back(unit);
	}
	void RemoveUnit(const Unit* unit)
	{
		friendly_units.erase(std::remove(friendly_units.begin(), friendly_units.end(), unit), friendly_units.end());

		if (std::find(new_units.begin(), new_units.end(), unit) != new_units.end())
		{
			new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());
			return;
		}

		if (unit->unit_type == ORACLE)
			oracles.erase(std::remove(oracles.begin(), oracles.end(), unit), oracles.end());
		else if (unit->unit_type == PRISM || unit->unit_type == PRISM_SIEGED)
			prisms.erase(std::remove(prisms.begin(), prisms.end(), unit), prisms.end());
		else
			basic_units.erase(std::remove(basic_units.begin(), basic_units.end(), unit), basic_units.end());
	}
};


class DefenseArmyGroup : public AttackArmyGroup
{
protected:
	Point2D central_pos;
	int air_harassers = 0;
	Units unassigned_units;
	std::vector<DefensiveGroup> defensive_groups;

	void AllocateUnitsToDefensiveGroups(std::vector<EnemyArmyGroup>);

public:
	DefenseArmyGroup(Mediator*);

	void SetUp() override;
	void Run() override;
	std::string ToString() const override
	{
		return "Defense army group";
	}

	void AddNewUnit(const Unit* unit) override;
	void AddUnit(const Unit* unit) override;
	void RemoveUnit(const Unit* unit) override;

	AttackLineResult AttackLine(DefensiveGroup&);
	void IndividualAttack(DefensiveGroup&);

	void LogDebugInfo();

};

}