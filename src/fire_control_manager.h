#pragma once
#include <string>

#include "sc2api/sc2_interfaces.h"




namespace sc2 {

class TheBigBot;
class Mediator;

struct EnemyUnitInfo;

struct FriendlyUnitInfo
{
	const Unit* unit;
	std::vector<EnemyUnitInfo*> units_in_range;
	bool operator>(const FriendlyUnitInfo& rhs) const
	{
		return this->units_in_range.size() > rhs.units_in_range.size();
	}
	bool operator<(const FriendlyUnitInfo& rhs) const
	{
		return this->units_in_range.size() < rhs.units_in_range.size();
	}
};

struct EnemyUnitInfo
{
	const Unit* unit;
	std::vector<FriendlyUnitInfo*> units_in_range;
	int priority;
	int health;
	int total_damage_possible;
	float dps;

	// > mean lower priority
	bool operator>(const EnemyUnitInfo& rhs) const
	{
		if (this->priority < rhs.priority)
			return false;
		if (rhs.priority < this->priority)
			return true;

		/*if (this->dps / this->health > rhs.dps / rhs.health)
			return false;
		if (rhs.dps / rhs.health > this->dps / this->health)
			return true;*/

		if (this->total_damage_possible >= this->health)
		{
			if (rhs.total_damage_possible >= rhs.health)
			{
				if (this->units_in_range.size() == rhs.units_in_range.size())
				{
					return this->health / (this->unit->health_max + this->unit->shield_max) > rhs.health / (rhs.unit->health_max + rhs.unit->shield_max);
				}
				return this->units_in_range.size() < rhs.units_in_range.size();
			}
			return false;
		}
		else if (rhs.total_damage_possible >= rhs.health)
		{
			return true;
		}

		if (this->units_in_range.size() == rhs.units_in_range.size())
		{
			return this->health / (this->unit->health_max + this->unit->shield_max) > rhs.health / (rhs.unit->health_max + rhs.unit->shield_max);
		}
		return this->units_in_range.size() < rhs.units_in_range.size();
	}

	// < mean higher priority
	bool operator<(const EnemyUnitInfo& rhs) const
	{
		if (rhs.priority < this->priority)
			return false;
		if (this->priority < rhs.priority)
			return true;

		/*if (rhs.dps / rhs.health > this->dps / this->health)
			return false;
		if (this->dps / this->health > rhs.dps / rhs.health)
			return true;*/

		if (rhs.total_damage_possible >= rhs.health)
		{
			if (this->total_damage_possible >= this->health)
			{
				if (rhs.units_in_range.size() == this->units_in_range.size())
				{
					return rhs.health / (rhs.unit->health_max + rhs.unit->shield_max) > this->health / (this->unit->health_max + this->unit->shield_max);
				}
				return rhs.units_in_range.size() < this->units_in_range.size();
			}
			return false;
		}
		else if (this->total_damage_possible >= this->health)
		{
			return true;
		}

		if (rhs.units_in_range.size() == this->units_in_range.size())
		{
			return rhs.health / (rhs.unit->health_max + rhs.unit->shield_max) > this->health / (this->unit->health_max + this->unit->shield_max);
		}
		return rhs.units_in_range.size() < this->units_in_range.size();
	}
};

struct OutgoingDamage
{
	const Unit* attacker;
	const Unit* target;
	int damage;
	int frame_of_hit;
	bool confirmend = false;
	OutgoingDamage(const Unit* attacker, const Unit* target, int damage, int frame_of_hit)
	{
		this->attacker = attacker;
		this->target = target;
		this->damage = damage;
		this->frame_of_hit = frame_of_hit;
	}
};

class FireControl
{
public:
	TheBigBot* agent;
	std::vector<FriendlyUnitInfo*> friendly_units;
	std::vector<EnemyUnitInfo*> enemy_units;
	std::map<const Unit*, const Unit*> attacks;
	std::vector<UNIT_TYPEID> priority;
	std::vector<std::vector<UNIT_TYPEID>> priority2D;

	FireControl(TheBigBot*, std::map<const Unit*, std::vector<const Unit*>>, std::vector<UNIT_TYPEID>); 
	FireControl(TheBigBot*, std::map<const Unit*, Units>, std::map<const Unit*, int>, std::vector<UNIT_TYPEID>);
	FireControl(TheBigBot*, std::map<const Unit*, Units>, std::map<const Unit*, int>, std::vector<std::vector<UNIT_TYPEID>>);

	EnemyUnitInfo* GetEnemyUnitInfo(const Unit*);
	FriendlyUnitInfo* GetFriendlyUnitInfo(const Unit*);
	int GetDamage(const Unit*, const Unit*);

	bool ApplyAttack(FriendlyUnitInfo*, EnemyUnitInfo*);
	bool ApplyDamage(EnemyUnitInfo*, int);
	void RemoveFriendlyUnit(FriendlyUnitInfo*);
	void RemoveEnemyUnit(EnemyUnitInfo*);

	std::map<const Unit*, const Unit*> FindAttacks();
};


class FireControlManager
{
public:
	Mediator* mediator;
	Units units_ready_to_attack;
	std::map<const Unit*, int> enemy_unit_hp;
	std::vector<OutgoingDamage> outgoing_attacks;
	std::map<const Unit*, bool> attack_status;

	FireControlManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void UpdateInfo();
	void UpdateEnemyUnitHealth();

	bool GetAttackStatus(const Unit*);
	void AddUnit(const Unit*);
	void ApplyAttack(const Unit*, const Unit*);
	void ConfirmAttack(const Unit*, const Unit*);
	void CancelAttack(const Unit*);

	void DoAttacks();

	void OnUnitTakesDamage(const Unit*, float, float);
	void OnUnitEntersVision(const Unit*);
	void OnUnitDestroyed(const Unit*);
	void OnUnitCreated(const Unit*);

};

}
