
#include "mediator.h"
#include "definitions.h"
#include "upgrade_manager.h"

namespace sc2 {


UPGRADE_ID UpgradeManager::AbilityIDToUpgradeID(ABILITY_ID ability_id)
{
	switch (ability_id)
	{
	case A_RESEARCH_SHIELDS:
		switch (shields)
		{
		case 0:
			return U_SHIELDS_1;
		case 1:
			return U_SHIELDS_2;
		case 2:
			return U_SHIELDS_3;
		default:
			return U_INVALID;
		}
	case A_RESEARCH_GROUND_WEAPONS:
		switch (ground_weapons)
		{
		case 0:
			return U_GROUND_WEAPONS_1;
		case 1:
			return U_GROUND_WEAPONS_2;
		case 2:
			return U_GROUND_WEAPONS_3;
		default:
			return U_INVALID;
		}
	case A_RESEARCH_GROUND_ARMOR:
		switch (ground_armor)
		{
		case 0:
			return U_GROUND_ARMOR_1;
		case 1:
			return U_GROUND_ARMOR_2;
		case 2:
			return U_GROUND_ARMOR_3;
		default:
			return U_INVALID;
		}
	case A_RESEARCH_AIR_WEAPONS:
		switch (air_weapons)
		{
		case 0:
			return U_AIR_WEAPONS_1;
		case 1:
			return U_AIR_WEAPONS_2;
		case 2:
			return U_AIR_WEAPONS_3;
		default:
			return U_INVALID;
		}
	case A_RESEARCH_AIR_ARMOR:
		switch (air_armor)
		{
		case 0:
			return U_AIR_ARMOR_1;
		case 1:
			return U_AIR_ARMOR_2;
		case 2:
			return U_AIR_ARMOR_3;
		default:
			return U_INVALID;
		}
	default:
		return U_INVALID;
	}
}


void UpgradeManager::StartUpgradeManager()
{
	automatic_upgrades = true;
}

void UpgradeManager::Run()
{
	if (automatic_upgrades == false)
		return;

	bool low_resources = false;
	for (auto& upgrade : required_upgrades)
	{
		UNIT_TYPEID structure_type = Utility::GetUpgradeStructure(upgrade);
		for (const auto& structure : mediator->GetUnits(Unit::Alliance::Self, IsFinishedUnit(structure_type)))
		{
			if (structure->orders.size() == 0)
			{
				TryActionResult result = mediator->TryResearchUpgrade(structure, upgrade);
				if (result == TryActionResult::cannot_afford)
					low_resources = true;
			}
		}
	}
	if (low_resources)
		return;

	std::vector<ABILITY_ID> forge_upgrades = {};
	if (mediator->GetEnemyRace() == Race::Zerg)
	{
		if (shields < 3)
			forge_upgrades.push_back(A_RESEARCH_SHIELDS);
		if (ground_weapons < 3)
			forge_upgrades.push_back(A_RESEARCH_GROUND_WEAPONS);
		if (ground_armor < 3)
			forge_upgrades.push_back(A_RESEARCH_GROUND_ARMOR);
	}
	else if (mediator->GetEnemyRace() == Race::Terran)
	{
		if (ground_armor < 3)
			forge_upgrades.push_back(A_RESEARCH_GROUND_ARMOR);
		if (ground_weapons < 3)
			forge_upgrades.push_back(A_RESEARCH_GROUND_WEAPONS);
		if (shields < 3)
			forge_upgrades.push_back(A_RESEARCH_SHIELDS);
	}
	else
	{
		if (ground_weapons < 3)
			forge_upgrades.push_back(A_RESEARCH_GROUND_WEAPONS);
		if (ground_armor < 3)
			forge_upgrades.push_back(A_RESEARCH_GROUND_ARMOR);
		if (shields < 3)
			forge_upgrades.push_back(A_RESEARCH_SHIELDS);
	}

	Units idle_forges;
	for (const auto& forge : mediator->GetUnits(IsFinishedUnit(FORGE)))
	{
		if (forge->orders.size() > 0)
			forge_upgrades.erase(std::remove(forge_upgrades.begin(), forge_upgrades.end(), forge->orders[0].ability_id), forge_upgrades.end());
		else
			idle_forges.push_back(forge);
	}
	if (idle_forges.size() > 0 && forge_upgrades.size() > 0)
	{
		for (const auto& forge : idle_forges)
		{
			mediator->TryResearchUpgrade(forge, AbilityIDToUpgradeID(forge_upgrades[0]));
			forge_upgrades.erase(forge_upgrades.begin());
			if (forge_upgrades.size() == 0)
				break;
		}
	}

	if (mediator->GetStargateProduction() == UNIT_TYPEID::INVALID)
		return;

	std::vector<ABILITY_ID> air_upgrades = {};
	if (mediator->GetUpgradeLevel(UpgradeType::air_weapons) < 3)
		air_upgrades.push_back(A_RESEARCH_AIR_WEAPONS);
	if (mediator->GetUpgradeLevel(UpgradeType::air_armor) < 3)
		air_upgrades.push_back(A_RESEARCH_AIR_ARMOR);

	Units idle_cybers;
	for (const auto& cyber : mediator->GetUnits(IsFinishedUnit(CYBERCORE)))
	{
		if (cyber->orders.size() > 0)
			air_upgrades.erase(std::remove(air_upgrades.begin(), air_upgrades.end(), cyber->orders[0].ability_id), air_upgrades.end());
		else
			idle_cybers.push_back(cyber);
	}
	if (idle_cybers.size() > 0 && air_upgrades.size() > 0)
	{
		for (const auto& cyber : idle_cybers)
		{
			mediator->TryResearchUpgrade(cyber, AbilityIDToUpgradeID(air_upgrades[0]));
			air_upgrades.erase(air_upgrades.begin());
			if (air_upgrades.size() == 0)
				break;
		}
	}
}

void UpgradeManager::AddRequiredUpgrade(UPGRADE_ID upgrade_id)
{
	if (std::find(finished_upgrades.begin(), finished_upgrades.end(), upgrade_id) == finished_upgrades.end() &&
		std::find(required_upgrades.begin(), required_upgrades.end(), upgrade_id) == required_upgrades.end())
		required_upgrades.push_back(upgrade_id);
}

int UpgradeManager::GetUpgradeLevel(UpgradeType upgrade_type) const
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
	required_upgrades.erase(std::remove(required_upgrades.begin(), required_upgrades.end(), upgrade), required_upgrades.end());

	switch (upgrade)
	{
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
		finished_upgrades.push_back(upgrade);
		break;
	}
}

bool UpgradeManager::CheckUpgrade(UPGRADE_ID upgrade_id) const
{
	switch (upgrade_id)
	{
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
		return std::find(finished_upgrades.begin(), finished_upgrades.end(), upgrade_id) != finished_upgrades.end();
	}
}

}