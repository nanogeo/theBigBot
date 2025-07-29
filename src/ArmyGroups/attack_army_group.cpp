
#include "attack_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "definitions.h"

#include "theBigBot.h"

namespace sc2 {


float AttackArmyGroup::CalculateDesiredRange(Units units)
{
	// TODO pass in this Units
	Units enemies_in_range = Utility::GetUnitsWithin(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), concave_origin, LONG_RANGE);
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

	float desired_range = default_range;

	if (friendly_dps > enemy_dps * 2)
		desired_range = 2;
	else if (friendly_dps > enemy_dps)
		desired_range = default_range - 2;

	desired_range += unit_size;

	return desired_range;
}

Point2D AttackArmyGroup::CalculateConcaveTarget()
{
	//Units close_enemies = Utility::NClosestUnits(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), concave_origin, 5);
	Units close_enemies = Utility::NClosestUnits(mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy)), concave_origin, 5);
	// TODO use some clustering algorithm to better find the grouping of close enemies to face toward

	for (int i = 0; i < close_enemies.size(); i++)
	{
		if (Distance2D(concave_origin, close_enemies[i]->pos) > LONG_RANGE)
		{
			close_enemies.erase(close_enemies.begin() + i);
			i--;
		}
	}

	if (close_enemies.size() > 0)
	{
		return Utility::MedianCenter(close_enemies);
	}
	else
	{
		return attack_path.GetPointFrom(concave_origin, 8, true); // default target is 8 units in front of army center
	}
}

Point2D AttackArmyGroup::CalculateNewConcaveOrigin(Units close_enemies, float desired_range, Point2D limit)
{
	Point2D new_origin;

	// too close
	if (Distance2D(concave_origin, concave_target) < desired_range - .1f ||
		(close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, concave_origin) < 2 + unit_size) ||
		concave_origin == attack_path.GetFurthestForward({ concave_origin, limit }))
	{
		// if were already retreating and we have the correct number of points then check if we need a new concave
		if (!advancing && unit_position_asignments.size() == basic_units.size() + warp_prisms.size())
		{
			float total_dist = 0;
			float furthest_dist = 0;
			for (const auto& pos : unit_position_asignments)
			{
				// maybe ignore very far off units
				float dist = Distance2D(pos.first->pos, pos.second);
				total_dist += dist;
				if (dist > furthest_dist)
					furthest_dist = dist;
			}
			// ignore the furthest unit
			total_dist -= furthest_dist;
			if (total_dist / basic_units.size() > 1)
			{
				// units are far away on average so dont make new concave
				return concave_origin;
			}
		}
		// need a new concave
		float dist_to_move_origin = std::min(1.0f, abs(desired_range - Distance2D(concave_origin, concave_target)));

		if (close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, concave_origin) < 2)
			dist_to_move_origin = std::max(dist_to_move_origin, 2 + unit_size - Utility::DistanceToClosest(close_enemies, concave_origin));

		new_origin = attack_path.GetPointFrom(concave_origin, dist_to_move_origin, false);

		return new_origin;

	}// too far away
	else if (Distance2D(concave_origin, concave_target) > desired_range + .1 && (close_enemies.size() == 0 || Utility::DistanceToClosest(close_enemies, concave_origin) > 2 + unit_size))
	{
		// if were already advancing and we have the correct number of points then check if we need a new concave
		if (advancing && unit_position_asignments.size() == basic_units.size() + warp_prisms.size())
		{
			float total_dist = 0;
			for (const auto& pos : unit_position_asignments)
			{
				// maybe ignore very far off units
				total_dist += Distance2D(pos.first->pos, pos.second);
			}
			if (total_dist / basic_units.size() > 1)
			{
				// units are far away on average so dont make new concave
				return concave_origin;
			}
		}
		// need a new concave
		float dist_to_move_origin = std::min(1.0f, abs(Distance2D(concave_origin, concave_target) - desired_range));
		new_origin = attack_path.GetPointFrom(concave_origin, dist_to_move_origin, true);
		new_origin = attack_path.GetFurthestBack({ new_origin, limit });

		return new_origin;
	}
	else // perfect range
	{
		return concave_origin;
	}
}

