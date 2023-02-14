#pragma once
#include "fire_control.h"




namespace sc2 {

	FireControl::FireControl(TossBot* agent, std::map<const Unit*, std::vector<const Unit*>> units, std::vector<UNIT_TYPEID> priority)
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
		return agent->GetDamage(Funit, Eunit);
	}

	bool FireControl::ApplyAttack(FriendlyUnitInfo* friendly_unit, EnemyUnitInfo* enemy_unit)
	{
		int damage = GetDamage(friendly_unit->unit, enemy_unit->unit);
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
		for (const auto &unit : friendly_units)
		{
			if (unit->units_in_range.size() == 1)
				ApplyAttack(unit, unit->units_in_range[0]);
		}
		/*auto Fcmp = [](std::pair<const Unit*, FriendlyUnit> const & a, std::pair<const Unit*, FriendlyUnit> const & b)
		{
			return a.second.enemy_units_in_range.size() < b.second.enemy_units_in_range.size();
		};

		auto Ecmp = [](std::pair<const Unit*, EnemyUnit> const & a, std::pair<const Unit*, EnemyUnit> const & b)
		{
			return a.second.friendly_units_in_range.size() < b.second.friendly_units_in_range.size();
		};*/

		// order enemy units by number of friendly units that can hit them
		EnemyMinHeap enemy_units_ordered = EnemyMinHeap(enemy_units.size());
		for (const auto &unit : enemy_units)
		{
			enemy_units_ordered.Insert(unit);
		}

		while (enemy_units_ordered.size > 0)
		{
			EnemyUnitInfo* current_enemy = enemy_units_ordered.GetMin();
			enemy_units_ordered.DeleteMinimum();

			// remove any units that cant be killed
			if (current_enemy->total_damage_possible < current_enemy->health)
			{
				continue;
			}
			else
			{
				// order friendly units that can hit the current enemy unit by number of enemies they can hit
				// ApplyAttack
				FriendlyMinHeap friendly_units_ordered = FriendlyMinHeap(current_enemy->units_in_range.size());
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
			}
		}

		FriendlyMinHeap friendly_units_ordered = FriendlyMinHeap(friendly_units.size());
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

		return attacks;
	}

#pragma region EnemyMinHeap

	EnemyMinHeap::EnemyMinHeap(int capacity) {
		arr.reserve(capacity);
		this->capacity = capacity;
		size = 0;
	}

	int EnemyMinHeap::Parent(int i) {
		return (i - 1) / 2;
	}

	int EnemyMinHeap::LeftChild(int i) {
		return (2 * i + 1);
	}

	int EnemyMinHeap::RightChild(int i) {
		return (2 * i + 2);
	}

	EnemyUnitInfo* EnemyMinHeap::GetMin() {
		return arr[0];
	}

	void EnemyMinHeap::Insert(EnemyUnitInfo* element) {
		if (size == capacity) {
			std::cout << "Cannot insert. Heap is already full!\n";
			return;
		}
		// We can add it. Increase the size and add it to the end
		size++;
		arr.push_back(element);

		// Keep swapping until we reach the root
		int curr = size - 1;
		// As long as you aren't in the root node, and while the 
		// parent of the last element is greater than it
		while (curr > 0 && arr[Parent(curr)]->units_in_range.size() > arr[curr]->units_in_range.size()) {
			// Swap
			EnemyUnitInfo* temp = arr[Parent(curr)];
			arr[Parent(curr)] = arr[curr];
			arr[curr] = temp;
			// Update the current index of element
			curr = Parent(curr);
		}
	}

	void EnemyMinHeap::Heapify(int index) {
		// Rearranges the heap as to maintain
		// the min-heap property
		if (size <= 1)
			return;

		int left = LeftChild(index);
		int right = RightChild(index);

		// Variable to get the smallest element of the subtree
		// of an element an index
		int smallest = index;

		// If the left child is smaller than this element, it is
		// the smallest
		if (left < size && arr[left]->units_in_range.size() < arr[index]->units_in_range.size())
			smallest = left;

		// Similarly for the right, but we are updating the smallest element
		// so that it will definitely give the least element of the subtree
		if (right < size && arr[right]->units_in_range.size() < arr[smallest]->units_in_range.size())
			smallest = right;

		// Now if the current element is not the smallest,
		// swap with the current element. The min heap property
		// is now satisfied for this subtree. We now need to
		// recursively keep doing this until we reach the root node,
		// the point at which there will be no change!
		if (smallest != index)
		{
			EnemyUnitInfo* temp = arr[index];
			arr[index] = arr[smallest];
			arr[smallest] = temp;
			Heapify(smallest);
		}
	}

	void EnemyMinHeap::DeleteMinimum() {
		// Deletes the minimum element, at the root
		if (size == 0)
			return;

		EnemyUnitInfo* last_element = arr[size - 1];

		// Update root value with the last element
		arr[0] = last_element;

		arr.pop_back();
		size--;

		Heapify(0);
	}

	void EnemyMinHeap::DeleteElement(int index) {
		// Deletes an element, indexed by index
		// Ensure that it's lesser than the current root
		arr[index]->units_in_range.clear();

		// Now keep swapping, until we update the tree
		int curr = index;
		while (curr > 0 && arr[Parent(curr)]->units_in_range.size() > arr[curr]->units_in_range.size()) {
			EnemyUnitInfo* temp = arr[Parent(curr)];
			arr[Parent(curr)] = arr[curr];
			arr[curr] = temp;
			curr = Parent(curr);
		}

		// Now simply delete the minimum element
		DeleteMinimum();
	}

	// Decreases value of key at index 'i' to new_val.  It is assumed that
	// new_val is smaller than harr[i].
	void EnemyMinHeap::DecreaseKey(int index)
	{
		//arr[index].id = new_val;
		while (index != 0 && arr[Parent(index)]->units_in_range.size() > arr[index]->units_in_range.size())
		{
			EnemyUnitInfo* temp = arr[Parent(index)];
			arr[Parent(index)] = arr[index];
			arr[index] = temp;
			index = Parent(index);
		}
	}

