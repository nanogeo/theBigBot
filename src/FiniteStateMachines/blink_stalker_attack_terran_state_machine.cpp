
#include <iostream>

#include "blink_stalker_attack_terran_state_machine.h"
#include "army_group.h"
#include "theBigBot.h"


namespace sc2 {



#pragma region BlinkStalkerAttackTerranMoveAcross

void BlinkStalkerAttackTerranMoveAcross::TickState()
{
	//mediator->SetUnitCommand(state_machine->stalkers, A_ATTACK, state_machine->consolidation_pos, 0);
	//mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->prism_consolidation_pos, 0);
}

void BlinkStalkerAttackTerranMoveAcross::EnterState()
{
	return;
}

void BlinkStalkerAttackTerranMoveAcross::ExitState()
{
	return;
}

State* BlinkStalkerAttackTerranMoveAcross::TestTransitions()
{
	if (state_machine->attacking_stalkers.size() == 0)
	{
		while (state_machine->moving_to_standby_stalkers.size() > 0)
		{
			state_machine->attacking_stalkers.push_back(state_machine->moving_to_standby_stalkers[0]);
			state_machine->moving_to_standby_stalkers.erase(state_machine->moving_to_standby_stalkers.begin());
		}
	}
	else
	{
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
				mediator->SetUnitCommand(stalker, A_ATTACK, Utility::MedianCenter(state_machine->attacking_stalkers), CommandPriorty::low);
			}
		}
	}
	if (state_machine->attached_army_group->AttackLine() != AttackLineResult::normal ||
		state_machine->attached_army_group->GetAttackPath().GetFurthestForward({state_machine->attached_army_group->GetConcaveOrigin(),
			state_machine->attached_army_group->GetAttackPath().FindClosestPoint(state_machine->consolidation_pos)}) == state_machine->attached_army_group->GetConcaveOrigin())
	{
		if (state_machine->attached_army_group)
			state_machine->attached_army_group->SetUseStandby(true);
		return new BlinkStalkerAttackTerranConsolidate(mediator, state_machine);
	}

	if (Utility::DistanceToFurthest(state_machine->moving_to_standby_stalkers, state_machine->consolidation_pos) < 4 &&
		Distance2D(state_machine->prism->pos, state_machine->prism_consolidation_pos) < 2)
	{
		if (state_machine->attached_army_group)
			state_machine->attached_army_group->SetUseStandby(true);
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

void BlinkStalkerAttackTerranConsolidate::TickState()
{
	if(state_machine->prism && Distance2D(state_machine->prism->pos, state_machine->prism_consolidation_pos) > 3)
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->prism_consolidation_pos, CommandPriorty::low);

	for (int i = 0; i < state_machine->moving_to_standby_stalkers.size(); i++)
	{
		const Unit* stalker = state_machine->moving_to_standby_stalkers[i];

		if (Distance2D(stalker->pos, state_machine->consolidation_pos) > 10 || stalker->weapon_cooldown > 0)
		{
			mediator->SetUnitCommand(stalker, A_MOVE, state_machine->consolidation_pos, CommandPriorty::low);
		}
		else if (Distance2D(stalker->pos, state_machine->consolidation_pos) > 5)
		{
			mediator->SetUnitCommand(stalker, A_ATTACK, state_machine->consolidation_pos, CommandPriorty::low);
		}
		else if (Distance2D(stalker->pos, state_machine->consolidation_pos) <= 5)
		{
			state_machine->standby_stalkers.push_back(stalker);
			state_machine->moving_to_standby_stalkers.erase(state_machine->moving_to_standby_stalkers.begin() + i);
			i--;
		}
	}
}

void BlinkStalkerAttackTerranConsolidate::EnterState()
{
	while (state_machine->attacking_stalkers.size() > 0)
	{
		state_machine->moving_to_standby_stalkers.push_back(state_machine->attacking_stalkers[0]);
		state_machine->attacking_stalkers.erase(state_machine->attacking_stalkers.begin());
	}
	for ( int i = 0; i < state_machine->moving_to_standby_stalkers.size(); i++)
	{
		if (Distance2D(state_machine->consolidation_pos, state_machine->moving_to_standby_stalkers[i]->pos) <= 5)
		{
			state_machine->standby_stalkers.push_back(state_machine->moving_to_standby_stalkers[0]);
			state_machine->moving_to_standby_stalkers.erase(state_machine->moving_to_standby_stalkers.begin());
			i--;
		}
	}
}

void BlinkStalkerAttackTerranConsolidate::ExitState()
{
	while (state_machine->standby_stalkers.size() > 0)
	{
		state_machine->attacking_stalkers.push_back(state_machine->standby_stalkers[0]);
		state_machine->standby_stalkers.erase(state_machine->standby_stalkers.begin());
	}
}

