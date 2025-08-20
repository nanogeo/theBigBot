
#include "definitions.h"
#include "oracle_harass_state_machine.h"
#include "theBigBot.h"

namespace sc2 {

#pragma region OracleDefendLocation


OracleDefendLocation::OracleDefendLocation(Mediator* mediator, OracleHarassStateMachine* state_machine, Point2D denfensive_position)
{
	this->mediator = mediator;
	this->state_machine = state_machine;
	this->denfensive_position = denfensive_position;
	event_id = mediator->GetUniqueId();
}

std::string OracleDefendLocation::toString() const
{
	return "Oracle Defend Location";
}

void OracleDefendLocation::TickState()
{
	Units enemy_units = mediator->GetUnits(Unit::Alliance::Enemy, IsNotFlyingUnit());
	if (enemy_units.size() > 0 && Utility::DistanceToClosest(enemy_units, denfensive_position) < 10)
	{
		for (const auto &oracle : state_machine->oracles)
		{
			float now = mediator->GetGameLoop() / FRAME_TIME;
			bool weapon_ready = now - state_machine->time_last_attacked[oracle] > .61;
			bool beam_active = mediator->IsOracleBeamActive(oracle);
			bool beam_activatable = false;

			if (!beam_active && oracle->energy >= 40)
				beam_activatable = true;

			const Unit* closest_unit = Utility::ClosestTo(enemy_units, oracle->pos);
			if (beam_active)
			{
				if (Distance2D(oracle->pos, closest_unit->pos) > 4)
				{
					float dist = Distance2D(oracle->pos, closest_unit->pos);
					mediator->SetUnitCommand(oracle, A_MOVE, Utility::PointBetween(oracle->pos, closest_unit->pos, dist + 1), CommandPriority::low);
				}
				else if (weapon_ready)
				{
					mediator->SetUnitCommand(oracle, A_ATTACK, closest_unit, CommandPriority::low);
					state_machine->time_last_attacked[oracle] = now;
					state_machine->has_attacked[oracle] = false;
					//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 255, 0));
				}
				else if (state_machine->has_attacked[oracle])
				{
					mediator->SetUnitCommand(oracle, A_MOVE, closest_unit->pos, CommandPriority::low);
					//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 0, 255));
				}
				else
				{
					//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
				}
			}
			else if (beam_activatable)
			{
				if (Distance2D(oracle->pos, closest_unit->pos) < 3)
				{
					mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_ON, CommandPriority::low);
				}
				else
				{
					mediator->SetUnitCommand(oracle, A_MOVE, closest_unit->pos, CommandPriority::low);
				}
			}
			else
			{
				mediator->SetUnitCommand(oracle, A_MOVE, closest_unit->pos, CommandPriority::low);
				//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
			}
			//agent->Debug()->DebugTextOut(std::to_string(now - state_machine->time_last_attacked[oracle]), Point2D(.7, .7), Color(0, 255, 255), 20);
			//agent->Debug()->DebugTextOut(std::to_string(agent->Observation()->GetGameLoop()), Point2D(.7, .75), Color(0, 255, 255), 20);

		}
	}
	else
	{
		for (const auto &oracle : state_machine->oracles)
		{
			if (mediator->IsOracleBeamActive(oracle))
			{
				mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, CommandPriority::low);
			}

			if (Distance2D(oracle->pos, denfensive_position) > 1)
				mediator->SetUnitCommand(oracle, A_MOVE, denfensive_position, CommandPriority::low);
		}
	}


}

void OracleDefendLocation::EnterState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		mediator->SetUnitCommand(oracle, A_MOVE, denfensive_position, CommandPriority::low);
	}
	std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
		this->OnUnitDamagedListener(unit, health, shields);
	};
	mediator->AddListenerToOnUnitDamagedEvent(event_id, onUnitDamaged);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);
}

