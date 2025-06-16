
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
	for (const auto& type : { UNIT_TYPEID::PROTOSS_PHOTONCANNON, /*UNIT_TYPEID::PROTOSS_PROBE,*/ UNIT_TYPEID::PROTOSS_ZEALOT, UNIT_TYPEID::PROTOSS_SENTRY,
		UNIT_TYPEID::PROTOSS_STALKER, UNIT_TYPEID::PROTOSS_ADEPT, UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, UNIT_TYPEID::PROTOSS_DARKTEMPLAR, UNIT_TYPEID::PROTOSS_ARCHON, PRISM, PRISM_SIEGED,
		UNIT_TYPEID::PROTOSS_IMMORTAL, UNIT_TYPEID::PROTOSS_COLOSSUS, UNIT_TYPEID::PROTOSS_PHOENIX, UNIT_TYPEID::PROTOSS_VOIDRAY, UNIT_TYPEID::PROTOSS_ORACLE,
		UNIT_TYPEID::PROTOSS_CARRIER, UNIT_TYPEID::PROTOSS_TEMPEST, UNIT_TYPEID::PROTOSS_MOTHERSHIP, UNIT_TYPEID::TERRAN_PLANETARYFORTRESS, UNIT_TYPEID::TERRAN_MISSILETURRET,
		/*UNIT_TYPEID::TERRAN_SCV,*/ UNIT_TYPEID::TERRAN_MARINE, UNIT_TYPEID::TERRAN_MARAUDER, UNIT_TYPEID::TERRAN_REAPER, UNIT_TYPEID::TERRAN_GHOST,
		UNIT_TYPEID::TERRAN_HELLION, UNIT_TYPEID::TERRAN_HELLIONTANK, UNIT_TYPEID::TERRAN_SIEGETANK, UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, UNIT_TYPEID::TERRAN_CYCLONE,
		UNIT_TYPEID::TERRAN_THOR, UNIT_TYPEID::TERRAN_THORAP, UNIT_TYPEID::TERRAN_AUTOTURRET, UNIT_TYPEID::TERRAN_VIKINGASSAULT, UNIT_TYPEID::TERRAN_VIKINGFIGHTER,
		UNIT_TYPEID::TERRAN_LIBERATOR, UNIT_TYPEID::TERRAN_LIBERATORAG, UNIT_TYPEID::TERRAN_BANSHEE, UNIT_TYPEID::TERRAN_BATTLECRUISER, UNIT_TYPEID::ZERG_SPINECRAWLER,
		UNIT_TYPEID::ZERG_SPORECRAWLER, /*UNIT_TYPEID::ZERG_DRONE,*/ UNIT_TYPEID::ZERG_QUEEN, UNIT_TYPEID::ZERG_ZERGLING, UNIT_TYPEID::ZERG_BANELING, UNIT_TYPEID::ZERG_ROACH,
		UNIT_TYPEID::ZERG_RAVAGER, UNIT_TYPEID::ZERG_HYDRALISK, UNIT_TYPEID::ZERG_LURKERMP, UNIT_TYPEID::ZERG_ULTRALISK, UNIT_TYPEID::ZERG_MUTALISK, UNIT_TYPEID::ZERG_CORRUPTOR,
		UNIT_TYPEID::ZERG_BROODLORD, UNIT_TYPEID::ZERG_LOCUSTMP, UNIT_TYPEID::ZERG_BROODLING })
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
	for (const auto& type : { UNIT_TYPEID::PROTOSS_PHOTONCANNON, UNIT_TYPEID::PROTOSS_PROBE, UNIT_TYPEID::PROTOSS_ZEALOT, UNIT_TYPEID::PROTOSS_SENTRY,
		UNIT_TYPEID::PROTOSS_STALKER, UNIT_TYPEID::PROTOSS_ADEPT, UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, UNIT_TYPEID::PROTOSS_DARKTEMPLAR, UNIT_TYPEID::PROTOSS_ARCHON, PRISM, PRISM_SIEGED,
		UNIT_TYPEID::PROTOSS_IMMORTAL, UNIT_TYPEID::PROTOSS_COLOSSUS, UNIT_TYPEID::PROTOSS_PHOENIX, UNIT_TYPEID::PROTOSS_VOIDRAY, UNIT_TYPEID::PROTOSS_ORACLE,
		UNIT_TYPEID::PROTOSS_CARRIER, UNIT_TYPEID::PROTOSS_TEMPEST, UNIT_TYPEID::PROTOSS_MOTHERSHIP, UNIT_TYPEID::TERRAN_PLANETARYFORTRESS, UNIT_TYPEID::TERRAN_MISSILETURRET,
		UNIT_TYPEID::TERRAN_SCV, UNIT_TYPEID::TERRAN_MARINE, UNIT_TYPEID::TERRAN_MARAUDER, UNIT_TYPEID::TERRAN_REAPER, UNIT_TYPEID::TERRAN_GHOST,
		UNIT_TYPEID::TERRAN_HELLION, UNIT_TYPEID::TERRAN_HELLIONTANK, UNIT_TYPEID::TERRAN_SIEGETANK, UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, UNIT_TYPEID::TERRAN_CYCLONE,
		UNIT_TYPEID::TERRAN_THOR, UNIT_TYPEID::TERRAN_THORAP, UNIT_TYPEID::TERRAN_AUTOTURRET, UNIT_TYPEID::TERRAN_VIKINGASSAULT, UNIT_TYPEID::TERRAN_VIKINGFIGHTER, MEDIVAC,
		UNIT_TYPEID::TERRAN_LIBERATOR, UNIT_TYPEID::TERRAN_LIBERATORAG, UNIT_TYPEID::TERRAN_BANSHEE, UNIT_TYPEID::TERRAN_BATTLECRUISER, UNIT_TYPEID::ZERG_SPINECRAWLER,
		UNIT_TYPEID::ZERG_SPORECRAWLER, UNIT_TYPEID::ZERG_DRONE, UNIT_TYPEID::ZERG_QUEEN, UNIT_TYPEID::ZERG_ZERGLING, UNIT_TYPEID::ZERG_BANELING, UNIT_TYPEID::ZERG_ROACH,
		UNIT_TYPEID::ZERG_RAVAGER, UNIT_TYPEID::ZERG_HYDRALISK, UNIT_TYPEID::ZERG_LURKERMP, UNIT_TYPEID::ZERG_ULTRALISK, UNIT_TYPEID::ZERG_MUTALISK, UNIT_TYPEID::ZERG_CORRUPTOR, // TODO overlord
		UNIT_TYPEID::ZERG_BROODLORD, UNIT_TYPEID::ZERG_LOCUSTMP, UNIT_TYPEID::ZERG_BROODLING })
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
	case UNIT_TYPEID::TERRAN_ARMORY:
	case UNIT_TYPEID::TERRAN_BARRACKS:
	case UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
	case UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
	case UNIT_TYPEID::TERRAN_BUNKER:
	case UNIT_TYPEID::TERRAN_COMMANDCENTER:
	case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
	case UNIT_TYPEID::TERRAN_FACTORY:
	case UNIT_TYPEID::TERRAN_FACTORYREACTOR:
	case UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
	case UNIT_TYPEID::TERRAN_FUSIONCORE:
	case UNIT_TYPEID::TERRAN_GHOSTACADEMY:
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
	case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
	case UNIT_TYPEID::TERRAN_REFINERY:
	case UNIT_TYPEID::TERRAN_SENSORTOWER:
	case UNIT_TYPEID::TERRAN_STARPORT:
	case UNIT_TYPEID::TERRAN_STARPORTREACTOR:
	case UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
	case UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
	case UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED:
	case UNIT_TYPEID::TERRAN_REACTOR:
	case UNIT_TYPEID::TERRAN_TECHLAB:
		// Zerg
	case UNIT_TYPEID::ZERG_BANELINGNEST:
	case UNIT_TYPEID::ZERG_CREEPTUMOR:
	case UNIT_TYPEID::ZERG_CREEPTUMORBURROWED:
	case UNIT_TYPEID::ZERG_CREEPTUMORQUEEN:
	case UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:
	case UNIT_TYPEID::ZERG_EXTRACTOR:
	case UNIT_TYPEID::ZERG_GREATERSPIRE:
	case UNIT_TYPEID::ZERG_HATCHERY:
	case UNIT_TYPEID::ZERG_HIVE:
	case UNIT_TYPEID::ZERG_HYDRALISKDEN:
	case UNIT_TYPEID::ZERG_INFESTATIONPIT:
	case UNIT_TYPEID::ZERG_LAIR:
	case UNIT_TYPEID::ZERG_LURKERDENMP:
	case UNIT_TYPEID::ZERG_NYDUSCANAL:
	case UNIT_TYPEID::ZERG_NYDUSNETWORK:
	case UNIT_TYPEID::ZERG_ROACHWARREN:
	case UNIT_TYPEID::ZERG_SPAWNINGPOOL:
	case UNIT_TYPEID::ZERG_SPINECRAWLER:
	case UNIT_TYPEID::ZERG_SPINECRAWLERUPROOTED:
	case UNIT_TYPEID::ZERG_SPIRE:
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
	case UNIT_TYPEID::ZERG_SPORECRAWLERUPROOTED:
	case UNIT_TYPEID::ZERG_ULTRALISKCAVERN:
		// Protoss
	case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR:
	case sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
	case sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE:
	case sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON:
	case sc2::UNIT_TYPEID::PROTOSS_FORGE:
	case sc2::UNIT_TYPEID::PROTOSS_GATEWAY:
	case sc2::UNIT_TYPEID::PROTOSS_NEXUS:
	case sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON:
	case sc2::UNIT_TYPEID::PROTOSS_PYLON:
	case sc2::UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED:
	case sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
	case sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
	case sc2::UNIT_TYPEID::PROTOSS_STARGATE:
	case sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
	case sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
	case sc2::UNIT_TYPEID::PROTOSS_WARPGATE:
	case sc2::UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
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
	theta = theta * PI / 180;
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
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		damage = 20;
		break;
	case UNIT_TYPEID::PROTOSS_PROBE:
		if (is_flying)
			return 0;
		damage = 5;
		break;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		if (is_flying)
			return 0;
		damage = 8 + (upgrade_level * 1);
		attacks = 2;
		break;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		damage = 6 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::PROTOSS_STALKER:
		damage = 13 + (upgrade_level * 1) + (is_armored * (5 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		if (is_flying)
			return 0;
		damage = 10 + (upgrade_level * 1) + (is_light * (12 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		if (is_flying)
			return 0;
		damage = 4 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		if (is_flying)
			return 0;
		damage = 45 + (upgrade_level * 5);
		break;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		damage = 25 + (upgrade_level * 3) + (is_biological * (10 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		if (is_flying)
			return 0;
		damage = 20 + (upgrade_level * 2) + (is_armored * (30 + (upgrade_level * 3)));
		break;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		if (is_flying)
			return 0;
		damage = 10 + (upgrade_level * 1) + (is_light * (5 + (upgrade_level * 1)));
		attacks = 2;
		break;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		if (!is_flying)
			return 0;
		damage = 5 + (upgrade_level * 1) + (is_light * 5);
		attacks = 2;
		break;
	case UNIT_TYPEID::PROTOSS_VOIDRAY: // TODO prismatic alignment
		damage = 6 + (upgrade_level * 1) + (is_armored * 4);
		break;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		if (is_flying)
			return 0;
		damage = 15 + (is_light * 7);
		break;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		damage = 5 + (upgrade_level * 1);
		attacks = 16;
		break;
	case UNIT_TYPEID::PROTOSS_TEMPEST: // TODO techtonic destabilizers
		damage = 40 + (upgrade_level * 4);
		break;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		damage = 6 + (upgrade_level * 1);
		attacks = 6;
		break;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: // terran
		if (is_flying)
			return 0;
		damage = 40;
		break;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		if (!is_flying)
			return 0;
		damage = 12;
		attacks = 2;
		break;
	case UNIT_TYPEID::TERRAN_SCV:
		if (is_flying)
			return 0;
		damage = 5;
		break;
	case UNIT_TYPEID::TERRAN_MARINE:
		damage = 6 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::TERRAN_MARAUDER:
		if (is_flying)
			return 0;
		damage = 10 + (upgrade_level * 1) + (is_armored * (10 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_REAPER:
		if (is_flying)
			return 0;
		damage = 4 + (upgrade_level * 1);
		attacks = 2;
		break;
	case UNIT_TYPEID::TERRAN_GHOST:
		damage = 10 + (upgrade_level * 1) + (is_light * (10 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_HELLION: // TODO blue flame
		if (is_flying)
			return 0;
		damage = 8 + (upgrade_level * 1) + (is_light * (6 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		if (is_flying)
			return 0;
		damage = 18 + (upgrade_level * 2) + (is_light * (0 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		if (is_flying)
			return 0;
		damage = 15 + (upgrade_level * 2) + (is_armored * (10 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		if (is_flying)
			return 0;
		damage = 40 + (upgrade_level * 4) + (is_armored * (30 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		damage = 18 + (upgrade_level * 2);
		break;
	case UNIT_TYPEID::TERRAN_THOR:
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
	case UNIT_TYPEID::TERRAN_THORAP:
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
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		damage = 18;
		break;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		if (is_flying)
			return 0;
		damage = 12 + (upgrade_level * 1) + (is_mechanical * (8 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		if (!is_flying)
			return 0;
		damage = 10 + (upgrade_level * 1) + (is_armored * 4);
		attacks = 2;
		break;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		if (!is_flying)
			return 0;
		damage = 5 + (upgrade_level * 1);
		attacks = 2;
		break;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		if (is_flying)
			return 0;
		damage = 75 + (upgrade_level * 5);
		break;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		if (is_flying)
			return 0;
		damage = 12 + (upgrade_level * 1);
		attacks = 2;
		break;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		if (is_flying)
		{
			damage = 5 + (upgrade_level * 1);
		}
		else
		{
			damage = 8 + (upgrade_level * 1);
		}
		break;
	case UNIT_TYPEID::ZERG_SPINECRAWLER: // zerg
		if (is_flying)
			return 0;
		damage = 25 + (is_armored * 5);
		break;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		if (!is_flying)
			return 0;
		damage = 15 + (is_biological * 15);
		break;
	case UNIT_TYPEID::ZERG_DRONE:
	case UNIT_TYPEID::ZERG_DRONEBURROWED:
		if (is_flying)
			return 0;
		damage = 5;
		break;
	case UNIT_TYPEID::ZERG_QUEEN:
	case UNIT_TYPEID::ZERG_QUEENBURROWED:
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
	case UNIT_TYPEID::ZERG_ZERGLING:
	case UNIT_TYPEID::ZERG_ZERGLINGBURROWED:
		if (is_flying)
			return 0;
		damage = 5 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::ZERG_BANELING:
	case UNIT_TYPEID::ZERG_BANELINGBURROWED:
		if (is_flying)
			return 0;
		damage = 16 + (upgrade_level * 2) + (is_light * (19 + (upgrade_level * 2)));
		break;
	case UNIT_TYPEID::ZERG_ROACH:
	case UNIT_TYPEID::ZERG_ROACHBURROWED:
		if (is_flying)
			return 0;
		damage = 16 + (upgrade_level * 2);
		break;
	case UNIT_TYPEID::ZERG_RAVAGER:
	case UNIT_TYPEID::RAVAGERBURROWED:
		if (is_flying)
			return 0;
		damage = 16 + (upgrade_level * 2);
		break;
	case UNIT_TYPEID::ZERG_HYDRALISK:
	case UNIT_TYPEID::ZERG_HYDRALISKBURROWED:
		damage = 12 + (upgrade_level * 2);
		break;
	case UNIT_TYPEID::ZERG_LURKERMP:
	case UNIT_TYPEID::ZERG_LURKERMPBURROWED:
		if (is_flying)
			return 0;
		damage = 20 + (upgrade_level * 2) + (is_armored * (10 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::ZERG_ULTRALISK:
	case UNIT_TYPEID::ZERG_ULTRALISKBURROWED:
		if (is_flying)
			return 0;
		damage = 35 + (upgrade_level * 3);
		break;
	case UNIT_TYPEID::ZERG_MUTALISK:
		damage = 9 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		if (!is_flying)
			return 0;
		damage = 14 + (upgrade_level * 1) + (is_massive * (6 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::ZERG_BROODLORD:
		if (is_flying)
			return 0;
		damage = 20 + (upgrade_level * 2);
		attacks = 2;
		break;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		if (is_flying)
			return 0;
		damage = 10 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::ZERG_BROODLING:
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
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		return FRAME_TIME;
	case UNIT_TYPEID::PROTOSS_PROBE:
		return 4.67f;
		break;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return 18.6f + (upgrade_level * 2.33f);
		break;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return 14.0f + (upgrade_level * 1.4f);
		break;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return 13.4f + (upgrade_level * 1.5f);
		break;
	case UNIT_TYPEID::PROTOSS_ADEPT: // TODO glaives
		return 13.65f + (upgrade_level * 1.24f);
		break;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return 3.2f + (upgrade_level * .8f);
		break;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return 37.2f + (upgrade_level * 4.13f);
		break;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return 28.0f + (upgrade_level * 3.2f);
		break;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return 48.1f + (upgrade_level * 4.82f);
		break;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return 28.0f + (upgrade_level * 3.74f);
		break;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return 25.4f + (upgrade_level * 2.5f);
		break;
	case UNIT_TYPEID::PROTOSS_VOIDRAY: // TODO prismatic alignment
		return 28.0f + (upgrade_level * 2.8f);
		break;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return 35.9f;
		break;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return 37.4f + (upgrade_level * 7.5f);
		break;
	case UNIT_TYPEID::PROTOSS_TEMPEST: // TODO techtonic destabilizers
		return 16.97f + (upgrade_level * 1.697f);
		break;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return 22.8f + (upgrade_level * 3.78f);
		break;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: // terran
		return 28.0f;
		break;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		return 39.3f;
		break;
	case UNIT_TYPEID::TERRAN_SCV:
		return 4.67f;
		break;
	case UNIT_TYPEID::TERRAN_MARINE: // TODO stim
		return 9.8f + (upgrade_level * 1.6f);
		break;
	case UNIT_TYPEID::TERRAN_MARAUDER:
		return 18.6f + (upgrade_level * 1.86f);
		break;
	case UNIT_TYPEID::TERRAN_REAPER:
		return 10.1f + (upgrade_level * 2.5f);
		break;
	case UNIT_TYPEID::TERRAN_GHOST:
		return 18.6f + (upgrade_level * 1.86f);
		break;
	case UNIT_TYPEID::TERRAN_HELLION: // TODO blue flame
		return 7.88f + (upgrade_level * 1.13f);
		break;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return 21.0f + (upgrade_level * 1.4f);
		break;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return 33.78f + (upgrade_level * 4.05f);
		break;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		return 32.71f + (upgrade_level * 2.34f);
		break;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return 24.14f + (upgrade_level * 1.72f);
		break;
	case UNIT_TYPEID::TERRAN_THOR:
		return 65.9f + (upgrade_level * 6.59f);
		break;
	case UNIT_TYPEID::TERRAN_THORAP:
		return 65.9f + (upgrade_level * 6.59f);
		break;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return 31.58f;
		break;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return 28.2f + (upgrade_level * 2.82f);
		break;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return 19.59f + (upgrade_level * 1.4f);
		break;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return 7.75f + (upgrade_level * 1.55f);
		break;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		return 65.8f + (upgrade_level * 4.39f);
		break;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		return 27.0f + (upgrade_level * 2.25f);
		break;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return 49.8f + (upgrade_level * 6.2f);
		break;
	case UNIT_TYPEID::ZERG_SPINECRAWLER: // zerg
		return 22.7f;
		break;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		return 24.4f;
		break;
	case UNIT_TYPEID::ZERG_DRONE:
		return 4.67f;
		break;
	case UNIT_TYPEID::ZERG_QUEEN:
		return 12.6f + (upgrade_level * 1.4f);
		break;
	case UNIT_TYPEID::ZERG_ZERGLING: // TODO adrenal
		return 10.0f + (upgrade_level * 2.0f);
		break;
	case UNIT_TYPEID::ZERG_BANELING:
		return 35.0f + (upgrade_level * 2.0f);
		break;
	case UNIT_TYPEID::ZERG_ROACH:
		return 11.2f + (upgrade_level * 1.4f);
		break;
	case UNIT_TYPEID::ZERG_RAVAGER:
		return 14.04f + (upgrade_level * 1.75f);
		break;
	case UNIT_TYPEID::ZERG_HYDRALISK:
		return 20.4f + (upgrade_level * 1.7f);
		break;
	case UNIT_TYPEID::ZERG_LURKERMP:
		return 21.0f + (upgrade_level * 2.1f);
		break;
	case UNIT_TYPEID::ZERG_ULTRALISK:
		return 57.38f + (upgrade_level * 4.9f);
		break;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return 11.93f + (upgrade_level * 1.33f);
		break;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		return 14.69f + (upgrade_level * 1.48f);
		break;
	case UNIT_TYPEID::ZERG_BROODLORD:
		return 22.4f + (upgrade_level * 2.2f);
		break;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return 23.25f + (upgrade_level * 2.33f);
		break;
	case UNIT_TYPEID::ZERG_BROODLING:
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
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		return damage / .89f;
	case UNIT_TYPEID::PROTOSS_PROBE:
		return damage / 1.07f;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return damage / .86f;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return damage / .71f;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return damage / 1.34f;
	case UNIT_TYPEID::PROTOSS_ADEPT: // TODO glaives
		return damage / 1.61f; // .64
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return damage / 1.25f;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return damage / 1.21f;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return damage / 1.25f;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return damage / 1.04f;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return damage / 1.07f;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return damage / .79f;
	case UNIT_TYPEID::PROTOSS_VOIDRAY: // TODO prismatic alignment
		return damage / .36f;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return damage / .61f;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return damage / 2.14f;
	case UNIT_TYPEID::PROTOSS_TEMPEST: // TODO techtonic destabilizers
		return damage / 2.36f;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return damage / 1.58f;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: // terran
		return damage / 1.43f;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		return damage / .61f;
	case UNIT_TYPEID::TERRAN_SCV:
		return damage / 1.07f;
	case UNIT_TYPEID::TERRAN_MARINE: // TODO stim
		return damage / .61f; // .41
	case UNIT_TYPEID::TERRAN_MARAUDER:
		return damage / 1.07f; // .71
	case UNIT_TYPEID::TERRAN_REAPER:
		return damage / .79f;
	case UNIT_TYPEID::TERRAN_GHOST:
		return damage / 1.07f;
	case UNIT_TYPEID::TERRAN_HELLION: // TODO blue flame
		return damage / 1.79f;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return damage / 1.43f;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return damage / .74f;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		return damage / 2.14f;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return damage / .71f;
	case UNIT_TYPEID::TERRAN_THOR:
		if (target->is_flying)
			return damage / 2.14f;
		else
			return damage / .91f;
	case UNIT_TYPEID::TERRAN_THORAP:
			return damage / .91f;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return damage / .57f;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return damage / .71f;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return damage / 1.43f;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return damage / 1.29f;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		return damage / 1.14f;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		return damage / .89f;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return damage / .16f;
	case UNIT_TYPEID::ZERG_SPINECRAWLER: // zerg
		return damage / 1.32f;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		return damage / .61f;
	case UNIT_TYPEID::ZERG_DRONE:
	case UNIT_TYPEID::ZERG_DRONEBURROWED:
		return damage / 1.07f;
	case UNIT_TYPEID::ZERG_QUEEN:
	case UNIT_TYPEID::ZERG_QUEENBURROWED:
		return damage / .71f;
	case UNIT_TYPEID::ZERG_ZERGLING: // TODO adrenal
	case UNIT_TYPEID::ZERG_ZERGLINGBURROWED:
		return damage / .5f; // .35
	case UNIT_TYPEID::ZERG_BANELING:
	case UNIT_TYPEID::ZERG_BANELINGBURROWED:
		return damage;
	case UNIT_TYPEID::ZERG_ROACH:
	case UNIT_TYPEID::ZERG_ROACHBURROWED:
		return damage / 1.43f;
	case UNIT_TYPEID::ZERG_RAVAGER:
	case UNIT_TYPEID::RAVAGERBURROWED:
		return damage / 1.14f;
	case UNIT_TYPEID::ZERG_HYDRALISK:
	case UNIT_TYPEID::ZERG_HYDRALISKBURROWED:
		return damage / .59f;
	case UNIT_TYPEID::ZERG_LURKERMP:
	case UNIT_TYPEID::ZERG_LURKERMPBURROWED:
		return damage / 1.43f;
	case UNIT_TYPEID::ZERG_ULTRALISK:
	case UNIT_TYPEID::ZERG_ULTRALISKBURROWED:
		return damage / .61f;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return damage / 1.09f;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		return damage / 1.36f;
	case UNIT_TYPEID::ZERG_BROODLORD:
		return damage / 1.79f;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return damage / .43f;
	case UNIT_TYPEID::ZERG_BROODLING:
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
	case UNIT_TYPEID::PROTOSS_PYLON: // protoss buildings
		break;
	case UNIT_TYPEID::PROTOSS_NEXUS:
		break;
	case UNIT_TYPEID::PROTOSS_GATEWAY:
		break;
	case UNIT_TYPEID::PROTOSS_WARPGATE:
		break;
	case UNIT_TYPEID::PROTOSS_FORGE:
		break;
	case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
		break;
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
		break;
	case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
		break;
	case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
		break;
	case UNIT_TYPEID::PROTOSS_STARGATE:
		break;
	case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
		break;
	case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
		break;
	case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
		break;
	case UNIT_TYPEID::PROTOSS_DARKSHRINE:
		break;
	case UNIT_TYPEID::PROTOSS_FLEETBEACON:
		break;
	case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
		break;
	case UNIT_TYPEID::PROTOSS_PROBE: // protoss units
		break;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		base_armor = 1;
		break;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		base_armor = 1;
		break;
	case UNIT_TYPEID::PROTOSS_STALKER:
		base_armor = 1;
		break;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		base_armor = 1;
		break;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		break;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		base_armor = 1;
		break;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		base_armor = 1;
		break;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		base_armor = 1;
		break;
	case UNIT_TYPEID::PROTOSS_DISRUPTOR:
		base_armor = 1;
		break;
	case UNIT_TYPEID::PROTOSS_OBSERVER:
		break;
	case UNIT_TYPEID::PROTOSS_WARPPRISM:
		break;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		break;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		break;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		break;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		base_armor = 2;
		break;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		base_armor = 2;
		break;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		base_armor = 2;
		break;
	case UNIT_TYPEID::TERRAN_COMMANDCENTER: // terran buildings
		break;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
		break;
	case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
		break;
	case UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
		break;
	case UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED:
		break;
	case UNIT_TYPEID::TERRAN_REFINERY:
		break;
	case UNIT_TYPEID::TERRAN_BARRACKS:
		break;
	case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
		break;
	case UNIT_TYPEID::TERRAN_BUNKER:
		break;
	case UNIT_TYPEID::TERRAN_SENSORTOWER:
		break;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		break;
	case UNIT_TYPEID::TERRAN_FACTORY:
		break;
	case UNIT_TYPEID::TERRAN_GHOSTACADEMY:
		break;
	case UNIT_TYPEID::TERRAN_STARPORT:
		break;
	case UNIT_TYPEID::TERRAN_ARMORY:
		break;
	case UNIT_TYPEID::TERRAN_FUSIONCORE:
		break;
	case UNIT_TYPEID::TERRAN_TECHLAB:
		break;
	case UNIT_TYPEID::TERRAN_REACTOR:
		break;
	case UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
		break;
	case UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
		break;
	case UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
		break;
	case UNIT_TYPEID::TERRAN_FACTORYREACTOR:
		break;
	case UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
		break;
	case UNIT_TYPEID::TERRAN_STARPORTREACTOR:
		break;
	case UNIT_TYPEID::TERRAN_SCV:
		break;
	case UNIT_TYPEID::TERRAN_MULE:
		break;
	case UNIT_TYPEID::TERRAN_MARINE: // terran units
		break;
	case UNIT_TYPEID::TERRAN_MARAUDER:
		base_armor = 1;
		break;
	case UNIT_TYPEID::TERRAN_REAPER:
		break;
	case UNIT_TYPEID::TERRAN_GHOST:
		break;
	case UNIT_TYPEID::TERRAN_HELLION:
		break;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		break;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		base_armor = 1;
		break;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		base_armor = 1;
		break;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		base_armor = 1;
		break;
	case UNIT_TYPEID::TERRAN_WIDOWMINE:
		break;
	case UNIT_TYPEID::TERRAN_THOR:
		base_armor = 1;
		break;
	case UNIT_TYPEID::TERRAN_THORAP:
		base_armor = 1;
		break;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		break;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		break;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		break;
	case UNIT_TYPEID::TERRAN_MEDIVAC:
		base_armor = 1;
		break;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		break;
	case UNIT_TYPEID::TERRAN_RAVEN:
		base_armor = 1;
		break;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		break;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		base_armor = 3;
		break;
	case UNIT_TYPEID::ZERG_HATCHERY: // zerg buildings
		break;
	case UNIT_TYPEID::ZERG_LAIR:
		break;
	case UNIT_TYPEID::ZERG_HIVE:
		break;
	case UNIT_TYPEID::ZERG_SPINECRAWLER:
		break;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		break;
	case UNIT_TYPEID::ZERG_EXTRACTOR:
		break;
	case UNIT_TYPEID::ZERG_SPAWNINGPOOL:
		break;
	case UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:
		break;
	case UNIT_TYPEID::ZERG_ROACHWARREN:
		break;
	case UNIT_TYPEID::ZERG_BANELINGNEST:
		break;
	case UNIT_TYPEID::ZERG_HYDRALISKDEN:
		break;
	case UNIT_TYPEID::ZERG_LURKERDENMP:
		break;
	case UNIT_TYPEID::ZERG_INFESTATIONPIT:
		break;
	case UNIT_TYPEID::ZERG_SPIRE:
		break;
	case UNIT_TYPEID::ZERG_GREATERSPIRE:
		break;
	case UNIT_TYPEID::ZERG_NYDUSNETWORK:
		break;
	case UNIT_TYPEID::ZERG_ULTRALISKCAVERN:
		break;
	case UNIT_TYPEID::ZERG_LARVA: // zerg units
		base_armor = 10;
		break;
	case UNIT_TYPEID::ZERG_EGG:
		base_armor = 10;
		break;
	case UNIT_TYPEID::ZERG_DRONE:
		break;
	case UNIT_TYPEID::ZERG_QUEEN:
		base_armor = 1;
		break;
	case UNIT_TYPEID::ZERG_ZERGLING:
		break;
	case UNIT_TYPEID::ZERG_BANELING:
		break;
	case UNIT_TYPEID::ZERG_ROACH:
		base_armor = 1;
		break;
	case UNIT_TYPEID::ZERG_RAVAGER:
		base_armor = 1;
		break;
	case UNIT_TYPEID::ZERG_HYDRALISK:
		break;
	case UNIT_TYPEID::ZERG_LURKERMP:
		base_armor = 1;
		break;
	case UNIT_TYPEID::ZERG_INFESTOR:
		break;
	case UNIT_TYPEID::ZERG_SWARMHOSTMP:
		base_armor = 1;
		break;
	case UNIT_TYPEID::ZERG_ULTRALISK: // TODO chitinous
		base_armor = 1;
		break;
	case UNIT_TYPEID::ZERG_OVERLORD:
		break;
	case UNIT_TYPEID::ZERG_OVERSEER:
		base_armor = 1;
		break;
	case UNIT_TYPEID::ZERG_MUTALISK:
		break;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		base_armor = 2;
		break;
	case UNIT_TYPEID::ZERG_BROODLORD:
		base_armor = 1;
		break;
	case UNIT_TYPEID::ZERG_VIPER:
		base_armor = 1;
		break;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		break;
	case UNIT_TYPEID::ZERG_BROODLING:
		break;
	case UNIT_TYPEID::ZERG_TRANSPORTOVERLORDCOCOON:
		base_armor = 2;
		break;
	case UNIT_TYPEID::ZERG_BANELINGCOCOON:
		base_armor = 2;
		break;
	case UNIT_TYPEID::ZERG_BROODLORDCOCOON:
		base_armor = 2;
		break;
	case UNIT_TYPEID::ZERG_OVERLORDCOCOON:
		base_armor = 2;
		break;
	case UNIT_TYPEID::ZERG_RAVAGERCOCOON:
		base_armor = 5;
		break;
	case UNIT_TYPEID::ZERG_LURKERMPEGG:
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
	case UNIT_TYPEID::PROTOSS_PROBE:
		return 1;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return 2;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return 2;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return 2;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return 2;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return 2;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return 2;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return 4;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return 4;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return 8;
	case UNIT_TYPEID::PROTOSS_DISRUPTOR:
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
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		return 7.0f;
	case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
		return 6.0f;
	case UNIT_TYPEID::PROTOSS_PROBE:
		return .1f;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return .1f;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return 5.0f;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return 6.0f;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return 4.0f;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return 6.0f;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return .1f;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return 3.0f;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return 6.0f;
	case UNIT_TYPEID::PROTOSS_COLOSSUS: // TODO extended thermal lance
		return 7.0f;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return 6.0f;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return 4.0f;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return 14.0f;
	case UNIT_TYPEID::PROTOSS_TEMPEST: // air 14
		return 10.0f;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return 7.0f;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: // terran hi sec auto tracking
		return 6.0f;
	case UNIT_TYPEID::TERRAN_SCV:
		return .1f;
	case UNIT_TYPEID::TERRAN_MARINE:
		return 5.0f;
	case UNIT_TYPEID::TERRAN_MARAUDER:
		return 6.0f;
	case UNIT_TYPEID::TERRAN_REAPER:
		return 5.0f;
	case UNIT_TYPEID::TERRAN_GHOST:
		return 6.0f;
	case UNIT_TYPEID::TERRAN_HELLION:
		return 5.0f;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return 2.0f;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return 7.0f;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		return 13.0f;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return 5.0f;
	case UNIT_TYPEID::TERRAN_THOR: // flying 10
		return 7.0f;
	case UNIT_TYPEID::TERRAN_THORAP: // flying 11
		return 7.0f;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return 6.0f;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return 9.0f;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		return 6.0f;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return 6.0f;
	case UNIT_TYPEID::ZERG_SPINECRAWLER: // zerg
		return 7.0f;
	case UNIT_TYPEID::ZERG_DRONE:
		return .1f;
	case UNIT_TYPEID::ZERG_QUEEN: // flying 7
		return 5.0f;
	case UNIT_TYPEID::ZERG_ZERGLING:
		return .1f;
	case UNIT_TYPEID::ZERG_BANELING:
		return 2.5f;
	case UNIT_TYPEID::ZERG_ROACH:
		return 4.0f;
	case UNIT_TYPEID::ZERG_RAVAGER:
		return 6.0f;
	case UNIT_TYPEID::ZERG_HYDRALISK: // grooved spines
		return 6.0f;
	case UNIT_TYPEID::ZERG_LURKERMP: // seismic spine
		return 8.0f;
	case UNIT_TYPEID::ZERG_ULTRALISK:
		return .1f;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return 3.0f;
	case UNIT_TYPEID::ZERG_BROODLORD:
		return 10.0f;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return 3.0f;
	case UNIT_TYPEID::ZERG_BROODLING:
		return 0.0f;
	case UNIT_TYPEID::ZERG_LARVA:
		return 0.0f;
	case UNIT_TYPEID::ZERG_OVERLORD:
		return 0.0f;
	case UNIT_TYPEID::ZERG_EGG:
		return 0.0f;
	case UNIT_TYPEID::ZERG_INFESTOR:
		return 0.0f;
	case UNIT_TYPEID::ZERG_INFESTORBURROWED:
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
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		return 7.0f;
	case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
		return 6.0f;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return 5.0f;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return 6.0f;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return 3.0f;
	case UNIT_TYPEID::PROTOSS_PHOENIX: // TODO anion pulse crystals
		return 5.0f;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return 6.0f;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return 14.0f;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return 14.0f;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return 7.0f;
	case UNIT_TYPEID::TERRAN_MISSILETURRET: // terran
		return 6.0f;
	case UNIT_TYPEID::TERRAN_MARINE:
		return 5.0f;
	case UNIT_TYPEID::TERRAN_GHOST:
		return 6.0f;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return 5.0f;
	case UNIT_TYPEID::TERRAN_THOR:
		return 10.0f;
	case UNIT_TYPEID::TERRAN_THORAP:
		return 11.0f;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return 6.0f;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return 6.0f;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return 5.0f;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return 6.0f;
	case UNIT_TYPEID::ZERG_SPORECRAWLER: // zerg
		return 7.0f;
	case UNIT_TYPEID::ZERG_QUEEN:
		return 7.0f;
	case UNIT_TYPEID::ZERG_HYDRALISK: // grooved spines
		return 6.0f;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return 3.0f;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
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
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		return .1193f;
	case UNIT_TYPEID::PROTOSS_PROBE:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return .2579f;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return .0594f;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return .1193f;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
		return .1193f;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		return .1193f;
	case UNIT_TYPEID::TERRAN_SCV:
		return .1193f;
	case UNIT_TYPEID::TERRAN_MARINE:
		return .0357f;
	case UNIT_TYPEID::TERRAN_MARAUDER:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_REAPER:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_GHOST:
		return .0593f;
	case UNIT_TYPEID::TERRAN_HELLION:
		return .1786f;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return .1193f;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return .1193f;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		return .1193f;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return .1193f;
	case UNIT_TYPEID::TERRAN_THOR:
		return .5936f;
	case UNIT_TYPEID::TERRAN_THORAP:
		return .5936f;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return .1193f;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return .1193f;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return .0357f;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return .1193f;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		return .0893f;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		return .1193f;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return .1193f;
	case UNIT_TYPEID::ZERG_SPINECRAWLER:
		return .238f;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		return .1193f;
	case UNIT_TYPEID::ZERG_DRONE:
		return .1193f;
	case UNIT_TYPEID::ZERG_QUEEN:
		return .1193f;
	case UNIT_TYPEID::ZERG_ZERGLING:
		return .1193f;
	case UNIT_TYPEID::ZERG_BANELING:
		return 0.0f;
	case UNIT_TYPEID::ZERG_ROACH:
		return .1193f;
	case UNIT_TYPEID::ZERG_RAVAGER:
		return .1429f;
	case UNIT_TYPEID::ZERG_HYDRALISK:
		return .1f;
	case UNIT_TYPEID::ZERG_LURKERMP:
		return 0.0f;
	case UNIT_TYPEID::ZERG_ULTRALISK:
		return .238f;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return 0.0f;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		return .0446f;
	case UNIT_TYPEID::ZERG_BROODLORD:
		return .1193f;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return .1904f;
	case UNIT_TYPEID::ZERG_BROODLING:
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
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		return dist;
	case UNIT_TYPEID::PROTOSS_PROBE:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return dist;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return dist;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return dist;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return dist;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return 0.0f;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return dist;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		return dist;
	case UNIT_TYPEID::TERRAN_SCV:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_MARINE:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_MARAUDER:
		return dist;
	case UNIT_TYPEID::TERRAN_REAPER:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_GHOST:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_HELLION:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return dist;
	case UNIT_TYPEID::TERRAN_THOR:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_THORAP:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return dist;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return dist;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		return 0.0f;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		return dist;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return dist;
	case UNIT_TYPEID::ZERG_SPINECRAWLER:
		return dist;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		return dist;
	case UNIT_TYPEID::ZERG_DRONE:
		return 0.0f;
	case UNIT_TYPEID::ZERG_QUEEN:
		return dist;
	case UNIT_TYPEID::ZERG_ZERGLING:
		return 0.0f;
	case UNIT_TYPEID::ZERG_BANELING:
		return 0.0f;
	case UNIT_TYPEID::ZERG_ROACH:
		return dist;
	case UNIT_TYPEID::ZERG_RAVAGER:
		return dist;
	case UNIT_TYPEID::ZERG_HYDRALISK:
		return dist;
	case UNIT_TYPEID::ZERG_LURKERMP:
		return 0.0f;
	case UNIT_TYPEID::ZERG_ULTRALISK:
		return 0.0f;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return dist;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		return dist;
	case UNIT_TYPEID::ZERG_BROODLORD:
		return dist;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return dist;
	case UNIT_TYPEID::ZERG_BROODLING:
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
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		return .89f;
	case UNIT_TYPEID::PROTOSS_PROBE:
		return 1.07f;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return .86f;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return .71f;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return 1.34f;
	case UNIT_TYPEID::PROTOSS_ADEPT: // TODO glaives
		return 1.61f;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return 1.25f;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return 1.21f;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return 1.25f;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return 1.04f;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return 1.07f;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return .79f;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return .36f;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return .61f;
	case UNIT_TYPEID::PROTOSS_CARRIER: // TODO each interceptor?
		return 2.14f;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return 2.36f;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return 1.58f;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
		return 1.43f;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		return .61f;
	case UNIT_TYPEID::TERRAN_SCV:
		return 1.07f;
	case UNIT_TYPEID::TERRAN_MARINE: // TODO stim
		return .61f;
	case UNIT_TYPEID::TERRAN_MARAUDER: // TODO stim
		return 1.07f;
	case UNIT_TYPEID::TERRAN_REAPER:
		return .79f;
	case UNIT_TYPEID::TERRAN_GHOST:
		return 1.07f;
	case UNIT_TYPEID::TERRAN_HELLION:
		return 1.79f;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return 1.43f;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return .79f;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		return 2.14f;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return .71f;
	case UNIT_TYPEID::TERRAN_THOR: // TODO anit air
		return .91f;
	case UNIT_TYPEID::TERRAN_THORAP:
		return .91f;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return .57f;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return .71f;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return 1.43f;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return 1.29f;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		return 1.14f;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		return .89f;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return .16f;
	case UNIT_TYPEID::ZERG_SPINECRAWLER:
		return 1.32f;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		return .61f;
	case UNIT_TYPEID::ZERG_DRONE:
		return 1.07f;
	case UNIT_TYPEID::ZERG_QUEEN:
		return .71f;
	case UNIT_TYPEID::ZERG_ZERGLING: //TODO adrenal
		return .497f;
	case UNIT_TYPEID::ZERG_BANELING:
		return 0.0f;
	case UNIT_TYPEID::ZERG_ROACH:
		return 1.43f;
	case UNIT_TYPEID::ZERG_RAVAGER:
		return 1.43f;
	case UNIT_TYPEID::ZERG_HYDRALISK:
		return .59f;
	case UNIT_TYPEID::ZERG_LURKERMP:
		return 1.43f;
	case UNIT_TYPEID::ZERG_ULTRALISK:
		return .61f;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return 1.09f;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		return 1.36f;
	case UNIT_TYPEID::ZERG_BROODLORD:
		return 1.79f;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return .43f;
	case UNIT_TYPEID::ZERG_BROODLING:
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
	case UNIT_TYPEID::PROTOSS_PROBE:
		return 3.94f;
	case UNIT_TYPEID::PROTOSS_ZEALOT: // TODO charge
		return 3.15f; // 4.72
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return 3.15f;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return 4.13f;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return 3.5f;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return 2.62f;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return 3.94f;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return 3.94f;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return 3.15f;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return 3.15f;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return 5.95f;
	case UNIT_TYPEID::PROTOSS_VOIDRAY: // TODO flux vanes
		return 3.85f; // 4.65
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return 5.6f;
	case UNIT_TYPEID::PROTOSS_CARRIER: // TODO each interceptor?
		return 2.62f;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return 3.15f;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return 2.62f;
	case UNIT_TYPEID::TERRAN_SCV:
		return 3.94f;
	case UNIT_TYPEID::TERRAN_MULE:
		return 3.94f;
	case UNIT_TYPEID::TERRAN_MARINE: // TODO stim
		return 3.15f; // 4.72
	case UNIT_TYPEID::TERRAN_MARAUDER: // TODO stim
		return 3.15f; // 4.72
	case UNIT_TYPEID::TERRAN_REAPER:
		return 5.25f;
	case UNIT_TYPEID::TERRAN_GHOST:
		return 3.94f;
	case UNIT_TYPEID::TERRAN_HELLION:
		return 5.95f;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return 3.15f;
	case UNIT_TYPEID::TERRAN_WIDOWMINE:
		return 3.94f;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return 3.15f;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return 4.72f;
	case UNIT_TYPEID::TERRAN_THOR:
	case UNIT_TYPEID::TERRAN_THORAP:
		return 2.62f;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return 3.15f;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return 3.85f;
	case UNIT_TYPEID::TERRAN_MEDIVAC: // TODO boost
		return 3.5f; // 5.94
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return 4.72f;
	case UNIT_TYPEID::TERRAN_BANSHEE: // TODO hyperflight rotors
		return 3.85f; // 5.25
	case UNIT_TYPEID::TERRAN_RAVEN:
		return 4.13f;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return 2.62f;
	case UNIT_TYPEID::ZERG_DRONE: // TODO all zerg units off creep
		return 3.94f;
	case UNIT_TYPEID::ZERG_OVERLORD: // TODO speed
	case UNIT_TYPEID::ZERG_OVERLORDTRANSPORT:
		return .902f;
	case UNIT_TYPEID::ZERG_OVERSEER:
		return 2.62f;
	case UNIT_TYPEID::ZERG_QUEEN:
		return 3.5f;
	case UNIT_TYPEID::ZERG_ZERGLING: // TODO ling speed
		return 5.37f;
	case UNIT_TYPEID::ZERG_BANELING: // TODO bane speed
		return 4.55f;
	case UNIT_TYPEID::ZERG_ROACH: // TODO speed
		return 4.09f; // 4.09
	case UNIT_TYPEID::ZERG_ROACHBURROWED: // TODO speed
		return 2.56f; // 4.09
	case UNIT_TYPEID::ZERG_RAVAGER:
		return 5.0f;
	case UNIT_TYPEID::ZERG_HYDRALISK: // TODO speed
		return 4.09f;
	case UNIT_TYPEID::ZERG_LURKERMP:
		return 5.37f;
	case UNIT_TYPEID::ZERG_INFESTOR:
		return 4.09f;
	case UNIT_TYPEID::ZERG_INFESTORBURROWED:
		return 3.64f;
	case UNIT_TYPEID::ZERG_ULTRALISK: // TODO speed
		return 5.37f;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return 5.6f;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		return 4.73f;
	case UNIT_TYPEID::ZERG_BROODLORD:
		return 1.97f;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return 3.68f;
	case UNIT_TYPEID::ZERG_BROODLING:
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
	float build_time = 0;
	switch (unit->unit_type.ToType())
	{
	case UNIT_TYPEID::ZERG_HATCHERY:
		build_time = 71.0f;
		break;
	case UNIT_TYPEID::ZERG_EXTRACTOR:
		build_time = 21.0f;
		break;
	case UNIT_TYPEID::ZERG_SPAWNINGPOOL:
		build_time = 46.0f;
		break;
	case UNIT_TYPEID::ZERG_ROACHWARREN:
		build_time = 39.0f;
		break;
	case UNIT_TYPEID::TERRAN_COMMANDCENTER:
		build_time = 71.0f;
		break;
	case UNIT_TYPEID::TERRAN_REFINERY:
		build_time = 30.0f;
		break;
	case UNIT_TYPEID::TERRAN_BARRACKS:
		build_time = 46.0f;
		break;
	case UNIT_TYPEID::TERRAN_FACTORY:
		build_time = 43.0f;
		break;
	default:
		//std::cout << "Error Unknown building in GetTimeBuilt\n";
		break;
	}
	return curr_time - (build_time * unit->build_progress);
}

AbilityID Utility::UnitToWarpInAbility(UNIT_TYPEID type)
{
	switch (type)
	{
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return ABILITY_ID::TRAINWARP_ZEALOT;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return ABILITY_ID::TRAINWARP_STALKER;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return ABILITY_ID::TRAINWARP_ADEPT;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return ABILITY_ID::TRAINWARP_SENTRY;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return ABILITY_ID::TRAINWARP_HIGHTEMPLAR;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
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

int Utility::GetTrainingTime(UNIT_TYPEID type)
{
	switch (type)
	{
	case UNIT_TYPEID::PROTOSS_PROBE:
		return 12;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return 27;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return 23;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return 27;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return 30;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return 39;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return 39;
	case UNIT_TYPEID::PROTOSS_OBSERVER:
		return 18;
	case UNIT_TYPEID::PROTOSS_WARPPRISM:
		return 36;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return 39;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return 54;
	case UNIT_TYPEID::PROTOSS_DISRUPTOR:
		return 36;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return 25;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return 43;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return 37;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return 64;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return 43;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
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
#ifdef DEBUG_TIMING
	std::ofstream amaing_at_time;
	amaing_at_time.open("amaing_at_time.txt", std::ios_base::app);

	unsigned long long distance_check_total = 0;
	unsigned long long angle_check_total = 0;
	unsigned long long total = 0;
#endif

	float smallest_angle = 180;
	const Unit* target = nullptr;


	for (const auto Funit : allied_units)
	{ // try flipped
#ifdef DEBUG_TIMING
		unsigned long long unit_start = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
#endif

		if (Distance2D(unit->pos, Funit->pos) >= RealRange(unit, Funit))
		{
#ifdef DEBUG_TIMING
			/*unsigned long long dist_check = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				).count();
			distance_check_total += dist_check - unit_start;*/
#endif
			continue;
		}
#ifdef DEBUG_TIMING
		unsigned long long dist_check = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
#endif
		float angle = GetFacingAngle(unit, Funit);

		if (angle < smallest_angle)
		{
			smallest_angle = angle;
			target = Funit;
		}
#ifdef DEBUG_TIMING
		unsigned long long angle_check = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();

		distance_check_total += dist_check - unit_start;
		angle_check_total += angle_check - dist_check;
#endif
	}

#ifdef DEBUG_TIMING
	amaing_at_time << distance_check_total << ", ";
	amaing_at_time << angle_check_total << "\n";
	amaing_at_time.close();
#endif

	return target;
}

float Utility::BuildingSize(UNIT_TYPEID buildingId)
{
	if (buildingId == UNIT_TYPEID::PROTOSS_NEXUS)
		return 2.5;
	if (buildingId == UNIT_TYPEID::PROTOSS_PYLON || buildingId == UNIT_TYPEID::PROTOSS_SHIELDBATTERY || buildingId == UNIT_TYPEID::PROTOSS_PHOTONCANNON)
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
	case UNIT_TYPEID::PROTOSS_PYLON:
		return ABILITY_ID::BUILD_PYLON;
	case UNIT_TYPEID::PROTOSS_NEXUS:
		return ABILITY_ID::BUILD_NEXUS;
	case UNIT_TYPEID::PROTOSS_GATEWAY:
		return ABILITY_ID::BUILD_GATEWAY;
	case UNIT_TYPEID::PROTOSS_FORGE:
		return ABILITY_ID::BUILD_FORGE;
	case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
		return ABILITY_ID::BUILD_CYBERNETICSCORE;
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
		return ABILITY_ID::BUILD_PHOTONCANNON;
	case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
		return ABILITY_ID::BUILD_SHIELDBATTERY;
	case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
		return ABILITY_ID::BUILD_TWILIGHTCOUNCIL;
	case UNIT_TYPEID::PROTOSS_STARGATE:
		return ABILITY_ID::BUILD_STARGATE;
	case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
		return ABILITY_ID::BUILD_ROBOTICSFACILITY;
	case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
		return ABILITY_ID::BUILD_ROBOTICSBAY;
	case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
		return ABILITY_ID::BUILD_TEMPLARARCHIVE;
	case UNIT_TYPEID::PROTOSS_DARKSHRINE:
		return ABILITY_ID::BUILD_DARKSHRINE;
	case UNIT_TYPEID::PROTOSS_FLEETBEACON:
		return ABILITY_ID::BUILD_FLEETBEACON;
	case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
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
	case UNIT_TYPEID::PROTOSS_PROBE:
		return ABILITY_ID::TRAIN_PROBE;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return ABILITY_ID::TRAIN_ZEALOT;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return ABILITY_ID::TRAIN_ADEPT;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return ABILITY_ID::TRAIN_STALKER;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return ABILITY_ID::TRAIN_SENTRY;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return ABILITY_ID::TRAIN_HIGHTEMPLAR;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return ABILITY_ID::TRAIN_DARKTEMPLAR;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return ABILITY_ID::TRAIN_IMMORTAL;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return ABILITY_ID::TRAIN_COLOSSUS;
	case UNIT_TYPEID::PROTOSS_DISRUPTOR:
		return ABILITY_ID::TRAIN_DISRUPTOR;
	case UNIT_TYPEID::PROTOSS_OBSERVER:
		return ABILITY_ID::TRAIN_OBSERVER;
	case UNIT_TYPEID::PROTOSS_WARPPRISM:
		return ABILITY_ID::TRAIN_WARPPRISM;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return ABILITY_ID::TRAIN_PHOENIX;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return ABILITY_ID::TRAIN_VOIDRAY;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return ABILITY_ID::TRAIN_ORACLE;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return ABILITY_ID::TRAIN_CARRIER;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return ABILITY_ID::TRAIN_TEMPEST;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
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
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return ABILITY_ID::TRAINWARP_ZEALOT;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return ABILITY_ID::TRAINWARP_ADEPT;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return ABILITY_ID::TRAINWARP_STALKER;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return ABILITY_ID::TRAINWARP_SENTRY;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return ABILITY_ID::TRAINWARP_HIGHTEMPLAR;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
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
		return ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:
		return ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL1:
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL2:
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL3:
		return ABILITY_ID::RESEARCH_PROTOSSSHIELDS;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL1:
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL2:
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL3:
		return ABILITY_ID::RESEARCH_PROTOSSAIRARMOR;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1:
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2:
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3:
		return ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONS;
	default:
		std::cout << "Error invalid unit id in GetUpgradeAbility " << UpgradeIDToName(upgrade_id) << std::endl;
		return ABILITY_ID::BUILD_CANCEL;
	}
}

int Utility::GetWarpCooldown(UNIT_TYPEID unitId)
{
	switch (unitId)
	{
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return 20;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return 20;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return 23;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return 23;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return 32;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
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
	case UNIT_TYPEID::PROTOSS_PYLON:
		return UnitCost(100, 0, 0);
	case UNIT_TYPEID::PROTOSS_NEXUS:
		return UnitCost(400, 0, 0);
	case UNIT_TYPEID::PROTOSS_GATEWAY:
		return UnitCost(150, 0, 0);
	case UNIT_TYPEID::PROTOSS_FORGE:
		return UnitCost(150, 0, 0);
	case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
		return UnitCost(150, 0, 0);
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
		return UnitCost(150, 0, 0);
	case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
		return UnitCost(100, 0, 0);
	case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
		return UnitCost(150, 100, 0);
	case UNIT_TYPEID::PROTOSS_STARGATE:
		return UnitCost(150, 150, 0);
	case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
		return UnitCost(150, 100, 0);
	case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
		return UnitCost(150, 150, 0);
	case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
		return UnitCost(150, 200, 0);
	case UNIT_TYPEID::PROTOSS_DARKSHRINE:
		return UnitCost(150, 150, 0);
	case UNIT_TYPEID::PROTOSS_FLEETBEACON:
		return UnitCost(300, 200, 0);
	case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
		return UnitCost(75, 0, 0);
	case UNIT_TYPEID::PROTOSS_PROBE:
		return UnitCost(50, 0, 1);
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return UnitCost(100, 0, 2);
	case UNIT_TYPEID::PROTOSS_STALKER:
		return UnitCost(125, 50, 2);
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return UnitCost(50, 100, 2);
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return UnitCost(100, 25, 2);
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return UnitCost(50, 150, 2);
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return UnitCost(125, 125, 2);
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return UnitCost(275, 100, 4);
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return UnitCost(300, 200, 6);
	case UNIT_TYPEID::PROTOSS_DISRUPTOR:
		return UnitCost(150, 150, 3);
	case UNIT_TYPEID::PROTOSS_OBSERVER:
		return UnitCost(25, 75, 1);
	case UNIT_TYPEID::PROTOSS_WARPPRISM:
		return UnitCost(250, 0, 2);
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return UnitCost(150, 100, 2);
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return UnitCost(250, 150, 4);
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return UnitCost(150, 150, 3);
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return UnitCost(350, 250, 6);
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return UnitCost(250, 175, 5);
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return UnitCost(400, 400, 8);
	case UNIT_TYPEID::BEACON_PROTOSS: // represents void ray + fleet beacon
		return UnitCost(550, 350, 4);
	default:
		std::cerr << "Error invalid unit id in GetCost " << UnitTypeToName(unit_type) << std::endl;
		return UnitCost(0, 0, 0);
	}
}



UnitCost Utility::GetCost(UPGRADE_ID upgrade_id)
{
	switch (upgrade_id)
	{
	case UPGRADE_ID::WARPGATERESEARCH:
		return UnitCost(50, 50, 0);
	case UPGRADE_ID::BLINKTECH:
		return UnitCost(150, 150, 0);
	case UPGRADE_ID::CHARGE:
		return UnitCost(100, 100, 0);
	case UPGRADE_ID::ADEPTPIERCINGATTACK:
		return UnitCost(100, 100, 0);
	case UPGRADE_ID::DARKTEMPLARBLINKUPGRADE:
		return UnitCost(100, 100, 0);
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1:
		return UnitCost(100, 100, 0);
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2:
		return UnitCost(150, 150, 0);
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3:
		return UnitCost(200, 200, 0);
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:
		return UnitCost(100, 100, 0);
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:
		return UnitCost(150, 150, 0);
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:
		return UnitCost(200, 200, 0);
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL1:
		return UnitCost(150, 150, 0);
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL2:
		return UnitCost(225, 225, 0);
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL3:
		return UnitCost(300, 300, 0);
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL1:
		return UnitCost(150, 150, 0);
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL2:
		return UnitCost(225, 225, 0);
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL3:
		return UnitCost(300, 300, 0);
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1:
		return UnitCost(100, 100, 0);
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2:
		return UnitCost(175, 175, 0);
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3:
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
	case UNIT_TYPEID::PROTOSS_PYLON:
		cost = UnitCost(100, 0, 0);
		break;
	case UNIT_TYPEID::PROTOSS_NEXUS:
		cost = UnitCost(400, 0, 0);
		break;
	case UNIT_TYPEID::PROTOSS_GATEWAY:
		cost = UnitCost(150, 0, 0);
		break;
	case UNIT_TYPEID::PROTOSS_FORGE:
		cost = UnitCost(150, 0, 0);
		break;
	case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
		cost = UnitCost(150, 0, 0);
		break;
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
		cost = UnitCost(150, 0, 0);
		break;
	case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
		cost = UnitCost(100, 0, 0);
		break;
	case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
		cost = UnitCost(150, 100, 0);
		break;
	case UNIT_TYPEID::PROTOSS_STARGATE:
		cost = UnitCost(150, 150, 0);
		break;
	case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
		cost = UnitCost(150, 100, 0);
		break;
	case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
		cost = UnitCost(150, 150, 0);
		break;
	case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
		cost = UnitCost(150, 200, 0);
		break;
	case UNIT_TYPEID::PROTOSS_DARKSHRINE:
		cost = UnitCost(150, 150, 0);
		break;
	case UNIT_TYPEID::PROTOSS_FLEETBEACON:
		cost = UnitCost(300, 200, 0);
		break;
	case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
		cost = UnitCost(75, 0, 0);
		break;
	case UNIT_TYPEID::PROTOSS_PROBE:
		cost = UnitCost(50, 0, 1);
		break;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		cost = UnitCost(100, 0, 2);
		break;
	case UNIT_TYPEID::PROTOSS_STALKER:
		cost = UnitCost(125, 50, 2);
		break;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		cost = UnitCost(50, 100, 2);
		break;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		cost = UnitCost(100, 25, 2);
		break;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		cost = UnitCost(50, 150, 2);
		break;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		cost = UnitCost(125, 125, 2);
		break;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		cost = UnitCost(275, 100, 4);
		break;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		cost = UnitCost(300, 200, 6);
		break;
	case UNIT_TYPEID::PROTOSS_DISRUPTOR:
		cost = UnitCost(150, 150, 3);
		break;
	case UNIT_TYPEID::PROTOSS_OBSERVER:
		cost = UnitCost(25, 75, 1);
		break;
	case UNIT_TYPEID::PROTOSS_WARPPRISM:
		cost = UnitCost(250, 0, 2);
		break;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		cost = UnitCost(150, 100, 2);
		break;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		cost = UnitCost(250, 150, 4);
		break;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		cost = UnitCost(150, 150, 3);
		break;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		cost = UnitCost(350, 250, 6);
		break;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		cost = UnitCost(250, 175, 5);
		break;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
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
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS:
		return "research ground attack";
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1:
		return "research +1 attack";
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR:
		return "research ground armor";
	case ABILITY_ID::RESEARCH_PROTOSSSHIELDS:
		return "research plasma shields";
	case ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONS:
		return "research air weapons";
	case ABILITY_ID::RESEARCH_PROTOSSAIRARMOR:
		return "research air armor";
	case ABILITY_ID::MORPH_WARPGATE:
		return "morph into warpgate";
	case ABILITY_ID::GENERAL_MOVE:
		return "general move";
	case ABILITY_ID::HARVEST_RETURN:
		return "harvest return";
	case ABILITY_ID::HARVEST_RETURN_PROBE:
		return "harvest return probe";
	case ABILITY_ID::HARVEST_GATHER:
		return "harvest gather";
	case ABILITY_ID::HARVEST_GATHER_PROBE:
		return "harvest gather probe";
	case ABILITY_ID::SMART:
		return "smart";
	case ABILITY_ID::ATTACK:
	case ABILITY_ID::ATTACK_ATTACK:
		return "attack";
	case ABILITY_ID::STOP:
		return "stop";
	case ABILITY_ID::GENERAL_HOLDPOSITION:
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
	case ABILITY_ID::EFFECT_CHRONOBOOSTENERGYCOST:
		return "chrono";
	case ABILITY_ID::BATTERYOVERCHARGE: // 4126
		return "energy recharge";
	case ABILITY_ID::BEHAVIOR_PULSARBEAMON:
		return "pulsar beam on";
	case ABILITY_ID::BEHAVIOR_PULSARBEAMOFF:
		return "pulsar beam off";
	default:
		return "Error invalid abilityId in AbilityIdToString";
	}
}



}