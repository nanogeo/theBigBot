
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <functional>
#include <array>
#include <chrono>
#include <fstream>
#include <complex>


#include "theBigBot.h"
#include "utility.h"

#include "quartic.h"

namespace sc2
{


IsFinishedUnit::IsFinishedUnit(UNIT_TYPEID type_) : m_type(type_) {
}

bool IsFinishedUnit::operator()(const Unit& unit_) const {
	return unit_.unit_type == m_type && unit_.build_progress == 1;
}

IsNotFinishedUnit::IsNotFinishedUnit(UNIT_TYPEID type_) : m_type(type_) {
}

bool IsNotFinishedUnit::operator()(const Unit& unit_) const {
	return unit_.unit_type == m_type && unit_.build_progress < 1;
}

IsFightingUnit::IsFightingUnit(Unit::Alliance alliance_) : m_type(alliance_) {
}

bool IsFightingUnit::operator()(const Unit& unit_) const {
	if (unit_.alliance != m_type)
		return false;
	for (const auto& type : { CANNON, /*PROBE,*/ ZEALOT, SENTRY,
		STALKER, ADEPT, HIGH_TEMPLAR, DARK_TEMPLAR, ARCHON, PRISM, PRISM_SIEGED,
		IMMORTAL, COLOSSUS, PHOENIX, VOID_RAY, ORACLE,
		CARRIER, TEMPEST, MOTHERSHIP, PLANETARY, MISSILE_TURRET,
		/*SCV,*/ MARINE, MARAUDER, REAPER, GHOST,
		HELLION, HELLBAT, SIEGE_TANK, SIEGE_TANK_SIEGED, CYCLONE,
		THOR_AOE, THOR_AP, AUTO_TURRET, VIKING_LANDED, VIKING,
		LIBERATOR, LIBERATOR_SIEGED, BANSHEE, BATTLECRUISER, SPINE_CRAWLER,
		SPORE_CRAWLER, /*DRONE,*/ QUEEN, ZERGLING, BANELING, ROACH,
		RAVAGER, HYDRA, LURKER, ULTRALISK, MUTALISK, CORRUPTER,
		BROOD_LORD, LOCUST, BROODLING })
	{
		if (unit_.unit_type.ToType() == type)
			return true;
	}
	return false;
}

IsNonbuilding::IsNonbuilding(Unit::Alliance alliance_) : m_type(alliance_) {
}

bool IsNonbuilding::operator()(const Unit& unit_) const {
	if (unit_.alliance != m_type)
		return false;
	for (const auto& type : { PROBE, ZEALOT, SENTRY,
		STALKER, ADEPT, HIGH_TEMPLAR, DARK_TEMPLAR, ARCHON, PRISM, PRISM_SIEGED,
		IMMORTAL, COLOSSUS, PHOENIX, VOID_RAY, ORACLE,
		CARRIER, TEMPEST, MOTHERSHIP,
		SCV, MARINE, MARAUDER, REAPER, GHOST,
		HELLION, HELLBAT, SIEGE_TANK, SIEGE_TANK_SIEGED, CYCLONE,
		THOR_AOE, THOR_AP, VIKING_LANDED, VIKING, MEDIVAC,
		LIBERATOR, LIBERATOR_SIEGED, BANSHEE, BATTLECRUISER, 
		DRONE, QUEEN, ZERGLING, BANELING, ROACH,
		RAVAGER, HYDRA, LURKER, ULTRALISK, MUTALISK, CORRUPTER, // TODO overlord
		BROOD_LORD, LOCUST, BROODLING })
	{
		if (unit_.unit_type.ToType() == type)
			return true;
	}
	return false;
}

IsGroundBuilding::IsGroundBuilding() {
}

bool IsGroundBuilding::operator()(const Unit& unit_) const {
	switch (unit_.unit_type.ToType()) 
	{
		// Terran
	case ARMORY:
	case BARRACKS:
	case BARRACKS_REACTOR:
	case BARRACKS_TECH_LAB:
	case BUNKER:
	case COMMAND_CENTER:
	case ENGINEERING_BAY:
	case FACTORY:
	case FACTORY_REACTOR:
	case FACTORY_TECH_LAB:
	case FUSION_CORE:
	case GHOST_ACADEMY:
	case MISSILE_TURRET:
	case ORBITAL:
	case PLANETARY:
	case REFINERY:
	case SENSOR_TOWER:
	case STARPORT:
	case STARPORT_REACTOR:
	case STARPORT_TECH_LAB:
	case SUPPLY_DEPOT:
	case SUPPLY_DEPOT_LOWERED:
	case REACTOR:
	case TECH_LAB:
		// Zerg
	case BANELING_NEST:
	case CREEP_TUMOR_1:
	case CREEP_TUMOR_2:
	case CREEP_TUMOR_3:
	case EVO_CHAMBER:
	case EXTRACTOR:
	case GREATER_SPIRE:
	case HATCHERY:
	case HIVE:
	case HYDRA_DEN:
	case INFESTATION_PIT:
	case LAIR:
	case LURKER_DEN:
	case NYDUS_WORM:
	case NYDUS:
	case ROACH_WARREN:
	case SPAWNING_POOL:
	case SPINE_CRAWLER:
	case SPINE_CRAWLER_UPROOTED:
	case SPIRE:
	case SPORE_CRAWLER:
	case SPORE_CRAWLER_UPROOTED:
	case ULTRALISK_CAVERN:
		// Protoss
	case ASSIMILATOR:
	case CYBERCORE:
	case DARK_SHRINE:
	case FLEET_BEACON:
	case FORGE:
	case GATEWAY:
	case NEXUS:
	case CANNON:
	case PYLON:
	case ROBO_BAY:
	case ROBO:
	case STARGATE:
	case TEMPLAR_ARCHIVE:
	case TWILIGHT:
	case WARP_GATE:
	case BATTERY:
		return true;
	default:
		return false;
	}
}

IsNonPlaceholderUnit::IsNonPlaceholderUnit(UNIT_TYPEID type_) : m_type(type_) {
}

bool IsNonPlaceholderUnit::operator()(const Unit& unit_) const {
	return unit_.unit_type == m_type && unit_.display_type != Unit::DisplayType::Placeholder;
}

IsFriendlyUnit::IsFriendlyUnit(UNIT_TYPEID type_) : m_type(type_) {
}

bool IsFriendlyUnit::operator()(const Unit& unit_) const {
	return unit_.unit_type == m_type && unit_.alliance == Unit::Alliance::Self;
}

IsEnemyUnit::IsEnemyUnit(UNIT_TYPEID type_) : m_type(type_) {
}

bool IsEnemyUnit::operator()(const Unit& unit_) const {
	return unit_.unit_type == m_type && unit_.alliance == Unit::Alliance::Enemy;
}

IsFlyingUnit::IsFlyingUnit() {
}

bool IsFlyingUnit::operator()(const Unit& unit_) const {
	return unit_.is_flying;
}

IsNotFlyingUnit::IsNotFlyingUnit() {
}

bool IsNotFlyingUnit::operator()(const Unit& unit_) const {
	return !unit_.is_flying;
}

IsNotCarryingResources::IsNotCarryingResources(UNIT_TYPEID type_) : m_type(type_) {
}

bool IsNotCarryingResources::operator()(const Unit& unit_) const {
	return unit_.unit_type == m_type && !IsCarryingMinerals(unit_);
}


Point2D Utility::ToPoint2D(Point3D pos)
{
	return Point2D(pos.x, pos.y);
}

const Unit* Utility::ClosestTo(Units units, Point2D position)
{
	const Unit* current_closest = nullptr;
	float current_distance = INFINITY;
	for (const auto &unit : units)
	{
		float distance = Distance2D(unit->pos, position);
		if (distance < current_distance)
		{
			current_closest = unit;
			current_distance = distance;
		}
	}
	if (units.size() == 0 || current_closest == nullptr)
	{
		//std::cout << "Error current closest is nullptr\n";
		return nullptr;
	}
	return current_closest;
}

const Unit* Utility::ClosestToLine(Units units, LineSegmentLinearX line)
{
	const Unit* current_closest = nullptr;
	float current_distance = INFINITY;
	for (const auto& unit : units)
	{
		float distance = Distance2D(unit->pos, line.FindClosestPoint(unit->pos));
		if (distance < current_distance)
		{
			current_closest = unit;
			current_distance = distance;
		}
	}
	if (units.size() == 0 || current_closest == nullptr)
	{
		//std::cout << "Error current closest is nullptr\n";
		return nullptr;
	}
	return current_closest;
}

const Unit* Utility::ClosestToLine(Units units, Point2D start, Point2D end)
{
	const Unit* current_closest = nullptr;
	float current_distance = INFINITY;
	for (const auto& unit : units)
	{
		float distance = Distance2D(unit->pos, ClosestPointOnLine(unit->pos, start, end));
		if (distance < current_distance)
		{
			current_closest = unit;
			current_distance = distance;
		}
	}
	if (units.size() == 0 || current_closest == nullptr)
	{
		//std::cout << "Error current closest is nullptr\n";
		return nullptr;
	}
	return current_closest;
}

const Unit* Utility::NthClosestTo(Units units, Point2D position, int n)
{
	if (units.size() == 0)
		return nullptr;

	std::vector<Point2D> points;
	for (const auto &unit : units)
	{
		points.push_back(Point2D(unit->pos));
	}
	Point2D nthClosestPoint = NthClosestTo(points, position, n);

	for (const auto &unit : units)
	{
		if (nthClosestPoint == Point2D(unit->pos))
			return unit;
	}
	//std::cout << "Error no unit found in NthClosestTo";
	return units[0];
}

Units Utility::NClosestUnits(Units units, Point2D position, int n)
{
	std::sort(units.begin(), units.end(), [&position](const Unit* &a, const Unit* &b)
	{
		return Distance2D(a->pos, position) < Distance2D(b->pos, position);
	});
	if (units.size() > n)
	{
		return std::vector<const Unit*>(units.begin(), units.begin() + n);
	}
	return units;
}

Point2D Utility::ClosestTo(std::vector<Point2D> points, Point2D position)
{
	Point2D current_closest;
	float current_distance = INFINITY;
	for (const auto &point : points)
	{
		float distance = Distance2D(point, position);
		if (distance < current_distance)
		{
			current_closest = point;
			current_distance = distance;
		}
	}
	/*if (points.size() == 0 || current_closest == nullptr)
	{
		//std::cout << "Error current closest is nullptr\n";
		return nullptr;
	}*/
	return current_closest;
}

const Unit* Utility::ClosestUnitTo(Units units, Point2D position)
{
	const Unit* current_closest = nullptr;
	float current_distance = INFINITY;
	for (const auto &unit : units)
	{
		float distance = Distance2D(unit->pos, position);
		if (distance < current_distance)
		{
			current_closest = unit;
			current_distance = distance;
		}
	}
	if (units.size() == 0 || current_closest == nullptr)
	{
		//std::cout << "Error current closest is nullptr\n";
		return nullptr;
	}
	return current_closest;
}

Point2D Utility::NthClosestTo(std::vector<Point2D> points, Point2D position, int n)
{
	if (n == 0)
		n++;
	std::sort(points.begin(), points.end(), [&position](const Point2D &a, const Point2D &b) -> bool
	{
		return Distance2D(a, position) < Distance2D(b, position);
	});
	if (points.size() > n)
		return points[n - 1];
	else
		return points[points.size() - 1];
}

const Unit* Utility::FurthestFrom(Units units, Point2D position)
{
	const Unit* current_furthest = nullptr;
	float current_distance = 0;
	for (const auto &unit : units)
	{
		float distance = Distance2D(unit->pos, position);
		if (distance > current_distance)
		{
			current_furthest = unit;
			current_distance = distance;
		}
	}
	if (units.size() == 0 || current_furthest == nullptr)
	{
		//std::cout << "Error current closest is nullptr\n";
		return nullptr;
	}
	return current_furthest;
}

Point2D Utility::FurthestFrom(std::vector<Point2D> points, Point2D position)
{
	Point2D current_furthest = Point2D(0, 0);
	float current_distance = 0;
	for (const auto& point : points)
	{
		float distance = Distance2D(point, position);
		if (distance > current_distance)
		{
			current_furthest = point;
			current_distance = distance;
		}
	}
	if (points.size() == 0 || current_furthest == Point2D(0, 0))
	{
		//std::cout << "Error current closest is nullptr\n";
		return Point2D(0, 0);
	}
	return current_furthest;
}

float Utility::DistanceToClosest(Units units, Point2D position)
{
	const Unit* closest_unit = ClosestTo(units, position);
	if (closest_unit == nullptr)
		return INFINITY;
	return Distance2D(closest_unit->pos, position);
}

float Utility::DistanceToClosest(std::vector<Point2D> points, Point2D position)
{
	Point2D current_closest;
	float current_distance = INFINITY;
	for (const auto& point : points)
	{
		float distance = Distance2D(point, position);
		if (distance < current_distance)
		{
			current_closest = point;
			current_distance = distance;
		}
	}
	return current_distance;
}

float Utility::DistanceToClosestOnLine(Units units, LineSegmentLinearX line)
{
	float current_distance = INFINITY;
	for (const auto& unit : units)
	{
		float distance = Distance2D(unit->pos, line.FindClosestPoint(unit->pos));
		if (distance < current_distance)
		{
			current_distance = distance;
		}
	}
	return current_distance;
}

float Utility::DistanceToFurthest(Units units, Point2D position)
{
	const Unit* furthest_unit = FurthestFrom(units, position);
	if (furthest_unit == nullptr)
		return INFINITY;
	return Distance2D(furthest_unit->pos, position);
}

float Utility::DistanceToFurthest(std::vector<Point2D> points, Point2D position)
{
	const Point2D furthest_point = FurthestFrom(points, position);
	return Distance2D(furthest_point, position);
}

float Utility::AnyUnitWithin(Units units, Point2D position, float dist)
{
	for (const auto &unit : units)
	{
		if (Distance2D(unit->pos, position) <= dist)
			return true;
	}
	return false;
}

Units Utility::GetUnitsWithin(Units units, Point2D position, float dist)
{
	Units close_units;
	for (const auto& unit : units)
	{
		if (Distance2D(unit->pos, position) <= dist)
			close_units.push_back(unit);
	}
	return close_units;
}

Units Utility::GetUnitsInRange(Units units, const Unit* attacker, float extra)
{
	Units within_range;
	for (const auto& unit : units)
	{
		float range = RealRange(attacker, unit);
		if (range > 0 && Distance2D(attacker->pos, unit->pos) <= range + extra)
			within_range.push_back(unit);
	}
	return within_range;
}

Units Utility::GetUnitsThatCanAttack(Units units, const Unit* target, float extra)
{
	Units within_range;
	for (const auto& unit : units)
	{
		float range = RealRange(unit, target);
		if (range > 0 && Distance2D(unit->pos, target->pos) <= range + extra)
			within_range.push_back(unit);
	}
	return within_range;
}

Point2D Utility::ClosestPointOnLine(Point2D point, Point2D start, Point2D end)
{
	// undefined / 0 slope
	if (end.y - start.y == 0)
		return Point2D(point.x, start.y);
	if (end.x - start.x == 0)
		return Point2D(start.x, point.y);

	float line_slope = (end.y - start.y) / (end.x - start.x);
	float perpendicular_slope = -1 / line_slope;

	float coef1 = line_slope * start.x - start.y;
	float coef2 = perpendicular_slope * point.x - point.y;

	float x_pos = (coef1 - coef2) / (line_slope - perpendicular_slope);
	float y_pos = line_slope * x_pos - coef1;

	if (start.x < end.x)
	{
		if (x_pos < start.x)
			return start;
		if (x_pos > end.x)
			return end;
		return Point2D(x_pos, y_pos);
	}
	if (start.x > end.x)
	{
		if (x_pos > start.x)
			return start;
		if (x_pos < end.x)
			return end;
		return Point2D(x_pos, y_pos);
	}
	if (start.y < end.y)
	{
		if (y_pos < start.y)
			return start;
		if (y_pos > end.y)
			return end;
		return Point2D(x_pos, y_pos);
	}
	if (start.y > end.y)
	{
		if (y_pos > start.y)
			return start;
		if (y_pos < end.y)
			return end;
		return Point2D(x_pos, y_pos);
	}

	return Point2D(x_pos, y_pos);
}

Units Utility::CloserThan(Units units, float distance, Point2D position)
{
	Units close_units;
	for (const auto &unit : units)
	{
		if (Distance2D(unit->pos, position) <= distance)
		{
			close_units.push_back(unit);
		}
	}
	return close_units;
}

bool Utility::HasBuff(const Unit *unit, BUFF_ID buffId)
{
	for (const auto &buff : unit->buffs)
	{
		if (buff == buffId)
		{
			return true;
		}
	}
	return false;
}

Point2D Utility::Center(Units units)
{
	if (units.size() == 0)
		return Point2D(0, 0);

	Point2D total = Point2D(0, 0);
	for (const auto &unit : units)
	{
		total += unit->pos;
	}
	return total / (float)units.size();
}

Point2D Utility::Center(std::vector<Point2D> points)
{
	if (points.size() == 0)
		return Point2D(0, 0);

	Point2D total = Point2D(0, 0);
	for (const auto& point : points)
	{
		total += point;
	}
	return total / (float)points.size();
}

Point2D Utility::MedianCenter(Units units)
{
	if (units.size() == 0)
		return Point2D(0, 0);
	Point2D guess = Center(units);
	bool done = false;
	std::vector<Point2D> tests = { Point2D(0, 1), Point2D(1, 0), Point2D(0, -1), Point2D(-1, 0) };
	float distance = 0;
	for (const auto &unit : units)
	{
		distance += Distance2D(unit->pos, guess);
	}

	while (!done)
	{
		done = true;
		for (const auto &test : tests)
		{
			Point2D new_guess = guess + test;
			float new_distance = 0;
			for (const auto &unit : units)
			{
				new_distance += Distance2D(unit->pos, new_guess);
			}
			if (new_distance < distance)
			{
				guess = new_guess;
				distance = new_distance;
				done = false;
				break;
			}
		}
	}
	return guess;
}

Point2D Utility::MedianCenter(std::vector<Point2D> points)
{
	if (points.size() == 0)
		return Point2D(0, 0);
	Point2D guess = Center(points);
	bool done = false;
	std::vector<Point2D> tests = { Point2D(0, 1), Point2D(1, 0), Point2D(0, -1), Point2D(-1, 0) };
	float distance = 0;
	for (const auto& point : points)
	{
		distance += Distance2D(point, guess);
	}

	while (!done)
	{
		done = true;
		for (const auto& test : tests)
		{
			Point2D new_guess = guess + test;
			float new_distance = 0;
			for (const auto& point : points)
			{
				new_distance += Distance2D(point, new_guess);
			}
			if (new_distance < distance)
			{
				guess = new_guess;
				distance = new_distance;
				done = false;
				break;
			}
		}
	}
	return guess;
}


const Unit* Utility::GetMostDamagedUnit(Units units)
{
	if (units.size() == 0)
		return nullptr;
	float lowest_healh = units[0]->health + units[0]->shield;
	const Unit* lowest_unit = units[0];
	for (const auto& unit : units)
	{
		float health = unit->health + unit->shield;
		if (health < lowest_healh)
		{
			lowest_healh = health;
			lowest_unit = unit;
		}
	}
	return lowest_unit;
}

Point2D Utility::PointBetween(Point2D start, Point2D end, float dist)
{
	float total_dist = Distance2D(start, end);
	if (total_dist == 0)
	{
		//std::cout << "Warning PointBetween called on the same point";
		return start;
	}
	return Point2D(start.x + dist * (end.x - start.x) / total_dist, start.y + dist * (end.y - start.y) / total_dist);
}

Point2D Utility::RunAwayCircle(Point2D start, Point2D end, float dist, float theta)
{
	theta = theta * (float)PI / 180.0f;
	float total_dist = Distance2D(start, end);
	if (total_dist == 0)
	{
		//std::cout << "Warning RunAwayCircle called on the same point";
		return start;
	}
	Point2D unit_vector = (end - start) / total_dist;
	Point2D offset_vector = Point2D(unit_vector.x * cos(theta) - unit_vector.y * sin(theta), 
									unit_vector.x * sin(theta) + unit_vector.y * cos(theta));

	return end + offset_vector * dist;
}

Point2D Utility::ClosestIntersectionTo(Point2D center1, double radius1, Point2D center2, double radius2, Point2D point)
{
	double dist = Distance2D(center1, center2);
	if (dist > radius1 + radius2 ||
		dist < abs(radius1 - radius2) ||
		(dist == 0 && radius1 == radius2))
	{
		return point;
	}
	double a = (pow(radius1, 2) - pow(radius2, 2) + pow(dist, 2)) / (2 * dist);
	double h = sqrt(pow(radius1, 2) - pow(a, 2));
	double x = center1.x + a * (center2.x - center1.x) / dist;
	double y = center1.y + a * (center2.y - center1.y) / dist;
	Point2D intersection1 = Point2D((float)(x + h * (center2.y - center1.y) / dist), (float)(y - h * (center2.x - center1.x) / dist));
	Point2D intersection2 = Point2D((float)(x - h * (center2.y - center1.y) / dist), (float)(y + h * (center2.x - center1.x) / dist));

	if (Distance2D(intersection1, point) <= Distance2D(intersection2, point))
		return intersection1;
	else
		return intersection2;
}

int Utility::DangerLevel(const Unit* unit, const ObservationInterface* observation)
{
	return DangerLevelAt(unit, unit->pos, observation);
}

int Utility::DangerLevelAt(const Unit* unit, Point2D pos, const ObservationInterface* observation)
{
	int possible_damage = 0;
	for (const auto &enemy_unit : observation->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)))
	{
		if (!enemy_unit->is_building && Distance2D(pos, enemy_unit->pos) <= RealRange(enemy_unit, unit))
			possible_damage += GetDamage(enemy_unit, unit);
	}
	return possible_damage;
}

