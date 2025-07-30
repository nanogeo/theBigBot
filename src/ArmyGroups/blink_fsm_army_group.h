#pragma once

#include "utility.h"
#include "piecewise_path.h"
#include "definitions.h"
#include "army_group.h"
#include "attack_army_group.h"



namespace sc2
{

class Mediator;
class BlinkStalkerAttackTerran;

class BlinkFSMArmyGroup : public AttackArmyGroup
{
protected:
	BlinkStalkerAttackTerran* state_machine;
	std::vector<PiecewisePath> main_attack_path;
	PiecewisePath natural_attack_path;
	std::vector<PiecewisePath> third_attack_path;

	std::vector<std::pair<const Unit*, UnitDanger>> CalculateUnitDanger(Units);
	bool FindNewConcaveOrigin(Units, bool);
	Point2D CalculateNewConcaveOrigin(Units, float, Point2D, int, int);
public:
	BlinkFSMArmyGroup(Mediator*, BlinkStalkerAttackTerran*, int, int, int, int);

	void SetUp() override;
	void Run() override;
	std::string ToString() const override
	{
		return "Blink FSM army group";
	}
	void AddUnit(const Unit*) override;
	void RemoveUnit(const Unit*) override;

	bool MobilizeNewUnits(Units);
	void GroupUpNewUnits();

	Point2D GetConcaveOrigin() const;
	void ResetConcaveOrigin();
	void SetAttackPath(PiecewisePath);
	const PiecewisePath& GetAttackPath() const;
	void SetUseStandby(bool);
	void SetStandbyPos(Point2D);

	AttackLineResult AttackLine(Units);
	AttackLineResult AttackLine(Units, const Unit*);

	void ConvertToRegularAttackArmyGroup();
};

}