#pragma endregion

#pragma region FriendlyMinHeap

	FriendlyMinHeap::FriendlyMinHeap(int capacity) {
		arr.reserve(capacity);
		this->capacity = capacity;
		size = 0;
	}

	int FriendlyMinHeap::Parent(int i) {
		return (i - 1) / 2;
	}

	int FriendlyMinHeap::LeftChild(int i) {
		return (2 * i + 1);
	}

	int FriendlyMinHeap::RightChild(int i) {
		return (2 * i + 2);
	}

	FriendlyUnitInfo* FriendlyMinHeap::GetMin() {
		return arr[0];
	}

	void FriendlyMinHeap::Insert(FriendlyUnitInfo* element) {
		if (size == capacity) {
			std::cout << "Cannot insert. Heap is already full!\n";
			return;
		}
		// We can add it. Increase the size and add it to the end
		size++;
		arr.push_back(element);

		// Keep swapping until we reach the root
		int curr = size - 1;
		// As long as you aren't in the root node, and while the 
		// parent of the last element is greater than it
		while (curr > 0 && arr[Parent(curr)]->units_in_range.size() > arr[curr]->units_in_range.size()) {
			// Swap
			FriendlyUnitInfo* temp = arr[Parent(curr)];
			arr[Parent(curr)] = arr[curr];
			arr[curr] = temp;
			// Update the current index of element
			curr = Parent(curr);
		}
	}

	void FriendlyMinHeap::Heapify(int index) {
		// Rearranges the heap as to maintain
		// the min-heap property
		if (size <= 1)
			return;

		int left = LeftChild(index);
		int right = RightChild(index);

		// Variable to get the smallest element of the subtree
		// of an element an index
		int smallest = index;

		// If the left child is smaller than this element, it is
		// the smallest
		if (left < size && arr[left]->units_in_range.size() < arr[index]->units_in_range.size())
			smallest = left;

		// Similarly for the right, but we are updating the smallest element
		// so that it will definitely give the least element of the subtree
		if (right < size && arr[right]->units_in_range.size() < arr[smallest]->units_in_range.size())
			smallest = right;

		// Now if the current element is not the smallest,
		// swap with the current element. The min heap property
		// is now satisfied for this subtree. We now need to
		// recursively keep doing this until we reach the root node,
		// the point at which there will be no change!
		if (smallest != index)
		{
			FriendlyUnitInfo* temp = arr[index];
			arr[index] = arr[smallest];
			arr[smallest] = temp;
			Heapify(smallest);
		}
	}

	void FriendlyMinHeap::DeleteMinimum() {
		// Deletes the minimum element, at the root
		if (size == 0)
			return;

		FriendlyUnitInfo* last_element = arr[size - 1];

		// Update root value with the last element
		arr[0] = last_element;

		arr.pop_back();
		size--;

		Heapify(0);
	}

	void FriendlyMinHeap::DeleteElement(int index) {
		// Deletes an element, indexed by index
		// Ensure that it's lesser than the current root
		arr[index]->units_in_range.clear();

		// Now keep swapping, until we update the tree
		int curr = index;
		while (curr > 0 && arr[Parent(curr)]->units_in_range.size() > arr[curr]->units_in_range.size()) {
			FriendlyUnitInfo* temp = arr[Parent(curr)];
			arr[Parent(curr)] = arr[curr];
			arr[curr] = temp;
			curr = Parent(curr);
		}

		// Now simply delete the minimum element
		DeleteMinimum();
	}

	// Decreases value of key at index 'i' to new_val.  It is assumed that
	// new_val is smaller than harr[i].
	void FriendlyMinHeap::DecreaseKey(int index)
	{
		//arr[index].id = new_val;
		while (index != 0 && arr[Parent(index)]->units_in_range.size() > arr[index]->units_in_range.size())
		{
			FriendlyUnitInfo* temp = arr[Parent(index)];
			arr[Parent(index)] = arr[index];
			arr[index] = temp;
			index = Parent(index);
		}
	}

#pragma endregion
	
}