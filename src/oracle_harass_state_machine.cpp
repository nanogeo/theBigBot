#pragma once

#include "oracle_harass_state_machine.h"
#include "theBigBot.h"

namespace sc2 {

#pragma region OracleDefendLocation


OracleDefendLocation::OracleDefendLocation(TheBigBot* agent, OracleHarassStateMachine* state_machine, Point2D denfensive_position)
{
	this->agent = agent;
	this->state_machine = state_machine;
	this->denfensive_position = denfensive_position;
	event_id = agent->GetUniqueId();
}

std::string OracleDefendLocation::toString()
{
	return "Oracle Defend Location";
}

void OracleDefendLocation::TickState()
{
	Units enemy_units = agent->Observation()->GetUnits(Unit::Alliance::Enemy, IsNotFlyingUnit());
	if (enemy_units.size() > 0 && Utility::DistanceToClosest(enemy_units, denfensive_position) < 10)
	{
		for (const auto &oracle : state_machine->oracles)
		{
			float now = agent->Observation()->GetGameLoop() / 22.4;
			bool weapon_ready = now - state_machine->time_last_attacked[oracle] > .61;
			bool beam_active = state_machine->is_beam_active[oracle];
			bool beam_activatable = false;

			if (!beam_active && oracle->energy >= 40)
				beam_activatable = true;

			const Unit* closest_unit = Utility::ClosestTo(enemy_units, oracle->pos);
			if (beam_active)
			{
				if (Distance2D(oracle->pos, closest_unit->pos) > 4)
				{
					float dist = Distance2D(oracle->pos, closest_unit->pos);
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(oracle->pos, closest_unit->pos, dist + 1), false);
				}
				else if (weapon_ready)
				{
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::ATTACK_ATTACK, closest_unit, false);
					state_machine->time_last_attacked[oracle] = now;
					state_machine->has_attacked[oracle] = false;
					//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 255, 0));
				}
				else if (state_machine->has_attacked[oracle])
				{
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, false);
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
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMON, false);
					state_machine->is_beam_active[oracle] = true;
				}
				else
				{
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, false);
				}
			}
			else
			{
				agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, false);
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
			if (state_machine->is_beam_active[oracle])
			{
				agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
				state_machine->is_beam_active[oracle] = false;
			}

			if (Distance2D(oracle->pos, denfensive_position) > 1)
				agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, denfensive_position);
		}
	}


}

void OracleDefendLocation::EnterState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, denfensive_position);
	}
	std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
		this->OnUnitDamagedListener(unit, health, shields);
	};
	agent->AddListenerToOnUnitDamagedEvent(event_id, onUnitDamaged);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);
}

void OracleDefendLocation::ExitState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		// remove event onUnitDamaged
		agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
	}
	agent->RemoveListenerToOnUnitDamagedEvent(event_id);
	agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
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
			return new OracleHarassGroupUp(agent, state_machine, agent->locations->oracle_path.entrance_point);
		}
		else
		{
			state_machine->harass_direction = false;
			state_machine->harass_index = agent->locations->oracle_path.entrance_points.size() - 1;
			return new OracleHarassGroupUp(agent, state_machine, agent->locations->oracle_path.exit_point);
		}
	}
	return NULL;
}

void OracleDefendLocation::OnUnitDamagedListener(const Unit* unit, float health, float shields)
{
	//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage\n";
	for (const auto &oracle : state_machine->oracles)
	{
		if (oracle->engaged_target_tag == unit->tag)
		{
			//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage from orale\n";
			state_machine->has_attacked[oracle] = true;
		}
	}
}

void OracleDefendLocation::OnUnitDestroyedListener(const Unit* unit)
{
	//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed\n";
	for (const auto &oracle : state_machine->oracles)
	{
		if (oracle->engaged_target_tag == unit->tag)
		{
			//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed by orale\n";
			state_machine->has_attacked[oracle] = true;
		}
	}
}


#pragma endregion

#pragma region OracleDefendLine

OracleDefendLine::OracleDefendLine(TheBigBot* agent, OracleHarassStateMachine* state_machine, Point2D start, Point2D end)
{
	this->agent = agent;
	this->state_machine = state_machine;

	double slope = (start.y - end.y) / (start.x - end.x);

	double line_a = slope;
	double line_b = start.y - (slope * start.x);

	line = new LineSegmentLinearX(line_a, line_b, start.x, end.x, false, Point2D(0, 0), false);

	event_id = agent->GetUniqueId();
}