Point2D AttackArmyGroup::FindLimitToAdvance(std::vector<UNIT_TYPEID> types_to_avoid, float extra_range, bool air,
	int num_units_to_ignore)
{
	// TODO ignore units behind army but intersecting with attack path
	std::vector<Point2D> danger_points;
	for (const auto& unit : mediator->GetEnemySavedPositions())
	{
		if (std::find(types_to_avoid.begin(), types_to_avoid.end(), unit.first->unit_type) == types_to_avoid.end())
			continue;
		Point2D pos = unit.second.pos;
		std::vector<Point2D> intersection_points;

		if (air && Utility::GetAirRange(unit.first) > 0)
			intersection_points = attack_path.FindCircleIntersection(pos, Utility::GetAirRange(unit.first) + extra_range);
		else if (Utility::GetGroundRange(unit.first) > 0)
			intersection_points = attack_path.FindCircleIntersection(pos, Utility::GetGroundRange(unit.first) + extra_range);

		if (intersection_points.size() == 0)
			continue;
		Point2D danger_point = attack_path.GetFurthestBack(intersection_points);
		danger_points.push_back(danger_point);
	}
	if (danger_points.size() <= num_units_to_ignore)
		return attack_path.GetEndPoint();

	return danger_points[num_units_to_ignore];
}

void AttackArmyGroup::FindNewConcaveOrigin()
{
	// first concave origin can just be the closest points on the line
	if (concave_origin == Point2D(0, 0))
	{
		concave_origin = attack_path.FindClosestPoint(Utility::MedianCenter(basic_units));
	}

	float desired_range = CalculateDesiredRange(basic_units);

	// find concave target
	concave_target = CalculateConcaveTarget();

	// find limit to advance
	Point2D limit = attack_path.GetEndPoint();
	if (limit_advance)
	{
		limit = FindLimitToAdvance({ SIEGE_TANK_SIEGED }, 1, false, 1);
	}
	mediator->DebugSphere(mediator->ToPoint3D(limit), 1.5, Color(255, 255, 255));

	// find close enemies and average distance to them
	Units close_enemies = Utility::NClosestUnits(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), concave_origin, 5);
	float avg_distance = 0;
	for (int i = 0; i < close_enemies.size(); i++)
	{
		if (Distance2D(concave_origin, close_enemies[i]->pos) > LONG_RANGE)
		{
			close_enemies.erase(close_enemies.begin() + i);
			i--;
		}
		else
		{
			avg_distance += Distance2D(concave_origin, close_enemies[i]->pos);
		}
	}

	if (close_enemies.size() > 0)
	{
		avg_distance /= close_enemies.size();
	}
	else
	{
		avg_distance = 5;
	}

	// find new concave origin
	concave_origin = CalculateNewConcaveOrigin(close_enemies, desired_range, limit);
}

