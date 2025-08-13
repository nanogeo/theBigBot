
#include "scouting_manager.h"
#include "game_state_manager_zerg.h"
#include "game_state_manager_terran.h"
#include "mediator.h"
#include "definitions.h"

namespace sc2 {


void ScoutingManager::UpdateEnemyWeaponCooldowns()
{
	Units allied_units = mediator->GetUnits(Unit::Alliance::Self);
	Units enemy_attacking_units = mediator->GetUnits(Unit::Alliance::Enemy, IsUnits(PROJECTILE_UNIT_TYPES));

	for (const auto &Eunit : enemy_attacking_units)
	{
		if (enemy_unit_saved_position.count(Eunit) == 0)
			continue;
		if (enemy_weapon_cooldown.count(Eunit) == 0)
			enemy_weapon_cooldown[Eunit] = 0;

		const Unit* target = Utility::AimingAt(Eunit, allied_units);

		if (target != NULL && enemy_weapon_cooldown[Eunit] == 0 && enemy_unit_saved_position[Eunit].frames > Utility::GetDamagePoint(Eunit) * 22.4)
		{
			float damage_point = Utility::GetDamagePoint(Eunit);
			if (damage_point == 0)
			{
				enemy_weapon_cooldown[Eunit] = Utility::GetWeaponCooldown(Eunit) - damage_point - (1 / FRAME_TIME);
				EnemyAttack attack = EnemyAttack(Eunit, mediator->GetGameLoop() + (int)std::floor(Utility::GetProjectileTime(Eunit, Distance2D(Eunit->pos, target->pos) - Eunit->radius - target->radius) - 1));
				if (enemy_attacks.count(target) == 0)
					enemy_attacks[target] = { attack };
				else
					enemy_attacks[target].push_back(attack);
			}
			else
			{
				enemy_weapon_cooldown[Eunit] = Utility::GetWeaponCooldown(Eunit);
				EnemyAttack attack = EnemyAttack(Eunit, mediator->GetGameLoop() + (int)std::floor(Utility::GetProjectileTime(Eunit, Distance2D(Eunit->pos, target->pos) - Eunit->radius - target->radius)));
				if (enemy_attacks.count(target) == 0)
					enemy_attacks[target] = { attack };
				else
					enemy_attacks[target].push_back(attack);
			}
		}

		if (enemy_weapon_cooldown[Eunit] > 0)
			enemy_weapon_cooldown[Eunit] -= 1 / FRAME_TIME;
		if (enemy_weapon_cooldown[Eunit] < 0)
		{
			enemy_weapon_cooldown[Eunit] = 0;
			enemy_unit_saved_position[Eunit].frames = -1;
		}
		//Debug()->DebugTextOut(std::to_string(enemy_weapon_cooldown[Eunit]), Eunit->pos + Point3D(0, 0, .2), Color(255, 0, 255), 20);

	}

}

void ScoutingManager::RemoveCompletedAttacks()
{
	for (auto& attack : enemy_attacks)
	{
		for (int i = (int)attack.second.size() - 1; i >= 0; i--)
		{
			if (attack.second[i].impact_frame <= mediator->GetGameLoop())
			{
				attack.second.erase(attack.second.begin() + i);
			}
		}
	}
}

void ScoutingManager::UpdateEffectPositions()
{
	for (auto& zone : liberator_zone_current)
	{
		zone.current = false;
	}

	for (const auto& effect : mediator->GetEffects())
	{
		if (effect.effect_id == 11) // EFFECT_CORROSIVEBILE
		{
			for (const auto& pos : effect.positions)
			{
				if (std::find(corrosive_bile_positions.begin(), corrosive_bile_positions.end(), pos) == corrosive_bile_positions.end())
				{
					corrosive_bile_positions.push_back(pos);
					corrosive_bile_times.push_back(mediator->GetGameLoop() + 48);
				}
			}
		}
		if (effect.effect_id == 9 || effect.effect_id == 8) // LIBERATORDEFENDERZONE || LIBERATORDEFENDERZONESETUP
		{
			for (const auto& pos : effect.positions)
			{
				auto itr = std::find_if(liberator_zone_current.begin(), liberator_zone_current.end(), [pos](const LiberatorZone& zone) { return zone.pos == pos; });

				if (itr == liberator_zone_current.end())
				{
					liberator_zone_current.push_back(LiberatorZone(pos));
				}
				else
				{
					itr->current = true;
				}
			}
		}
	}
	for (int i = 0; i < corrosive_bile_positions.size(); i++)
	{
		if (mediator->GetGameLoop() > corrosive_bile_times[i])
		{
			corrosive_bile_positions.erase(corrosive_bile_positions.begin() + i);
			corrosive_bile_times.erase(corrosive_bile_times.begin() + i);
		}
	}
	/*for (int i = 0; i < corrosive_bile_positions.size(); i++)
	{
		Debug()->DebugSphereOut(ToPoint3D(corrosive_bile_positions[i]), .5, Color(255, 0, 255));
		Debug()->DebugTextOut(std::to_string(corrosive_bile_times[i]), ToPoint3D(corrosive_bile_positions[i]), Color(255, 0, 255), 14);
	}*/


	for (int k = 0; k < liberator_zone_current.size(); k++)
	{
		if (liberator_zone_current[k].current == false && mediator->IsVisible(liberator_zone_current[k].pos))
		{
			liberator_zone_current.erase(std::remove(liberator_zone_current.begin(), liberator_zone_current.end(), liberator_zone_current[k]), liberator_zone_current.end());
			k--;
		}
	}

}

void ScoutingManager::GroupEnemyUnits()
{
	std::map<OrderedPoint2D, Units> defending_groups;
	std::map<OrderedPoint2D, Units> attacking_groups;
	for (const auto& unit : enemy_unit_saved_position)
	{
		if (unit.first->is_building)
			continue;

		std::pair<Point2D, NodeControl> pos = mediator->FindClosestSkeletonPointWithControl(unit.first->pos);

		if (pos.second == NodeControl::enemy_control)
		{
			auto itr = defending_groups.find(pos.first);
			if (itr != defending_groups.end())
			{
				(*itr).second.push_back(unit.first);
			}
			else
			{
				defending_groups[OrderedPoint2D(unit.first->pos)] = { unit.first };
			}
		}
		else
		{
			auto itr = attacking_groups.find(pos.first);
			if (itr != attacking_groups.end())
			{
				(*itr).second.push_back(unit.first);
			}
			else
			{
				attacking_groups[OrderedPoint2D(pos.first)] = { unit.first };
			}
		}
	}

	for (auto& curr = attacking_groups.begin(); curr != attacking_groups.end(); curr++)
	{
		bool connection_found = false;
		std::vector<Point2D> points = { (*curr).first };
		do
		{
			connection_found = false;
			for (auto& itr = std::next(curr); itr != attacking_groups.end();)
			{
				if (Utility::DistanceToClosest(points, (*itr).first) < CLOSE_RANGE)
				{
					(*curr).second.insert((*curr).second.end(), (*itr).second.begin(), (*itr).second.end());
					points.push_back((*itr).first);
					itr = attacking_groups.erase(itr);
					connection_found = true;
				}
				else
				{
					itr++;
				}
			}

		} while (connection_found);
	}

	for (const auto& group : attacking_groups)
	{
		std::vector<Point2D> attacking_path = mediator->FindPathToFriendlyControlledArea(group.first);

		if (attacking_path.size() == 0)
		{
			// error no path found
			continue;
		}
		// add incoming atack to path.back() with units group[1]
		std::vector<Point2D> defensive_path = mediator->FindPath(mediator->GetStartLocation(), attacking_path.back());

		for (const auto& pos : attacking_path)
		{
			mediator->DebugSphere(mediator->ToPoint3D(pos), .7, Color(255, 0, 0));
		}
		for (const auto& pos : defensive_path)
		{
			mediator->DebugSphere(mediator->ToPoint3D(pos), .7, Color(0, 128, 255));
		}
	}
}


void ScoutingManager::DisplayEnemyAttacks() const
{
	std::string message = "Current frame: " + std::to_string(mediator->GetGameLoop()) + "\n";
	message += "Current time: " + std::to_string(mediator->GetCurrentTime()) + "\n";
	for (const auto& unit : enemy_attacks)
	{
		message += UnitTypeToName(unit.first->unit_type.ToType());
		message += ":\n";
		for (const auto& attack : unit.second)
		{
			message += "    ";
			message += UnitTypeToName(attack.unit->unit_type.ToType());
			message += " - " + std::to_string(attack.impact_frame) + "\n";
		}
	}
	mediator->DebugText(message, Point2D(.8f, .4f), Color(255, 0, 0), 20);
}

void ScoutingManager::DisplayEnemyPositions() const
{
	for (const auto& unit : enemy_unit_saved_position)
	{
		mediator->DebugText(UnitTypeToName(unit.first->unit_type), mediator->ToPoint3D(unit.second.pos), Color(255, 128, 128), 20);
		if (unit.first->unit_type == SIEGE_TANK || unit.first->unit_type == SIEGE_TANK_SIEGED)
			mediator->DebugSphere(mediator->ToPoint3D(unit.second.pos), 14, Color(255, 128, 128));
	}
}

void ScoutingManager::DisplayKnownEffects() const
{
	for (const auto& bile : corrosive_bile_positions)
	{
		mediator->DebugText("bile", mediator->ToPoint3D(bile), Color(255, 140, 0), 20);
		mediator->DebugSphere(mediator->ToPoint3D(bile), .5, Color(255, 140, 0));
	}
	for (const auto& zone : liberator_zone_current)
	{
		mediator->DebugText("liberator zone", mediator->ToPoint3D(zone.pos), Color(0, 0, 160), 20);
		mediator->DebugSphere(mediator->ToPoint3D(zone.pos), 5, Color(0, 0, 160));
	}
}

void ScoutingManager::SetEnemyRace(Race race)
{
	enemy_race = race;
	InitializeGameStateManager();
}

void ScoutingManager::SetEnemyRace(UNIT_TYPEID type)
{
	InitializeGameStateManager();
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
	case TEMPLAR_ARCHIVE:
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
		mediator->SendChat("Tag:race_protoss", ChatChannel::Team);
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
		mediator->SendChat("Tag:race_terran", ChatChannel::Team);
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
		mediator->SendChat("Tag:race_zerg", ChatChannel::Team);
		break;
	default:
		std::cerr << "Unknown unit type in SetEnemyRace: " << UnitTypeToName(type) << std::endl;
		mediator->LogMinorError();
	}
}

