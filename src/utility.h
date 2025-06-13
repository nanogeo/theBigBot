#pragma once

#include "path_manager.h"
#include "definitions.h"


namespace sc2
{


struct IsFinishedUnit {
    explicit IsFinishedUnit(UNIT_TYPEID type_);

    bool operator()(const Unit& unit_) const;

private:
    UNIT_TYPEID m_type;
};

struct IsNotFinishedUnit {
	explicit IsNotFinishedUnit(UNIT_TYPEID type_);

	bool operator()(const Unit& unit_) const;

private:
	UNIT_TYPEID m_type;
};

struct IsFightingUnit {
	explicit IsFightingUnit(Unit::Alliance alliance_);

	bool operator()(const Unit& unit_) const;

private:
	Unit::Alliance m_type;
};

struct IsNonbuilding {
	explicit IsNonbuilding(Unit::Alliance alliance_);

	bool operator()(const Unit& unit_) const;

private:
	Unit::Alliance m_type;
};

struct IsNonPlaceholderUnit {
	explicit IsNonPlaceholderUnit(UNIT_TYPEID type_);

	bool operator()(const Unit& unit_) const;

private:
	UNIT_TYPEID m_type;
};

struct IsFriendlyUnit {
	explicit IsFriendlyUnit(UNIT_TYPEID type_);

	bool operator()(const Unit& unit_) const;

private:
	UNIT_TYPEID m_type;
};

struct IsEnemyUnit {
	explicit IsEnemyUnit(UNIT_TYPEID type_);

	bool operator()(const Unit& unit_) const;

private:
	UNIT_TYPEID m_type;
};

struct IsFlyingUnit {
	explicit IsFlyingUnit();

	bool operator()(const Unit& unit_) const;
};

struct IsNotFlyingUnit {
	explicit IsNotFlyingUnit();

	bool operator()(const Unit& unit_) const;
};

struct IsNotCarryingResources {
	explicit IsNotCarryingResources(UNIT_TYPEID type_);

	bool operator()(const Unit& unit_) const;

private:
	UNIT_TYPEID m_type;
};


class Utility
{
public:
	static Point2D ToPoint2D(Point3D);
	static const Unit* ClosestTo(Units, Point2D);
	static const Unit* ClosestToLine(Units, LineSegmentLinearX);
	static const Unit* ClosestToLine(Units, Point2D, Point2D);
	static const Unit* NthClosestTo(Units, Point2D, int);
	static Units NClosestUnits(Units, Point2D, int);
	static Point2D ClosestTo(std::vector<Point2D>, Point2D);
	static const Unit* ClosestUnitTo(Units, Point2D);
	static Point2D NthClosestTo(std::vector<Point2D>, Point2D, int);
	static const Unit* FurthestFrom(Units, Point2D);
	static Point2D FurthestFrom(std::vector<Point2D>, Point2D);
	static float DistanceToClosest(Units, Point2D);
	static float DistanceToClosest(std::vector<Point2D>, Point2D);
	static float DistanceToClosestOnLine(Units, LineSegmentLinearX);
	static float DistanceToFurthest(Units, Point2D);
	static float DistanceToFurthest(std::vector<Point2D>, Point2D);
	static float AnyUnitWithin(Units, Point2D, float);
	static Units GetUnitsWithin(Units, Point2D, float);
	static Units GetUnitsInRange(Units, const Unit*, float);
	static Units GetUnitsThatCanAttack(Units, const Unit*, float);
	static Point2D ClosestPointOnLine(Point2D, Point2D, Point2D);
	static Units CloserThan(Units, float, Point2D);
	static bool HasBuff(const Unit*, BUFF_ID);
	static Point2D Center(Units);
	static Point2D Center(std::vector<Point2D>);
	static Point2D MedianCenter(Units);
	static Point2D MedianCenter(std::vector<Point2D>);
	static const Unit* GetMostDamagedUnit(Units);
	static Point2D PointBetween(Point2D, Point2D, float);
	static Point2D RunAwayCircle(Point2D, Point2D, float, float);
	static Point2D ClosestIntersectionTo(Point2D, double, Point2D, double, Point2D);
	static int DangerLevel(const Unit *, const ObservationInterface*);
	static int DangerLevelAt(const Unit *, Point2D, const ObservationInterface*);
	static int GetDamage(const Unit*, const Unit*);
	static float GetDPS(const Unit*);
	static float GetDPS(const Unit*, const Unit*);
	static int GetArmor(const Unit*);
	static int GetCargoSize(const Unit*);
	static float GetGroundRange(const Unit *);
	static float GetAirRange(const Unit*);
	static float RealRange(const Unit *, const Unit *);
	static float GetMaxGroundRange(Units);
	static float GetMaxAirRange(Units);
	static float GetDamagePoint(const Unit*);
	static float GetProjectileTime(const Unit*, float dist);
	static float GetWeaponCooldown(const Unit*);
	static bool IsOnHighGround(Point3D, Point3D);
	static float GetTimeBuilt(const Unit*, float);
	static AbilityID UnitToWarpInAbility(UNIT_TYPEID);
	static UNIT_TYPEID GetBuildStructure(UNIT_TYPEID);
	static int GetTrainingTime(UNIT_TYPEID);
	static bool IsFacing(const Unit*, const Unit*);
	static float GetFacingAngle(const Unit*, const Unit*);
	static const Unit* AimingAt(const Unit*, Units);
	static float BuildingSize(UNIT_TYPEID);
	static float GetLargestUnitSize(Units);
	static const Unit* GetLeastFullPrism(Units);
	static ABILITY_ID GetBuildAbility(UNIT_TYPEID);
	static ABILITY_ID GetTrainAbility(UNIT_TYPEID);
	static ABILITY_ID GetWarpAbility(UNIT_TYPEID);
	static ABILITY_ID GetUpgradeAbility(UPGRADE_ID);
	static int GetWarpCooldown(UNIT_TYPEID);
	static int BuildingsReady(UNIT_TYPEID, const ObservationInterface*);
	static UnitCost GetCost(UNIT_TYPEID);
	static UnitCost GetCost(UPGRADE_ID);
	static bool CanAfford(UNIT_TYPEID, int, const ObservationInterface*);
	static bool CanAffordAfter(UNIT_TYPEID, UnitCost, const ObservationInterface*);
	static bool CanAffordUpgrade(UPGRADE_ID, const ObservationInterface*);
	static int MaxCanAfford(UNIT_TYPEID, const ObservationInterface*);
	static std::vector<double> GetRealQuarticRoots(double, double, double, double, double);
	static bool IsBiological(UNIT_TYPEID);
	static bool IsMechanical(UNIT_TYPEID);
	static bool IsLight(UNIT_TYPEID);
	static bool IsArmored(UNIT_TYPEID);
	static bool IsMassive(UNIT_TYPEID);
	static bool IsStructure(UNIT_TYPEID);
	static bool IsMelee(UNIT_TYPEID);
	static bool OnSameLevel(Point3D, Point3D);