void OracleDefendLocation::ExitState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		// remove event onUnitDamaged
		mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, CommandPriority::low);
	}
	mediator->RemoveListenerToOnUnitDamagedEvent(event_id);
	mediator->RemoveListenerToOnUnitDestroyedEvent(event_id);
	return;
}

State* OracleDefendLocation::TestTransitions()
{
	if (state_machine->oracles.size() > 1) //> 1
	{
		if (true) //(agent->Observation()->GetGameLoop() % 2 == 0)
		{
			state_machine->harass_direction = true;
			state_machine->harass_index = 0;
			return new OracleHarassGroupUp(mediator, state_machine, mediator->GetLocations().oracle_path.entrance_point);
		}
		else
		{
			state_machine->harass_direction = false;
			state_machine->harass_index = (int)mediator->GetLocations().oracle_path.entrance_points.size() - 1;
			return new OracleHarassGroupUp(mediator, state_machine, mediator->GetLocations().oracle_path.exit_point);
		}
	}
	return nullptr;
}

#pragma warning(push)
#pragma warning(disable : 4100)
void OracleDefendLocation::OnUnitDamagedListener(const Unit* unit, float health, float shields)
{
	//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage\n";
	for (const auto &oracle : state_machine->oracles)
	{
		if (oracle->engaged_target_tag == unit->tag)
		{
			//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage from orale\n";
			state_machine->has_attacked[oracle] = true;
		}
	}
}
#pragma warning(pop)

void OracleDefendLocation::OnUnitDestroyedListener(const Unit* unit)
{
	//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " destroyed\n";
	for (const auto &oracle : state_machine->oracles)
	{
		if (oracle->engaged_target_tag == unit->tag)
		{
			//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " destroyed by orale\n";
			state_machine->has_attacked[oracle] = true;
		}
	}
}


#pragma endregion

#pragma region OracleDefendLine

OracleDefendLine::OracleDefendLine(Mediator* mediator, OracleHarassStateMachine* state_machine, Point2D start, Point2D end)
{
	this->mediator = mediator;
	this->state_machine = state_machine;

	double slope = (start.y - end.y) / (start.x - end.x);

	double line_a = slope;
	double line_b = start.y - (slope * start.x);

	line = new LineSegmentLinearX((float)line_a, (float)line_b, start.x, end.x);

	event_id = mediator->GetUniqueId();
}

std::string OracleDefendLine::toString() const
{
	return "Oracle Defend Line";
}

