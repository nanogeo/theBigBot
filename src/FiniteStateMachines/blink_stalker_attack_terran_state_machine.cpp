
#include <iostream>

#include "blink_stalker_attack_terran_state_machine.h"
#include "army_group.h"
#include "theBigBot.h"


namespace sc2 {

	// attacking_stalkers
	// standby_stalkers
	// moving_to_standby_stalkers
	// prism


#pragma region BlinkStalkerAttackTerranMoveAcross

void BlinkStalkerAttackTerranMoveAcross::TickState()
{
	
}

void BlinkStalkerAttackTerranMoveAcross::EnterState()
{
	return;
}

void BlinkStalkerAttackTerranMoveAcross::ExitState()
{
	state_machine->attached_army_group->SetUseStandby(true);
}

State* BlinkStalkerAttackTerranMoveAcross::TestTransitions()
{
	if (state_machine->attacking_stalkers.size() == 0)
	{
		while (state_machine->moving_to_standby_stalkers.size() > 0)
		{
			state_machine->attacking_stalkers.push_back(state_machine->moving_to_standby_stalkers.back());
			state_machine->moving_to_standby_stalkers.pop_back();
		}
		while (state_machine->standby_stalkers.size() > 0)
		{
			state_machine->attacking_stalkers.push_back(state_machine->standby_stalkers.back());
			state_machine->standby_stalkers.pop_back();
		}
	}

	// standby_stalkers
	// moving_to_standby_stalkers
	for (int i = 0; i < state_machine->moving_to_standby_stalkers.size(); i++)
	{
		const Unit* stalker = state_machine->moving_to_standby_stalkers[i];
		if (Distance2D(stalker->pos, Utility::MedianCenter(state_machine->attacking_stalkers)) < 3)
		{
			state_machine->attacking_stalkers.push_back(stalker);
			state_machine->moving_to_standby_stalkers.erase(state_machine->moving_to_standby_stalkers.begin() + i);
			i--;
		}
		else
		{
			if (mediator->GetAttackStatus(stalker) == false)
			{
				mediator->AddUnitToAttackers(stalker);
				mediator->SetUnitCommand(stalker, A_MOVE, Utility::MedianCenter(state_machine->attacking_stalkers), CommandPriorty::low);
			}
		}
	}

	// attacking_stalkers
	// prism
	AttackLineResult result = state_machine->attached_army_group->AttackLine(state_machine->attacking_stalkers, state_machine->prism);
	if (result != AttackLineResult::normal ||
		state_machine->attached_army_group->GetAttackPath().GetFurthestForward({state_machine->attached_army_group->GetConcaveOrigin(),
			state_machine->attached_army_group->GetAttackPath().FindClosestPoint(state_machine->consolidation_pos)}) == state_machine->attached_army_group->GetConcaveOrigin())
	{
		return new BlinkStalkerAttackTerranConsolidate(mediator, state_machine);
	}

	if (Utility::DistanceToFurthest(state_machine->moving_to_standby_stalkers, state_machine->consolidation_pos) < 4 &&
		(state_machine->prism == nullptr || Distance2D(state_machine->prism->pos, state_machine->prism_consolidation_pos) < 2))
	{
		return new BlinkStalkerAttackTerranConsolidate(mediator, state_machine);
	}
	return nullptr;
}

std::string BlinkStalkerAttackTerranMoveAcross::toString() const
{
	return "move across";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranConsolidate

// prism
void BlinkStalkerAttackTerranConsolidate::TickState()
{
	if(state_machine->prism && Distance2D(state_machine->prism->pos, state_machine->prism_consolidation_pos) > 3)
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->prism_consolidation_pos, CommandPriorty::low);

	// standby_stalkers
	// moving_to_standby_stalkers
	state_machine->GroupUpStandbyStalkers();
	// attacking_stalkers empty
}

void BlinkStalkerAttackTerranConsolidate::EnterState()
{
	while (state_machine->attacking_stalkers.size() > 0)
	{
		state_machine->moving_to_standby_stalkers.push_back(state_machine->attacking_stalkers.back());
		state_machine->attacking_stalkers.pop_back();
	}

	for (auto itr = state_machine->moving_to_standby_stalkers.begin(); itr != state_machine->moving_to_standby_stalkers.end();)
	{
		if (Distance2D(state_machine->consolidation_pos, (*itr)->pos) <= 5)
		{
			state_machine->standby_stalkers.push_back((*itr));
			itr = state_machine->moving_to_standby_stalkers.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void BlinkStalkerAttackTerranConsolidate::ExitState()
{
	for (auto itr = state_machine->standby_stalkers.begin(); itr != state_machine->standby_stalkers.end();)
	{
		if ((*itr)->shield >= 70)
		{
			state_machine->attacking_stalkers.push_back((*itr));
			itr = state_machine->standby_stalkers.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	for (auto itr = state_machine->moving_to_standby_stalkers.begin(); itr != state_machine->moving_to_standby_stalkers.end();)
	{
		if ((*itr)->shield >= 70 && Distance2D((*itr)->pos, state_machine->consolidation_pos) < MEDIUM_RANGE)
		{
			state_machine->attacking_stalkers.push_back((*itr));
			itr = state_machine->moving_to_standby_stalkers.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

State* BlinkStalkerAttackTerranConsolidate::TestTransitions()
{
	if (Utility::DistanceToClosest(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), state_machine->consolidation_pos) < 12)
	{
		state_machine->attack_location = BlinkAttackLocation::natural_defensive;
		while (state_machine->moving_to_standby_stalkers.size() > 0)
		{
			state_machine->attacking_stalkers.push_back(state_machine->moving_to_standby_stalkers.back());
			state_machine->moving_to_standby_stalkers.pop_back();
		}
		while (state_machine->standby_stalkers.size() > 0)
		{
			state_machine->attacking_stalkers.push_back(state_machine->standby_stalkers.back());
			state_machine->standby_stalkers.pop_back();
		}
		return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
	}

	if (state_machine->standby_stalkers.size() == 0 ||
		(float)state_machine->moving_to_standby_stalkers.size() / (float)state_machine->standby_stalkers.size() > .5 || // TODO figure out a better way
		(state_machine->prism && state_machine->prism->unit_type != PRISM) || // probably doesnt need to be checked anymore
		state_machine->standby_stalkers.size() < 8) // TODO totally arbitrary number of stalkers, should be based on game state/enemy supply
		return nullptr;
	
	float stalkers_healthy = 0;
	for (const auto& stalker : state_machine->standby_stalkers) // TODO change to enough stalkers not all
	{
		if (stalker->shield >= 70)
		{
			stalkers_healthy += 1;
		}
	}
	if (stalkers_healthy / state_machine->standby_stalkers.size() > .5) // another arbitrary amount of stalkers
	{
		// attack enemy 3rd bases if they have one
		Units enemy_bases = mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ COMMAND_CENTER, ORBITAL, PLANETARY }));

		for (int i = 0; i < (int)mediator->GetPossibleEnemyThirdBaseLocations().size(); i++)
		{
			Point2D possible_third = mediator->GetPossibleEnemyThirdBaseLocations()[i];
			if (Utility::DistanceToClosest(enemy_bases, possible_third) < VERY_CLOSE_RANGE)
			{
				if (i == 0)
					state_machine->attack_location = BlinkAttackLocation::third_1;
				else
					state_machine->attack_location = BlinkAttackLocation::third_2;
				return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
			}
		}

		// acropolis and torches do not have a blink up spot
		if (mediator->GetMapName() == "Acropolis AIE" || mediator->GetMapName() == "Torches AIE" || state_machine->prism == nullptr || state_machine->prism->is_alive == false)
		{
			state_machine->attack_location = BlinkAttackLocation::natural;
			return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
		}
			
		float danger_nat = mediator->GetLineDangerLevel(mediator->GetLocations().blink_nat_attack_path_line);
		float danger_main_1 = mediator->GetLineDangerLevel(mediator->GetLocations().blink_main_attack_path_lines[0]);
		float danger_main_2 = mediator->GetLineDangerLevel(mediator->GetLocations().blink_main_attack_path_lines[1]);
			
		if (danger_nat <= danger_main_1 && danger_nat <= danger_main_2)
		{
			state_machine->attack_location = BlinkAttackLocation::natural;
			return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
		}
		else
		{
			state_machine->attack_location = BlinkAttackLocation::main;
			return new BlinkStalkerAttackTerranBlinkUp(mediator, state_machine);
		}
	}
	
	return nullptr;
}

std::string BlinkStalkerAttackTerranConsolidate::toString() const
{
	return "consolidate";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranBlinkUp

void BlinkStalkerAttackTerranBlinkUp::TickState()
{
	// prism
	if (state_machine->prism != nullptr)
	{
		mediator->ForceUnitCommand(state_machine->prism, A_UNLOAD_AT, state_machine->prism);
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->blink_down_pos, CommandPriorty::low);
	}
	
	// attacking_stalkers
	for (auto itr = stalkers_to_blink.begin(); itr != stalkers_to_blink.end();)
	{
		if ((*itr) == nullptr || (*itr)->is_alive == false)
		{
			itr = stalkers_to_blink.erase(itr);
			continue;
		}
		if (Utility::OnSameLevel(mediator->ToPoint3D((*itr)->pos), mediator->ToPoint3D(state_machine->blink_down_pos)))
		{
			itr = stalkers_to_blink.erase(itr);
			continue;
		}
		if (Distance2D((*itr)->pos, state_machine->blink_up_pos) < 2)
		{
			mediator->SetUnitCommand((*itr), A_BLINK, state_machine->blink_down_pos, CommandPriorty::high);
		}
		else
		{
			mediator->SetUnitCommand((*itr), A_MOVE, state_machine->blink_up_pos, CommandPriorty::low);
		}
		itr++;
	}

	// standby_stalkers
	// moving_to_standby_stalkers
	state_machine->GroupUpStandbyStalkers();
}

void BlinkStalkerAttackTerranBlinkUp::EnterState()
{
	if (state_machine->prism != nullptr)
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->blink_down_pos, CommandPriorty::low);

	mediator->SetUnitsCommand(state_machine->attacking_stalkers, A_MOVE, state_machine->blink_up_pos, CommandPriorty::low);
	stalkers_to_blink = state_machine->attacking_stalkers;
}

void BlinkStalkerAttackTerranBlinkUp::ExitState()
{
	
}

State* BlinkStalkerAttackTerranBlinkUp::TestTransitions()
{
	if (stalkers_to_blink.size() == state_machine->attacking_stalkers.size() && 
		Utility::DistanceToClosest(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), Utility::MedianCenter(stalkers_to_blink)) < MEDIUM_RANGE)
	{
		state_machine->attack_location = BlinkAttackLocation::natural;
		return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
	}
	if (stalkers_to_blink.size() == 0)
	{
		state_machine->attached_army_group->SetStandbyPos(state_machine->blink_down_pos); // probably not necessary
		return new BlinkStalkerAttackTerranAttackMain(mediator, state_machine);
	}
	return nullptr;
}

std::string BlinkStalkerAttackTerranBlinkUp::toString() const
{
	return "blink up";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranAttackMain

void BlinkStalkerAttackTerranAttackMain::TickState()
{
	// prism
	if (state_machine->prism != nullptr)
	{
		mediator->ForceUnitCommand(state_machine->prism, A_UNLOAD_AT, state_machine->prism);
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->blink_up_pos, CommandPriorty::normal);
	}

	// moving_to_standby_stalkers
	for (auto itr = state_machine->moving_to_standby_stalkers.begin(); itr != state_machine->moving_to_standby_stalkers.end();)
	{
		if (Utility::OnSameLevel(mediator->ToPoint3D((*itr)->pos), mediator->ToPoint3D(state_machine->blink_down_pos)))
		{
			if (Distance2D((*itr)->pos, state_machine->blink_down_pos) < VERY_CLOSE_RANGE)
			{
				if (mediator->IsStalkerBlinkOffCooldown((*itr)) || state_machine->prism == nullptr)
					mediator->SetUnitCommand((*itr), A_BLINK, state_machine->blink_up_pos, CommandPriorty::high);
				else
					mediator->SetUnitCommand((*itr), A_SMART, state_machine->prism, CommandPriorty::normal);
			}
			else
			{
				mediator->SetUnitCommand((*itr), A_MOVE, state_machine->blink_down_pos, CommandPriorty::low);
			}
			itr++;
		}
		else
		{
			if (Distance2D((*itr)->pos, state_machine->consolidation_pos) < 5)
			{
				state_machine->standby_stalkers.push_back((*itr));
				itr = state_machine->moving_to_standby_stalkers.erase(itr);
			}
			else
			{
				mediator->SetUnitCommand((*itr), A_MOVE, state_machine->consolidation_pos, CommandPriorty::low);
				itr++;
			}
		}
	}

	// standby_stalkers
	for (const auto& stalker : state_machine->standby_stalkers)
	{
		if (stalker->weapon_cooldown > 0)
		{
			mediator->SetUnitCommand(stalker, A_MOVE, state_machine->consolidation_pos, CommandPriorty::low);
		}
		else if (mediator->GetAttackStatus(stalker) == false)
		{
			mediator->AddUnitToAttackers(stalker);
			mediator->SetUnitCommand(stalker, A_MOVE, state_machine->consolidation_pos, CommandPriorty::low);
		}
	}
}

void BlinkStalkerAttackTerranAttackMain::EnterState()
{
	float danger_main_1 = mediator->GetLineDangerLevel(mediator->GetLocations().blink_main_attack_path_lines[0]);
	float danger_main_2 = mediator->GetLineDangerLevel(mediator->GetLocations().blink_main_attack_path_lines[1]);

	if (danger_main_1 > danger_main_2)
		state_machine->attached_army_group->SetAttackPath(mediator->GetLocations().blink_main_attack_path_lines[0]);
	else
		state_machine->attached_army_group->SetAttackPath(mediator->GetLocations().blink_main_attack_path_lines[1]);

	state_machine->attached_army_group->ResetConcaveOrigin();
}

void BlinkStalkerAttackTerranAttackMain::ExitState()
{
	state_machine->attached_army_group->ResetConcaveOrigin();
}

State* BlinkStalkerAttackTerranAttackMain::TestTransitions()
{
	// TODO check if its a good idea to blink forward and snipe a unit
	for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ SIEGE_TANK, SIEGE_TANK_SIEGED, MEDIVAC, BATTLECRUISER }))) // TODO move units types to list that is const or can be chenaged
	{
		Units stalkers_in_range;
		for (const auto& stalker : state_machine->attacking_stalkers)
		{
			if (stalker->weapon_cooldown > 0 || mediator->GetAttackStatus(stalker) == true)
				continue;

			if ((mediator->IsStalkerBlinkOffCooldown(stalker) && Distance2D(stalker->pos, unit->pos) < Utility::RealRange(stalker, unit) + 7) ||
				(Distance2D(stalker->pos, unit->pos) < Utility::RealRange(stalker, unit)))
			{
				stalkers_in_range.push_back(stalker);
			}
		}
		if (stalkers_in_range.size() == 0)
			continue;
		if (stalkers_in_range.size() * Utility::GetDamage(stalkers_in_range[0], unit) >= unit->health) // TODO check if theres a lack of other units first and check for the best unit to snipe
		{
			return new BlinkStalkerAttackTerranSnipeUnit(mediator, state_machine, unit);
		}
	}
	// attacking_stalkers
	AttackLineResult result = state_machine->attached_army_group->AttackLine(state_machine->attacking_stalkers);
	if (result != AttackLineResult::normal ||
		state_machine->standby_stalkers.size() > state_machine->attacking_stalkers.size())
	{
		return new BlinkStalkerAttackTerranLeaveHighground(mediator, state_machine);
	}

	int stalkers_ok = 0;
	for (const auto& stalker : state_machine->attacking_stalkers)
	{
		if (stalker->shield > 20)
			stalkers_ok++;
	}
	if (stalkers_ok < 6) // very arbitrary numbers again
	{
		return new BlinkStalkerAttackTerranLeaveHighground(mediator, state_machine);
	}

	return nullptr;
}