	static std::string AbilityIdToString(ABILITY_ID);

};

template <class T>
class MinHeap
{
public:
	std::vector<T> arr;
	int size;
	int capacity;

	MinHeap(int);
	int Parent(int);
	int LeftChild(int);
	int RightChild(int);
	T GetMin();
	void Insert(T);
	void Heapify(int);
	void DeleteMinimum();
	void DeleteElement(int);
	void DecreaseKey(int);
};

template <class T>
class MinHeap<T*>
{
public:
	std::vector<T*> arr;
	int size;
	int capacity;

	MinHeap(int capacity) {
		arr.reserve(capacity);
		this->capacity = capacity;
		size = 0;
	};
	int Parent(int i) {
		return (i - 1) / 2;
	};
	int LeftChild(int i) {
		return (2 * i + 1);
	};
	int RightChild(int i) {
		return (2 * i + 2);
	};
	T* GetMin() {
		return arr[0];
	};
	void Insert(T* element) {
		if (size == capacity) {
			//std::cout << "Cannot insert. Heap is already full!\n";
			return;
		}
		// We can add it. Increase the size and add it to the end
		size++;
		arr.push_back(element);

		// Keep swapping until we reach the root
		int curr = size - 1;
		// As long as you aren't in the root node, and while the 
		// parent of the last element is greater than it
		while (curr > 0 && *(arr[Parent(curr)]) > *(arr[curr])) {
			// Swap
			T* temp = arr[Parent(curr)];
			arr[Parent(curr)] = arr[curr];
			arr[curr] = temp;
			// Update the current index of element
			curr = Parent(curr);
		}
	};
	void Heapify(int index) {
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
		if (left < size && *(arr[index]) > *(arr[left]))
			smallest = left;

		// Similarly for the right, but we are updating the smallest element
		// so that it will definitely give the least element of the subtree
		if (right < size && *(arr[smallest]) > *(arr[right]))
			smallest = right;

		// Now if the current element is not the smallest,
		// swap with the current element. The min heap property
		// is now satisfied for this subtree. We now need to
		// recursively keep doing this until we reach the root node,
		// the point at which there will be no change!
		if (smallest != index)
		{
			T* temp = arr[index];
			arr[index] = arr[smallest];
			arr[smallest] = temp;
			Heapify(smallest);
		}
	};
	void DeleteMinimum() {
		// Deletes the minimum element, at the root
		if (size == 0)
			return;

		T* last_element = arr[size - 1];

		// Update root value with the last element
		arr[0] = last_element;

		arr.pop_back();
		size--;

		Heapify(0);
	};
	void DeleteElement(int index) {
		// Deletes an element, indexed by index
		// Ensure that it's lesser than the current root
		arr[index]->units_in_range.clear();

		// Now keep swapping, until we update the tree
		int curr = index;
		while (curr > 0 && *(arr[Parent(curr)]) > *(arr[curr])) {
			T* temp = arr[Parent(curr)];
			arr[Parent(curr)] = arr[curr];
			arr[curr] = temp;
			curr = Parent(curr);
		}

		// Now simply delete the minimum element
		DeleteMinimum();
	};
	void DecreaseKey(int index)
	{
		//arr[index].id = new_val;
		while (index != 0 && *(arr[Parent(index)]) > *(arr[index]))
		{
			T* temp = arr[Parent(index)];
			arr[Parent(index)] = arr[index];
			arr[index] = temp;
			index = Parent(index);
		}
	};
};


struct OrderedPoint2D : Point2D
{
	OrderedPoint2D(Point2D point)
	{
		x = point.x;
		y = point.y;
	}
	bool operator<(const OrderedPoint2D& rhs) const
	{
		if (x == rhs.x)
			return y < rhs.y;
		return x < rhs.x;
	}
};

}