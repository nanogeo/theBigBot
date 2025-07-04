
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>


#include "army_group.h"
#include "theBigBot.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {


	ArmyGroup::~ArmyGroup()
	{

	}

	void ArmyGroup::ScourMap()
	{
		for (int i = new_units.size() - 1; i >= 0; i--)
		{
			AddUnit(new_units[i]);
		}
		Units enemy_buildings = mediator->GetUnits(Unit::Alliance::Enemy, IsBuilding());
		Units enemy_ground_buildings;
		Units enemy_flying_buildings;
		for (const auto& building : enemy_buildings)
		{
			if (building->is_flying)
				enemy_flying_buildings.push_back(building);
			else
				enemy_ground_buildings.push_back(building);
		}
		ImageData raw_map = mediator->GetPathingGrid();
		for (const auto& unit : all_units)
		{
			if (unit->orders.size() == 0)
			{
				if (Utility::GetGroundRange(unit) == 0 && enemy_flying_buildings.size() > 0)
				{
					const Unit* closest = Utility::ClosestTo(enemy_flying_buildings, unit->pos);
					mediator->SetUnitCommand(unit, A_ATTACK, closest->pos, 0);
				}
				else if (Utility::GetAirRange(unit) == 0 && enemy_ground_buildings.size() > 0)
				{
					const Unit* closest = Utility::ClosestTo(enemy_ground_buildings, unit->pos);
					mediator->SetUnitCommand(unit, A_ATTACK, closest->pos, 0);
				}
				else if (Utility::GetGroundRange(unit) > 0 &&
					Utility::GetAirRange(unit) > 0 &&
					enemy_buildings.size() > 0)
				{
					const Unit* closest = Utility::ClosestTo(enemy_buildings, unit->pos);
					mediator->SetUnitCommand(unit, A_ATTACK, closest->pos, 0);
				}
				else
				{
					std::srand((unsigned int)(unit->tag + mediator->GetGameLoop()));
					int x = std::rand() % raw_map.width;
					int y = std::rand() % raw_map.height;
					Point2D pos = Point2D((float)x, (float)y);
					while (!unit->is_flying && !mediator->IsPathable(pos))
					{
						x = std::rand() % raw_map.width;
						y = std::rand() % raw_map.height;
						pos = Point2D((float)x, (float)y);
					}
					mediator->SetUnitCommand(unit, A_ATTACK, pos, 0);
				}
			}
		}
	}

	void ArmyGroup::AddUnit(const Unit* unit)
	{
		if (std::find(all_units.begin(), all_units.end(), unit) != all_units.end())
			return;

		all_units.push_back(unit);
		new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());
	}

	void ArmyGroup::AddNewUnit(const Unit* unit)
	{
		if (!ready)
		{
			AddUnit(unit);
			return;
		}

		if (std::find(new_units.begin(), new_units.end(), unit) != new_units.end())
			return;

		new_units.push_back(unit);
	}

	void ArmyGroup::RemoveUnit(const Unit* unit)
	{
		if (unit == nullptr)
			return; // TODO log error and callstack

		mediator->SetUnitCommand(unit, A_STOP, 1);

		all_units.erase(std::remove(all_units.begin(), all_units.end(), unit), all_units.end());
		new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());
	}

	Units ArmyGroup::GetExtraUnits()
	{
		Units extra_units;
		int extra_num = (int)(all_units.size() + new_units.size() - desired_units);
		if (extra_num <= 0)
			return extra_units;

		for (int i = (int)new_units.size() - 1; i >= 0; i --)
		{
			extra_units.push_back(new_units[i]);
			if (extra_units.size() == extra_num)
				return extra_units;
		}
		for (int i = (int)all_units.size() - 1; i >= 0; i--)
		{
			extra_units.push_back(all_units[i]);
			if (extra_units.size() == extra_num)
				break;
		}
		return extra_units;
	}

	/*
	std::vector<Point2D> ArmyGroup::FindConcave(Point2D origin, Point2D fallback_point, int num_units, float unit_size, float dispersion, float concave_degree)
	{
		float range = 0; //r
		float unit_radius = unit_size + dispersion; //u
		//float concave_degree = 30; //p
		int max_width = 4;

		Point2D backward_vector = fallback_point - origin;
		Point2D forward_vector = origin - fallback_point;
		forward_vector /= sqrt(forward_vector.x * forward_vector.x + forward_vector.y * forward_vector.y);

		Point2D offset_circle_center = Point2D(origin.x + concave_degree * forward_vector.x, origin.y + concave_degree * forward_vector.y);

		float backwards_direction = atan2(backward_vector.y, backward_vector.x);

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
					if (mediator->IsPathable(unit_position))
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
					if (mediator->IsPathable(unit_position))
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
			if (mediator->IsPathable(unit_position))
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
					unit_direction = backwards_direction + i * arclength;
					unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
					if (mediator->IsPathable(unit_position))
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
					unit_direction = backwards_direction - i * arclength;
					unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
					if (mediator->IsPathable(unit_position))
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
		int tries = 0;
		while (true)
		{
			tries++;
			std::vector<Point2D> concave_points = FindConcave(current_origin, fallback_point, num_units, unit_size, dispersion, 30);
			Point2D furthest_back = Utility::ClosestPointOnLine(concave_points.back(), origin, fallback_point);
			if (Distance2D(origin, fallback_point) < Distance2D(furthest_back, fallback_point))
				return concave_points;
			current_origin = Utility::PointBetween(current_origin, fallback_point, - Distance2D(origin, furthest_back) - (unit_size + dispersion));
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
		Point2D center_units = Utility::MedianCenter(units);
		Point2D center_positions = Utility::MedianCenter(positions);

		std::map<const Unit*, Point2D> relative_unit_position;
		std::map<OrderedPoint2D, Point2D> relative_position_positions;

		for (const auto& unit : units)
		{
			relative_unit_position[unit] = unit->pos - center_units;
		}
		for (const auto& position : positions)
		{
			relative_position_positions[position] = position - center_positions;
		}

		std::map<const Unit*, Point2D> unit_assignments;
		for (const auto &unit : units)
		{
			if (relative_position_positions.size() == 0)
				return unit_assignments;
			Point2D relative_pos = relative_unit_position[unit];
			Point2D current_closest;
			float current_distance = INFINITY;
			for (const auto& relative_point : relative_position_positions)
			{
				float distance = Distance2D(relative_pos, relative_point.second);
				if (distance < current_distance)
				{
					current_closest = relative_point.first;
					current_distance = distance;
				}
			}

			unit_assignments[unit] = current_closest;
			relative_position_positions.erase(current_closest);
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

				mediator->CancelAttack(unit);

				if (unit->orders.size() > 0 && unit->orders[0].ability_id == A_ATTACK && unit->weapon_cooldown == 0)
					mediator->SetUnitCommand(unit, A_SMART, prism.first, 0, true);
				else
					mediator->SetUnitCommand(unit, A_SMART, prism.first, 0);
				prism.second -= cargo_size;
				break;
			}
		}
	}

	void ArmyGroup::DodgeShots()
	{
		for (const auto &Funit : mediator->GetUnits(IsFriendlyUnit(STALKER)))
		{
			int danger = mediator->agent->IncomingDamage(Funit);
			if (danger)
			{
				bool blink_ready = mediator->IsStalkerBlinkOffCooldown(Funit);
				if (blink_ready && (danger > Funit->shield || danger > (Funit->shield_max / 2)))
				{
					mediator->SetUnitCommand(Funit, A_BLINK, Funit->pos + Point2D(0, 4), 2);
					mediator->SetUnitCommand(Funit, A_ATTACK, Funit->pos - Point2D(0, 4), 0, true);
					//agent->Debug()->DebugTextOut(std::to_string(danger), Funit->pos, Color(0, 255, 0), 20);
				}
				else
				{
					//agent->Debug()->DebugTextOut(std::to_string(danger), Funit->pos, Color(255, 0, 0), 20);//
				}
			}
		}
	}

	void ArmyGroup::FindStalkerPositions(std::map<const Unit*, Point2D>& attacking_unit_positions, std::map<const Unit*, Point2D>& retreating_unit_positions, float unit_size, float unit_dispersion)
	{
		Units enemies = mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
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


		Point2D concave_target = attack_path_line.GetPointFrom(stalker_line_pos, 8, true);

		float max_range = 7;
		if (close_enemies.size() > 0)
		{
			concave_target = Utility::MedianCenter(close_enemies);
			max_range = std::max(Utility::GetMaxGroundRange(close_enemies) + 2, 6.0f);
		}

		Point2D retreat_concave_origin = attack_path_line.GetPointFrom(concave_target, max_range, false);
		if (retreat_concave_origin == Point2D(0, 0))
			retreat_concave_origin = attack_path_line.GetPointFrom(stalker_line_pos, 2 * unit_size + unit_dispersion, false);

		Point2D attack_concave_origin = attack_path_line.GetPointFrom(stalker_line_pos, 2 * unit_size + unit_dispersion, true);



		std::vector<Point2D> attack_concave_positions = FindConcaveFromBack(attack_concave_origin, (2 * attack_concave_origin) - concave_target, (int)stalkers.size(), .625, unit_dispersion);
		std::vector<Point2D> retreat_concave_positions = FindConcave(retreat_concave_origin, (2 * retreat_concave_origin) - concave_target, (int)stalkers.size(), .625, unit_dispersion, 30);

		attacking_unit_positions = AssignUnitsToPositions(stalkers, attack_concave_positions);
		retreating_unit_positions = AssignUnitsToPositions(stalkers, retreat_concave_positions);


		//for (const auto &pos : attack_concave_positions)
		//{
		//	agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .625, Color(255, 0, 0));
		//}
		//for (const auto &pos : retreat_concave_positions)
		//{
		//	agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .625, Color(0, 255, 0));
		//}
	}
	*/
	/*
	void ArmyGroup::OraclesDefendArmy(Units basic_units)
	{
		if (oracles.size() == 0)
			return;
		

		Point2D center = attack_path[0];
		if (basic_units.size() > 0)
		{
			Point2D median_center = Utility::MedianCenter(basic_units);
			center = attack_path_line.GetPointFrom(median_center, 3, false);
		}

		Units enemy_burrowed_units = mediator->GetUnits(IsUnits(BURROWED_UNIT_TYPES));

		bool revelation_cast = false;
		for (const auto& oracle : oracles)
		{
			if (mediator->IsOracleCasting(oracle))
			{
				revelation_cast = true;
				break;
			}
		}
		// revelate when units are burrowing
		if (!revelation_cast)
		{
			const Unit* unit_to_revelate = nullptr;
			for (const auto& unit : enemy_burrowed_units)
			{
				if (Utility::DistanceToClosest(oracles, unit->pos) <= 9)
				{
					if (std::find(unit->buffs.begin(), unit->buffs.end(), BUFF_ID::ORACLEREVELATION) == unit->buffs.end())
					{
						unit_to_revelate = unit;
						break;
					}
				}
			}
			if (unit_to_revelate != nullptr)
			{
				const Unit* highest_over_75 = nullptr;
				const Unit* lowest_over_25 = nullptr;
				for (const auto& oracle : oracles)
				{
					if (oracle->energy > 75)
					{
						if (highest_over_75 == nullptr || highest_over_75->energy < oracle->energy)
							highest_over_75 = oracle;
					}
					else if (oracle->energy > 25)
					{
						if (lowest_over_25 == nullptr || lowest_over_25->energy > oracle->energy)
							lowest_over_25 = oracle;
					}
				}
				if (highest_over_75 != nullptr)
				{
					mediator->SetUnitCommand(highest_over_75, A_REVELATION, unit_to_revelate->pos, 10);
					//agent->Debug()->DebugSphereOut(highest_over_75->pos, 2, Color(255, 0, 0));

				}
				else if (lowest_over_25 != nullptr)
				{
					mediator->SetUnitCommand(lowest_over_25, A_REVELATION, unit_to_revelate->pos, 10);
					//agent->Debug()->DebugSphereOut(lowest_over_25->pos, 2, Color(255, 0, 0));
				}
			}
		}


		Units enemy_lings = mediator->GetUnits(IsUnit(ZERGLING));
		int num_close_lings = 0;
		for (const auto& ling : enemy_lings)
		{
			if (Utility::DistanceToClosest(basic_units, ling->pos) < 4)
				num_close_lings++;
		}
		if (num_close_lings > 4)
		{
			int num_stalkers_with_blink = 0;
			for (const auto& stalker : stalkers)
			{
				if (mediator->IsStalkerBlinkOffCooldown(stalker))
					num_stalkers_with_blink++;
			}
			float percent_stalkers_with_blink = 1;
			if (stalkers.size() > 0)
				percent_stalkers_with_blink = static_cast<float>(num_stalkers_with_blink) / static_cast<float>(stalkers.size());

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
			for (const auto& oracle : oracles)
			{
				if (mediator->IsOracleBeamActive(oracle))
					num_oracles_active++;
			}
			
			//agent->Debug()->DebugTextOut(std::to_string(num_close_lings), Point2D(.2, .18), Color(0, 255, 0), 20);
			//agent->Debug()->DebugTextOut(std::to_string(num_oracles_active), Point2D(.2, .2), Color(0, 255, 255), 20);
			//agent->Debug()->DebugTextOut(std::to_string(num_oracles_needed), Point2D(.2, .22), Color(0, 255, 255), 20);
			//agent->Debug()->DebugTextOut(std::to_string(percent_stalkers_with_blink), Point2D(.2, .24), Color(0, 255, 255), 20);
			//agent->Debug()->DebugTextOut(std::to_string(num_stalkers_with_blink), Point2D(.2, .26), Color(0, 255, 255), 20);
			//agent->Debug()->DebugTextOut(std::to_string(state_machine->attached_army_group->blink_ready.size()), Point2D(.2, .28), Color(0, 255, 255), 20);

			//for (int i = 0; i < state_machine->oracles.size(); i++)
			//{
			//	if (state_machine->is_beam_active[i])
			//		agent->Debug()->DebugTextOut(std::to_string(state_machine->oracles[i]->energy), Point2D(.2, .3 + .02 * i), Color(0, 255, 0), 20);
			//	else
			//		agent->Debug()->DebugTextOut(std::to_string(state_machine->oracles[i]->energy), Point2D(.2, .3 + .02 * i), Color(255, 0, 255), 20);

			//}

			if (num_oracles_active > num_oracles_needed) // deactivate oracles
			{
				Units oracles1 = Units(oracles);
				std::sort(oracles1.begin(), oracles1.end(), [](const Unit*& a, const Unit*& b) -> bool
				{
					return a->energy > b->energy;
				});
				for (const auto& oracle : oracles1)
				{
					if (num_oracles_active == num_oracles_needed)
						break;
					if (oracle->energy > 10 && Utility::DistanceToClosest(enemy_lings, oracle->pos) > 5)
					{
						if (mediator->IsOracleBeamActive(oracle))
						{
							mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, 0);
							num_oracles_active--;
						}
					}
				}
			}
			else if (num_oracles_active < num_oracles_needed) // activate more oracles
			{
				Units oracles2 = Units(oracles);
				std::sort(oracles2.begin(), oracles2.end(), [](const Unit*& a, const Unit*& b) -> bool
				{
					return a->energy < b->energy;
				});
				for (const auto& oracle : oracles2)
				{
					if (num_oracles_active == num_oracles_needed)
						break;
					if (oracle->energy > 40)
					{
						if (mediator->IsOracleBeamActive(oracle) == false)
						{
							mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_ON, 0);
							num_oracles_active++;
						}
					}
				}
			}
		}
		else
		{
			Units oracles2 = Units(oracles);
			std::sort(oracles2.begin(), oracles2.end(), [](const Unit*& a, const Unit*& b) -> bool
			{
				return a->energy > b->energy;
			});
			for (const auto& oracle : oracles2)
			{
				if (oracle->energy > 10 && (enemy_lings.size() == 0 || Utility::DistanceToClosest(enemy_lings, oracle->pos)))
				{
					if (mediator->IsOracleBeamActive(oracle))
					{
						mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, 0);
					}
				}
			}
		}
		// add oracle to volley or agnore units targetted in volley?
		// add event listeners for oracle
		for (const auto& oracle : oracles)
		{
			if (mediator->IsOracleCasting(oracle))
			{
				continue;
			}
			if (mediator->IsOracleBeamActive(oracle) == false)
			{
				mediator->SetUnitCommand(oracle, A_MOVE, center, 0);
				continue;
			}
			float now = mediator->GetGameLoop() / FRAME_TIME;
			bool weapon_ready = now - time_last_attacked[oracle] > .8; //.61

			//agent->Debug()->DebugTextOut("weapon ready " + std::to_string(weapon_ready), Point2D(.2, .35), Color(0, 255, 0), 20);
			//agent->Debug()->DebugTextOut("has attacked " + std::to_string(state_machine->has_attacked[oracle]), Point2D(.2, .37), Color(0, 255, 0), 20);
			//agent->Debug()->DebugTextOut("target " + std::to_string(state_machine->target[oracle]), Point2D(.2, .39), Color(0, 255, 0), 20);


			if (weapon_ready)
			{
				const Unit* closest_unit = Utility::ClosestTo(enemy_lings, oracle->pos);
				if (closest_unit == nullptr || Distance2D(closest_unit->pos, oracle->pos) > 6)
				{
					mediator->SetUnitCommand(oracle, A_MOVE, center, 0);
					continue;
				}


				mediator->SetUnitCommand(oracle, A_ATTACK, closest_unit, 0);
				//agent->Debug()->DebugSphereOut(closest_unit->pos, .75, Color(0, 255, 255));

				target[oracle] = closest_unit->tag;
				time_last_attacked[oracle] = mediator->GetGameLoop() / FRAME_TIME;
				has_attacked[oracle] = false;
				//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
			}
			else if (has_attacked[oracle])
			{
				mediator->SetUnitCommand(oracle, A_MOVE, center, 0);

				//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 0, 255));
			}
			else
			{
				//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
			}
		}
	}

	bool ArmyGroup::FindUnitPositions(Units units, Units prisms, float dispersion, float max_range, Point2D limit)
	{
		float unit_size = .625; // TODO figure this out by the larget unit in current_units?
		// first concave origin can just be the closest points on the line
		if (concave_origin == Point2D(0, 0))
		{
			if (units.size() == 0)
				return false;
			Point2D army_center = Utility::MedianCenter(units);
			concave_origin = attack_path_line.FindClosestPoint(army_center);
		}

		// find target range
		Units enemies_in_range = Utility::GetUnitsWithin(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), concave_origin, 15);
		float enemy_dps = 0;
		for (const auto& unit : enemies_in_range)
		{
			enemy_dps += Utility::GetDPS(unit);
		}

		float friendly_dps = 0;
		for (const auto& unit : units)
		{
			friendly_dps += Utility::GetDPS(unit);
		}

		float target_range = max_range;

		if (friendly_dps > enemy_dps * 2)
			target_range = 2;
		else if (friendly_dps > enemy_dps)
			target_range = 4;

		target_range += unit_size;

		// find concave target
		Units close_enemies = Utility::NClosestUnits(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), concave_origin, 5);

		float avg_distance = 0;
		for (int i = 0; i < close_enemies.size(); i++)
		{
			if (Distance2D(concave_origin, close_enemies[i]->pos) > 12)
			{
				close_enemies.erase(close_enemies.begin() + i);
				i--;
			}
			else
			{
				avg_distance += Distance2D(concave_origin, close_enemies[i]->pos);
			}
		}

		bool reached_end = false;
		Point2D concave_target;
		if (close_enemies.size() > 0)
		{
			concave_target = Utility::MedianCenter(close_enemies);
			Point2D attack_line_end = attack_path_line.GetEndPoint();
			if (Distance2D(concave_target, attack_line_end) < 1 || Distance2D(concave_origin, attack_line_end) < 1)
				reached_end = true;
			avg_distance /= close_enemies.size();
		}
		else
		{
			concave_target = attack_path_line.GetPointFrom(concave_origin, 8, true); // default target is 8 units in front of army center
			Point2D attack_line_end = attack_path_line.GetEndPoint();
			if (Distance2D(concave_target, attack_line_end) < 1 || Distance2D(concave_origin, attack_line_end) < 1)
				reached_end = true;
			avg_distance = 5;
		}

		Point2D new_origin;

		// too close
		if (Distance2D(concave_origin, concave_target) < target_range - .1 || 
			(close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, concave_origin) < 2 + unit_size) ||
			concave_origin == attack_path_line.GetFurthestForward({ concave_origin, limit }))
		{

			// if were already retreating and we have the correct number of points then check if we need a new concave
			if (!advancing && unit_position_asignments.size() == units.size() + prisms.size())
			{
				float total_dist = 0;
				for (const auto& pos : unit_position_asignments)
				{
					// maybe ignore very far off units
					total_dist += Distance2D(pos.first->pos, pos.second);
				}
				if (total_dist / units.size() > 1)
				{
					// units are far away on average so dont make new concave
					return reached_end;
				}
			}
			// need a new concave
			float dist_to_move_origin = std::min(1.0f, abs(target_range - Distance2D(concave_origin, concave_target)));

			if (close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, concave_origin) < 2)
				dist_to_move_origin = std::max(dist_to_move_origin, 2 + unit_size - Utility::DistanceToClosest(close_enemies, concave_origin));

			new_origin = attack_path_line.GetPointFrom(concave_origin, dist_to_move_origin, false);
			// if new origin is closer then ignore it
			//if (Distance2D(concave_origin, concave_target) > Distance2D(new_origin, concave_target))
			//	return reached_end;

		}// too far away
		else if (Distance2D(concave_origin, concave_target) > target_range + .1 && (close_enemies.size() == 0 || Utility::DistanceToClosest(close_enemies, concave_origin) > 2 + unit_size))
		{
			// if were already advancing and we have the correct number of points then check if we need a new concave
			if (advancing && unit_position_asignments.size() == units.size() + prisms.size())
			{
				float total_dist = 0;
				for (const auto& pos : unit_position_asignments)
				{
					// maybe ignore very far off units
					total_dist += Distance2D(pos.first->pos, pos.second);
				}
				if (total_dist / units.size() > 1)
				{
					// units are far away on average so dont make new concave
					return reached_end;
				}
			}
			// need a new concave
			float dist_to_move_origin = std::min(1.0f, abs(Distance2D(concave_origin, concave_target) - target_range));
			new_origin = attack_path_line.GetPointFrom(concave_origin, dist_to_move_origin, true);
			new_origin = attack_path_line.GetFurthestBack({ new_origin, limit });
			// if new origin is further away then ignore it
			//if (Distance2D(concave_origin, concave_target) < Distance2D(new_origin, concave_target))
			//	return reached_end;

		}
		else // perfect range
		{
			return reached_end;
		}

		if (new_origin == Point2D(0, 0))
			return reached_end;

		concave_origin = new_origin;
		float concave_degree = (5 * avg_distance) + 4;

		std::vector<Point2D> prism_positions;
		std::vector<Point2D> concave_positions = FindConcaveWithPrism(concave_origin, (2 * concave_origin) - concave_target, (int)units.size(), (int)prisms.size(), unit_size, dispersion, concave_degree, prism_positions);

		unit_position_asignments = AssignUnitsToPositions(units, concave_positions);
		if (prisms.size() > 0)
		{
			std::map<const Unit*, Point2D> prism_position_assignments = AssignUnitsToPositions(prisms, prism_positions);
			for (const auto& assignment : prism_position_assignments)
			{
				unit_position_asignments[assignment.first] = assignment.second;
			}
		}
		return reached_end;
	}

	void ArmyGroup::FindReadyUnits(Units units, Units& units_ready, Units& units_not_ready)
	{

		for (const auto& unit : units)
		{
			if (unit->weapon_cooldown == 0 && mediator->GetAttackStatus(unit) == false)
			{
				// ignore units inside prisms
				if (warp_prisms.size() > 0)
				{
					bool in_prism = false;
					for (const auto& prism : warp_prisms)
					{
						for (const auto& passanger : prism->passengers)
						{
							if (passanger.tag == unit->tag)
							{
								in_prism = true;
								break;
							}
						}
						if (in_prism)
							break;
					}
					if (in_prism)
						units_not_ready.push_back(unit);
				}
				units_ready.push_back(unit);
			}
			else
			{
				units_not_ready.push_back(unit);
			}
		}
	}

	std::vector<std::pair<const Unit*, UnitDanger>> ArmyGroup::MicroNonReadyUnits(Units units)
	{
		std::vector<std::pair<const Unit*, UnitDanger>> incoming_damage;
		for (const auto& unit : units)
		{
			if (mediator->GetAttackStatus(unit) == false)
			{
				float damage = (float)mediator->agent->IncomingDamage(unit);
				//agent->Debug()->DebugTextOut(std::to_string(damage), unit->pos, Color(255, 255, 255), 16);
				if (damage > 0)
				{
					float shield_damage = std::min(damage, unit->shield);
					float health_damage = damage - shield_damage;
					float total_damage = shield_damage + ((health_damage / unit->health) * unit->health_max * 1.5f);
					int prio = 3;
					if (health_damage >= unit->health)
						prio = 1;
					else if (total_damage > 40)
						prio = 2;
					incoming_damage.push_back(std::pair<const Unit*, UnitDanger>(unit, UnitDanger(prio, total_damage)));
				}
				//else if (using_standby)
				//{
				//	if ((unit->shield + unit->health) / (unit->shield_max + unit->health_max) < .3) // TODO this threshold should be passed in
				//	{
				//		incoming_damage.push_back(std::pair<const Unit*, UnitDanger>(unit, UnitDanger(0, 4)));
				//	}
				//}

				// no order but no danger so just move forward
				if (unit_position_asignments.find(unit) != unit_position_asignments.end())
					mediator->SetUnitCommand(unit, A_MOVE, unit_position_asignments[unit], 0);
				else
					mediator->SetUnitCommand(unit, A_MOVE, unit->pos, 0);


				if (using_standby)
				{
					if (unit->shield == 0 || (unit->shield + unit->health) / (unit->shield_max + unit->health_max) < .3) // TODO this threshold should be passed in
					{
						standby_units.push_back(unit);

						if (standby_pos == Point2D(0, 0))
							mediator->SetUnitCommand(unit, A_BLINK,
								Utility::PointBetween(unit->pos, Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), unit->pos)->pos, -4), 2);
						else
							mediator->SetUnitCommand(unit, A_BLINK, standby_pos, 2); // TODO adjustable blink distance

						mediator->SetUnitCommand(unit, A_MOVE, standby_pos, 0, true);
					}
				}
			}
		}
		return incoming_damage;
	}

	void ArmyGroup::MicroWarpPrisms(std::vector<std::pair<const Unit*, UnitDanger>> units_requesting_pickup)
	{
		for (const auto& prism : warp_prisms)
		{

			if (unit_position_asignments.find(prism) != unit_position_asignments.end())
				mediator->SetUnitCommand(prism, A_MOVE, unit_position_asignments[prism], 0);
			else
				mediator->SetUnitCommand(prism, A_MOVE, prism->pos, 0);
			mediator->SetUnitCommand(prism, A_UNLOAD_AT, prism, 0);
		}

		for (auto cargo = units_in_cargo.begin(); cargo != units_in_cargo.end();)
		{
			if (cargo->second.confirmed == false)
			{
				bool unit_found = false;
				for (const auto& prism : warp_prisms)
				{
					for (const auto& passanger : prism->passengers)
					{
						if (cargo->first->tag == passanger.tag)
						{
							cargo->second.confirmed = true;
							unit_found = true;
							break;
						}
					}
					if (unit_found)
						break;
				}
				cargo++;
			}
			else
			{
				bool unit_found = false;
				for (const auto& prism : warp_prisms)
				{
					for (const auto& passanger : prism->passengers)
					{
						if (cargo->first->tag == passanger.tag)
						{
							unit_found = true;
							break;
						}
					}
					if (unit_found)
						break;
				}
				if (!unit_found)
				{
					prism_cargo[cargo->second.prism] -= Utility::GetCargoSize(cargo->first);
					units_in_cargo.erase(cargo++);
				}
				else
				{
					cargo++;
				}
			}
		}

		// order units by priority
		// TODO check if this should be swapped
		std::sort(units_requesting_pickup.begin(), units_requesting_pickup.end(),
			[](const std::pair<const Unit*, UnitDanger> a, const std::pair<const Unit*, UnitDanger> b) -> bool
		{
			return a.second < b.second;
		});

		for (const auto& request : units_requesting_pickup)
		{
			bool unit_found_space = false;
			if (units_in_cargo.find(request.first) != units_in_cargo.end())
			{
				// unit is already being picked up but just in case give the order again
				mediator->SetUnitCommand(request.first, A_SMART, units_in_cargo[request.first].prism, 0);
				continue;
			}
			for (const auto& prism : warp_prisms)
			{
				int size = Utility::GetCargoSize(request.first);
				int necessary_space = std::max(size, request.second.unit_prio * 2);
				if (8 - (prism_cargo[prism]) >= necessary_space)
				{
					unit_found_space = true;
					mediator->SetUnitCommand(request.first, A_SMART, prism, 0);
					units_in_cargo[request.first] = PrismCargo(prism);
					prism_cargo[prism] += size;
					break;
				}
			}
			if (unit_found_space == false)
			{
				if (request.first->unit_type == STALKER && request.second.unit_prio >= 2)
				{
					bool blink_off_cooldown = mediator->IsStalkerBlinkOffCooldown(request.first);

					if (mediator->upgrade_manager.has_blink && blink_off_cooldown)
					{
						if (standby_pos == Point2D(0, 0))
							mediator->SetUnitCommand(request.first, A_BLINK,
								Utility::PointBetween(request.first->pos, Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), request.first->pos)->pos, -4), 2);
						else
							mediator->SetUnitCommand(request.first, A_BLINK, standby_pos, 2); // TODO adjustable blink distance

						mediator->CancelAttack(request.first);
					}
				}
				break;
			}
		}
	}

	Point2D ArmyGroup::FindLimitToAdvance()
	{
		std::vector<Point2D> danger_points;
		for (const auto& unit : mediator->GetEnemySavedPositions())
		{
			if (unit.first->unit_type != SIEGE_TANK_SIEGED)
				continue;
			Point2D pos = unit.second.pos;
			std::vector<Point2D> intersection_points = attack_path_line.FindCircleIntersection(pos, 14);
			Point2D danger_point = attack_path_line.GetFurthestBack(intersection_points);
			danger_points.push_back(danger_point);
		}
		Point2D furthest_back = attack_path_line.GetFurthestBack(danger_points);
		danger_points.erase(std::remove(danger_points.begin(), danger_points.end(), furthest_back), danger_points.end());
		return attack_path_line.GetFurthestBack(danger_points);
	}
	*/

#pragma warning(push)
#pragma warning(disable : 4100)
	void ArmyGroup::OnUnitDamagedListener(const Unit* unit, float health_damage, float shields_damage)
	{
		
	}
#pragma warning(pop)

	void ArmyGroup::OnUnitDestroyedListener(const Unit* unit)
	{

		if (std::find(all_units.begin(), all_units.end(), unit) != all_units.end())
			RemoveUnit(unit);
		if (std::find(new_units.begin(), new_units.end(), unit) != new_units.end())
			RemoveUnit(unit);
	}

}