int Utility::GetDamage(const Unit* attacker, const Unit* target)
{
	bool is_light = IsLight(target->unit_type);
	bool is_armored = IsArmored(target->unit_type);
	bool is_biological = IsBiological(target->unit_type);
	bool is_mechanical = IsMechanical(target->unit_type);
	bool is_massive = IsMassive(target->unit_type);
	bool is_flying = target->is_flying;
	int upgrade_level = attacker->attack_upgrade_level;
	int damage = 0;
	int attacks = 1;
	switch (attacker->unit_type.ToType())
	{
	case CANNON: // protoss
		damage = 20;
		break;
	case PROBE:
		if (is_flying)
			return 0;
		damage = 5;
		break;
	case ZEALOT:
		if (is_flying)
			return 0;
		damage = 8 + (upgrade_level * 1);
		attacks = 2;
		break;
	case SENTRY:
		damage = 6 + (upgrade_level * 1);
		break;
	case STALKER:
		damage = 13 + (upgrade_level * 1) + (is_armored * (5 + (upgrade_level * 1)));
		break;
	case ADEPT:
		if (is_flying)
			return 0;
		damage = 10 + (upgrade_level * 1) + (is_light * (12 + (upgrade_level * 1)));
		break;
	case HIGH_TEMPLAR:
		if (is_flying)
			return 0;
		damage = 4 + (upgrade_level * 1);
		break;
	case DARK_TEMPLAR:
		if (is_flying)
			return 0;
		damage = 45 + (upgrade_level * 5);
		break;
	case ARCHON:
		damage = 25 + (upgrade_level * 3) + (is_biological * (10 + (upgrade_level * 1)));
		break;
	case IMMORTAL:
		if (is_flying)
			return 0;
		damage = 20 + (upgrade_level * 2) + (is_armored * (30 + (upgrade_level * 3)));
		break;
	case COLOSSUS:
		if (is_flying)
			return 0;
		damage = 10 + (upgrade_level * 1) + (is_light * (5 + (upgrade_level * 1)));
		attacks = 2;
		break;
	case PHOENIX:
		if (!is_flying)
			return 0;
		damage = 5 + (upgrade_level * 1) + (is_light * 5);
		attacks = 2;
		break;
	case VOID_RAY: // TODO prismatic alignment
		damage = 6 + (upgrade_level * 1) + (is_armored * 4);
		break;
	case ORACLE:
		if (is_flying)
			return 0;
		damage = 15 + (is_light * 7);
		break;
	case CARRIER:
		damage = 5 + (upgrade_level * 1);
		attacks = 16;
		break;
	case TEMPEST: // TODO techtonic destabilizers
		damage = 40 + (upgrade_level * 4);
		break;
	case MOTHERSHIP:
		damage = 6 + (upgrade_level * 1);
		attacks = 6;
		break;
	case PLANETARY: // terran
		if (is_flying)
			return 0;
		damage = 40;
		break;
	case MISSILE_TURRET:
		if (!is_flying)
			return 0;
		damage = 12;
		attacks = 2;
		break;
	case SCV:
		if (is_flying)
			return 0;
		damage = 5;
		break;
	case MARINE:
		damage = 6 + (upgrade_level * 1);
		break;
	case MARAUDER:
		if (is_flying)
			return 0;
		damage = 10 + (upgrade_level * 1) + (is_armored * (10 + (upgrade_level * 1)));
		break;
	case REAPER:
		if (is_flying)
			return 0;
		damage = 4 + (upgrade_level * 1);
		attacks = 2;
		break;
	case GHOST:
		damage = 10 + (upgrade_level * 1) + (is_light * (10 + (upgrade_level * 1)));
		break;
	case HELLION: // TODO blue flame
		if (is_flying)
			return 0;
		damage = 8 + (upgrade_level * 1) + (is_light * (6 + (upgrade_level * 1)));
		break;
	case HELLBAT:
		if (is_flying)
			return 0;
		damage = 18 + (upgrade_level * 2) + (is_light * (0 + (upgrade_level * 1)));
		break;
	case SIEGE_TANK:
		if (is_flying)
			return 0;
		damage = 15 + (upgrade_level * 2) + (is_armored * (10 + (upgrade_level * 1)));
		break;
	case SIEGE_TANK_SIEGED:
		if (is_flying)
			return 0;
		damage = 40 + (upgrade_level * 4) + (is_armored * (30 + (upgrade_level * 1)));
		break;
	case CYCLONE:
		damage = 18 + (upgrade_level * 2);
		break;
	case THOR_AOE:
		if (is_flying)
		{
			damage = 6 + (upgrade_level * 1) + (is_light * (6 + (upgrade_level * 1)));
			attacks = 4;
		}
		else
		{
			damage = 30 + (upgrade_level * 3);
			attacks = 2;
		}
		break;
	case THOR_AP:
		if (is_flying)
		{
			damage = 25 + (upgrade_level * 3) + (is_massive * (10 + (upgrade_level * 1)));
		}
		else
		{
			damage = 30 + (upgrade_level * 3);
			attacks = 2;
		}
		break;
	case AUTO_TURRET:
		damage = 18;
		break;
	case VIKING_LANDED:
		if (is_flying)
			return 0;
		damage = 12 + (upgrade_level * 1) + (is_mechanical * (8 + (upgrade_level * 1)));
		break;
	case VIKING:
		if (!is_flying)
			return 0;
		damage = 10 + (upgrade_level * 1) + (is_armored * 4);
		attacks = 2;
		break;
	case LIBERATOR:
		if (!is_flying)
			return 0;
		damage = 5 + (upgrade_level * 1);
		attacks = 2;
		break;
	case LIBERATOR_SIEGED:
		if (is_flying)
			return 0;
		damage = 75 + (upgrade_level * 5);
		break;
	case BANSHEE:
		if (is_flying)
			return 0;
		damage = 12 + (upgrade_level * 1);
		attacks = 2;
		break;
	case BATTLECRUISER:
		if (is_flying)
		{
			damage = 5 + (upgrade_level * 1);
		}
		else
		{
			damage = 8 + (upgrade_level * 1);
		}
		break;
	case SPINE_CRAWLER: // zerg
		if (is_flying)
			return 0;
		damage = 25 + (is_armored * 5);
		break;
	case SPORE_CRAWLER:
		if (!is_flying)
			return 0;
		damage = 15 + (is_biological * 15);
		break;
	case DRONE:
	case DRONE_BURROWED:
		if (is_flying)
			return 0;
		damage = 5;
		break;
	case QUEEN:
	case QUEEN_BURROWED:
		if (is_flying)
		{
			damage = 9 + (upgrade_level * 1);
		}
		else
		{
			damage = 4 + (upgrade_level * 1);
			attacks = 2;
		}
		break;
	case ZERGLING:
	case ZERGLING_BURROWED:
		if (is_flying)
			return 0;
		damage = 5 + (upgrade_level * 1);
		break;
	case BANELING:
	case BANELING_BURROWED:
		if (is_flying)
			return 0;
		damage = 16 + (upgrade_level * 2) + (is_light * (19 + (upgrade_level * 2)));
		break;
	case ROACH:
	case ROACH_BURROWED:
		if (is_flying)
			return 0;
		damage = 16 + (upgrade_level * 2);
		break;
	case RAVAGER:
	case RAVAGER_BURROWED:
		if (is_flying)
			return 0;
		damage = 16 + (upgrade_level * 2);
		break;
	case HYDRA:
	case HYDRA_BURROWED:
		damage = 12 + (upgrade_level * 2);
		break;
	case LURKER:
	case LURKER_BURROWED:
		if (is_flying)
			return 0;
		damage = 20 + (upgrade_level * 2) + (is_armored * (10 + (upgrade_level * 1)));
		break;
	case ULTRALISK:
	case ULTRALISK_BURROWED:
		if (is_flying)
			return 0;
		damage = 35 + (upgrade_level * 3);
		break;
	case MUTALISK:
		damage = 9 + (upgrade_level * 1);
		break;
	case CORRUPTER:
		if (!is_flying)
			return 0;
		damage = 14 + (upgrade_level * 1) + (is_massive * (6 + (upgrade_level * 1)));
		break;
	case BROOD_LORD:
		if (is_flying)
			return 0;
		damage = 20 + (upgrade_level * 2);
		attacks = 2;
		break;
	case LOCUST:
		if (is_flying)
			return 0;
		damage = 10 + (upgrade_level * 1);
		break;
	case BROODLING:
		if (is_flying)
			return 0;
		damage = 4 + (upgrade_level * 1);
		break;
	default:
		//std::cout << "Error invalid unit type in GetDamage\n";
		return 0;
	}

	int armor = GetArmor(target);
	damage -= armor;
	return damage * attacks;

}

