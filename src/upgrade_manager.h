#pragma once

#include "definitions.h"


namespace sc2
{

class Mediator;


class UpgradeManager
{
private:
	Mediator* mediator;
	bool automatic_upgrades = false;
	std::vector<UPGRADE_ID> required_upgrades;
	std::vector<UPGRADE_ID> finished_upgrades;
	int ground_weapons = 0;
	int ground_armor = 0;
	int shields = 0;
	int air_weapons = 0;
	int air_armor = 0;

	UPGRADE_ID AbilityIDToUpgradeID(ABILITY_ID);
public:
	UpgradeManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void StartUpgradeManager();
	void Run();
	void AddRequiredUpgrade(UPGRADE_ID);

	int GetUpgradeLevel(UpgradeType) const;
	void OnUpgradeCompleted(UPGRADE_ID);
	bool CheckUpgrade(UPGRADE_ID) const;
};

}