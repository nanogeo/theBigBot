
#include <iostream>

#include "blink_stalker_attack_terran_state_machine.h"
#include "army_group.h"
#include "theBigBot.h"


namespace sc2 {



#pragma region BlinkStalkerAttackTerranMoveAcross

void BlinkStalkerAttackTerranMoveAcross::TickState()
{
	//agent->mediator.SetUnitCommand(state_machine->stalkers, ABILITY_ID::ATTACK_ATTACK, state_machine->consolidation_pos, 0);
	//agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->prism_consolidation_pos, 0);
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
				agent->mediator.SetUnitCommand(stalker, ABILITY_ID::ATTACK_ATTACK, Utility::MedianCenter(state_machine->attacking_stalkers), 0);
			}
		}
	}
	if (state_machine->attached_army_group->AttackLine(state_machine->attacking_stalkers, .2f, 7, TERRAN_PRIO) > 0 ||
		state_machine->attached_army_group->attack_path_line.GetFurthestForward({ state_machine->attached_army_group->concave_origin,
			state_machine->attached_army_group->attack_path_line.FindClosestPoint(state_machine->consolidation_pos) }) == state_machine->attached_army_group->concave_origin)
	{
		if (state_machine->attached_army_group)
			state_machine->attached_army_group->using_standby = true;
		return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
	}

	if (Utility::DistanceToFurthest(state_machine->moving_to_standby_stalkers, state_machine->consolidation_pos) < 4 &&
		Distance2D(state_machine->prism->pos, state_machine->prism_consolidation_pos) < 2)
	{
		if (state_machine->attached_army_group)
			state_machine->attached_army_group->using_standby = true;
		return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
	}
	return nullptr;
}

std::string BlinkStalkerAttackTerranMoveAcross::toString()
{
	return "move across";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranWarpIn

void BlinkStalkerAttackTerranWarpIn::TickState()
{
	/*for (const auto& stalker : state_machine->stalkers)
	{
		agent->mediator.SetUnitCommand(stalker, ABILITY_ID::ATTACK, state_machine->consolidation_pos, 0);
	}
	if (state_machine->warping_in == false && state_machine->prism->unit_type == UNIT_TYPEID::PROTOSS_WARPPRISMPHASING)
	{
		// try to warp in
		Units gates = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
		if (Utility::CanAfford(UNIT_TYPEID::PROTOSS_STALKER, gates.size(), agent->Observation()))
		{
			std::vector<Point2D> spots = agent->FindWarpInSpots(agent->Observation()->GetGameInfo().enemy_start_locations[0], gates.size());
			//std::cout << "spots " << spots.size() << "\n";
			if (spots.size() >= gates.size())
			{
				for (int i = 0; i < gates.size(); i++)
				{
					//std::cout << "warp in at " << spots[i].x << ", " << spots[i].y << "\n";
					agent->mediator.SetUnitCommand(gates[i], ABILITY_ID::TRAINWARP_STALKER, spots[i], 0);
					agent->warpgate_status[gates[i]].used = true;
					agent->warpgate_status[gates[i]].frame_ready = agent->Observation()->GetGameLoop() + round(23 * FRAME_TIME);
				}

				state_machine->warping_in = true;
				state_machine->warp_in_time = agent->Observation()->GetGameLoop();
			}
		}
	}*/
}

void BlinkStalkerAttackTerranWarpIn::EnterState()
{
	agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE, 0);
}

void BlinkStalkerAttackTerranWarpIn::ExitState()
{
	agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMTRANSPORTMODE, 0);
}

State* BlinkStalkerAttackTerranWarpIn::TestTransitions()
{
	//if (state_machine->warping_in && agent->Observation()->GetGameLoop() > state_machine->warp_in_time + 90)
	//{
	//	state_machine->warping_in = false;
		return new BlinkStalkerAttackTerranMoveAcross(agent, state_machine);
	//}
	//return nullptr;
}