std::string OracleDefendLine::toString()
{
	return "Oracle Defend Line";
}

void OracleDefendLine::TickState()
{
	Units enemy_units = agent->Observation()->GetUnits(Unit::Alliance::Enemy, IsNotFlyingUnit());
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
			float now = agent->Observation()->GetGameLoop() / 22.4;
			bool weapon_ready = now - state_machine->time_last_attacked[oracle] > .61;
			bool beam_active = state_machine->is_beam_active[oracle];
			bool beam_activatable = false;

			if (!beam_active && oracle->energy >= 40 && enemy_units.size() > 5)
				beam_activatable = true;

			const Unit* closest_unit = Utility::ClosestTo(enemy_units, oracle->pos);
			if (beam_active)
			{
				if (Distance2D(oracle->pos, closest_unit->pos) > 4)
				{
					float dist = Distance2D(oracle->pos, closest_unit->pos);
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(oracle->pos, closest_unit->pos, dist + 1), false);
				}
				else if (weapon_ready)
				{
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::ATTACK_ATTACK, closest_unit, false);
					state_machine->time_last_attacked[oracle] = now;
					state_machine->has_attacked[oracle] = false;
					//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 255, 0));
				}
				else if (state_machine->has_attacked[oracle])
				{
					if ((agent->Observation()->GetUnit(oracle->engaged_target_tag) == NULL || 
						Distance2D(oracle->pos, agent->Observation()->GetUnit(oracle->engaged_target_tag)->pos) > 3) ||
						Distance2D(oracle->pos, closest_unit->pos) > 3)  // only move if target is getting away
						agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, false);
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
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMON, false);
					state_machine->is_beam_active[oracle] = true;
				}
				else
				{
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, false);
				}
			}
			else
			{
				agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, false);
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
			if (state_machine->is_beam_active[oracle])
			{
				agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
				state_machine->is_beam_active[oracle] = false;
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
				agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, line->GetEndPoint());
			}
			else if (dist_to_end < 1)
			{
				agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, line->GetStartPoint());
			}
			else
			{
				if (dist_to_end < dist_to_start)
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, line->GetEndPoint());
				else
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, line->GetStartPoint());
			}
		}
	}


}

void OracleDefendLine::EnterState()
{
	for (const auto& oracle : state_machine->oracles)
	{
		agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, line->GetStartPoint());
	}
	std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
		this->OnUnitDamagedListener(unit, health, shields);
	};
	agent->AddListenerToOnUnitDamagedEvent(event_id, onUnitDamaged);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

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
		agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
	}
	agent->RemoveListenerToOnUnitDamagedEvent(event_id);
	agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
	//agent->RemoveListenerToOnUnitCreatedEvent(event_id);
	return;
}

State* OracleDefendLine::TestTransitions()
{
	if (state_machine->oracles.size() > 1 && state_machine->sent_harass == false) 
	{
		if (agent->Observation()->GetGameLoop() % 2 == 0)
		{
			state_machine->harass_direction = true;
			state_machine->harass_index = 0;
			return new OracleHarassGroupUp(agent, state_machine, agent->locations->oracle_path.entrance_point);
		}
		else
		{
			state_machine->harass_direction = false;
			state_machine->harass_index = agent->locations->oracle_path.entrance_points.size() - 1;
			return new OracleHarassGroupUp(agent, state_machine, agent->locations->oracle_path.exit_point);
		}
	}
	return NULL;
}

void OracleDefendLine::OnUnitDamagedListener(const Unit* unit, float health, float shields)
{
	//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage\n";
	for (const auto& oracle : state_machine->oracles)
	{
		if (oracle->engaged_target_tag == unit->tag)
		{
			//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage from orale\n";
			state_machine->has_attacked[oracle] = true;
		}
	}
}

void OracleDefendLine::OnUnitDestroyedListener(const Unit* unit)
{
	//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed\n";
	for (const auto& oracle : state_machine->oracles)
	{
		if (oracle->engaged_target_tag == unit->tag)
		{
			//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed by orale\n";
			state_machine->has_attacked[oracle] = true;
		}
	}
}

