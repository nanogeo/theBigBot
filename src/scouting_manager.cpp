#pragma once
#include "scouting_manager.h"
#include "mediator.h"
#include "definitions.h"

namespace sc2 {

void ScoutingManager::SetEnemyRace(Race race)
{
	enemy_race = race;
}

void ScoutingManager::SetEnemyRace(UNIT_TYPEID type)
{
	switch (type)
	{
	case NEXUS:
	case PYLON:
	case GATEWAY:
	case WARP_GATE:
	case ASSIMILATOR:
	case FORGE:
	case CYBERCORE:
	case CANNON:
	case BATTERY:
	case TWILIGHT:
	case TEMPLAR_ARCHIVES:
	case STARGATE:
	case FLEET_BEACON:
	case DARK_SHRINE:
	case ROBO:
	case ROBO_BAY:
	case PROBE:
	case ZEALOT:
	case STALKER:
	case SENTRY:
	case ADEPT:
	case HIGH_TEMPLAR:
	case DARK_TEMPLAR:
	case IMMORTAL:
	case COLOSSUS:
	case DISRUPTOR:
	case ARCHON:
	case OBSERVER:
	case OBSERVER_SIEGED:
	case PRISM:
	case PRISM_SIEGED:
	case PHOENIX:
	case VOID_RAY:
	case ORACLE:
	case CARRIER:
	case TEMPEST:
	case MOTHERSHIP:
		enemy_race = Race::Protoss;
		break;
	case COMMAND_CENTER:
	case COMMAND_CENTER_FLYING:
	case PLANETARY:
	case ORBITAL:
	case ORBITAL_FLYING:
	case SUPPLY_DEPOT:
	case SUPPLY_DEPOT_LOWERED:
	case REFINERY:
	case BARRACKS:
	case BARRACKS_FLYING:
	case ENGINEERING_BAY:
	case BUNKER:
	case SENSOR_TOWER:
	case MISSILE_TURRET:
	case FACTORY:
	case FACTORY_FLYING:
	case GHOST_ACADEMY:
	case STARPORT:
	case STARPORT_FLYING:
	case ARMORY:
	case FUSION_CORE:
	case TECH_LAB:
	case REACTOR:
	case BARRACKS_TECH_LAB:
	case BARRACKS_REACTOR:
	case FACTORY_TECH_LAB:
	case FACTORY_REACTOR:
	case STARPORT_TECH_LAB:
	case STARPORT_REACTOR:
	case SCV:
	case MULE:
	case MARINE:
	case MARAUDER:
	case REAPER:
	case GHOST:
	case HELLION:
	case HELLBAT:
	case SIEGE_TANK:
	case SIEGE_TANK_SIEGED:
	case CYCLONE:
	case THOR_AOE:
	case THOR_AP:
	case WIDOW_MINE:
	case AUTO_TURRET:
	case VIKING:
	case VIKING_LANDED:
	case MEDIVAC:
	case LIBERATOR:
	case LIBERATOR_SIEGED:
	case RAVEN:
	case BANSHEE:
	case BATTLECRUISER:
		enemy_race = Race::Terran;
		break;
	case LARVA:
	case EGG:
	case DRONE:
	case DRONE_BURROWED:
	case QUEEN:
	case QUEEN_BURROWED:
	case ZERGLING:
	case ZERGLING_BURROWED:
	case BANELING_EGG:
	case BANELING:
	case BANELING_BURROWED:
	case ROACH:
	case ROACH_BURROWED:
	case RAVAGER_EGG:
	case RAVAGER:
	case RAVAGER_BURROWED:
	case HYDRA:
	case HYDRA_BURROWED:
	case LURKER_EGG:
	case LURKER:
	case LURKER_BURROWED:
	case INFESTOR:
	case INFESTOR_BURROWED:
	case SWARMHOST:
	case SWARMHOST_BURROWED:
	case ULTRALISK:
	case ULTRALISK_BURROWED:
	case LOCUST:
	case LOCUST_FLYING:
	case BROODLING:
	case CHANGELING:
	case CHANGELING_ZEALOT:
	case NYDUS_WORM:
	case OVERLORD:
	case DROPPERLORD:
	case OVERSEER_EGG:
	case OVERSEER:
	case OVERSEER_SIEGED:
	case MUTALISK:
	case CORRUPTER:
	case BROOD_LORD_EGG:
	case BROOD_LORD:
	case VIPER:
	case HATCHERY:
	case LAIR:
	case HIVE:
	case SPINE_CRAWLER:
	case SPINE_CRAWLER_UPROOTED:
	case SPORE_CRAWLER:
	case SPORE_CRAWLER_UPROOTED:
	case SPAWNING_POOL:
	case ROACH_WARREN:
	case EVO_CHAMBER:
	case EXTRACTOR:
	case BANELING_NEST:
	case HYDRA_DEN:
	case LURKER_DEN:
	case INFESTATION_PIT:
	case NYDUS:
	case SPIRE:
	case GREATER_SPIRE:
	case ULTRALISK_CAVERN:
	case CREEP_TUMOR_1:
	case CREEP_TUMOR_2:
	case CREEP_TUMOR_3:
		enemy_race = Race::Zerg;
		break;
	default:
		std::cerr << "Unknown unit type in SetEnemyRace: " << Utility::UnitTypeIdToString(type) << std::endl;
	}
}

int ScoutingManager::GetEnemyUnitCount(UNIT_TYPEID type)
{
	if (enemy_unit_counts.count(type) > 0)
		return enemy_unit_counts[type];
	else
		return 0;
}

void ScoutingManager::UpdateInfo()
{
	for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy))
	{
		if (unit->display_type != Unit::DisplayType::Visible)
			continue;
		if (enemy_unit_saved_position.count(unit) > 0)
		{
			if (enemy_unit_saved_position[unit].pos == unit->pos)
			{
				enemy_unit_saved_position[unit].frames++;
			}
			else
			{
				enemy_unit_saved_position[unit].pos = unit->pos;
				enemy_unit_saved_position[unit].frames = 0;
			}
		}
		else
		{
			AddNewUnit(unit);
		}
	}
}

void ScoutingManager::AddNewUnit(const Unit* unit)
{
	if (enemy_race == Race::Random)
		SetEnemyRace(unit->unit_type);

	enemy_unit_saved_position[unit] = EnemyUnitPosition(unit->pos);
	if (enemy_unit_counts.count(unit->unit_type) > 0)
		enemy_unit_counts[unit->unit_type] += 1;
	else
		enemy_unit_counts[unit->unit_type] = 1;

	switch (unit->unit_type.ToType())
	{
	case BARRACKS:
		if (first_barrack_time == 0)
			first_barrack_time = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	case FACTORY:
		if (factory_timing == 0)
			factory_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	case REFINERY:
		if (gas_timing == 0)
			gas_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		else if (second_gas_timing == 0)
			second_gas_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	case COMMAND_CENTER:
		if (natural_timing == 0 && unit->build_progress < 1)
			natural_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;

	}
}

void ScoutingManager::OnUnitDestroyed(const Unit* unit)
{
	if (enemy_unit_saved_position.find(unit) != enemy_unit_saved_position.end())
	{
		enemy_unit_saved_position.erase(unit);
		if (enemy_unit_counts.count(unit->unit_type) > 0)
			enemy_unit_counts[unit->unit_type] -= 1;
	}
}

}