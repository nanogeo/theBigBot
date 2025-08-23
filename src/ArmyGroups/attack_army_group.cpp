
#include "attack_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "definitions.h"

#include "theBigBot.h"

namespace sc2 {


float AttackArmyGroup::CalculateDesiredRange(Units basic_units, Units enemy_units, float default_range, float unit_size)
{
	// TODO pass in this Units
	float enemy_dps = 0;
	for (const auto& unit : enemy_units)
	{
		enemy_dps += Utility::GetDPS(unit);
	}

	float friendly_dps = 0;
	for (const auto& unit : basic_units)
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

Point2D AttackArmyGroup::CalculateConcaveTarget(Point2D origin, Units closest_enemies, Path* path)
{
	// TODO use some clustering algorithm to better find the grouping of close enemies to face toward

	for (int i = 0; i < closest_enemies.size(); i++)
	{
		if (Distance2D(origin, closest_enemies[i]->pos) > LONG_RANGE)
		{
			closest_enemies.erase(closest_enemies.begin() + i);
			i--;
		}
	}

	if (closest_enemies.size() > 0)
	{
		return Utility::MedianCenter(closest_enemies);
	}
	else
	{
		return path->GetPointFrom(origin, 8, true); // default target is 8 units in front of army center
	}
}

Point2D AttackArmyGroup::FindLimitToAdvance(Path* path, Point2D pre_prism_limit, float dist)
{
	std::vector<Point2D> intersection_points;
	intersection_points = path->FindCircleIntersection(pre_prism_limit, dist);
	if (intersection_points.size() != 0)
	{
		return path->GetFurthestBack(intersection_points);
	}
	return Point2D(0, 0);
}

Point2D AttackArmyGroup::FindLimitToAdvance(Path* path, std::vector<UNIT_TYPEID> types_to_avoid, float extra_range, bool air,
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
			intersection_points = path->FindCircleIntersection(pos, Utility::GetAirRange(unit.first) + extra_range);
		else if (Utility::GetGroundRange(unit.first) > 0)
			intersection_points = path->FindCircleIntersection(pos, Utility::GetGroundRange(unit.first) + extra_range);

		if (intersection_points.size() == 0)
			continue;
		Point2D danger_point = path->GetFurthestBack(intersection_points);
		danger_points.push_back(danger_point);
	}
	if (danger_points.size() <= num_units_to_ignore)
		return path->GetEndPoint();

	return danger_points[num_units_to_ignore];
}

PathDirection AttackArmyGroup::ShouldMoveConcaveOrigin(Point2D origin, Point2D target, Path* path, float desired_range, Units close_enemies, Point2D limit)
{
	// too close
	if (Distance2D(origin, target) < desired_range - .1f ||
		(close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, origin) < VERY_CLOSE_RANGE) ||
		(origin == path->GetFurthestForward({ origin, limit }) && origin != limit))
	{
		return PathDirection::backward;

	}// too far away
	else if (Distance2D(origin, target) > desired_range + .1 && 
		(close_enemies.size() == 0 || Utility::DistanceToClosest(close_enemies, origin) > VERY_CLOSE_RANGE))
	{
		return PathDirection::forward;
	}
	else // perfect range
	{
		return PathDirection::none;
	}
}

float AttackArmyGroup::GetAverageDistanceForUnitPosition(std::map<const Unit*, Point2D> position_assignments)
{
	// TODO maybe ignore very far off units
	if (position_assignments.size() == 0)
		return 0;

	float total_dist = 0;
	float furthest_dist = 0;
	for (const auto& pos : position_assignments)
	{
		float dist = Distance2D(pos.first->pos, pos.second);
		total_dist += dist;
		if (dist > furthest_dist)
			furthest_dist = dist;
	}
	// ignore the furthest unit
	total_dist -= furthest_dist;

	return total_dist / position_assignments.size();
}

