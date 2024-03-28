#pragma once
#include <string>

#include "sc2api/sc2_interfaces.h"




namespace sc2 {

class TossBot;

struct EnemyUnitInfo;

struct FriendlyUnitInfo
{
	const Unit* unit;
	std::vector<EnemyUnitInfo*> units_in_range;
};

struct EnemyUnitInfo
{
	const Unit* unit;
	std::vector<FriendlyUnitInfo*> units_in_range;
	int priority;
	int health;
	int total_damage_possible;
	float dps;
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
	TossBot* agent;
	std::vector<FriendlyUnitInfo*> friendly_units;
	std::vector<EnemyUnitInfo*> enemy_units;
	std::map<const Unit*, const Unit*> attacks;
	std::vector<UNIT_TYPEID> priority;

	FireControl(TossBot*, std::map<const Unit*, std::vector<const Unit*>>, std::vector<UNIT_TYPEID>);
	FireControl(TossBot*, std::map<const Unit*, Units>, std::map<const Unit*, int>, std::vector<UNIT_TYPEID>);

	EnemyUnitInfo* GetEnemyUnitInfo(const Unit*);
	FriendlyUnitInfo* GetFriendlyUnitInfo(const Unit*);
	int GetDamage(const Unit*, const Unit*);

	bool ApplyAttack(FriendlyUnitInfo*, EnemyUnitInfo*);
	bool ApplyDamage(EnemyUnitInfo*, int);
	void RemoveFriendlyUnit(FriendlyUnitInfo*);
	void RemoveEnemyUnit(EnemyUnitInfo*);

	std::map<const Unit*, const Unit*> FindAttacks();
};

class PersistentFireControl
{
public:
	TossBot* agent;
	Units friendly_units;
	int event_id;
	std::map<const Unit*, int> enemy_unit_hp;
	std::vector<OutgoingDamage> outgoing_attacks; // enemy unit, damage, frame

	PersistentFireControl() {};
	PersistentFireControl(TossBot*);

	void OnUnitTakesDamageListener(const Unit*, float, float);
	void OnUnitEntersVisionListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);

	void AddFriendlyUnit(const Unit*);
	void ApplyAttack(const Unit*, const Unit*);
	void ConfirmAttack(const Unit*, const Unit*);
	void UpdateEnemyUnitHealth();

	std::map<const Unit*, const Unit*> FindAttacks(std::vector<UNIT_TYPEID>);
	std::map<const Unit*, const Unit*> FindAttacks(Units, std::vector<UNIT_TYPEID>, float);

};

class EnemyMinHeap
{
public:
	std::vector<EnemyUnitInfo*> arr;
	int size;
	int capacity;

	EnemyMinHeap(int);
	int Parent(int);
	int LeftChild(int);
	int RightChild(int);
	EnemyUnitInfo* GetMin();
	void Insert(EnemyUnitInfo*);
	void Heapify(int);
	void DeleteMinimum();
	void DeleteElement(int);
	void DecreaseKey(int);
	bool TestOrder(EnemyUnitInfo*, EnemyUnitInfo*);
};

class FriendlyMinHeap
{
public:
	std::vector<FriendlyUnitInfo*> arr;
	int size;
	int capacity;

	FriendlyMinHeap(int);
	int Parent(int);
	int LeftChild(int);
	int RightChild(int);
	FriendlyUnitInfo* GetMin();
	void Insert(FriendlyUnitInfo*);
	void Heapify(int);
	void DeleteMinimum();
	void DeleteElement(int);
	void DecreaseKey(int);
};

}
