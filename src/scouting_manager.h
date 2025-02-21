#pragma once

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_unit_filters.h"

#include "definitions.h"

namespace sc2
{

class Mediator;

class ScoutingManager
{
public:
	Mediator* mediator;
	Race enemy_race = Race::Random;
	std::map<const Unit*, EnemyUnitPosition> enemy_unit_saved_position;
	std::map<UNIT_TYPEID, int> enemy_unit_counts;

	// Terran scouting info
	float first_barrack_time = 0;
	float factory_timing = 0;
	float natural_timing = 0;
	float gas_timing = 0;
	float second_gas_timing = 0;
	FirstRaxProduction first_rax_production = FirstRaxProduction::idle;

	

	ScoutingManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void SetEnemyRace(Race);
	void SetEnemyRace(UNIT_TYPEID);

	int GetEnemyUnitCount(UNIT_TYPEID);

	void UpdateInfo();
	void AddNewUnit(const Unit*);

	void OnUnitDestroyed(const Unit*);

};

}