void OracleDefendLine::TickState()
{
	Units enemy_units = mediator->GetUnits(Unit::Alliance::Enemy, IsNotFlyingUnit());
	for (int i = 0; i < enemy_units.size(); i++) // remove any enemies too far from defense location
	{
		Point2D closest_pos = line->FindClosestPoint(enemy_units[i]->pos);
		if (Distance2D(enemy_units[i]->pos, closest_pos) > 10)
		{
			enemy_units.erase(enemy_units.begin() + i);
			i--;
		}
	}
	if (enemy_units.size() > 0)
	{
		for (const auto& oracle : state_machine->oracles)
		{
			float now = mediator->GetGameLoop() / FRAME_TIME;
			bool weapon_ready = now - state_machine->time_last_attacked[oracle] > .61;
			bool beam_active = mediator->IsOracleBeamActive(oracle);
			bool beam_activatable = false;

			if (!beam_active && oracle->energy >= 40 && enemy_units.size() > 5)
				beam_activatable = true;

			const Unit* closest_unit = Utility::ClosestTo(enemy_units, oracle->pos);
			if (beam_active)
			{
				if (Distance2D(oracle->pos, closest_unit->pos) > 4)
				{
					float dist = Distance2D(oracle->pos, closest_unit->pos);
					mediator->SetUnitCommand(oracle, A_MOVE, Utility::PointBetween(oracle->pos, closest_unit->pos, dist + 1), CommandPriority::low);
				}
				else if (weapon_ready)
				{
					mediator->SetUnitCommand(oracle, A_ATTACK, closest_unit, CommandPriority::low);
					state_machine->time_last_attacked[oracle] = now;
					state_machine->has_attacked[oracle] = false;
					//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 255, 0));
				}
				else if (state_machine->has_attacked[oracle])
				{
					if ((mediator->GetUnit(oracle->engaged_target_tag) == nullptr ||
						Distance2D(oracle->pos, mediator->GetUnit(oracle->engaged_target_tag)->pos) > 3) ||
						Distance2D(oracle->pos, closest_unit->pos) > 3)  // only move if target is getting away
						mediator->SetUnitCommand(oracle, A_MOVE, closest_unit->pos, CommandPriority::low);
					//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 0, 255));
				}
				else
				{
					//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
				}
			}
			else if (beam_activatable)
			{
				if (Distance2D(oracle->pos, closest_unit->pos) < 2)
				{
					mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_ON, CommandPriority::low);
				}
				else
				{
					mediator->SetUnitCommand(oracle, A_MOVE, closest_unit->pos, CommandPriority::low);
				}
			}
			else
			{
				mediator->SetUnitCommand(oracle, A_MOVE, closest_unit->pos, CommandPriority::low);
				//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
			}
			//agent->Debug()->DebugTextOut(std::to_string(now - state_machine->time_last_attacked[oracle]), Point2D(.7, .7), Color(0, 255, 255), 20);
			//agent->Debug()->DebugTextOut(std::to_string(agent->Observation()->GetGameLoop()), Point2D(.7, .75), Color(0, 255, 255), 20);

		}
	}
	else
	{
		for (const auto& oracle : state_machine->oracles)
		{
			if (mediator->IsOracleBeamActive(oracle))
			{
				mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, CommandPriority::low);
				continue;
			}

			double dist_to_start = Distance2D(oracle->pos, line->GetStartPoint());
			double dist_to_end = Distance2D(oracle->pos, line->GetEndPoint());

			if (oracle->orders.size() > 0 &&
				(oracle->orders[0].target_pos == line->GetStartPoint() || oracle->orders[0].target_pos == line->GetEndPoint()))
			{
				continue;
			}
			else if (dist_to_start < 1)
			{
				mediator->SetUnitCommand(oracle, A_MOVE, line->GetEndPoint(), CommandPriority::low);
			}
			else if (dist_to_end < 1)
			{
				mediator->SetUnitCommand(oracle, A_MOVE, line->GetStartPoint(), CommandPriority::low);
			}
			else
			{
				if (dist_to_end < dist_to_start)
					mediator->SetUnitCommand(oracle, A_MOVE, line->GetEndPoint(), CommandPriority::low);
				else
					mediator->SetUnitCommand(oracle, A_MOVE, line->GetStartPoint(), CommandPriority::low);
			}
		}
	}


}

void OracleDefendLine::EnterState()
{
	for (const auto& oracle : state_machine->oracles)
	{
		mediator->SetUnitCommand(oracle, A_MOVE, line->GetStartPoint(), CommandPriority::low);
	}
	std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
		this->OnUnitDamagedListener(unit, health, shields);
	};
	mediator->AddListenerToOnUnitDamagedEvent(event_id, onUnitDamaged);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	/*std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
		this->OnUnitCreatedListener(unit);
	};
	agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated)*/
}

void OracleDefendLine::ExitState()
{
	for (const auto& oracle : state_machine->oracles)
	{
		// remove event onUnitDamaged
		mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, CommandPriority::low);
	}
	mediator->RemoveListenerToOnUnitDamagedEvent(event_id);
	mediator->RemoveListenerToOnUnitDestroyedEvent(event_id);
	//agent->RemoveListenerToOnUnitCreatedEvent(event_id);
	return;
}