void OracleDefendLine::OnUnitCreatedListener(const Unit* unit)
{
	//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed\n";
	/*if (unit->unit_type == ORACLE)
	{
		state_machine->oracles.push_back(unit);
	}*/
}


#pragma endregion


#pragma region OracleDefendArmyGroup

OracleDefendArmyGroup::OracleDefendArmyGroup(TheBigBot* agent, OracleHarassStateMachine* state_machine)
{
	this->agent = agent;
	this->state_machine = state_machine;
	event_id = agent->GetUniqueId();
}

std::string OracleDefendArmyGroup::toString()
{
	return "Oracle Defend ArmyGroup";
}

void OracleDefendArmyGroup::TickState()
{
	Point2D center = state_machine->attached_army_group->attack_path[state_machine->attached_army_group->current_attack_index - 2];
	if (state_machine->attached_army_group->all_units.size() > 0)
	{
		Point2D median_center = Utility::MedianCenter(state_machine->attached_army_group->all_units);
		center = state_machine->attached_army_group->attack_path_line.GetPointFrom(median_center, 2, false);
	}

	Units enemy_units = agent->Observation()->GetUnits(IsUnits(Utility::GetBurrowedUnitTypes()));

	bool revelation_cast = false;
	for (const auto &oracle : state_machine->oracles)
	{
		if (state_machine->casting[oracle])
		{
			revelation_cast = true;
			break;
		}
	}
	// revelate when units are burrowing
	if (!revelation_cast)
	{
		const Unit* unit_to_revelate = NULL;
		for (const auto &unit : enemy_units)
		{
			if (Utility::DistanceToClosest(state_machine->oracles, unit->pos) <= 9)
			{
				if (std::find(unit->buffs.begin(), unit->buffs.end(), BUFF_ID::ORACLEREVELATION) == unit->buffs.end())
				{
					unit_to_revelate = unit;
					break;
				}
			}
		}
		if (unit_to_revelate != NULL)
		{
			const Unit* highest_over_75 = NULL;
			const Unit* lowest_over_25 = NULL;
			for (const auto &oracle : state_machine->oracles)
			{
				if (oracle->energy > 75)
				{
					if (highest_over_75 == NULL || highest_over_75->energy < oracle->energy)
						highest_over_75 = oracle;
				}
				else if (oracle->energy > 25)
				{
					if (lowest_over_25 == NULL || lowest_over_25->energy > oracle->energy)
						lowest_over_25 = oracle;
				}
			}
			if (highest_over_75 != NULL)
			{
				agent->Actions()->UnitCommand(highest_over_75, ABILITY_ID::EFFECT_ORACLEREVELATION, unit_to_revelate->pos);
				state_machine->casting[highest_over_75] = true;
				state_machine->casting_energy[highest_over_75] = highest_over_75->energy;
				//agent->Debug()->DebugSphereOut(highest_over_75->pos, 2, Color(255, 0, 0));

			}
			else if (lowest_over_25 != NULL)
			{
				agent->Actions()->UnitCommand(lowest_over_25, ABILITY_ID::EFFECT_ORACLEREVELATION, unit_to_revelate->pos);
				state_machine->casting[lowest_over_25] = true;
				state_machine->casting_energy[lowest_over_25] = lowest_over_25->energy;
				//agent->Debug()->DebugSphereOut(lowest_over_25->pos, 2, Color(255, 0, 0));
			}
		}
	}


	Units enemy_lings = agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::ZERG_ZERGLING));
	int num_close_lings = 0;
	for (const auto &ling : enemy_lings)
	{
		if (Utility::DistanceToClosest(state_machine->attached_army_group->all_units, ling->pos) < 4)
			num_close_lings++;
	}
	if (num_close_lings > 4)
	{
		int num_stalkers_with_blink = 0;
		float now = agent->Observation()->GetGameLoop() / 22.4;
		for (const auto &last_blink_time : state_machine->attached_army_group->last_time_blinked)
		{
			if (now - last_blink_time.second > 7)
				num_stalkers_with_blink++;
		}
		float percent_stalkers_with_blink = 1;
		if (state_machine->attached_army_group->last_time_blinked.size() > 0)
			percent_stalkers_with_blink = static_cast<float>(num_stalkers_with_blink) / static_cast<float>(state_machine->attached_army_group->last_time_blinked.size());

		int num_oracles_needed = 0;

		if (percent_stalkers_with_blink < .25)
			num_oracles_needed = 3;
		else if (percent_stalkers_with_blink < .5)
			num_oracles_needed = 2;
		else if (percent_stalkers_with_blink < .75)
			num_oracles_needed = 1;

		if (num_close_lings > 30)
			num_oracles_needed += 3;
		else if (num_close_lings > 20)
			num_oracles_needed += 2;
		else if (num_close_lings > 10)
			num_oracles_needed += 1;
			

		num_oracles_needed = std::min(num_oracles_needed, 3);

		int num_oracles_active = 0;
		for (const auto &beam_active : state_machine->is_beam_active)
		{
			if (beam_active.second)
				num_oracles_active++;
		}
		/*
		agent->Debug()->DebugTextOut(std::to_string(num_close_lings), Point2D(.2, .18), Color(0, 255, 0), 20);
		agent->Debug()->DebugTextOut(std::to_string(num_oracles_active), Point2D(.2, .2), Color(0, 255, 255), 20);
		agent->Debug()->DebugTextOut(std::to_string(num_oracles_needed), Point2D(.2, .22), Color(0, 255, 255), 20);
		agent->Debug()->DebugTextOut(std::to_string(percent_stalkers_with_blink), Point2D(.2, .24), Color(0, 255, 255), 20);
		agent->Debug()->DebugTextOut(std::to_string(num_stalkers_with_blink), Point2D(.2, .26), Color(0, 255, 255), 20);
		agent->Debug()->DebugTextOut(std::to_string(state_machine->attached_army_group->blink_ready.size()), Point2D(.2, .28), Color(0, 255, 255), 20);
			
		for (int i = 0; i < state_machine->oracles.size(); i++)
		{
			if (state_machine->is_beam_active[i])
				agent->Debug()->DebugTextOut(std::to_string(state_machine->oracles[i]->energy), Point2D(.2, .3 + .02 * i), Color(0, 255, 0), 20);
			else
				agent->Debug()->DebugTextOut(std::to_string(state_machine->oracles[i]->energy), Point2D(.2, .3 + .02 * i), Color(255, 0, 255), 20);

		}*/

		if (num_oracles_active > num_oracles_needed) // deactivate oracles
		{
			Units oracles = Units(state_machine->oracles);
			std::sort(oracles.begin(), oracles.end(), [](const Unit* &a, const Unit* &b) -> bool
			{
				return a->energy > b->energy;
			});
			for (const auto &oracle : oracles)
			{
				if (num_oracles_active == num_oracles_needed)
					break;
				if (oracle->energy > 10 && Utility::DistanceToClosest(enemy_lings, oracle->pos) > 5)
				{
					if (state_machine->is_beam_active.count(oracle) && state_machine->is_beam_active[oracle] == true)
					{
						state_machine->is_beam_active[oracle] = false;
						agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
						num_oracles_active--;
					}
				}
			}
		}
		else if (num_oracles_active < num_oracles_needed) // activate more oracles
		{
			Units oracles = Units(state_machine->oracles);
			std::sort(oracles.begin(), oracles.end(), [](const Unit* &a, const Unit* &b) -> bool
			{
				return a->energy < b->energy;
			});
			for (const auto &oracle : oracles)
			{
				if (num_oracles_active == num_oracles_needed)
					break;
				if (oracle->energy > 40)
				{
					if (state_machine->is_beam_active.count(oracle) && state_machine->is_beam_active[oracle] == false)
					{
						state_machine->is_beam_active[oracle] = true;
						agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMON);
						num_oracles_active++;
					}
				}
			}
		}
	}
	else
	{
		Units oracles = Units(state_machine->oracles);
		std::sort(oracles.begin(), oracles.end(), [](const Unit* &a, const Unit* &b) -> bool
		{
			return a->energy > b->energy;
		});
		for (const auto &oracle : oracles)
		{
			if (oracle->energy > 10 && (enemy_lings.size() == 0 || Utility::DistanceToClosest(enemy_lings, oracle->pos)))
			{
				if (state_machine->is_beam_active.count(oracle) && state_machine->is_beam_active[oracle] == true)
				{
					state_machine->is_beam_active[oracle] = false;
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
				}
			}
		}
	}
	// add oracle to volley or agnore units targetted in volley?
	// add event listeners for oracle
	for (const auto &oracle : state_machine->oracles)
	{
		if (state_machine->casting[oracle])
		{
			if (state_machine->casting_energy[oracle] > oracle->energy || state_machine->casting_energy[oracle] + 5 < oracle->energy)
			{
				state_machine->casting[oracle] = false;
			}
			else
			{
				continue;
			}
		}
		if (state_machine->is_beam_active[oracle] == false)
		{
			agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, center);
			continue;
		}
		float now = agent->Observation()->GetGameLoop() / 22.4;
		bool weapon_ready = now - state_machine->time_last_attacked[oracle] > .8; //.61

		/*agent->Debug()->DebugTextOut("weapon ready " + std::to_string(weapon_ready), Point2D(.2, .35), Color(0, 255, 0), 20);
		agent->Debug()->DebugTextOut("has attacked " + std::to_string(state_machine->has_attacked[oracle]), Point2D(.2, .37), Color(0, 255, 0), 20);
		agent->Debug()->DebugTextOut("target " + std::to_string(state_machine->target[oracle]), Point2D(.2, .39), Color(0, 255, 0), 20);*/


		if (weapon_ready)
		{
			const Unit* closest_unit = Utility::ClosestTo(enemy_lings, oracle->pos);
			if (closest_unit == NULL || Distance2D(closest_unit->pos, oracle->pos) > 6)
			{
				agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, center);
				continue;
			}


			agent->Actions()->UnitCommand(oracle, ABILITY_ID::ATTACK_ATTACK, closest_unit);
			//agent->Debug()->DebugSphereOut(closest_unit->pos, .75, Color(0, 255, 255));

			state_machine->target[oracle] = closest_unit->tag;
			state_machine->time_last_attacked[oracle] = agent->Observation()->GetGameLoop() / 22.4;
			state_machine->has_attacked[oracle] = false;
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
		}
		else if (state_machine->has_attacked[oracle])
		{
			agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, center);

			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 0, 255));
		}
		else
		{
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
		}
	}
	// update beam status for tired oracles
	for (const auto &oracle : state_machine->oracles)
	{
		if (oracle->energy <= 2)
			state_machine->is_beam_active[oracle] = false;
	}

}

