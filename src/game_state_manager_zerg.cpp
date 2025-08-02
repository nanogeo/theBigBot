
#include "game_state_manager_zerg.h"
#include "scouting_manager.h"
#include "definitions.h"
#include "utility.h"
#include "mediator.h"

namespace sc2 {
	
void GameStateManagerZerg::UpdateWorkerCount()
{
	float current_time = scouting_manager->GetCurrentTime();

	for (auto& base : known_bases)
	{
		if (current_time >= base.next_larva)
		{
			UseLarva();
			base.next_larva += CD_GENERATE_LARVA;
		}
		if (current_time >= base.next_spawn_larva)
		{
			UseLarva();
			UseLarva();
			UseLarva();
			base.next_spawn_larva += DURATION_SPAWN_LARVA;
		}
	}
	for (auto& base : assumed_bases)
	{
		if (current_time >= base.next_larva)
		{
			UseLarva();
			base.next_larva += CD_GENERATE_LARVA;
		}
		if (current_time >= base.next_spawn_larva)
		{
			UseLarva();
			UseLarva();
			UseLarva();
			base.next_spawn_larva += DURATION_SPAWN_LARVA;
		}
	}
	if (assumed_workers > (assumed_bases.size() + known_bases.size()) * 16 + scouting_manager->GetEnemyUnitCount(EXTRACTOR) * 3)
	{
		assumed_workers = std::max(assumed_workers - 1, 0);
		assumed_max_supply = std::min(200, assumed_max_supply + 4);
		assumed_bases.push_back(EnemyBase(nullptr, current_time + Utility::GetTimeToBuild(HATCHERY),
			current_time + Utility::GetTimeToBuild(HATCHERY) + Utility::GetTimeToBuild(QUEEN) + 29));
	}
}

void GameStateManagerZerg::UseLarva()
{
	if (scouting_manager->GetEnemyArmySupply() + assumed_workers >= assumed_max_supply)
		assumed_max_supply = std::min(200, assumed_max_supply + 8);
	else
		assumed_workers = std::min(90, assumed_workers + 1);
}

GameStateManagerZerg::GameStateManagerZerg(ScoutingManager* scouting_manager, Mediator* mediator)
{
	this->mediator = mediator;
	this->scouting_manager = scouting_manager;
	Units enemy_bases = mediator->GetUnits(Unit::Alliance::Enemy, IsUnits(TOWNHALL_TYPES));
	float current_time = scouting_manager->GetCurrentTime();
	known_workers = scouting_manager->GetEnemyUnitCount(DRONE);
	assumed_workers = scouting_manager->GetEnemyUnitCount(DRONE) + 12;
	known_max_supply = (scouting_manager->GetEnemyUnitCount(OVERLORD) * 8) + ((int)enemy_bases.size() * 6);
	assumed_max_supply = ((scouting_manager->GetEnemyUnitCount(OVERLORD) + 1) * 8) + (((int)enemy_bases.size() + 1) * 6);

	assumed_bases.push_back(EnemyBase(nullptr, current_time, current_time + 120));

	for (const auto& base : enemy_bases)
	{
		float larva_time = base->build_progress == 1 ? current_time : Utility::GetTimeBuilt(base, current_time) + Utility::GetTimeToBuild(HATCHERY);
		float spawn_larva_time1 = Utility::GetTimeBuilt(base, current_time) + Utility::GetTimeToBuild(HATCHERY) + Utility::GetTimeToBuild(QUEEN) + DURATION_SPAWN_LARVA;
		float spawn_larva_time2 = scouting_manager->spawning_pool_timing + Utility::GetTimeToBuild(SPAWNING_POOL) + Utility::GetTimeToBuild(QUEEN) + DURATION_SPAWN_LARVA;

		known_bases.push_back(EnemyBase(base, larva_time, std::max(spawn_larva_time1, spawn_larva_time2)));
	}
}

GameState GameStateManagerZerg::GetCurrentGameState()
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

void GameStateManagerZerg::AddNewUnit(const Unit* unit)
{
	float current_time = scouting_manager->GetCurrentTime();

	switch (unit->unit_type.ToType())
	{
	case DRONE:
		known_workers++;
		assumed_workers = std::max(assumed_workers, known_workers);
		break;
	case ZERGLING:
	case ZERGLING_BURROWED:
		if (odd_zergling)
		{
			odd_zergling = false;
			assumed_workers = std::max(assumed_workers - 1, 0);
		}
		else
		{
			odd_zergling = true;
		}
		break;
	case HATCHERY:
		known_bases.push_back(EnemyBase(unit, Utility::GetTimeBuilt(unit, current_time) + Utility::GetTimeToBuild(HATCHERY), 
			Utility::GetTimeBuilt(unit, current_time) + Utility::GetTimeToBuild(HATCHERY) + Utility::GetTimeToBuild(QUEEN) + DURATION_SPAWN_LARVA));
		if (assumed_bases.size() > 0)
			assumed_bases.pop_back();
	case ROACH:
	case ROACH_BURROWED:
	case HYDRA:
	case HYDRA_BURROWED:
	case INFESTOR:
	case INFESTOR_BURROWED:
	case SWARMHOST:
	case SWARMHOST_BURROWED:
	case ULTRALISK:
	case ULTRALISK_BURROWED:
	case OVERLORD:
	case MUTALISK:
	case CORRUPTER:
	case VIPER:
	case SPINE_CRAWLER:
	case SPORE_CRAWLER:
	case SPAWNING_POOL:
	case ROACH_WARREN:
	case EVO_CHAMBER:
	case EXTRACTOR:
	case EXTRACTOR_RICH:
	case BANELING_NEST:
	case HYDRA_DEN:
	case LURKER_DEN:
	case INFESTATION_PIT:
	case NYDUS:
	case SPIRE:
	case ULTRALISK_CAVERN:
		assumed_workers = std::max(assumed_workers - 1, 0);
		break;
	case BANELING_EGG:
	case BANELING:
	case BANELING_BURROWED:
	case RAVAGER_EGG:
	case RAVAGER:
	case RAVAGER_BURROWED:
	case LURKER_EGG:
	case LURKER:
	case LURKER_BURROWED:
	case DROPPERLORD_EGG:
	case DROPPERLORD:
	case OVERSEER_EGG:
	case OVERSEER:
	case OVERSEER_SIEGED:
	case BROOD_LORD_EGG:
	case BROOD_LORD:
	case LAIR:
	case HIVE:
	case SPINE_CRAWLER_UPROOTED:
	case SPORE_CRAWLER_UPROOTED:
	case GREATER_SPIRE:
		// TODO figure out what to do with these units
		break;
	}
}

std::string GameStateManagerZerg::GameStateToString() const
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