State* OracleDefendLine::TestTransitions()
{
	if (state_machine->oracles.size() > 1 && state_machine->sent_harass == false) 
	{
		bool low_energy = false;
		for (const auto& oracle : state_machine->oracles)
		{
			if (oracle->energy < 25)
			{
				low_energy = true;
				break;
			}
		}
		if (low_energy || mediator->GetOngoingAttacks().size() > 0)
			return nullptr;

		if (mediator->GetGameLoop() % 2 == 0)
		{
			state_machine->harass_direction = true;
			state_machine->harass_index = 0;
			return new OracleHarassGroupUp(mediator, state_machine, mediator->GetLocations().oracle_path.entrance_point);
		}
		else
		{
			state_machine->harass_direction = false;
			state_machine->harass_index = (int)mediator->GetLocations().oracle_path.entrance_points.size() - 1;
			return new OracleHarassGroupUp(mediator, state_machine, mediator->GetLocations().oracle_path.exit_point);
		}
	}
	return nullptr;
}

#pragma warning(push)
#pragma warning(disable : 4100)
void OracleDefendLine::OnUnitDamagedListener(const Unit* unit, float health, float shields)
{
	//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage\n";
	for (const auto& oracle : state_machine->oracles)
	{
		if (oracle->engaged_target_tag == unit->tag)
		{
			//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage from orale\n";
			state_machine->has_attacked[oracle] = true;
		}
	}
}
#pragma warning(pop)

void OracleDefendLine::OnUnitDestroyedListener(const Unit* unit)
{
	//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " destroyed\n";
	for (const auto& oracle : state_machine->oracles)
	{
		if (oracle->engaged_target_tag == unit->tag)
		{
			//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " destroyed by orale\n";
			state_machine->has_attacked[oracle] = true;
		}
	}
}

#pragma warning(push)
#pragma warning(disable : 4100)
void OracleDefendLine::OnUnitCreatedListener(const Unit* unit)
{
	//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " destroyed\n";
	/*if (unit->unit_type == ORACLE)
	{
		state_machine->oracles.push_back(unit);
	}*/
}
#pragma warning(pop)


#pragma endregion


#pragma region OracleHarassGroupUp

std::string OracleHarassGroupUp::toString() const
{
	return "Oracle Harass Group Up";
}

void OracleHarassGroupUp::EnterState()
{
	state_machine->sent_harass = true;
	for (const auto &oracle : state_machine->oracles)
	{
		mediator->SetUnitCommand(oracle, A_MOVE, consolidation_pos, CommandPriority::low);
	}
}

State* OracleHarassGroupUp::TestTransitions()
{
	for (auto itr = state_machine->oracles.begin(); itr != state_machine->oracles.end();)
	{
		if ((*itr)->is_alive == false)
		{
			itr = state_machine->oracles.erase(itr);
		}
		else
		{
			itr++;
		}
	}
	if (state_machine->oracles.size() < 2)
	{
		// delete sm and ag
		mediator->MarkStateMachineForDeletion(state_machine);
		mediator->MarkArmyGroupForDeletion(state_machine->attached_army_group);
		return nullptr;
	}

	if (mediator->GetWorstOngoingAttackValue() < -50)
	{
		// delete sm and ag
		mediator->MarkStateMachineForDeletion(state_machine);
		mediator->MarkArmyGroupForDeletion(state_machine->attached_army_group);
		return nullptr;
	}

	for (const auto& oracle : state_machine->oracles)
	{
		if (oracle->orders.size() == 0)
			mediator->SetUnitCommand(oracle, A_MOVE, consolidation_pos, CommandPriority::low);
	}
	for (const auto &oracle : state_machine->oracles)
	{
		if (Distance2D(oracle->pos, consolidation_pos) > 1)
		{
			return nullptr;
		}
	}
	if (state_machine->harass_direction)
		return new OracleHarassMoveToEntrance(mediator, state_machine, mediator->GetLocations().oracle_path.entrance_points[state_machine->harass_index]);
	else
		return new OracleHarassMoveToEntrance(mediator, state_machine, mediator->GetLocations().oracle_path.exit_points[state_machine->harass_index]);

}