std::vector<Point2D> AttackArmyGroup::FindConcaveWithPrism(std::vector<Point2D>& prism_positions, int num_units, int num_prisms)
{
	float min_height = mediator->ToPoint3D(concave_origin).z - .5f;
	float max_height = min_height + 1;
	float range = 0; //r
	float unit_radius = unit_size + dispersion; //u
	//float concave_degree = 30; //p
	int max_width = 4;


	Point2D backward_vector = ((2 * concave_origin) - concave_target) - concave_origin;
	Point2D forward_vector = concave_origin - ((2 * concave_origin) - concave_target);
	forward_vector /= sqrt(forward_vector.x * forward_vector.x + forward_vector.y * forward_vector.y);

	Point2D offset_circle_center = Point2D(concave_origin.x + concave_degree * forward_vector.x, concave_origin.y + concave_degree * forward_vector.y);

	float backwards_direction = atan2(backward_vector.y, backward_vector.x);

	std::vector<Point2D> concave_points;

	int row = 0;

	while (concave_points.size() < num_units)
	{
		if (row > 10)
			return concave_points;
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
				float point_height = mediator->ToPoint3D(unit_position).z;
				if (mediator->IsPathable(unit_position) && point_height > min_height && point_height < max_height)
				{
					if (point_height + .5f > max_height)
						max_height = point_height + .5f;
					if (point_height - .5f < min_height)
						min_height = point_height - .5f;
					if (Utility::DistanceToClosest(mediator->GetCorrosiveBilePositions(), unit_position) > .5f + unit_size + .1f)
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
				float point_height = mediator->ToPoint3D(unit_position).z;
				if (mediator->IsPathable(unit_position) && point_height > min_height && point_height < max_height)
				{
					if (point_height + .5f > max_height)
						max_height = point_height + .5f;
					if (point_height - .5f < min_height)
						min_height = point_height - .5f;
					if (Utility::DistanceToClosest(mediator->GetCorrosiveBilePositions(), unit_position) > .5f + unit_size + .1f)
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
		{
			if (num_prisms == 0)
				break;
			// find prism positions
			row++;
			float offset = .5;
			if (num_prisms % 2 == 1)
			{
				// middle point
				float unit_direction = backwards_direction;
				Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
					offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
				if (mediator->IsPathable(unit_position))
				{
					prism_positions.push_back(unit_position);
				}
				offset = 1;
			}

			arclength = (2 * unit_radius * ((6 / num_prisms) - .5f)) / (range + concave_degree + (((row * 2) - 1) * unit_radius));
			for (float i = offset; i <= std::ceil((num_prisms / 2) - offset); i += 1)
			{
				// right position
				{
					float unit_direction = backwards_direction + i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));

					prism_positions.push_back(unit_position);
				}

				// left position
				{
					float unit_direction = backwards_direction - i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));

					prism_positions.push_back(unit_position);
				}
			}
			break;
		}

		// odd row
		row++;
		// middle point
		float unit_direction = backwards_direction;
		Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
			offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
		float point_height = mediator->ToPoint3D(unit_position).z;
		if (mediator->IsPathable(unit_position) && point_height > min_height && point_height < max_height)
		{
			if (point_height + .5f > max_height)
				max_height = point_height + .5f;
			if (point_height - .5f < min_height)
				min_height = point_height - .5f;
			if (Utility::DistanceToClosest(mediator->GetCorrosiveBilePositions(), unit_position) > .5f + unit_size + .1f)
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
				point_height = mediator->ToPoint3D(unit_position).z;
				if (mediator->IsPathable(unit_position) && point_height > min_height && point_height < max_height)
				{
					if (point_height + .5f > max_height)
						max_height = point_height + .5f;
					if (point_height - .5f < min_height)
						min_height = point_height - .5f;
					if (Utility::DistanceToClosest(mediator->GetCorrosiveBilePositions(), unit_position) > .5f + unit_size + .1f)
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
				point_height = mediator->ToPoint3D(unit_position).z;
				if (mediator->IsPathable(unit_position) && point_height > min_height && point_height < max_height)
				{
					if (point_height + .5f > max_height)
						max_height = point_height + .5f;
					if (point_height - .5f < min_height)
						min_height = point_height - .5f;
					if (Utility::DistanceToClosest(mediator->GetCorrosiveBilePositions(), unit_position) > .5f + unit_size + .1f)
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
		{
			if (num_prisms == 0)
				break;
			// find prism positions
			row++;
			float offset = .5;
			if (num_prisms % 2 == 1)
			{
				// middle point
				unit_direction = backwards_direction;
				unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
					offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));
				if (mediator->IsPathable(unit_position))
				{
					prism_positions.push_back(unit_position);
				}
				offset = 1;
			}

			arclength = (2 * unit_radius * ((6 / num_prisms) - .5f)) / (range + concave_degree + (((row * 2) - 1) * unit_radius));
			for (float i = offset; i <= std::ceil((num_prisms / 2) - offset); i += 1)
			{
				// right position
				{
					unit_direction = backwards_direction + i * arclength;
					unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));

					prism_positions.push_back(unit_position);
				}

				// left position
				{
					unit_direction = backwards_direction - i * arclength;
					unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_radius)) * sin(unit_direction));

					prism_positions.push_back(unit_position);
				}
			}
			break;
		}
	}
	return concave_points;
}

