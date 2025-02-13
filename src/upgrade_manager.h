#pragma once

#include "definitions.h"


namespace sc2
{

class Mediator;


class UpgradeManager
{
public:
	Mediator* mediator;

	bool has_blink = false;
	bool has_charge = false;
	bool has_storm = false;
	bool has_dt_blink = false;
	int ground_weapon = 0;
	int ground_armor = 0;
	int shields = 0;
	int air_weapon = 0;
	int air_armor = 0;

	UpgradeManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	int GetUpgradeLevel(UpgradeType);
	void OnUpgradeCompleted(UPGRADE_ID);
};

}