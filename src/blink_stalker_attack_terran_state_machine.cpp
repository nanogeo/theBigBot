#pragma once

#include "blink_stalker_attack_terran_state_machine.h"
#include "theBigBot.h"


namespace sc2 {



#pragma region BlinkStalkerAttackTerranMoveAcross

void BlinkStalkerAttackTerranMoveAcross::TickState()
{
	//agent->Actions()->UnitCommand(state_machine->stalkers, ABILITY_ID::ATTACK_ATTACK, state_machine->consolidation_pos);
	//agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->prism_consolidation_pos);
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
	if (state_machine->stalkers.size() > 0 && (state_machine->attached_army_group->AttackLine(.2, 7, TERRAN_PRIO) > 0 ||
		state_machine->attached_army_group->attack_path_line.GetFurthestForward({ state_machine->attached_army_group->concave_origin,
			state_machine->attached_army_group->attack_path_line.FindClosestPoint(state_machine->consolidation_pos) }) == state_machine->attached_army_group->concave_origin))
	{
		if (state_machine->attached_army_group)
			state_machine->attached_army_group->using_standby = true;
		return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
	}

	if (Utility::DistanceToFurthest(state_machine->stalkers, state_machine->consolidation_pos) < 4 &&
		Distance2D(state_machine->prism->pos, state_machine->prism_consolidation_pos) < 2)
	{
		if (state_machine->attached_army_group)
			state_machine->attached_army_group->using_standby = true;
		return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
	}
	return NULL;
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
		agent->Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, state_machine->consolidation_pos);
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
					agent->Actions()->UnitCommand(gates[i], ABILITY_ID::TRAINWARP_STALKER, spots[i]);
					agent->warpgate_status[gates[i]].used = true;
					agent->warpgate_status[gates[i]].frame_ready = agent->Observation()->GetGameLoop() + round(23 * 22.4);
				}

				state_machine->warping_in = true;
				state_machine->warp_in_time = agent->Observation()->GetGameLoop();
			}
		}
	}*/
}

void BlinkStalkerAttackTerranWarpIn::EnterState()
{
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE);
}

void BlinkStalkerAttackTerranWarpIn::ExitState()
{
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMTRANSPORTMODE);
}

State* BlinkStalkerAttackTerranWarpIn::TestTransitions()
{
	//if (state_machine->warping_in && agent->Observation()->GetGameLoop() > state_machine->warp_in_time + 90)
	//{
	//	state_machine->warping_in = false;
		return new BlinkStalkerAttackTerranMoveAcross(agent, state_machine);
	//}
	//return NULL;
}

std::string BlinkStalkerAttackTerranWarpIn::toString()
{
	return "warp in";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranConsolidate

void BlinkStalkerAttackTerranConsolidate::TickState()
{
	if(Distance2D(state_machine->prism->pos, state_machine->prism_consolidation_pos) > 3)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->prism_consolidation_pos);
	for (const auto& unit : state_machine->stalkers)
	{
		if (Distance2D(unit->pos, state_machine->consolidation_pos) > 10 || unit->weapon_cooldown > 0)
			agent->Actions()->UnitCommand(unit, ABILITY_ID::GENERAL_MOVE, state_machine->consolidation_pos);
		else if (Distance2D(unit->pos, state_machine->consolidation_pos) > 3)
			agent->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, state_machine->consolidation_pos);
	}


	/*if (state_machine->prism->unit_type == UNIT_TYPEID::PROTOSS_WARPPRISM && Distance2D(state_machine->prism->pos, state_machine->prism_consolidation_pos) < 1)
	{
		bool gates_almost_ready = true;
		for (const auto &gate_status : agent->warpgate_status)
		{
			if (agent->Observation()->GetGameLoop() + 112 < gate_status.second.frame_ready)
			{
				gates_almost_ready = false;
				break;
			}
		}
		if (gates_almost_ready)
		{
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE);
		}
	}
	else if (state_machine->prism->unit_type == UNIT_TYPEID::PROTOSS_WARPPRISMPHASING && state_machine->warping_in == false)
	{
		bool gates_ready = true;
		for (const auto &gate_status : agent->warpgate_status)
		{
			if (agent->Observation()->GetGameLoop() < gate_status.second.frame_ready)
			{
				gates_ready = false;
				break;
			}
		}
		if (gates_ready)
		{
			// try warp in
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
						agent->Actions()->UnitCommand(gates[i], ABILITY_ID::TRAINWARP_STALKER, spots[i]);
						agent->warpgate_status[gates[i]].used = true;
						agent->warpgate_status[gates[i]].frame_ready = agent->Observation()->GetGameLoop() + round(23 * 22.4);
					}

					state_machine->warping_in = true;
					state_machine->warp_in_time = agent->Observation()->GetGameLoop();
				}
			}
		}
	}
	else if (state_machine->warping_in && agent->Observation()->GetGameLoop() > state_machine->warp_in_time + 90)
	{
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMTRANSPORTMODE);
		state_machine->warping_in = false;
	}*/
}