std::vector<std::pair<const Unit*, UnitDanger>> AttackArmyGroup::CalculateUnitDanger()
{
	std::vector<std::pair<const Unit*, UnitDanger>> incoming_damage;
	for (const auto& unit : basic_units)
	{
		float damage = (float)mediator->GetIncomingDamage(unit);
		//agent->Debug()->DebugTextOut(std::to_string(damage), unit->pos, Color(255, 255, 255), 16);
		if (damage > 0)
		{
			float shield_damage = std::min(damage, unit->shield);
			float health_damage = damage - shield_damage;
			float total_damage = shield_damage + ((health_damage / unit->health) * unit->health_max * 1.5f);
			int prio = 3;
			if (health_damage >= unit->health)
				prio = 1;
			else if (total_damage > 50 || health_damage > 10)
				prio = 2;
			incoming_damage.push_back(std::pair<const Unit*, UnitDanger>(unit, UnitDanger(prio, total_damage)));
		}
	}
	std::sort(incoming_damage.begin(), incoming_damage.end(),
		[](const std::pair<const Unit*, UnitDanger> a, const std::pair<const Unit*, UnitDanger> b) -> bool
		{
			return a.second < b.second;
		});
	return incoming_damage;
}

std::map<const Unit*, Point2D> AttackArmyGroup::AssignUnitsToPositions(Units units, std::vector<Point2D> positions)
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
	for (const auto& unit : units)
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

void AttackArmyGroup::OraclesDefendArmy()
{
	if (oracles.size() == 0)
		return;


	Point2D center = attack_path.GetStartPoint();
	if (basic_units.size() > 0)
	{
		Point2D median_center = Utility::MedianCenter(basic_units);
		center = attack_path.GetPointFrom(median_center, 3, false);
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
				mediator->SetUnitCommand(highest_over_75, A_REVELATION, unit_to_revelate->pos, CommandPriorty::low);
				//agent->Debug()->DebugSphereOut(highest_over_75->pos, 2, Color(255, 0, 0));

			}
			else if (lowest_over_25 != nullptr)
			{
				mediator->SetUnitCommand(lowest_over_25, A_REVELATION, unit_to_revelate->pos, CommandPriorty::low);
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
		for (const auto& unit : basic_units)
		{
			if (unit->unit_type == STALKER && mediator->IsStalkerBlinkOffCooldown(unit))
				num_stalkers_with_blink++;
		}
		float percent_stalkers_with_blink = 1;
		if (basic_units.size() > 0)
			percent_stalkers_with_blink = static_cast<float>(num_stalkers_with_blink) / static_cast<float>(basic_units.size());

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
						mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, CommandPriorty::low);
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
						mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_ON, CommandPriorty::low);
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
					mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, CommandPriorty::low);
				}
			}
		}
	}
	// add oracle to volley or ignore units targetted in volley?
	// add event listeners for oracle
	for (const auto& oracle : oracles)
	{
		if (mediator->IsOracleCasting(oracle))
		{
			continue;
		}
		if (mediator->IsOracleBeamActive(oracle) == false)
		{
			mediator->SetUnitCommand(oracle, A_MOVE, center, CommandPriorty::low);
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
				mediator->SetUnitCommand(oracle, A_MOVE, center, CommandPriorty::low);
				continue;
			}


			mediator->SetUnitCommand(oracle, A_ATTACK, closest_unit, CommandPriorty::low);
			//agent->Debug()->DebugSphereOut(closest_unit->pos, .75, Color(0, 255, 255));

			target[oracle] = closest_unit->tag;
			time_last_attacked[oracle] = mediator->GetGameLoop() / FRAME_TIME;
			has_attacked[oracle] = false;
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
		}
		else if (has_attacked[oracle])
		{
			mediator->SetUnitCommand(oracle, A_MOVE, center, CommandPriorty::low);

			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 0, 255));
		}
		else
		{
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
		}
	}
}

