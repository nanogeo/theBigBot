
#include <iostream>
#include <map>

#include "mediator.h"
#include "defense_manager.h"
#include "definitions.h"
#include "utility.h"

#include "army_group.h"


namespace sc2 
{


void DefenseManager::DisplayOngoingAttacks() const
{
	std::string attacks_message = "Ongoing attacks:\n";
	for (int i = 0; i < ongoing_attacks.size(); i++)
	{
		attacks_message += "pos: (" + std::to_string(ongoing_attacks[i].location.x) + ", " + std::to_string(ongoing_attacks[i].location.y) + ") ";
		attacks_message += "pulled workers: " + std::to_string(ongoing_attacks[i].pulled_workers.size()) + ", ";
		attacks_message += "status: " + std::to_string(ongoing_attacks[i].status) + "\n";

	}
	mediator->DebugText(attacks_message, Point2D(.5, 0), Color(0, 255, 0), 20);
}


void DefenseManager::CheckForAttacks()
{
	for (const auto& base : mediator->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS)))
	{
		Units close_enemies = Utility::GetUnitsWithin(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), base->pos, VERY_LONG_RANGE);
		// consider wall when defending nat vs zerg
		
		auto itr = std::find_if(ongoing_attacks.begin(), ongoing_attacks.end(), [base](const OngoingAttack& attack) { return Point2D(base->pos) == attack.location; });
		if (close_enemies.size() == 0 && itr != ongoing_attacks.end())
		{
			std::cerr << "Attack ended at " << base->pos.x << ", " << base->pos.y <<  " at " << mediator->GetCurrentTime() << std::endl;
			ongoing_attacks.erase(itr);
		}
		else if (close_enemies.size() > 0 && itr == ongoing_attacks.end())
		{
			std::cerr << "Attack started at " << base->pos.x << ", " << base->pos.y << " at " << mediator->GetCurrentTime() << std::endl;
			ongoing_attacks.push_back(OngoingAttack(base->pos, 0, {}));
		}
	}
}

void DefenseManager::UpdateOngoingAttacks()
{
	bool scary_attack = false;
	for (auto& attack : ongoing_attacks)
	{
		Units close_enemies = Utility::GetUnitsWithin(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), attack.location, VERY_LONG_RANGE);
		Units close_allies = Utility::GetUnitsWithin(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Self)), attack.location, VERY_LONG_RANGE);
		Units batteries = Utility::GetUnitsWithin(mediator->GetUnits(IsFinishedUnit(BATTERY)), attack.location, VERY_LONG_RANGE);
		float total_energy = 0.0f;
		for (const auto& battery : batteries)
		{
			total_energy += battery->energy;
		}
		bool sim_city = (mediator->GetEnemyRace() == Race::Zerg && mediator->GetNaturalLocation() == attack.location) ? true : false;
		attack.status = JudgeFight(close_allies, close_enemies, 0.0f, total_energy, sim_city);
		std::cerr << "Attack at " << attack.location.x << ", " << attack.location.y << " current value " << attack.status << std::endl;
		if (attack.status < 0 && run_defense_manager)
		{
			scary_attack = true;
			// pause build
			//mediator->PauseBuildOrder();

			if (!temp_warpgate_production && mediator->GetWarpgateProduction() == UNIT_TYPEID::INVALID)
			{
				mediator->SetUnitProduction(STALKER);
				temp_warpgate_production = true;
			}
			if (!temp_robo_production && mediator->GetRoboProduction() == UNIT_TYPEID::INVALID)
			{
				mediator->SetUnitProduction(IMMORTAL);
				temp_robo_production = true;
				
			}
			if (!temp_stargate_production && mediator->GetStargateProduction() == UNIT_TYPEID::INVALID)
			{
				mediator->SetUnitProduction(VOID_RAY);
				temp_stargate_production = true;
			}

			// make a new battery
			if (attack.location == mediator->GetNaturalLocation() && attack.status <= OGA_LOSING &&
				mediator->GetNumBuildActions(BATTERY) == 0 && 
				mediator->GetUnits(Unit::Alliance::Self, IsNotFinishedUnit(BATTERY)).size() <= 1)
				mediator->BuildDefensiveBuilding(BATTERY, attack.location);

			/*if (attack.location == mediator->GetNaturalLocation() && attack.status <= -50 && attack.pulled_workers.size() == 0)
			{
				mediator->AddAction(&ActionManager::ActionContinueBuildingPylons, new ActionArgData());
				mediator->SetBalanceIncome(true); // TODO move this and above to somewhere more central for when build order is interrupted
				// make a new battery
				if (mediator->GetNumBuildActions(BATTERY) == 0)
					mediator->BuildDefensiveBuilding(BATTERY, attack.location);
				// pull workers
				int max_works_to_pull = 8;
				for (const auto& worker : Utility::GetUnitsWithin(mediator->GetUnits(Unit::Alliance::Self, IsUnit(PROBE)), attack.location, 10))
				{
					if (max_works_to_pull == 0)
						break;
					if (mediator->RemoveWorker(worker) == RemoveWorkerResult::NOT_FOUND)
						continue;
					max_works_to_pull--;
					attack.pulled_workers.push_back(worker);
					ArmyGroup* army_group = mediator->GetArmyGroupDefendingBase(attack.location);
					if (army_group)
						army_group->AddUnit(worker);
				}
				mediator->AddToDefense(attack.location, (int)attack.pulled_workers.size());
				
			}*/


			// increase desired defenders
			// make defensive building(s)
		}
	}
	if (!scary_attack)
	{
		if (temp_warpgate_production)
		{
			temp_warpgate_production = false;
			mediator->CancelWarpgateUnitProduction();
		}
		if (temp_robo_production)
		{
			temp_robo_production = false;
			mediator->CancelRoboUnitProduction();

		}
		if (temp_stargate_production)
		{
			temp_stargate_production = false;
			mediator->CancelStargateUnitProduction();
		}
	}
}