std::string BlinkStalkerAttackTerranAttackMain::toString() const
{
	return "attack main";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranAttack

void BlinkStalkerAttackTerranAttack::TickState()
{
	if (state_machine->attack_location == BlinkAttackLocation::natural_defensive)
	{
		Point2D stalkers_center = Utility::MedianCenter(state_machine->attacking_stalkers);
		Point2D stalker_line_pos = state_machine->attached_army_group->GetAttackPath().FindClosestPoint(stalkers_center);

		Point2D new_consolidation_pos = state_machine->attached_army_group->GetAttackPath().GetPointFrom(stalker_line_pos, 6, false); // TODO adjust distance
		state_machine->SetConsolidationPos(new_consolidation_pos);
	}

	// standby_stalkers
	// moving_to_standby_stalkers
	state_machine->GroupUpStandbyStalkers();
}

void BlinkStalkerAttackTerranAttack::EnterState()
{
	if (state_machine->attack_location == BlinkAttackLocation::natural || state_machine->attack_location == BlinkAttackLocation::natural_defensive)
	{
		state_machine->attached_army_group->SetAttackPath(mediator->GetLocations().blink_nat_attack_path_line);
	}
	else if (state_machine->attack_location == BlinkAttackLocation::third_1)
	{
		state_machine->attached_army_group->SetAttackPath(mediator->GetLocations().blink_third_attack_path_lines[0]);
	}
	else if (state_machine->attack_location == BlinkAttackLocation::third_2)
	{
		state_machine->attached_army_group->SetAttackPath(mediator->GetLocations().blink_third_attack_path_lines[1]);
	}
}

void BlinkStalkerAttackTerranAttack::ExitState()
{
	state_machine->ResetConsolidationPos();
}

State* BlinkStalkerAttackTerranAttack::TestTransitions()
{
	for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ SIEGE_TANK, SIEGE_TANK_SIEGED, MEDIVAC, BATTLECRUISER })))
	{
		Units stalkers_in_range;
		for (const auto& stalker : state_machine->attacking_stalkers)
		{
			if (stalker->weapon_cooldown > 0 || mediator->GetAttackStatus(stalker) == true)
				continue;

			if ((mediator->IsStalkerBlinkOffCooldown(stalker) && Distance2D(stalker->pos, unit->pos) < Utility::RealRange(stalker, unit) + 7) ||
				(Distance2D(stalker->pos, unit->pos) < Utility::RealRange(stalker, unit)))
			{
				stalkers_in_range.push_back(stalker);
			}
		}
		if (stalkers_in_range.size() == 0)
			continue;
		if (stalkers_in_range.size() * Utility::GetDamage(stalkers_in_range[0], unit) >= unit->health)
		{
			return new BlinkStalkerAttackTerranSnipeUnit(mediator, state_machine, unit);
		}
	}
	// prism
	// attacking_stalkers
	AttackLineResult result = AttackLineResult::normal;
	if (state_machine->prism == nullptr)
		result = state_machine->attached_army_group->AttackLine(state_machine->attacking_stalkers);
	else
		result = state_machine->attached_army_group->AttackLine(state_machine->attacking_stalkers, state_machine->prism);
	if (result != AttackLineResult::normal || 
		state_machine->standby_stalkers.size() > state_machine->attacking_stalkers.size())
	{
		state_machine->attached_army_group->ResetConcaveOrigin();
		
		return new BlinkStalkerAttackTerranConsolidate(mediator, state_machine);
	}

	int stalkers_ok = 0;
	for (const auto &stalker : state_machine->attacking_stalkers)
	{
		if (stalker->shield > 20)
			stalkers_ok++;
	}
	if (stalkers_ok < 6)
	{
		if (Utility::DistanceToClosest(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), state_machine->consolidation_pos) >= 12) // TODO what is this check for
		{
			state_machine->attached_army_group->ResetConcaveOrigin();
			return new BlinkStalkerAttackTerranConsolidate(mediator, state_machine);
		}
	}

	return nullptr;
}