void BlinkStalkerAttackTerranConsolidate::EnterState()
{

}

void BlinkStalkerAttackTerranConsolidate::ExitState()
{

}

State* BlinkStalkerAttackTerranConsolidate::TestTransitions()
{
	if (Utility::DistanceToClosest(agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), state_machine->consolidation_pos) < 12)
	{
		state_machine->attacking_main = false;
		return new BlinkStalkerAttackTerranAttack(agent, state_machine);
	}
	if (state_machine->prism->unit_type == UNIT_TYPEID::PROTOSS_WARPPRISM)
	{
		if (state_machine->stalkers.size() < 8)
			return NULL;

		float stalkers_healthy = 0;
		for (const auto& stalker : state_machine->stalkers) // TODO change to enough stalkers not all
		{
			if (stalker->shield >= 70 && Distance2D(stalker->pos, state_machine->consolidation_pos) < 7)
			{
				stalkers_healthy += 1;
			}
		}
		if (stalkers_healthy / state_machine->stalkers.size() > .5)
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
				return new BlinkStalkerAttackTerranBlinkUp(agent, state_machine, state_machine->stalkers);
			}
		}
	}
	return NULL;
}

std::string BlinkStalkerAttackTerranConsolidate::toString()
{
	return "consolidate";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranBlinkUp

void BlinkStalkerAttackTerranBlinkUp::TickState()
{
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism);
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->blink_down_pos);
	for (int i = 0; i < stalkers_to_blink.size(); i++)
	{
		bool blinked = true;
		for (const auto& ability : agent->Query()->GetAbilitiesForUnit(stalkers_to_blink[i]).abilities)
		{
			if (ability.ability_id == ABILITY_ID::EFFECT_BLINK)
			{
				blinked = false;
				break;
			}
		}
		if (blinked && Distance2D(stalkers_to_blink[i]->pos, state_machine->blink_down_pos) < Distance2D(stalkers_to_blink[i]->pos, state_machine->blink_up_pos))
		{
			state_machine->attached_army_group->last_time_blinked[stalkers_to_blink[i]] = agent->Observation()->GetGameLoop();
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
			continue;
		}
		if (Distance2D(stalkers_to_blink[i]->pos, state_machine->blink_up_pos) < 2)
		{
			agent->Actions()->UnitCommand(stalkers_to_blink[i], ABILITY_ID::EFFECT_BLINK, state_machine->blink_down_pos);
		}
		else
		{
			agent->Actions()->UnitCommand(stalkers_to_blink[i], ABILITY_ID::GENERAL_MOVE, state_machine->blink_up_pos);
		}
	}
}

void BlinkStalkerAttackTerranBlinkUp::EnterState()
{
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->blink_down_pos);
	agent->Actions()->UnitCommand(state_machine->stalkers, ABILITY_ID::ATTACK_ATTACK, state_machine->blink_up_pos);
}

void BlinkStalkerAttackTerranBlinkUp::ExitState()
{
	
}