float Utility::GetDPS(const Unit* attacker)
{
	int upgrade_level = attacker->attack_upgrade_level;

	switch (attacker->unit_type.ToType())
	{
	case CANNON: // protoss
		return FRAME_TIME;
	case PROBE:
		return 4.67f;
		break;
	case ZEALOT:
		return 18.6f + (upgrade_level * 2.33f);
		break;
	case SENTRY:
		return 14.0f + (upgrade_level * 1.4f);
		break;
	case STALKER:
		return 13.4f + (upgrade_level * 1.5f);
		break;
	case ADEPT: // TODO glaives
		return 13.65f + (upgrade_level * 1.24f);
		break;
	case HIGH_TEMPLAR:
		return 3.2f + (upgrade_level * .8f);
		break;
	case DARK_TEMPLAR:
		return 37.2f + (upgrade_level * 4.13f);
		break;
	case ARCHON:
		return 28.0f + (upgrade_level * 3.2f);
		break;
	case IMMORTAL:
		return 48.1f + (upgrade_level * 4.82f);
		break;
	case COLOSSUS:
		return 28.0f + (upgrade_level * 3.74f);
		break;
	case PHOENIX:
		return 25.4f + (upgrade_level * 2.5f);
		break;
	case VOID_RAY: // TODO prismatic alignment
		return 28.0f + (upgrade_level * 2.8f);
		break;
	case ORACLE:
		return 35.9f;
		break;
	case CARRIER:
		return 37.4f + (upgrade_level * 7.5f);
		break;
	case TEMPEST: // TODO techtonic destabilizers
		return 16.97f + (upgrade_level * 1.697f);
		break;
	case MOTHERSHIP:
		return 22.8f + (upgrade_level * 3.78f);
		break;
	case PLANETARY: // terran
		return 28.0f;
		break;
	case MISSILE_TURRET:
		return 39.3f;
		break;
	case SCV:
		return 4.67f;
		break;
	case MARINE: // TODO stim
		return 9.8f + (upgrade_level * 1.6f);
		break;
	case MARAUDER:
		return 18.6f + (upgrade_level * 1.86f);
		break;
	case REAPER:
		return 10.1f + (upgrade_level * 2.5f);
		break;
	case GHOST:
		return 18.6f + (upgrade_level * 1.86f);
		break;
	case HELLION: // TODO blue flame
		return 7.88f + (upgrade_level * 1.13f);
		break;
	case HELLBAT:
		return 21.0f + (upgrade_level * 1.4f);
		break;
	case SIEGE_TANK:
		return 33.78f + (upgrade_level * 4.05f);
		break;
	case SIEGE_TANK_SIEGED:
		return 32.71f + (upgrade_level * 2.34f);
		break;
	case CYCLONE:
		return 24.14f + (upgrade_level * 1.72f);
		break;
	case THOR_AOE:
		return 65.9f + (upgrade_level * 6.59f);
		break;
	case THOR_AP:
		return 65.9f + (upgrade_level * 6.59f);
		break;
	case AUTO_TURRET:
		return 31.58f;
		break;
	case VIKING_LANDED:
		return 28.2f + (upgrade_level * 2.82f);
		break;
	case VIKING:
		return 19.59f + (upgrade_level * 1.4f);
		break;
	case LIBERATOR:
		return 7.75f + (upgrade_level * 1.55f);
		break;
	case LIBERATOR_SIEGED:
		return 65.8f + (upgrade_level * 4.39f);
		break;
	case BANSHEE:
		return 27.0f + (upgrade_level * 2.25f);
		break;
	case BATTLECRUISER:
		return 49.8f + (upgrade_level * 6.2f);
		break;
	case SPINE_CRAWLER: // zerg
		return 22.7f;
		break;
	case SPORE_CRAWLER:
		return 24.4f;
		break;
	case DRONE:
		return 4.67f;
		break;
	case QUEEN:
		return 12.6f + (upgrade_level * 1.4f);
		break;
	case ZERGLING: // TODO adrenal
		return 10.0f + (upgrade_level * 2.0f);
		break;
	case BANELING:
		return 35.0f + (upgrade_level * 2.0f);
		break;
	case ROACH:
		return 11.2f + (upgrade_level * 1.4f);
		break;
	case RAVAGER:
		return 14.04f + (upgrade_level * 1.75f);
		break;
	case HYDRA:
		return 20.4f + (upgrade_level * 1.7f);
		break;
	case LURKER:
		return 21.0f + (upgrade_level * 2.1f);
		break;
	case ULTRALISK:
		return 57.38f + (upgrade_level * 4.9f);
		break;
	case MUTALISK:
		return 11.93f + (upgrade_level * 1.33f);
		break;
	case CORRUPTER:
		return 14.69f + (upgrade_level * 1.48f);
		break;
	case BROOD_LORD:
		return 22.4f + (upgrade_level * 2.2f);
		break;
	case LOCUST:
		return 23.25f + (upgrade_level * 2.33f);
		break;
	case BROODLING:
		return 7.0f + (upgrade_level * 1.75f);
		break;
	default:
		//std::cout << "Error invalid unit type in GetDPS\n";
		return 0.0f;
	}
}

float Utility::GetDPS(const Unit* attacker, const Unit* target)
{
	float damage = (float)(GetDamage(attacker, target) - GetArmor(target));

	switch (attacker->unit_type.ToType())
	{
	case CANNON: // protoss
		return damage / .89f;
	case PROBE:
		return damage / 1.07f;
	case ZEALOT:
		return damage / .86f;
	case SENTRY:
		return damage / .71f;
	case STALKER:
		return damage / 1.34f;
	case ADEPT: // TODO glaives
		return damage / 1.61f; // .64
	case HIGH_TEMPLAR:
		return damage / 1.25f;
	case DARK_TEMPLAR:
		return damage / 1.21f;
	case ARCHON:
		return damage / 1.25f;
	case IMMORTAL:
		return damage / 1.04f;
	case COLOSSUS:
		return damage / 1.07f;
	case PHOENIX:
		return damage / .79f;
	case VOID_RAY: // TODO prismatic alignment
		return damage / .36f;
	case ORACLE:
		return damage / .61f;
	case CARRIER:
		return damage / 2.14f;
	case TEMPEST: // TODO techtonic destabilizers
		return damage / 2.36f;
	case MOTHERSHIP:
		return damage / 1.58f;
	case PLANETARY: // terran
		return damage / 1.43f;
	case MISSILE_TURRET:
		return damage / .61f;
	case SCV:
		return damage / 1.07f;
	case MARINE: // TODO stim
		return damage / .61f; // .41
	case MARAUDER:
		return damage / 1.07f; // .71
	case REAPER:
		return damage / .79f;
	case GHOST:
		return damage / 1.07f;
	case HELLION: // TODO blue flame
		return damage / 1.79f;
	case HELLBAT:
		return damage / 1.43f;
	case SIEGE_TANK:
		return damage / .74f;
	case SIEGE_TANK_SIEGED:
		return damage / 2.14f;
	case CYCLONE:
		return damage / .71f;
	case THOR_AOE:
		if (target->is_flying)
			return damage / 2.14f;
		else
			return damage / .91f;
	case THOR_AP:
			return damage / .91f;
	case AUTO_TURRET:
		return damage / .57f;
	case VIKING_LANDED:
		return damage / .71f;
	case VIKING:
		return damage / 1.43f;
	case LIBERATOR:
		return damage / 1.29f;
	case LIBERATOR_SIEGED:
		return damage / 1.14f;
	case BANSHEE:
		return damage / .89f;
	case BATTLECRUISER:
		return damage / .16f;
	case SPINE_CRAWLER: // zerg
		return damage / 1.32f;
	case SPORE_CRAWLER:
		return damage / .61f;
	case DRONE:
	case DRONE_BURROWED:
		return damage / 1.07f;
	case QUEEN:
	case QUEEN_BURROWED:
		return damage / .71f;
	case ZERGLING: // TODO adrenal
	case ZERGLING_BURROWED:
		return damage / .5f; // .35
	case BANELING:
	case BANELING_BURROWED:
		return damage;
	case ROACH:
	case ROACH_BURROWED:
		return damage / 1.43f;
	case RAVAGER:
	case RAVAGER_BURROWED:
		return damage / 1.14f;
	case HYDRA:
	case HYDRA_BURROWED:
		return damage / .59f;
	case LURKER:
	case LURKER_BURROWED:
		return damage / 1.43f;
	case ULTRALISK:
	case ULTRALISK_BURROWED:
		return damage / .61f;
	case MUTALISK:
		return damage / 1.09f;
	case CORRUPTER:
		return damage / 1.36f;
	case BROOD_LORD:
		return damage / 1.79f;
	case LOCUST:
		return damage / .43f;
	case BROODLING:
		return damage / .46f;
	default:
		//std::cout << "Error invalid unit type in GetDPS\n";
		return 0.0f;
	}
}

int Utility::GetArmor(const Unit* unit)
{
	if (unit->shield > 0)
	{
		return unit->shield_upgrade_level;
	}
	int base_armor = 0;
	switch (unit->unit_type.ToType())
	{
	case PYLON: // protoss buildings
		break;
	case NEXUS:
		break;
	case GATEWAY:
		break;
	case WARP_GATE:
		break;
	case FORGE:
		break;
	case CYBERCORE:
		break;
	case CANNON:
		break;
	case BATTERY:
		break;
	case TWILIGHT:
		break;
	case STARGATE:
		break;
	case ROBO:
		break;
	case ROBO_BAY:
		break;
	case TEMPLAR_ARCHIVE:
		break;
	case DARK_SHRINE:
		break;
	case FLEET_BEACON:
		break;
	case ASSIMILATOR:
		break;
	case PROBE: // protoss units
		break;
	case ZEALOT:
		base_armor = 1;
		break;
	case SENTRY:
		base_armor = 1;
		break;
	case STALKER:
		base_armor = 1;
		break;
	case ADEPT:
		base_armor = 1;
		break;
	case HIGH_TEMPLAR:
		break;
	case DARK_TEMPLAR:
		base_armor = 1;
		break;
	case IMMORTAL:
		base_armor = 1;
		break;
	case COLOSSUS:
		base_armor = 1;
		break;
	case DISRUPTOR:
		base_armor = 1;
		break;
	case OBSERVER:
		break;
	case PRISM:
		break;
	case PHOENIX:
		break;
	case VOID_RAY:
		break;
	case ORACLE:
		break;
	case CARRIER:
		base_armor = 2;
		break;
	case TEMPEST:
		base_armor = 2;
		break;
	case MOTHERSHIP:
		base_armor = 2;
		break;
	case COMMAND_CENTER: // terran buildings
		break;
	case PLANETARY:
		break;
	case ORBITAL:
		break;
	case SUPPLY_DEPOT:
		break;
	case SUPPLY_DEPOT_LOWERED:
		break;
	case REFINERY:
		break;
	case BARRACKS:
		break;
	case ENGINEERING_BAY:
		break;
	case BUNKER:
		break;
	case SENSOR_TOWER:
		break;
	case MISSILE_TURRET:
		break;
	case FACTORY:
		break;
	case GHOST_ACADEMY:
		break;
	case STARPORT:
		break;
	case ARMORY:
		break;
	case FUSION_CORE:
		break;
	case TECH_LAB:
		break;
	case REACTOR:
		break;
	case BARRACKS_TECH_LAB:
		break;
	case BARRACKS_REACTOR:
		break;
	case FACTORY_TECH_LAB:
		break;
	case FACTORY_REACTOR:
		break;
	case STARPORT_TECH_LAB:
		break;
	case STARPORT_REACTOR:
		break;
	case SCV:
		break;
	case MULE:
		break;
	case MARINE: // terran units
		break;
	case MARAUDER:
		base_armor = 1;
		break;
	case REAPER:
		break;
	case GHOST:
		break;
	case HELLION:
		break;
	case HELLBAT:
		break;
	case SIEGE_TANK:
		base_armor = 1;
		break;
	case SIEGE_TANK_SIEGED:
		base_armor = 1;
		break;
	case CYCLONE:
		base_armor = 1;
		break;
	case WIDOW_MINE:
		break;
	case THOR_AOE:
		base_armor = 1;
		break;
	case THOR_AP:
		base_armor = 1;
		break;
	case AUTO_TURRET:
		break;
	case VIKING_LANDED:
		break;
	case VIKING:
		break;
	case MEDIVAC:
		base_armor = 1;
		break;
	case LIBERATOR:
		break;
	case RAVEN:
		base_armor = 1;
		break;
	case BANSHEE:
		break;
	case BATTLECRUISER:
		base_armor = 3;
		break;
	case HATCHERY: // zerg buildings
		break;
	case LAIR:
		break;
	case HIVE:
		break;
	case SPINE_CRAWLER:
		break;
	case SPORE_CRAWLER:
		break;
	case EXTRACTOR:
		break;
	case SPAWNING_POOL:
		break;
	case EVO_CHAMBER:
		break;
	case ROACH_WARREN:
		break;
	case BANELING_NEST:
		break;
	case HYDRA_DEN:
		break;
	case LURKER_DEN:
		break;
	case INFESTATION_PIT:
		break;
	case SPIRE:
		break;
	case GREATER_SPIRE:
		break;
	case NYDUS:
		break;
	case ULTRALISK_CAVERN:
		break;
	case LARVA: // zerg units
		base_armor = 10;
		break;
	case EGG:
		base_armor = 10;
		break;
	case DRONE:
		break;
	case QUEEN:
		base_armor = 1;
		break;
	case ZERGLING:
		break;
	case BANELING:
		break;
	case ROACH:
		base_armor = 1;
		break;
	case RAVAGER:
		base_armor = 1;
		break;
	case HYDRA:
		break;
	case LURKER:
		base_armor = 1;
		break;
	case INFESTOR:
		break;
	case SWARMHOST:
		base_armor = 1;
		break;
	case ULTRALISK: // TODO chitinous
		base_armor = 1;
		break;
	case OVERLORD:
		break;
	case OVERSEER:
		base_armor = 1;
		break;
	case MUTALISK:
		break;
	case CORRUPTER:
		base_armor = 2;
		break;
	case BROOD_LORD:
		base_armor = 1;
		break;
	case VIPER:
		base_armor = 1;
		break;
	case LOCUST:
		break;
	case BROODLING:
		break;
	case BANELING_EGG:
		base_armor = 2;
		break;
	case BROOD_LORD_EGG:
		base_armor = 2;
		break;
	case OVERSEER_EGG:
	case DROPPERLORD_EGG:
		base_armor = 2;
		break;
	case RAVAGER_EGG:
		base_armor = 5;
		break;
	case LURKER_EGG:
		base_armor = 1;
		break;
	default:
		//std::cout << "Error invalid unit type in GetArmor\n";
		return 0;
	}

	return base_armor + unit->armor_upgrade_level;

}