void OracleHarassGroupUp::TickState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		mediator->SetUnitCommand(oracle, A_MOVE, consolidation_pos, CommandPriority::low);
	}
}

void OracleHarassGroupUp::ExitState()
{
	return;
}

#pragma endregion

#pragma region OracleHarassMoveToEntrance

std::string OracleHarassMoveToEntrance::toString() const
{
	return "Oracle Harass Move to Entrance";
}

void OracleHarassMoveToEntrance::EnterState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		mediator->SetUnitCommand(oracle, A_MOVE, entrance_pos, CommandPriority::low);
	}
}

State* OracleHarassMoveToEntrance::TestTransitions()
{
	for (const auto &oracle : state_machine->oracles)
	{
		if (Distance2D(oracle->pos, entrance_pos) > 2)
		{
			return nullptr;
		}
	}
	if (state_machine->harass_direction)
		return new OracleHarassAttackMineralLine(mediator, state_machine, mediator->GetLocations().oracle_path.exit_points[state_machine->harass_index]);
	else
		return new OracleHarassAttackMineralLine(mediator, state_machine, mediator->GetLocations().oracle_path.entrance_points[state_machine->harass_index]);
}

void OracleHarassMoveToEntrance::TickState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		mediator->SetUnitCommand(oracle, A_MOVE, entrance_pos, CommandPriority::low);
	}
}

void OracleHarassMoveToEntrance::ExitState()
{
	return;
}

#pragma endregion

#pragma region OracleHarassAttackMineralLine

OracleHarassAttackMineralLine::OracleHarassAttackMineralLine(Mediator* mediator, OracleHarassStateMachine* state_machine, Point2D exit_pos)
{
	this->mediator = mediator;
	this->state_machine = state_machine;
	this->exit_pos = exit_pos;
	event_id = mediator->GetUniqueId();
}

std::string OracleHarassAttackMineralLine::toString() const
{
	return "Oracle Harass Attack Mineral Line";
}

void OracleHarassAttackMineralLine::EnterState()
{
	mediator->SetUnitsCommand(state_machine->oracles, A_ORACLE_BEAM_ON, CommandPriority::low);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);
}

State* OracleHarassAttackMineralLine::TestTransitions()
{
	if (!lost_oracle)
	{
		for (const auto &oracle : state_machine->oracles)
		{
			if (Distance2D(oracle->pos, exit_pos) > 2)
			{
				return nullptr;
			}
		}
	}
	if (state_machine->harass_direction)
		return new OracleHarassReturnToBase(mediator, state_machine, mediator->GetLocations().oracle_path.exfi_paths[state_machine->harass_index]);
	else
		return new OracleHarassReturnToBase(mediator, state_machine, mediator->GetLocations().oracle_path.exfi_paths[state_machine->harass_index - 1]);
}
	