Point2D AttackArmyGroup::GetNewOriginForward(Point2D origin, Point2D target, float desired_range, Path* path, Point2D limit)
{
	float dist_to_move_origin = std::min(1.0f, abs(Distance2D(origin, target) - desired_range));
	Point2D new_origin = path->GetPointFrom(origin, dist_to_move_origin, true);
	return path->GetFurthestBack({ new_origin, limit });
}

Point2D AttackArmyGroup::GetNewOriginBackward(Point2D origin, Point2D target, float desired_range, Path* path, Units close_enemies, float unit_size)
{

	float dist_to_move_origin = std::min(1.0f, abs(desired_range - Distance2D(origin, target)));

	if (close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, origin) < VERY_CLOSE_RANGE)
		dist_to_move_origin = std::max(dist_to_move_origin, VERY_CLOSE_RANGE + unit_size - Utility::DistanceToClosest(close_enemies, origin));

	return path->GetPointFrom(origin, dist_to_move_origin, false);
}

std::vector<Point2D> AttackArmyGroup::FindConcaveWithPrism(int num_units, int num_prisms, float min_height, float unit_spacing, float unit_size, Point2D origin, Point2D target, std::vector<Point2D>& prism_positions)
{
	float max_height = min_height + 1;
	float range = 0; //r
	float concave_degree = 30; //p
	int max_width = 4;


	Point2D backward_vector = ((2 * origin) - target) - origin;
	Point2D forward_vector = origin - ((2 * origin) - target);
	forward_vector /= sqrt(forward_vector.x * forward_vector.x + forward_vector.y * forward_vector.y);

	Point2D offset_circle_center = Point2D(origin.x + concave_degree * forward_vector.x, origin.y + concave_degree * forward_vector.y);

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
		float arclength = (2 * unit_spacing) / (range + concave_degree + (((row * 2) - 1) * unit_spacing));
		for (float i = .5; i <= max_width - .5; i += 1)
		{
			if (!right_limit)
			{
				float unit_direction = backwards_direction + i * arclength;
				Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_size)) * cos(unit_direction),
					offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));
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
				Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * cos(unit_direction),
					offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));
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
				Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * cos(unit_direction),
					offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));
				if (mediator->IsPathable(unit_position))
				{
					prism_positions.push_back(unit_position);
				}
				offset = 1;
			}

			arclength = (2 * unit_spacing * ((6 / num_prisms) - .5f)) / (range + concave_degree + (((row * 2) - 1) * unit_spacing));
			for (float i = offset; i <= std::ceil((num_prisms / 2) - offset); i += 1)
			{
				// right position
				{
					float unit_direction = backwards_direction + i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));

					prism_positions.push_back(unit_position);
				}

				// left position
				{
					float unit_direction = backwards_direction - i * arclength;
					Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));

					prism_positions.push_back(unit_position);
				}
			}
			break;
		}

		// odd row
		row++;
		// middle point
		float unit_direction = backwards_direction;
		Point2D unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * cos(unit_direction),
			offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));
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
		arclength = (2 * unit_spacing) / (range + concave_degree + (((row * 2) - 1) * unit_spacing));
		for (int i = 1; i <= max_width - 1; i++)
		{
			if (!right_limit)
			{
				unit_direction = backwards_direction + i * arclength;
				unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * cos(unit_direction),
					offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));
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
				unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * cos(unit_direction),
					offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));
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
				unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * cos(unit_direction),
					offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));
				if (mediator->IsPathable(unit_position))
				{
					prism_positions.push_back(unit_position);
				}
				offset = 1;
			}

			arclength = (2 * unit_spacing * ((6 / num_prisms) - .5f)) / (range + concave_degree + (((row * 2) - 1) * unit_spacing));
			for (float i = offset; i <= std::ceil((num_prisms / 2) - offset); i += 1)
			{
				// right position
				{
					unit_direction = backwards_direction + i * arclength;
					unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));

					prism_positions.push_back(unit_position);
				}

				// left position
				{
					unit_direction = backwards_direction - i * arclength;
					unit_position = Point2D(offset_circle_center.x + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * cos(unit_direction),
						offset_circle_center.y + (range + concave_degree + (((row * 2) - 1) * unit_spacing)) * sin(unit_direction));

					prism_positions.push_back(unit_position);
				}
			}
			break;
		}
	}
	return concave_points;
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