void OracleDefendArmyGroup::EnterState()
{
	std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
		this->OnUnitDamagedListener(unit, health, shields);
	};
	agent->AddListenerToOnUnitDamagedEvent(event_id, onUnitDamaged);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);
}

void OracleDefendArmyGroup::ExitState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
	}
	agent->RemoveListenerToOnUnitDamagedEvent(event_id);
	agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
	return;
}

State* OracleDefendArmyGroup::TestTransitions()
{
	return NULL;
}

void OracleDefendArmyGroup::OnUnitDamagedListener(const Unit* unit, float health, float shields)
{
	//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage\n";
	int i = 0;
	//agent->Debug()->DebugTextOut(std::to_string(unit->tag), Point2D(.1, .35), Color(0, 255, 0), 20);

	for (const auto &oracle : state_machine->oracles)
	{
		if (state_machine->target[oracle] == unit->tag)
		{
			//agent->Debug()->DebugTextOut(Utility::UnitTypeIdToString(unit->unit_type.ToType()) + " took " + std::to_string(health) + " damage from orale", Point2D(.2, .4 + .02 * i), Color(0, 255, 0), 20);
			//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage from orale\n";
			state_machine->has_attacked[oracle] = true;
		}
		i++;
	}
}

void OracleDefendArmyGroup::OnUnitDestroyedListener(const Unit* unit)
{
	//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed\n";
	int i = 0;
	//agent->Debug()->DebugTextOut(std::to_string(unit->tag), Point2D(.1, .39), Color(0, 255, 255), 20);
	for (const auto &oracle : state_machine->oracles)
	{
		if (state_machine->target[oracle] == unit->tag)
		{
			//agent->Debug()->DebugTextOut(Utility::UnitTypeIdToString(unit->unit_type.ToType()) + " desroyed by oracle", Point2D(.2, .45 + .02 * i), Color(0, 255, 0), 20);
			//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed by orale\n";
			state_machine->has_attacked[oracle] = true;
		}
		i++;
	}
}