void OracleHarassAttackMineralLine::TickState()
{
	bool weapons_ready = true;
	for (const auto &oracle : state_machine->oracles)
	{
		float now = mediator->GetCurrentTime();
		bool weapon_ready = now - state_machine->time_last_attacked[oracle] > .8; //.61
		if (!weapon_ready)
		{
			weapons_ready = false;
			break;
		}
	}


	Point2D oracle_center = Utility::MedianCenter(state_machine->oracles);
	float s = (oracle_center.y - exit_pos.y) / (oracle_center.x - exit_pos.x);
	float d = (float)(sqrt(pow(oracle_center.x - exit_pos.x, 2) + pow(oracle_center.y - exit_pos.y, 2)));
	Point2D l1 = Point2D(oracle_center.x + 2 * (exit_pos.x - oracle_center.x) / d + 3, oracle_center.y + 2 * (exit_pos.y - oracle_center.y) / d - (3 / s));
	Point2D l2 = Point2D(oracle_center.x + 2 * (exit_pos.x - oracle_center.x) / d - 3, oracle_center.y + 2 * (exit_pos.y - oracle_center.y) / d + (3 / s));
	Point3D L1 = mediator->ToPoint3D(l1) + Point3D(0, 0, .2f);
	Point3D L2 = mediator->ToPoint3D(l2) + Point3D(0, 0, .2f);

	//agent->Debug()->DebugLineOut(L1, L2, Color(255, 0, 255));


	if (weapons_ready)
	{
		Units drones = mediator->GetUnits(IsUnit(DRONE));
		if (drones.size() == 0)
		{
			if (Distance2D(oracle_center, exit_pos) > 4)
				mediator->SetUnitsCommand(state_machine->oracles, A_MOVE, Utility::PointBetween(oracle_center, exit_pos, 4), CommandPriority::low);
			else
				mediator->SetUnitsCommand(state_machine->oracles, A_MOVE, exit_pos, CommandPriority::low);
			//SetUnitCommand(state_machine->oracles, A_MOVE, exit_pos);
			//agent->Debug()->DebugSphereOut(state_machine->oracles[0]->pos, 1, Color(0, 255, 255));
			return;
		}

		//float perp_direction_to_exit = -1 / sqrt(pow(oracle_center.x + exit_pos.x, 2) + pow(oracle_center.y + exit_pos.y, 2));

		float best_angle = 145;
		Point2D ideal_pos = Utility::PointBetween(oracle_center, exit_pos, 2);
		Point2D exit_vector = Point2D(exit_pos.x - ideal_pos.x, exit_pos.y - ideal_pos.y);
		//agent->Debug()->DebugSphereOut(agent->ToPoint3D(ideal_pos), 1, Color(255, 255, 0));

		/*for (const auto &extractor : mediator->GetUnits(IsUnit(EXTRACTOR)))
		{
			agent->Debug()->DebugSphereOut(extractor->pos, 3, Color(255, 0, 255));
		}*/

		for (const auto &drone : drones)
		{
			if (Utility::DistanceToClosest(mediator->GetUnits(IsUnit(EXTRACTOR)), drone->pos) < 3)
				continue;
			Point2D drone_vector = Point2D(drone->pos.x - ideal_pos.x, drone->pos.y - ideal_pos.y);
			float dot_product = exit_vector.x * drone_vector.x + exit_vector.y * drone_vector.y;
			float exit_mag = (float)(sqrt(pow(exit_vector.x, 2) + pow(exit_vector.y, 2)));
			float drone_mag = (float)(sqrt(pow(drone_vector.x, 2) + pow(drone_vector.y, 2)));

			float angle = acos(dot_product / (exit_mag * drone_mag));

			//agent->Debug()->DebugTextOut(std::to_string(round(angle * 57)), drone->pos, Color(255, 255, 255), 20);

			if (angle < best_angle)
			{
				bool out_of_range = false;
				for (const auto &oracle : state_machine->oracles)
				{
					if (Distance2D(oracle->pos, drone->pos) > 4)
					{
						out_of_range = true;
						break;
					}
				}
				if (out_of_range)
					continue;
				target_drone = drone;
				best_angle = angle;
			}
		}
		if (target_drone == nullptr)
		{
			if (Distance2D(oracle_center, exit_pos) > 4)
				mediator->SetUnitsCommand(state_machine->oracles, A_MOVE, Utility::PointBetween(oracle_center, exit_pos, 4), CommandPriority::low);
			else
				mediator->SetUnitsCommand(state_machine->oracles, A_MOVE, exit_pos, CommandPriority::low);
			//SetUnitCommand(state_machine->oracles, A_MOVE, exit_pos);
			//agent->Debug()->DebugSphereOut(state_machine->oracles[0]->pos, 1, Color(0, 255, 255));
			return;
		}


		/*agent->Debug()->DebugTextOut(std::to_string(target_drone->tag), Point2D(.5, .5), Color(255, 0, 255), 20);
		agent->Debug()->DebugSphereOut(target_drone->pos, 1, Color(0, 255, 255));*/


		mediator->SetUnitsCommand(state_machine->oracles, A_ATTACK, target_drone, CommandPriority::low);


		for (const auto &oracle : state_machine->oracles)
		{
			state_machine->time_last_attacked[oracle] = mediator->GetCurrentTime();
			state_machine->has_attacked[oracle] = false;
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
		}
	}
	else if (state_machine->has_attacked[state_machine->oracles[0]])
	{
		if (Distance2D(oracle_center, exit_pos) > 4)
			mediator->SetUnitsCommand(state_machine->oracles, A_MOVE, Utility::PointBetween(oracle_center, exit_pos, 4), CommandPriority::low);
		else
			mediator->SetUnitsCommand(state_machine->oracles, A_MOVE, exit_pos, CommandPriority::low);
		//SetUnitCommand(state_machine->oracles, A_MOVE, exit_pos);

		/*for (const auto &oracle : state_machine->oracles)
		{
			agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 0, 255));
		}*/
	}
	else
	{
		/*for (const auto &oracle : state_machine->oracles)
		{
			agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
		}*/
	}
}

