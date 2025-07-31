
#include "game_state_manager_terran.h"
#include "scouting_manager.h"
#include "definitions.h"
#include "utility.h"
#include "mediator.h"

namespace sc2 {
	
void GameStateManagerTerran::UpdateWorkerCount()
{
	float current_time = scouting_manager->GetCurrentTime();

	for (auto& base : known_bases)
	{
		if (current_time >= base.next_scv)
		{
			assumed_workers = std::min(90, assumed_workers + 1);
			base.next_scv += Utility::GetTrainingTime(SCV);
		}
	}
	for (auto& base : assumed_bases)
	{
		if (current_time >= base.next_scv)
		{
			assumed_workers = std::min(90, assumed_workers + 1);
			base.next_scv += Utility::GetTrainingTime(SCV);
		}
	}
	if (assumed_workers > (assumed_bases.size() + known_bases.size()) * 16 + scouting_manager->GetEnemyUnitCount(REFINERY) * 3)
	{
		assumed_max_supply = std::min(200, assumed_max_supply + 15);
		assumed_bases.push_back(EnemyBaseTerran(nullptr, current_time + Utility::GetTimeToBuild(COMMAND_CENTER) + Utility::GetTrainingTime(SCV)));
	}
}

GameStateManagerTerran::GameStateManagerTerran(ScoutingManager* scouting_manager, Mediator* mediator)
{
	this->mediator = mediator;
	this->scouting_manager = scouting_manager;
	Units enemy_bases = mediator->GetUnits(Unit::Alliance::Enemy, IsUnits(TOWNHALL_TYPES));
	float current_time = scouting_manager->GetCurrentTime();
	known_workers = scouting_manager->GetEnemyUnitCount(SCV);
	assumed_workers = scouting_manager->GetEnemyUnitCount(SCV);
	known_max_supply = (scouting_manager->GetEnemyUnitCount(SUPPLY_DEPOT) * 8) + (scouting_manager->GetEnemyUnitCount(SUPPLY_DEPOT_LOWERED) * 8) + ((int)enemy_bases.size() * 15);
	assumed_max_supply = (scouting_manager->GetEnemyUnitCount(SUPPLY_DEPOT) * 8) + (scouting_manager->GetEnemyUnitCount(SUPPLY_DEPOT_LOWERED) * 8) + ((int)enemy_bases.size() * 15);

	for (const auto& base : enemy_bases)
	{
		float scv_time = base->build_progress == 1 ? current_time : Utility::GetTimeBuilt(base, current_time) + Utility::GetTimeToBuild(COMMAND_CENTER);
		scv_time += Utility::GetTrainingTime(SCV);

		known_bases.push_back(EnemyBaseTerran(base, scv_time));
	}
}

GameState GameStateManagerTerran::GetCurrentGameState()
{
	GameState game_state = GameState();
	UpdateWorkerCount();
	if ((float)assumed_workers / known_workers >= 2)
		game_state.good_worker_intel = false;
	else
		game_state.good_worker_intel = true;

	int workers = (int)mediator->GetUnits(Unit::Alliance::Self, IsUnit(PROBE)).size();
	if (workers + 20 < assumed_workers)
		game_state.game_state_worker = GameStateWorker::much_less;
	else if (workers - 20 > assumed_workers)
		game_state.game_state_worker = GameStateWorker::much_more;
	else if (workers + 5 < assumed_workers)
		game_state.game_state_worker = GameStateWorker::slightly_less;
	else if (workers - 5 > assumed_workers)
		game_state.game_state_worker = GameStateWorker::slightly_more;
	else
		game_state.game_state_worker = GameStateWorker::even;

	return game_state;
}

void GameStateManagerTerran::AddNewUnit(const Unit* unit)
{
	float current_time = scouting_manager->GetCurrentTime();

	switch (unit->unit_type.ToType())
	{
	case SCV:
		known_workers++;
		break;
	case COMMAND_CENTER:
		known_bases.push_back(EnemyBaseTerran(unit, Utility::GetTimeBuilt(unit, current_time) + Utility::GetTimeToBuild(COMMAND_CENTER) + Utility::GetTrainingTime(SCV)));
		if (assumed_bases.size() > 0)
			assumed_bases.pop_back();
	}
}

std::string GameStateManagerTerran::GameStateToString() const
{
	std::string str;
	str += "Current game state\n";
	str += "known enemy workers: " + std::to_string(known_workers) + '\n';
	str += "assumed enemy workers: " + std::to_string(assumed_workers) + '\n';
	str += "known enemy bases: " + std::to_string(known_bases.size()) + '\n';
	str += "assumed enemy bases: " + std::to_string(assumed_bases.size()) + '\n';
	str += "known enemy max supply: " + std::to_string(known_max_supply) + '\n';
	str += "assumed enemy max supply: " + std::to_string(assumed_max_supply) + '\n';
	return str;
}
}