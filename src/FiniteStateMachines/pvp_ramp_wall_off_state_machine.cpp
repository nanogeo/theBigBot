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
		mediator->SetUnitCommand(state_machine->probe, ABILITY_ID::MOVE_MOVE, state_machine->wall_off_pos, CommandPriorty::normal);
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

std::string PvPMainRampWallOffWaitForEnemies::toString() const
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
	TryActionResult result = mediator->TryBuildBuilding(state_machine->probe, BATTERY, state_machine->wall_off_pos);

	//mediator->SetUnitCommand(state_machine->probe, ABILITY_ID::BUILD_SHIELDBATTERY, state_machine->wall_off_pos, 1);
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
	TryActionResult result = mediator->TryBuildBuilding(state_machine->probe, BATTERY, state_machine->wall_off_pos);
	
	//mediator->SetUnitCommand(state_machine->probe, ABILITY_ID::BUILD_SHIELDBATTERY, state_machine->wall_off_pos, 1);
}

void PvPMainRampWallOffBuildBuilding::ExitState()
{
	return;
}

State* PvPMainRampWallOffBuildBuilding::TestTransitions()
{
	const Unit* wall = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Self, IsNonPlaceholderUnit(BATTERY)), state_machine->wall_off_pos);
	if (wall != nullptr && Distance2D(wall->pos, state_machine->wall_off_pos) < 1)
		return new PvPMainRampWallOffWaitForSafety(mediator, state_machine, wall, Utility::PointBetween(state_machine->wall_off_pos, mediator->GetStartLocation(), 5));
	return nullptr;
}

std::string PvPMainRampWallOffBuildBuilding::toString() const
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
		mediator->SetUnitCommand(state_machine->probe, ABILITY_ID::MOVE_MOVE, fallback, CommandPriorty::normal);
}

void PvPMainRampWallOffWaitForSafety::EnterState()
{
	build_time = mediator->GetCurrentTime();
}

void PvPMainRampWallOffWaitForSafety::ExitState()
{
	if (wall != nullptr && wall->is_alive)
	{
		mediator->SetUnitCommand(wall, ABILITY_ID::CANCEL, CommandPriorty::normal);
		mediator->AddBuildingToDoNotRebuild(wall->pos);
	}
	return;
}

State* PvPMainRampWallOffWaitForSafety::TestTransitions()
{
	if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy), state_machine->wall_off_pos) > 8 && mediator->GetCurrentTime() > build_time + 25)
		return new PvPMainRampWallOffWaitForEnemies(mediator, state_machine);
	return nullptr;
}

std::string PvPMainRampWallOffWaitForSafety::toString() const
{
	return "waiting for safety";
}

#pragma endregion


PvPMainRampWallOffStateMachine::PvPMainRampWallOffStateMachine(Mediator* mediator, std::string name, const Unit* probe, Point2D wall_off_pos) : StateMachine(mediator, name)
{
	this->probe = probe;
	this->wall_off_pos = wall_off_pos;

	current_state = new PvPMainRampWallOffWaitForEnemies(mediator, this);
	current_state->EnterState();
}

PvPMainRampWallOffStateMachine::~PvPMainRampWallOffStateMachine()
{
	if (probe != nullptr)
		mediator->PlaceWorker(probe);
}

}