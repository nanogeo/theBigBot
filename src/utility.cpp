
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <functional>
#include <array>
#include <chrono>

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

#include "sc2api/sc2_unit_filters.h"

#include "utility.h"

namespace sc2
{

const Unit* Utility::ClosestTo(Units units, Point2D position)
{
	const Unit* current_closest;
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
	if (units.size() == 0 || current_closest == NULL)
	{
		std::cout << "Error current closest is NULL\n";
		return NULL;
	}
	return current_closest;
}

const Unit* Utility::NthClosestTo(Units units, Point2D position, int n)
{
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
	std::cout << "Error no unit found in NthClosestTo";
	return units[0];
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
	/*if (points.size() == 0 || current_closest == NULL)
	{
		std::cout << "Error current closest is NULL\n";
		return NULL;
	}*/
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
	const Unit* current_furthest;
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
	if (units.size() == 0 || current_furthest == NULL)
	{
		std::cout << "Error current closest is NULL\n";
		return NULL;
	}
	return current_furthest;
}

float Utility::DistanceToClosest(Units units, Point2D position)
{
	const Unit* closest_unit = ClosestTo(units, position);
	if (closest_unit == NULL)
		return INFINITY;
	return Distance2D(closest_unit->pos, position);
}

float Utility::DistanceToClosest(std::vector<Point2D> points, Point2D position)
{
	const Point2D closest_point = ClosestTo(points, position);
	return Distance2D(closest_point, position);
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
	return total / units.size();
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

Point2D Utility::PointBetween(Point2D start, Point2D end, float dist)
{
	float total_dist = Distance2D(start, end);
	if (total_dist == 0)
	{
		std::cout << "Warning PointBetween called on the same point";
		return start;
	}
	return Point2D(start.x + dist * (end.x - start.x) / total_dist, start.y + dist * (end.y - start.y) / total_dist);
}

int Utility::DangerLevel(const Unit* unit, const ObservationInterface* observation)
{
	return DangerLevelAt(unit, unit->pos, observation);
}

int Utility::DangerLevelAt(const Unit* unit, Point2D pos, const ObservationInterface* observation)
{
	int possible_damage = 0;
	for (const auto &enemy_unit : observation->GetUnits(Unit::Alliance::Enemy))
	{
		if (!enemy_unit->is_building && Distance2D(pos, enemy_unit->pos) <= RealGroundRange(enemy_unit, unit))
			possible_damage += GetDamage(enemy_unit, unit, observation);
	}
	return possible_damage;
}

int Utility::GetDamage(const Unit* attacker, const Unit* target, const ObservationInterface* observation)
{
	bool is_light = 0;
	bool is_armored = 0;
	bool is_biological = 0;
	bool is_mechanical = 0;
	bool is_massive = 0;
	bool is_flying = target->is_flying;
	for (const auto &attribute : observation->GetUnitTypeData()[target->unit_type].attributes)
	{
		if (attribute == Attribute::Light)
			is_light = 1;
		else if (attribute == Attribute::Armored)
			is_armored = 1;
		else if (attribute == Attribute::Biological)
			is_biological = 1;
		else if (attribute == Attribute::Mechanical)
			is_mechanical = 1;
		else if (attribute == Attribute::Massive)
			is_massive = 1;
	}
	int upgrade_level = attacker->attack_upgrade_level;
	int damage = 0;
	int attacks = 1;
	switch (attacker->unit_type.ToType())
	{
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		damage = 20;
		break;
	case UNIT_TYPEID::PROTOSS_PROBE:
		damage = 5;
		break;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
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
		damage = 10 + (upgrade_level * 1) + (is_light * (12 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		damage = 4 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		damage = 45 + (upgrade_level * 5);
		break;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		damage = 25 + (upgrade_level * 3) + (is_biological * (10 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		damage = 20 + (upgrade_level * 2) + (is_armored * (30 + (upgrade_level * 3)));
		break;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		damage = 10 + (upgrade_level * 1) + (is_light * (5 + (upgrade_level * 1)));
		attacks = 2;
		break;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		damage = 5 + (upgrade_level * 1) + (is_light * 5);
		attacks = 2;
		break;
	case UNIT_TYPEID::PROTOSS_VOIDRAY: // TODO prismatic alignment
		damage = 6 + (upgrade_level * 1) + (is_armored * 4);
		break;
	case UNIT_TYPEID::PROTOSS_ORACLE:
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
		damage = 40;
		break;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		damage = 12;
		attacks = 2;
		break;
	case UNIT_TYPEID::TERRAN_SCV:
		damage = 5;
		break;
	case UNIT_TYPEID::TERRAN_MARINE:
		damage = 6 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::TERRAN_MARAUDER:
		damage = 10 + (upgrade_level * 1) + (is_armored * (10 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_REAPER:
		damage = 4 + (upgrade_level * 1);
		attacks = 2;
		break;
	case UNIT_TYPEID::TERRAN_GHOST:
		damage = 10 + (upgrade_level * 1) + (is_light * (10 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_HELLION: // TODO blue flame
		damage = 8 + (upgrade_level * 1) + (is_light * (6 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		damage = 18 + (upgrade_level * 2) + (is_light * (0 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		damage = 15 + (upgrade_level * 2) + (is_armored * (10 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
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
		damage = 12 + (upgrade_level * 1) + (is_mechanical * (8 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		damage = 10 + (upgrade_level * 1) + (is_armored * 4);
		attacks = 2;
		break;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		damage = 5 + (upgrade_level * 1);
		attacks = 2;
		break;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		damage = 75 + (upgrade_level * 5);
		break;
	case UNIT_TYPEID::TERRAN_BANSHEE:
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
		damage = 25 + (is_armored * 5);
		break;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		damage = 15 + (is_biological * 15);
		break;
	case UNIT_TYPEID::ZERG_DRONE:
		damage = 5;
		break;
	case UNIT_TYPEID::ZERG_QUEEN:
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
		damage = 5 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::ZERG_BANELING:
		damage = 16 + (upgrade_level * 2) + (is_light * (19 + (upgrade_level * 2)));
		break;
	case UNIT_TYPEID::ZERG_ROACH:
		damage = 16 + (upgrade_level * 2);
		break;
	case UNIT_TYPEID::ZERG_RAVAGER:
		damage = 16 + (upgrade_level * 2);
		break;
	case UNIT_TYPEID::ZERG_HYDRALISK:
		damage = 12 + (upgrade_level * 2);
		break;
	case UNIT_TYPEID::ZERG_LURKERMP:
		damage = 20 + (upgrade_level * 2) + (is_armored * (10 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::ZERG_ULTRALISK:
		damage = 35 + (upgrade_level * 3);
		break;
	case UNIT_TYPEID::ZERG_MUTALISK:
		damage = 9 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		damage = 14 + (upgrade_level * 1) + (is_massive * (6 + (upgrade_level * 1)));
		break;
	case UNIT_TYPEID::ZERG_BROODLORD:
		damage = 20 + (upgrade_level * 2);
		attacks = 2;
		break;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		damage = 10 + (upgrade_level * 1);
		break;
	case UNIT_TYPEID::ZERG_BROODLING:
		damage = 4 + (upgrade_level * 1);
		break;
	default:
		std::cout << "Error invalid unit type in GetDamage\n";
		return 0;
	}

	int armor = GetArmor(target);
	damage -= armor;
	return damage * attacks;

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
		std::cout << "Error invalid unit type in GetArmor\n";
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
		std::cout << "Error invalid unit type in GetCargoSize\n";
		return 0;
	}

}

float Utility::RealGroundRange(const Unit* attacking_unit, const Unit * target)
{
	float range = attacking_unit->radius + target->radius;
	switch (attacking_unit->unit_type.ToType())
	{
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		range += 7;
		break;
	case UNIT_TYPEID::PROTOSS_PROBE:
		range += .1;
		break;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		range += .1;
		break;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		range += 5;
		break;
	case UNIT_TYPEID::PROTOSS_STALKER:
		range += 6;
		break;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		range += 4;
		break;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		range += 6;
		break;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		range += .1;
		break;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		range += 3;
		break;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		range += 6;
		break;
	case UNIT_TYPEID::PROTOSS_COLOSSUS: // TODO extended thermal lance
		range += 7;
		break;
	case UNIT_TYPEID::PROTOSS_PHOENIX: // TODO anion pulse crystals
		range += 5;
		break;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		range += 6;
		break;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		range += 4;
		break;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		range += 14;
		break;
	case UNIT_TYPEID::PROTOSS_TEMPEST: // air 14
		range += 10;
		break;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		range += 7;
		break;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: // terran hi sec auto tracking
		range += 6;
		break;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		range += 6;
		break;
	case UNIT_TYPEID::TERRAN_SCV:
		range += .1;
		break;
	case UNIT_TYPEID::TERRAN_MULE:
		range += 0;
		break;
	case UNIT_TYPEID::TERRAN_MARINE:
		range += 5;
		break;
	case UNIT_TYPEID::TERRAN_MARAUDER:
		range += 6;
		break;
	case UNIT_TYPEID::TERRAN_REAPER:
		range += 5;
		break;
	case UNIT_TYPEID::TERRAN_GHOST:
		range += 6;
		break;
	case UNIT_TYPEID::TERRAN_HELLION:
		range += 5;
		break;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		range += 2;
		break;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		range += 7;
		break;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		range += 13;
		break;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		range += 5;
		break;
	case UNIT_TYPEID::TERRAN_THOR: // flying 10
		range += 7;
		break;
	case UNIT_TYPEID::TERRAN_THORAP: // flying 11
		range += 7;
		break;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		range += 6;
		break;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		range += 9;
		break;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		range += 6;
		break;
	case UNIT_TYPEID::TERRAN_LIBERATOR: // flying
		range += 5;
		break;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		range += 0;
		break;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		range += 6;
		break;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		range += 6;
		break;
	case UNIT_TYPEID::ZERG_SPINECRAWLER: // zerg
		range += 7;
		break;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		range += 7;
		break;
	case UNIT_TYPEID::ZERG_DRONE:
		range += .1;
		break;
	case UNIT_TYPEID::ZERG_QUEEN: // flying 7
		range += 5;
		break;
	case UNIT_TYPEID::ZERG_ZERGLING:
		range += .1;
		break;
	case UNIT_TYPEID::ZERG_BANELING:
		range += 2.5;
		break;
	case UNIT_TYPEID::ZERG_ROACH:
		range += 4;
		break;
	case UNIT_TYPEID::ZERG_RAVAGER:
		range += 6;
		break;
	case UNIT_TYPEID::ZERG_HYDRALISK: // grooved spines
		range += 6;
		break;
	case UNIT_TYPEID::ZERG_LURKERMP: // seismic spine
		range += 8;
		break;
	case UNIT_TYPEID::ZERG_ULTRALISK:
		range += .1;
		break;
	case UNIT_TYPEID::ZERG_MUTALISK:
		range += 3;
		break;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		range += 6;
		break;
	case UNIT_TYPEID::ZERG_BROODLORD:
		range += 10;
		break;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		range += 3;
		break;
	case UNIT_TYPEID::ZERG_BROODLING:
		range += 0;
		break;
	default:
		std::cout << "Error invalid unit type in RealGroundRange\n";
		range += 0;
	}
	return range;
}

float Utility::GetDamagePoint(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		return .1193;
	case UNIT_TYPEID::PROTOSS_PROBE:
		return .1193;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return 0;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return .1193;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return .1193;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return .1193;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return .1193;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return .2579;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return .1193;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return .1193;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return .0594;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return .1193;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return .1193;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return .1193;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return 0;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return .1193;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return 0;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
		return .1193;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		return .1193;
	case UNIT_TYPEID::TERRAN_SCV:
		return .1193;
	case UNIT_TYPEID::TERRAN_MARINE:
		return .0357;
	case UNIT_TYPEID::TERRAN_MARAUDER:
		return 0;
	case UNIT_TYPEID::TERRAN_REAPER:
		return 0;
	case UNIT_TYPEID::TERRAN_GHOST:
		return .0593;
	case UNIT_TYPEID::TERRAN_HELLION:
		return .1786;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return .1193;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return .1193;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		return .1193;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return .1193;
	case UNIT_TYPEID::TERRAN_THOR:
		return .5936;
	case UNIT_TYPEID::TERRAN_THORAP:
		return .5936;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return .1193;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return .1193;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return .0357;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return .1193;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		return .0893;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		return .1193;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return .1193;
	case UNIT_TYPEID::ZERG_SPINECRAWLER:
		return .238;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		return .1193;
	case UNIT_TYPEID::ZERG_DRONE:
		return .1193;
	case UNIT_TYPEID::ZERG_QUEEN:
		return .1193;
	case UNIT_TYPEID::ZERG_ZERGLING:
		return .1193;
	case UNIT_TYPEID::ZERG_BANELING:
		return 0;
	case UNIT_TYPEID::ZERG_ROACH:
		return .1193;
	case UNIT_TYPEID::ZERG_RAVAGER:
		return .1429;
	case UNIT_TYPEID::ZERG_HYDRALISK:
		return .1;
	case UNIT_TYPEID::ZERG_LURKERMP:
		return 0;
	case UNIT_TYPEID::ZERG_ULTRALISK:
		return .238;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return 0;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		return .0446;
	case UNIT_TYPEID::ZERG_BROODLORD:
		return .1193;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return .1904;
	case UNIT_TYPEID::ZERG_BROODLING:
		return .1193;
	default:
		std::cout << "Error invalid unit type in GetDamagePoint\n";
		return 0;
	}
}

int Utility::GetProjectileTime(const Unit* unit, float dist)
{
	switch (unit->unit_type.ToType())
	{
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		return dist;
	case UNIT_TYPEID::PROTOSS_PROBE:
		return 0;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return 0;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return 0;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return dist;
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return dist;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return dist;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return 0;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return 0;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return 0;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return 0;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return dist;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return 0;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return 0;
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return 0;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return dist;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return 0;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
		return 0;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		return dist;
	case UNIT_TYPEID::TERRAN_SCV:
		return 0;
	case UNIT_TYPEID::TERRAN_MARINE:
		return 0;
	case UNIT_TYPEID::TERRAN_MARAUDER:
		return dist;
	case UNIT_TYPEID::TERRAN_REAPER:
		return 0;
	case UNIT_TYPEID::TERRAN_GHOST:
		return 0;
	case UNIT_TYPEID::TERRAN_HELLION:
		return 0;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return 0;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return 0;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		return 0;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return dist;
	case UNIT_TYPEID::TERRAN_THOR:
		return 0;
	case UNIT_TYPEID::TERRAN_THORAP:
		return 0;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return 0;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return 0;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return dist;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return dist;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		return 0;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		return dist;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return dist;
	case UNIT_TYPEID::ZERG_SPINECRAWLER:
		return dist;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		return dist;
	case UNIT_TYPEID::ZERG_DRONE:
		return 0;
	case UNIT_TYPEID::ZERG_QUEEN:
		return dist;
	case UNIT_TYPEID::ZERG_ZERGLING:
		return 0;
	case UNIT_TYPEID::ZERG_BANELING:
		return 0;
	case UNIT_TYPEID::ZERG_ROACH:
		return dist;
	case UNIT_TYPEID::ZERG_RAVAGER:
		return dist;
	case UNIT_TYPEID::ZERG_HYDRALISK:
		return dist;
	case UNIT_TYPEID::ZERG_LURKERMP:
		return 0;
	case UNIT_TYPEID::ZERG_ULTRALISK:
		return 0;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return dist;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		return dist;
	case UNIT_TYPEID::ZERG_BROODLORD:
		return dist;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return dist;
	case UNIT_TYPEID::ZERG_BROODLING:
		return 0;
	default:
		std::cout << "Error invalid unit type in GetProjectileTime\n";
		return 0;
	}
}

float Utility::GetWeaponCooldown(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON: // protoss
		return .89;
	case UNIT_TYPEID::PROTOSS_PROBE:
		return 1.07;
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return .86;
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return .71;
	case UNIT_TYPEID::PROTOSS_STALKER:
		return 1.34;
	case UNIT_TYPEID::PROTOSS_ADEPT: // TODO glaives
		return 1.61;
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return 1.25;
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return 1.21;
	case UNIT_TYPEID::PROTOSS_ARCHON:
		return 1.25;
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return 1.04;
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return 1.07;
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return .79;
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return .36;
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return .61;
	case UNIT_TYPEID::PROTOSS_CARRIER: // TODO each interceptor?
		return 2.14;
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return 2.36;
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return 1.58;
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
		return 1.43;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		return .61;
	case UNIT_TYPEID::TERRAN_SCV:
		return 1.07;
	case UNIT_TYPEID::TERRAN_MARINE: // TODO stim
		return .61;
	case UNIT_TYPEID::TERRAN_MARAUDER: // TODO stim
		return 1.07;
	case UNIT_TYPEID::TERRAN_REAPER:
		return .79;
	case UNIT_TYPEID::TERRAN_GHOST:
		return 1.07;
	case UNIT_TYPEID::TERRAN_HELLION:
		return 1.79;
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return 1.43;
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return .79;
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		return 2.14;
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return .71;
	case UNIT_TYPEID::TERRAN_THOR: // TODO anit air
		return .91;
	case UNIT_TYPEID::TERRAN_THORAP:
		return .91;
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return .57;
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return .71;
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return 1.43;
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return 1.29;
	case UNIT_TYPEID::TERRAN_LIBERATORAG:
		return 1.14;
	case UNIT_TYPEID::TERRAN_BANSHEE:
		return .89;
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return .16;
	case UNIT_TYPEID::ZERG_SPINECRAWLER:
		return 1.32;
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		return .61;
	case UNIT_TYPEID::ZERG_DRONE:
		return 1.07;
	case UNIT_TYPEID::ZERG_QUEEN:
		return .71;
	case UNIT_TYPEID::ZERG_ZERGLING: //TODO adrenal
		return .497;
	case UNIT_TYPEID::ZERG_BANELING:
		return 0;
	case UNIT_TYPEID::ZERG_ROACH:
		return 1.43;
	case UNIT_TYPEID::ZERG_RAVAGER:
		return 1.43;
	case UNIT_TYPEID::ZERG_HYDRALISK:
		return .59;
	case UNIT_TYPEID::ZERG_LURKERMP:
		return 1.43;
	case UNIT_TYPEID::ZERG_ULTRALISK:
		return .61;
	case UNIT_TYPEID::ZERG_MUTALISK:
		return 1.09;
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		return 1.36;
	case UNIT_TYPEID::ZERG_BROODLORD:
		return 1.79;
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return .43;
	case UNIT_TYPEID::ZERG_BROODLING:
		return .46;
	default:
		std::cout << "Error invalid unit type in GetWeaponCooldown\n";
		return 0;
	}
}

bool Utility::IsOnHighGround(Point3D unit, Point3D enemy_unit)
{
	return unit.z + .5 < enemy_unit.z;
}

float Utility::GetTimeBuilt(const Unit* unit, const ObservationInterface* observation)
{
	float build_time = 0;
	switch (unit->unit_type.ToType())
	{
	case UNIT_TYPEID::ZERG_HATCHERY:
		build_time = 71;
		break;
	case UNIT_TYPEID::ZERG_EXTRACTOR:
		build_time = 21;
		break;
	case UNIT_TYPEID::ZERG_SPAWNINGPOOL:
		build_time = 46;
		break;
	case UNIT_TYPEID::ZERG_ROACHWARREN:
		build_time = 39;
		break;
	case UNIT_TYPEID::TERRAN_COMMANDCENTER:
		build_time = 71;
		break;
	case UNIT_TYPEID::TERRAN_REFINERY:
		build_time = 30;
		break;
	case UNIT_TYPEID::TERRAN_BARRACKS:
		build_time = 46;
		break;
	case UNIT_TYPEID::TERRAN_FACTORY:
		build_time = 43;
		break;
	default:
		std::cout << "Error Unknown building in GetTimeBuilt\n";
		break;
	}
	return (observation->GetGameLoop() / 22.4) - (build_time * unit->build_progress);
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
		return NULL;
	}
}

bool Utility::IsFacing(const Unit* unit, const Unit* target)
{
	Point2D vec = Point2D(target->pos.x - unit->pos.x, target->pos.y - unit->pos.y);
	float angle = atan2(vec.y, vec.x);
	if (angle < 0)
		angle += 2 * M_PI;
	float facing = unit->facing;
	return angle >= facing - .005 && angle <= facing + .005;
}

float Utility::GetFacingAngle(const Unit* unit, const Unit* target)
{
	Point2D vec = Point2D(target->pos.x - unit->pos.x, target->pos.y - unit->pos.y);
	float angle = atan2(vec.y, vec.x);
	if (angle < 0)
		angle += 2 * M_PI;
	return std::abs(angle - unit->facing);
}

const Unit* Utility::AimingAt(const Unit* unit, const ObservationInterface* observation)
{
	float smallest_angle = 180;
	const Unit* target = NULL;
	for (const auto Funit : observation->GetUnits(Unit::Alliance::Self))
	{
		if (Distance2D(unit->pos, Funit->pos) >= RealGroundRange(unit, Funit))
			continue;
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
	if (buildingId == UNIT_TYPEID::PROTOSS_NEXUS)
		return 2.5;
	if (buildingId == UNIT_TYPEID::PROTOSS_PYLON || buildingId == UNIT_TYPEID::PROTOSS_SHIELDBATTERY || buildingId == UNIT_TYPEID::PROTOSS_PHOTONCANNON)
		return 1;
	return 1.5;
}

bool Utility::CanBuildBuilding(UNIT_TYPEID buildingId, const ObservationInterface* observation)
{
	if (CanAfford(buildingId, 1, observation))
	{
		if (buildingId == UNIT_TYPEID::PROTOSS_GATEWAY)
			return BuildingsReady(UNIT_TYPEID::PROTOSS_PYLON, observation) > 0;
		return true;
	}
	return false;
}

int Utility::BuildingsReady(UNIT_TYPEID buildingId, const ObservationInterface* observation)
{
	int ready = 0;
	for (const auto &building : observation->GetUnits(IsUnit(buildingId)))
	{
		if (building->build_progress == 1)
			ready++;
	}
	return ready;
}

const Unit* Utility::GetLeastFullPrism(Units units)
{
	const Unit* least_full = NULL;
	for (const auto &unit : units)
	{
		if (least_full == NULL || unit->cargo_space_taken < least_full->cargo_space_taken)
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
		std::cout << "Error invalid building id in GetBuildAbility";
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
		std::cout << "Error invalid unit id in GetTrainAbility";
		return ABILITY_ID::BUILD_CANCEL;
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
		cost = UnitCost(50, 150, 2);
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
		std::cout << "Error invalid unit id in CanAfford";
		return false;
	}
	bool enough_minerals = observation->GetMinerals() >= cost.mineral_cost * amount;
	bool enough_vespene = observation->GetVespene() >= cost.vespene_cost * amount;
	bool enough_supply = observation->GetFoodCap() - observation->GetFoodUsed() >= cost.supply * amount;
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
	default:
		std::cout << "Error invalid upgrade id in CanAffordUpgrade";
		return false;
	}
	bool enough_minerals = observation->GetMinerals() >= cost.mineral_cost;
	bool enough_vespene = observation->GetVespene() >= cost.vespene_cost;
	return enough_minerals && enough_vespene;
}

std::vector<UNIT_TYPEID> Utility::GetBurrowedUnitTypes()
{
	return { UNIT_TYPEID::RAVAGERBURROWED, UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED, UNIT_TYPEID::ZERG_BANELINGBURROWED, UNIT_TYPEID::ZERG_DRONEBURROWED,
			UNIT_TYPEID::ZERG_HYDRALISKBURROWED, UNIT_TYPEID::ZERG_INFESTORBURROWED, UNIT_TYPEID::ZERG_LURKERMPBURROWED, UNIT_TYPEID::ZERG_QUEENBURROWED,
			UNIT_TYPEID::ZERG_ROACHBURROWED, UNIT_TYPEID::ZERG_SWARMHOSTBURROWEDMP, UNIT_TYPEID::ZERG_ULTRALISKBURROWED, UNIT_TYPEID::ZERG_ZERGLINGBURROWED };
}

std::string Utility::UnitTypeIdToString(UNIT_TYPEID typeId)
{
	switch (typeId)
	{
	case UNIT_TYPEID::PROTOSS_PYLON: // protoss buildings
		return "pylon";
	case UNIT_TYPEID::PROTOSS_NEXUS:
		return "nexus";
	case UNIT_TYPEID::PROTOSS_GATEWAY:
		return "gateway";
	case UNIT_TYPEID::PROTOSS_WARPGATE:
		return "warpgate";
	case UNIT_TYPEID::PROTOSS_FORGE:
		return "forge";
	case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
		return "cybercore";
	case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
		return "photon cannon";
	case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
		return "shield battery";
	case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
		return "twilight";
	case UNIT_TYPEID::PROTOSS_STARGATE:
		return "stargate";
	case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
		return "robo";
	case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
		return "robo bay";
	case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
		return "templar archives";
	case UNIT_TYPEID::PROTOSS_DARKSHRINE:
		return "dark shrine";
	case UNIT_TYPEID::PROTOSS_FLEETBEACON:
		return "fleet beacon";
	case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
		return "assimilator";
	case UNIT_TYPEID::PROTOSS_PROBE: // protoss units
		return "probe";
	case UNIT_TYPEID::PROTOSS_ZEALOT:
		return "zealot";
	case UNIT_TYPEID::PROTOSS_SENTRY:
		return "sentry";
	case UNIT_TYPEID::PROTOSS_STALKER:
		return "stalker";
	case UNIT_TYPEID::PROTOSS_ADEPT:
		return "adept";
	case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
		return "high templar";
	case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
		return "dark templar";
	case UNIT_TYPEID::PROTOSS_IMMORTAL:
		return "immortal";
	case UNIT_TYPEID::PROTOSS_COLOSSUS:
		return "colossus";
	case UNIT_TYPEID::PROTOSS_DISRUPTOR:
		return "disruptor";
	case UNIT_TYPEID::PROTOSS_OBSERVER:
		return "observer";
	case UNIT_TYPEID::PROTOSS_WARPPRISM:
		return "warp prism";
	case UNIT_TYPEID::PROTOSS_PHOENIX:
		return "phoenis";
	case UNIT_TYPEID::PROTOSS_VOIDRAY:
		return "void ray";
	case UNIT_TYPEID::PROTOSS_ORACLE:
		return "oracle";
	case UNIT_TYPEID::PROTOSS_CARRIER:
		return "carrier";
	case UNIT_TYPEID::PROTOSS_TEMPEST:
		return "tempest";
	case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
		return "mothership";
	case UNIT_TYPEID::TERRAN_COMMANDCENTER: // terran buildings
		return "command center";
	case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
		return "planetary forttress";
	case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
		return "orbital command";
	case UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
		return "supply depot";
	case UNIT_TYPEID::TERRAN_REFINERY:
		return "refinery";
	case UNIT_TYPEID::TERRAN_BARRACKS:
		return "barracks";
	case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
		return "engineering bay";
	case UNIT_TYPEID::TERRAN_BUNKER:
		return "bunker";
	case UNIT_TYPEID::TERRAN_SENSORTOWER:
		return "sensor tower";
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		return "missile turret";
	case UNIT_TYPEID::TERRAN_FACTORY:
		return "factory";
	case UNIT_TYPEID::TERRAN_GHOSTACADEMY:
		return "ghost academy";
	case UNIT_TYPEID::TERRAN_STARPORT:
		return "starport";
	case UNIT_TYPEID::TERRAN_ARMORY:
		return "armory";
	case UNIT_TYPEID::TERRAN_FUSIONCORE:
		return "fusion core";
	case UNIT_TYPEID::TERRAN_TECHLAB:
		return "teck lab";
	case UNIT_TYPEID::TERRAN_REACTOR:
		return "reactor";
	case UNIT_TYPEID::TERRAN_SCV:
		return "SCV";
	case UNIT_TYPEID::TERRAN_MULE:
		return "MULE";
	case UNIT_TYPEID::TERRAN_MARINE: // terran units
		return "marine";
	case UNIT_TYPEID::TERRAN_MARAUDER:
		return "marauder";
	case UNIT_TYPEID::TERRAN_REAPER:
		return "reaper";
	case UNIT_TYPEID::TERRAN_GHOST:
		return "ghost";
	case UNIT_TYPEID::TERRAN_HELLION:
		return "hellion";
	case UNIT_TYPEID::TERRAN_HELLIONTANK:
		return "hellbat";
	case UNIT_TYPEID::TERRAN_SIEGETANK:
		return "siege tank";
	case UNIT_TYPEID::TERRAN_CYCLONE:
		return "cyclone";
	case UNIT_TYPEID::TERRAN_WIDOWMINE:
		return "widow mine";
	case UNIT_TYPEID::TERRAN_THOR:
		return "thor";
	case UNIT_TYPEID::TERRAN_AUTOTURRET:
		return "auto turret";
	case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		return "viking assault";
	case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		return "viking fighter";
	case UNIT_TYPEID::TERRAN_MEDIVAC:
		return "medivac";
	case UNIT_TYPEID::TERRAN_LIBERATOR:
		return "liberator";
	case UNIT_TYPEID::TERRAN_RAVEN:
		return "raven";
	case UNIT_TYPEID::TERRAN_BANSHEE:
		return "banshee";
	case UNIT_TYPEID::TERRAN_BATTLECRUISER:
		return "battlecruiser";
	case UNIT_TYPEID::ZERG_HATCHERY: // zerg buildings
		return "hatchery";
	case UNIT_TYPEID::ZERG_LAIR:
		return "lair";
	case UNIT_TYPEID::ZERG_HIVE:
		return "hive";
	case UNIT_TYPEID::ZERG_SPINECRAWLER:
		return "spine crawler";
	case UNIT_TYPEID::ZERG_SPORECRAWLER:
		return "spore crawler";
	case UNIT_TYPEID::ZERG_EXTRACTOR:
		return "extractor";
	case UNIT_TYPEID::ZERG_SPAWNINGPOOL:
		return "spawning pool";
	case UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:
		return "evolution chamber";
	case UNIT_TYPEID::ZERG_ROACHWARREN:
		return "roach warren";
	case UNIT_TYPEID::ZERG_BANELINGNEST:
		return "baneling next";
	case UNIT_TYPEID::ZERG_HYDRALISKDEN:
		return "hydralisk den";
	case UNIT_TYPEID::ZERG_LURKERDENMP:
		return "lurker den";
	case UNIT_TYPEID::ZERG_INFESTATIONPIT:
		return "infestation pit";
	case UNIT_TYPEID::ZERG_SPIRE:
		return "spire";
	case UNIT_TYPEID::ZERG_GREATERSPIRE:
		return "greater spire";
	case UNIT_TYPEID::ZERG_NYDUSNETWORK:
		return "nydus network";
	case UNIT_TYPEID::ZERG_ULTRALISKCAVERN:
		return "ultralisk cavern";
	case UNIT_TYPEID::ZERG_LARVA: // zerg units
		return "larva";
	case UNIT_TYPEID::ZERG_EGG:
		return "egg";
	case UNIT_TYPEID::ZERG_DRONE:
		return "drone";
	case UNIT_TYPEID::ZERG_QUEEN:
		return "queen";
	case UNIT_TYPEID::ZERG_ZERGLING:
		return "zergling";
	case UNIT_TYPEID::ZERG_BANELING:
		return "baneling";
	case UNIT_TYPEID::ZERG_ROACH:
		return "roach";
	case UNIT_TYPEID::ZERG_RAVAGER:
		return "ravager";
	case UNIT_TYPEID::ZERG_HYDRALISK:
		return "hydralisk";
	case UNIT_TYPEID::ZERG_LURKERMP:
		return "lurker";
	case UNIT_TYPEID::ZERG_INFESTOR:
		return "infestor";
	case UNIT_TYPEID::ZERG_SWARMHOSTMP:
		return "swarm host";
	case UNIT_TYPEID::ZERG_ULTRALISK:
		return "ultralisk";
	case UNIT_TYPEID::ZERG_OVERLORD:
		return "overlord";
	case UNIT_TYPEID::ZERG_OVERSEER:
		return "overseer";
	case UNIT_TYPEID::ZERG_MUTALISK:
		return "mutalisk";
	case UNIT_TYPEID::ZERG_CORRUPTOR:
		return "corruptor";
	case UNIT_TYPEID::ZERG_BROODLORD:
		return "broodlord";
	case UNIT_TYPEID::ZERG_VIPER:
		return "viper";
	case UNIT_TYPEID::ZERG_LOCUSTMP:
		return "locust";
	case UNIT_TYPEID::ZERG_BROODLING:
		return "broodling";
	case UNIT_TYPEID::ZERG_TRANSPORTOVERLORDCOCOON:
		return "dropperlord cocoon";
	case UNIT_TYPEID::ZERG_BANELINGCOCOON:
		return "baneling cocoon";
	case UNIT_TYPEID::ZERG_BROODLORDCOCOON:
		return "broodlord cocoon";
	case UNIT_TYPEID::ZERG_OVERLORDCOCOON:
		return "overseer cocoon";
	case UNIT_TYPEID::ZERG_RAVAGERCOCOON:
		return "ravager cocoon";
	case UNIT_TYPEID::ZERG_LURKERMPEGG:
		return "lurker cocoon";
	default:
		std::cout << "Error invalid typeId in UnitTypeIdToString";
		return "Error invalid abilityId in UnitTypeIdToString";
		break;
	}
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
	case ABILITY_ID::RESEARCH_WARPGATE:
		return "research warpgate";
	case ABILITY_ID::RESEARCH_BLINK:
		return "research blink";
	case ABILITY_ID::RESEARCH_CHARGE:
		return "research charge";
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS:
		return "research ground attack";
	case ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1:
		return "research +1 attack";
	case ABILITY_ID::MORPH_WARPGATE:
		return "morph into warpgate";
	default:
		std::cout << "Error invalid abilityId in AbilityIdToString\n";
		return "Error invalid abilityId in AbilityIdToString";
		break;
	}
}


}