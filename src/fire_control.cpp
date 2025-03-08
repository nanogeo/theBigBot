#pragma once
#include "fire_control.h"
#include "utility.h"
#include "theBigBot.h"

#include <fstream>
#include <chrono>



namespace sc2 {

	FireControl::FireControl(TheBigBot* agent, std::map<const Unit*, Units> units, std::vector<UNIT_TYPEID> priority)
	{
		this->agent = agent;
		this->priority = priority;
		for (const auto &Funit : units)
		{
			FriendlyUnitInfo* new_unit = new FriendlyUnitInfo();
			new_unit->unit = Funit.first;
			for (const auto &Eunit : Funit.second)
			{
				EnemyUnitInfo* info = GetEnemyUnitInfo(Eunit);
				if (info == NULL)
				{
					info = new EnemyUnitInfo();
					info->unit = Eunit;
					info->health = Eunit->health + Eunit->shield;
					auto index = std::find(priority.begin(), priority.end(), Eunit->unit_type.ToType());
					if (index == priority.end())
						info->priority = priority.size();
					else
						info->priority = index - priority.begin();
					info->units_in_range.push_back(new_unit);
					info->total_damage_possible = GetDamage(Funit.first, Eunit);
					info->dps = Utility::GetDPS(Eunit);

					enemy_units.push_back(info);
				}
				else
				{
					info->units_in_range.push_back(new_unit);
					info->total_damage_possible += GetDamage(Funit.first, Eunit);
				}
				new_unit->units_in_range.push_back(info);
			}
			friendly_units.push_back(new_unit);
		}
	}

	FireControl::FireControl(TheBigBot* agent, std::map<const Unit*, Units> units, std::map<const Unit*, int> enemy_health, std::vector<UNIT_TYPEID> priority)
	{
		this->agent = agent;
		this->priority = priority;
		for (const auto& Funit : units)
		{
			FriendlyUnitInfo* new_unit = new FriendlyUnitInfo();
			new_unit->unit = Funit.first;
			for (const auto& Eunit : Funit.second)
			{
				EnemyUnitInfo* info = GetEnemyUnitInfo(Eunit);
				if (info == NULL)
				{
					info = new EnemyUnitInfo();
					info->unit = Eunit;
					info->health = enemy_health[Eunit];
					auto index = std::find(priority.begin(), priority.end(), Eunit->unit_type.ToType());
					if (index == priority.end())
						info->priority = priority.size();
					else
						info->priority = index - priority.begin();
					info->units_in_range.push_back(new_unit);
					info->total_damage_possible = GetDamage(Funit.first, Eunit);
					info->dps = Utility::GetDPS(Eunit);

					enemy_units.push_back(info);
				}
				else
				{
					info->units_in_range.push_back(new_unit);
					info->total_damage_possible += GetDamage(Funit.first, Eunit);
				}
				new_unit->units_in_range.push_back(info);
			}
			friendly_units.push_back(new_unit);
		}
	}

	FireControl::FireControl(TheBigBot* agent, std::map<const Unit*, Units> units, std::map<const Unit*, int> enemy_health, std::vector<std::vector<UNIT_TYPEID>> priority)
	{
		this->agent = agent;
		this->priority2D = priority;
		for (const auto& Funit : units)
		{
			FriendlyUnitInfo* new_unit = new FriendlyUnitInfo();
			new_unit->unit = Funit.first;
			for (const auto& Eunit : Funit.second)
			{
				EnemyUnitInfo* info = GetEnemyUnitInfo(Eunit);
				if (info == NULL)
				{
					info = new EnemyUnitInfo();
					info->unit = Eunit;
					info->health = enemy_health[Eunit];
					info->priority = priority.size();
					for (int i = 0; i < priority.size(); i++)
					{
						auto index = std::find(priority[i].begin(), priority[i].end(), Eunit->unit_type.ToType());
						if (index != priority[i].end())
						{
							info->priority = i;
							break;
						}
					}
					info->units_in_range.push_back(new_unit);
					info->total_damage_possible = GetDamage(Funit.first, Eunit);
					info->dps = Utility::GetDPS(Eunit);

					enemy_units.push_back(info);
				}
				else
				{
					info->units_in_range.push_back(new_unit);
					info->total_damage_possible += GetDamage(Funit.first, Eunit);
				}
				new_unit->units_in_range.push_back(info);
			}
			friendly_units.push_back(new_unit);
		}
	}

