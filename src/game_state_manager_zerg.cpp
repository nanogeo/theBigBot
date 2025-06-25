
#include "game_state_manager_zerg.h"
#include "scouting_manager.h"
#include "definitions.h"
#include "utility.h"

namespace sc2 {

GameStateManagerZerg::GameStateManagerZerg(ScoutingManager* scouting_manager, Units enemy_bases)
{
	this->scouting_manager = scouting_manager;
	float current_time = scouting_manager->GetCurrentTime();
	known_workers = scouting_manager->enemy_unit_counts[DRONE];
	assumed_workers = scouting_manager->enemy_unit_counts[DRONE];
	known_max_supply = (scouting_manager->enemy_unit_counts[OVERLORD] * 8) + (enemy_bases.size() * 6);
	assumed_max_supply = (scouting_manager->enemy_unit_counts[OVERLORD] * 8) + (enemy_bases.size() * 6);

	for (const auto& base : enemy_bases)
	{
		float larva_time = base->build_progress == 1 ? current_time : Utility::GetTimeBuilt(base, current_time) + Utility::GetTimeToBuild(HATCHERY);
		float spawn_larva_time1 = Utility::GetTimeBuilt(base, current_time) + Utility::GetTimeToBuild(HATCHERY) + Utility::GetTimeToBuild(QUEEN) + 29;
		float spawn_larva_time2 = scouting_manager->spawning_pool_timing + Utility::GetTimeToBuild(SPAWNING_POOL) + Utility::GetTimeToBuild(QUEEN) + 29;

		known_bases.push_back(EnemyBase(base, base->build_progress == 1 ? current_time : Utility::GetTimeBuilt(base, current_time) + Utility::GetTimeToBuild(HATCHERY),
			std::max(Utility::GetTimeBuilt(base, current_time) + Utility::GetTimeToBuild(HATCHERY) + Utility::GetTimeToBuild(QUEEN) + 29,
				scouting_manager->spawning_pool_timing + Utility::GetTimeToBuild(SPAWNING_POOL) + Utility::GetTimeToBuild(QUEEN) + 29)));
	}
}

GameState GameStateManagerZerg::GetCurrentGameState()
{
	UpdateWorkerCount();
	return GameState::unknown;
}

void GameStateManagerZerg::UpdateWorkerCount()
{
	float current_time = scouting_manager->GetCurrentTime();

	for (auto& base : known_bases)
	{
		if (current_time >= base.next_larva)
		{
			UseLarva();
			base.next_larva += 11;
		}
		if (current_time >= base.next_spawn_larva)
		{
			UseLarva();
			UseLarva();
			UseLarva();
			base.next_spawn_larva += 29;
		}
	}
	for (auto& base : assumed_bases)
	{
		if (current_time >= base.next_larva)
		{
			UseLarva();
			base.next_larva += 11;
		}
		if (current_time >= base.next_spawn_larva)
		{
			UseLarva();
			UseLarva();
			UseLarva();
			base.next_spawn_larva += 29;
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

void GameStateManagerZerg::AddNewUnit(const Unit* unit)
{
	float current_time = scouting_manager->GetCurrentTime();

	switch (unit->unit_type.ToType())
	{
	case DRONE:
		known_workers++;
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
			Utility::GetTimeBuilt(unit, current_time) + Utility::GetTimeToBuild(HATCHERY) + Utility::GetTimeToBuild(QUEEN) + 29));
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

void GameStateManagerZerg::UseLarva()
{
	if (scouting_manager->GetEnemyArmySupply() + assumed_workers >= assumed_max_supply)
		assumed_max_supply = std::min(200, assumed_max_supply + 8);
	else
		assumed_workers = std::min(90, assumed_workers + 1);
}

std::string GameStateManagerZerg::GameStateToString()
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