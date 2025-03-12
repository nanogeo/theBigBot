#pragma once

#include "mediator.h"
#include "defense_manager.h"
#include "definitions.h"
#include "utility.h"

#include "army_group.h"


namespace sc2 {


void DefenseManager::CheckForAttacks()
{
	for (const auto& base : mediator->GetUnits(IsUnit(NEXUS)))
	{
		Units close_enemies = Utility::GetUnitsWithin(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), base->pos, 20);
		ArmyGroup* defense_group = mediator->GetArmyGroupDefendingBase(base->pos);
		// consider wall when defending nat vs zerg
		
		if (close_enemies.size() == 0 && std::find(ongoing_attacks.begin(), ongoing_attacks.end(), base->pos) != ongoing_attacks.end())
		{
			std::cerr << "Attack ended at " << base->pos.x << ", " << base->pos.y <<  " at " << mediator->GetCurrentTime() << std::endl;
			ongoing_attacks.erase(std::remove(ongoing_attacks.begin(), ongoing_attacks.end(), base->pos), ongoing_attacks.end());
		}
		else if (close_enemies.size() > 0 && std::find(ongoing_attacks.begin(), ongoing_attacks.end(), base->pos) == ongoing_attacks.end())
		{
			std::cerr << "Attack started at " << base->pos.x << ", " << base->pos.y << " at " << mediator->GetCurrentTime() << std::endl;
			ongoing_attacks.push_back(base->pos);
		}
	}
}

void DefenseManager::UpdateOngoingAttacks()
{
	for (const auto& attack : ongoing_attacks)
	{
		Units close_enemies = Utility::GetUnitsWithin(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), attack, 20);
		Units close_allies = Utility::GetUnitsWithin(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Self)), attack, 20);
		Units batteries = Utility::GetUnitsWithin(mediator->GetUnits(IsUnit(BATTERY)), attack, 20);
		int total_energy = 0;
		for (const auto& battery : batteries)
		{
			total_energy += battery->energy;
		}
		bool sim_city = (mediator->GetEnemyRace() == Race::Zerg && mediator->GetNaturalLocation() == attack) ? true : false;
		float value = JudgeFight(close_enemies, close_allies, 0, total_energy, sim_city);
		std::cerr << "Attack at " << attack.x << ", " << attack.y << " current value " << value << std::endl;
	}

}