	EnemyUnitInfo* FireControl::GetEnemyUnitInfo(const Unit* unit)
	{
		for (auto &unit_info : enemy_units)
		{
			if (unit_info->unit == unit)
				return unit_info;
		}
		return NULL;
	}

	FriendlyUnitInfo* FireControl::GetFriendlyUnitInfo(const Unit* unit)
	{
		for (auto &unit_info : friendly_units)
		{
			if (unit_info->unit == unit)
				return unit_info;
		}
		return NULL;
	}

	int FireControl::GetDamage(const Unit* Funit, const Unit* Eunit)
	{
		return Utility::GetDamage(Funit, Eunit, agent->Observation());
	}

	bool FireControl::ApplyAttack(FriendlyUnitInfo* friendly_unit, EnemyUnitInfo* enemy_unit)
	{
		int damage = Utility::GetDamage(friendly_unit->unit, enemy_unit->unit, agent->Observation());
		bool Eunit_died = ApplyDamage(enemy_unit, damage);
		attacks[friendly_unit->unit] = enemy_unit->unit;
		RemoveFriendlyUnit(friendly_unit);
		return Eunit_died;
	}

	bool FireControl::ApplyDamage(EnemyUnitInfo* enemy_unit, int damage)
	{
		int new_health = enemy_unit->health -= damage;
		if (new_health <= 0)
		{
			RemoveEnemyUnit(enemy_unit);
			return true;
		}
		return false;
	}

	void FireControl::RemoveFriendlyUnit(FriendlyUnitInfo* friendly_unit)
	{
		for (const auto &Eunit : friendly_unit->units_in_range)
		{
			int damage = GetDamage(friendly_unit->unit, Eunit->unit);
			Eunit->total_damage_possible -= damage;
			Eunit->units_in_range.erase(std::remove(Eunit->units_in_range.begin(), Eunit->units_in_range.end(), friendly_unit), Eunit->units_in_range.end());
		}
		friendly_units.erase(std::remove(friendly_units.begin(), friendly_units.end(), friendly_unit), friendly_units.end());
	}

	void FireControl::RemoveEnemyUnit(EnemyUnitInfo* enemy_unit)
	{
		for (const auto &Funit : enemy_unit->units_in_range)
		{
			Funit->units_in_range.erase(std::remove(Funit->units_in_range.begin(), Funit->units_in_range.end(), enemy_unit), Funit->units_in_range.end());
		}
		enemy_units.erase(std::remove(enemy_units.begin(), enemy_units.end(), enemy_unit), enemy_units.end());
	}

	std::map<const Unit*, const Unit*> FireControl::FindAttacks()
	{
#ifdef DEBUG_TIMING
		unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();

		std::ofstream fire_control_time;
		fire_control_time.open("fire_control_time.txt", std::ios_base::app);

		unsigned long long single_target = 0;
		unsigned long long enemy_min_heap = 0;
		unsigned long long friendly_min_heap = 0;
#endif

		for (const auto &unit : friendly_units)
		{
			if (unit->units_in_range.size() == 1)
				ApplyAttack(unit, unit->units_in_range[0]);
		}

#ifdef DEBUG_TIMING
		single_target = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count() - start_time;

		start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
#endif

		// order enemy units by number of friendly units that can hit them
		MinHeap<EnemyUnitInfo*> enemy_units_ordered = MinHeap<EnemyUnitInfo*>(enemy_units.size());
		//EnemyMinHeap enemy_units_ordered = EnemyMinHeap(enemy_units.size());
		for (const auto &unit : enemy_units)
		{
			enemy_units_ordered.Insert(unit);
		}

		while (enemy_units_ordered.size > 0)
		{
			EnemyUnitInfo* current_enemy = enemy_units_ordered.GetMin();
			enemy_units_ordered.DeleteMinimum();

			// remove any units that cant be killed
			/*if (current_enemy->total_damage_possible < current_enemy->health)
			{
				continue;
			}
			else*/
			//{
				// order friendly units that can hit the current enemy unit by number of enemies they can hit
				// ApplyAttack
				MinHeap<FriendlyUnitInfo*> friendly_units_ordered = MinHeap<FriendlyUnitInfo*>(current_enemy->units_in_range.size());
				for (const auto &unit : current_enemy->units_in_range)
				{
					friendly_units_ordered.Insert(unit);
				}
				while (friendly_units_ordered.size > 0 && current_enemy->health > 0)
				{
					FriendlyUnitInfo* current_friendly = friendly_units_ordered.GetMin();
					ApplyAttack(current_friendly, current_enemy);
					friendly_units_ordered.DeleteMinimum();
				}
				enemy_units_ordered.Heapify(0);
			//}
		}

#ifdef DEBUG_TIMING
		enemy_min_heap = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count() - start_time;

		start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
#endif

		MinHeap<FriendlyUnitInfo*>  friendly_units_ordered = MinHeap<FriendlyUnitInfo*>(friendly_units.size());
		for (const auto &unit : friendly_units)
		{
			friendly_units_ordered.Insert(unit);
		}
		while (friendly_units_ordered.size > 0)
		{
			FriendlyUnitInfo* current_unit = friendly_units_ordered.GetMin();
			friendly_units_ordered.DeleteMinimum();

			if (current_unit->units_in_range.size() == 0)
				RemoveFriendlyUnit(current_unit);
			else
			{
				std::sort(current_unit->units_in_range.begin(), current_unit->units_in_range.end(),
					[](const EnemyUnitInfo* a, const EnemyUnitInfo* b) -> bool
				{
					if (a->priority == b->priority)
						return a->health < b->health;
					return a->priority < b->priority;
				});
				ApplyAttack(current_unit, current_unit->units_in_range[0]);
			}
		}
#ifdef DEBUG_TIMING
		friendly_min_heap = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count() - start_time;

		fire_control_time << single_target << ", ";
		fire_control_time << enemy_min_heap << ", ";
		fire_control_time << friendly_min_heap << "\n";
		fire_control_time.close();
#endif

		return attacks;
	}


#pragma region PersistentFireControl

