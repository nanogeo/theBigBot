
#include "immortal_drop_state_machine.h"
#include "theBigBot.h"


namespace sc2 {



#pragma region ImmortalDropWaitForImmortals

void ImmortalDropWaitForImmortals::TickState()
{
	if (state_machine->prism == nullptr)
	{
		for (const auto &prism : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_WARPPRISM)))
		{
			state_machine->prism = prism;
			break;
		}
	}
	if (state_machine->immortal1 == nullptr || state_machine->immortal2 == nullptr)
	{
		for (const auto &immortal : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_IMMORTAL)))
		{
			if (state_machine->immortal1 == nullptr && immortal != state_machine->immortal2)
				state_machine->immortal1 = immortal;
			else if (state_machine->immortal2 == nullptr && immortal != state_machine->immortal1)
				state_machine->immortal2 = immortal;
		}
	}
}

void ImmortalDropWaitForImmortals::EnterState()
{
	return;
}

void ImmortalDropWaitForImmortals::ExitState()
{
	agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::SMART, state_machine->prism);
	agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::SMART, state_machine->prism);
	return;
}

State* ImmortalDropWaitForImmortals::TestTransitions()
{
	if (state_machine->prism != nullptr && state_machine->immortal1 != nullptr  && state_machine->immortal2 != nullptr)
		return new ImmortalDropInitialMove(agent, state_machine);
	return nullptr;
}

std::string ImmortalDropWaitForImmortals::toString()
{
	return "wait for immortals";
}

#pragma endregion

#pragma region ImmortalDropInitialMove

void ImmortalDropInitialMove::TickState()
{
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->entry_pos);
}

void ImmortalDropInitialMove::EnterState()
{
	return;
}

void ImmortalDropInitialMove::ExitState()
{
	return;
}

State* ImmortalDropInitialMove::TestTransitions()
{
	if (Distance2D(state_machine->prism->pos, state_machine->entry_pos) < 15)
		return new ImmortalDropMicroDrop(agent, state_machine);
	return nullptr;
}

std::string ImmortalDropInitialMove::toString()
{
	return "initial move";
}

#pragma endregion

#pragma region ImmortalDropMicroDrop

void ImmortalDropMicroDrop::TickState()
{
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism);
	agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(state_machine->immortal1->pos, state_machine->prism->pos, 1));
	agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(state_machine->immortal2->pos, state_machine->prism->pos, 1));
}


void ImmortalDropMicroDrop::EnterState()
{
	return;
}

void ImmortalDropMicroDrop::ExitState()
{
	return;
}

State* ImmortalDropMicroDrop::TestTransitions()
{
	if (state_machine->prism->cargo_space_taken == 0)
		return new ImmortalDropMicroDropDropped2(agent, state_machine);
	return nullptr;
}

std::string ImmortalDropMicroDrop::toString()
{
	return "micro immortal drop";
}

#pragma endregion

#pragma region ImmortalDropMicroDropCarrying1

ImmortalDropMicroDropCarrying1::ImmortalDropMicroDropCarrying1(TheBigBot* agent, ImmortalDropStateMachine* state_machine)
{
	this->agent = agent;
	this->state_machine = state_machine;
	entry_frame = agent->Observation()->GetGameLoop();
}

void ImmortalDropMicroDropCarrying1::TickState()
{
	// don't leave immortals behind
	if (Distance2D(state_machine->prism->pos, state_machine->immortal2->pos) > 4.5)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->immortal2->pos);
	else
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->UpdatePrismPathing());

	if (agent->Observation()->GetGameLoop() >= entry_frame + 15)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism);
	if (state_machine->immortal1->orders.size() == 0 || state_machine->immortal1->orders[0].ability_id != ABILITY_ID::SMART)
		agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(state_machine->immortal1->pos, state_machine->prism->pos, 1));
	if (state_machine->immortal2->orders.size() == 0 || state_machine->immortal2->orders[0].ability_id != ABILITY_ID::SMART)
		agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(state_machine->immortal2->pos, state_machine->prism->pos, 1));
}

