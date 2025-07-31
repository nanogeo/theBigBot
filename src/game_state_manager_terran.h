#pragma once

#include "definitions.h"
#include "scouting_manager.h"

namespace sc2
{

struct EnemyBaseTerran
{
	const Unit* base; // TODO deal with orbitals/planetaries
	float next_scv;
	EnemyBaseTerran(const Unit* base, float next_scv)
	{
		this->base = base;
		this->next_scv = next_scv;
	}
};

class GameStateManagerTerran : public GameStateManager
{
private:
	ScoutingManager* scouting_manager;
	Mediator* mediator;

	int known_workers;
	int assumed_workers;
	std::vector<EnemyBaseTerran> known_bases;
	std::vector<EnemyBaseTerran> assumed_bases;
	int known_max_supply;
	int assumed_max_supply;

	void UpdateWorkerCount();

public:
	GameStateManagerTerran(ScoutingManager*, Mediator*);

	GameState GetCurrentGameState() override;
	void AddNewUnit(const Unit*) override;
	std::string GameStateToString() const override;
};

} 