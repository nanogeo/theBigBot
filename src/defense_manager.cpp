
#include <iostream>
#include <map>

#include "mediator.h"
#include "defense_manager.h"
#include "definitions.h"
#include "utility.h"

#include "army_group.h"


namespace sc2 {


void DefenseManager::CheckForAttacks()
{
	for (const auto& base : mediator->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS)))
	{
		Units close_enemies = Utility::GetUnitsWithin(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), base->pos, 20);
		ArmyGroup* defense_group = mediator->GetArmyGroupDefendingBase(base->pos);
		// consider wall when defending nat vs zerg
		
		auto itr = std::find_if(ongoing_attacks.begin(), ongoing_attacks.end(), [base](const OngoingAttack& attack) { return Point2D(base->pos) == attack.location; });
		if (close_enemies.size() == 0 && itr != ongoing_attacks.end())
		{
			std::cerr << "Attack ended at " << base->pos.x << ", " << base->pos.y <<  " at " << mediator->GetCurrentTime() << std::endl;
			ArmyGroup* army_group = mediator->GetArmyGroupDefendingBase(itr->location);
			if (army_group)
			{
				for (const auto& worker : itr->pulled_workers)
				{
					army_group->RemoveUnit(worker);
					if (worker->is_alive)
						mediator->PlaceWorker(worker);
				}
				// reset desired defenses
				mediator->AddToDefense(itr->location, -1 * army_group->desired_units);
			}
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
		Units close_enemies = Utility::GetUnitsWithin(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), attack.location, 20);
		Units close_allies = Utility::GetUnitsWithin(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Self)), attack.location, 20);
		Units batteries = Utility::GetUnitsWithin(mediator->GetUnits(IsUnit(BATTERY)), attack.location, 20);
		int total_energy = 0;
		for (const auto& battery : batteries)
		{
			total_energy += battery->energy;
		}
		bool sim_city = (mediator->GetEnemyRace() == Race::Zerg && mediator->GetNaturalLocation() == attack.location) ? true : false;
		attack.status = JudgeFight(close_enemies, close_allies, 0, total_energy, sim_city);
		std::cerr << "Attack at " << attack.location.x << ", " << attack.location.y << " current value " << attack.status<< std::endl;
		if (attack.status < 0)
		{
			scary_attack = true;
			// pause build
			//mediator->PauseBuildOrder();

			// warp in or make units from gates
			if (mediator->CheckUpgrade(UPGRADE_ID::WARPGATERESEARCH))
			{
				if (mediator->GetWarpgateProduction() == UNIT_TYPEID::BALL)
				{
					mediator->SetUnitProduction(STALKER);
					reset_warpgate_production = true;
				}
			}

			// make units from other tech structures
			if (mediator->GetRoboProduction() == UNIT_TYPEID::BALL)
			{
				mediator->SetUnitProduction(IMMORTAL);
				reset_robo_production = true;
			}
			if (mediator->GetStargateProduction() == UNIT_TYPEID::BALL)
			{
				mediator->SetUnitProduction(VOID_RAY);
				reset_stargate_production = true;
			}

			if (attack.location == mediator->GetNaturalLocation() && attack.status <= -50 && attack.pulled_workers.size() == 0)
			{
				mediator->AddAction(&ActionManager::ActionContinueBuildingPylons, new ActionArgData());
				// make a new battery
				if (mediator->GetNumBuildActions(BATTERY) == 0)
					mediator->BuildDefensiveBuilding(BATTERY, attack.location);
				// pull workers
				for (const auto& worker : Utility::GetUnitsWithin(mediator->GetUnits(Unit::Alliance::Self, IsUnit(PROBE)), attack.location, 10))
				{
					if (mediator->RemoveWorker(worker) == RemoveWorkerResult::NOT_FOUND)
						continue;
					attack.pulled_workers.push_back(worker);
					ArmyGroup* army_group = mediator->GetArmyGroupDefendingBase(attack.location);
					if (army_group)
						army_group->AddUnit(worker);
				}
				mediator->AddToDefense(attack.location, attack.pulled_workers.size());

			}

			// BATTERY_OVERCHARGE
			if (mediator->IsBatteryOverchargeOffCooldown() && 
				attack.status <= -50 && 
				Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Self, IsUnit(BATTERY)), attack.location) < 8)
			{
				UseBatteryOvercharge(attack.location);
			}

			// increase desired defenders
			if (close_allies.size() >= mediator->GetArmyGroupDefendingBase(attack.location)->desired_units)
			{
				if (attack.status < -100)
					mediator->AddToDefense(attack.location, 5);
				else if (attack.status < -50)
					mediator->AddToDefense(attack.location, 4);
				else if (attack.status < 0)
					mediator->AddToDefense(attack.location, 2);
			}

			// make defensive building(s)
		}
	}
	if (!scary_attack)
	{
		// make sure build is continuing
		mediator->UnPauseBuildOrder();

		// stop unnecessary production
		if (reset_warpgate_production)
			mediator->CancelWarpgateUnitProduction();
		if (reset_robo_production)
			mediator->CancelRoboUnitProduction();
		if (reset_stargate_production)
			mediator->CancelStargateUnitProduction();

		reset_warpgate_production = false;
		reset_robo_production = false;
		reset_stargate_production = false;

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

	
	int max_runs = 20;
	while (enemy_units.size() > 0 && friendly_units.size() > 0 && max_runs > 0)
	{
		int melee_attacks_this_round = 0;
		bool enemy_damage_done = false;
		bool friendly_damage_done = false;
		for (int i = enemy_units.size() - 1; i >= 0; i--)
		{
			if (enemy_units[i]->unit_type == ORACLE && (enemy_units[i]->energy - 10) / 2 <= 20 - max_runs)
				continue;

			float dps = 0;
			const Unit* curr_target = nullptr;
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
			if (curr_target == nullptr) // couldnt find target to attack
				continue;

			if (dps > 0)
				enemy_damage_done = true;

			if (Utility::IsMelee(enemy_units[i]->unit_type)) // account for melee units needing to reach their target and only a certain amount can attack at once
			{
				if (sim_city)
				{
					if (melee_attacks_this_round > 1)
						dps = 0;
					else
						melee_attacks_this_round++;
				}
				else
				{
					dps /= 1.5;
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

			float dps = 0;
			const Unit* curr_target = nullptr;
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
			if (curr_target == nullptr) // couldnt find target to attack
				continue;

			if (friendly_units[i]->unit_type == ORACLE)
			{
				if (mediator->IsOracleBeamActive(friendly_units[i]))
				{
					if ((friendly_units[i]->energy / 2) - (20 - max_runs) <= 0)
						dps = 0;
				}
				else if (friendly_units[i]->energy >= 40) // TODO maybe dont take into account this 40 energy cutoff to switch the oracle on?
				{
					if (((friendly_units[i]->energy - 25) / 2) - (20 - max_runs) <= 0)
						dps = 0;
				}
			}

			if (dps > 0)
				friendly_damage_done = true;

			if (Utility::IsMelee(friendly_units[i]->unit_type)) // account for melee units needing to reach their target and only a certain amount can attack at once
				dps /= 1.5;

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

void DefenseManager::UseBatteryOvercharge(Point2D location)  // BATTERY_OVERCHARGE
{
	if (mediator->GetCurrentTime() - last_time_overcharge_used <= 60)
		return;

	Units fighting_units = mediator->GetUnits(IsFightingUnit(Unit::Alliance::Self));
	Units batteries = mediator->GetUnits(Unit::Alliance::Self, IsUnit(BATTERY));
	const Unit* nexus = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS)), location);

	if (nexus == nullptr || Distance2D(nexus->pos, location) > 5) // arbitrary distance
		return;

	for (int i = batteries.size() - 1; i >= 0; i--)
	{
		if (Distance2D(batteries[i]->pos, nexus->pos) > 10.5)
			batteries.erase(batteries.begin() + i);
	}

	if (batteries.size() == 0)
		return;

	std::sort(batteries.begin(), batteries.end(), [fighting_units](const Unit*& a, const Unit*& b) -> bool
	{
		return Utility::GetUnitsWithin(fighting_units, a->pos, 6).size() > Utility::GetUnitsWithin(fighting_units, b->pos, 6).size();
	});

	mediator->SetUnitCommand(nexus, ABILITY_ID::BATTERYOVERCHARGE, batteries[0], 0);
	last_time_overcharge_used = mediator->GetCurrentTime();
}

void DefenseManager::RemoveOngoingAttackAt(Point2D location)
{
	for (int i = 0; i < ongoing_attacks.size(); i++)
	{
		if (ongoing_attacks[i].location == location)
		{
			ArmyGroup* army_group = mediator->GetArmyGroupDefendingBase(location);
			if (army_group)
			{
				for (const auto& worker : ongoing_attacks[i].pulled_workers)
				{
					army_group->RemoveUnit(worker);
					if (worker->is_alive)
						mediator->PlaceWorker(worker);
				}
				// reset desired defenses
				mediator->AddToDefense(location, -1 * army_group->desired_units);
			}
			ongoing_attacks.erase(ongoing_attacks.begin() + i);
			i--;
		}
	}
}

}