float DefenseManager::JudgeFight(Units friendly_units, Units enemy_units, float enemy_battery_energy, float friendly_battery_energy, bool sim_city) const
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
		mediator->LogMinorError();
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

	
	int max_runs = 20;
	while (enemy_units.size() > 0 && friendly_units.size() > 0 && max_runs > 0)
	{
		int melee_attacks_this_round = 0;
		bool enemy_damage_done = false;
		bool friendly_damage_done = false;
		for (int i = (int)enemy_units.size() - 1; i >= 0; i--)
		{
			if (enemy_units[i]->unit_type == ORACLE && (enemy_units[i]->energy - 10) / 2 <= 20 - max_runs)
				continue;

			float dps = 0;
			const Unit* curr_target = nullptr;
			for (int j = (int)friendly_units.size() - 1; j >= 0; j--)
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
			if (curr_target == nullptr) // couldnt find target to attack
				continue;

			if (dps > 0)
				enemy_damage_done = true;

			if (Utility::IsMelee(enemy_units[i]->unit_type)) // account for melee units needing to reach their target and only a certain amount can attack at once
			{
				if (sim_city)
				{
					if (melee_attacks_this_round > 1)
						dps = 0.0f;
					else
						melee_attacks_this_round++;
				}
				else
				{
					dps /= 1.5f;
				}
			}

			if (friendly_battery_energy > 0)
			{
				if (friendly_battery_energy > dps)
				{
					friendly_battery_energy -= dps;
					dps = 0.0f;
				}
				else
				{
					dps -= friendly_battery_energy;
					friendly_battery_energy = 0;
				}
			}

			friendly_unit_hp[curr_target] -= dps;
		}


		for (int i = (int)friendly_units.size() - 1; i >= 0; i--)
		{
			if (friendly_units[i]->unit_type == ORACLE)
			{
				if (mediator->IsOracleBeamActive(friendly_units[i]))
				{
					if ((friendly_units[i]->energy / 2) - (20 - max_runs) <= 0)
					{
						friendly_units.erase(friendly_units.begin() + i);
						continue;
					}
				}
				else if (friendly_units[i]->energy >= 40) // TODO maybe dont take into account this 40 energy cutoff to switch the oracle on? 
				{
					if (((friendly_units[i]->energy - 25) / 2) - (20 - max_runs) <= 0)
					{
						friendly_units.erase(friendly_units.begin() + i);
						continue;
					}
				}
				else
				{
					friendly_units.erase(friendly_units.begin() + i);
					continue;
				}
			}

			float dps = 0;
			const Unit* curr_target = nullptr;
			for (int j = (int)enemy_units.size() - 1; j >= 0; j--)
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
			if (curr_target == nullptr) // couldnt find target to attack
				continue;

			

			if (dps > 0)
				friendly_damage_done = true;

			if (Utility::IsMelee(friendly_units[i]->unit_type)) // account for melee units needing to reach their target and only a certain amount can attack at once
				dps /= 1.5;

			if (enemy_battery_energy > 0)
			{
				if (enemy_battery_energy > dps)
				{
					enemy_battery_energy -= dps;
					dps = 0.0f;
				}
				else
				{
					dps -= enemy_battery_energy;
					enemy_battery_energy = 0;
				}
			}

			enemy_unit_hp[curr_target] -= dps;
		}

		for (int i = (int)enemy_units.size() - 1; i >= 0; i--)
		{
			if (enemy_unit_hp[enemy_units[i]] <= 0)
				enemy_units.erase(enemy_units.begin() + i);
		}
		for (int i = (int)friendly_units.size() - 1; i >= 0; i--)
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

void DefenseManager::UseBatteries()
{
	Units batteries = mediator->GetUnits(Unit::Alliance::Self, IsFinishedUnit(BATTERY));
	Units nexi = mediator->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS));
	
	for (const auto& battery : batteries)
	{
		Units close_units = mediator->GetUnits(Unit::Alliance::Self);
		for (auto itr = close_units.begin(); itr != close_units.end();)
		{
			// ignore units that are out of range, have enough shields or enough health
			if ((*itr)->build_progress < 1 ||
				Distance2D((*itr)->pos, battery->pos) > Utility::RealRange(battery, (*itr)) ||
				(((*itr)->shield > 10 || (*itr)->health > 50)))
			{
				itr = close_units.erase(itr);
			}
			else
			{
				itr++;
			}
		}
		if (close_units.size() > 0)
		{
			// sort from least to most health
			std::sort(close_units.begin(), close_units.end(), [](const Unit*& a, const Unit*& b) -> bool
			{
				return a->health + a->shield < b->health + b->shield;
			});
			mediator->SetUnitCommand(battery, A_SMART, close_units[0], CommandPriority::normal);
			mediator->DebugSphere(close_units[0]->pos, 2, Color(255, 0, 0));
		}
	}

}

void DefenseManager::RemoveOngoingAttackAt(Point2D location)
{
	for (int i = 0; i < ongoing_attacks.size(); i++)
	{
		if (ongoing_attacks[i].location == location)
		{
			ongoing_attacks.erase(ongoing_attacks.begin() + i);
			i--;
		}
	}
}

}