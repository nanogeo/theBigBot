
#include "definitions.h"
#include "chargelot_allin_state_machine.h"
#include "theBigBot.h"

namespace sc2 {



#pragma region ChargeAllInMovingToWarpinSpot

std::string ChargeAllInMovingToWarpinSpot::toString()
{
	float time_left = state_machine->last_warp_in_time + 20 - agent->Observation()->GetGameLoop() / FRAME_TIME;
	return "ChargeAllIn looking for warp in spot " + std::to_string((int)time_left);
}

void ChargeAllInMovingToWarpinSpot::TickState()
{
	if (state_machine->prism == nullptr)
		return;
	// move prism to spot
	if (Distance2D(state_machine->prism->pos, state_machine->next_warp_in_location) > 1)
		agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->next_warp_in_location, 1);
	return;
}

void ChargeAllInMovingToWarpinSpot::EnterState()
{
	// find warp in spot
	state_machine->prism_spots_index++;
	if (state_machine->prism_spots_index >= state_machine->prism_spots.size())
		state_machine->prism_spots_index = 0;
	state_machine->next_warp_in_location = state_machine->prism_spots[state_machine->prism_spots_index];
}

void ChargeAllInMovingToWarpinSpot::ExitState()
{
	agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE, 1);
}

State* ChargeAllInMovingToWarpinSpot::TestTransitions()
{
	if (state_machine->prism == nullptr)
		return nullptr;

	float time_left = state_machine->last_warp_in_time + 20 - agent->mediator.GetCurrentTime();
	if (time_left < 2 && agent->mediator.IsPathable(state_machine->prism->pos))
		return new ChargeAllInWarpingIn(agent, state_machine);
	return nullptr;
}

#pragma endregion

#pragma region ChargeAllInWarpingIn

std::string ChargeAllInWarpingIn::toString()
{
	return "ChargeAllIn warping in";
}

void ChargeAllInWarpingIn::TickState()
{
	if (state_machine->prism == nullptr)
		return;

	if (state_machine->prism->unit_type != PRISM_SIEGED)
	{
		agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE, 10);
		return;
	}
	Units gates = agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
	bool all_gates_ready = agent->mediator.GetNumWarpgatesReady() == gates.size();

	if (all_gates_ready && agent->mediator.WarpInUnitsAt(ZEALOT, gates.size(), state_machine->prism->pos))
	{
		state_machine->last_warp_in_time = agent->mediator.GetCurrentTime();
	}
}

void ChargeAllInWarpingIn::EnterState()
{
	if (state_machine->prism->unit_type != UNIT_TYPEID::PROTOSS_WARPPRISMPHASING)
		agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE, 1);
}

void ChargeAllInWarpingIn::ExitState()
{
	agent->mediator.SetUnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMTRANSPORTMODE, 1);
}

State* ChargeAllInWarpingIn::TestTransitions()
{
	if (state_machine->prism == nullptr)
		return nullptr;

	float time_left = state_machine->last_warp_in_time + 20 - agent->mediator.GetCurrentTime();
	if (time_left < 16 && time_left > 10)
		return new ChargeAllInMovingToWarpinSpot(agent, state_machine);
	// if last warp in time >3 and < 8
	// return new ChargeAllInLookingForWarpinSpot)(
	return nullptr;
}

#pragma endregion


void ChargelotAllInStateMachine::RunStateMachine()
{
	StateMachine::RunStateMachine();

	for (const auto& zealot : zealots)
	{
		if (zealot->orders.size() == 0)
		{
			const Unit* closest_base = Utility::ClosestTo(agent->mediator.GetUnits(Unit::Alliance::Enemy,
				IsUnits({ HATCHERY, LAIR, HIVE, COMMAND_CENTER, PLANETARY, ORBITAL, NEXUS })), zealot->pos);
			if (closest_base != nullptr)
			{
				agent->mediator.SetUnitCommand(zealot, ABILITY_ID::ATTACK_ATTACK, closest_base->pos, 1);
			}
			else
			{
				const Unit* closest_building = Utility::ClosestTo(agent->mediator.GetUnits(Unit::Alliance::Enemy, IsBuilding()), zealot->pos);
				if (closest_building != nullptr)
				{
					agent->mediator.SetUnitCommand(zealot, ABILITY_ID::ATTACK_ATTACK, closest_building->pos, 1);
				}
				else
				{
					Point2D pos = agent->Observation()->GetGameInfo().enemy_start_locations[0]; // TODO scour map instead
					agent->mediator.SetUnitCommand(zealot, ABILITY_ID::ATTACK_ATTACK, pos, 1);
				}
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
			agent->mediator.CancelWarpgateUnitProduction();
			return true;
		}
	}
	return false;
}

void ChargelotAllInStateMachine::RemoveUnit(const Unit* unit)
{
	if (unit == prism)
	{
		agent->mediator.SetUnitProduction(ZEALOT);
		prism = nullptr;
	}
}

}