#pragma endregion



#pragma region OracleHarassGroupUp

std::string OracleHarassGroupUp::toString()
{
	return "Oracle Harass Group Up";
}

void OracleHarassGroupUp::EnterState()
{
	state_machine->sent_harass = true;
	for (const auto &oracle : state_machine->oracles)
	{
		agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, consolidation_pos);
	}
}

State* OracleHarassGroupUp::TestTransitions()
{
	if (state_machine->oracles.size() == 0)
		return NULL;
	for (const auto& oracle : state_machine->oracles)
	{
		if (oracle->orders.size() == 0)
			agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, consolidation_pos);
	}
	for (const auto &oracle : state_machine->oracles)
	{
		if (Distance2D(oracle->pos, consolidation_pos) > 1)
		{
			return NULL;
		}
	}
	if (state_machine->harass_direction)
		return new OracleHarassMoveToEntrance(agent, state_machine, agent->locations->oracle_path.entrance_points[state_machine->harass_index]);
	else
		return new OracleHarassMoveToEntrance(agent, state_machine, agent->locations->oracle_path.exit_points[state_machine->harass_index]);

}

void OracleHarassGroupUp::TickState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, consolidation_pos);
	}
}

void OracleHarassGroupUp::ExitState()
{
	return;
}

#pragma endregion