void ImmortalDropMicroDropCarrying1::EnterState()
{
	return;
}

void ImmortalDropMicroDropCarrying1::ExitState()
{
	return;
}

State* ImmortalDropMicroDropCarrying1::TestTransitions()
{
	if (state_machine->immortal1->weapon_cooldown == 0 && state_machine->immortal2->weapon_cooldown == 0 && state_machine->prism->cargo_space_taken == 0)
		return new ImmortalDropMicroDropDropped1(agent, state_machine);
	return nullptr;
}

std::string ImmortalDropMicroDropCarrying1::toString()
{
	return "micro immortal drop carrying 1";
}

#pragma endregion

#pragma region ImmortalDropMicroDropCarrying2

ImmortalDropMicroDropCarrying2::ImmortalDropMicroDropCarrying2(TheBigBot* agent, ImmortalDropStateMachine* state_machine)
{
	this->agent = agent;
	this->state_machine = state_machine;
	entry_frame = agent->Observation()->GetGameLoop();
}

void ImmortalDropMicroDropCarrying2::TickState()
{
	// don't leave immortals behind
	if (Distance2D(state_machine->prism->pos, state_machine->immortal1->pos) > 4.5)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->immortal1->pos);
	else
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->UpdatePrismPathing());

	if (agent->Observation()->GetGameLoop() >= entry_frame + 15)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism);
	if (state_machine->immortal1->orders.size() == 0 || state_machine->immortal1->orders[0].ability_id != ABILITY_ID::SMART)
		agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(state_machine->immortal1->pos, state_machine->prism->pos, 1));
	if (state_machine->immortal2->orders.size() == 0 || state_machine->immortal2->orders[0].ability_id != ABILITY_ID::SMART)
		agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(state_machine->immortal2->pos, state_machine->prism->pos, 1));
}

void ImmortalDropMicroDropCarrying2::EnterState()
{
	return;
}

void ImmortalDropMicroDropCarrying2::ExitState()
{
	return;
}

State* ImmortalDropMicroDropCarrying2::TestTransitions()
{
	if (state_machine->immortal1->weapon_cooldown == 0 && state_machine->immortal2->weapon_cooldown == 0 && state_machine->prism->cargo_space_taken == 0)
		return new ImmortalDropMicroDropDropped2(agent, state_machine);
	return nullptr;
}

std::string ImmortalDropMicroDropCarrying2::toString()
{
	return "micro immortal drop carrying 2";
}

#pragma endregion

#pragma region ImmortalDropMicroDropDropped1

void ImmortalDropMicroDropDropped1::TickState()
{
	// don't leave immortals behind
	if (Distance2D(state_machine->prism->pos, state_machine->immortal1->pos) > 4.5)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->immortal1->pos);
	else if (Distance2D(state_machine->prism->pos, state_machine->immortal2->pos) > 4.5)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->immortal2->pos);
	else
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->UpdatePrismPathing());

	if ((!immortal1_has_attack_order || state_machine->immortal1->weapon_cooldown > 0) && (!immortal2_has_attack_order || state_machine->immortal2->weapon_cooldown > 0))
		agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::SMART, state_machine->prism);
}

void ImmortalDropMicroDropDropped1::EnterState()
{
	std::map<const Unit*, const Unit*> attacks = agent->FindTargets({ state_machine->immortal1, state_machine->immortal2 }, state_machine->target_priority, 0);
	if (attacks.count(state_machine->immortal1))
	{
		agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::ATTACK, attacks[state_machine->immortal1]);
		immortal1_has_attack_order = true;
	}

	if (attacks.count(state_machine->immortal2))
	{
		agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::ATTACK, attacks[state_machine->immortal2]);
		immortal2_has_attack_order = true;
	}
}

void ImmortalDropMicroDropDropped1::ExitState()
{
		
}

