
#include "immortal_drop_state_machine.h"
#include "theBigBot.h"


namespace sc2 {



#pragma region ImmortalDropWaitForImmortals

#pragma warning(push)
#pragma warning(disable : 4702)
void ImmortalDropWaitForImmortals::TickState()
{
	if (state_machine->prism == nullptr)
	{
		for (const auto &prism : mediator->GetUnits(IsFriendlyUnit(PRISM)))
		{
			state_machine->prism = prism;
			break;
		}
	}
	if (state_machine->immortal1 == nullptr || state_machine->immortal2 == nullptr)
	{
		for (const auto &immortal : mediator->GetUnits(IsFriendlyUnit(IMMORTAL)))
		{
			if (state_machine->immortal1 == nullptr && immortal != state_machine->immortal2)
				state_machine->immortal1 = immortal;
			else if (state_machine->immortal2 == nullptr && immortal != state_machine->immortal1)
				state_machine->immortal2 = immortal;
		}
	}
}
#pragma warning(pop)

void ImmortalDropWaitForImmortals::EnterState()
{
	return;
}

void ImmortalDropWaitForImmortals::ExitState()
{
	mediator->SetUnitCommand(state_machine->immortal1, A_SMART, state_machine->prism, 0);
	mediator->SetUnitCommand(state_machine->immortal2, A_SMART, state_machine->prism, 0);
	return;
}

State* ImmortalDropWaitForImmortals::TestTransitions()
{
	if (state_machine->prism != nullptr && state_machine->immortal1 != nullptr  && state_machine->immortal2 != nullptr)
		return new ImmortalDropInitialMove(mediator, state_machine);
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
	mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->entry_pos, 0);
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
		return new ImmortalDropMicroDrop(mediator, state_machine);
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
	mediator->SetUnitCommand(state_machine->prism, A_UNLOAD_AT, state_machine->prism, 0);
	mediator->SetUnitCommand(state_machine->immortal1, A_MOVE, Utility::PointBetween(state_machine->immortal1->pos, state_machine->prism->pos, 1), 0);
	mediator->SetUnitCommand(state_machine->immortal2, A_MOVE, Utility::PointBetween(state_machine->immortal2->pos, state_machine->prism->pos, 1), 0);
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
		return new ImmortalDropMicroDropDropped2(mediator, state_machine);
	return nullptr;
}

std::string ImmortalDropMicroDrop::toString()
{
	return "micro immortal drop";
}

#pragma endregion

#pragma region ImmortalDropMicroDropCarrying1

ImmortalDropMicroDropCarrying1::ImmortalDropMicroDropCarrying1(Mediator* mediator, ImmortalDropStateMachine* state_machine)
{
	this->mediator = mediator;
	this->state_machine = state_machine;
	entry_frame = mediator->GetGameLoop();
}

void ImmortalDropMicroDropCarrying1::TickState()
{
	// don't leave immortals behind
	if (Distance2D(state_machine->prism->pos, state_machine->immortal2->pos) > 4.5f)
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->immortal2->pos, 0);
	else
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->UpdatePrismPathing(), 0);

	if (mediator->GetGameLoop() >= entry_frame + 15)
		mediator->SetUnitCommand(state_machine->prism, A_UNLOAD_AT, state_machine->prism, 0);
	if (state_machine->immortal1->orders.size() == 0 || state_machine->immortal1->orders[0].ability_id != A_SMART)
		mediator->SetUnitCommand(state_machine->immortal1, A_MOVE, Utility::PointBetween(state_machine->immortal1->pos, state_machine->prism->pos, 1), 0);
	if (state_machine->immortal2->orders.size() == 0 || state_machine->immortal2->orders[0].ability_id != A_SMART)
		mediator->SetUnitCommand(state_machine->immortal2, A_MOVE, Utility::PointBetween(state_machine->immortal2->pos, state_machine->prism->pos, 1), 0);
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
		return new ImmortalDropMicroDropDropped1(mediator, state_machine);
	return nullptr;
}

std::string ImmortalDropMicroDropCarrying1::toString()
{
	return "micro immortal drop carrying 1";
}

#pragma endregion

#pragma region ImmortalDropMicroDropCarrying2

ImmortalDropMicroDropCarrying2::ImmortalDropMicroDropCarrying2(Mediator* mediator, ImmortalDropStateMachine* state_machine)
{
	this->mediator = mediator;
	this->state_machine = state_machine;
	entry_frame = mediator->GetGameLoop();
}

void ImmortalDropMicroDropCarrying2::TickState()
{
	// don't leave immortals behind
	if (Distance2D(state_machine->prism->pos, state_machine->immortal1->pos) > 4.5)
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->immortal1->pos, 0);
	else
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->UpdatePrismPathing(), 0);

	if (mediator->GetGameLoop() >= entry_frame + 15)
		mediator->SetUnitCommand(state_machine->prism, A_UNLOAD_AT, state_machine->prism, 0);
	if (state_machine->immortal1->orders.size() == 0 || state_machine->immortal1->orders[0].ability_id != A_SMART)
		mediator->SetUnitCommand(state_machine->immortal1, A_MOVE, Utility::PointBetween(state_machine->immortal1->pos, state_machine->prism->pos, 1), 0);
	if (state_machine->immortal2->orders.size() == 0 || state_machine->immortal2->orders[0].ability_id != A_SMART)
		mediator->SetUnitCommand(state_machine->immortal2, A_MOVE, Utility::PointBetween(state_machine->immortal2->pos, state_machine->prism->pos, 1), 0);
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
		return new ImmortalDropMicroDropDropped2(mediator, state_machine);
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
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->immortal1->pos, 0);
	else if (Distance2D(state_machine->prism->pos, state_machine->immortal2->pos) > 4.5)
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->immortal2->pos, 0);
	else
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->UpdatePrismPathing(), 0);

	if ((!immortal1_has_attack_order || state_machine->immortal1->weapon_cooldown > 0) && (!immortal2_has_attack_order || state_machine->immortal2->weapon_cooldown > 0))
		mediator->SetUnitCommand(state_machine->immortal2, A_SMART, state_machine->prism, 0);
}

