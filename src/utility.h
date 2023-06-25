#pragma once
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

#include "sc2api/sc2_unit_filters.h"

namespace sc2
{
class Utility
{
public:
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
};

}