int Utility::GetCargoSize(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case PROBE:
		return 1;
	case ZEALOT:
		return 2;
	case SENTRY:
		return 2;
	case STALKER:
		return 2;
	case ADEPT:
		return 2;
	case HIGH_TEMPLAR:
		return 2;
	case DARK_TEMPLAR:
		return 2;
	case ARCHON:
		return 4;
	case IMMORTAL:
		return 4;
	case COLOSSUS:
		return 8;
	case DISRUPTOR:
		return 4;
	default:
		//std::cout << "Error invalid unit type in GetCargoSize\n";
		return 0;
	}

}

float Utility::GetGroundRange(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case CANNON: // protoss
		return 7.0f;
	case BATTERY:
		return 6.0f;
	case PROBE:
		return .1f;
	case ZEALOT:
		return .1f;
	case SENTRY:
		return 5.0f;
	case STALKER:
		return 6.0f;
	case ADEPT:
		return 4.0f;
	case HIGH_TEMPLAR:
		return 6.0f;
	case DARK_TEMPLAR:
		return .1f;
	case ARCHON:
		return 3.0f;
	case IMMORTAL:
		return 6.0f;
	case COLOSSUS: // TODO extended thermal lance
		return 7.0f;
	case VOID_RAY:
		return 6.0f;
	case ORACLE:
		return 4.0f;
	case CARRIER:
		return 14.0f;
	case TEMPEST: // air 14
		return 10.0f;
	case MOTHERSHIP:
		return 7.0f;
	case PLANETARY: // terran hi sec auto tracking
		return 6.0f;
	case SCV:
		return .1f;
	case MARINE:
		return 5.0f;
	case MARAUDER:
		return 6.0f;
	case REAPER:
		return 5.0f;
	case GHOST:
		return 6.0f;
	case HELLION:
		return 5.0f;
	case HELLBAT:
		return 2.0f;
	case SIEGE_TANK:
		return 7.0f;
	case SIEGE_TANK_SIEGED:
		return 13.0f;
	case CYCLONE:
		return 5.0f;
	case THOR_AOE: // flying 10
		return 7.0f;
	case THOR_AP: // flying 11
		return 7.0f;
	case AUTO_TURRET:
		return 6.0f;
	case VIKING_LANDED:
		return 9.0f;
	case LIBERATOR_SIEGED:
		return 0.0f;
	case BANSHEE:
		return 6.0f;
	case BATTLECRUISER:
		return 6.0f;
	case SPINE_CRAWLER: // zerg
		return 7.0f;
	case DRONE:
		return .1f;
	case QUEEN: // flying 7
		return 5.0f;
	case ZERGLING:
		return .1f;
	case BANELING:
		return 2.5f;
	case ROACH:
		return 4.0f;
	case RAVAGER:
		return 6.0f;
	case HYDRA: // grooved spines
		return 6.0f;
	case LURKER: // seismic spine
		return 8.0f;
	case ULTRALISK:
		return .1f;
	case MUTALISK:
		return 3.0f;
	case BROOD_LORD:
		return 10.0f;
	case LOCUST:
		return 3.0f;
	case BROODLING:
		return 0.0f;
	case LARVA:
		return 0.0f;
	case OVERLORD:
		return 0.0f;
	case EGG:
		return 0.0f;
	case INFESTOR:
		return 0.0f;
	case INFESTOR_BURROWED:
		return 0.0f;
	default:
		//std::cout << "Error invalid unit type in GetRange\n";
		return 0.0f;
	}
}

float Utility::GetAirRange(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case CANNON: // protoss
		return 7.0f;
	case BATTERY:
		return 6.0f;
	case SENTRY:
		return 5.0f;
	case STALKER:
		return 6.0f;
	case ARCHON:
		return 3.0f;
	case PHOENIX: // TODO anion pulse crystals
		return 5.0f;
	case VOID_RAY:
		return 6.0f;
	case CARRIER:
		return 14.0f;
	case TEMPEST:
		return 14.0f;
	case MOTHERSHIP:
		return 7.0f;
	case MISSILE_TURRET: // terran
		return 6.0f;
	case MARINE:
		return 5.0f;
	case GHOST:
		return 6.0f;
	case CYCLONE:
		return 5.0f;
	case THOR_AOE:
		return 10.0f;
	case THOR_AP:
		return 11.0f;
	case AUTO_TURRET:
		return 6.0f;
	case VIKING:
		return 6.0f;
	case LIBERATOR:
		return 5.0f;
	case BATTLECRUISER:
		return 6.0f;
	case SPORE_CRAWLER: // zerg
		return 7.0f;
	case QUEEN:
		return 7.0f;
	case HYDRA: // grooved spines
		return 6.0f;
	case MUTALISK:
		return 3.0f;
	case CORRUPTER:
		return 6.0f;
	default:
		//std::cout << "Error invalid unit type in GetRange\n";
		return 0.0f;
	}
}

float Utility::RealRange(const Unit* attacking_unit, const Unit * target)
{
	if (target->is_flying)
	{
		float range = attacking_unit->radius + target->radius;
		range += GetAirRange(attacking_unit);
		return range;
	}
	else
	{
		float range = attacking_unit->radius + target->radius;
		range += GetGroundRange(attacking_unit);
		return range;
	}
}

float Utility::GetMaxGroundRange(Units units)
{
	float highest_range = 0;
	for (const auto &unit : units)
	{
		if (GetGroundRange(unit) > highest_range)
			highest_range = GetGroundRange(unit);
	}
	return highest_range;
}

float Utility::GetMaxAirRange(Units units)
{
	float highest_range = 0;
	for (const auto& unit : units)
	{
		if (GetAirRange(unit) > highest_range)
			highest_range = GetAirRange(unit);
	}
	return highest_range;
}

float Utility::GetDamagePoint(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case CANNON: // protoss
		return .1193f;
	case PROBE:
		return .1193f;
	case ZEALOT:
		return 0.0f;
	case SENTRY:
		return .1193f;
	case STALKER:
		return .1193f;
	case ADEPT:
		return .1193f;
	case HIGH_TEMPLAR:
		return .1193f;
	case DARK_TEMPLAR:
		return .2579f;
	case ARCHON:
		return .1193f;
	case IMMORTAL:
		return .1193f;
	case COLOSSUS:
		return .0594f;
	case PHOENIX:
		return .1193f;
	case VOID_RAY:
		return .1193f;
	case ORACLE:
		return .1193f;
	case CARRIER:
		return 0.0f;
	case TEMPEST:
		return .1193f;
	case MOTHERSHIP:
		return 0.0f;
	case PLANETARY:
		return .1193f;
	case MISSILE_TURRET:
		return .1193f;
	case SCV:
		return .1193f;
	case MARINE:
		return .0357f;
	case MARAUDER:
		return 0.0f;
	case REAPER:
		return 0.0f;
	case GHOST:
		return .0593f;
	case HELLION:
		return .1786f;
	case HELLBAT:
		return .1193f;
	case SIEGE_TANK:
		return .1193f;
	case SIEGE_TANK_SIEGED:
		return .1193f;
	case CYCLONE:
		return .1193f;
	case THOR_AOE:
		return .5936f;
	case THOR_AP:
		return .5936f;
	case AUTO_TURRET:
		return .1193f;
	case VIKING_LANDED:
		return .1193f;
	case VIKING:
		return .0357f;
	case LIBERATOR:
		return .1193f;
	case LIBERATOR_SIEGED:
		return .0893f;
	case BANSHEE:
		return .1193f;
	case BATTLECRUISER:
		return .1193f;
	case SPINE_CRAWLER:
		return .238f;
	case SPORE_CRAWLER:
		return .1193f;
	case DRONE:
		return .1193f;
	case QUEEN:
		return .1193f;
	case ZERGLING:
		return .1193f;
	case BANELING:
		return 0.0f;
	case ROACH:
		return .1193f;
	case RAVAGER:
		return .1429f;
	case HYDRA:
		return .1f;
	case LURKER:
		return 0.0f;
	case ULTRALISK:
		return .238f;
	case MUTALISK:
		return 0.0f;
	case CORRUPTER:
		return .0446f;
	case BROOD_LORD:
		return .1193f;
	case LOCUST:
		return .1904f;
	case BROODLING:
		return .1193f;
	default:
		//std::cout << "Error invalid unit type in GetDamagePoint\n";
		return 0.0f;
	}
}

float Utility::GetProjectileTime(const Unit* unit, float dist)
{
	switch (unit->unit_type.ToType())
	{
	case CANNON: // protoss
		return dist;
	case PROBE:
		return 0.0f;
	case ZEALOT:
		return 0.0f;
	case SENTRY:
		return 0.0f;
	case STALKER:
		return dist;
	case ADEPT:
		return dist;
	case HIGH_TEMPLAR:
		return dist;
	case DARK_TEMPLAR:
		return 0.0f;
	case ARCHON:
		return 0.0f;
	case IMMORTAL:
		return 0.0f;
	case COLOSSUS:
		return 0.0f;
	case PHOENIX:
		return dist;
	case VOID_RAY:
		return 0.0f;
	case ORACLE:
		return 0.0f;
	case CARRIER:
		return 0.0f;
	case TEMPEST:
		return dist;
	case MOTHERSHIP:
		return 0.0f;
	case PLANETARY:
		return 0.0f;
	case MISSILE_TURRET:
		return dist;
	case SCV:
		return 0.0f;
	case MARINE:
		return 0.0f;
	case MARAUDER:
		return dist;
	case REAPER:
		return 0.0f;
	case GHOST:
		return 0.0f;
	case HELLION:
		return 0.0f;
	case HELLBAT:
		return 0.0f;
	case SIEGE_TANK:
		return 0.0f;
	case SIEGE_TANK_SIEGED:
		return 0.0f;
	case CYCLONE:
		return dist;
	case THOR_AOE:
		return 0.0f;
	case THOR_AP:
		return 0.0f;
	case AUTO_TURRET:
		return 0.0f;
	case VIKING_LANDED:
		return 0.0f;
	case VIKING:
		return dist;
	case LIBERATOR:
		return dist;
	case LIBERATOR_SIEGED:
		return 0.0f;
	case BANSHEE:
		return dist;
	case BATTLECRUISER:
		return dist;
	case SPINE_CRAWLER:
		return dist;
	case SPORE_CRAWLER:
		return dist;
	case DRONE:
		return 0.0f;
	case QUEEN:
		return dist;
	case ZERGLING:
		return 0.0f;
	case BANELING:
		return 0.0f;
	case ROACH:
		return dist;
	case RAVAGER:
		return dist;
	case HYDRA:
		return dist;
	case LURKER:
		return 0.0f;
	case ULTRALISK:
		return 0.0f;
	case MUTALISK:
		return dist;
	case CORRUPTER:
		return dist;
	case BROOD_LORD:
		return dist;
	case LOCUST:
		return dist;
	case BROODLING:
		return 0.0f;
	default:
		//std::cout << "Error invalid unit type in GetProjectileTime\n";
		return 0.0f;
	}
}

float Utility::GetWeaponCooldown(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case CANNON: // protoss
		return .89f;
	case PROBE:
		return 1.07f;
	case ZEALOT:
		return .86f;
	case SENTRY:
		return .71f;
	case STALKER:
		return 1.34f;
	case ADEPT: // TODO glaives
		return 1.61f;
	case HIGH_TEMPLAR:
		return 1.25f;
	case DARK_TEMPLAR:
		return 1.21f;
	case ARCHON:
		return 1.25f;
	case IMMORTAL:
		return 1.04f;
	case COLOSSUS:
		return 1.07f;
	case PHOENIX:
		return .79f;
	case VOID_RAY:
		return .36f;
	case ORACLE:
		return .61f;
	case CARRIER: // TODO each interceptor?
		return 2.14f;
	case TEMPEST:
		return 2.36f;
	case MOTHERSHIP:
		return 1.58f;
	case PLANETARY:
		return 1.43f;
	case MISSILE_TURRET:
		return .61f;
	case SCV:
		return 1.07f;
	case MARINE: // TODO stim
		return .61f;
	case MARAUDER: // TODO stim
		return 1.07f;
	case REAPER:
		return .79f;
	case GHOST:
		return 1.07f;
	case HELLION:
		return 1.79f;
	case HELLBAT:
		return 1.43f;
	case SIEGE_TANK:
		return .79f;
	case SIEGE_TANK_SIEGED:
		return 2.14f;
	case CYCLONE:
		return .71f;
	case THOR_AOE: // TODO anit air
		return .91f;
	case THOR_AP:
		return .91f;
	case AUTO_TURRET:
		return .57f;
	case VIKING_LANDED:
		return .71f;
	case VIKING:
		return 1.43f;
	case LIBERATOR:
		return 1.29f;
	case LIBERATOR_SIEGED:
		return 1.14f;
	case BANSHEE:
		return .89f;
	case BATTLECRUISER:
		return .16f;
	case SPINE_CRAWLER:
		return 1.32f;
	case SPORE_CRAWLER:
		return .61f;
	case DRONE:
		return 1.07f;
	case QUEEN:
		return .71f;
	case ZERGLING: //TODO adrenal
		return .497f;
	case BANELING:
		return 0.0f;
	case ROACH:
		return 1.43f;
	case RAVAGER:
		return 1.43f;
	case HYDRA:
		return .59f;
	case LURKER:
		return 1.43f;
	case ULTRALISK:
		return .61f;
	case MUTALISK:
		return 1.09f;
	case CORRUPTER:
		return 1.36f;
	case BROOD_LORD:
		return 1.79f;
	case LOCUST:
		return .43f;
	case BROODLING:
		return .46f;
	default:
		//std::cout << "Error invalid unit type in GetWeaponCooldown\n";
		return 0.0f;
	}
}

float Utility::GetSpeed(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case PROBE:
		return 3.94f;
	case ZEALOT: // TODO charge
		return 3.15f; // 4.72
	case SENTRY:
		return 3.15f;
	case STALKER:
		return 4.13f;
	case ADEPT:
		return 3.5f;
	case HIGH_TEMPLAR:
		return 2.62f;
	case DARK_TEMPLAR:
		return 3.94f;
	case ARCHON:
		return 3.94f;
	case IMMORTAL:
		return 3.15f;
	case COLOSSUS:
		return 3.15f;
	case PHOENIX:
		return 5.95f;
	case VOID_RAY: // TODO flux vanes
		return 3.85f; // 4.65
	case ORACLE:
		return 5.6f;
	case CARRIER: // TODO each interceptor?
		return 2.62f;
	case TEMPEST:
		return 3.15f;
	case MOTHERSHIP:
		return 2.62f;
	case SCV:
		return 3.94f;
	case MULE:
		return 3.94f;
	case MARINE: // TODO stim
		return 3.15f; // 4.72
	case MARAUDER: // TODO stim
		return 3.15f; // 4.72
	case REAPER:
		return 5.25f;
	case GHOST:
		return 3.94f;
	case HELLION:
		return 5.95f;
	case HELLBAT:
		return 3.15f;
	case WIDOW_MINE:
		return 3.94f;
	case SIEGE_TANK:
		return 3.15f;
	case CYCLONE:
		return 4.72f;
	case THOR_AOE:
	case THOR_AP:
		return 2.62f;
	case VIKING_LANDED:
		return 3.15f;
	case VIKING:
		return 3.85f;
	case MEDIVAC: // TODO boost
		return 3.5f; // 5.94
	case LIBERATOR:
		return 4.72f;
	case BANSHEE: // TODO hyperflight rotors
		return 3.85f; // 5.25
	case RAVEN:
		return 4.13f;
	case BATTLECRUISER:
		return 2.62f;
	case DRONE: // TODO all zerg units off creep
		return 3.94f;
	case OVERLORD: // TODO speed
	case DROPPERLORD:
		return .902f;
	case OVERSEER:
		return 2.62f;
	case QUEEN:
		return 3.5f;
	case ZERGLING: // TODO ling speed
		return 5.37f;
	case BANELING: // TODO bane speed
		return 4.55f;
	case ROACH: // TODO speed
		return 4.09f; // 4.09
	case ROACH_BURROWED: // TODO speed
		return 2.56f; // 4.09
	case RAVAGER:
		return 5.0f;
	case HYDRA: // TODO speed
		return 4.09f;
	case LURKER:
		return 5.37f;
	case INFESTOR:
		return 4.09f;
	case INFESTOR_BURROWED:
		return 3.64f;
	case ULTRALISK: // TODO speed
		return 5.37f;
	case MUTALISK:
		return 5.6f;
	case CORRUPTER:
		return 4.73f;
	case BROOD_LORD:
		return 1.97f;
	case LOCUST:
		return 3.68f;
	case BROODLING:
		return 5.37f;
	default:
		return 0.0f;
	}
}


