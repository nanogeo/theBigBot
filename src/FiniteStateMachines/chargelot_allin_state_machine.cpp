
#include "definitions.h"
#include "chargelot_allin_state_machine.h"
#include "theBigBot.h"

namespace sc2 {



#pragma region ChargeAllInMovingToWarpinSpot

std::string ChargeAllInMovingToWarpinSpot::toString() const
{
	float time_left = state_machine->last_warp_in_time + 20 - mediator->GetGameLoop() / FRAME_TIME;
	return "ChargeAllIn looking for warp in spot " + std::to_string((int)time_left);
}

void ChargeAllInMovingToWarpinSpot::TickState()
{
	if (state_machine->prism == nullptr)
		return;
	// move prism to spot
	if (Distance2D(state_machine->prism->pos, state_machine->next_warp_in_location) > 1)
		mediator->SetUnitCommand(state_machine->prism, A_MOVE, state_machine->next_warp_in_location, CommandPriority::normal);
	return;
}

void ChargeAllInMovingToWarpinSpot::EnterState()
{
	// find warp in spot
	state_machine->next_warp_in_location = state_machine->prism_spots[state_machine->prism_spots_index];
}

void ChargeAllInMovingToWarpinSpot::ExitState()
{
	// increment warp in spot
	state_machine->prism_spots_index++;
	if (state_machine->prism_spots_index >= state_machine->prism_spots.size())
		state_machine->prism_spots_index = 0;

	mediator->SetUnitCommand(state_machine->prism, A_PRISM_PHASING_MODE, CommandPriority::normal);
}

State* ChargeAllInMovingToWarpinSpot::TestTransitions()
{
	if (state_machine->prism == nullptr)
		return nullptr;

	float time_left = state_machine->last_warp_in_time + 20 - mediator->GetCurrentTime();
	if (time_left < 2 && (Distance2D(state_machine->prism->pos, state_machine->next_warp_in_location) < 1 ||
		mediator->IsPathable(state_machine->prism->pos) &&
		mediator->IsPathable(state_machine->prism->pos + Point2D(0, 2)) &&
		mediator->IsPathable(state_machine->prism->pos + Point2D(0, -2)) &&
		mediator->IsPathable(state_machine->prism->pos + Point2D(2, 0)) &&
		mediator->IsPathable(state_machine->prism->pos + Point2D(-2, 0))))
		return new ChargeAllInWarpingIn(mediator, state_machine, mediator->GetCurrentTime());
	return nullptr;
}

#pragma endregion

#pragma region ChargeAllInWarpingIn

std::string ChargeAllInWarpingIn::toString() const
{
	return "ChargeAllIn warping in";
}

void ChargeAllInWarpingIn::TickState()
{
	if (state_machine->prism == nullptr || done_warp_in)
		return;

	if (state_machine->prism->unit_type != PRISM_SIEGED)
	{
		mediator->SetUnitCommand(state_machine->prism, A_PRISM_PHASING_MODE, CommandPriority::high);
		return;
	}
	Units gates = mediator->GetUnits(IsFinishedUnit(WARP_GATE));
	int num_gates_ready = mediator->GetNumWarpgatesReady();
	int num_zealots_afforded = mediator->MaxCanAfford(ZEALOT);

	if (num_zealots_afforded > 0 &&
		num_gates_ready >= std::min(num_zealots_afforded, (int)gates.size()) &&
		mediator->WarpInUnitsAt(ZEALOT, std::min(num_zealots_afforded, num_gates_ready), state_machine->prism->pos))
	{
		state_machine->last_warp_in_time = mediator->GetCurrentTime();
		done_warp_in = true;
	}
}

void ChargeAllInWarpingIn::EnterState()
{
	if (state_machine->prism->unit_type != PRISM_SIEGED)
		mediator->SetUnitCommand(state_machine->prism, A_PRISM_PHASING_MODE, CommandPriority::normal);
}

void ChargeAllInWarpingIn::ExitState()
{
	mediator->SetUnitCommand(state_machine->prism, A_PRISM_TRANSPORT_MODE, CommandPriority::normal);
	if (!state_machine->first_warp_in_done)
		state_machine->first_warp_in_done = true;
}

State* ChargeAllInWarpingIn::TestTransitions()
{
	if (state_machine->prism == nullptr)
		return nullptr;

	float time_left = state_machine->last_warp_in_time + 20 - mediator->GetCurrentTime();
	if (time_left < 16 && time_left > 10)
		return new ChargeAllInMovingToWarpinSpot(mediator, state_machine);
	if (time_left < 0 && mediator->GetCurrentTime() - time_arrived > 5)
	{
		state_machine->last_warp_in_time = mediator->GetCurrentTime() - 10;
		return new ChargeAllInMovingToWarpinSpot(mediator, state_machine);
	}
	return nullptr;
}

#pragma endregion


void ChargelotAllInStateMachine::RunStateMachine()
{
	StateMachine::RunStateMachine();

	if (first_warp_in_done)
	{
		for (const auto& zealot : zealots)
		{
			if (zealot->orders.size() == 0)
			{
				const Unit* closest_base = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy,
					IsUnits({ HATCHERY, LAIR, HIVE, COMMAND_CENTER, PLANETARY, ORBITAL, NEXUS })), zealot->pos);
				if (closest_base != nullptr)
				{
					mediator->SetUnitCommand(zealot, A_ATTACK, closest_base->pos, CommandPriority::normal);
				}
				else
				{
					const Unit* closest_building = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy, IsGroundBuilding()), zealot->pos);
					if (closest_building != nullptr)
					{
						mediator->SetUnitCommand(zealot, A_ATTACK, closest_building->pos, CommandPriority::normal);
					}
					else if (attached_army_group != nullptr)
					{
						attached_army_group->ScourMap();
						return;
					}
				}
			}
		}
	}
	else
	{
		if (prism != nullptr && prism->is_alive)
		{
			for (const auto& zealot : zealots)
			{
				if (zealot->orders.size() == 0)
					mediator->SetUnitCommand(zealot, A_ATTACK, next_warp_in_location, CommandPriority::normal);
			}
		}
	}
}


bool ChargelotAllInStateMachine::AddUnit(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case ZEALOT:
		zealots.push_back(unit);
		return true;
	case PRISM:
		if (prism == nullptr || prism->is_alive == false)
		{
			prism = unit;
			mediator->CancelWarpgateUnitProduction();
			return true;
		}
	}
	return false;
}

void ChargelotAllInStateMachine::RemoveUnit(const Unit* unit)
{
	if (unit == prism)
	{
		mediator->SetUnitProduction(ZEALOT);
		prism = nullptr;
	}
	else
	{
		zealots.erase(std::remove(zealots.begin(), zealots.end(), unit), zealots.end());
	}
}

}