State* BlinkStalkerAttackTerranBlinkUp::TestTransitions()
{
	if (stalkers_to_blink.size() == state_machine->attached_army_group->stalkers.size() && 
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
	return NULL;
}

std::string BlinkStalkerAttackTerranBlinkUp::toString()
{
	return "blink up";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranAttack

void BlinkStalkerAttackTerranAttack::TickState()
{
	
}

void BlinkStalkerAttackTerranAttack::EnterState()
{
	state_machine->attached_army_group->standby_units = {};
	if (state_machine->attacking_main)
	{
		float danger_main_1 = agent->mediator.GetLineDangerLevel(agent->locations->blink_main_attack_path_lines[0]);
		float danger_main_2 = agent->mediator.GetLineDangerLevel(agent->locations->blink_main_attack_path_lines[1]);

		if (danger_main_1 < danger_main_2)
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
	float now = agent->Observation()->GetGameLoop() / 22.4;
	for (const auto& unit : agent->Observation()->GetUnits(Unit::Alliance::Enemy, IsUnits({ SIEGE_TANK, SIEGE_TANK_SIEGED, MEDIVAC, BATTLECRUISER })))
	{
		Units stalkers_in_range;
		for (const auto& stalker : state_machine->stalkers)
		{
			if (stalker->weapon_cooldown > 0)
				continue;
			bool blink_off_cooldown = now - state_machine->attached_army_group->last_time_blinked[stalker] > 7;
			if ((blink_off_cooldown && Distance2D(stalker->pos, unit->pos) < Utility::RealRange(stalker, unit) + 8) ||
				(Distance2D(stalker->pos, unit->pos) < Utility::RealRange(stalker, unit)))
			{
				stalkers_in_range.push_back(stalker);
			}
		}
		if (stalkers_in_range.size() * (unit->unit_type == BATTLECRUISER ? 15 : 17) >= unit->health)
		{
			target = unit;
			return new BlinkStalkerAttackTerranSnipeUnit(agent, state_machine, target);
		}
	}
	int attack_line_status = state_machine->attached_army_group->AttackLine(.2, 7, TERRAN_PRIO, true);
	if (attack_line_status == 1 || attack_line_status == 2 || state_machine->attached_army_group->new_units.size() > state_machine->stalkers.size() * 1.5)
	{
		if (state_machine->attacking_main)
		{
			state_machine->attacking_main = !state_machine->attacking_main;
			state_machine->attached_army_group->concave_origin = Point2D(0, 0);
			return new BlinkStalkerAttackTerranLeaveHighground(agent, state_machine, state_machine->stalkers);
		}
		else
		{
			state_machine->attacking_main = !state_machine->attacking_main;
			state_machine->attached_army_group->concave_origin = Point2D(0, 0);
			return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
		}
	}
	if (state_machine->attacking_main)
	{
		for (int i = 0; i < state_machine->attached_army_group->standby_units.size(); i++)
		{
			const Unit* unit = state_machine->attached_army_group->standby_units[i];
			if (Distance2D(unit->pos, state_machine->attached_army_group->standby_pos) > 2)
			{
				agent->Actions()->UnitCommand(unit, ABILITY_ID::GENERAL_MOVE, state_machine->blink_down_pos);
				continue;
			}
			for (const auto& ability : agent->Query()->GetAbilitiesForUnit(unit).abilities)
			{
				if (ability.ability_id == ABILITY_ID::EFFECT_BLINK)
				{
					agent->Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_BLINK, state_machine->blink_up_pos);
					state_machine->attached_army_group->RemoveUnit(unit);
					state_machine->attached_army_group->AddNewUnit(unit);
					i--;
					break;
				}
			}
		}
	}

	int stalkers_ok = 0;
	for (const auto &stalker : state_machine->stalkers)
	{
		if (stalker->shield > 20)
			stalkers_ok++;
	}
	if (stalkers_ok < 6)
	{
		if (state_machine->attacking_main)
		{
			state_machine->attacking_main = !state_machine->attacking_main;
			state_machine->attached_army_group->concave_origin = Point2D(0, 0);
			return new BlinkStalkerAttackTerranLeaveHighground(agent, state_machine, state_machine->stalkers);
		}
		else
		{
			if (Utility::DistanceToClosest(agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), state_machine->consolidation_pos) >= 12)
			{
				state_machine->attacking_main = !state_machine->attacking_main;
				state_machine->attached_army_group->concave_origin = Point2D(0, 0);
				return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
			}
		}
	}

	return NULL;
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
	float now = agent->Observation()->GetGameLoop() / 22.4; 
	for (const auto& stalker : state_machine->stalkers)
	{
		if (stalker->weapon_cooldown == 0)
		{
			bool blink_off_cooldown = now - state_machine->attached_army_group->last_time_blinked[stalker] > 7;
			if (blink_off_cooldown)
			{
				agent->Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, target->pos);
				agent->Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK_ATTACK, target, true);
			}
			else
			{
				agent->Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK_ATTACK, target);
			}
		}
	}
}

void BlinkStalkerAttackTerranSnipeUnit::ExitState()
{

}

State* BlinkStalkerAttackTerranSnipeUnit::TestTransitions()
{
	if (target->is_alive == false)
		return new BlinkStalkerAttackTerranAttack(agent, state_machine);
	return NULL;
}

std::string BlinkStalkerAttackTerranSnipeUnit::toString()
{
	return "snipe unit";
}

#pragma endregion

#pragma region BlinkStalkerAttackTerranLeaveHighground

BlinkStalkerAttackTerranLeaveHighground::BlinkStalkerAttackTerranLeaveHighground(TheBigBot* agent, BlinkStalkerAttackTerran* state_machine, Units stalkers)
{
	this->agent = agent;
	this->state_machine = state_machine;
	stalkers_to_blink = stalkers;

	event_id = agent->GetUniqueId();
}

