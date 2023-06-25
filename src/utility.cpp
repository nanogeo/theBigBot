
#define _USE_MATH_DEFINES
#include <math.h>


#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

#include "sc2api/sc2_unit_filters.h"

#include "utility.h"

namespace sc2
{

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


}