float DefenseManager::JudgeFight(Units enemy_units, Units friendly_units, float enemy_battery_energy, float friendly_battery_energy, bool sim_city)
{
	/*std::vector<std::vector<UNIT_TYPEID>> prio;
	switch (mediator->GetEnemyRace())
	{
	case Race::Protoss:
		prio = PROTOSS_PRIO;
		break;
	case Race::Terran:
		prio = TERRAN_PRIO;
		break;
	case Race::Zerg:
		prio = ZERG_PRIO;
		break;
	default:
		std::cerr << "Invalid race in DefenseManager::JudgeFight" << std::endl;
	}*/

	// sort from most to least health
	std::sort(enemy_units.begin(), enemy_units.end(), [](const Unit*& a, const Unit*& b) -> bool
	{
		return a->health + a->shield > b->health + b->shield;
	});
	std::sort(friendly_units.begin(), friendly_units.end(), [](const Unit*& a, const Unit*& b) -> bool
	{
		return a->health + a->shield > b->health + b->shield;
	});

	std::map<const Unit*, float> enemy_unit_hp;
	for (const auto& unit : enemy_units)
	{
		enemy_unit_hp[unit] = unit->health + unit->shield;
	}
	std::map<const Unit*, float> friendly_unit_hp;
	for (const auto& unit : friendly_units)
	{
		friendly_unit_hp[unit] = unit->health + unit->shield;
	}

	
	int max_runs = 40;
	while (enemy_units.size() > 0 && friendly_units.size() > 0 && max_runs > 0)
	{
		int melee_attacks_this_round = 0;
		bool enemy_damage_done = false;
		bool friendly_damage_done = false;
		for (int i = enemy_units.size() - 1; i >= 0; i--)
		{
			if (enemy_units[i]->unit_type == ORACLE && enemy_units[i]->energy / 2 <= 40 - max_runs)
				continue;

			float dps = 0;
			const Unit* curr_target = NULL;
			for (int j = friendly_units.size() - 1; j >= 0; j--)
			{
				if (friendly_unit_hp[friendly_units[j]] <= 0)
					continue;
				dps = Utility::GetDPS(enemy_units[i], friendly_units[j]);
				if (dps > 0)
				{
					curr_target = friendly_units[j];
					break;
				}
			}
			if (curr_target == NULL) // couldnt find target to attack
				continue;

			if (dps > 0)
				enemy_damage_done = true;

			if (Utility::IsMelee(enemy_units[i]->unit_type)) // account for melee units needing to reach their target and only a certain amount can attack at once
			{
				if (sim_city && melee_attacks_this_round > 1)
				{
					dps = 0;
				}
				else
				{
					melee_attacks_this_round++;
					dps /= 2;
				}
			}

			if (friendly_battery_energy > 0)
			{
				if (friendly_battery_energy > dps)
				{
					friendly_battery_energy -= dps;
					dps = 0;
				}
				else
				{
					dps -= friendly_battery_energy;
					friendly_battery_energy = 0;
				}
			}

			friendly_unit_hp[curr_target] -= dps;
		}


		for (int i = friendly_units.size() - 1; i >= 0; i--)
		{
			if (friendly_units[i]->unit_type == ORACLE && friendly_units[i]->energy / 2 <= 40 - max_runs)
				continue;

			float dps = 0;
			const Unit* curr_target = NULL;
			for (int j = enemy_units.size() - 1; j >= 0; j--)
			{
				if (enemy_unit_hp[enemy_units[j]] <= 0)
					continue;
				dps = Utility::GetDPS(friendly_units[i], enemy_units[j]);
				if (dps > 0)
				{
					curr_target = enemy_units[j];
					break;
				}
			}
			if (curr_target == NULL) // couldnt find target to attack
				continue;


			if (dps > 0)
				friendly_damage_done = true;

			if (Utility::IsMelee(friendly_units[i]->unit_type)) // account for melee units needing to reach their target and only a certain amount can attack at once
				dps /= 2;

			if (enemy_battery_energy > 0)
			{
				if (enemy_battery_energy > dps)
				{
					enemy_battery_energy -= dps;
					dps = 0;
				}
				else
				{
					dps -= enemy_battery_energy;
					enemy_battery_energy = 0;
				}
			}

			enemy_unit_hp[curr_target] -= dps;
		}

		for (int i = enemy_units.size() - 1; i >= 0; i--)
		{
			if (enemy_unit_hp[enemy_units[i]] <= 0)
				enemy_units.erase(enemy_units.begin() + i);
		}
		for (int i = friendly_units.size() - 1; i >= 0; i--)
		{
			if (friendly_unit_hp[friendly_units[i]] <= 0)
				friendly_units.erase(friendly_units.begin() + i);
		}
		if (!enemy_damage_done || !friendly_damage_done)
			break;

		max_runs--;
	}

	if (enemy_units.size() == 0)
	{
		// simiulated fight won
		float total_dps = 0;
		for (const auto& unit : friendly_units)
		{
			total_dps += Utility::GetDPS(unit);
		}
		return total_dps;
	}
	if (friendly_units.size() == 0)
	{
		// simiulated fight lost
		float total_dps = 0;
		for (const auto& unit : enemy_units)
		{
			total_dps -= Utility::GetDPS(unit);
		}
		return total_dps;
	}

	float total_dps = 0;
	for (const auto& unit : friendly_units)
	{
		total_dps += Utility::GetDPS(unit);
	}
	for (const auto& unit : enemy_units)
	{
		total_dps -= Utility::GetDPS(unit);
	}
	return total_dps; // simulated fight tied or took too long
}

}