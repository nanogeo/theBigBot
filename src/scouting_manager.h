#pragma once

#include "definitions.h"

namespace sc2
{
	
struct EnemyAttack
{
	const Unit* unit;
	uint32_t impact_frame;
	EnemyAttack(const Unit* unit, int impact_frame)
	{
		this->unit = unit;
		this->impact_frame = impact_frame;
	}
};

struct LiberatorZone
{
	Point2D pos;
	bool current;
	LiberatorZone(Point2D pos)
	{
		this->pos = pos;
		current = true;
	}
	bool operator==(const LiberatorZone &b)
	{
		return pos == b.pos;
	}
};

class Mediator;

class ScoutingManager;

class GameStateManager
{
protected:
	ScoutingManager* scouting_manager;
public:
	virtual GameState GetCurrentGameState() = 0;
	virtual void AddNewUnit(const Unit*) = 0;
	virtual std::string GameStateToString() const = 0;
};

class ScoutingManager
{
	friend class GameStateManagerZerg;
	friend Mediator;
private:
	Mediator* mediator;
	Race enemy_race = Race::Random;
	std::map<const Unit*, EnemyUnitPosition> enemy_unit_saved_position;
	std::map<UNIT_TYPEID, int> enemy_unit_counts;

	std::map<const Unit*, float> enemy_weapon_cooldown;
	std::map<const Unit*, std::vector<EnemyAttack>> enemy_attacks;
	std::vector<Point2D> corrosive_bile_positions;
	std::vector<uint32_t> corrosive_bile_times;
	std::vector<LiberatorZone> liberator_zone_current;

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
	GameState current_game_state;
	GameStateManager* game_state_manager = nullptr;

	void UpdateEnemyWeaponCooldowns();
	void RemoveCompletedAttacks();
	void UpdateEffectPositions();

public:
	ScoutingManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void DisplayEnemyAttacks() const;
	void DisplayEnemyPositions() const;
	void DisplayKnownEffects() const;

	void SetEnemyRace(Race);
	void SetEnemyRace(UNIT_TYPEID);

	int GetEnemyUnitCount(UNIT_TYPEID) const;
	float GetEnemyArmySupply() const;
	const std::vector<Point2D>& GetCorrosiveBilePositions() const;

	void UpdateInfo();
	void AddNewUnit(const Unit*);
	int CheckTerranScoutingInfoEarly();

	int GetIncomingDamage(const Unit*) const;

	void OnUnitDestroyed(const Unit*);

	// game state
	void InitializeGameStateManager();
	float GetCurrentTime() const;

};


}