std::string BlinkStalkerAttackTerranAttack::toString() const
{
	return "attack";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranSnipeUnit

void BlinkStalkerAttackTerranSnipeUnit::TickState()
{

}

void BlinkStalkerAttackTerranSnipeUnit::EnterState()
{
	enter_time = mediator->GetCurrentTime(); 
	// attacking stalkers
	for (const auto& stalker : state_machine->attacking_stalkers)
	{
		if (stalker->weapon_cooldown == 0 && mediator->GetAttackStatus(stalker) == false)
		{
			if (mediator->IsStalkerBlinkOffCooldown(stalker))
			{
				mediator->SetUnitCommand(stalker, A_BLINK, target->pos, CommandPriorty::high);
				mediator->SetUnitCommand(stalker, A_ATTACK, target, CommandPriorty::high, true);
			}
			else
			{
				mediator->SetUnitCommand(stalker, A_ATTACK, target, CommandPriorty::high);
			}
		}
	}
	// prism, standby_stalkers, and moving_to_standby_stalkers 
	// ignored to make it simpler and this state can only exist for a max of .5 seconds
}

void BlinkStalkerAttackTerranSnipeUnit::ExitState()
{

}

State* BlinkStalkerAttackTerranSnipeUnit::TestTransitions()
{
	if (target == nullptr ||
		target->is_alive == false ||
		target->display_type != Unit::DisplayType::Visible ||
		mediator->GetCurrentTime() > enter_time + .5)
	{
		if (state_machine->attack_location == BlinkAttackLocation::main)
			return new BlinkStalkerAttackTerranAttackMain(mediator, state_machine);
		else
			return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
	}
	return nullptr;
}

std::string BlinkStalkerAttackTerranSnipeUnit::toString() const
{
	return "snipe unit";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranLeaveHighground

void BlinkStalkerAttackTerranLeaveHighground::TickState()
{
	// prism
	if (state_machine->prism != nullptr)
	{
		mediator->ForceUnitCommand(state_machine->prism, A_UNLOAD_AT, state_machine->prism);
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->blink_up_pos, CommandPriorty::low);
	}

	// moving_to_standby_stalkers
	for (auto itr = state_machine->moving_to_standby_stalkers.begin(); itr != state_machine->moving_to_standby_stalkers.end();)
	{
		if (Utility::OnSameLevel(mediator->ToPoint3D((*itr)->pos), mediator->ToPoint3D(state_machine->blink_down_pos)))
		{
			if (Distance2D((*itr)->pos, state_machine->blink_down_pos) < VERY_CLOSE_RANGE)
			{
				if (mediator->IsStalkerBlinkOffCooldown((*itr)) || state_machine->prism == nullptr)
					mediator->SetUnitCommand((*itr), A_BLINK, state_machine->blink_up_pos, CommandPriorty::high);
				else
					mediator->SetUnitCommand((*itr), A_SMART, state_machine->prism, CommandPriorty::normal);
			}
			else
			{
				mediator->SetUnitCommand((*itr), A_MOVE, state_machine->blink_down_pos, CommandPriorty::low);
			}
			itr++;
		}
		else
		{
			if (Distance2D((*itr)->pos, state_machine->consolidation_pos) < 5)
			{
				state_machine->standby_stalkers.push_back((*itr));
				itr = state_machine->moving_to_standby_stalkers.erase(itr);
			}
			else
			{
				mediator->SetUnitCommand((*itr), A_MOVE, state_machine->consolidation_pos, CommandPriorty::low);
				itr++;
			}
		}
	}

	// standby_stalkers
	for (const auto& stalker : state_machine->standby_stalkers)
	{
		if (stalker->weapon_cooldown > 0)
		{
			mediator->SetUnitCommand(stalker, A_MOVE, state_machine->consolidation_pos, CommandPriorty::low);
		}
		else if (mediator->GetAttackStatus(stalker) == false)
		{
			mediator->AddUnitToAttackers(stalker);
			mediator->SetUnitCommand(stalker, A_MOVE, state_machine->consolidation_pos, CommandPriorty::low);
		}
	}
}

void BlinkStalkerAttackTerranLeaveHighground::EnterState()
{
	// attacking_stalkers
	while (state_machine->attacking_stalkers.size() > 0)
	{
		state_machine->moving_to_standby_stalkers.push_back(state_machine->attacking_stalkers.back());
		state_machine->attacking_stalkers.pop_back();
	}
}

void BlinkStalkerAttackTerranLeaveHighground::ExitState()
{
	
}

State* BlinkStalkerAttackTerranLeaveHighground::TestTransitions()
{
	for (const auto& stalker : state_machine->moving_to_standby_stalkers)
	{
		if (mediator->OnSameLevel(mediator->ToPoint3D(stalker->pos), mediator->ToPoint3D(state_machine->blink_down_pos)))
		{
			if (state_machine->prism && std::find_if(state_machine->prism->passengers.begin(), state_machine->prism->passengers.end(), [stalker](const PassengerUnit& unit) { return unit.tag == stalker->tag; }) == state_machine->prism->passengers.end())
				return nullptr;
		}
	}
	return new BlinkStalkerAttackTerranConsolidate(mediator, state_machine);
}

std::string BlinkStalkerAttackTerranLeaveHighground::toString() const
{
	return "leave high ground";
}


#pragma endregion


#pragma region BlinkStalkerAttackTerran

void BlinkStalkerAttackTerran::GroupUpStandbyStalkers()
{
	for (int i = 0; i < moving_to_standby_stalkers.size(); i++)
	{
		const Unit* stalker = moving_to_standby_stalkers[i];
		if (mediator->GetAttackStatus(stalker))
			continue;

		if (Distance2D(stalker->pos, consolidation_pos) > 10 || stalker->weapon_cooldown > 0)
		{
			mediator->SetUnitCommand(stalker, A_MOVE, consolidation_pos, CommandPriorty::low);
		}
		else if (Distance2D(stalker->pos, consolidation_pos) > 5)
		{
			mediator->AddUnitToAttackers(stalker);
			mediator->SetUnitCommand(stalker, A_MOVE, consolidation_pos, CommandPriorty::low);
		}
		else
		{
			standby_stalkers.push_back(stalker);
			moving_to_standby_stalkers.erase(moving_to_standby_stalkers.begin() + i);
			i--;
		}
	}

	for (const auto& stalker : standby_stalkers)
	{
		if (stalker->weapon_cooldown > 0)
		{
			mediator->SetUnitCommand(stalker, A_MOVE, consolidation_pos, CommandPriorty::low);
		}
		else if (mediator->GetAttackStatus(stalker) == false)
		{
			mediator->AddUnitToAttackers(stalker);
			mediator->SetUnitCommand(stalker, A_MOVE, consolidation_pos, CommandPriorty::low);
		}
	}
}

BlinkStalkerAttackTerran::BlinkStalkerAttackTerran(Mediator* mediator, std::string name,
	Point2D consolidation_pos, Point2D prism_consolidation_pos, Point2D blink_up_pos, Point2D blink_down_pos) : StateMachine(mediator, name)
{
	this->consolidation_pos = consolidation_pos;
	this->default_consolidation_pos = consolidation_pos;
	this->prism_consolidation_pos = prism_consolidation_pos;
	this->blink_up_pos = blink_up_pos;
	this->blink_down_pos = blink_down_pos;
	current_state = new BlinkStalkerAttackTerranMoveAcross(mediator, this);

	current_state->EnterState();
}

void BlinkStalkerAttackTerran::RunStateMachine()
{
	StateMachine::RunStateMachine();
}

bool BlinkStalkerAttackTerran::AddUnit(const Unit* unit)
{
	if (unit->unit_type == STALKER)
		moving_to_standby_stalkers.push_back(unit);
	else if (unit->unit_type == PRISM && (prism == nullptr || prism->is_alive == false))
		prism = unit;
	else
		return false;
	return true;
}

void BlinkStalkerAttackTerran::RemoveUnit(const Unit* unit)
{
	if (unit == prism)
	{
		prism = nullptr;
		return;
	}
	attacking_stalkers.erase(std::remove(attacking_stalkers.begin(), attacking_stalkers.end(), unit), attacking_stalkers.end());
	standby_stalkers.erase(std::remove(standby_stalkers.begin(), standby_stalkers.end(), unit), standby_stalkers.end());
	moving_to_standby_stalkers.erase(std::remove(moving_to_standby_stalkers.begin(), moving_to_standby_stalkers.end(), unit), moving_to_standby_stalkers.end());
}

void BlinkStalkerAttackTerran::SetConsolidationPos(Point2D pos)
{
	consolidation_pos = pos;
}

void BlinkStalkerAttackTerran::ResetConsolidationPos()
{
	consolidation_pos = default_consolidation_pos;
}

void BlinkStalkerAttackTerran::SetAttachedArmyGroup(BlinkFSMArmyGroup* army_group)
{
	attached_army_group = army_group;
}

BlinkStalkerAttackTerran::~BlinkStalkerAttackTerran()
{
	
}


#pragma endregion

}