std::vector<std::pair<const Unit*, UnitDanger>> AttackArmyGroup::CalculateUnitDanger(Units units) const
{
	std::vector<std::pair<const Unit*, UnitDanger>> incoming_damage;
	for (const auto& unit : units)
	{
		float damage = (float)mediator->GetIncomingDamage(unit);
		//agent->Debug()->DebugTextOut(std::to_string(damage), unit->pos, Color(255, 255, 255), 16);
		if (damage > 0)
		{
			incoming_damage.push_back(std::pair<const Unit*, UnitDanger>(unit, UnitDanger(unit, damage)));
		}
	}
	std::sort(incoming_damage.begin(), incoming_damage.end(),
		[](const std::pair<const Unit*, UnitDanger> a, const std::pair<const Unit*, UnitDanger> b) -> bool
		{
			return a.second < b.second;
		});
	return incoming_damage;
}

bool AttackArmyGroup::TestSwap(Point2D pos1, Point2D target1, Point2D pos2, Point2D target2)
{
	float curr_max = std::max(Distance2D(pos1, target1), Distance2D(pos2, target2));
	float swap_max = std::max(Distance2D(pos1, target2), Distance2D(pos2, target1));
	return swap_max < curr_max;
}

Units AttackArmyGroup::EvadeDamage(std::vector<std::pair<const Unit*, UnitDanger>> incoming_damage, Units prisms, Point2D standby_pos)
{
	Units escaping_units;

	std::sort(incoming_damage.begin(), incoming_damage.end(),
		[](const std::pair<const Unit*, UnitDanger> a, const std::pair<const Unit*, UnitDanger> b) -> bool
	{
		return a.second > b.second;
	});

	bool has_blink = mediator->CheckUpgrade(U_BLINK);

	std::vector<Tag> units_in_cargo;
	std::map<const Unit*, int> cargo_available;
	// if prisms -> pickup units in enough danger
	if (prisms.size() > 0)
	{
		for (const auto& prism : prisms)
		{
			cargo_available[prism] = prism->cargo_space_max - prism->cargo_space_taken;
			for (const auto& passanger : prism->passengers)
			{
				units_in_cargo.push_back(passanger.tag);
			}
		}
	}

	for (const auto& request : incoming_damage)
	{
		bool unit_found_space = false;
		if (std::find(units_in_cargo.begin(), units_in_cargo.end(), request.first->tag) != units_in_cargo.end())
		{
			// unit is already inside prism
			continue;
		}

		bool can_blink = false;
		if (has_blink && request.first->unit_type == STALKER && mediator->IsStalkerBlinkOffCooldown(request.first))
			can_blink = true;

		const Unit* prism_in_range = nullptr;
		for (const auto& cargo : cargo_available)
		{
			if (Distance2D(cargo.first->pos, request.first->pos) < RANGE_PRISM_PICKUP && 
				cargo.second >= Utility::GetCargoSize(request.first) &&
				(prism_in_range == nullptr || cargo.second > cargo_available[prism_in_range]))
				prism_in_range = cargo.first;
		}

		if (prism_in_range != nullptr && can_blink == false)
		{
			int cargo_space = cargo_available[prism_in_range];
			if (request.second.damage_value >= 40 || // TODO figure out less arbitrary numbers
				(request.second.damage_value == 20 && cargo_space >= 4) ||
				(request.second.damage_value == 10 && cargo_space >= 6))
			{
				escaping_units.push_back(request.first);
				mediator->SetUnitCommand(request.first, A_SMART, prism_in_range, CommandPriority::high);
				if (mediator->GetAttackStatus(request.first))
					mediator->CancelAttack(request.first);

				if (cargo_available[prism_in_range] == Utility::GetCargoSize(request.first))
					cargo_available.erase(prism_in_range);
				else
					cargo_available[prism_in_range] = cargo_available[prism_in_range] - Utility::GetCargoSize(request.first);
			}
		}
		else if (prism_in_range == nullptr && can_blink == true)
		{
			if (request.second.damage_value >= 20)
			{
				escaping_units.push_back(request.first);
				mediator->SetUnitCommand(request.first, A_BLINK, standby_pos, CommandPriority::high);
				if (mediator->GetAttackStatus(request.first))
					mediator->CancelAttack(request.first);
			}
		}
		else if (prism_in_range != nullptr && can_blink == true)
		{
			int cargo_space = cargo_available[prism_in_range];
			if (request.second.damage_value >= 40 ||
				(request.second.damage_value == 30 && cargo_space >= 4) ||
				(request.second.damage_value == 20 && cargo_space >= 6))
			{
				escaping_units.push_back(request.first);
				mediator->SetUnitCommand(request.first, A_SMART, prism_in_range, CommandPriority::high);
				if (mediator->GetAttackStatus(request.first))
					mediator->CancelAttack(request.first);

				if (cargo_available[prism_in_range] == Utility::GetCargoSize(request.first))
					cargo_available.erase(prism_in_range);
				else
					cargo_available[prism_in_range] = cargo_available[prism_in_range] - Utility::GetCargoSize(request.first);
			}
			else if (request.second.damage_value >= 20)
			{
				escaping_units.push_back(request.first);
				mediator->SetUnitCommand(request.first, A_BLINK, standby_pos, CommandPriority::high);
				if (mediator->GetAttackStatus(request.first))
					mediator->CancelAttack(request.first);
			}
		}
	}
	return escaping_units;
}

