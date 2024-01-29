#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"

#include "army_group.h"
#include "TossBot.h"
#include "utility.h"
#include "finish_state_machine.h"
#include "locations.h"

namespace sc2 {

	ArmyGroup::ArmyGroup(TossBot* agent) : persistent_fire_control(agent)
	{
		this->agent = agent;
		event_id = agent->GetUniqueId();
		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);
	}

	ArmyGroup::ArmyGroup(TossBot* agent, Units all_units, std::vector<Point2D> path, int index) : persistent_fire_control(agent)
	{
		this->agent = agent;
		this->all_units = all_units;

		for (const auto &unit : all_units)
		{
			if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ZEALOT)
				zealots.push_back(unit);
			else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_STALKER)
			{
				stalkers.push_back(unit);
				last_time_blinked[unit] = 0;
			}
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
		}
		attack_path = path;
		current_attack_index = 2;
		high_ground_index = index;
		event_id = agent->GetUniqueId();
	}

	void ArmyGroup::AddUnit(const Unit* unit)
	{
		UNIT_TYPEID type = unit->unit_type.ToType();
		if (type != UNIT_TYPEID::PROTOSS_WARPPRISM)
			persistent_fire_control.AddFriendlyUnit(unit);

		attack_status[unit] = false;
		all_units.push_back(unit);
		new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());

		switch (type)
		{
		case UNIT_TYPEID::PROTOSS_ZEALOT:
			zealots.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_STALKER:
			stalkers.push_back(unit);
			last_time_blinked[unit] = 0;
			break;
		case UNIT_TYPEID::PROTOSS_ADEPT:
			adepts.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_SENTRY:
			sentries.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
			high_templar.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
			dark_templar.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_ARCHON:
			archons.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_IMMORTAL:
			immortals.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_COLOSSUS:
			collossi.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_DISRUPTOR:
			disrupter.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_OBSERVER:
			observers.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_WARPPRISM:
			warp_prisms.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_PHOENIX:
			phoenixes.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_VOIDRAY:
			void_rays.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_ORACLE:
			oracles.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_CARRIER:
			carriers.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_TEMPEST:
			tempests.push_back(unit);
			break;
		default:
			std::cout << "Error unknown unit type in ArmyGroup::AddUnit";
		}
	}

	void ArmyGroup::AddNewUnit(const Unit* unit)
	{
		new_units.push_back(unit);
	}

	void ArmyGroup::RemoveUnit(const Unit* unit)
	{
		all_units.erase(std::remove(all_units.begin(), all_units.end(), unit), all_units.end());
		attack_status.erase(unit);

		Units* units;
		if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ZEALOT)
			units = &zealots;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_STALKER)
		{
			units = &stalkers;
			auto index = std::find(stalkers.begin(), stalkers.end(), unit);
			if (index == stalkers.end())
				std::cout << "Error trying to remove stalker not in stalkers in RemoveUnit\n";
			else
			{
				stalkers.erase(std::remove(stalkers.begin(), stalkers.end(), unit), stalkers.end());
				if (last_time_blinked.count(unit))
					last_time_blinked.erase(unit);
			}
		}
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ADEPT)
			units = &adepts;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_SENTRY)
			units = &sentries;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_HIGHTEMPLAR)
			units = &high_templar;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_DARKTEMPLAR)
			units = &dark_templar;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ARCHON)
			units = &archons;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_IMMORTAL)
			units = &immortals;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_COLOSSUS)
			units = &collossi;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_DISRUPTOR)
			units = &disrupter;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_OBSERVER)
			units = &observers;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_WARPPRISM)
			units = &warp_prisms;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_PHOENIX)
			units = &phoenixes;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_VOIDRAY)
			units = &void_rays;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_ORACLE)
			units = &oracles;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_CARRIER)
			units = &carriers;
		else if (unit->unit_type.ToType() == UNIT_TYPEID::PROTOSS_TEMPEST)
			units = &tempests;
		else
		{
			std::cout << "Error unknown unit type in ArmyGroup::AddUnit";
			return;
		}

		units->erase(std::remove(units->begin(), units->end(), unit), units->end());
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
		for (const auto &Funit : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_STALKER)))
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

	void ArmyGroup::FindStalkerPositions(std::map<const Unit*, Point2D>& attacking_unit_positions, std::map<const Unit*, Point2D>& retreating_unit_positions, float unit_size, float unit_dispersion)
	{
		Units enemies = agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
		Point2D stalkers_center = Utility::MedianCenter(stalkers);
		Point2D stalker_line_pos = attack_path_line.FindClosestPoint(stalkers_center);


		Units close_enemies = Utility::NClosestUnits(enemies, stalkers_center, 5);
		// remove far enemies
		for (int i = 0; i < close_enemies.size(); i++)
		{
			if (Distance2D(stalkers_center, close_enemies[i]->pos) > 12)
			{
				close_enemies.erase(close_enemies.begin() + i);
				i--;
			}
		}


		//Point2D concave_target = attack_path_line.GetPointFrom(stalker_line_pos, 8, true);
		Point2D concave_target = agent->Observation()->GetGameInfo().enemy_start_locations[0];

		float max_range = 7;
		if (close_enemies.size() > 0)
		{
			concave_target = Utility::MedianCenter(close_enemies);
			max_range = std::max(Utility::GetMaxRange(close_enemies) + 2, 6.0f);
		}

		Point2D retreat_concave_origin = attack_path_line.GetPointFrom(concave_target, max_range, false);
		if (retreat_concave_origin == Point2D(0, 0))
			retreat_concave_origin = attack_path_line.GetPointFrom(stalker_line_pos, 2 * unit_size + unit_dispersion, false);

		Point2D attack_concave_origin = attack_path_line.GetPointFrom(stalker_line_pos, 2 * unit_size + unit_dispersion, true);



		std::vector<Point2D> attack_concave_positions = FindConcaveFromBack(attack_concave_origin, (2 * attack_concave_origin) - concave_target, stalkers.size(), .625, unit_dispersion);
		std::vector<Point2D> retreat_concave_positions = FindConcave(retreat_concave_origin, (2 * retreat_concave_origin) - concave_target, stalkers.size(), .625, unit_dispersion);

		attacking_unit_positions = AssignUnitsToPositions(stalkers, attack_concave_positions);
		retreating_unit_positions = AssignUnitsToPositions(stalkers, retreat_concave_positions);


		for (const auto &pos : attack_concave_positions)
		{
			agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .625, Color(255, 0, 0));
		}
		for (const auto &pos : retreat_concave_positions)
		{
			agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .625, Color(0, 255, 0));
		}
	}

	void ArmyGroup::MicroStalkerAttack()
	{
		if (stalkers.size() == 0)
			return;

		for (const auto &pos : attack_path_line.GetPoints())
		{
			agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .5, Color(255, 255, 255));
		}

		std::map<const Unit*, Point2D> attacking_unit_positions;
		std::map<const Unit*, Point2D> retreating_unit_positions;
		float unit_size = .625;
		float unit_dispersion = .16;

		FindStalkerPositions(attacking_unit_positions, retreating_unit_positions, unit_size, unit_dispersion);

		Point2D center = Utility::MedianCenter(stalkers);
		std::map<const Unit*, int> units_requesting_pickup;

		bool all_ready = true;
		Units stalkers_ready;
		Units stalkers_not_ready;
		for (const auto &stalker : stalkers)
		{
			if (stalker->weapon_cooldown == 0 && attack_status[stalker] == false)
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
						stalkers_not_ready.push_back(stalker);
				}
				stalkers_ready.push_back(stalker);
			}
			else
			{
				stalkers_not_ready.push_back(stalker);
			}
		}
		bool enough_stalkers_ready = true;
		if (static_cast<float>(stalkers_ready.size()) / static_cast<float>(stalkers.size()) >= 1)
		{
			std::map<const Unit*, const Unit*> found_targets = agent->FindTargets(stalkers_ready, {}, 0);
			if (found_targets.size() == 0)
			{
				found_targets = agent->FindTargets(stalkers_ready, {}, 2);
				std::cout << "extra distance\n";
			}

			//agent->PrintAttacks(found_targets);

			for (const auto &stalker : stalkers_ready)
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
			}
		}
		else
		{
			enough_stalkers_ready = false;
		}

		if (stalkers_not_ready.size() > 0)
		{
			for (const auto &stalker : stalkers)
			{
				if (enough_stalkers_ready == true && std::find(stalkers_not_ready.begin(), stalkers_not_ready.end(), stalker) == stalkers_not_ready.end())
					continue;

				int danger = agent->IncomingDamage(stalker);
				if (danger > 0)
				{
					/*if (danger > stalker->shield || danger > (stalker->shield_max / 2) || stalker->shield == 0)
					{
						float now = agent->Observation()->GetGameLoop() / 22.4;
						bool blink_off_cooldown = now - last_time_blinked[stalker] > 7;

						if (agent->has_blink && blink_off_cooldown)
						{
							if (stalker->orders.size() > 0 && stalker->orders[0].ability_id == ABILITY_ID::ATTACK && stalker->weapon_cooldown == 0)
								agent->Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, Utility::PointBetween(stalker->pos, retreat_point, 7), true); // TODO adjustable blink distance
							else
								agent->Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, Utility::PointBetween(stalker->pos, retreat_point, 7)); // TODO adjustable blink distance
							//agent->Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attack_point, true);
							attack_status[stalker] = false;
							last_time_blinked[stalker] = now;
							continue;
						}
					}*/

					units_requesting_pickup[stalker] = danger;
				}

				agent->Debug()->DebugSphereOut(stalker->pos, .5, Color(0, 255, 255));
				agent->Debug()->DebugTextOut(std::to_string(danger), stalker->pos, Color(0, 255, 255), 15);

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
				else if (stalker->orders.size() == 0 || stalker->orders[0].ability_id == ABILITY_ID::MOVE_MOVE || stalker->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE)
				{
					// attack order is no longer valid
					attack_status[stalker] = false;
				}
			}
		}

		for (const auto &prism : warp_prisms)
		{
			agent->Actions()->UnitCommand(prism, ABILITY_ID::MOVE_MOVE, center);
			agent->Actions()->UnitCommand(prism, ABILITY_ID::UNLOADALLAT, prism->pos);
		}

		PickUpUnits(units_requesting_pickup);
	}

	void ArmyGroup::ApplyPressureGrouped(Point2D attack_point, Point2D retreat_point, std::map<const Unit*, Point2D> retreating_unit_positions, std::map<const Unit*, Point2D> attacking_unit_positions)
	{
		unsigned long long start_time = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
		
		std::ofstream pressure_time;
		pressure_time.open("pressure_time.txt", std::ios_base::app);

		unsigned long long ready_check = 0;
		unsigned long long find_targets = 0;
		unsigned long long print_attacks = 0;
		unsigned long long give_targets = 0;
		unsigned long long not_ready = 0;
		unsigned long long pick_up = 0;
		

		std::map<const Unit*, int> units_requesting_pickup;
		if (stalkers.size() > 0)
		{
			Point2D center = Utility::MedianCenter(stalkers);
			for (const auto &carrier : carriers)
			{
				if (Distance2D(carrier->pos, center) > 2)
					agent->Actions()->UnitCommand(carrier, ABILITY_ID::MOVE_MOVE, center);
				else
					agent->Actions()->UnitCommand(carrier, ABILITY_ID::ATTACK_ATTACK, attack_point);
			}

			bool all_ready = true;
			Units stalkers_ready;
			Units stalkers_not_ready;
			for (const auto &stalker : stalkers)
			{
				if (stalker->weapon_cooldown == 0 && attack_status[stalker] == false)
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
							stalkers_not_ready.push_back(stalker);
					}
					stalkers_ready.push_back(stalker);
				}
				else
				{
					stalkers_not_ready.push_back(stalker);
				}
			}
			bool enough_stalkers_ready = true;
			if (static_cast<float>(stalkers_ready.size()) / static_cast<float>(stalkers.size()) >= 1)
			{
				start_time = std::chrono::duration_cast<std::chrono::microseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()
					).count();

				std::map<const Unit*, const Unit*> found_targets = agent->FindTargets(stalkers_ready, {}, 0);
				if (found_targets.size() == 0)
				{
					found_targets = agent->FindTargets(stalkers_ready, {}, 2);
					std::cout << "extra distance\n";
				}
				find_targets = std::chrono::duration_cast<std::chrono::microseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()
					).count() - start_time;

				start_time = std::chrono::duration_cast<std::chrono::microseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()
					).count();

				//agent->PrintAttacks(found_targets);

				print_attacks = std::chrono::duration_cast<std::chrono::microseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()
					).count() - start_time;

				for (const auto &stalker : stalkers_ready)
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
				enough_stalkers_ready = false;
			}
			if (stalkers_not_ready.size() > 0)
			{
				for (const auto &stalker : stalkers)
				{
					if (enough_stalkers_ready == true && std::find(stalkers_not_ready.begin(), stalkers_not_ready.end(), stalker) == stalkers_not_ready.end())
						continue;

					int danger = agent->IncomingDamage(stalker);
					if (danger > 0)
					{
						if (danger > stalker->shield || danger > (stalker->shield_max / 2) || stalker->shield == 0)
						{
							float now = agent->Observation()->GetGameLoop() / 22.4;
							bool blink_off_cooldown = now - last_time_blinked[stalker] > 7;

							if (agent->has_blink && blink_off_cooldown)
							{
								if (stalker->orders.size() > 0 && stalker->orders[0].ability_id == ABILITY_ID::ATTACK && stalker->weapon_cooldown == 0)
									agent->Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, Utility::PointBetween(stalker->pos, retreat_point, 7), true); // TODO adjustable blink distance
								else
									agent->Actions()->UnitCommand(stalker, ABILITY_ID::EFFECT_BLINK, Utility::PointBetween(stalker->pos, retreat_point, 7)); // TODO adjustable blink distance
								//agent->Actions()->UnitCommand(stalker, ABILITY_ID::ATTACK, attack_point, true);
								attack_status[stalker] = false;
								last_time_blinked[stalker] = now;
								continue;
							}
						}

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
					else if (stalker->orders.size() == 0 || stalker->orders[0].ability_id == ABILITY_ID::MOVE_MOVE || stalker->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE)
					{
						// attack order is no longer valid
						attack_status[stalker] = false;
					}
				}
			}
		}

		PickUpUnits(units_requesting_pickup);


		pressure_time << ready_check << ", ";
		pressure_time << find_targets << ", ";
		pressure_time << print_attacks << ", ";
		pressure_time << give_targets << ", ";
		pressure_time << not_ready << ", ";
		pressure_time << pick_up << "\n";
		pressure_time.close();
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
		StateMachine* oracle_fsm = new OracleHarassStateMachine(agent, oracles, base_location, "Oracles");
		agent->active_FSMs.push_back(oracle_fsm);
	}

	void ArmyGroup::CannonRushPressure()
	{
		Point2D attack_pos = agent->Observation()->GetGameInfo().enemy_start_locations[0];
		Point2D retreat_pos;
		if (agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_SHIELDBATTERY)).size() > 0)
			retreat_pos = Utility::FurthestFrom(agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_SHIELDBATTERY)), attack_pos)->pos;
		else if (agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_PHOTONCANNON)).size() > 0)
			retreat_pos = Utility::FurthestFrom(agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_PHOTONCANNON)), attack_pos)->pos;

		Units enemies = agent->Observation()->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
		if (enemies.size() > 0)
		{
			const Unit* closest_enemy = Utility::ClosestUnitTo(enemies, retreat_pos);
			if (Distance2D(retreat_pos, closest_enemy->pos) > 11)
				retreat_pos = Utility::PointBetween(closest_enemy->pos, retreat_pos, 11);
		}
		else
		{
			enemies = agent->Observation()->GetUnits(Unit::Alliance::Enemy);
			const Unit* closest_enemy = Utility::ClosestUnitTo(enemies, retreat_pos);
			if (Distance2D(retreat_pos, closest_enemy->pos) > 8)
				retreat_pos = Utility::PointBetween(closest_enemy->pos, retreat_pos, 8);
		}
		
		agent->Debug()->DebugSphereOut(agent->ToPoint3D(attack_pos), 2, Color(255, 255, 0));
		agent->Debug()->DebugSphereOut(agent->ToPoint3D(retreat_pos), 2, Color(0, 255, 0));

		Units units_ready;
		Units units_on_cd;
		for (const auto &tempest : tempests)
		{
			agent->Debug()->DebugSphereOut(agent->ToPoint3D(tempest->pos), 10, Color(255, 255, 255));
			if (attack_status[tempest] == false && tempest->weapon_cooldown == 0)
				units_ready.push_back(tempest);
			if (tempest->weapon_cooldown > 0)
			{
				units_on_cd.push_back(tempest);
				if (attack_status[tempest] == true)
					attack_status[tempest] = false;
			}
		}
		for (const auto& stalker : stalkers)
		{
			if (attack_status[stalker] == false && stalker->weapon_cooldown == 0)
				units_ready.push_back(stalker);
			if (stalker->weapon_cooldown > 0)
			{
				units_on_cd.push_back(stalker);
				if (attack_status[stalker] == true)
					attack_status[stalker] = false;
			}
		}

		if (units_ready.size() > 0)
		{
			persistent_fire_control.UpdateEnemyUnitHealth();
			std::map<const Unit*, const Unit*> attacks = persistent_fire_control.FindAttacks(units_ready, 
				{UNIT_TYPEID::TERRAN_SCV, UNIT_TYPEID::TERRAN_VIKINGFIGHTER, UNIT_TYPEID::TERRAN_THORAP, UNIT_TYPEID::TERRAN_WIDOWMINE, 
				UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED, UNIT_TYPEID::TERRAN_CYCLONE, UNIT_TYPEID::TERRAN_THOR, UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, UNIT_TYPEID::TERRAN_SIEGETANK,
				UNIT_TYPEID::TERRAN_MARINE, UNIT_TYPEID::TERRAN_MEDIVAC, UNIT_TYPEID::TERRAN_RAVEN, UNIT_TYPEID::TERRAN_BATTLECRUISER, 
				UNIT_TYPEID::TERRAN_LIBERATOR, UNIT_TYPEID::TERRAN_LIBERATORAG, UNIT_TYPEID::TERRAN_BANSHEE, UNIT_TYPEID::TERRAN_GHOST, UNIT_TYPEID::TERRAN_VIKINGASSAULT,
				UNIT_TYPEID::TERRAN_MARAUDER, UNIT_TYPEID::TERRAN_REAPER, UNIT_TYPEID::TERRAN_HELLION, UNIT_TYPEID::TERRAN_HELLIONTANK, 
				UNIT_TYPEID::TERRAN_STARPORTREACTOR, UNIT_TYPEID::TERRAN_FACTORYREACTOR, UNIT_TYPEID::TERRAN_BARRACKSREACTOR, UNIT_TYPEID::TERRAN_REACTOR, 
				UNIT_TYPEID::TERRAN_STARPORT, UNIT_TYPEID::TERRAN_STARPORTFLYING, UNIT_TYPEID::TERRAN_FACTORY, UNIT_TYPEID::TERRAN_FACTORYFLYING, 
				UNIT_TYPEID::TERRAN_BARRACKS, UNIT_TYPEID::TERRAN_BARRACKSFLYING, UNIT_TYPEID::TERRAN_MISSILETURRET, UNIT_TYPEID::TERRAN_BUNKER, 
				UNIT_TYPEID::TERRAN_REFINERY, UNIT_TYPEID::TERRAN_ORBITALCOMMAND, UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING, UNIT_TYPEID::TERRAN_COMMANDCENTER, 
				UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING }, 0);
			for (const auto& unit : units_ready)
			{
				if (attacks.size() == 0)
				{
					agent->Actions()->UnitCommand(unit, ABILITY_ID::MOVE_MOVE, attack_pos);
				}
				if (attacks.count(unit) > 0)
				{
					agent->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, attacks[unit]);
					attack_status[unit] = true;
					agent->Debug()->DebugLineOut(unit->pos, attacks[unit]->pos, Color(0, 255, 0));
				}
			}
			agent->PrintAttacks(attacks);
		}

		for (const auto& unit : persistent_fire_control.enemy_unit_hp)
		{
			agent->Debug()->DebugTextOut(std::to_string(unit.second), unit.first->pos, Color(0, 255, 0), 15);
		}

		for (const auto& unit : units_on_cd)
		{
			agent->Actions()->UnitCommand(unit, ABILITY_ID::MOVE_MOVE, retreat_pos);
		}
	}

	void ArmyGroup::MicroUnits()
	{

	}


	void ArmyGroup::AutoAddNewUnits(std::vector<UNIT_TYPEID> unit_types)
	{
		std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
			this->OnNewUnitCreatedListener(unit);
		};
		agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);
		this->unit_types = unit_types;
	}

	void ArmyGroup::OnNewUnitCreatedListener(const Unit* unit)
	{
		if (std::find(unit_types.begin(), unit_types.end(), unit->unit_type) != unit_types.end())
			AddNewUnit(unit);
	}

	void ArmyGroup::AutoAddUnits(std::vector<UNIT_TYPEID> unit_types)
	{
		std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
			this->OnUnitCreatedListener(unit);
		};
		agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);
		this->unit_types = unit_types;
	}

	void ArmyGroup::OnUnitCreatedListener(const Unit* unit)
	{
		if (std::find(unit_types.begin(), unit_types.end(), unit->unit_type) != unit_types.end())
			AddUnit(unit);
	}

	void ArmyGroup::OnUnitDestroyedListener(const Unit* unit)
	{
		if (std::find(all_units.begin(), all_units.end(), unit) != all_units.end())
			RemoveUnit(unit);
	}

}