State* BlinkStalkerAttackTerranConsolidate::TestTransitions()
{
	if ((float)state_machine->moving_to_standby_stalkers.size() / (float)state_machine->standby_stalkers.size() > .5)
		return nullptr;
	if (Utility::DistanceToClosest(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), state_machine->consolidation_pos) < 12)
	{
		state_machine->attack_location = BlinkAtackLocation::natural_defensive;
		return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
	}
	if (state_machine->prism->unit_type == PRISM)
	{
		if (state_machine->standby_stalkers.size() < 8)
			return nullptr;

		float stalkers_healthy = 0;
		for (const auto& stalker : state_machine->standby_stalkers) // TODO change to enough stalkers not all
		{
			if (stalker->shield >= 70)
			{
				stalkers_healthy += 1;
			}
		}
		if (stalkers_healthy / state_machine->standby_stalkers.size() > .5)
		{
			// attack enemy 3rd bases if they have one
			Units enemy_bases = mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ COMMAND_CENTER, ORBITAL, PLANETARY }));

			for (size_t i = 0; i < mediator->GetPossibleEnemyThirdBaseLocations().size(); i++)
			{
				Point2D possible_third = mediator->GetPossibleEnemyThirdBaseLocations()[i];
				if (Utility::DistanceToClosest(enemy_bases, possible_third) < 2)
				{
					if (i == 0)
						state_machine->attack_location = BlinkAtackLocation::third_1;
					else
						state_machine->attack_location = BlinkAtackLocation::third_2;
					return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
				}
			}

			// acropolis does not have a blink up spot
			if (mediator->GetMapName() == "Acropolis AIE" || state_machine->prism->is_alive == false)
			{
				state_machine->attack_location = BlinkAtackLocation::natural;
				return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
			}
			
			float danger_nat = mediator->GetLineDangerLevel(mediator->GetLocations().blink_nat_attack_path_line);
			float danger_main_1 = mediator->GetLineDangerLevel(mediator->GetLocations().blink_main_attack_path_lines[0]);
			float danger_main_2 = mediator->GetLineDangerLevel(mediator->GetLocations().blink_main_attack_path_lines[1]);
			
			if (danger_nat <= danger_main_1 && danger_nat <= danger_main_2)
			{
				state_machine->attack_location = BlinkAtackLocation::natural;
				return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
			}
			else
			{
				state_machine->attack_location = BlinkAtackLocation::main;
				return new BlinkStalkerAttackTerranBlinkUp(mediator, state_machine);
			}
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
	mediator->SetUnitCommand(state_machine->prism, A_UNLOAD_AT, state_machine->prism, CommandPriorty::low);
	mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->blink_down_pos, CommandPriorty::low, true);
	for (int i = 0; i < stalkers_to_blink.size(); i++)
	{
		if (stalkers_to_blink[i] == nullptr) // TODO look into why this is ever the case
		{
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
			continue;
		}
		if (Utility::OnSameLevel(mediator->ToPoint3D(stalkers_to_blink[i]->pos), mediator->ToPoint3D(state_machine->blink_down_pos)))
		{
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
			continue;
		}
		if (Distance2D(stalkers_to_blink[i]->pos, state_machine->blink_up_pos) < 2)
		{
			mediator->SetUnitCommand(stalkers_to_blink[i], A_BLINK, state_machine->blink_down_pos, CommandPriorty::high);
		}
		else
		{
			mediator->SetUnitCommand(stalkers_to_blink[i], A_MOVE, state_machine->blink_up_pos, CommandPriorty::low);
		}
	}
}

void BlinkStalkerAttackTerranBlinkUp::EnterState()
{
	mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->blink_down_pos, CommandPriorty::low);
	mediator->SetUnitsCommand(state_machine->attacking_stalkers, A_ATTACK, state_machine->blink_up_pos, CommandPriorty::low);
	stalkers_to_blink = state_machine->attacking_stalkers;
}

void BlinkStalkerAttackTerranBlinkUp::ExitState()
{
	
}

State* BlinkStalkerAttackTerranBlinkUp::TestTransitions()
{
	if (stalkers_to_blink.size() == state_machine->attacking_stalkers.size() && 
		Utility::DistanceToClosest(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), Utility::MedianCenter(stalkers_to_blink)) < 10)
	{
		state_machine->attack_location = BlinkAtackLocation::natural;
		return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
	}
	if (stalkers_to_blink.size() == 0)
	{
		state_machine->attached_army_group->SetStandbyPos(state_machine->blink_down_pos);
		return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
	}
	return nullptr;
}