int ScoutingManager::GetEnemyUnitCount(UNIT_TYPEID type) const
{
	if (enemy_unit_counts.count(type) > 0)
		return enemy_unit_counts.at(type);
	else
		return 0;
}

float ScoutingManager::GetEnemyArmySupply() const
{
	float total_supply = 0;
	for (const auto& unit : enemy_unit_saved_position)
	{
		if (unit.first->unit_type != DRONE && unit.first->unit_type != SCV && unit.first->unit_type != PROBE)
			total_supply += Utility::GetCost(unit.first->unit_type).supply;
	}
	return total_supply;
}

const std::vector<Point2D>& ScoutingManager::GetCorrosiveBilePositions() const
{
	return corrosive_bile_positions;
}

GameState ScoutingManager::GetGameState()
{
	if (game_state_manager)
		return game_state_manager->GetCurrentGameState();
	else
		return GameState();
}

std::string ScoutingManager::GameStateToString()
{
	if (game_state_manager)
		return game_state_manager->GameStateToString();
	else
		return "";
}

void ScoutingManager::UpdateInfo()
{
	for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy))
	{
		if (unit->display_type != Unit::DisplayType::Visible)
			continue;
		if (enemy_unit_saved_position.count(unit) > 0)
		{
			if (Distance2D(enemy_unit_saved_position[unit].pos, unit->pos) < .05)
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

	UpdateEffectPositions();
	UpdateEnemyWeaponCooldowns();
	RemoveCompletedAttacks();
	GroupEnemyUnits();
}

void ScoutingManager::AddNewUnit(const Unit* unit)
{
	if (enemy_race == Race::Random)
		SetEnemyRace(unit->unit_type);

	if (game_state_manager != nullptr)
		game_state_manager->AddNewUnit(unit);

	enemy_unit_saved_position[unit] = EnemyUnitPosition(unit->pos);
	if (enemy_unit_counts.count(unit->unit_type) > 0)
		enemy_unit_counts[unit->unit_type] += 1;
	else
		enemy_unit_counts[unit->unit_type] = 1;

	if (unit->is_building)
	{
		mediator->ChangeAreaControl(unit->pos, MEDIUM_RANGE, mediator->ToPoint3D(unit->pos).z, NodeControl::enemy_control);
	}

	switch (unit->unit_type.ToType())
	{
	case REFINERY:
	case ASSIMILATOR:
	case EXTRACTOR:
		if (first_gas_timing == 0)
			first_gas_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		else if (second_gas_timing == 0)
			second_gas_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	case COMMAND_CENTER:
	case HATCHERY:
	case NEXUS:
		if (natural_timing == 0 && unit->build_progress < 1)
			natural_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		else if (third_timing == 0 && unit->build_progress < 1)
			third_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	case BARRACKS:
		if (first_barrack_timing == 0)
			first_barrack_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	case BARRACKS_REACTOR:
		if (first_rax_production == FirstRaxProduction::idle)
			first_rax_production = FirstRaxProduction::reactor;
		break;
	case BARRACKS_TECH_LAB:
		if (first_rax_production == FirstRaxProduction::idle)
			first_rax_production = FirstRaxProduction::techlab;
		break;
	case FACTORY:
		if (factory_timing == 0)
			factory_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	case GATEWAY:
		if (first_gate_timing == 0)
			first_gate_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		else if (second_gate_timing == 0)
			second_gate_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	case PYLON:
		if (first_pylon_timing == 0)
			first_pylon_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		else if (second_pylon_timing == 0)
			second_pylon_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	case ROBO:
	case TWILIGHT:
	case STARGATE:
		if (tech_choice == UNIT_TYPEID::INVALID)
			tech_choice = unit->unit_type;
		break;
	case SPAWNING_POOL:
		if (spawning_pool_timing == 0)
			spawning_pool_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	case ROACH_WARREN:
		if (roach_warren_timing == 0)
			roach_warren_timing = Utility::GetTimeBuilt(unit, mediator->GetCurrentTime());
		break;
	}
}

int ScoutingManager::CheckTerranScoutingInfoEarly()
{
	if (mediator->GetCurrentTime() > 240) // 4 mins is no longer early
		return 0;

	if (natural_timing > 0)
		return 0;

	int correct_scv_count = (int)(floor(mediator->GetCurrentTime() / 12) + 12);
	if (GetEnemyUnitCount(ORBITAL) > 0)
		correct_scv_count -= 2;

	int actual_scv_count = 1; // start at 1 in case one was just produced
	for (const auto& scv : enemy_unit_saved_position)
	{
		if (scv.first->unit_type == SCV && 
			(Distance2D(scv.second.pos, mediator->GetEnemyStartLocation()) > 15 ||
			Distance2D(scv.second.pos, mediator->GetStartLocation()) > 15))
			actual_scv_count++;
	}
	int difference = correct_scv_count - actual_scv_count;
	if (difference <= 1)
	{
		// no proxy
		return 0;
	}
	else if (difference <= 2)
	{
		// minor proxy
		return 1;
	}
	else
	{
		// major proxy
		return 2;
	}
}

int ScoutingManager::GetIncomingDamage(const Unit* unit) const
{
	int damage = 0;
	for (const auto& buff : unit->buffs)
	{
		if (buff == B_LOCK_ON)
			damage += 60;
	}
	if (enemy_attacks.count(unit) > 0)
	{
		for (const auto& attack : enemy_attacks.at(unit))
		{
			damage += Utility::GetDamage(attack.unit, unit);
		}
	}
	for (int i = 0; i < corrosive_bile_positions.size(); i++)
	{
		if (mediator->GetGameLoop() + 5 > corrosive_bile_times[i])
		{
			if (Distance2D(corrosive_bile_positions[i], unit->pos) < .5 + unit->radius + .3) // TODO maybe change extra distance
			{
				damage += CORROSIVE_BILE_DAMAGE;
			}
		}
		else
		{
			break;
		}
	}
	for (const auto& enemy : Utility::GetUnitsThatCanAttack(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits(HIT_SCAN_UNIT_TYPES)), unit, 0))
	{
		damage += Utility::GetDamage(enemy, unit) / 2;
	}
	return damage;
}