bool Utility::IsOnHighGround(Point3D unit, Point3D enemy_unit)
{
	return unit.z + .5 < enemy_unit.z;
}

float Utility::GetTimeBuilt(const Unit* unit, float curr_time)
{
	float build_time = GetTimeToBuild(unit->unit_type);
	
	return curr_time - (build_time * unit->build_progress);
}

float Utility::GetTimeToBuild(UNIT_TYPEID unit_type)
{
	switch (unit_type)
	{
	case NEXUS:
		return 71.0f;
	case PYLON:
		return 18.0f;
	case GATEWAY:
	case WARP_GATE:
		return 46.0f;
	case ASSIMILATOR:
	case ASSIMILATOR_RICH:
		return 21.0f;
	case FORGE:
		return 32.0f;
	case CYBERCORE:
		return 36.0f;
	case CANNON:
		return 29.0f;
	case BATTERY:
		return 29.0f;
	case TWILIGHT:
		return 36.0f;
	case TEMPLAR_ARCHIVE:
		return 36.0f;
	case STARGATE:
		return 43.0f;
	case FLEET_BEACON:
		return 43.0f;
	case DARK_SHRINE:
		return 71.0f;
	case ROBO:
		return 46.0f;
	case ROBO_BAY:
		return 46.0f;
	case PROBE:
		return 12.0f;
	case ZEALOT:
		return 27.0f;
	case STALKER:
		return 27.0f;
	case SENTRY:
		return 23.0f;
	case ADEPT:
		return 30.0f;
	case HIGH_TEMPLAR:
		return 39.0f;
	case DARK_TEMPLAR:
		return 39.0f;
	case IMMORTAL:
		return 39.0f;
	case COLOSSUS:
		return 54.0f;
	case DISRUPTOR:
		return 36.0f;
	case OBSERVER:
	case OBSERVER_SIEGED:
		return 18.0f;
	case PRISM:
	case PRISM_SIEGED:
		return 36.0f;
	case PHOENIX:
		return 25.0f;
	case VOID_RAY:
		return 43.0f;
	case ORACLE:
		return 37.0f;
	case CARRIER:
		return 64.0f;
	case TEMPEST:
		return 43.0f;
	case MOTHERSHIP:
		return 89.0f;
	case COMMAND_CENTER:
	case COMMAND_CENTER_FLYING:
		return 71.0f;
	case SUPPLY_DEPOT:
	case SUPPLY_DEPOT_LOWERED:
		return 21.0f;
	case REFINERY:
	case REFINERY_RICH:
		return 21.0f;
	case BARRACKS:
	case BARRACKS_FLYING:
		return 46.0f;
	case ENGINEERING_BAY:
		return 25.0f;
	case BUNKER:
	case NEOSTEEL_BUNKER:
		return 29.0f;
	case SENSOR_TOWER:
		return 18.0f;
	case MISSILE_TURRET:
		return 18.0f;
	case FACTORY:
	case FACTORY_FLYING:
		return 43.0f;
	case GHOST_ACADEMY:
		return 29.0f;
	case STARPORT:
	case STARPORT_FLYING:
		return 36.0f;
	case ARMORY:
		return 46.0f;
	case FUSION_CORE:
		return 46.0f;
	case REACTOR:
	case BARRACKS_REACTOR:
	case FACTORY_REACTOR:
	case STARPORT_REACTOR:
		return 36.0f;
	case TECH_LAB:
	case BARRACKS_TECH_LAB:
	case FACTORY_TECH_LAB:
	case STARPORT_TECH_LAB:
		return 18.0f;
	case SCV:
		return 12.0f;
	case MARINE:
		return 18.0f;
	case MARAUDER:
		return 21.0f;
	case REAPER:
		return 32.0f;
	case GHOST:
		return 29.0f;
	case HELLION:
	case HELLBAT:
		return 21.0f;
	case SIEGE_TANK:
	case SIEGE_TANK_SIEGED:
		return 32.0f;
	case CYCLONE:
		return 32.0f;
	case THOR_AOE:
	case THOR_AP:
		return 43.0f;
	case WIDOW_MINE:
	case WIDOW_MINE_BURROWED:
		return 21.0f;
	case VIKING:
	case VIKING_LANDED:
		return 30.0f;
	case MEDIVAC:
		return 30.0f;
	case LIBERATOR:
	case LIBERATOR_SIEGED:
		return 43.0f;
	case RAVEN:
		return 34.0f;
	case BANSHEE:
		return 43.0f;
	case BATTLECRUISER:
		return 64.0f;
	case DRONE:
	case DRONE_BURROWED:
		return 12.0f;
	case QUEEN:
	case QUEEN_BURROWED:
		return 36.0f;
	case ZERGLING:
	case ZERGLING_BURROWED:
		return 17.0f;
	case BANELING_EGG:
	case BANELING:
	case BANELING_BURROWED:
		return 14.0f;
	case ROACH:
	case ROACH_BURROWED:
		return 19.0f;
	case RAVAGER_EGG:
	case RAVAGER:
	case RAVAGER_BURROWED:
		return 12.0f;
	case HYDRA:
	case HYDRA_BURROWED:
		return 25.0f;
	case LURKER_EGG:
	case LURKER:
	case LURKER_BURROWED:
		return 18.0f;
	case INFESTOR:
	case INFESTOR_BURROWED:
		return 36.0f;
	case SWARMHOST:
	case SWARMHOST_BURROWED:
		return 29.0f;
	case ULTRALISK:
	case ULTRALISK_BURROWED:
		return 39.0f;
	case NYDUS_WORM:
		return 14.0f;
	case OVERLORD:
		return 18.0f;
	case DROPPERLORD_EGG:
	case DROPPERLORD:
		return 15.0f;
	case OVERSEER_EGG:
	case OVERSEER:
	case OVERSEER_SIEGED:
		return 12.0f;
	case MUTALISK:
		return 24.0f;
	case CORRUPTER:
		return 29.0f;
	case BROOD_LORD_EGG:
	case BROOD_LORD:
		return 24.0f;
	case VIPER:
		return 29.0f;
	case HATCHERY:
		return 71.0f;
	case LAIR:
		return 57.0f;
	case HIVE:
		return 71.0f;
	case SPINE_CRAWLER:
	case SPINE_CRAWLER_UPROOTED:
		return 36.0f;
	case SPORE_CRAWLER:
	case SPORE_CRAWLER_UPROOTED:
		return 21.0f;
	case SPAWNING_POOL:
		return 46.0f;
	case ROACH_WARREN:
		return 39.0f;
	case EVO_CHAMBER:
		return 25.0f;
	case EXTRACTOR:
	case EXTRACTOR_RICH:
		return 21.0f;
	case BANELING_NEST:
		return 43.0f;
	case HYDRA_DEN:
		return 29.0f; 
	case LURKER_DEN:
		return 57.0f;
	case INFESTATION_PIT:
		return 36.0f;
	case NYDUS:
		return 36.0f;
	case SPIRE:
		return 71.0f;
	case GREATER_SPIRE:
		return 71.0f;
	case ULTRALISK_CAVERN:
		return 46.0f;
	default:
		return 0.0f;
	}
}

AbilityID Utility::UnitToWarpInAbility(UNIT_TYPEID type)
{
	switch (type)
	{
	case ZEALOT:
		return ABILITY_ID::TRAINWARP_ZEALOT;
	case STALKER:
		return ABILITY_ID::TRAINWARP_STALKER;
	case ADEPT:
		return ABILITY_ID::TRAINWARP_ADEPT;
	case SENTRY:
		return ABILITY_ID::TRAINWARP_SENTRY;
	case HIGH_TEMPLAR:
		return ABILITY_ID::TRAINWARP_HIGHTEMPLAR;
	case DARK_TEMPLAR:
		return ABILITY_ID::TRAINWARP_DARKTEMPLAR;
	default:
		return ABILITY_ID::INVALID;
	}
}

UNIT_TYPEID Utility::GetBuildStructure(UNIT_TYPEID type)
{
	switch (type)
	{
	case ZEALOT:
	case SENTRY:
	case STALKER:
	case ADEPT:
	case HIGH_TEMPLAR:
	case DARK_TEMPLAR:
		return GATEWAY;
	case OBSERVER:
	case PRISM:
	case IMMORTAL:
	case COLOSSUS:
	case DISRUPTOR:
		return ROBO;
	case PHOENIX:
	case VOID_RAY:
	case ORACLE:
	case CARRIER:
	case TEMPEST:
		return STARGATE;
	case PROBE:
	case MOTHERSHIP:
		return NEXUS;
	default:
		std::cerr << "Error unknown unit in GetBuildStructure " << UnitTypeToName(type) << std::endl;
		return UNIT_TYPEID::INVALID;
	}
}


UNIT_TYPEID Utility::GetUpgradeStructure(UPGRADE_ID upgrade_id)
{
	switch (upgrade_id)
	{
	case U_AIR_WEAPONS_1:
	case U_AIR_WEAPONS_2:
	case U_AIR_WEAPONS_3:
	case U_AIR_ARMOR_1:
	case U_AIR_ARMOR_2:
	case U_AIR_ARMOR_3:
	case U_WARPGATE:
		return CYBERCORE;
	case U_BLINK:
	case U_CHARGE:
	case U_GLAIVES:
		return TWILIGHT;
	case U_DT_BLINK:
		return DARK_SHRINE;
	case U_STORM:
		return TEMPLAR_ARCHIVE;
	case U_THERMAL_LANCE:
	case U_PRISM_SPEED:
	case U_OBS_SPEED:
		return ROBO_BAY;
	case U_PHOENIX_RANGE:
	case U_FLUX_VANES:
	case U_TECTONIC_DESTABALIZERS:
		return FLEET_BEACON;
	case U_GROUND_WEAPONS_1:
	case U_GROUND_WEAPONS_2:
	case U_GROUND_WEAPONS_3:
	case U_GROUND_ARMOR_1:
	case U_GROUND_ARMOR_2:
	case U_GROUND_ARMOR_3:
	case U_SHIELDS_1:
	case U_SHIELDS_2:
	case U_SHIELDS_3:
		return FORGE;
	default:
		return UNIT_TYPEID::INVALID;
	}
}

int Utility::GetTrainingTime(UNIT_TYPEID type)
{
	switch (type)
	{
	case PROBE:
		return 12;
	case ZEALOT:
		return 27;
	case SENTRY:
		return 23;
	case STALKER:
		return 27;
	case ADEPT:
		return 30;
	case HIGH_TEMPLAR:
		return 39;
	case DARK_TEMPLAR:
		return 39;
	case OBSERVER:
		return 18;
	case PRISM:
		return 36;
	case IMMORTAL:
		return 39;
	case COLOSSUS:
		return 54;
	case DISRUPTOR:
		return 36;
	case PHOENIX:
		return 25;
	case VOID_RAY:
		return 43;
	case ORACLE:
		return 37;
	case CARRIER:
		return 64;
	case TEMPEST:
		return 43;
	case MOTHERSHIP:
		return 89;
	default:
		std::cerr << "Error unknown unit in GetTrainingTime " << UnitTypeToName(type) << std::endl;
		return 1;
	}
}

bool Utility::IsFacing(const Unit* unit, const Unit* target)
{
	Point2D vec = Point2D(target->pos.x - unit->pos.x, target->pos.y - unit->pos.y);
	double angle = atan2(vec.y, vec.x);
	if (angle < 0)
		angle += 2 * M_PI;
	float facing = unit->facing;
	return angle >= facing - .005 && angle <= facing + .005;
}

float Utility::GetFacingAngle(const Unit* unit, const Unit* target)
{
	Point2D vec = Point2D(target->pos.x - unit->pos.x, target->pos.y - unit->pos.y);
	double angle = atan2(vec.y, vec.x);
	if (angle < 0)
		angle += 2 * M_PI;
	return (float)std::abs(angle - unit->facing);
}

const Unit* Utility::AimingAt(const Unit* unit, Units allied_units)
{
	float smallest_angle = 180;
	const Unit* target = nullptr;


	for (const auto Funit : allied_units)
	{ // try flipped
		if (Distance2D(unit->pos, Funit->pos) >= RealRange(unit, Funit))
		{
			continue;
		}
		float angle = GetFacingAngle(unit, Funit);

		if (angle < smallest_angle)
		{
			smallest_angle = angle;
			target = Funit;
		}
	}

	return target;
}

float Utility::BuildingSize(UNIT_TYPEID buildingId)
{
	if (buildingId == NEXUS)
		return 2.5;
	if (buildingId == PYLON || buildingId == BATTERY || buildingId == CANNON)
		return 1;
	return 1.5;
}

float Utility::GetLargestUnitSize(Units units)
{
	float largest = 0;
	for (const auto& unit : units)
	{
		if (unit->radius > largest)
			largest = unit->radius;
	}
	return largest;
}

int Utility::BuildingsReady(UNIT_TYPEID buildingId, const ObservationInterface* observation)
{
	int ready = 0;
	for (const auto &building : observation->GetUnits(IsFriendlyUnit(buildingId)))
	{
		if (building->build_progress == 1)
			ready++;
	}
	return ready;
}

const Unit* Utility::GetLeastFullPrism(Units units)
{
	const Unit* least_full = nullptr;
	for (const auto &unit : units)
	{
		if (least_full == nullptr || unit->cargo_space_taken < least_full->cargo_space_taken)
			least_full = unit;
	}
	return least_full;
}

