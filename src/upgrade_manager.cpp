
#include "mediator.h"
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
	std::cerr << "Error unknown upgrade in GetUpgradeLevel" << std::to_string(upgrade_type) << std::endl;
	mediator->LogMinorError();
	return 0;
}

void UpgradeManager::OnUpgradeCompleted(UPGRADE_ID upgrade)
{
	switch (upgrade)
	{
	case UPGRADE_ID::WARPGATERESEARCH:
		has_warpgate = true;
		break;
	case UPGRADE_ID::BLINKTECH:
		has_blink = true;
		break;
	case UPGRADE_ID::CHARGE:
		has_charge = true;
		break;
	case UPGRADE_ID::DARKTEMPLARBLINKUPGRADE:
		has_dt_blink = true;
		break;
	case UPGRADE_ID::PSISTORMTECH:
		has_storm = true;
		break;
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1:
		ground_weapons = 1;
		break;
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2:
		ground_weapons = 2;
		break;
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3:
		ground_weapons = 3;
		break;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:
		ground_armor = 1;
		break;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:
		ground_armor = 2;
		break;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:
		ground_armor = 3;
		break;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL1:
		shields = 1;
		break;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL2:
		shields = 2;
		break;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL3:
		shields = 3;
		break;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1:
		air_weapons = 1;
		break;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2:
		air_weapons = 2;
		break;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3:
		air_weapons = 3;
		break;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL1:
		air_armor = 1;
		break;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL2:
		air_armor = 2;
		break;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL3:
		air_armor = 3;
		break;
	default:
		std::cerr << "Unknown upgrade found in OnUpgradeCompleted" << std::endl;
		mediator->LogMinorError();
		break;
	}
}

bool UpgradeManager::CheckUpgrade(UPGRADE_ID upgrade_id)
{
	switch (upgrade_id)
	{
	case UPGRADE_ID::WARPGATERESEARCH:
		return has_warpgate;
		break;
	case UPGRADE_ID::BLINKTECH:
		return has_blink;
		break;
	case UPGRADE_ID::CHARGE:
		return has_charge;
		break;
	case UPGRADE_ID::DARKTEMPLARBLINKUPGRADE:
		return has_dt_blink;
		break;
	case UPGRADE_ID::PSISTORMTECH:
		return has_storm;
		break;
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1:
		return ground_weapons > 0;
		break;
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2:
		return ground_weapons > 1;
		break;
	case UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3:
		return ground_weapons > 2;
		break;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:
		return ground_armor > 0;
		break;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:
		return ground_armor > 1;
		break;
	case UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:
		return ground_armor > 2;
		break;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL1:
		return shields > 0;
		break;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL2:
		return shields > 1;
		break;
	case UPGRADE_ID::PROTOSSSHIELDSLEVEL3:
		return shields > 2;
		break;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1:
		return air_weapons > 0;
		break;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2:
		return air_weapons > 1;
		break;
	case UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3:
		return air_weapons > 2;
		break;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL1:
		return air_armor > 0;
		break;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL2:
		return air_armor > 1;
		break;
	case UPGRADE_ID::PROTOSSAIRARMORSLEVEL3:
		return air_armor > 2;
		break;
	default:
		std::cerr << "Unknown upgrade found in CheckUpgrade" << std::endl;
		mediator->LogMinorError();
		return false;
		break;
	}
}

}