bool AttackArmyGroup::TestSwap(Point2D pos1, Point2D target1, Point2D pos2, Point2D target2) const
{
	float curr_max = std::max(Distance2D(pos1, target1), Distance2D(pos2, target2));
	float swap_max = std::max(Distance2D(pos1, target2), Distance2D(pos2, target1));
	return swap_max < curr_max;
}

void AttackArmyGroup::MicroReadyUnits(Units units, float percent_needed, int total_units)
{
	// if enough units can attack something
	if (static_cast<float>(units.size()) / static_cast<float>(total_units) >= percent_needed)
	{
		// find targets for each unit
		mediator->AddUnitsToAttackers(units);
		for (const auto& unit : units)
		{
			if (unit_position_asignments.find(unit) != unit_position_asignments.end())
				mediator->SetUnitCommand(unit, A_MOVE, unit_position_asignments[unit], CommandPriorty::low);
			else
				mediator->SetUnitCommand(unit, A_MOVE, unit->pos, CommandPriorty::low);
		}
	}
	else
	{
		for (const auto& unit : units)
		{
			if (unit_position_asignments.find(unit) != unit_position_asignments.end())
				mediator->SetUnitCommand(unit, A_MOVE, unit_position_asignments[unit], CommandPriorty::low);
			else
				mediator->SetUnitCommand(unit, A_MOVE, unit->pos, CommandPriorty::low);
		}
	}
}

AttackArmyGroup::AttackArmyGroup(Mediator* mediator, PiecewisePath attack_path, std::vector<UNIT_TYPEID> unit_types, 
	int desired_units, int max_units, int required_units, int min_reinforce_group_size) : ArmyGroup(mediator)
{
	this->unit_types = unit_types;
	this->desired_units = desired_units;
	this->max_units = max_units;
	this->required_units = required_units;
	this->min_reinforce_group_size = min_reinforce_group_size;

	this->attack_path = attack_path;
	dispersion = 0;
	switch (mediator->GetEnemyRace())
	{
	case Race::Protoss:
		target_priority = PROTOSS_PRIO;
		break;
	case Race::Terran:
		target_priority = TERRAN_PRIO;
		break;
	case Race::Zerg:
		target_priority = ZERG_PRIO;
		break;
	default:
		std::cerr << "Unknown enemy race in AttackArmyGroup" << std::endl;
		break;
	}
	limit_advance = false;
}

AttackArmyGroup::AttackArmyGroup(Mediator* mediator, ArmyTemplate<AttackArmyGroup>* army_template) : ArmyGroup(mediator)
{
	unit_types = army_template->unit_types;
	desired_units = army_template->desired_units;
	max_units = army_template->max_units;

	required_units = 0;
	for (const auto& temp : army_template->required_units)
	{
		required_units += temp.second;
	}
	this->min_reinforce_group_size = 5;
	dispersion = 0;
	switch (mediator->GetEnemyRace())
	{
	case Race::Protoss:
		target_priority = PROTOSS_PRIO;
		break;
	case Race::Terran:
		dispersion = .2f;
		target_priority = TERRAN_PRIO;
		break;
	case Race::Zerg:
		target_priority = ZERG_PRIO;
		break;
	default:
		std::cerr << "Unknown enemy race in AttackArmyGroup" << std::endl;
		break;
	}
	limit_advance = false;
	attack_path = mediator->GetDirectAttackLine();
}
	