#pragma region OracleHarassMoveToEntrance

std::string OracleHarassMoveToEntrance::toString()
{
	return "Oracle Harass Move to Entrance";
}

void OracleHarassMoveToEntrance::EnterState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, entrance_pos);
	}
}

State* OracleHarassMoveToEntrance::TestTransitions()
{
	for (const auto &oracle : state_machine->oracles)
	{
		if (Distance2D(oracle->pos, entrance_pos) > 2)
		{
			return NULL;
		}
	}
	if (state_machine->harass_direction)
		return new OracleHarassAttackMineralLine(agent, state_machine, agent->locations->oracle_path.exit_points[state_machine->harass_index]);
	else
		return new OracleHarassAttackMineralLine(agent, state_machine, agent->locations->oracle_path.entrance_points[state_machine->harass_index]);
}

void OracleHarassMoveToEntrance::TickState()
{
	for (const auto &oracle : state_machine->oracles)
	{
		agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, entrance_pos);
	}
}

void OracleHarassMoveToEntrance::ExitState()
{
	return;
}

#pragma endregion

#pragma region OracleHarassAttackMineralLine

OracleHarassAttackMineralLine::OracleHarassAttackMineralLine(TheBigBot* agent, OracleHarassStateMachine* state_machine, Point2D exit_pos)
{
	this->agent = agent;
	this->state_machine = state_machine;
	this->exit_pos = exit_pos;
	event_id = agent->GetUniqueId();
}

std::string OracleHarassAttackMineralLine::toString()
{
	return "Oracle Harass Attack Mineral Line";
}

void OracleHarassAttackMineralLine::EnterState()
{
	agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::BEHAVIOR_PULSARBEAMON);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);
}

State* OracleHarassAttackMineralLine::TestTransitions()
{
	if (!lost_oracle)
	{
		for (const auto &oracle : state_machine->oracles)
		{
			if (Distance2D(oracle->pos, exit_pos) > 2)
			{
				return NULL;
			}
		}
	}
	if (state_machine->harass_direction)
		return new OracleHarassReturnToBase(agent, state_machine, agent->locations->oracle_path.exfi_paths[state_machine->harass_index]);
	else
		return new OracleHarassReturnToBase(agent, state_machine, agent->locations->oracle_path.exfi_paths[state_machine->harass_index - 1]);
}
	