std::string BlinkStalkerAttackTerranBlinkUp::toString() const
{
	return "blink up";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranAttack

void BlinkStalkerAttackTerranAttack::TickState()
{
	if (state_machine->attack_location == BlinkAtackLocation::main)
	{
		mediator->SetUnitCommand(state_machine->prism, A_UNLOAD_AT, state_machine->prism, CommandPriorty::normal);
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->blink_up_pos, CommandPriorty::normal, true);

		for (int i = 0; i < state_machine->moving_to_standby_stalkers.size(); i++)
		{
			const Unit* stalker = state_machine->moving_to_standby_stalkers[i];
			
			if (Utility::OnSameLevel(mediator->ToPoint3D(stalker->pos), mediator->ToPoint3D(state_machine->blink_down_pos)))
			{
				if (Distance2D(stalker->pos, state_machine->blink_down_pos) < 1)
				{
					if (mediator->IsStalkerBlinkOffCooldown(stalker))
						mediator->SetUnitCommand(stalker, A_BLINK, state_machine->blink_up_pos, CommandPriorty::high);
					else
						mediator->SetUnitCommand(stalker, A_SMART, state_machine->prism, CommandPriorty::low);
				}
				else
				{
					mediator->SetUnitCommand(stalker, A_MOVE, state_machine->blink_down_pos, CommandPriorty::low);
				}
			}
			else
			{
				if (Distance2D(stalker->pos, state_machine->consolidation_pos) < 5)
				{
					state_machine->standby_stalkers.push_back(stalker);
					state_machine->moving_to_standby_stalkers.erase(state_machine->moving_to_standby_stalkers.begin() + i);
					i--;
				}
				else
				{
					mediator->SetUnitCommand(stalker, A_MOVE, state_machine->consolidation_pos, CommandPriorty::low);
				}
			}
		}
	}
	else
	{
		if (state_machine->attack_location == BlinkAtackLocation::natural_defensive)
		{
			Point2D stalkers_center = Utility::MedianCenter(state_machine->attacking_stalkers);
			Point2D stalker_line_pos = state_machine->attached_army_group->GetAttackPath().FindClosestPoint(stalkers_center);

			Point2D new_consolidation_pos = state_machine->attached_army_group->GetAttackPath().GetPointFrom(stalker_line_pos, 6, false); // TODO adjust distance
			state_machine->SetConsolidationPos(new_consolidation_pos);
		}

		for (int i = 0; i < state_machine->moving_to_standby_stalkers.size(); i++)
		{
			const Unit* stalker = state_machine->moving_to_standby_stalkers[i];

			if (Distance2D(stalker->pos, state_machine->consolidation_pos) > 10 || stalker->weapon_cooldown > 0)
			{
				mediator->SetUnitCommand(stalker, A_MOVE, state_machine->consolidation_pos, CommandPriorty::low);
			}
			else if (Distance2D(stalker->pos, state_machine->consolidation_pos) > 5)
			{
				mediator->SetUnitCommand(stalker, A_ATTACK, state_machine->consolidation_pos, CommandPriorty::low);
			}
			else if (Distance2D(stalker->pos, state_machine->consolidation_pos) <= 5)
			{
				state_machine->standby_stalkers.push_back(stalker);
				state_machine->moving_to_standby_stalkers.erase(state_machine->moving_to_standby_stalkers.begin() + i);
				i--;
			}
		}
	}
}

