#pragma once

#include "definitions.h"
#include "scouting_manager.h"

namespace sc2
{

struct EnemyBase
{
	const Unit* base;
	float next_larva;
	float next_spawn_larva;
	EnemyBase(const Unit* base, float next_larva, float next_spawn_larva)
	{
		this->base = base;
		this->next_larva = next_larva;
		this->next_spawn_larva = next_spawn_larva;
	}
};

class GameStateManagerZerg : public GameStateManager
{
public:
	ScoutingManager* scouting_manager;
	Mediator* mediator;

	uint16_t known_workers;
	uint16_t assumed_workers;
	std::vector<EnemyBase> known_bases;
	std::vector<EnemyBase> assumed_bases;
	uint16_t known_max_supply;
	uint16_t assumed_max_supply;

	bool odd_zergling = false;


	GameStateManagerZerg(ScoutingManager*, Mediator*);

	GameState GetCurrentGameState() override;
	void UpdateWorkerCount();
	void AddNewUnit(const Unit*) override;
	void UseLarva();
	std::string GameStateToString() override;
};

} 