ABILITY_ID Utility::GetBuildAbility(UNIT_TYPEID buildingId)
{
	switch (buildingId)
	{
	case PYLON:
		return ABILITY_ID::BUILD_PYLON;
	case NEXUS:
		return ABILITY_ID::BUILD_NEXUS;
	case GATEWAY:
		return ABILITY_ID::BUILD_GATEWAY;
	case FORGE:
		return ABILITY_ID::BUILD_FORGE;
	case CYBERCORE:
		return ABILITY_ID::BUILD_CYBERNETICSCORE;
	case CANNON:
		return ABILITY_ID::BUILD_PHOTONCANNON;
	case BATTERY:
		return ABILITY_ID::BUILD_SHIELDBATTERY;
	case TWILIGHT:
		return ABILITY_ID::BUILD_TWILIGHTCOUNCIL;
	case STARGATE:
		return ABILITY_ID::BUILD_STARGATE;
	case ROBO:
		return ABILITY_ID::BUILD_ROBOTICSFACILITY;
	case ROBO_BAY:
		return ABILITY_ID::BUILD_ROBOTICSBAY;
	case TEMPLAR_ARCHIVE:
		return ABILITY_ID::BUILD_TEMPLARARCHIVE;
	case DARK_SHRINE:
		return ABILITY_ID::BUILD_DARKSHRINE;
	case FLEET_BEACON:
		return ABILITY_ID::BUILD_FLEETBEACON;
	case ASSIMILATOR:
		return ABILITY_ID::BUILD_ASSIMILATOR;
	default:
		std::cout << "Error invalid building id in GetBuildAbility " << UnitTypeToName(buildingId) << std::endl;
		return ABILITY_ID::BUILD_CANCEL;
	}
}

ABILITY_ID Utility::GetTrainAbility(UNIT_TYPEID unitId)
{
	switch (unitId)
	{
	case PROBE:
		return ABILITY_ID::TRAIN_PROBE;
	case ZEALOT:
		return ABILITY_ID::TRAIN_ZEALOT;
	case ADEPT:
		return ABILITY_ID::TRAIN_ADEPT;
	case STALKER:
		return ABILITY_ID::TRAIN_STALKER;
	case SENTRY:
		return ABILITY_ID::TRAIN_SENTRY;
	case HIGH_TEMPLAR:
		return ABILITY_ID::TRAIN_HIGHTEMPLAR;
	case DARK_TEMPLAR:
		return ABILITY_ID::TRAIN_DARKTEMPLAR;
	case IMMORTAL:
		return ABILITY_ID::TRAIN_IMMORTAL;
	case COLOSSUS:
		return ABILITY_ID::TRAIN_COLOSSUS;
	case DISRUPTOR:
		return ABILITY_ID::TRAIN_DISRUPTOR;
	case OBSERVER:
		return ABILITY_ID::TRAIN_OBSERVER;
	case PRISM:
		return ABILITY_ID::TRAIN_WARPPRISM;
	case PHOENIX:
		return ABILITY_ID::TRAIN_PHOENIX;
	case VOID_RAY:
		return ABILITY_ID::TRAIN_VOIDRAY;
	case ORACLE:
		return ABILITY_ID::TRAIN_ORACLE;
	case CARRIER:
		return ABILITY_ID::TRAIN_CARRIER;
	case TEMPEST:
		return ABILITY_ID::TRAIN_TEMPEST;
	case MOTHERSHIP:
		return ABILITY_ID::TRAIN_MOTHERSHIP;
	default:
		std::cout << "Error invalid unit id in GetTrainAbility " << UnitTypeToName(unitId) << std::endl;
		return ABILITY_ID::BUILD_CANCEL;
	}
}

ABILITY_ID Utility::GetWarpAbility(UNIT_TYPEID unitId)
{
	switch (unitId)
	{
	case ZEALOT:
		return ABILITY_ID::TRAINWARP_ZEALOT;
	case ADEPT:
		return ABILITY_ID::TRAINWARP_ADEPT;
	case STALKER:
		return ABILITY_ID::TRAINWARP_STALKER;
	case SENTRY:
		return ABILITY_ID::TRAINWARP_SENTRY;
	case HIGH_TEMPLAR:
		return ABILITY_ID::TRAINWARP_HIGHTEMPLAR;
	case DARK_TEMPLAR:
		return ABILITY_ID::TRAINWARP_DARKTEMPLAR;
	default:
		std::cout << "Error invalid unit id in GetWarpAbility " << UnitTypeToName(unitId) << std::endl;
		return ABILITY_ID::BUILD_CANCEL;
	}
}

ABILITY_ID Utility::GetUpgradeAbility(UPGRADE_ID upgrade_id)
{
	switch (upgrade_id)
	{
	case UPGRADE_ID::WARPGATERESEARCH:
		return ABILITY_ID::RESEARCH_WARPGATE;
	case UPGRADE_ID::BLINKTECH:
		return ABILITY_ID::RESEARCH_BLINK;
	case UPGRADE_ID::CHARGE:
		return ABILITY_ID::RESEARCH_CHARGE;
	case UPGRADE_ID::ADEPTPIERCINGATTACK:
		return ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES;
	case UPGRADE_ID::DARKTEMPLARBLINKUPGRADE:
		return ABILITY_ID::RESEARCH_SHADOWSTRIKE;
	case UPGRADE_ID::PSISTORMTECH:
		return ABILITY_ID::RESEARCH_PSISTORM;
	case UPGRADE_ID::OBSERVERGRAVITICBOOSTER:
		return ABILITY_ID::RESEARCH_GRAVITICBOOSTER;
	case UPGRADE_ID::GRAVITICDRIVE:
		return ABILITY_ID::RESEARCH_GRAVITICDRIVE;
	case UPGRADE_ID::EXTENDEDTHERMALLANCE:
		return ABILITY_ID::RESEARCH_EXTENDEDTHERMALLANCE;
	case UPGRADE_ID::ANIONPULSECRYSTALS:
		return ABILITY_ID::RESEARCH_PHOENIXANIONPULSECRYSTALS;
	case UPGRADE_ID::VOIDRAYSPEEDUPGRADE:
		return ABILITY_ID::RESEARCH_VOIDRAYSPEEDUPGRADE;
	case UPGRADE_ID::TEMPESTGROUNDATTACKUPGRADE:
		return ABILITY_ID::RESEARCH_TEMPESTRESEARCHGROUNDATTACKUPGRADE;
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1:
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2:
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3:
		return A_RESEARCH_GROUND_WEAPONS;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:
		return A_RESEARCH_GROUND_ARMOR;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL1:
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL2:
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL3:
		return A_RESEARCH_SHIELDS;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL1:
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL2:
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL3:
		return A_RESEARCH_AIR_ARMOR;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1:
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2:
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3:
		return A_RESEARCH_AIR_WEAPONS;
	default:
		std::cout << "Error invalid unit id in GetUpgradeAbility " << UpgradeIDToName(upgrade_id) << std::endl;
		return ABILITY_ID::BUILD_CANCEL;
	}
}

int Utility::GetWarpCooldown(UNIT_TYPEID unitId)
{
	switch (unitId)
	{
	case ZEALOT:
		return 20;
	case ADEPT:
		return 20;
	case STALKER:
		return 23;
	case SENTRY:
		return 23;
	case HIGH_TEMPLAR:
		return 32;
	case DARK_TEMPLAR:
		return 32;
	default:
		std::cout << "Error invalid unit id in GetWarpCooldown " << UnitTypeToName(unitId) << std::endl;
		return 0;
	}
}

UnitCost Utility::GetCost(UNIT_TYPEID unit_type)
{
	switch (unit_type)
	{
	case PYLON:
		return UnitCost(100, 0, 0);
	case NEXUS:
		return UnitCost(400, 0, 0);
	case GATEWAY:
		return UnitCost(150, 0, 0);
	case FORGE:
		return UnitCost(150, 0, 0);
	case CYBERCORE:
		return UnitCost(150, 0, 0);
	case CANNON:
		return UnitCost(150, 0, 0);
	case BATTERY:
		return UnitCost(100, 0, 0);
	case TWILIGHT:
		return UnitCost(150, 100, 0);
	case STARGATE:
		return UnitCost(150, 150, 0);
	case ROBO:
		return UnitCost(150, 100, 0);
	case ROBO_BAY:
		return UnitCost(150, 150, 0);
	case TEMPLAR_ARCHIVE:
		return UnitCost(150, 200, 0);
	case DARK_SHRINE:
		return UnitCost(150, 150, 0);
	case FLEET_BEACON:
		return UnitCost(300, 200, 0);
	case ASSIMILATOR:
		return UnitCost(75, 0, 0);
	case PROBE:
		return UnitCost(50, 0, 1);
	case ZEALOT:
		return UnitCost(100, 0, 2);
	case STALKER:
		return UnitCost(125, 50, 2);
	case SENTRY:
		return UnitCost(50, 100, 2);
	case ADEPT:
		return UnitCost(100, 25, 2);
	case HIGH_TEMPLAR:
		return UnitCost(50, 150, 2);
	case DARK_TEMPLAR:
		return UnitCost(125, 125, 2);
	case IMMORTAL:
		return UnitCost(275, 100, 4);
	case COLOSSUS:
		return UnitCost(300, 200, 6);
	case DISRUPTOR:
		return UnitCost(150, 150, 3);
	case OBSERVER:
		return UnitCost(25, 75, 1);
	case PRISM:
		return UnitCost(250, 0, 2);
	case PHOENIX:
		return UnitCost(150, 100, 2);
	case VOID_RAY:
		return UnitCost(250, 150, 4);
	case ORACLE:
		return UnitCost(150, 150, 3);
	case CARRIER:
		return UnitCost(350, 250, 6);
	case TEMPEST:
		return UnitCost(250, 175, 5);
	case MOTHERSHIP:
		return UnitCost(400, 400, 8);
	case UNIT_TYPEID::BEACON_PROTOSS: // represents void ray + fleet beacon
		return UnitCost(550, 350, 4);
	case COMMAND_CENTER:
	case COMMAND_CENTER_FLYING:
		return UnitCost(400, 0, 0);
	case PLANETARY:
		return UnitCost(550, 150, 0);
	case ORBITAL:
	case ORBITAL_FLYING:
		return UnitCost(550, 0, 0);
	case SUPPLY_DEPOT:
	case SUPPLY_DEPOT_LOWERED:
		return UnitCost(100, 0, 0);
	case REFINERY:
	case REFINERY_RICH:
		return UnitCost(75, 0, 0);
	case BARRACKS:
	case BARRACKS_FLYING:
		return UnitCost(150, 0, 0);
	case ENGINEERING_BAY:
		return UnitCost(125, 0, 0);
	case BUNKER:
	case NEOSTEEL_BUNKER:
		return UnitCost(100, 0, 0);
	case SENSOR_TOWER:
		return UnitCost(100, 50, 0);
	case MISSILE_TURRET:
		return UnitCost(100, 0, 0);
	case FACTORY:
		return UnitCost(150, 100, 0);
	case FACTORY_FLYING:
		return UnitCost(150, 100, 0);
	case GHOST_ACADEMY:
		return UnitCost(150, 50, 0);
	case STARPORT:
		return UnitCost(150, 100, 0);
	case STARPORT_FLYING:
		return UnitCost(150, 100, 0);
	case ARMORY:
		return UnitCost(150, 50, 0);
	case FUSION_CORE:
		return UnitCost(1505, 150, 0);
	case TECH_LAB:
	case BARRACKS_TECH_LAB:
	case FACTORY_TECH_LAB:
	case STARPORT_TECH_LAB:
		return UnitCost(50, 25, 0);
	case REACTOR:
	case BARRACKS_REACTOR:
	case FACTORY_REACTOR:
	case STARPORT_REACTOR:
		return UnitCost(50, 50, 0);
	case SCV:
		return UnitCost(50, 0, 1);
	case MULE:
		return UnitCost(0, 0, 0);
	case MARINE:
		return UnitCost(50, 0, 1);
	case MARAUDER:
		return UnitCost(100, 25, 2);
	case REAPER:
		return UnitCost(50, 50, 1);
	case GHOST:
		return UnitCost(150, 125, 3);
	case HELLION:
	case HELLBAT:
		return UnitCost(100, 0, 2);
	case SIEGE_TANK:
	case SIEGE_TANK_SIEGED:
		return UnitCost(150, 125, 3);
	case CYCLONE:
		return UnitCost(150, 100, 3);
	case THOR_AOE:
	case THOR_AP:
		return UnitCost(300, 200, 6);
	case WIDOW_MINE:
	case WIDOW_MINE_BURROWED:
		return UnitCost(75, 25, 2);
	case VIKING:
	case VIKING_LANDED:
		return UnitCost(150, 75, 2);
	case MEDIVAC:
		return UnitCost(100, 100, 2);
	case LIBERATOR:
	case LIBERATOR_SIEGED:
		return UnitCost(150, 125, 3);
	case RAVEN:
		return UnitCost(100, 150, 2);
	case BANSHEE:
		return UnitCost(150, 100, 3);
	case BATTLECRUISER:
		return UnitCost(400, 300, 6);
	case DRONE:
	case DRONE_BURROWED:
		return UnitCost(50, 0, 1);
	case QUEEN:
	case QUEEN_BURROWED:
		return UnitCost(175, 0, 2);
	case ZERGLING:
	case ZERGLING_BURROWED:
		return UnitCost(25, 0, .5f);
	case BANELING_EGG:
	case BANELING:
	case BANELING_BURROWED:
		return UnitCost(50, 25, .5f);
	case ROACH:
	case ROACH_BURROWED:
		return UnitCost(75, 25, 2);
	case RAVAGER_EGG:
	case RAVAGER:
	case RAVAGER_BURROWED:
		return UnitCost(100, 100, 3);
	case HYDRA:
	case HYDRA_BURROWED:
		return UnitCost(100, 50, 2);
	case LURKER_EGG:
	case LURKER:
	case LURKER_BURROWED:
		return UnitCost(150, 150, 3);
	case INFESTOR:
	case INFESTOR_BURROWED:
		return UnitCost(100, 150, 2);
	case SWARMHOST:
	case SWARMHOST_BURROWED:
		return UnitCost(100, 75, 3);
	case ULTRALISK:
	case ULTRALISK_BURROWED:
		return UnitCost(275, 200, 6);
	case OVERLORD:
		return UnitCost(100, 0, 0);
	case DROPPERLORD_EGG:
	case DROPPERLORD:
		return UnitCost(125, 25, 0);
	case OVERSEER_EGG:
	case OVERSEER:
	case OVERSEER_SIEGED:
		return UnitCost(150, 50, 0);
	case MUTALISK:
		return UnitCost(100, 100, 2);
	case CORRUPTER:
		return UnitCost(150, 100, 2);
	case BROOD_LORD_EGG:
	case BROOD_LORD:
		return UnitCost(300, 250, 4);
	case VIPER:
		return UnitCost(100, 200, 3);
	case HATCHERY:
		return UnitCost(275, 0, 0);
	case LAIR:
		return UnitCost(425, 100, 0);
	case HIVE:
		return UnitCost(650, 250, 0);
	case SPINE_CRAWLER:
	case SPINE_CRAWLER_UPROOTED:
		return UnitCost(100, 0, 0);
	case SPORE_CRAWLER:
	case SPORE_CRAWLER_UPROOTED:
		return UnitCost(75, 0, 0);
	case SPAWNING_POOL:
		return UnitCost(200, 0, 0);
	case ROACH_WARREN:
		return UnitCost(150, 0, 0);
	case EVO_CHAMBER:
		return UnitCost(75, 0, 0);
	case EXTRACTOR:
	case EXTRACTOR_RICH:
		return UnitCost(25, 0, 0);
	case BANELING_NEST:
		return UnitCost(100, 50, 0);
	case HYDRA_DEN:
		return UnitCost(100, 100, 0);
	case LURKER_DEN:
		return UnitCost(100, 150, 0);
	case INFESTATION_PIT:
		return UnitCost(100, 100, 0);
	case NYDUS:
		return UnitCost(150, 150, 0);
	case SPIRE:
		return UnitCost(200, 200, 0);
	case GREATER_SPIRE:
		return UnitCost(300, 350, 0);
	case ULTRALISK_CAVERN:
		return UnitCost(150, 200, 0);
	default:
		return UnitCost(0, 0, 0);
	}
}