void BlinkStalkerAttackTerranLeaveHighground::TickState()
{
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->blink_up_pos);
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism);
	//agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->prism_consolidation_pos, true);

	for (int i = 0; i < stalkers_to_blink.size(); i++)
	{
		bool blink_ready = false;
		const Unit* stalker = stalkers_to_blink[i];
		if (stalker->is_alive == false)
		{
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
			continue;
		}
		for (const auto& ability : agent->Query()->GetAbilitiesForUnit(stalker).abilities)
		{
			if (ability.ability_id == ABILITY_ID::EFFECT_BLINK)
			{
				blink_ready = true;
				break;
			}
		}
		if (Distance2D(stalker->pos, state_machine->blink_up_pos) < 3)
		{
			state_machine->attached_army_group->RemoveUnit(stalker);
			state_machine->attached_army_group->AddNewUnit(stalker);
			i--;
			continue;
		}
		if (Distance2D(stalker->pos, state_machine->blink_down_pos) < 2)
		{
			if (blink_ready)
			{
				agent->Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, state_machine->blink_up_pos);
			}
			else
			{
				agent->Actions()->UnitCommand(stalker, ABILITY_ID::SMART, state_machine->prism);
			}
		}
		else
		{
			agent->Actions()->UnitCommand(stalker, ABILITY_ID::GENERAL_MOVE, state_machine->blink_down_pos);
		}
	}
}

void BlinkStalkerAttackTerranLeaveHighground::EnterState()
{
	for (const auto& stalker : state_machine->stalkers)
	{
		stalkers_to_blink.push_back(stalker);
	}
}

void BlinkStalkerAttackTerranLeaveHighground::ExitState()
{
	state_machine->attached_army_group->standby_pos = state_machine->consolidation_pos;
	agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

State* BlinkStalkerAttackTerranLeaveHighground::TestTransitions()
{
	if (stalkers_to_blink.size() == 0 || Utility::DistanceToFurthest(state_machine->stalkers, state_machine->consolidation_pos) < 8)
		return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
	return NULL;
}

std::string BlinkStalkerAttackTerranLeaveHighground::toString()
{
	return "leave high ground";
}

void BlinkStalkerAttackTerranLeaveHighground::RemoveUnit(const Unit* unit)
{
	stalkers_to_blink.erase(std::remove(stalkers_to_blink.begin(), stalkers_to_blink.end(), unit), stalkers_to_blink.end());
}


#pragma endregion


#pragma region BlinkStalkerAttackTerran

BlinkStalkerAttackTerran::BlinkStalkerAttackTerran(TheBigBot* agent, std::string name, 
	Point2D consolidation_pos, Point2D prism_consolidation_pos, Point2D blink_up_pos, Point2D blink_down_pos) {
	this->agent = agent;
	this->name = name;
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
	agent->Actions()->UnitCommand(attached_army_group->new_units, ABILITY_ID::ATTACK_ATTACK, consolidation_pos);
}

bool BlinkStalkerAttackTerran::AddUnit(const Unit* unit)
{
	if (dynamic_cast<BlinkStalkerAttackTerranMoveAcross*>(current_state) || dynamic_cast<BlinkStalkerAttackTerranConsolidate*>(current_state))
	{
		if (unit->unit_type == PRISM && prism == NULL)
		{
			prism = unit;
			return true;
		}
		else if (unit->unit_type == STALKER)
		{
			if (Distance2D(unit->pos, consolidation_pos) < 6)
			{
				stalkers.push_back(unit);
				return true;
			}
			else if (dynamic_cast<BlinkStalkerAttackTerranMoveAcross*>(current_state))
			{
				if (stalkers.size() == 0 || Distance2D(unit->pos, Utility::MedianCenter(stalkers)) < 6)
				{
					stalkers.push_back(unit);
					return true;
				}
				else
				{
					agent->Actions()->UnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(stalkers));
				}
			}
			else
			{
				agent->Actions()->UnitCommand(unit, ABILITY_ID::GENERAL_MOVE, consolidation_pos);
			}
		}
	}
	return false;
}

void BlinkStalkerAttackTerran::RemoveUnit(const Unit* unit)
{
	stalkers.erase(std::remove(stalkers.begin(), stalkers.end(), unit), stalkers.end());

	if (dynamic_cast<BlinkStalkerAttackTerranLeaveHighground*>(current_state))
		dynamic_cast<BlinkStalkerAttackTerranLeaveHighground*>(current_state)->RemoveUnit(unit);
}

BlinkStalkerAttackTerran::~BlinkStalkerAttackTerran()
{
	
}


#pragma endregion

}