void AttackArmyGroup::SetUp()
{
	// TODO check for units mid warp in
	if (all_units.size() == 0)
		return;

	mediator->SetUnitsCommand(all_units, A_MOVE, Utility::MedianCenter(all_units), CommandPriorty::low);
	if (Utility::GetUnitsWithin(all_units, Utility::MedianCenter(all_units), 5).size() >= required_units)
	{
		ready = true;
	}
}

void AttackArmyGroup::Run()
{
	GroupUpNewUnits();

	for (auto itr = units_on_their_way.begin(); itr != units_on_their_way.end();)
	{
		if (MobilizeNewUnits(*itr))
		{
			for (const auto& unit : *itr)
			{
				AddUnit(unit);
			}
			itr = units_on_their_way.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	if (AttackLine() == AttackLineResult::all_units_dead)
		mediator->MarkArmyGroupForDeletion(this);
}

void AttackArmyGroup::ScourMap()
{
	for (auto itr = units_on_their_way.begin(); itr != units_on_their_way.end();)
	{
		for (const auto& unit : *itr)
		{
			AddUnit(unit);
		}
		itr = units_on_their_way.erase(itr);
	}
	ArmyGroup::ScourMap();
}

void AttackArmyGroup::AddUnit(const Unit* unit)
{
	if (unit->unit_type == STALKER || unit->unit_type == SENTRY || unit->unit_type == ADEPT ||
		unit->unit_type == ARCHON || unit->unit_type == IMMORTAL)
	{
		if (std::find(basic_units.begin(), basic_units.end(), unit) == basic_units.end())
			basic_units.push_back(unit);
	}
	else if (unit->unit_type == ORACLE)
	{
		if (std::find(oracles.begin(), oracles.end(), unit) == oracles.end())
			oracles.push_back(unit);
	}
	else if (unit->unit_type == PRISM || unit->unit_type == PRISM_SIEGED)
	{
		if (std::find(warp_prisms.begin(), warp_prisms.end(), unit) == warp_prisms.end())
			warp_prisms.push_back(unit);
	}

	ArmyGroup::AddUnit(unit);
}

void AttackArmyGroup::RemoveUnit(const Unit* unit)
{
	// TODO remove units in units_on_their_way;

	basic_units.erase(std::remove(basic_units.begin(), basic_units.end(), unit), basic_units.end());
	oracles.erase(std::remove(oracles.begin(), oracles.end(), unit), oracles.end());
	warp_prisms.erase(std::remove(warp_prisms.begin(), warp_prisms.end(), unit), warp_prisms.end());

	ArmyGroup::RemoveUnit(unit);
}

// returns true when group is close enough to add into main army
bool AttackArmyGroup::MobilizeNewUnits(Units units)
{
	if (Distance2D(Utility::MedianCenter(units), Utility::MedianCenter(all_units)) < 5)
		return true;
	for (const auto& unit : units)
	{
		if (mediator->GetAttackStatus(unit))
			continue;
		if (unit->weapon_cooldown == 0)
			mediator->AddUnitToAttackers(unit);

		mediator->SetUnitCommand(unit, A_MOVE, Utility::MedianCenter(all_units), CommandPriorty::low);
		// TODO make sure units stay grouped up
	}
	return false;
}

void AttackArmyGroup::GroupUpNewUnits()
{
	// TODO check for enemies in range
	mediator->SetUnitsCommand(new_units, A_MOVE, Utility::MedianCenter(new_units), CommandPriorty::low);

	Units group = Utility::GetUnitsWithin(new_units, Utility::MedianCenter(new_units), 5);
	if (group.size() >= min_reinforce_group_size)
	{
		units_on_their_way.push_back(group);
		for (const auto& unit : group)
		{
			new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());
		}
	}
}


AttackLineResult AttackArmyGroup::AttackLine()
{
	if (all_units.size() == 0 && new_units.size() == 0 && basic_units.size() == 0 && standby_units.size() == 0)
	{
		return AttackLineResult::all_units_dead;
	}
	if (basic_units.size() == 0)
	{
		if (units_on_their_way.size() > 0)
		{
			for (const auto& unit : units_on_their_way[0])
			{
				AddUnit(unit);
			}
			units_on_their_way.erase(units_on_their_way.begin());
			concave_origin = Point2D(0, 0);
		}
		else if (new_units.size() > 0)
		{
			for (const auto& unit : new_units)
			{
				AddUnit(unit);
			}
		}
		else
		{
			return AttackLineResult::all_units_dead;
		}
		// TODO add a case for standby units
	}

	// update unit size TODO maybe move to addunit/removeunit
	unit_size = Utility::GetLargestUnitSize(basic_units);

	// move concave origin if necessary
	FindNewConcaveOrigin();
	standby_pos = attack_path.GetPointFrom(concave_origin, 8, false);
	
	// Find positions
	std::vector<Point2D> prism_positions;
	std::vector<Point2D> concave_positions = FindConcaveWithPrism(prism_positions, basic_units.size(), warp_prisms.size());

	// assign units to positions
	unit_position_asignments = AssignUnitsToPositions(basic_units, concave_positions);
	if (warp_prisms.size() > 0)
	{
		std::map<const Unit*, Point2D> prism_position_assignments = AssignUnitsToPositions(warp_prisms, prism_positions);
		for (const auto& assignment : prism_position_assignments)
		{
			unit_position_asignments[assignment.first] = assignment.second;
		}
	}

	// calculate unit danger
	std::vector<std::pair<const Unit*, UnitDanger>> incoming_damage = CalculateUnitDanger();

	Units escaping_units;
	// if blink -> blink stalkers in enough danger
	if (mediator->CheckUpgrade(U_BLINK))
	{
		for (const auto& incoming : incoming_damage)
		{
			if (incoming.first->unit_type == STALKER && incoming.second.unit_prio <= 2 && mediator->IsStalkerBlinkOffCooldown(incoming.first))
			{
				if (mediator->GetAttackStatus(incoming.first))
					mediator->CancelAttack(incoming.first);

				mediator->SetUnitCommand(incoming.first, A_BLINK, standby_pos, CommandPriorty::high);
				escaping_units.push_back(incoming.first);
			}
		}
	}

	std::vector<Tag> units_in_cargo;
	// if prisms -> pickup units in enough danger
	if (warp_prisms.size() > 0)
	{
		for (const auto& prism : warp_prisms)
		{
			for (const auto& passanger : prism->passengers)
			{
				units_in_cargo.push_back(passanger.tag);
			}
		}

	}

	// move units to positions
	for (const auto& assignment : unit_position_asignments)
	{
		if (mediator->GetAttackStatus(assignment.first) == false)
			mediator->SetUnitCommand(assignment.first, A_MOVE, assignment.second, CommandPriorty::low);
	}

	// if units that can attack > threshold -> add units to attackers
	// ignore units entering prisms, in prisms, or blinking/casting
	Units ready_to_attack;
	for (const auto& unit : basic_units)
	{
		if (std::find(escaping_units.begin(), escaping_units.end(), unit) != escaping_units.end())
			continue;
		if (std::find(units_in_cargo.begin(), units_in_cargo.end(), unit->tag) != units_in_cargo.end())
			continue;
		if (unit->weapon_cooldown > 0 || mediator->GetAttackStatus(unit))
			continue;

		ready_to_attack.push_back(unit);
	}

	if ((float)ready_to_attack.size() / basic_units.size() >= attack_threshold)
	{
		mediator->AddUnitsToAttackers(ready_to_attack);
	}

	// micro special units
	// oracles
	if (oracles.size())
		OraclesDefendArmy();

	return AttackLineResult::normal;
		
		
}

}