void OracleHarassAttackMineralLine::ExitState()
{
	mediator->SetUnitsCommand(state_machine->oracles, A_ORACLE_BEAM_OFF, CommandPriority::low);
	mediator->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

void OracleHarassAttackMineralLine::OnUnitDestroyedListener(const Unit* unit)
{
	if (unit == nullptr)
	{
		std::cerr << "nullptr passed into OracleHarassAttackMineralLine::OnUnitDestroyedListener" << std::endl;
		mediator->LogMinorError();
		return;
	}
	//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " destroyed\n";
	if (target_drone != nullptr && unit->tag == target_drone->tag)
	{
		target_drone = nullptr;
		for (const auto &oracle : state_machine->oracles)
		{
			state_machine->has_attacked[oracle] = true;
			/*
			if (state_machine->oracles[i]->engaged_target_tag == unit->tag || state_machine->oracles[i]->engaged_target_tag == 0)
			{
				//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " destroyed by orale\n";
				state_machine->has_attacked[i] = true;
			}*/
		}
		//agent->Debug()->DebugTextOut(std::to_string(unit->tag), Point2D(.5, .45), Color(0, 255, 255), 20);

	}
}

#pragma endregion

#pragma region OracleHarassReturnToBase

std::string OracleHarassReturnToBase::toString() const
{
	return "Oracle Harass Return to Base";
}

void OracleHarassReturnToBase::EnterState()
{
	mediator->SetUnitsCommand(state_machine->oracles, A_ORACLE_BEAM_OFF, CommandPriority::low);
	for (int i = 0; i < exfil_path.size(); i++)
	{
		mediator->SetUnitsCommand(state_machine->oracles, A_MOVE, exfil_path[i], CommandPriority::low, i > 0);
	}
	mediator->SetUnitsCommand(state_machine->oracles, A_MOVE, mediator->GetStartLocation(), CommandPriority::low, true);
}

State* OracleHarassReturnToBase::TestTransitions()
{
	for (const auto &oracle : state_machine->oracles)
	{
		if (Distance2D(oracle->pos, mediator->GetStartLocation()) > 40)
			return nullptr;
	}
	// delete sm and ag
	mediator->MarkStateMachineForDeletion(state_machine);
	mediator->MarkArmyGroupForDeletion(state_machine->attached_army_group);
	return nullptr;
}

void OracleHarassReturnToBase::TickState()
{
	return;
}

void OracleHarassReturnToBase::ExitState()
{
	/*for (const auto& oracle : mediator->GetUnits(IsFriendlyUnit(ORACLE)))
	{
		if (std::find(state_machine->oracles.begin(), state_machine->oracles.end(), oracle) == state_machine->oracles.end())
		{
			state_machine->AddOracle(oracle);
		}
	}*/
	return;
}

#pragma endregion



#pragma region OracleHarassStateMachine

OracleHarassStateMachine::OracleHarassStateMachine(Mediator* mediator, Units oracles, Point2D third_base_pos,
	Point2D door_guard_pos, std::string name) : StateMachine(mediator, name)
{
	this->oracles = oracles;
	this->third_base_pos = third_base_pos;
	this->door_guard_pos = door_guard_pos;
	event_id = mediator->GetUniqueId();
	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	current_state = new OracleDefendLine(mediator, this, third_base_pos, door_guard_pos);
	for (int i = 0; i < oracles.size(); i++)
	{
		time_last_attacked[oracles[i]] = 0;
		has_attacked[oracles[i]] = true;
	}
	current_state->EnterState();
}

OracleHarassStateMachine::OracleHarassStateMachine(Mediator* mediator, Units oracles, std::string name) : StateMachine(mediator, name)
{
	this->oracles = oracles;
	event_id = mediator->GetUniqueId();
	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	if (mediator->GetGameLoop() % 2 == 0)
	{
		harass_direction = true;
		harass_index = 0;
		current_state = new OracleHarassGroupUp(mediator, this, mediator->GetLocations().oracle_path.entrance_point);
	}
	else
	{
		harass_direction = false;
		harass_index = mediator->GetLocations().oracle_path.entrance_points.size() - 1;
		current_state = new OracleHarassGroupUp(mediator, this, mediator->GetLocations().oracle_path.exit_point);
	}
	
	for (size_t i = 0; i < oracles.size(); i++)
	{
		time_last_attacked[oracles[i]] = 0;
		has_attacked[oracles[i]] = true;
	}
	current_state->EnterState();
}

OracleHarassStateMachine::OracleHarassStateMachine(Mediator* mediator, ArmyTemplateStateMachine<OutsideControlArmyGroup, OracleHarassStateMachine>* army_template) : StateMachine(mediator, "Oracle harass")
{

	event_id = mediator->GetUniqueId();
	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	if (mediator->GetGameLoop() % 2 == 0)
	{
		harass_direction = true;
		harass_index = 0;
		current_state = new OracleHarassGroupUp(mediator, this, mediator->GetLocations().oracle_path.entrance_point);
	}
	else
	{
		harass_direction = false;
		harass_index = mediator->GetLocations().oracle_path.entrance_points.size() - 1;
		current_state = new OracleHarassGroupUp(mediator, this, mediator->GetLocations().oracle_path.exit_point);
	}

	for (size_t i = 0; i < oracles.size(); i++)
	{
		time_last_attacked[oracles[i]] = 0;
		has_attacked[oracles[i]] = true;
	}
	current_state->EnterState();
}

OracleHarassStateMachine::~OracleHarassStateMachine()
{
	mediator->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

void OracleHarassStateMachine::AddOracle(const Unit* oracle)
{
	oracles.push_back(oracle);
	time_last_attacked[oracle] = 0;
	has_attacked[oracle] = true;
}

bool OracleHarassStateMachine::AddUnit(const Unit* unit)
{
	if (unit->unit_type != ORACLE)
		return false;
	if (dynamic_cast<OracleDefendLocation*>(current_state) || dynamic_cast<OracleDefendLine*>(current_state) ||
		dynamic_cast<OracleHarassGroupUp*>(current_state))
	{
		AddOracle(unit);
		return true;
	}
	return false;
}

void OracleHarassStateMachine::OnUnitDestroyedListener(const Unit* oracle)
{
	auto found = std::find(oracles.begin(), oracles.end(), oracle);
	if (found != oracles.end())
	{
		auto index = found - oracles.begin();
		oracles.erase(oracles.begin() + index);
		OracleHarassAttackMineralLine* state = dynamic_cast<OracleHarassAttackMineralLine*>(current_state);
		if (state != nullptr)
		{
			state->lost_oracle = true;
		}
	}
}

#pragma endregion

}