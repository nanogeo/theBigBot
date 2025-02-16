#pragma once

#include "blink_stalker_attack_terran_state_machine.h"
#include "theBigBot.h"


namespace sc2 {



#pragma region BlinkStalkerAttackTerranMoveAcross

void BlinkStalkerAttackTerranMoveAcross::TickState()
{
	agent->Actions()->UnitCommand(state_machine->stalkers, ABILITY_ID::ATTACK_ATTACK, state_machine->consolidation_pos);
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->prism_consolidation_pos);
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
	/*bool gates_almost_ready = true;
	for (const auto& status : agent->warpgate_status)
	{
		if (status.second.frame_ready > agent->Observation()->GetGameLoop() + 45)
		{
			gates_almost_ready = false;
			break;
		}
	}
	if (gates_almost_ready && agent->Observation()->IsPathable(state_machine->prism->pos) && Utility::CanAfford(UNIT_TYPEID::PROTOSS_STALKER, agent->warpgate_status.size() - 1, agent->Observation()))
	{
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->prism->pos);
		return new BlinkStalkerAttackTerranWarpIn(agent, state_machine);
	}*/

	if (Utility::DistanceToFurthest(state_machine->stalkers, state_machine->consolidation_pos) < 4 &&
		Distance2D(state_machine->prism->pos, state_machine->prism_consolidation_pos) < 2)
	{
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
		if (Distance2D(unit->pos, state_machine->consolidation_pos) > 3)
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
	if (state_machine->prism->unit_type == UNIT_TYPEID::PROTOSS_WARPPRISM)
	{
		if (state_machine->stalkers.size() < 8)
			return NULL;

		float stalkers_healthy = 0;
		for (const auto& stalker : state_machine->stalkers) // TODO change to enough stalkers not all
		{
			if (stalker->shield >= 70 && Distance2D(stalker->pos, state_machine->consolidation_pos) < 5)
			{
				stalkers_healthy += 1;
			}
		}
		if (stalkers_healthy / state_machine->stalkers.size() > .5)
		{
			if (state_machine->attacking_main)
				return new BlinkStalkerAttackTerranBlinkUp(agent, state_machine, state_machine->stalkers);
			else
				return new BlinkStalkerAttackTerranAttack(agent, state_machine);
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
		if (blinked)
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
	state_machine->attached_army_group->standby_pos = state_machine->blink_down_pos;
}

State* BlinkStalkerAttackTerranBlinkUp::TestTransitions()
{
	if (stalkers_to_blink.size() == 0)
		return new BlinkStalkerAttackTerranAttack(agent, state_machine);
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
		state_machine->attached_army_group->attack_path_line = agent->locations->blink_main_attack_path_lines[agent->Observation()->GetGameLoop() % 2 ? 0 : 1];
	else
		state_machine->attached_army_group->attack_path_line = agent->locations->blink_nat_attack_path_line;
}

void BlinkStalkerAttackTerranAttack::ExitState()
{
	state_machine->attacking_main = !state_machine->attacking_main;
	state_machine->attached_army_group->concave_origin = Point2D(0, 0);
}

State* BlinkStalkerAttackTerranAttack::TestTransitions()
{
	int attack_line_status = state_machine->attached_army_group->AttackLine(.2, 2, TERRAN_PRIO);
	if (attack_line_status == 1 || attack_line_status == 2)
	{
		if (state_machine->attacking_main)
		{
			return new BlinkStalkerAttackTerranLeaveHighground(agent, state_machine, state_machine->stalkers);
		}
		else
		{
			return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
		}
	}
	//bool gates_almost_ready = true;
	//for (const auto &status : agent->warpgate_status)
	//{
	//	if (status.second.frame_ready > agent->Observation()->GetGameLoop() + 45)
	//	{
	//		gates_almost_ready = false;
	//		break;
	//	}
	//}
	//if (gates_almost_ready && agent->Observation()->IsPathable(state_machine->prism->pos) && Utility::CanAfford(UNIT_TYPEID::PROTOSS_STALKER, agent->warpgate_status.size() - 1, agent->Observation())) // TODO why check if prism on pathable terrain?
	//{
	//	if (state_machine->attacking_main)
	//	{
	//		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->blink_up_pos);
	//		return new BlinkStalkerAttackTerranLeaveHighground(agent, state_machine, state_machine->stalkers);

	//	}
	//	else
	//	{
	//		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->prism->pos);
	//		return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);
	//	}
	//}

	/*int stalkers_ok = 0;
	for (const auto &stalker : state_machine->stalkers)
	{
		if (stalker->shield > 20)
			stalkers_ok++;
	}
	if (stalkers_ok < 4)
		return new BlinkStalkerAttackTerranConsolidate(agent, state_machine);*/
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

}

void BlinkStalkerAttackTerranSnipeUnit::ExitState()
{

}

State* BlinkStalkerAttackTerranSnipeUnit::TestTransitions()
{
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
	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);
}

void BlinkStalkerAttackTerranLeaveHighground::TickState()
{
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->blink_up_pos);
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism);
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->prism_consolidation_pos, true);

	for (int i = 0; i < stalkers_to_blink.size(); i++)
	{
		bool blinked = true;
		if (stalkers_to_blink[i]->health <= 0)
		{
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
			continue;
		}
		for (const auto& ability : agent->Query()->GetAbilitiesForUnit(stalkers_to_blink[i]).abilities)
		{
			if (ability.ability_id == ABILITY_ID::EFFECT_BLINK)
			{
				blinked = false;
				break;
			}
		}
		if (blinked && Distance2D(stalkers_to_blink[i]->pos, state_machine->blink_up_pos) < 4)
		{
			state_machine->attached_army_group->last_time_blinked[stalkers_to_blink[i]] = agent->Observation()->GetGameLoop();
			stalkers_to_blink.erase(stalkers_to_blink.begin() + i);
			i--;
			continue;
		}
		if (Distance2D(stalkers_to_blink[i]->pos, state_machine->blink_down_pos) < 2)
		{
			agent->Actions()->UnitCommand(stalkers_to_blink[i], ABILITY_ID::EFFECT_BLINK, state_machine->blink_up_pos);
			agent->Actions()->UnitCommand(stalkers_to_blink[i], ABILITY_ID::GENERAL_MOVE, state_machine->consolidation_pos, true);
		}
		else
		{
			agent->Actions()->UnitCommand(stalkers_to_blink[i], ABILITY_ID::GENERAL_MOVE, state_machine->blink_down_pos);
		}
	}
}

void BlinkStalkerAttackTerranLeaveHighground::EnterState()
{

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


void BlinkStalkerAttackTerranLeaveHighground::OnUnitDestroyedListener(const Unit* unit)
{
	if (stalkers_to_blink.size() == 0)
		return;
	auto found = std::find(stalkers_to_blink.begin(), stalkers_to_blink.end(), unit);
	if (found != stalkers_to_blink.end())
	{
		int index = found - stalkers_to_blink.begin();
		stalkers_to_blink.erase(stalkers_to_blink.begin() + index);
	}
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
		else if (unit->unit_type == STALKER && Distance2D(unit->pos, consolidation_pos) < 6)
		{
			stalkers.push_back(unit);
			return true;
		}
	}
	return false;
}

BlinkStalkerAttackTerran::~BlinkStalkerAttackTerran()
{
	
}


#pragma endregion

}