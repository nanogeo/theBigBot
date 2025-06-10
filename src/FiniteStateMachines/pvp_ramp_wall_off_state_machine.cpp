#pragma once

#include "pvp_ramp_wall_off_state_machine.h"
#include "theBigBot.h"

namespace sc2 {


#pragma region PvPMainRampWallOffWaitForEnemies

void PvPMainRampWallOffWaitForEnemies::TickState()
{
	if (state_machine->probe == nullptr || state_machine->probe->is_alive == false)
	{
		state_machine->probe = nullptr;
		state_machine->probe = mediator->GetBuilder(state_machine->wall_off_pos);
		if (state_machine->probe == nullptr)
			return;
		mediator->RemoveWorker(state_machine->probe);
	}
	if (Distance2D(state_machine->probe->pos, state_machine->wall_off_pos) > 1)
		mediator->SetUnitCommand(state_machine->probe, ABILITY_ID::MOVE_MOVE, state_machine->wall_off_pos, 1);
}

void PvPMainRampWallOffWaitForEnemies::EnterState()
{
	
}

void PvPMainRampWallOffWaitForEnemies::ExitState()
{
	return;
}

State* PvPMainRampWallOffWaitForEnemies::TestTransitions()
{
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), state_machine->wall_off_pos) < 8)
		return new PvPMainRampWallOffBuildBuilding(mediator, state_machine);
	return nullptr;
}

std::string PvPMainRampWallOffWaitForEnemies::toString()
{
	return "waiting for enemies";
}

#pragma endregion

#pragma region PvPMainRampWallOffBuildBuilding

void PvPMainRampWallOffBuildBuilding::TickState()
{
	if (state_machine->probe == nullptr || state_machine->probe->is_alive == false)
	{
		state_machine->probe = nullptr;
		state_machine->probe = mediator->GetBuilder(state_machine->wall_off_pos);
		if (state_machine->probe == nullptr)
			return;
		mediator->RemoveWorker(state_machine->probe);
	}
	mediator->SetUnitCommand(state_machine->probe, ABILITY_ID::BUILD_GATEWAY, state_machine->wall_off_pos, 1);
}

void PvPMainRampWallOffBuildBuilding::EnterState()
{
	if (state_machine->probe == nullptr || state_machine->probe->is_alive == false)
	{
		state_machine->probe = nullptr;
		state_machine->probe = mediator->GetBuilder(state_machine->wall_off_pos);
		if (state_machine->probe == nullptr)
			return;
		mediator->RemoveWorker(state_machine->probe);
	}
	mediator->SetUnitCommand(state_machine->probe, ABILITY_ID::BUILD_GATEWAY, state_machine->wall_off_pos, 1);
}

void PvPMainRampWallOffBuildBuilding::ExitState()
{
	return;
}

State* PvPMainRampWallOffBuildBuilding::TestTransitions()
{
	const Unit* wall = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Self, IsNonPlaceholderUnit(GATEWAY)), state_machine->wall_off_pos);
	if (Distance2D(wall->pos, state_machine->wall_off_pos) < 1)
		return new PvPMainRampWallOffWaitForSafety(mediator, state_machine, wall, Utility::PointBetween(state_machine->wall_off_pos, mediator->GetStartLocation(), 5));
	return nullptr;
}

std::string PvPMainRampWallOffBuildBuilding::toString()
{
	return "build wall off";
}

#pragma endregion


#pragma region PvPMainRampWallOffWaitForSafety

void PvPMainRampWallOffWaitForSafety::TickState()
{
	if (state_machine->probe == nullptr || state_machine->probe->is_alive == false)
	{
		state_machine->probe = nullptr;
		state_machine->probe = mediator->GetBuilder(state_machine->wall_off_pos);
		if (state_machine->probe == nullptr)
			return;
		mediator->RemoveWorker(state_machine->probe);
	}
	if (Distance2D(state_machine->probe->pos, fallback) > 1)
		mediator->SetUnitCommand(state_machine->probe, ABILITY_ID::MOVE_MOVE, fallback, 1);
}

void PvPMainRampWallOffWaitForSafety::EnterState()
{

}

void PvPMainRampWallOffWaitForSafety::ExitState()
{
	if (wall != nullptr && wall->is_alive)
		mediator->SetUnitCommand(wall, ABILITY_ID::CANCEL, 1);
	return;
}

State* PvPMainRampWallOffWaitForSafety::TestTransitions()
{
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), state_machine->wall_off_pos) > 8)
		return new PvPMainRampWallOffWaitForEnemies(mediator, state_machine);
	return nullptr;
}

std::string PvPMainRampWallOffWaitForSafety::toString()
{
	return "waiting for safety";
}

#pragma endregion

}