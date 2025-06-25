#pragma once

#include "definitions.h"

namespace sc2
{

class Mediator;

class ScoutingManager;

class GameStateManager
{
public:
	ScoutingManager* scouting_manager;

	virtual GameState GetCurrentGameState() = 0;
	virtual void AddNewUnit(const Unit*) = 0;
	virtual std::string GameStateToString() = 0;
};

class ScoutingManager
{
public:
	Mediator* mediator;
	Race enemy_race = Race::Random;
	std::map<const Unit*, EnemyUnitPosition> enemy_unit_saved_position;
	std::map<UNIT_TYPEID, int> enemy_unit_counts;


	float first_gas_timing = 0;
	float second_gas_timing = 0;
	float natural_timing = 0;
	float third_timing = 0;

	// Terran scouting info
	float first_barrack_timing = 0;
	float factory_timing = 0;
	FirstRaxProduction first_rax_production = FirstRaxProduction::idle;

	// Protoss scouting info
	float first_gate_timing = 0;
	float second_gate_timing = 0;
	float first_pylon_timing = 0;
	float second_pylon_timing = 0;
	UNIT_TYPEID tech_choice = UNIT_TYPEID::INVALID;

	// Zerg scouting info
	float spawning_pool_timing = 0;
	float roach_warren_timing = 0;
	

	// game state
	GameState current_game_state = GameState::early_build;
	GameStateManager* game_state_manager = nullptr;


	ScoutingManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void SetEnemyRace(Race);
	void SetEnemyRace(UNIT_TYPEID);

	int GetEnemyUnitCount(UNIT_TYPEID);
	uint16_t GetEnemyArmySupply();

	void UpdateInfo();
	void AddNewUnit(const Unit*);
	int CheckTerranScoutingInfoEarly();

	void OnUnitDestroyed(const Unit*);

	// game state
	void InitializeGameState();
	float GetCurrentTime();

};


}