void OracleHarassAttackMineralLine::TickState()
{
	bool weapons_ready = true;
	for (const auto &oracle : state_machine->oracles)
	{
		float now = agent->Observation()->GetGameLoop() / 22.4;
		bool weapon_ready = now - state_machine->time_last_attacked[oracle] > .8; //.61
		if (!weapon_ready)
		{
			weapons_ready = false;
			break;
		}
	}


	Point2D oracle_center = Utility::MedianCenter(state_machine->oracles);
	float s = (oracle_center.y - exit_pos.y) / (oracle_center.x - exit_pos.x);
	float d = sqrt(pow(oracle_center.x - exit_pos.x, 2) + pow(oracle_center.y - exit_pos.y, 2));
	Point2D l1 = Point2D(oracle_center.x + 2 * (exit_pos.x - oracle_center.x) / d + 3, oracle_center.y + 2 * (exit_pos.y - oracle_center.y) / d - (3 / s));
	Point2D l2 = Point2D(oracle_center.x + 2 * (exit_pos.x - oracle_center.x) / d - 3, oracle_center.y + 2 * (exit_pos.y - oracle_center.y) / d + (3 / s));
	Point3D L1 = agent->ToPoint3D(l1) + Point3D(0, 0, .2);
	Point3D L2 = agent->ToPoint3D(l2) + Point3D(0, 0, .2);

	//agent->Debug()->DebugLineOut(L1, L2, Color(255, 0, 255));


	if (weapons_ready)
	{
		Units drones = agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::ZERG_DRONE));
		if (drones.size() == 0)
		{
			if (Distance2D(oracle_center, exit_pos) > 4)
				agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(oracle_center, exit_pos, 4));
			else
				agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, exit_pos);
			//agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, exit_pos);
			//agent->Debug()->DebugSphereOut(state_machine->oracles[0]->pos, 1, Color(0, 255, 255));
			return;
		}

		Point2D oracle_center = Utility::MedianCenter(state_machine->oracles);
		float perp_direction_to_exit = -1 / sqrt(pow(oracle_center.x + exit_pos.x, 2) + pow(oracle_center.y + exit_pos.y, 2));

		float best_angle = 145;
		Point2D ideal_pos = Utility::PointBetween(oracle_center, exit_pos, 2);
		Point2D exit_vector = Point2D(exit_pos.x - ideal_pos.x, exit_pos.y - ideal_pos.y);
		//agent->Debug()->DebugSphereOut(agent->ToPoint3D(ideal_pos), 1, Color(255, 255, 0));

		/*for (const auto &extractor : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::ZERG_EXTRACTOR)))
		{
			agent->Debug()->DebugSphereOut(extractor->pos, 3, Color(255, 0, 255));
		}*/

		for (const auto &drone : drones)
		{
			if (Utility::DistanceToClosest(agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::ZERG_EXTRACTOR)), drone->pos) < 3)
				continue;
			Point2D drone_vector = Point2D(drone->pos.x - ideal_pos.x, drone->pos.y - ideal_pos.y);
			float dot_product = exit_vector.x * drone_vector.x + exit_vector.y * drone_vector.y;
			float exit_mag = sqrt(pow(exit_vector.x, 2) + pow(exit_vector.y, 2));
			float drone_mag = sqrt(pow(drone_vector.x, 2) + pow(drone_vector.y, 2));

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
		if (target_drone == NULL)
		{
			if (Distance2D(oracle_center, exit_pos) > 4)
				agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(oracle_center, exit_pos, 4));
			else
				agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, exit_pos);
			//agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, exit_pos);
			//agent->Debug()->DebugSphereOut(state_machine->oracles[0]->pos, 1, Color(0, 255, 255));
			return;
		}


		/*agent->Debug()->DebugTextOut(std::to_string(target_drone->tag), Point2D(.5, .5), Color(255, 0, 255), 20);
		agent->Debug()->DebugSphereOut(target_drone->pos, 1, Color(0, 255, 255));*/


		agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::ATTACK_ATTACK, target_drone);


		for (const auto &oracle : state_machine->oracles)
		{
			state_machine->time_last_attacked[oracle] = agent->Observation()->GetGameLoop() / 22.4;
			state_machine->has_attacked[oracle] = false;
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
		}
	}
	else if (state_machine->has_attacked[state_machine->oracles[0]])
	{
		if (Distance2D(oracle_center, exit_pos) > 4)
			agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(oracle_center, exit_pos, 4));
		else
			agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, exit_pos);
		//agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, exit_pos);

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
	agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
	agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

void OracleHarassAttackMineralLine::OnUnitDestroyedListener(const Unit* unit)
{
	//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed\n";
	if (target_drone != NULL && unit->tag == target_drone->tag)
	{
		target_drone = NULL;
		for (const auto &oracle : state_machine->oracles)
		{
			state_machine->has_attacked[oracle] = true;
			/*
			if (state_machine->oracles[i]->engaged_target_tag == unit->tag || state_machine->oracles[i]->engaged_target_tag == 0)
			{
				//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed by orale\n";
				state_machine->has_attacked[i] = true;
			}*/
		}
		//agent->Debug()->DebugTextOut(std::to_string(unit->tag), Point2D(.5, .45), Color(0, 255, 255), 20);

	}
}

