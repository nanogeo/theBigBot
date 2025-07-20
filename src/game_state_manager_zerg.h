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
private:
	ScoutingManager* scouting_manager;
	Mediator* mediator;

	int known_workers;
	int assumed_workers;
	std::vector<EnemyBase> known_bases;
	std::vector<EnemyBase> assumed_bases;
	int known_max_supply;
	int assumed_max_supply;

	bool odd_zergling = false;

	void UpdateWorkerCount();
	void UseLarva();

public:
	GameStateManagerZerg(ScoutingManager*, Mediator*);

	GameState GetCurrentGameState() override;
	void AddNewUnit(const Unit*) override;
	std::string GameStateToString() const override;
};

} 