std::string BlinkStalkerAttackTerranWarpIn::toString()
{
	return "warp in";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranConsolidate

void BlinkStalkerAttackTerranConsolidate::TickState()
{
	if(state_machine->prism && Distance2D(state_machine->prism->pos, state_machine->prism_consolidation_pos) > 3)
		agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->prism_consolidation_pos, 0);

	for (int i = 0; i < state_machine->moving_to_standby_stalkers.size(); i++)
	{
		const Unit* stalker = state_machine->moving_to_standby_stalkers[i];

		if (Distance2D(stalker->pos, state_machine->consolidation_pos) > 10 || stalker->weapon_cooldown > 0)
		{
			agent->mediator.SetUnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, state_machine->consolidation_pos, 0);
		}
		else if (Distance2D(stalker->pos, state_machine->consolidation_pos) > 5)
		{
			agent->mediator.SetUnitCommand(stalker, ABILITY_ID::ATTACK_ATTACK, state_machine->consolidation_pos, 0);
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
	if (Utility::DistanceToClosest(agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), state_machine->consolidation_pos) < 12)
	{
		state_machine->attacking_main = false;
		return new BlinkStalkerAttackTerranAttack(agent, state_machine);
	}
	if (state_machine->prism->unit_type == UNIT_TYPEID::PROTOSS_WARPPRISM)
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
			// acropolis does not have a blink up spot
			if (agent->mediator.GetMapName() == "Acropolis AIE" || state_machine->prism->is_alive == false)
			{
				state_machine->attacking_main = false;
				return new BlinkStalkerAttackTerranAttack(agent, state_machine);
			}
			
			float danger_nat = agent->mediator.GetLineDangerLevel(agent->locations->blink_nat_attack_path_line);
			float danger_main_1 = agent->mediator.GetLineDangerLevel(agent->locations->blink_main_attack_path_lines[0]);
			float danger_main_2 = agent->mediator.GetLineDangerLevel(agent->locations->blink_main_attack_path_lines[1]);

			if (danger_nat <= danger_main_1 && danger_nat <= danger_main_2)
			{
				state_machine->attacking_main = false;
				return new BlinkStalkerAttackTerranAttack(agent, state_machine);
			}
			else
			{
				state_machine->attacking_main = true;
				return new BlinkStalkerAttackTerranBlinkUp(agent, state_machine);
			}
		}
	}
	return nullptr;
}

std::string BlinkStalkerAttackTerranConsolidate::toString()
{
	return "consolidate";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranBlinkUp

void BlinkStalkerAttackTerranBlinkUp::TickState()
{
	agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism, 0);
	agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->blink_down_pos, 0, true);
	for (int i = 0; i < stalkers_to_blink.size(); i++)
	{
		if (stalkers_to_blink[i] == nullptr) // TODO look into why this is ever the case
		{
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
			continue;
		}
		if (stalkers_to_blink[i]->pos.z - .1 < agent->ToPoint3D(state_machine->blink_down_pos).z && // TODO write utility function to check same height
			stalkers_to_blink[i]->pos.z + .1 > agent->ToPoint3D(state_machine->blink_down_pos).z)
		{
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
			continue;
		}
		if (Distance2D(stalkers_to_blink[i]->pos, state_machine->blink_up_pos) < 2)
		{
			agent->mediator.SetUnitCommand(stalkers_to_blink[i], ABILITY_ID::EFFECT_BLINK, state_machine->blink_down_pos, 2);
		}
		else
		{
			agent->mediator.SetUnitCommand(stalkers_to_blink[i], ABILITY_ID::GENERAL_MOVE, state_machine->blink_up_pos, 0);
		}
	}
}

void BlinkStalkerAttackTerranBlinkUp::EnterState()
{
	agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->blink_down_pos, 0);
	agent->mediator.SetUnitsCommand(state_machine->attacking_stalkers, ABILITY_ID::ATTACK_ATTACK, state_machine->blink_up_pos, 0);
	stalkers_to_blink = state_machine->attacking_stalkers;
}

void BlinkStalkerAttackTerranBlinkUp::ExitState()
{
	
}

State* BlinkStalkerAttackTerranBlinkUp::TestTransitions()
{
	if (stalkers_to_blink.size() == state_machine->attacking_stalkers.size() && 
		Utility::DistanceToClosest(agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), Utility::MedianCenter(stalkers_to_blink)) < 10)
	{
		state_machine->attacking_main = false;
		return new BlinkStalkerAttackTerranAttack(agent, state_machine);
	}
	if (stalkers_to_blink.size() == 0)
	{
		state_machine->attached_army_group->standby_pos = state_machine->blink_down_pos;
		return new BlinkStalkerAttackTerranAttack(agent, state_machine);
	}
	return nullptr;
}