State* ImmortalDropMicroDropDropped1::TestTransitions()
{
	if (state_machine->prism->cargo_space_taken == 4)
		return new ImmortalDropMicroDropCarrying2(agent, state_machine);
	return nullptr;
}

std::string ImmortalDropMicroDropDropped1::toString()
{
	return "micro immortal drop dropped 1";
}

#pragma endregion

#pragma region ImmortalDropMicroDropDropped2

void ImmortalDropMicroDropDropped2::TickState()
{
	// don't leave immortals behind
	if (Distance2D(state_machine->prism->pos, state_machine->immortal1->pos) > 4.5)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->immortal1->pos);
	else if (Distance2D(state_machine->prism->pos, state_machine->immortal2->pos) > 4.5)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->immortal2->pos);
	else
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->UpdatePrismPathing());

	if ((!immortal1_has_attack_order || state_machine->immortal1->weapon_cooldown > 0) && (!immortal2_has_attack_order || state_machine->immortal2->weapon_cooldown > 0))
		agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::SMART, state_machine->prism);
}

void ImmortalDropMicroDropDropped2::EnterState()
{
	std::map<const Unit*, const Unit*> attacks = agent->FindTargets({ state_machine->immortal1, state_machine->immortal2 }, state_machine->target_priority, 0);
	if (attacks.count(state_machine->immortal1))
	{
		agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::ATTACK, attacks[state_machine->immortal1]);
		immortal1_has_attack_order = true;
	}

	if (attacks.count(state_machine->immortal2))
	{
		agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::ATTACK, attacks[state_machine->immortal2]);
		immortal2_has_attack_order = true;
	}
}

void ImmortalDropMicroDropDropped2::ExitState()
{
	agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::SMART, state_machine->prism);
}

State* ImmortalDropMicroDropDropped2::TestTransitions()
{
	if (state_machine->prism->cargo_space_taken == 4)
		return new ImmortalDropMicroDropCarrying1(agent, state_machine);
	return nullptr;
}

std::string ImmortalDropMicroDropDropped2::toString()
{
	return "micro immortal drop dropped 2";
}

#pragma endregion


Point2D ImmortalDropStateMachine::UpdatePrismPathing()
{
	// if theres a tank then go ontop of it
	if (agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_SIEGETANKSIEGED)).size() > 0)
	{
		return agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_SIEGETANKSIEGED))[0]->pos;
	}
	if (agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_SIEGETANK)).size() > 0)
	{
		return agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_SIEGETANK))[0]->pos;
	}
	// if theres a cyclone then go ontop of it?
	if (agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_CYCLONE)).size() > 0)
	{
		return agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_CYCLONE))[0]->pos;
	}

	int prev_index = path_index - 1 >= 0 ? path_index - 1 : prism_path.size() - 1;
	int next_index = path_index + 1 < prism_path.size() ? path_index + 1 : 0;

	// update path index
	if (Distance2D(prism->pos, prism_path[prev_index]) < 2)
	{
		path_index = prev_index;
		prev_index = path_index - 1 >= 0 ? path_index - 1 : prism_path.size() - 1;
		next_index = path_index + 1 < prism_path.size() ? path_index + 1 : 0;
	}
	else if (Distance2D(prism->pos, prism_path[next_index]) < 2)
	{
		path_index = next_index;
		prev_index = path_index - 1 >= 0 ? path_index - 1 : prism_path.size() - 1;
		next_index = path_index + 1 < prism_path.size() ? path_index + 1 : 0;
	}

	// move away from danger?
	//std::cout << Utility::DangerLevelAt(prism, prism_path[prev_index], agent->Observation()) << std::endl;
	if (Utility::DangerLevelAt(prism, prism_path[prev_index], agent->Observation()) > 0 && Utility::DangerLevelAt(prism, prism_path[prev_index], agent->Observation()) < Utility::DangerLevelAt(prism, prism_path[next_index], agent->Observation()))
		return prism_path[prev_index];
	else
		return prism_path[next_index];

}




}