UnitCost Utility::GetCost(UPGRADE_ID upgrade_id)
{
	switch (upgrade_id)
	{
case U_WARPGATE:
	return UnitCost(50, 50, 0);
case U_BLINK:
	return UnitCost(150, 150, 0);
case U_CHARGE:
	return UnitCost(100, 100, 0);
case U_GLAIVES:
	return UnitCost(100, 100, 0);
case U_DT_BLINK:
	return UnitCost(100, 100, 0);
case U_STORM:
	return UnitCost(200, 200, 0);
case U_THERMAL_LANCE:
	return UnitCost(150, 150, 0);
case U_PRISM_SPEED:
	return UnitCost(100, 100, 0);
case U_OBS_SPEED:
	return UnitCost(100, 100, 0);
case U_PHOENIX_RANGE:
	return UnitCost(150, 150, 0);
case U_FLUX_VANES:
	return UnitCost(150, 150, 0);
case U_TECTONIC_DESTABALIZERS:
	return UnitCost(150, 150, 0);
case U_GROUND_WEAPONS_1:
	return UnitCost(100, 100, 0);
case U_GROUND_WEAPONS_2:
	return UnitCost(150, 150, 0);
case U_GROUND_WEAPONS_3:
	return UnitCost(200, 200, 0);
case U_GROUND_ARMOR_1:
	return UnitCost(100, 100, 0);
case U_GROUND_ARMOR_2:
	return UnitCost(150, 150, 0);
case U_GROUND_ARMOR_3:
	return UnitCost(200, 200, 0);
case U_SHIELDS_1:
	return UnitCost(150, 150, 0);
case U_SHIELDS_2:
	return UnitCost(225, 225, 0);
case U_SHIELDS_3:
	return UnitCost(300, 300, 0);
case U_AIR_WEAPONS_1:
	return UnitCost(150, 150, 0);
case U_AIR_WEAPONS_2:
	return UnitCost(225, 225, 0);
case U_AIR_WEAPONS_3:
	return UnitCost(300, 300, 0);
case U_AIR_ARMOR_1:
	return UnitCost(100, 100, 0);
case U_AIR_ARMOR_2:
	return UnitCost(175, 175, 0);
case U_AIR_ARMOR_3:
	return UnitCost(250, 250, 0);
	default:
		std::cerr << "Error invalid upgrade id in GetCost " << UpgradeIDToName(upgrade_id) << std::endl;
		return UnitCost(0, 0, 0);;
	}
}

bool Utility::CanAfford(UNIT_TYPEID unit, int amount, const ObservationInterface* observation)
{
	UnitCost cost;
	switch (unit)
	{
	case PYLON:
		cost = UnitCost(100, 0, 0);
		break;
	case NEXUS:
		cost = UnitCost(400, 0, 0);
		break;
	case GATEWAY:
		cost = UnitCost(150, 0, 0);
		break;
	case FORGE:
		cost = UnitCost(150, 0, 0);
		break;
	case CYBERCORE:
		cost = UnitCost(150, 0, 0);
		break;
	case CANNON:
		cost = UnitCost(150, 0, 0);
		break;
	case BATTERY:
		cost = UnitCost(100, 0, 0);
		break;
	case TWILIGHT:
		cost = UnitCost(150, 100, 0);
		break;
	case STARGATE:
		cost = UnitCost(150, 150, 0);
		break;
	case ROBO:
		cost = UnitCost(150, 100, 0);
		break;
	case ROBO_BAY:
		cost = UnitCost(150, 150, 0);
		break;
	case TEMPLAR_ARCHIVE:
		cost = UnitCost(150, 200, 0);
		break;
	case DARK_SHRINE:
		cost = UnitCost(150, 150, 0);
		break;
	case FLEET_BEACON:
		cost = UnitCost(300, 200, 0);
		break;
	case ASSIMILATOR:
		cost = UnitCost(75, 0, 0);
		break;
	case PROBE:
		cost = UnitCost(50, 0, 1);
		break;
	case ZEALOT:
		cost = UnitCost(100, 0, 2);
		break;
	case STALKER:
		cost = UnitCost(125, 50, 2);
		break;
	case SENTRY:
		cost = UnitCost(50, 100, 2);
		break;
	case ADEPT:
		cost = UnitCost(100, 25, 2);
		break;
	case HIGH_TEMPLAR:
		cost = UnitCost(50, 150, 2);
		break;
	case DARK_TEMPLAR:
		cost = UnitCost(125, 125, 2);
		break;
	case IMMORTAL:
		cost = UnitCost(275, 100, 4);
		break;
	case COLOSSUS:
		cost = UnitCost(300, 200, 6);
		break;
	case DISRUPTOR:
		cost = UnitCost(150, 150, 3);
		break;
	case OBSERVER:
		cost = UnitCost(25, 75, 1);
		break;
	case PRISM:
		cost = UnitCost(250, 0, 2);
		break;
	case PHOENIX:
		cost = UnitCost(150, 100, 2);
		break;
	case VOID_RAY:
		cost = UnitCost(250, 150, 4);
		break;
	case ORACLE:
		cost = UnitCost(150, 150, 3);
		break;
	case CARRIER:
		cost = UnitCost(350, 250, 6);
		break;
	case TEMPEST:
		cost = UnitCost(250, 175, 5);
		break;
	case MOTHERSHIP:
		cost = UnitCost(400, 400, 8);
		break;
	default:
		std::cerr << "Error invalid unit id in CanAfford " << UnitTypeToName(unit) << std::endl;
		return false;
	}
	bool enough_minerals = (int)observation->GetMinerals() >= cost.mineral_cost * amount;
	bool enough_vespene = (int)observation->GetVespene() >= cost.vespene_cost * amount;
	bool enough_supply = (int)(observation->GetFoodCap() - observation->GetFoodUsed()) >= cost.supply * amount;
	return enough_minerals && enough_vespene && enough_supply;
}

bool Utility::CanAffordAfter(UNIT_TYPEID unit, UnitCost already_spent, const ObservationInterface* observation)
{
	UnitCost total_cost = GetCost(unit) + already_spent;
	bool enough_minerals = (int)observation->GetMinerals() >= total_cost.mineral_cost;
	bool enough_vespene = (int)observation->GetVespene() >= total_cost.vespene_cost;
	bool enough_supply = (int)(observation->GetFoodCap() - observation->GetFoodUsed()) >= total_cost.supply;
	return enough_minerals && enough_vespene && enough_supply;
}

bool Utility::CanAffordUpgrade(UPGRADE_ID upgrade, const ObservationInterface* observation)
{
	UnitCost cost;
	switch (upgrade)
	{
	case UPGRADE_ID::WARPGATERESEARCH:
		cost = UnitCost(50, 50, 0);
		break;
	case UPGRADE_ID::BLINKTECH:
		cost = UnitCost(150, 150, 0);
		break;
	case UPGRADE_ID::CHARGE:
		cost = UnitCost(100, 100, 0);
		break;
	case UPGRADE_ID::ADEPTPIERCINGATTACK:
		cost = UnitCost(100, 100, 0);
		break;
	case UPGRADE_ID::DARKTEMPLARBLINKUPGRADE:
		cost = UnitCost(100, 100, 0);
		break;
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1:
		cost = UnitCost(100, 100, 0);
		break;
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2:
		cost = UnitCost(150, 150, 0);
		break;
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3:
		cost = UnitCost(200, 200, 0);
		break;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:
		cost = UnitCost(100, 100, 0);
		break;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:
		cost = UnitCost(150, 150, 0);
		break;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:
		cost = UnitCost(200, 200, 0);
		break;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL1:
		cost = UnitCost(150, 150, 0);
		break;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL2:
		cost = UnitCost(225, 225, 0);
		break;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL3:
		cost = UnitCost(300, 300, 0);
		break;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL1:
		cost = UnitCost(150, 150, 0);
		break;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL2:
		cost = UnitCost(225, 225, 0);
		break;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL3:
		cost = UnitCost(300, 300, 0);
		break;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1:
		cost = UnitCost(100, 100, 0);
		break;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2:
		cost = UnitCost(175, 175, 0);
		break;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3:
		cost = UnitCost(250, 250, 0);
		break;
	default:
		std::cerr << "Error invalid upgrade id in CanAffordUpgrade " << UpgradeIDToName(upgrade) << std::endl;
		return false;
	}
	bool enough_minerals = observation->GetMinerals() >= (uint32_t)cost.mineral_cost;
	bool enough_vespene = observation->GetVespene() >= (uint32_t)cost.vespene_cost;
	return enough_minerals && enough_vespene;
}

int Utility::MaxCanAfford(UNIT_TYPEID unit, const ObservationInterface* observation)
{
	int i = 1;
	while (true)
	{
		if (CanAfford(unit, i, observation) == false)
			return i - 1;
		i++;
	}
}

std::vector<double> Utility::GetRealQuarticRoots(double A, double B, double C, double D, double E)
{
	std::complex<double>* roots = solve_quartic(B / A, C / A, D / A, E / A);
	std::vector<double> real_roots;
	for (int i = 0; i < 4; i++)
	{
		if (roots[i].imag() == 0)
			real_roots.push_back(roots[i].real());
	}
	delete roots;
	return real_roots;
}

bool Utility::IsBiological(UNIT_TYPEID type)
{
	switch (type)
	{
	case ZEALOT:
	case ADEPT:
	case HIGH_TEMPLAR:
	case DARK_TEMPLAR:
	case SCV:
	case MARINE:
	case MARAUDER:
	case REAPER:
	case GHOST:
	case HELLBAT:
	case LARVA:
	case EGG:
	case DRONE:
	case DRONE_BURROWED:
	case QUEEN:
	case QUEEN_BURROWED:
	case ZERGLING:
	case ZERGLING_BURROWED:
	case BANELING_EGG:
	case BANELING:
	case BANELING_BURROWED:
	case ROACH:
	case ROACH_BURROWED:
	case RAVAGER_EGG:
	case RAVAGER:
	case RAVAGER_BURROWED:
	case HYDRA:
	case HYDRA_BURROWED:
	case LURKER_EGG:
	case LURKER:
	case LURKER_BURROWED:
	case INFESTOR:
	case INFESTOR_BURROWED:
	case SWARMHOST:
	case SWARMHOST_BURROWED:
	case ULTRALISK:
	case ULTRALISK_BURROWED:
	case LOCUST:
	case LOCUST_FLYING:
	case BROODLING:
	case CHANGELING:
	case CHANGELING_ZEALOT:
	case NYDUS_WORM:
	case OVERLORD:
	case DROPPERLORD:
	case OVERSEER_EGG:
	case OVERSEER:
	case OVERSEER_SIEGED:
	case MUTALISK:
	case CORRUPTER:
	case BROOD_LORD_EGG:
	case BROOD_LORD:
	case VIPER:
	case HATCHERY:
	case LAIR:
	case HIVE:
	case SPINE_CRAWLER:
	case SPINE_CRAWLER_UPROOTED:
	case SPORE_CRAWLER:
	case SPORE_CRAWLER_UPROOTED:
	case SPAWNING_POOL:
	case ROACH_WARREN:
	case EVO_CHAMBER:
	case EXTRACTOR:
	case BANELING_NEST:
	case HYDRA_DEN:
	case LURKER_DEN:
	case INFESTATION_PIT:
	case NYDUS:
	case SPIRE:
	case GREATER_SPIRE:
	case ULTRALISK_CAVERN:
	case CREEP_TUMOR_1:
	case CREEP_TUMOR_2:
	case CREEP_TUMOR_3:
		return true;
	}
	return false;
}


bool Utility::IsMechanical(UNIT_TYPEID type)
{
	switch (type)
	{
	case PROBE:
	case STALKER:
	case SENTRY:
	case IMMORTAL:
	case COLOSSUS:
	case DISRUPTOR:
	case OBSERVER:
	case OBSERVER_SIEGED:
	case PRISM:
	case PRISM_SIEGED:
	case PHOENIX:
	case VOID_RAY:
	case ORACLE:
	case CARRIER:
	case TEMPEST:
	case MOTHERSHIP:
	case COMMAND_CENTER:
	case COMMAND_CENTER_FLYING:
	case PLANETARY:
	case ORBITAL:
	case ORBITAL_FLYING:
	case SUPPLY_DEPOT:
	case SUPPLY_DEPOT_LOWERED:
	case REFINERY:
	case BARRACKS:
	case BARRACKS_FLYING:
	case ENGINEERING_BAY:
	case BUNKER:
	case SENSOR_TOWER:
	case MISSILE_TURRET:
	case FACTORY:
	case FACTORY_FLYING:
	case GHOST_ACADEMY:
	case STARPORT:
	case STARPORT_FLYING:
	case ARMORY:
	case FUSION_CORE:
	case TECH_LAB:
	case REACTOR:
	case BARRACKS_TECH_LAB:
	case BARRACKS_REACTOR:
	case FACTORY_TECH_LAB:
	case FACTORY_REACTOR:
	case STARPORT_TECH_LAB:
	case STARPORT_REACTOR:
	case SCV:
	case MULE:
	case HELLION:
	case HELLBAT:
	case SIEGE_TANK:
	case SIEGE_TANK_SIEGED:
	case CYCLONE:
	case THOR_AOE:
	case THOR_AP:
	case WIDOW_MINE:
	case AUTO_TURRET:
	case VIKING:
	case VIKING_LANDED:
	case MEDIVAC:
	case LIBERATOR:
	case LIBERATOR_SIEGED:
	case RAVEN:
	case BANSHEE:
	case BATTLECRUISER:
		return true;
	}
	return false;
}

bool Utility::IsLight(UNIT_TYPEID type)
{
	switch (type)
	{
	case PROBE:
	case ZEALOT:
	case ADEPT:
	case HIGH_TEMPLAR:
	case DARK_TEMPLAR:
	case OBSERVER:
	case OBSERVER_SIEGED:
	case PHOENIX:
	case SCV:
	case MULE:
	case MARINE:
	case REAPER:
	case HELLION:
	case HELLBAT:
	case WIDOW_MINE:
	case RAVEN:
	case BANSHEE:
	case LARVA:
	case DRONE:
	case DRONE_BURROWED:
	case ZERGLING:
	case ZERGLING_BURROWED:
	case HYDRA:
	case HYDRA_BURROWED:
	case LOCUST:
	case LOCUST_FLYING:
	case BROODLING:
	case CHANGELING:
	case CHANGELING_ZEALOT:
	case MUTALISK:
		return true;
	}
	return false;
}