std::string BlinkStalkerAttackTerranBlinkUp::toString()
{
	return "blink up";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranAttack

void BlinkStalkerAttackTerranAttack::TickState()
{
	if (state_machine->attacking_main)
	{
		agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism, 1);
		agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->blink_up_pos, 1, true);

		for (int i = 0; i < state_machine->moving_to_standby_stalkers.size(); i++)
		{
			const Unit* stalker = state_machine->moving_to_standby_stalkers[i];
			
			if (stalker->pos.z - .1 < agent->ToPoint3D(state_machine->blink_down_pos).z && // TODO write utility function to check same height
				stalker->pos.z + .1 > agent->ToPoint3D(state_machine->blink_down_pos).z)
			{
				if (Distance2D(stalker->pos, state_machine->blink_down_pos) < 1)
				{
					if (agent->mediator.IsStalkerBlinkOffCooldown(stalker))
						agent->mediator.SetUnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, state_machine->blink_up_pos, 2);
					else
						agent->mediator.SetUnitCommand(stalker, ABILITY_ID::SMART, state_machine->prism, 0);
				}
				else
				{
					agent->mediator.SetUnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, state_machine->blink_down_pos, 0);
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
					agent->mediator.SetUnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, state_machine->consolidation_pos, 0);
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < state_machine->moving_to_standby_stalkers.size(); i++)
		{
			const Unit* stalker = state_machine->moving_to_standby_stalkers[i];

			if (Distance2D(stalker->pos, state_machine->consolidation_pos) > 10 || stalker->weapon_cooldown > 0)
			{
				agent->mediator.SetUnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, state_machine->consolidation_pos, 0);
			}
			else if (Distance2D(stalker->pos, state_machine->consolidation_pos) > 5)
			{
				agent->mediator.SetUnitCommand(stalker, ABILITY_ID::ATTACK_ATTACK, state_machine->consolidation_pos, 0);
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
	if (state_machine->attacking_main)
	{
		float danger_main_1 = agent->mediator.GetLineDangerLevel(agent->locations->blink_main_attack_path_lines[0]);
		float danger_main_2 = agent->mediator.GetLineDangerLevel(agent->locations->blink_main_attack_path_lines[1]);

		if (danger_main_1 > danger_main_2)
			state_machine->attached_army_group->attack_path_line = agent->locations->blink_main_attack_path_lines[0];
		else
			state_machine->attached_army_group->attack_path_line = agent->locations->blink_main_attack_path_lines[1];
	}
	else
	{
		state_machine->attached_army_group->attack_path_line = agent->locations->blink_nat_attack_path_line;
	}
}

void BlinkStalkerAttackTerranAttack::ExitState()
{

}

State* BlinkStalkerAttackTerranAttack::TestTransitions()
{
	for (const auto& unit : agent->Observation()->GetUnits(Unit::Alliance::Enemy, IsUnits({ SIEGE_TANK, SIEGE_TANK_SIEGED, MEDIVAC, BATTLECRUISER })))
	{
		Units stalkers_in_range;
		for (const auto& stalker : state_machine->attacking_stalkers)
		{
			if (stalker->weapon_cooldown > 0)
				continue;

			if ((agent->mediator.IsStalkerBlinkOffCooldown(stalker) && Distance2D(stalker->pos, unit->pos) < Utility::RealRange(stalker, unit) + 7) ||
				(Distance2D(stalker->pos, unit->pos) < Utility::RealRange(stalker, unit)))
			{
				stalkers_in_range.push_back(stalker);
			}
		}
		if (stalkers_in_range.size() == 0)
			continue;
		if (stalkers_in_range.size() * Utility::GetDamage(stalkers_in_range[0], unit) >= unit->health)
		{
			return new BlinkStalkerAttackTerranSnipeUnit(agent, state_machine, unit);
		}
	}
	int attack_line_status = state_machine->attached_army_group->AttackLine(state_machine->attacking_stalkers, .2f, 7, TERRAN_PRIO, true);
	if (attack_line_status == 1 || attack_line_status == 2 || state_machine->standby_stalkers.size() > state_machine->attacking_stalkers.size())
	{
		if (state_machine->attacking_main)
		{
			state_machine->attached_army_group->concave_origin = Point2D(0, 0);
			return new BlinkStalkerAttackTerranLeaveHighground(agent, state_machine);
		}
		else
		{
			state_machine->attached_army_group->concave_origin = Point2D(0, 0);
			return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
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
		if (state_machine->attacking_main)
		{
			state_machine->attached_army_group->concave_origin = Point2D(0, 0);
			return new BlinkStalkerAttackTerranLeaveHighground(agent, state_machine);
		}
		else
		{
			if (Utility::DistanceToClosest(agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), state_machine->consolidation_pos) >= 12)
			{
				state_machine->attached_army_group->concave_origin = Point2D(0, 0);
				return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
			}
		}
	}

	return nullptr;
}

