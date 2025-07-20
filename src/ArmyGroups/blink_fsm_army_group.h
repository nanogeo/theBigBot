#pragma once

#include "utility.h"
#include "path_manager.h"
#include "definitions.h"
#include "army_group.h"
#include "attack_army_group.h"



namespace sc2
{

class Mediator;
class BlinkStalkerAttackTerran;

// TODO multiple inheritance from OutsideControlArmyGroup
class BlinkFSMArmyGroup : public AttackArmyGroup
{
protected:
	BlinkStalkerAttackTerran* state_machine;

	std::vector<PathManager> main_attack_path;
	PathManager natural_attack_path;
	std::vector<PathManager> third_attack_path;

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
	void SetAttackPath(PathManager);
	const PathManager& GetAttackPath() const;
	void SetUseStandby(bool);
	void SetStandbyPos(Point2D);
};

}