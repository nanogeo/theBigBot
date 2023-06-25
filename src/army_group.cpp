#pragma once
#include <iostream>
#include <string>

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"

#include "army_group.h"
#include "TossBot.h"

namespace sc2 {

	void ArmyGroup::AddUnit(Unit* unit)
	{
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
			Point2D furthest_back = agent->ClosestPointOnLine(concave_points.back(), origin, fallback_point);
			if (Distance2D(origin, fallback_point) < Distance2D(furthest_back, fallback_point))
				return concave_points;
			current_origin = agent->PointBetween(current_origin, fallback_point, -(unit_size + dispersion));
		}

	}

	bool TossBot::TestSwap(Point2D pos1, Point2D target1, Point2D pos2, Point2D target2)
	{
		float curr_max = std::max(Distance2D(pos1, target1), Distance2D(pos2, target2));
		float swap_max = std::max(Distance2D(pos1, target2), Distance2D(pos2, target1));
		return swap_max < curr_max;
	}

	std::map<const Unit*, Point2D> TossBot::AssignUnitsToPositions(Units units, std::vector<Point2D> positions)
	{
		std::map<const Unit*, Point2D> unit_assignments;
		for (const auto &unit : units)
		{
			Point2D closest = ClosestTo(positions, unit->pos);
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

	void MicroUnits()
	{

	}


}