std::string BlinkStalkerAttackTerranAttack::toString()
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
	enter_time = agent->mediator.GetCurrentTime(); 
	for (const auto& stalker : state_machine->attacking_stalkers)
	{
		if (stalker->weapon_cooldown == 0)
		{
			if (agent->mediator.IsStalkerBlinkOffCooldown(stalker))
			{
				agent->mediator.SetUnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, target->pos, 2);
				agent->mediator.SetUnitCommand(stalker, ABILITY_ID::ATTACK_ATTACK, target, 0, true);
			}
			else
			{
				agent->mediator.SetUnitCommand(stalker, ABILITY_ID::ATTACK_ATTACK, target, 0);
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
		agent->mediator.GetCurrentTime() > enter_time + .5)
		return new BlinkStalkerAttackTerranAttack(agent, state_machine);
	return nullptr;
}

std::string BlinkStalkerAttackTerranSnipeUnit::toString()
{
	return "snipe unit";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranLeaveHighground

void BlinkStalkerAttackTerranLeaveHighground::TickState()
{
	agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism, 0);
	agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->blink_up_pos, 0, true);
	//agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->prism_consolidation_pos, 0, true);

	for (int i = 0; i < stalkers_to_blink.size(); i++)
	{
		const Unit* stalker = stalkers_to_blink[i];
		if (stalker == nullptr || stalker->is_alive == false)
		{
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
			continue;
		}

		if (stalker->pos.z - .1 < agent->ToPoint3D(state_machine->blink_down_pos).z && // TODO write utility function to check same height
			stalker->pos.z + .1 > agent->ToPoint3D(state_machine->blink_down_pos).z)
		{
			if (Distance2D(stalker->pos, state_machine->blink_down_pos) < 1)
			{
				if (agent->mediator.IsStalkerBlinkOffCooldown(stalker))
					agent->mediator.SetUnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, state_machine->blink_up_pos, 2);
				else
					agent->mediator.SetUnitCommand(stalker, ABILITY_ID::SMART, state_machine->prism, 0);
			}
			else
			{
				agent->mediator.SetUnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, state_machine->blink_down_pos, 0);
			}
		}
		else
		{
			agent->mediator.SetUnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, state_machine->consolidation_pos, 0);
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
		return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
	return nullptr;
}

std::string BlinkStalkerAttackTerranLeaveHighground::toString()
{
	return "leave high ground";
}


#pragma endregion


#pragma region BlinkStalkerAttackTerran

BlinkStalkerAttackTerran::BlinkStalkerAttackTerran(TheBigBot* agent, std::string name, 
	Point2D consolidation_pos, Point2D prism_consolidation_pos, Point2D blink_up_pos, Point2D blink_down_pos) : StateMachine(agent, name)
{
	this->consolidation_pos = consolidation_pos;
	this->prism_consolidation_pos = prism_consolidation_pos;
	this->blink_up_pos = blink_up_pos;
	this->blink_down_pos = blink_down_pos;
	current_state = new BlinkStalkerAttackTerranMoveAcross(agent, this);


	current_state->EnterState();
}

void BlinkStalkerAttackTerran::RunStateMachine()
{
	StateMachine::RunStateMachine();
	agent->mediator.SetUnitsCommand(standby_stalkers, ABILITY_ID::ATTACK_ATTACK, consolidation_pos, 0);
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

BlinkStalkerAttackTerran::~BlinkStalkerAttackTerran()
{
	
}


#pragma endregion

}