void ScoutingManager::RemoveAllAttacksAtUnit(const Unit* unit)
{
	enemy_attacks.erase(unit);
}

void ScoutingManager::OnUnitDestroyed(const Unit* unit)
{
	if (enemy_weapon_cooldown.find(unit) != enemy_weapon_cooldown.end())
		enemy_weapon_cooldown.erase(unit);

	if (enemy_unit_saved_position.find(unit) != enemy_unit_saved_position.end())
	{
		enemy_unit_saved_position.erase(unit);
		if (enemy_unit_counts.count(unit->unit_type) > 0)
			enemy_unit_counts[unit->unit_type] -= 1;
	}
}

void ScoutingManager::InitializeGameStateManager()
{
	if (game_state_manager != nullptr)
		return;

	switch (enemy_race)
	{
	case Race::Zerg:
		game_state_manager = new GameStateManagerZerg(this, mediator);
		break;
	case Race::Protoss:
		break;
	case Race::Terran:
		game_state_manager = new GameStateManagerTerran(this, mediator);
		break;
	case Race::Random:
		std::cerr << "Error enemy race still unknown in ScoutingManager::InitializeGameState" << std::endl;
		break;
	}
}

float ScoutingManager::GetCurrentTime() const
{
	return mediator->GetCurrentTime();
}

}