bool Utility::IsArmored(UNIT_TYPEID type)
{
	switch (type)
	{
	case NEXUS:
	case PYLON:
	case GATEWAY:
	case WARP_GATE:
	case ASSIMILATOR:
	case FORGE:
	case CYBERCORE:
	case CANNON:
	case BATTERY:
	case TWILIGHT:
	case TEMPLAR_ARCHIVE:
	case STARGATE:
	case FLEET_BEACON:
	case DARK_SHRINE:
	case ROBO:
	case ROBO_BAY:
	case STALKER:
	case IMMORTAL:
	case COLOSSUS:
	case DISRUPTOR:
	case PRISM:
	case PRISM_SIEGED:
	case VOID_RAY:
	case ORACLE:
	case CARRIER:
	case TEMPEST:
	case MOTHERSHIP:
	case COMMAND_CENTER:
	case COMMAND_CENTER_FLYING:
	case PLANETARY:
	case ORBITAL:
	case ORBITAL_FLYING:
	case SUPPLY_DEPOT:
	case SUPPLY_DEPOT_LOWERED:
	case REFINERY:
	case BARRACKS:
	case BARRACKS_FLYING:
	case ENGINEERING_BAY:
	case BUNKER:
	case SENSOR_TOWER:
	case MISSILE_TURRET:
	case FACTORY:
	case FACTORY_FLYING:
	case GHOST_ACADEMY:
	case STARPORT:
	case STARPORT_FLYING:
	case ARMORY:
	case FUSION_CORE:
	case TECH_LAB:
	case REACTOR:
	case BARRACKS_TECH_LAB:
	case BARRACKS_REACTOR:
	case FACTORY_TECH_LAB:
	case FACTORY_REACTOR:
	case STARPORT_TECH_LAB:
	case STARPORT_REACTOR:
	case MARAUDER:
	case SIEGE_TANK:
	case SIEGE_TANK_SIEGED:
	case CYCLONE:
	case THOR_AOE:
	case THOR_AP:
	case AUTO_TURRET:
	case VIKING:
	case VIKING_LANDED:
	case MEDIVAC:
	case LIBERATOR:
	case LIBERATOR_SIEGED:
	case BATTLECRUISER:
	case ROACH:
	case ROACH_BURROWED:
	case LURKER:
	case LURKER_BURROWED:
	case SWARMHOST:
	case SWARMHOST_BURROWED:
	case ULTRALISK:
	case ULTRALISK_BURROWED:
	case NYDUS_WORM:
	case OVERLORD:
	case DROPPERLORD:
	case OVERSEER_EGG:
	case OVERSEER:
	case OVERSEER_SIEGED:
	case CORRUPTER:
	case BROOD_LORD:
	case VIPER:
	case HATCHERY:
	case LAIR:
	case HIVE:
	case SPINE_CRAWLER:
	case SPINE_CRAWLER_UPROOTED:
	case SPORE_CRAWLER:
	case SPORE_CRAWLER_UPROOTED:
	case SPAWNING_POOL:
	case ROACH_WARREN:
	case EVO_CHAMBER:
	case EXTRACTOR:
	case BANELING_NEST:
	case HYDRA_DEN:
	case LURKER_DEN:
	case INFESTATION_PIT:
	case NYDUS:
	case SPIRE:
	case GREATER_SPIRE:
	case ULTRALISK_CAVERN:
		return true;
	}
	return false;
}

bool Utility::IsMassive(UNIT_TYPEID type)
{
	switch (type)
	{
	case COLOSSUS:
	case ARCHON:
	case CARRIER:
	case TEMPEST:
	case MOTHERSHIP:
	case THOR_AOE:
	case THOR_AP:
	case BATTLECRUISER:
	case ULTRALISK:
	case ULTRALISK_BURROWED:
	case BROOD_LORD_EGG:
	case BROOD_LORD:
		return true;
	}
	return false;
}

bool Utility::IsStructure(UNIT_TYPEID type)
{
	switch (type)
	{
	case NEXUS:
	case PYLON:
	case GATEWAY:
	case WARP_GATE:
	case ASSIMILATOR:
	case FORGE:
	case CYBERCORE:
	case CANNON:
	case BATTERY:
	case TWILIGHT:
	case TEMPLAR_ARCHIVE:
	case STARGATE:
	case FLEET_BEACON:
	case DARK_SHRINE:
	case ROBO:
	case ROBO_BAY:
	case COMMAND_CENTER:
	case COMMAND_CENTER_FLYING:
	case PLANETARY:
	case ORBITAL:
	case ORBITAL_FLYING:
	case SUPPLY_DEPOT:
	case SUPPLY_DEPOT_LOWERED:
	case REFINERY:
	case BARRACKS:
	case BARRACKS_FLYING:
	case ENGINEERING_BAY:
	case BUNKER:
	case SENSOR_TOWER:
	case MISSILE_TURRET:
	case FACTORY:
	case FACTORY_FLYING:
	case GHOST_ACADEMY:
	case STARPORT:
	case STARPORT_FLYING:
	case ARMORY:
	case FUSION_CORE:
	case TECH_LAB:
	case REACTOR:
	case BARRACKS_TECH_LAB:
	case BARRACKS_REACTOR:
	case FACTORY_TECH_LAB:
	case FACTORY_REACTOR:
	case STARPORT_TECH_LAB:
	case STARPORT_REACTOR:
	case NYDUS_WORM:
	case HATCHERY:
	case LAIR:
	case HIVE:
	case SPINE_CRAWLER:
	case SPINE_CRAWLER_UPROOTED:
	case SPORE_CRAWLER:
	case SPORE_CRAWLER_UPROOTED:
	case SPAWNING_POOL:
	case ROACH_WARREN:
	case EVO_CHAMBER:
	case EXTRACTOR:
	case BANELING_NEST:
	case HYDRA_DEN:
	case LURKER_DEN:
	case INFESTATION_PIT:
	case NYDUS:
	case SPIRE:
	case GREATER_SPIRE:
	case ULTRALISK_CAVERN:
	case CREEP_TUMOR_1:
	case CREEP_TUMOR_2:
	case CREEP_TUMOR_3:
		return true;
	}
	return false;
}

bool Utility::IsMelee(UNIT_TYPEID type)
{
	switch (type)
	{
	case PROBE:
	case ZEALOT:
	case DARK_TEMPLAR:
	case SCV:
	case DRONE:
	case ZERGLING:
	case ULTRALISK:
	case BROODLING:
		return true;
	}
	return false;
}

bool Utility::OnSameLevel(Point3D pos1, Point3D pos2)
{
	return pos1.z + .15 > pos2.z && pos1.z - .15 < pos2.z;
}

float Utility::GetAbilityTime(ABILITY_ID ability)
{
	switch (ability)
	{
	case ABILITY_ID::TRAIN_PROBE:
		return 12;
	case ABILITY_ID::TRAIN_ZEALOT:
		return 27;
	case ABILITY_ID::TRAIN_ADEPT:
		return 30;
	case ABILITY_ID::TRAIN_STALKER:
		return 27;
	case ABILITY_ID::TRAIN_SENTRY:
		return 23;
	case ABILITY_ID::TRAIN_HIGHTEMPLAR:
		return 39;
	case ABILITY_ID::TRAIN_DARKTEMPLAR:
		return 39;
	case ABILITY_ID::TRAIN_IMMORTAL:
		return 39;
	case ABILITY_ID::TRAIN_OBSERVER:
		return 18;
	case ABILITY_ID::TRAIN_WARPPRISM:
		return 36;
	case ABILITY_ID::TRAIN_COLOSSUS:
		return 54;
	case ABILITY_ID::TRAIN_DISRUPTOR:
		return 36;
	case ABILITY_ID::TRAIN_PHOENIX:
		return 25;
	case ABILITY_ID::TRAIN_ORACLE:
		return 37;
	case ABILITY_ID::TRAIN_VOIDRAY:
		return 43;
	case ABILITY_ID::TRAIN_CARRIER:
		return 64;
	case ABILITY_ID::TRAIN_TEMPEST:
		return 43;
	case ABILITY_ID::TRAIN_MOTHERSHIP:
		return 589;
	case ABILITY_ID::RESEARCH_WARPGATE:
		return 100;
	case ABILITY_ID::RESEARCH_BLINK:
		return 121;
	case ABILITY_ID::RESEARCH_CHARGE:
		return 100;
	case ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES:
		return 100;
	case ABILITY_ID::RESEARCH_PSISTORM:
		return 79;
	case ABILITY_ID::RESEARCH_SHADOWSTRIKE:
		return 100;
	case ABILITY_ID::RESEARCH_GRAVITICBOOSTER:
		return 57;
	case ABILITY_ID::RESEARCH_GRAVITICDRIVE:
		return 57;
	case ABILITY_ID::RESEARCH_EXTENDEDTHERMALLANCE:
		return 100;
	case ABILITY_ID::RESEARCH_PHOENIXANIONPULSECRYSTALS:
		return 64;
	case ABILITY_ID::RESEARCH_VOIDRAYSPEEDUPGRADE:
		return 57;
	case ABILITY_ID::RESEARCH_TEMPESTRESEARCHGROUNDATTACKUPGRADE:
		return 100;
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1:
		return 121;
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL2:
		return 145;
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL3:
		return 168;
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL1:
		return 121;
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL2:
		return 145;
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL3:
		return 168;
	case ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL1:
		return 121;
	case ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL2:
		return 145;
	case ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL3:
		return 168;
	case ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL1:
		return 129;
	case ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL2:
		return 154;
	case ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL3:
		return 179;
	case ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL1:
		return 129;
	case ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL2:
		return 154;
	case ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL3:
		return 179;
	case ABILITY_ID::BUILD_PYLON:
		return 18;
	case ABILITY_ID::BUILD_NEXUS:
		return 71;
	case ABILITY_ID::BUILD_GATEWAY:
		return 46;
	case ABILITY_ID::BUILD_FORGE:
		return 32;
	case ABILITY_ID::BUILD_CYBERNETICSCORE:
		return 36;
	case ABILITY_ID::BUILD_PHOTONCANNON:
		return 29;
	case ABILITY_ID::BUILD_SHIELDBATTERY:
		return 29;
	case ABILITY_ID::BUILD_TWILIGHTCOUNCIL:
		return 36;
	case ABILITY_ID::BUILD_STARGATE:
		return 43;
	case ABILITY_ID::BUILD_ROBOTICSFACILITY:
		return 46;
	case ABILITY_ID::BUILD_ROBOTICSBAY:
		return 46;
	case ABILITY_ID::BUILD_TEMPLARARCHIVE:
		return 36;
	case ABILITY_ID::BUILD_DARKSHRINE:
		return 71;
	case ABILITY_ID::BUILD_FLEETBEACON:
		return 43;
	case ABILITY_ID::BUILD_ASSIMILATOR:
		return 21;
	default:
		std::cerr << "Error unknown ability id in Utility::GetAbilityTime " << AbilityIdToString(ability) << std::endl;
		return 0;
	}
}

float Utility::GetOrderTimeLeft(UnitOrder order)
{
	return GetAbilityTime(order.ability_id.ToType()) * (1 - order.progress);
}

std::string Utility::AbilityIdToString(ABILITY_ID abilityId)
{
	switch (abilityId)
	{
	case ABILITY_ID::TRAIN_PROBE:
		return "train probe";
	case ABILITY_ID::TRAIN_ZEALOT:
		return "train zealot";
	case ABILITY_ID::TRAIN_ADEPT:
		return "train adept";
	case ABILITY_ID::TRAIN_STALKER:
		return "train stalker";
	case ABILITY_ID::TRAIN_SENTRY:
		return "train sentry";
	case ABILITY_ID::TRAIN_IMMORTAL:
		return "train immortal";
	case ABILITY_ID::TRAIN_OBSERVER:
		return "train observer";
	case ABILITY_ID::TRAIN_WARPPRISM:
		return "train warpprism";
	case ABILITY_ID::TRAIN_PHOENIX:
		return "train phoenix";
	case ABILITY_ID::TRAIN_ORACLE:
		return "train oracle";
	case ABILITY_ID::TRAIN_VOIDRAY:
		return "train void ray";
	case ABILITY_ID::TRAIN_CARRIER:
		return "train carrier";
	case ABILITY_ID::TRAIN_TEMPEST:
		return "train tempest";
	case ABILITY_ID::TRAIN_MOTHERSHIP:
		return "train mothership";
	case ABILITY_ID::RESEARCH_WARPGATE:
		return "research warpgate";
	case ABILITY_ID::RESEARCH_BLINK:
		return "research blink";
	case ABILITY_ID::RESEARCH_CHARGE:
		return "research charge";
	case ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES:
		return "research glaives";
	case A_RESEARCH_GROUND_WEAPONS:
		return "research ground attack";
	case A_RESEARCH_GROUND_ARMOR:
		return "research ground armor";
	case A_RESEARCH_SHIELDS:
		return "research plasma shields";
	case A_RESEARCH_AIR_WEAPONS:
		return "research air weapons";
	case A_RESEARCH_AIR_ARMOR:
		return "research air armor";
	case ABILITY_ID::MORPH_WARPGATE:
		return "morph into warpgate";
	case A_MOVE:
		return "general move";
	case A_RETURN_RESOURCE:
		return "harvest return";
	case A_GATHER_RESOURCE:
		return "harvest gather";
	case A_SMART:
		return "smart";
	case A_ATTACK:
		return "attack";
	case A_STOP:
		return "stop";
	case A_HOLD_POSITION:
		return "hold position";
	case ABILITY_ID::BUILD_PYLON:
		return "build pylon";
	case ABILITY_ID::BUILD_NEXUS:
		return "build nexus";
	case ABILITY_ID::BUILD_GATEWAY:
		return "build gateway";
	case ABILITY_ID::BUILD_FORGE:
		return "build forge";
	case ABILITY_ID::BUILD_CYBERNETICSCORE:
		return "build cybercore";
	case ABILITY_ID::BUILD_PHOTONCANNON:
		return "build cannon";
	case ABILITY_ID::BUILD_SHIELDBATTERY:
		return "build battery";
	case ABILITY_ID::BUILD_TWILIGHTCOUNCIL:
		return "build twilight";
	case ABILITY_ID::BUILD_STARGATE:
		return "build stargate";
	case ABILITY_ID::BUILD_ROBOTICSFACILITY:
		return "build robo";
	case ABILITY_ID::BUILD_ROBOTICSBAY:
		return "build robo bay";
	case ABILITY_ID::BUILD_TEMPLARARCHIVE:
		return "build templar archive";
	case ABILITY_ID::BUILD_DARKSHRINE:
		return "build dark shrine";
	case ABILITY_ID::BUILD_FLEETBEACON:
		return "build fleet beacon";
	case ABILITY_ID::BUILD_ASSIMILATOR:
		return "build assimilator";
	case A_CHRONO:
		return "chrono";
	//case A_ENERGY_RECHARGE:
	//	return "energy recharge";
	case A_ORACLE_BEAM_ON:
		return "pulsar beam on";
	case A_ORACLE_BEAM_OFF:
		return "pulsar beam off";
	default:
		return "Error invalid abilityId in AbilityIdToString";
	}
}

std::string Utility::OrdersToString(std::vector<UnitOrder> orders)
{
	std::string text = "";
	for (const auto& order : orders)
	{
		text += Utility::AbilityIdToString(order.ability_id);
		int percent = (int)std::floor(order.progress * 10);
		std::string completed(percent, '|');
		std::string todo(10 - percent, '-');
		text += " <" + completed + todo + "> ";
	}
	return text;
}


}