void AttackArmyGroup::OraclesDefendArmy(Units oracles, Path* path, Units basic_units)
{
	if (oracles.size() == 0)
		return;


	Point2D center = path->GetStartPoint();
	if (basic_units.size() > 0)
	{
		Point2D median_center = Utility::MedianCenter(basic_units);
		center = path->GetPointFrom(median_center, 3, false);
	}

	OraclesCastRevelation(oracles);
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
						mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, CommandPriority::low);
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
						mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_ON, CommandPriority::low);
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
					mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, CommandPriority::low);
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
			mediator->SetUnitCommand(oracle, A_MOVE, center, CommandPriority::low);
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
				mediator->SetUnitCommand(oracle, A_MOVE, center, CommandPriority::low);
				continue;
			}


			mediator->SetUnitCommand(oracle, A_ATTACK, closest_unit, CommandPriority::low);
			//agent->Debug()->DebugSphereOut(closest_unit->pos, .75, Color(0, 255, 255));

			target[oracle] = closest_unit->tag;
			time_last_attacked[oracle] = mediator->GetGameLoop() / FRAME_TIME;
			has_attacked[oracle] = false;
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
		}
		else if (has_attacked[oracle])
		{
			mediator->SetUnitCommand(oracle, A_MOVE, center, CommandPriority::low);

			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 0, 255));
		}
		else
		{
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
		}
	}
}