	PersistentFireControl::PersistentFireControl(TheBigBot* agent)
	{
		this->agent = agent;

		event_id = agent->GetUniqueId();
		std::function<void(const Unit*, float, float)> onUnitTakesDamage = [=](const Unit* unit, float health, float shields) {
			this->OnUnitTakesDamageListener(unit, health, shields);
		};
		agent->AddListenerToOnUnitDamagedEvent(event_id, onUnitTakesDamage);

		std::function<void(const Unit*)> onUnitEntersVision = [=](const Unit* unit) {
			this->OnUnitEntersVisionListener(unit);
		};
		agent->AddListenerToOnUnitEntersVisionEvent(event_id, onUnitEntersVision);

		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);
	}

	PersistentFireControl::~PersistentFireControl()
	{
		agent->RemoveListenerToOnUnitDamagedEvent(event_id);
		agent->RemoveListenerToOnUnitEntersVisionEvent(event_id);
		agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
	}

	void PersistentFireControl::OnUnitTakesDamageListener(const Unit* unit, float health, float shields)
	{
		float total_damage = health + shields;
		for (int i = 0; i < outgoing_attacks.size(); i++)
		{
			if (outgoing_attacks[i].target == unit && outgoing_attacks[i].confirmend)
			{
				if (total_damage == outgoing_attacks[i].damage)
				{
					outgoing_attacks.erase(outgoing_attacks.begin() + i);
					return;
				}
				else if (total_damage > outgoing_attacks[i].damage)
				{
					total_damage -= outgoing_attacks[i].damage;
					outgoing_attacks.erase(outgoing_attacks.begin() + i);
					i--;
				}
				else if (total_damage < outgoing_attacks[i].damage)
				{
					outgoing_attacks[i].damage = outgoing_attacks[i].damage - total_damage;
					return;
				}
			}
		}
	}

	void PersistentFireControl::OnUnitDestroyedListener(const Unit* unit)
	{
		enemy_unit_hp.erase(unit);
		for(int i = 0; i < outgoing_attacks.size(); i++)
		{
			if (outgoing_attacks[i].target)
			{
				outgoing_attacks.erase(outgoing_attacks.begin() + i);
				i--;
			}
		}
	}

	void PersistentFireControl::OnUnitEntersVisionListener(const Unit* unit)
	{
		if (enemy_unit_hp.count(unit) == 0)
			enemy_unit_hp[unit] = unit->health + unit->shield + 1;
	}

	void PersistentFireControl::AddFriendlyUnit(const Unit* unit)
	{
		friendly_units.push_back(unit);
	}

	void PersistentFireControl::ApplyAttack(const Unit* attacker, const Unit* target)
	{
		int damage = Utility::GetDamage(attacker, target, agent->Observation());
		enemy_unit_hp[target] = enemy_unit_hp[target] - damage;
		outgoing_attacks.push_back(OutgoingDamage(attacker, target, damage, agent->Observation()->GetGameLoop() + 20)); // TODO calculate frame of hit
	}

	void PersistentFireControl::ConfirmAttack(const Unit* attacker, const Unit* target)
	{
		bool already_confirmed = false;
		for (int i = 0; i < outgoing_attacks.size(); i++)
		{
			if (outgoing_attacks[i].attacker == attacker)
			{
				if (outgoing_attacks[i].target != target || already_confirmed)
				{
					outgoing_attacks.erase(outgoing_attacks.begin() + i);
					i--;
				}
				else
				{
					already_confirmed = true;
					outgoing_attacks[i].confirmend = true;
				}
			}
		}
	}

	void PersistentFireControl::UpdateEnemyUnitHealth()
	{
		for (const auto unit_hp : enemy_unit_hp)
		{
			if (agent->mediator.GetEnemyRace() == Race::Zerg)
			{
				enemy_unit_hp[unit_hp.first] = ceil(unit_hp.first->health + unit_hp.first->shield + 1);
			}
			else if (agent->mediator.GetEnemyRace() == Race::Terran && Utility::IsBiological(unit_hp.first->unit_type))
			{
				Units medivacs = agent->Observation()->GetUnits(IsUnit(MEDIVAC));
				if (medivacs.size() > 0 && Utility::DistanceToClosest(medivacs, unit_hp.first->pos) < 4)
					enemy_unit_hp[unit_hp.first] = ceil(unit_hp.first->health + unit_hp.first->shield + 10);
				else
					enemy_unit_hp[unit_hp.first] = ceil(unit_hp.first->health + unit_hp.first->shield);
			}
			else
			{
				enemy_unit_hp[unit_hp.first] = ceil(unit_hp.first->health + unit_hp.first->shield + 1);
			}
		}

		for (int i = 0; i < outgoing_attacks.size(); i++)
		{
			if (outgoing_attacks[i].frame_of_hit < agent->Observation()->GetGameLoop())
			{
				outgoing_attacks.erase(outgoing_attacks.begin() + i);
				i--;
			}
		}

		for (const auto damage : outgoing_attacks)
		{
			enemy_unit_hp[damage.target] = enemy_unit_hp[damage.target] - damage.damage;
		}
	}

	std::map<const Unit*, const Unit*> PersistentFireControl::FindAttacks(std::vector<UNIT_TYPEID> prio)
	{
		std::map<const Unit*, std::vector<const Unit*>> unit_targets;
		Units Eunits = agent->Observation()->GetUnits(Unit::Alliance::Enemy);
		
		for (const auto& unit : friendly_units)
		{
			Units units_in_range;
			for (const auto& Eunit : Eunits)
			{
				if (Distance2D(unit->pos, Eunit->pos) <= Utility::RealRange(unit, Eunit))
					units_in_range.push_back(Eunit);
			}
			unit_targets[unit] = units_in_range;
		}

		FireControl fire_control = FireControl(agent, unit_targets, enemy_unit_hp, prio);
		std::map<const Unit*, const Unit*> attacks = fire_control.FindAttacks();
		for (const auto attack : attacks)
		{
			ApplyAttack(attack.first, attack.second);
		}
		return attacks;
	}

	std::map<const Unit*, const Unit*> PersistentFireControl::FindAttacks(Units units, std::vector<std::vector<UNIT_TYPEID>> prio, float max_extra_range)
	{
		UpdateEnemyUnitHealth();
		std::map<const Unit*, std::vector<const Unit*>> unit_targets;
		Units Eunits = agent->Observation()->GetUnits(Unit::Alliance::Enemy);

		for (const auto& unit : units)
		{
			Units units_in_range;
			float extra_range = 0;
			bool found_non_building = false;
			while (!found_non_building && units_in_range.size() == 0 && extra_range <= max_extra_range)
			{
				for (const auto& Eunit : Eunits)
				{
					if (enemy_unit_hp[Eunit] <= 0 || std::find(units_in_range.begin(), units_in_range.end(), Eunit) != units_in_range.end())
						continue;
					if (Distance2D(unit->pos, Eunit->pos) <= Utility::RealRange(unit, Eunit) + extra_range)
					{
						units_in_range.push_back(Eunit);
						if (!Eunit->is_building)
							found_non_building = true;
					}
				}
				extra_range += .5;
			}
			unit_targets[unit] = units_in_range;
		}

		FireControl fire_control = FireControl(agent, unit_targets, enemy_unit_hp, prio);
		std::map<const Unit*, const Unit*> attacks = fire_control.FindAttacks();
		for (const auto attack : attacks)
		{
			ApplyAttack(attack.first, attack.second);
		}
		return attacks;
	}



#pragma endregion


}