#pragma endregion

#pragma region OracleHarassReturnToBase

std::string OracleHarassReturnToBase::toString()
{
	return "Oracle Harass Return to Base";
}

void OracleHarassReturnToBase::EnterState()
{
	agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
	for (int i = 0; i < exfil_path.size(); i++)
	{
		agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, exfil_path[i], i > 0);
	}
	agent->Actions()->UnitCommand(state_machine->oracles, ABILITY_ID::GENERAL_MOVE, agent->locations->start_location, true);
}

State* OracleHarassReturnToBase::TestTransitions()
{
	for (const auto &oracle : state_machine->oracles)
	{
		if (Distance2D(oracle->pos, agent->locations->start_location) > 40)
			return NULL;
	}
	// delete sm and ag
	agent->mediator.MarkStateMachineForDeletion(state_machine);
	agent->mediator.MarkArmyGroupForDeletion(state_machine->attached_army_group);
	return NULL;
}

void OracleHarassReturnToBase::TickState()
{
	return;
}

void OracleHarassReturnToBase::ExitState()
{
	/*for (const auto& oracle : agent->Observation()->GetUnits(IsFriendlyUnit(ORACLE)))
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

OracleHarassStateMachine::OracleHarassStateMachine(TheBigBot* agent, Units oracles, Point2D third_base_pos, Point2D door_guard_pos, std::string name)
{
	this->agent = agent;
	this->oracles = oracles;
	this->name = name;
	this->third_base_pos = third_base_pos;
	this->door_guard_pos = door_guard_pos;
	event_id = agent->GetUniqueId();
	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	current_state = new OracleDefendLine(agent, this, third_base_pos, door_guard_pos);
	for (int i = 0; i < oracles.size(); i++)
	{
		time_last_attacked[oracles[i]] = 0;
		has_attacked[oracles[i]] = true;
		is_beam_active[oracles[i]] = false;
		casting[oracles[i]] = false;
		casting_energy[oracles[i]] = 0;
	}
	current_state->EnterState();
}

OracleHarassStateMachine::OracleHarassStateMachine(TheBigBot* agent, Units oracles, std::string name)
{
	this->agent = agent;
	this->oracles = oracles;
	this->name = name;
	event_id = agent->GetUniqueId();
	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	if (agent->Observation()->GetGameLoop() % 2 == 0)
	{
		harass_direction = true;
		harass_index = 0;
		current_state = new OracleHarassGroupUp(agent, this, agent->locations->oracle_path.entrance_point);
	}
	else
	{
		harass_direction = false;
		harass_index = agent->locations->oracle_path.entrance_points.size() - 1;
		current_state = new OracleHarassGroupUp(agent, this, agent->locations->oracle_path.exit_point);
	}
	
	for (int i = 0; i < oracles.size(); i++)
	{
		time_last_attacked[oracles[i]] = 0;
		has_attacked[oracles[i]] = true;
		is_beam_active[oracles[i]] = false;
		casting[oracles[i]] = false;
		casting_energy[oracles[i]] = 0;
	}
	current_state->EnterState();
}

OracleHarassStateMachine::~OracleHarassStateMachine()
{
	agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

void OracleHarassStateMachine::AddOracle(const Unit* oracle)
{
	oracles.push_back(oracle);
	time_last_attacked[oracle] = 0;
	has_attacked[oracle] = true;
	is_beam_active[oracle] = false;
	casting[oracle] = false;
	casting_energy[oracle] = 0;
}

bool OracleHarassStateMachine::AddUnit(const Unit* unit)
{
	if (unit->unit_type != ORACLE)
		return false;
	if (dynamic_cast<OracleDefendLocation*>(current_state) || dynamic_cast<OracleDefendLine*>(current_state) ||
		dynamic_cast<OracleDefendArmyGroup*>(current_state) || dynamic_cast<OracleHarassGroupUp*>(current_state))
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
		int index = found - oracles.begin();
		oracles.erase(oracles.begin() + index);
		OracleHarassAttackMineralLine* state = dynamic_cast<OracleHarassAttackMineralLine*>(current_state);
		if (state != NULL)
		{
			state->lost_oracle = true;
		}
	}
}

#pragma endregion

}