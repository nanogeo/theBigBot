

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
	// move prism to spot
	if (Distance2D(state_machine->prism->pos, state_machine->next_warp_in_location) > 1)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::GENERAL_MOVE, state_machine->next_warp_in_location);
	for (const auto& zealot : state_machine->zealots)
	{
		if (zealot->orders.size() == 0)
		{
			Point2D pos = agent->Observation()->GetGameInfo().enemy_start_locations[0];
			agent->Actions()->UnitCommand(zealot, ABILITY_ID::ATTACK_ATTACK, pos);
		}
	}
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
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE);
}

State* ChargeAllInMovingToWarpinSpot::TestTransitions()
{
	float time_left = state_machine->last_warp_in_time + 20 - agent->Observation()->GetGameLoop() / FRAME_TIME;
	if (time_left < 2)
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
	/*for (const auto& zealot : state_machine->zealots)
	{
		if (zealot->orders.size() == 0)
		{
			Point2D pos = agent->Observation()->GetGameInfo().enemy_start_locations[0];
			agent->Actions()->UnitCommand(zealot, ABILITY_ID::ATTACK_ATTACK, pos);
		}
	}

	bool all_gates_ready = true;
	Units gates = agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
	for (const auto& warpgate : gates)
	{
		if (agent->warpgate_status[warpgate].frame_ready > 0)
		{
			all_gates_ready = false;
			break;
		}
	}
	if (gates.size() > 0 && all_gates_ready && Utility::CanAfford(UNIT_TYPEID::PROTOSS_ZEALOT, gates.size(), agent->Observation()))
	{
		std::vector<Point2D> spots = agent->FindWarpInSpots(agent->Observation()->GetGameInfo().enemy_start_locations[0], gates.size());
		if (spots.size() >= gates.size())
		{
			for (int i = 0; i < gates.size(); i++)
			{
				Point3D pos = Point3D(gates[i]->pos.x, gates[i]->pos.y, agent->Observation()->TerrainHeight(gates[i]->pos));
				//agent->Debug()->DebugSphereOut(pos, 1, Color(255, 0, 255));
				agent->Actions()->UnitCommand(gates[i], ABILITY_ID::TRAINWARP_ZEALOT, spots[i]);
				agent->warpgate_status[gates[i]].used = true;
				agent->warpgate_status[gates[i]].frame_ready = agent->Observation()->GetGameLoop() + round(20 * FRAME_TIME);
				state_machine->last_warp_in_time = agent->Observation()->GetGameLoop() / FRAME_TIME;
			}
		}
	}*/
}

void ChargeAllInWarpingIn::EnterState()
{
	if (state_machine->prism->unit_type != UNIT_TYPEID::PROTOSS_WARPPRISMPHASING)
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE);
}

void ChargeAllInWarpingIn::ExitState()
{
	agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMTRANSPORTMODE);
}

State* ChargeAllInWarpingIn::TestTransitions()
{
	float time_left = state_machine->last_warp_in_time + 20 - agent->Observation()->GetGameLoop() / FRAME_TIME;
	if (time_left < 16 && time_left > 10)
		return new ChargeAllInMovingToWarpinSpot(agent, state_machine);
	// if last warp in time >3 and < 8
	// return new ChargeAllInLookingForWarpinSpot)(
	return nullptr;
}

#pragma endregion

}