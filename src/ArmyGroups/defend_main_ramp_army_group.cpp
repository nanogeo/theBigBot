
#include "defend_main_ramp_army_group.h"
#include "mediator.h"
#include "definitions.h"

namespace sc2 {


DefendMainRampArmyGroup::DefendMainRampArmyGroup(Mediator* mediator, Point2D ramp_top, Point2D forcefield_pos) : ArmyGroup(mediator)
{
	unit_types = { ZEALOT, ADEPT, STALKER, SENTRY };

	this->ramp_top = ramp_top;
	this->forcefield_pos = forcefield_pos;
	walloff_pos = mediator->GetWallOffLocation(GATEWAY);
}

void DefendMainRampArmyGroup::Run()
{
	Units enemy_units = mediator->GetUnits(Unit::Alliance::Enemy);

	// if supply > x, kill walloff gateway and move attack
	if (all_units.size() > 12 && 
		(enemy_units.size() == 0 ||
		mediator->JudgeFight(all_units, enemy_units, 0, 0, false) > 50 || 
		Utility::DistanceToClosest(enemy_units, forcefield_pos) > 3))
	{
		const Unit* walloff_gate = Utility::ClosestUnitTo(mediator->GetUnits(Unit::Alliance::Self, IsUnits({ GATEWAY, WARP_GATE })), walloff_pos);
		if (walloff_gate && Distance2D(walloff_pos, walloff_gate->pos) < 1)
		{
			for (const auto& unit : all_units)
			{
				if (Distance2D(unit->pos, walloff_pos) > 3)
					mediator->SetUnitCommand(unit, A_MOVE, walloff_gate, 0);
				else
					mediator->SetUnitCommand(unit, A_ATTACK, walloff_gate, 0);
			}
		}
		else
		{
			mediator->CreateAttack({ ADEPT, STALKER, SENTRY }, 12, 20, 4, 4);
			mediator->MarkArmyGroupForDeletion(this);
			mediator->SetUnitsCommand(all_units, A_ATTACK, mediator->GetEnemyStartLocation(), 1);
		}
		return;
	}

	Units enemies_in_base = mediator->GetUnits(Unit::Alliance::Enemy);
	for (auto itr = enemies_in_base.begin(); itr != enemies_in_base.end();)
	{
		if (mediator->OnSameLevel((*itr)->pos, mediator->GetStartLocation()) == false || 
			Distance2D((*itr)->pos, ramp_top) < 3 || 
			Distance2D((*itr)->pos, mediator->GetStartLocation()) > 30)
			itr = enemies_in_base.erase(itr);
		else
			itr++;
	}

	// defend ramp
	for (const auto& unit : all_units)
	{
		if (mediator->GetAttackStatus(unit))
			continue;
		if (unit->weapon_cooldown == 0)
		{
			mediator->AddUnitToAttackers(unit);
			if (enemies_in_base.size() > 0)
			{
				const Unit* closest = Utility::ClosestTo(enemies_in_base, unit->pos);
				mediator->SetUnitCommand(unit, A_MOVE, closest->pos, 0);
			}
			else
			{
				mediator->SetUnitCommand(unit, A_MOVE, ramp_top, 0);
			}
		}
		else
		{
			if (enemies_in_base.size() > 0)
			{
				const Unit* closest = Utility::ClosestTo(enemies_in_base, unit->pos);
				mediator->SetUnitCommand(unit, A_MOVE, closest->pos, 0);
			}
			else
			{
				// if enemies get too close, kite away individually
				const Unit* closest_enemy = Utility::ClosestTo(enemy_units, unit->pos);
				float enemy_range = Utility::RealRange(closest_enemy, unit);
				float unit_range = Utility::RealRange(unit, closest_enemy);
				if ((enemy_range < unit_range || unit->shield / unit->shield_max < .5) && Distance2D(closest_enemy->pos, unit->pos) <= enemy_range)
					mediator->SetUnitCommand(unit, A_MOVE, Utility::PointBetween(unit->pos, mediator->GetStartLocation(), unit_range), 1);
				else if (Distance2D(unit->pos, ramp_top) > 6) // move units close to ramp
					mediator->SetUnitCommand(unit, A_MOVE, ramp_top, 0);
			}
		}

	}

	// forcefield if necessary
	if (Utility::DistanceToClosest(mediator->GetUnits(IsUnit(FORCEFIELD)), forcefield_pos) > .1 &&
		enemy_units.size() > 2 &&
		Distance2D(Utility::NthClosestTo(enemy_units, forcefield_pos, 2)->pos, forcefield_pos) < 3)
	{
		Units available_sentries;
		bool forcefield_cast = false;
		for (const auto& sentry : sentries)
		{
			if (sentry->energy >= 50)
			{
				if (Distance2D(sentry->pos, forcefield_pos) < 9)
				{
					mediator->SetUnitCommand(sentry, A_FORCEFIELD, forcefield_pos, 10);
					forcefield_cast = true;
					break;
				}
				else
				{
					available_sentries.push_back(sentry);
				}
			}
		}
		if (forcefield_cast == false && available_sentries.size() > 0)
		{
			mediator->SetUnitCommand(Utility::ClosestTo(available_sentries, forcefield_pos), A_FORCEFIELD, forcefield_pos, 10);
		}
	}
}

void DefendMainRampArmyGroup::AddNewUnit(const Unit* unit)
{
	AddUnit(unit);
}

void DefendMainRampArmyGroup::AddUnit(const Unit* unit)
{
	if (unit->unit_type == SENTRY)
	{
		if (std::find(sentries.begin(), sentries.end(), unit) == sentries.end())
			sentries.push_back(unit);
	}
	ArmyGroup::AddUnit(unit);
}

void DefendMainRampArmyGroup::RemoveUnit(const Unit* unit)
{
	sentries.erase(std::remove(sentries.begin(), sentries.end(), unit), sentries.end());
	ArmyGroup::RemoveUnit(unit);
}


}