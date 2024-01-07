#pragma once
#include <string>

#include "sc2api/sc2_interfaces.h"

#include "TossBot.h"


namespace sc2 {

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
};

struct OutgoingDamage
{

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
	std::map<const Unit*, int> enemy_unit_hp;
	std::vector<std::tuple<const Unit*, int, int>> outgoing_damage; // enemy unit, damage, frame

	PersistentFireControl(TossBot* agent)
	{
		this->agent = agent;
	}

	void OnUnitCreatedListener(const Unit*);
	void OnUnitTakesDamageListener(const Unit*);
	void OnUnitEntersVisionListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);

	EnemyUnitInfo* GetEnemyUnitInfo(const Unit*);
	FriendlyUnitInfo* GetFriendlyUnitInfo(const Unit*);
	int GetDamage(const Unit*, const Unit*);

	bool ApplyAttack(FriendlyUnitInfo*, EnemyUnitInfo*);
	bool ApplyDamage(EnemyUnitInfo*, int);
	void RemoveFriendlyUnit(FriendlyUnitInfo*);
	void RemoveEnemyUnit(EnemyUnitInfo*);

	std::map<const Unit*, const Unit*> FindAttacks();

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