void ImmortalDropMicroDropDropped1::EnterState()
{
	/*std::map<const Unit*, const Unit*> attacks = agent->FindTargets({ state_machine->immortal1, state_machine->immortal2 }, state_machine->target_priority, 0);
	if (attacks.count(state_machine->immortal1))
	{
		mediator->SetUnitCommand(state_machine->immortal1, A_ATTACK, attacks[state_machine->immortal1], 0);
		immortal1_has_attack_order = true;
	}

	if (attacks.count(state_machine->immortal2))
	{
		mediator->SetUnitCommand(state_machine->immortal2, A_ATTACK, attacks[state_machine->immortal2], 0);
		immortal2_has_attack_order = true;
	}*/
}

void ImmortalDropMicroDropDropped1::ExitState()
{
		
}

State* ImmortalDropMicroDropDropped1::TestTransitions()
{
	if (state_machine->prism->cargo_space_taken == 4)
		return new ImmortalDropMicroDropCarrying2(mediator, state_machine);
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
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->immortal1->pos, 0);
	else if (Distance2D(state_machine->prism->pos, state_machine->immortal2->pos) > 4.5)
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->immortal2->pos, 0);
	else
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->UpdatePrismPathing(), 0);

	if ((!immortal1_has_attack_order || state_machine->immortal1->weapon_cooldown > 0) && (!immortal2_has_attack_order || state_machine->immortal2->weapon_cooldown > 0))
		mediator->SetUnitCommand(state_machine->immortal1, A_SMART, state_machine->prism, 0);
}

void ImmortalDropMicroDropDropped2::EnterState()
{
	/*std::map<const Unit*, const Unit*> attacks = agent->FindTargets({ state_machine->immortal1, state_machine->immortal2 }, state_machine->target_priority, 0);
	if (attacks.count(state_machine->immortal1))
	{
		mediator->SetUnitCommand(state_machine->immortal1, A_ATTACK, attacks[state_machine->immortal1], 0);
		immortal1_has_attack_order = true;
	}

	if (attacks.count(state_machine->immortal2))
	{
		mediator->SetUnitCommand(state_machine->immortal2, A_ATTACK, attacks[state_machine->immortal2], 0);
		immortal2_has_attack_order = true;
	}*/
}

void ImmortalDropMicroDropDropped2::ExitState()
{
	mediator->SetUnitCommand(state_machine->immortal1, A_SMART, state_machine->prism, 0);
}

State* ImmortalDropMicroDropDropped2::TestTransitions()
{
	if (state_machine->prism->cargo_space_taken == 4)
		return new ImmortalDropMicroDropCarrying1(mediator, state_machine);
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
	if (mediator->GetUnits(IsUnit(SIEGE_TANK_SIEGED)).size() > 0)
	{
		return mediator->GetUnits(IsUnit(SIEGE_TANK_SIEGED))[0]->pos;
	}
	if (mediator->GetUnits(IsUnit(SIEGE_TANK)).size() > 0)
	{
		return mediator->GetUnits(IsUnit(SIEGE_TANK))[0]->pos;
	}
	// if theres a cyclone then go ontop of it?
	if (mediator->GetUnits(IsUnit(CYCLONE)).size() > 0)
	{
		return mediator->GetUnits(IsUnit(CYCLONE))[0]->pos;
	}

	int prev_index = path_index - 1 >= 0 ? path_index - 1 : (int)prism_path.size() - 1;
	int next_index = path_index + 1 < (int)prism_path.size() ? path_index + 1 : 0;

	// update path index
	if (Distance2D(prism->pos, prism_path[prev_index]) < 2)
	{
		path_index = prev_index;
		prev_index = path_index - 1 >= 0 ? path_index - 1 : (int)prism_path.size() - 1;
		next_index = path_index + 1 < (int)prism_path.size() ? path_index + 1 : 0;
	}
	else if (Distance2D(prism->pos, prism_path[next_index]) < 2)
	{
		path_index = next_index;
		prev_index = path_index - 1 >= 0 ? path_index - 1 : (int)prism_path.size() - 1;
		next_index = path_index + 1 < (int)prism_path.size() ? path_index + 1 : 0;
	}

	// move away from danger?
	//std::cout << Utility::DangerLevelAt(prism, prism_path[prev_index], agent->Observation()) << std::endl;
	//if (Utility::DangerLevelAt(prism, prism_path[prev_index], agent->Observation()) > 0 && Utility::DangerLevelAt(prism, prism_path[prev_index], agent->Observation()) < Utility::DangerLevelAt(prism, prism_path[next_index], agent->Observation()))
	//	return prism_path[prev_index];
	//else
		return prism_path[next_index];

}




}