void BlinkStalkerAttackTerranAttack::EnterState()
{
	if (state_machine->attack_location == BlinkAtackLocation::main)
	{
		float danger_main_1 = mediator->GetLineDangerLevel(mediator->GetLocations().blink_main_attack_path_lines[0]);
		float danger_main_2 = mediator->GetLineDangerLevel(mediator->GetLocations().blink_main_attack_path_lines[1]);

		if (danger_main_1 > danger_main_2)
			state_machine->attached_army_group->SetAttackPath(mediator->GetLocations().blink_main_attack_path_lines[0]);
		else
			state_machine->attached_army_group->SetAttackPath(mediator->GetLocations().blink_main_attack_path_lines[1]);
	}
	else if (state_machine->attack_location == BlinkAtackLocation::natural)
	{
		state_machine->attached_army_group->SetAttackPath(mediator->GetLocations().blink_nat_attack_path_line);
	}
	else if (state_machine->attack_location == BlinkAtackLocation::natural_defensive)
	{
		state_machine->attached_army_group->SetAttackPath(mediator->GetLocations().blink_nat_attack_path_line);
	}
	else if (state_machine->attack_location == BlinkAtackLocation::third_1)
	{
		state_machine->attached_army_group->SetAttackPath(mediator->GetLocations().blink_third_attack_path_lines[0]);
	}
	else if (state_machine->attack_location == BlinkAtackLocation::third_2)
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
			if (stalker->weapon_cooldown > 0)
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
	if (state_machine->attached_army_group->AttackLine() != AttackLineResult::normal || 
		state_machine->standby_stalkers.size() > state_machine->attacking_stalkers.size())
	{
		state_machine->attached_army_group->ResetConcaveOrigin();
		if (state_machine->attack_location == BlinkAtackLocation::main)
		{
			return new BlinkStalkerAttackTerranLeaveHighground(mediator, state_machine);
		}
		else
		{
			return new BlinkStalkerAttackTerranConsolidate(mediator, state_machine);
		}
	}

	int stalkers_ok = 0;
	for (const auto &stalker : state_machine->attacking_stalkers)
	{
		if (stalker->shield > 20)
			stalkers_ok++;
	}
	if (stalkers_ok < 6)
	{
		if (state_machine->attack_location == BlinkAtackLocation::main)
		{
			state_machine->attached_army_group->ResetConcaveOrigin();
			return new BlinkStalkerAttackTerranLeaveHighground(mediator, state_machine);
		}
		else
		{
			if (Utility::DistanceToClosest(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), state_machine->consolidation_pos) >= 12)
			{
				state_machine->attached_army_group->ResetConcaveOrigin();
				return new BlinkStalkerAttackTerranConsolidate(mediator, state_machine);
			}
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
	for (const auto& stalker : state_machine->attacking_stalkers)
	{
		if (stalker->weapon_cooldown == 0)
		{
			if (mediator->IsStalkerBlinkOffCooldown(stalker))
			{
				mediator->SetUnitCommand(stalker, A_BLINK, target->pos, CommandPriorty::high);
				mediator->SetUnitCommand(stalker, A_ATTACK, target, CommandPriorty::low, true);
			}
			else
			{
				mediator->SetUnitCommand(stalker, A_ATTACK, target, CommandPriorty::low);
			}
		}
	}
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
		return new BlinkStalkerAttackTerranAttack(mediator, state_machine);
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
	mediator->SetUnitCommand(state_machine->prism, A_UNLOAD_AT, state_machine->prism, CommandPriorty::low);
	mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->blink_up_pos, CommandPriorty::low, true);
	//mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->prism_consolidation_pos, 0, true);

	for (int i = 0; i < stalkers_to_blink.size(); i++)
	{
		const Unit* stalker = stalkers_to_blink[i];
		if (stalker == nullptr || stalker->is_alive == false)
		{
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
			continue;
		}

		if (Utility::OnSameLevel(mediator->ToPoint3D(stalker->pos), mediator->ToPoint3D(state_machine->blink_down_pos)))
		{
			if (Distance2D(stalker->pos, state_machine->blink_down_pos) < 1)
			{
				if (mediator->IsStalkerBlinkOffCooldown(stalker))
					mediator->SetUnitCommand(stalker, A_BLINK, state_machine->blink_up_pos, CommandPriorty::high);
				else
					mediator->SetUnitCommand(stalker, A_SMART, state_machine->prism, CommandPriorty::low);
			}
			else
			{
				mediator->SetUnitCommand(stalker, A_MOVE, state_machine->blink_down_pos, CommandPriorty::low);
			}
		}
		else
		{
			mediator->SetUnitCommand(stalker, A_MOVE, state_machine->consolidation_pos, CommandPriorty::low);
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
		}
	}
}

void BlinkStalkerAttackTerranLeaveHighground::EnterState()
{
	stalkers_to_blink = state_machine->attacking_stalkers;
}

void BlinkStalkerAttackTerranLeaveHighground::ExitState()
{
	
}

State* BlinkStalkerAttackTerranLeaveHighground::TestTransitions()
{
	if (stalkers_to_blink.size() == 0)
		return new BlinkStalkerAttackTerranConsolidate(mediator, state_machine);
	return nullptr;
}

std::string BlinkStalkerAttackTerranLeaveHighground::toString() const
{
	return "leave high ground";
}


#pragma endregion


#pragma region BlinkStalkerAttackTerran

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
	mediator->SetUnitsCommand(standby_stalkers, A_ATTACK, consolidation_pos, CommandPriorty::low);
}

bool BlinkStalkerAttackTerran::AddUnit(const Unit* unit)
{
	if (unit->unit_type == STALKER)
		moving_to_standby_stalkers.push_back(unit);
	else if (unit->unit_type == PRISM)
		prism = unit;
	else
		return false;
	return true;
}

void BlinkStalkerAttackTerran::RemoveUnit(const Unit* unit)
{
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

BlinkStalkerAttackTerran::~BlinkStalkerAttackTerran()
{
	
}


#pragma endregion

}