#pragma once
#include "definitions.h"
#include "upgrade_manager.h"

namespace sc2 {



int UpgradeManager::GetUpgradeLevel(UpgradeType upgrade_type)
{
	switch (upgrade_type)
	{
	case UpgradeType::ground_weapons:
		return ground_weapons;
	case UpgradeType::ground_armor:
		return ground_armor;
	case UpgradeType::shields:
		return shields;
	case UpgradeType::air_weapons:
		return air_weapons;
	case UpgradeType::air_armor:
		return air_armor;
	}
}

void UpgradeManager::OnUpgradeCompleted(UPGRADE_ID upgrade)
{
	switch (upgrade)
	{
	case sc2::UPGRADE_ID::BLINKTECH:
		has_blink = true;
		break;
	case sc2::UPGRADE_ID::CHARGE:
		has_charge = true;
		break;
	case sc2::UPGRADE_ID::DARKTEMPLARBLINKUPGRADE:
		has_dt_blink = true;
		break;
	case sc2::UPGRADE_ID::PSISTORMTECH:
		has_storm = true;
		break;
	case sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1:
		ground_weapon = 1;
		break;
	case sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2:
		ground_weapon = 2;
		break;
	case sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3:
		ground_weapon = 3;
		break;
	case sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:
		ground_armor = 1;
		break;
	case sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:
		ground_armor = 2;
		break;
	case sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:
		ground_armor = 3;
		break;
	case sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL1:
		shields = 1;
		break;
	case sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL2:
		shields = 2;
		break;
	case sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL3:
		shields = 3;
		break;
	case sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1:
		air_weapon = 1;
		break;
	case sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2:
		air_weapon = 2;
		break;
	case sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3:
		air_weapon = 3;
		break;
	case sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL1:
		air_armor = 1;
		break;
	case sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL2:
		air_armor = 2;
		break;
	case sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL3:
		air_armor = 3;
		break;
	default:
		std::cerr << "Unknown upgrade found in OnUpgradeCompleted" << std::endl;
		break;
	}
}

}