void AttackArmyGroup::OraclesDefendLocation(Units oracles, Units enemy_units, Point2D pos)
{
	Units close_enemy_units;
	float total_enemy_health = 0;
	for (const auto& unit : enemy_units)
	{
		if (Distance2D(unit->pos, pos) < MEDIUM_RANGE)
		{
			close_enemy_units.push_back(unit);
			if (Utility::IsLight(unit->unit_type))
				total_enemy_health += (unit->health + unit->shield) / 2;
			else
				total_enemy_health += unit->health + unit->shield;
		}
	}
	if (close_enemy_units.size() > 0)
	{
		int num_oracles_needed = (int)std::ceil(total_enemy_health / 200);

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
				if (oracle->energy > 10 && Utility::DistanceToClosest(close_enemy_units, oracle->pos) > 5)
				{
					if (mediator->IsOracleBeamActive(oracle))
					{
						mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, CommandPriority::high);
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
						mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_ON, CommandPriority::low);
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
			if (oracle->energy > 10 && (close_enemy_units.size() == 0 || Utility::DistanceToClosest(close_enemy_units, oracle->pos)))
			{
				if (mediator->IsOracleBeamActive(oracle))
				{
					mediator->SetUnitCommand(oracle, A_ORACLE_BEAM_OFF, CommandPriority::low);
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
			mediator->SetUnitCommand(oracle, A_MOVE, pos, CommandPriority::low);
			continue;
		}
		float now = mediator->GetGameLoop() / FRAME_TIME;
		bool weapon_ready = now - time_last_attacked[oracle] > .8; //.61

		//agent->Debug()->DebugTextOut("weapon ready " + std::to_string(weapon_ready), Point2D(.2, .35), Color(0, 255, 0), 20);
		//agent->Debug()->DebugTextOut("has attacked " + std::to_string(state_machine->has_attacked[oracle]), Point2D(.2, .37), Color(0, 255, 0), 20);
		//agent->Debug()->DebugTextOut("target " + std::to_string(state_machine->target[oracle]), Point2D(.2, .39), Color(0, 255, 0), 20);


		if (weapon_ready)
		{
			const Unit* closest_unit = Utility::ClosestTo(close_enemy_units, oracle->pos);
			if (closest_unit == nullptr || Distance2D(closest_unit->pos, oracle->pos) > 6)
			{
				mediator->SetUnitCommand(oracle, A_MOVE, pos, CommandPriority::low);
				continue;
			}


			mediator->SetUnitCommand(oracle, A_ATTACK, closest_unit, CommandPriority::low);
			//agent->Debug()->DebugSphereOut(closest_unit->pos, .75, Color(0, 255, 255));

			target[oracle] = closest_unit->tag;
			time_last_attacked[oracle] = mediator->GetGameLoop() / FRAME_TIME;
			has_attacked[oracle] = false;
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
		}
		else if (has_attacked[oracle])
		{
			const Unit* closest_unit = Utility::ClosestTo(close_enemy_units, oracle->pos);
			if (closest_unit == nullptr || Distance2D(closest_unit->pos, oracle->pos) > 6)
			{
				mediator->SetUnitCommand(oracle, A_MOVE, pos, CommandPriority::low);
				continue;
			}
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 0, 255));
		}
		else
		{
			//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
		}
	}
}

void AttackArmyGroup::OraclesCastRevelation(Units oracles)
{
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
				mediator->SetUnitCommand(highest_over_75, A_REVELATION, unit_to_revelate->pos, CommandPriority::high);
				//agent->Debug()->DebugSphereOut(highest_over_75->pos, 2, Color(255, 0, 0));

			}
			else if (lowest_over_25 != nullptr)
			{
				mediator->SetUnitCommand(lowest_over_25, A_REVELATION, unit_to_revelate->pos, CommandPriority::high);
				//agent->Debug()->DebugSphereOut(lowest_over_25->pos, 2, Color(255, 0, 0));
			}
		}
	}
}

