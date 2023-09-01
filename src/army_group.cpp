#pragma once
#include <iostream>
#include <string>

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"

#include "army_group.h"
#include "TossBot.h"
#include "utility.h"
#include "finish_state_machine.h"
#include "locations.h"

namespace sc2 {

	void ArmyGroup::AddUnit(const Unit* unit)
	{
		all_units.push_back(unit);
		new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());

		if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ZEALOT)
			zealots.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_STALKER)
			stalkers.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ADEPT)
			adepts.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_SENTRY)
			sentries.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_HIGHTEMPLAR)
			high_templar.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_DARKTEMPLAR)
			dark_templar.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ARCHON)
			archons.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_IMMORTAL)
			immortals.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_COLOSSUS)
			collossi.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_DISRUPTOR)
			disrupter.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_OBSERVER)
			observers.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_WARPPRISM)
			warp_prisms.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_PHOENIX)
			phoenixes.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_VOIDRAY)
			void_rays.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ORACLE)
			oracles.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_CARRIER)
			carriers.push_back(unit);
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_TEMPEST)
			tempests.push_back(unit);
		else
			std::cout << "Error unknown unit type in ArmyGroup::AddUnit";
	}

	void ArmyGroup::AddNewUnit(const Unit* unit)
	{
		new_units.push_back(unit);
	}


	std::vector<Point2D> ArmyGroup::FindConcave(Point2D origin, Point2D fallback_point, int num_units, float unit_size, float dispersion)
	{
		float range = 0; //r
		float unit_radius = unit_size + dispersion; //u
		float concave_degree = 30; //p
		int max_width = 4;

		Point2D backward_vector = fallback_point - origin;
		Point2D forward_vector = origin - fallback_point;
		forward_vector /= sqrt(forward_vector.x * forward_vector.x + forward_vector.y * forward_vector.y);

		Point2D offset_circle_center = Point2D(origin.x + concave_degree * forward_vector.x, origin.y + concave_degree * forward_vector.y);

		float backwards_direction = atan2(backward_vector.y, backward_vector.x);
		float arclength = (2 * unit_radius) / (range + concave_degree + unit_radius);

		std::vector<Point2D> concave_points;

		int row = 0;

		while (concave_points.size() < num_units)
		{
			row++;
			// even row
			bool left_limit = false;
			bool right_limit = false;
			float arclength = (2 * unit_radius) / (range + concave_degree + (((row * 2) - 1) * unit_radius));
			for (float i = .5; i <= max_width - .5; i += 1)
			{
				if (!right_limit)
				{
					float unit_direction = backwards_direction + i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
					if (agent->Observation()->IsPathable(unit_position))
					{
						concave_points.push_back(unit_position);
					}
					else
					{
						right_limit = true;
					}
				}
				if ((right_limit && left_limit) || concave_points.size() >= num_units)
					break;

				if (!left_limit)
				{
					float unit_direction = backwards_direction - i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
					if (agent->Observation()->IsPathable(unit_position))
					{
						concave_points.push_back(unit_position);
					}
					else
					{
						left_limit = true;
					}
				}
				if ((right_limit && left_limit) || concave_points.size() >= num_units)
					break;
			}
			if (concave_points.size() >= num_units)
				break;

			// odd row
			row++;
			// middle point
			float unit_direction = backwards_direction;
			Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
				offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
			if (agent->Observation()->IsPathable(unit_position))
			{
				concave_points.push_back(unit_position);
			}

			left_limit = false;
			right_limit = false;
			arclength = (2 * unit_radius) / (range + concave_degree + (((row * 2) - 1) * unit_radius));
			for (int i = 1; i <= max_width - 1; i++)
			{
				if (!right_limit)
				{
					float unit_direction = backwards_direction + i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
					if (agent->Observation()->IsPathable(unit_position))
					{
						concave_points.push_back(unit_position);
					}
					else
					{
						right_limit = true;
					}
				}
				if ((right_limit && left_limit) || concave_points.size() >= num_units)
					break;

				if (!left_limit)
				{
					float unit_direction = backwards_direction - i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
					if (agent->Observation()->IsPathable(unit_position))
					{
						concave_points.push_back(unit_position);
					}
					else
					{
						left_limit = true;
					}
				}
				if ((right_limit && left_limit) || concave_points.size() >= num_units)
					break;

			}
		}
		return concave_points;
	}

	std::vector<Point2D> ArmyGroup::FindConcaveFromBack(Point2D origin, Point2D fallback_point, int num_units, float unit_size, float dispersion)
	{
		Point2D current_origin = origin;
		while (true)
		{
			std::vector<Point2D> concave_points = FindConcave(current_origin, fallback_point, num_units, unit_size, dispersion);
			Point2D furthest_back = Utility::ClosestPointOnLine(concave_points.back(), origin, fallback_point);
			if (Distance2D(origin, fallback_point) < Distance2D(furthest_back, fallback_point))
				return concave_points;
			current_origin = Utility::PointBetween(current_origin, fallback_point, -(unit_size + dispersion));
		}

	}

	bool ArmyGroup::TestSwap(Point2D pos1, Point2D target1, Point2D pos2, Point2D target2)
	{
		float curr_max = std::max(Distance2D(pos1, target1), Distance2D(pos2, target2));
		float swap_max = std::max(Distance2D(pos1, target2), Distance2D(pos2, target1));
		return swap_max < curr_max;
	}

	std::map<const Unit*, Point2D> ArmyGroup::AssignUnitsToPositions(Units units, std::vector<Point2D> positions)
	{
		std::map<const Unit*, Point2D> unit_assignments;
		for (const auto &unit : units)
		{
			Point2D closest = Utility::ClosestTo(positions, unit->pos);
			unit_assignments[unit] = closest;
			positions.erase(std::remove(positions.begin(), positions.end(), closest), positions.end());
		}
		for (int i = 0; i < units.size() - 1; i++)
		{
			for (int j = i + 1; j < units.size(); j++)
			{
				if (TestSwap(units[i]->pos, unit_assignments[units[i]], units[j]->pos, unit_assignments[units[j]]))
				{
					Point2D temp = unit_assignments[units[i]];
					unit_assignments[units[i]] = unit_assignments[units[j]];
					unit_assignments[units[j]] = temp;
				}
			}
		}
		return unit_assignments;
	}


	void ArmyGroup::PickUpUnits(std::map<const Unit*, int> unit_danger_levels)
	{
		std::map<const Unit*, int> prism_free_slots;
		for (const auto &prism : warp_prisms)
		{
			int free_slots = prism->cargo_space_max - prism->cargo_space_taken;
			if (free_slots > 0)
				prism_free_slots[prism] = free_slots;
		}
		if (warp_prisms.size() == 0 || prism_free_slots.size() == 0 || unit_danger_levels.size() == 0)
			return;

		Units units;
		for (const auto &unit : unit_danger_levels)
		{
			units.push_back(unit.first);
		}
		// order units by priority
		std::sort(units.begin(), units.end(),
			[&unit_danger_levels](const Unit* a, const Unit* b) -> bool
		{
			int a_danger = unit_danger_levels[a];
			int b_danger = unit_danger_levels[b];
			// priority units

			// higher danger vs hp/shields
			if (a->shield + a->health <= a_danger)
				return true;
			if (b->shield + b->health <= b_danger)
				return false;
			return a_danger - a->shield > b_danger - b->shield;
		});


		// find prism to pick up each unit if there is space
		for (const auto &unit : units)
		{
			int cargo_size = Utility::GetCargoSize(unit);
			for (auto &prism : prism_free_slots)
			{
				if (prism.second < cargo_size || Distance2D(prism.first->pos, unit->pos) > 5)
					continue;

				attack_status[unit] = false;
				if (unit->orders.size() > 0 && unit->orders[0].ability_id == ABILITY_ID::ATTACK && unit->weapon_cooldown == 0)
					agent->Actions()->UnitCommand(unit, ABILITY_ID::SMART, prism.first, true);
				else
					agent->Actions()->UnitCommand(unit, ABILITY_ID::SMART, prism.first);
				prism.second -= cargo_size;
				break;
			}
		}
	}

	void ArmyGroup::DodgeShots()
	{
		for (const auto &Funit : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_STALKER)))
		{
			int danger = agent->IncomingDamage(Funit);
			if (danger)
			{
				bool blink_ready = false;
				for (const auto &abiliy : agent->Query()->GetAbilitiesForUnit(Funit).abilities)
				{
					if (abiliy.ability_id == ABILITY_ID::EFFECT_BLINK)
					{
						blink_ready = true;
						break;
					}
				}
				if (blink_ready && (danger > Funit->shield || danger > (Funit->shield_max / 2)))
				{
					agent->Actions()->UnitCommand(Funit, ABILITY_ID::EFFECT_BLINK, Funit->pos + Point2D(0, 4));
					agent->Actions()->UnitCommand(Funit, ABILITY_ID::ATTACK, Funit->pos - Point2D(0, 4), true);
					agent->Debug()->DebugTextOut(std::to_string(danger), Funit->pos, Color(0, 255, 0), 20);
				}
				else
				{
					agent->Debug()->DebugTextOut(std::to_string(danger), Funit->pos, Color(255, 0, 0), 20);
				}
			}
		}
	}


	void ArmyGroup::ApplyPressureGrouped(Point2D attack_point, Point2D retreat_point, std::map<const Unit*, Point2D> retreating_unit_positions, std::map<const Unit*, Point2D> attacking_unit_positions)
	{
		std::map<const Unit*, int> units_requesting_pickup;
		if (stalkers.size() > 0)
		{
			bool all_ready = true;
			for (const auto &stalker : stalkers)
			{
				if (stalker->weapon_cooldown > 0 || attack_status[stalker] == true)
				{
					// ignore units inside prisms
					if (warp_prisms.size() > 0)
					{
						bool in_prism = false;
						for (const auto &prism : warp_prisms)
						{
							for (const auto &passanger : prism->passengers)
							{
								if (passanger.tag == stalker->tag)
								{
									in_prism = true;
									break;
								}
							}
							if (in_prism)
								break;
						}
						if (in_prism)
							continue;
					}
					all_ready = false;
					break;
				}
			}
			if (all_ready)
			{
				std::map<const Unit*, const Unit*> found_targets = agent->FindTargets(stalkers, {}, 2);
				if (found_targets.size() == 0)
				{
					found_targets = agent->FindTargets(stalkers, {}, 2);
					std::cout << "extra distance\n";
				}
				agent->PrintAttacks(found_targets);

				for (const auto &stalker : stalkers)
				{
					if (found_targets.size() == 0)
					{
						agent->Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attacking_unit_positions[stalker]);
					}
					if (found_targets.count(stalker) > 0)
					{
						agent->Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, found_targets[stalker]);
						attack_status[stalker] = true;
					}
					/*else
					{
						Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, army->attack_point);
					}*/
				}
			}
			else
			{
				for (const auto &stalker : stalkers)
				{
					int danger = agent->IncomingDamage(stalker);
					if (danger > 0)
					{
						bool using_blink = false;

						if (danger > stalker->shield || danger > (stalker->shield_max / 2) || stalker->shield == 0)
						{
							for (const auto &abiliy : agent->Query()->GetAbilitiesForUnit(stalker).abilities)
							{
								if (abiliy.ability_id == ABILITY_ID::EFFECT_BLINK)
								{
									if (stalker->orders.size() > 0 && stalker->orders[0].ability_id == ABILITY_ID::ATTACK && stalker->weapon_cooldown == 0)
										agent->Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, Utility::PointBetween(stalker->pos, retreat_point, 7), true); // TODO adjustable blink distance
									else
										agent->Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, Utility::PointBetween(stalker->pos, retreat_point, 7)); // TODO adjustable blink distance
									agent->Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attack_point, true);
									attack_status[stalker] = false;
									using_blink = true;
									break;
								}
							}
						}

						if (!using_blink)
							units_requesting_pickup[stalker] = danger;
					}
					if (attack_status[stalker] == false)
					{
						// no order but no danger so just move back
						agent->Actions()->UnitCommand(stalker, ABILITY_ID::MOVE_MOVE, retreating_unit_positions[stalker]);
					}
					else if (stalker->weapon_cooldown > 0)
					{
						// attack has gone off so reset order status
						attack_status[stalker] = false;
					}
				}
			}
		}
		PickUpUnits(units_requesting_pickup);
	}

	void ArmyGroup::DefendFrontDoor(Point2D door_open_pos, Point2D door_closed_pos)
	{
		DoorGuardStateMachine* door_guard_fsm = new DoorGuardStateMachine(agent, "Door Guard", adepts[0], door_open_pos, door_closed_pos);
		agent->active_FSMs.push_back(door_guard_fsm);
	}

	void ArmyGroup::DefendExpansion(Point2D base_location, Point2D pylon_gap_location)
	{
		for (const auto &adept : adepts)
		{
			if (Distance2D(adept->pos, pylon_gap_location) > 1)
			{
				agent->Actions()->UnitCommand(adept, ABILITY_ID::MOVE_MOVE, pylon_gap_location);
				agent->Actions()->UnitCommand(adept, ABILITY_ID::GENERAL_HOLDPOSITION, true);
			}
		}
		StateMachine* oracle_fsm = new StateMachine(agent, new OracleDefend(agent, oracles, base_location), "Oracles");
		agent->active_FSMs.push_back(oracle_fsm);
	}


	void ArmyGroup::MicroUnits()
	{

	}


	void ArmyGroup::AutoAddStalkers()
	{

		std::function<void(const Unit*)> onStalkerCreated = [=](const Unit* unit) {
			this->OnStalkerCreatedListener(unit);
		};
		agent->AddListenerToOnUnitCreatedEvent(onStalkerCreated);
	}

	void ArmyGroup::OnStalkerCreatedListener(const Unit* unit)
	{
		if (unit->unit_type == UNIT_TYPEID::PROTOSS_STALKER)
			AddNewUnit(unit);
	}

}