#pragma once

#include "mediator.h"
#include "defense_manager.h"
#include "definitions.h"
#include "utility.h"

#include "army_group.h"


namespace sc2 {


void DefenseManager::CheckForAttacks()
{
	for (const auto& base : mediator->GetUnits(IsUnit(NEXUS)))
	{
		Units close_enemies = Utility::GetUnitsWithin(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), base->pos, 20);
		ArmyGroup* defense_group = mediator->GetArmyGroupDefendingBase(base->pos);
		
		if (close_enemies.size() == 0 && std::find(ongoing_attacks.begin(), ongoing_attacks.end(), base->pos) != ongoing_attacks.end())
		{
			std::cerr << "Attack ended at " << base->pos.x << ", " << base->pos.y <<  " at " << mediator->GetCurrentTime() << std::endl;
			ongoing_attacks.erase(std::remove(ongoing_attacks.begin(), ongoing_attacks.end(), base->pos), ongoing_attacks.end());
		}
		else if (close_enemies.size() > 0 && std::find(ongoing_attacks.begin(), ongoing_attacks.end(), base->pos) == ongoing_attacks.end())
		{
			std::cerr << "Attack started at " << base->pos.x << ", " << base->pos.y << " at " << mediator->GetCurrentTime() << std::endl;
			ongoing_attacks.push_back(base->pos);
		}
	}
}

}