AttackLineResult AttackArmyGroup::AttackLine(Units units, Point2D& origin, float normal_range, Path* path, bool should_limit_advance,
	Point2D prism_limit, Units prisms, float spread, std::map<const Unit*, Point2D>& position_assignments, Units oracles, bool& is_advancing, float threshold)
{
	if (origin == Point2D(0, 0))
	{
		origin = path->FindClosestPoint(Utility::MedianCenter(units));
	}

	// update unit size TODO maybe move to addunit/removeunit
	float unit_size = Utility::GetLargestUnitSize(units);

	Units enemies_in_range = Utility::GetUnitsWithin(mediator->GetUnits(IsNonbuilding(Unit::Alliance::Enemy)), origin, LONG_RANGE);
	Units closest_enemies = Utility::NClosestUnits(enemies_in_range, origin, 5);

	float desired_range = CalculateDesiredRange(units, enemies_in_range, normal_range, unit_size);
	Point2D concave_target = CalculateConcaveTarget(origin, closest_enemies, path);

	Point2D limit = path->GetEndPoint();
	if (should_limit_advance)
	{
		if (mediator->GetEnemyRace() == Race::Terran)
		{
			limit = FindLimitToAdvance(path, { SIEGE_TANK_SIEGED }, 1, false, 1);
		}
		else if (mediator->GetEnemyRace() == Race::Protoss)
		{
			if (prism_limit != Point2D(0, 0) && prisms.size() == 0)
			{
				Point2D possible_limit = FindLimitToAdvance(path, prism_limit, MEDIUM_RANGE);
				if (possible_limit != Point2D(0, 0))
					limit = possible_limit;
			}
		}
	}

	// move concave origin if necessary
	PathDirection direction = ShouldMoveConcaveOrigin(origin, concave_target, path, desired_range, closest_enemies, limit);

	if (position_assignments.size() != units.size() + prisms.size() ||
		direction == PathDirection::backward && is_advancing == true ||
		direction == PathDirection::forward && is_advancing == false ||
		GetAverageDistanceForUnitPosition(position_assignments) < 1)
	{
		if (direction == PathDirection::backward)
		{
			origin = GetNewOriginBackward(origin, concave_target, desired_range, path, closest_enemies, unit_size);
			is_advancing = false;
		}
		else
		{
			origin = GetNewOriginForward(origin, concave_target, desired_range, path, limit);
			is_advancing = true;
		}
	}

	Point2D standby_pos = path->GetPointFrom(origin, 8, false);

	// Find positions
	std::vector<Point2D> prism_positions;
	std::vector<Point2D> concave_positions = FindConcaveWithPrism((int)units.size(), (int)prisms.size(),
		mediator->ToPoint3D(origin).z - .5f, unit_size + spread, unit_size, origin, concave_target, prism_positions);

	// assign units to positions
	position_assignments = AssignUnitsToPositions(units, concave_positions);
	if (prisms.size() > 0)
	{
		std::map<const Unit*, Point2D> prism_position_assignments = AssignUnitsToPositions(prisms, prism_positions);
		for (const auto& assignment : prism_position_assignments)
		{
			position_assignments[assignment.first] = assignment.second;
		}
	}

	// avoid danger
	std::vector<std::pair<const Unit*, UnitDanger>> incoming_damage = CalculateUnitDanger(units);
	Units escaping_units = EvadeDamage(incoming_damage, prisms, standby_pos);


	// move units to positions
	for (const auto& assignment : position_assignments)
	{
		if (mediator->GetAttackStatus(assignment.first) == false)
			mediator->SetUnitCommand(assignment.first, A_MOVE, assignment.second, CommandPriority::low);
	}
	std::vector<Tag> units_in_cargo;

	if (prisms.size() > 0)
	{
		for (const auto& prism : prisms)
		{
			mediator->ForceUnitCommand(prism, A_UNLOAD_AT, prism);
			for (const auto& passanger : prism->passengers)
			{
				units_in_cargo.push_back(passanger.tag);
				mediator->RemoveAllAttacksAtUnit(mediator->GetUnit(passanger.tag));
			}
		}
	}

	// if units that can attack > threshold -> add units to attackers
	// ignore units entering prisms, in prisms, or blinking/casting
	Units ready_to_attack;
	for (const auto& unit : units)
	{
		if (std::find(escaping_units.begin(), escaping_units.end(), unit) != escaping_units.end())
			continue;
		if (std::find(units_in_cargo.begin(), units_in_cargo.end(), unit->tag) != units_in_cargo.end())
			continue;
		if (unit->weapon_cooldown > 0 || mediator->GetAttackStatus(unit))
			continue;

		ready_to_attack.push_back(unit);
	}

	if ((float)ready_to_attack.size() / units.size() >= threshold)
	{
		mediator->AddUnitsToAttackers(ready_to_attack);
	}


	if (origin == path->GetEndPoint())
		return AttackLineResult::reached_end_of_path;

	return AttackLineResult::normal;
}

}