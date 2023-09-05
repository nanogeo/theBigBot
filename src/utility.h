#pragma once
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

#include "sc2api/sc2_unit_filters.h"

namespace sc2
{

struct UnitCost
{
	int mineral_cost;
	int vespene_cost;
	int supply;
	UnitCost() {};
	UnitCost(int x, int y, int z)
	{
		mineral_cost = x;
		vespene_cost = y;
		supply = z;
	}
};

class Utility
{
public:
	static const Unit* ClosestTo(Units, Point2D);
	static const Unit* NthClosestTo(Units, Point2D, int);
	static Point2D ClosestTo(std::vector<Point2D>, Point2D);
	static Point2D NthClosestTo(std::vector<Point2D>, Point2D, int);
	static const Unit* FurthestFrom(Units, Point2D);
	static float DistanceToClosest(Units, Point2D);
	static float DistanceToClosest(std::vector<Point2D>, Point2D);
	static Point2D ClosestPointOnLine(Point2D, Point2D, Point2D);
	static Units CloserThan(Units, float, Point2D);
	static bool HasBuff(const Unit*, BUFF_ID);
	static Point2D Center(Units);
	static Point2D MedianCenter(Units);
	static Point2D PointBetween(Point2D, Point2D, float);
	static int DangerLevel(const Unit *, const ObservationInterface*);
	static int DangerLevelAt(const Unit *, Point2D, const ObservationInterface*);
	static int GetDamage(const Unit*, const Unit*, const ObservationInterface*);
	static int GetArmor(const Unit*);
	static int GetCargoSize(const Unit*);
	static float RealGroundRange(const Unit *, const Unit *);
	static float GetDamagePoint(const Unit*);
	static int GetProjectileTime(const Unit*, float dist);
	static float GetWeaponCooldown(const Unit*);
	static bool IsOnHighGround(Point3D, Point3D);
	static float GetTimeBuilt(const Unit*, const ObservationInterface*);
	static AbilityID UnitToWarpInAbility(UNIT_TYPEID);
	static bool IsFacing(const Unit*, const Unit*);
	static float GetFacingAngle(const Unit*, const Unit*);
	static const Unit* AimingAt(const Unit*, const ObservationInterface*);
	static float BuildingSize(UNIT_TYPEID);
	static bool CanBuildBuilding(UNIT_TYPEID, const ObservationInterface*);
	static const Unit* GetLeastFullPrism(Units);
	static ABILITY_ID GetBuildAbility(UNIT_TYPEID);
	static ABILITY_ID GetTrainAbility(UNIT_TYPEID);
	static int BuildingsReady(UNIT_TYPEID, const ObservationInterface*);
	static bool CanAfford(UNIT_TYPEID, int, const ObservationInterface*);
	static bool CanAffordUpgrade(UPGRADE_ID, const ObservationInterface*);

	static std::string UnitTypeIdToString(UNIT_TYPEID);
	static std::string AbilityIdToString(ABILITY_ID);
};

}