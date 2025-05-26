
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

	ArmyGroup::ArmyGroup(Mediator* mediator, PathManager attack_line, std::vector<Point2D> attack_path, ArmyRole role, std::vector<UNIT_TYPEID> unit_types)
	{
		this->mediator = mediator;
		event_id = mediator->GetUniqueId();

		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
			this->OnUnitDamagedListener(unit, health, shields);
		};
		mediator->AddListenerToOnUnitDamagedEvent(event_id, onUnitDamaged);

		this->attack_path = attack_path;
		attack_path_line = attack_line;
		this->role = role;
		for (const auto& type : unit_types)
		{
			this->unit_types.push_back(type);
		}
	}

	ArmyGroup::ArmyGroup(Mediator* mediator, std::vector<Point2D> attack_path, ArmyRole role, std::vector<UNIT_TYPEID> unit_types)
	{
		this->mediator = mediator;
		event_id = mediator->GetUniqueId();

		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
			this->OnUnitDamagedListener(unit, health, shields);
		};
		mediator->AddListenerToOnUnitDamagedEvent(event_id, onUnitDamaged);

		this->attack_path = attack_path;
		this->role = role;
		for (const auto& type : unit_types)
		{
			this->unit_types.push_back(type);
		}
	}
	
	ArmyGroup::ArmyGroup(Mediator* mediator, ArmyRole role, std::vector<UNIT_TYPEID> unit_types)
	{
		this->mediator = mediator;
		event_id = mediator->GetUniqueId();

		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
			this->OnUnitDamagedListener(unit, health, shields);
		};
		mediator->AddListenerToOnUnitDamagedEvent(event_id, onUnitDamaged);

		this->role = role;
		for (const auto& type : unit_types)
		{
			this->unit_types.push_back(type);
		}
	}

	ArmyGroup::ArmyGroup(Mediator* mediator, Point2D target_pos, ArmyRole role, std::vector<UNIT_TYPEID> unit_types)
	{
		this->mediator = mediator;
		event_id = mediator->GetUniqueId();

		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
			this->OnUnitDamagedListener(unit, health, shields);
		};
		mediator->AddListenerToOnUnitDamagedEvent(event_id, onUnitDamaged);

		this->target_pos = target_pos;
		this->role = role;
		for (const auto& type : unit_types)
		{
			this->unit_types.push_back(type);
		}
	}

	ArmyGroup::ArmyGroup(Mediator* mediator, Point2D start, Point2D end, ArmyRole role, std::vector<UNIT_TYPEID> unit_types)
	{
		this->mediator = mediator;
		event_id = mediator->GetUniqueId();

		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		mediator->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
			this->OnUnitDamagedListener(unit, health, shields);
		};
		mediator->AddListenerToOnUnitDamagedEvent(event_id, onUnitDamaged);

		double slope = (start.y - end.y) / (start.x - end.x);

		double line_a = slope;
		double line_b = start.y - (slope * start.x);

		defense_line = new LineSegmentLinearX((float)line_a, (float)line_b, start.x, end.x);

		this->role = role;
		for (const auto& type : unit_types)
		{
			this->unit_types.push_back(type);
		}
	}

	ArmyGroup::~ArmyGroup()
	{
		mediator->RemoveListenerToOnUnitDamagedEvent(event_id);
		mediator->RemoveListenerToOnUnitDestroyedEvent(event_id);
	}

	void ArmyGroup::AddUnit(const Unit* unit)
	{
		if (std::find(all_units.begin(), all_units.end(), unit) != all_units.end())
			return;

		UNIT_TYPEID type = unit->unit_type.ToType();

		all_units.push_back(unit);
		new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());

		switch (type)
		{
		case UNIT_TYPEID::PROTOSS_ZEALOT:
			zealots.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_STALKER:
			stalkers.push_back(unit);
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
			time_last_attacked[unit] = 0;
			has_attacked[unit] = true;
			break;
		case UNIT_TYPEID::PROTOSS_CARRIER:
			carriers.push_back(unit);
			break;
		case UNIT_TYPEID::PROTOSS_TEMPEST:
			tempests.push_back(unit);
			break;
		default:
			//std::cout << "Error unknown unit type in ArmyGroup::AddUnit";
			break;
		}
	}

	void ArmyGroup::AddNewUnit(const Unit* unit)
	{
		if (std::find(new_units.begin(), new_units.end(), unit) != new_units.end())
			return;

		new_units.push_back(unit);
	}

	void ArmyGroup::RemoveUnit(const Unit* unit)
	{
		if (unit == nullptr)
			return; // TODO log error and callstack

		mediator->SetUnitCommand(unit, ABILITY_ID::STOP, 1);

		all_units.erase(std::remove(all_units.begin(), all_units.end(), unit), all_units.end());
		new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());
		standby_units.erase(std::remove(standby_units.begin(), standby_units.end(), unit), standby_units.end());
		unit_position_asignments.erase(unit);

		if (state_machine != nullptr)
			state_machine->RemoveUnit(unit);

		Units* units;
		switch (unit->unit_type.ToType())
		{
		case ZEALOT:
			units = &zealots;
			break;
		case STALKER:
			units = &stalkers;
			break;
		case ADEPT:
			units = &adepts;
			break;
		case SENTRY:
			units = &sentries;
			break;
		case HIGH_TEMPLAR:
			units = &high_templar;
			break;
		case DARK_TEMPLAR:
			units = &dark_templar;
			break;
		case ARCHON:
			units = &archons;
			break;
		case IMMORTAL:
			units = &immortals;
			break;
		case COLOSSUS:
			units = &collossi;
			break;
		case DISRUPTOR:
			units = &disrupter;
			break;
		case OBSERVER:
			units = &observers;
			break;
		case PRISM:
			units = &warp_prisms;
			break;
		case PHOENIX:
			units = &phoenixes;
			break;
		case VOID_RAY:
			units = &void_rays;
			break;
		case ORACLE:
			units = &oracles;
			break;
		case CARRIER:
			units = &carriers;
			break;
		case TEMPEST:
			units = &tempests;
			break;
		case PROBE:
			return;
		default:
			std::cerr << "Error unknown unit type in ArmyGroup::RemoveUnit " << UnitTypeToName(unit->unit_type.ToType()) << std::endl;
			mediator->LogMinorError();
			return;
		}

		units->erase(std::remove(units->begin(), units->end(), unit), units->end());
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

	std::vector<Point2D> ArmyGroup::FindConcaveWithPrism(Point2D origin, Point2D fallback_point, int num_units, int num_prisms, float unit_size, float dispersion, float concave_degree, std::vector<Point2D>& prism_positions)
	{
		float min_height = mediator->ToPoint3D(origin).z - .5f;
		float max_height = min_height + 1;
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
						if (Utility::DistanceToClosest(mediator->agent->corrosive_bile_positions, unit_position) > .5f + unit_size + .1f)
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
						if (Utility::DistanceToClosest(mediator->agent->corrosive_bile_positions, unit_position) > .5f + unit_size + .1f)
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
				if (Utility::DistanceToClosest(mediator->agent->corrosive_bile_positions, unit_position) > .5f + unit_size + .1f)
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
						if (Utility::DistanceToClosest(mediator->agent->corrosive_bile_positions, unit_position) > .5f + unit_size + .1f)
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
						if (Utility::DistanceToClosest(mediator->agent->corrosive_bile_positions, unit_position) > .5f + unit_size + .1f)
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

				if (unit->orders.size() > 0 && unit->orders[0].ability_id == ABILITY_ID::ATTACK && unit->weapon_cooldown == 0)
					mediator->SetUnitCommand(unit, ABILITY_ID::SMART, prism.first, 0, true);
				else
					mediator->SetUnitCommand(unit, ABILITY_ID::SMART, prism.first, 0);
				prism.second -= cargo_size;
				break;
			}
		}
	}

	void ArmyGroup::DodgeShots()
	{
		for (const auto &Funit : mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_STALKER)))
		{
			int danger = mediator->agent->IncomingDamage(Funit);
			if (danger)
			{
				bool blink_ready = mediator->IsStalkerBlinkOffCooldown(Funit);
				if (blink_ready && (danger > Funit->shield || danger > (Funit->shield_max / 2)))
				{
					mediator->SetUnitCommand(Funit, ABILITY_ID::EFFECT_BLINK, Funit->pos + Point2D(0, 4), 2);
					mediator->SetUnitCommand(Funit, ABILITY_ID::ATTACK, Funit->pos - Point2D(0, 4), 0, true);
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


		/*for (const auto &pos : attack_concave_positions)
		{
			agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .625, Color(255, 0, 0));
		}
		for (const auto &pos : retreat_concave_positions)
		{
			agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .625, Color(0, 255, 0));
		}*/
	}

	void ArmyGroup::DefendFrontDoor(Point2D door_open_pos, Point2D door_closed_pos)
	{
		if (new_units.size() > 0)
		{
			for (const auto& unit : new_units)
			{
				AddUnit(unit);
			}
		}
		if (all_units.size() == 0)
			return;
		const Unit* guard = all_units[0];

		Units enemies = mediator->GetUnits(Unit::Alliance::Enemy, IsNotFlyingUnit());
		for (int i = 0; i < enemies.size(); i++)
		{
			if (enemies[i]->is_building)
			{
				enemies.erase(enemies.begin() + i);
				i--;
			}
		}

		if (enemies.size() == 0)
		{
			mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_MOVE, door_open_pos, 0);
			return;
		}

		const Unit* closest_to_door = Utility::ClosestTo(enemies, door_closed_pos);
		double dist_to_closest = Distance2D(closest_to_door->pos, door_closed_pos);

		if (dist_to_closest > 10)
		{
			mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_MOVE, door_open_pos, 0);
			return;
		}

		Point2D guard_move_to = Utility::PointBetween(door_closed_pos, closest_to_door->pos, (float)std::min(4.0, std::max(0.0, (dist_to_closest / 2) - 1)));

		// hold position when door is fully closed to prevent being pushed away by pulled probes
		if (Distance2D(guard_move_to, door_closed_pos) < .5 && Distance2D(guard->pos, door_closed_pos) < .5)
		{
			mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_HOLDPOSITION, 10);
			return;
		}
		if (guard->weapon_cooldown != 0 && (Distance2D(guard->pos, door_closed_pos) < .5 || Distance2D(guard->pos, door_open_pos) > Distance2D(door_closed_pos, door_open_pos)))
		{
			mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_HOLDPOSITION, 10);
			return;
		}

		// TODO use fire control to find the best target
		const Unit* closest_to_guard = Utility::ClosestTo(enemies, guard->pos);
		
		if (Distance2D(closest_to_guard->pos, guard->pos) < Utility::RealRange(guard, closest_to_guard))
		{
			// enemy in range
			if (Distance2D(guard->pos, guard_move_to) > .5 && 
				Distance2D(guard->pos, Utility::ClosestTo(mediator->GetUnits(IsUnit(NEXUS)), guard->pos)->pos) > 
					Distance2D(door_closed_pos, Utility::ClosestTo(mediator->GetUnits(IsUnit(NEXUS)), guard->pos)->pos))
			{
				mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_MOVE, guard_move_to, 0);
				mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_HOLDPOSITION, 0, true);
			}
			else
			{
				mediator->SetUnitCommand(guard, ABILITY_ID::ATTACK_ATTACK, closest_to_guard, 0);
				mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_HOLDPOSITION, 0, true);
			}
		}
		else
		{
			mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_MOVE, guard_move_to, 0);
			mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_HOLDPOSITION, 0, true);
		}
	}

	void ArmyGroup::DefendThirdBase(Point2D third_base_pylon_gap)
	{
		while (new_units.size() > 0)
		{
			AddUnit(new_units[0]);
		}
		for (const auto& unit : all_units)
		{
			if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Self, IsUnit(PROBE)), unit->pos) < 2)
			{
				mediator->SetUnitCommand(unit, ABILITY_ID::STOP, 0);
			}
			else
			{
				if (Distance2D(unit->pos, third_base_pylon_gap) > 1 && unit->orders.size() == 0)
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, third_base_pylon_gap, 0);
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_HOLDPOSITION, 0, true);
				}
			}
		}

		// TODO use fire control to find the best target
	}

	void ArmyGroup::DefendLine()
	{
		int desired = 0;
		float status = mediator->GetWorstOngoingAttackValue();

		if (status < -50)
			desired = 5;
		else if (status < -25)
			desired = 3;
		else if (status < 0)
			desired = 1;

		desired_units = std::max(desired, desired_units);
		max_units = std::max(desired + 10, max_units);

		for (int i = 0; i < new_units.size(); i++)
		{
			if (Distance2D(new_units[i]->pos, defense_line->GetStartPoint()) > 10)
			{
				mediator->SetUnitCommand(new_units[i], ABILITY_ID::GENERAL_MOVE, defense_line->GetStartPoint(), 0);
			}
			else
			{
				AddUnit(new_units[i]);
				i--;
			}
		}

		Units enemy_ground_units = mediator->GetUnits(Unit::Alliance::Enemy, IsNotFlyingUnit());
		Units enemy_units = mediator->GetUnits(Unit::Alliance::Enemy);

		// remove any enemies too far from defense location
		for (int i = 0; i < enemy_ground_units.size(); i++) 
		{
			Point2D closest_pos = defense_line->FindClosestPoint(enemy_ground_units[i]->pos);
			if (Distance2D(enemy_ground_units[i]->pos, closest_pos) > 15)
			{
				enemy_ground_units.erase(enemy_ground_units.begin() + i);
				i--;
			}
		}
		for (int i = 0; i < enemy_units.size(); i++)
		{
			Point2D closest_pos = defense_line->FindClosestPoint(enemy_units[i]->pos);
			if (Distance2D(enemy_units[i]->pos, closest_pos) > 15)
			{
				enemy_units.erase(enemy_units.begin() + i);
				i--;
			}
		}

		if (enemy_units.size() == 0)
		{
			desired_units = 0;
			max_units = 10;
		}

		if (enemy_ground_units.size() > 0)
		{
			int num_active_units = (int)(all_units.size() - oracles.size());
			for (const auto& oracle : oracles)
			{
				if (mediator->IsOracleBeamActive(oracle))
					num_active_units++;
			}

			for (const auto& oracle : oracles)
			{
				float now = mediator->GetCurrentTime();
				bool weapon_ready = now - time_last_attacked[oracle] > .61;
				bool beam_active = mediator->IsOracleBeamActive(oracle);
				bool beam_activatable = false;

				if (!beam_active && oracle->energy >= 40 && enemy_ground_units.size() > num_active_units * 2)
					beam_activatable = true;

				const Unit* closest_unit = Utility::ClosestTo(enemy_ground_units, oracle->pos);
				if (beam_active)
				{
					if (Distance2D(oracle->pos, closest_unit->pos) > 4)
					{
						float dist = Distance2D(oracle->pos, closest_unit->pos);
						mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(oracle->pos, closest_unit->pos, dist + 1), 0, false);
					}
					else if (weapon_ready)
					{
						mediator->SetUnitCommand(oracle, ABILITY_ID::ATTACK_ATTACK, closest_unit, 0, false);
						time_last_attacked[oracle] = now;
						has_attacked[oracle] = false;
						//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 255, 0));
					}
					else if (has_attacked[oracle])
					{
						if ((mediator->GetUnit(oracle->engaged_target_tag) == nullptr ||
							Distance2D(oracle->pos, mediator->GetUnit(oracle->engaged_target_tag)->pos) > 3) ||
							Distance2D(oracle->pos, closest_unit->pos) > 3)  // only move if target is getting away
							mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, 0, false);
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
						mediator->SetUnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMON, 0, false);
					}
					else
					{
						mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, 0, false);
					}
				}
				else
				{
					mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, 0, false);
					//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
				}
				//agent->Debug()->DebugTextOut(std::to_string(now - state_machine->time_last_attacked[oracle]), Point2D(.7, .7), Color(0, 255, 255), 20);
				//agent->Debug()->DebugTextOut(std::to_string(agent->Observation()->GetGameLoop()), Point2D(.7, .75), Color(0, 255, 255), 20);

			}
		}
		else
		{
			for (const auto& oracle : oracles)
			{
				if (mediator->IsOracleBeamActive(oracle))
				{
					mediator->SetUnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF, 0);
					continue;
				}

				double dist_to_start = Distance2D(oracle->pos, defense_line->GetStartPoint());
				double dist_to_end = Distance2D(oracle->pos, defense_line->GetEndPoint());

				if (oracle->orders.size() > 0 &&
					(oracle->orders[0].target_pos == defense_line->GetStartPoint() || oracle->orders[0].target_pos == defense_line->GetEndPoint()))
				{
					continue;
				}
				else if (dist_to_start < 1)
				{
					mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, defense_line->GetEndPoint(), 0);
				}
				else if (dist_to_end < 1)
				{
					mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, defense_line->GetStartPoint(), 0);
				}
				else
				{
					if (dist_to_end < dist_to_start)
						mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, defense_line->GetEndPoint(), 0);
					else
						mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, defense_line->GetStartPoint(), 0);
				}
			}
		}

		if (enemy_units.size() > 0)
		{
			for (const auto& unit : all_units)
			{
				if (unit->unit_type == ORACLE)
					continue;

				const Unit* closest_unit = Utility::ClosestTo(enemy_units, unit->pos);
				if (closest_unit && (unit->orders.size() == 0 || unit->orders[0].ability_id != ABILITY_ID::ATTACK))
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK, closest_unit->pos, 0);
				}
				else if (closest_unit == nullptr)
				{
					double dist_to_start = Distance2D(unit->pos, defense_line->GetStartPoint());
					double dist_to_end = Distance2D(unit->pos, defense_line->GetEndPoint());

					if (unit->orders.size() > 0 &&
						(unit->orders[0].target_pos == defense_line->GetStartPoint() || unit->orders[0].target_pos == defense_line->GetEndPoint()))
					{
						continue;
					}
					else if (dist_to_start < 1)
					{
						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, defense_line->GetEndPoint(), 0);
					}
					else if (dist_to_end < 1)
					{
						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, defense_line->GetStartPoint(), 0);
					}
					else
					{
						if (dist_to_end < dist_to_start)
							mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, defense_line->GetEndPoint(), 0);
						else
							mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, defense_line->GetStartPoint(), 0);
					}
				}
			}
		}
	}

	void ArmyGroup::CannonRushPressure()
	{
		Point2D attack_pos = mediator->GetEnemyStartLocation();
		Point2D retreat_pos;
		if (mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_SHIELDBATTERY)).size() > 0)
			retreat_pos = Utility::FurthestFrom(mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_SHIELDBATTERY)), attack_pos)->pos;
		else if (mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_PHOTONCANNON)).size() > 0)
			retreat_pos = Utility::FurthestFrom(mediator->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_PHOTONCANNON)), attack_pos)->pos;

		Units enemies = mediator->GetUnits(IsFightingUnit(Unit::Alliance::Enemy));
		if (enemies.size() > 0)
		{
			const Unit* closest_enemy = Utility::ClosestUnitTo(enemies, retreat_pos);
			if (Distance2D(retreat_pos, closest_enemy->pos) > 11)
				retreat_pos = Utility::PointBetween(closest_enemy->pos, retreat_pos, 11);
		}
		else
		{
			enemies = mediator->GetUnits(Unit::Alliance::Enemy);
			const Unit* closest_enemy = Utility::ClosestUnitTo(enemies, retreat_pos);
			if (Distance2D(retreat_pos, closest_enemy->pos) > 8)
				retreat_pos = Utility::PointBetween(closest_enemy->pos, retreat_pos, 8);
		}
		
		//agent->Debug()->DebugSphereOut(agent->ToPoint3D(attack_pos), 2, Color(255, 255, 0));
		//agent->Debug()->DebugSphereOut(agent->ToPoint3D(retreat_pos), 2, Color(0, 255, 0));

		Units units_ready;
		Units units_on_cd;
		/*for (const auto &tempest : tempests)
		{
			//agent->Debug()->DebugSphereOut(agent->ToPoint3D(tempest->pos), 10, Color(255, 255, 255));
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
		}*/

		if (units_ready.size() > 0)
		{
			mediator->AddUnitsToAttackers(units_ready);
			mediator->SetUnitsCommand(units_ready, ABILITY_ID::GENERAL_MOVE, attack_pos, 0);
		}

	}

	void ArmyGroup::ScourMap()
	{
		Units enemy_buildings = mediator->GetUnits(Unit::Alliance::Enemy, IsBuilding());
		ImageData raw_map = mediator->GetPathingGrid();
		for (const auto& unit : mediator->GetUnits(IsFightingUnit(Unit::Alliance::Self)))
		{
			if (unit->orders.size() == 0)
			{
				if (enemy_buildings.size() > 0)
				{
					const Unit* closest = Utility::ClosestTo(enemy_buildings, unit->pos);
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK, closest->pos, 0);
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
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK, pos, 0);
				}
			}
		}
	}

	void ArmyGroup::SimpleAttack()
	{
		for (int i = 0; i < new_units.size(); i++)
		{
			AddUnit(new_units[i]);
			i--;
		}
		if (!ready)
		{
			mediator->SetUnitsCommand(all_units, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(all_units), 0);
			if (Utility::GetUnitsWithin(all_units, Utility::MedianCenter(all_units), 10).size() >= desired_units)
			{
				ready = true;
				accept_new_units = false;
			}
		}
		for (const auto& unit : all_units)
		{
			if (unit->orders.size() == 0)
			{
				for (const auto& point : attack_path)
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, point, 0, true);
				}
			}
		}
	}

	void ArmyGroup::DefendLocation()
	{
		for (int i = 0; i < new_units.size(); i++)
		{
			mediator->SetUnitCommand(new_units[i], ABILITY_ID::ATTACK_ATTACK, target_pos, 0);
			if (Distance2D(new_units[i]->pos, target_pos) <= 10)
			{
				AddUnit(new_units[i]);
				i--;
			}
		}
		const Unit* enemy_minerals = nullptr;
		const Unit* base_minerals = nullptr;

		if (enemy_minerals == nullptr)
		{
			enemy_minerals = Utility::ClosestTo(mediator->GetUnits(IsUnits(MINERAL_PATCH)), mediator->GetEnemyStartLocation());
			if (enemy_minerals == nullptr)
			{
				std::cerr << "Error could not find minerals close to " << std::to_string(mediator->GetEnemyStartLocation().x) << ", " <<
					std::to_string(mediator->GetEnemyStartLocation().y) << " in ArmyGroup::DefendLocation" << std::endl;
				mediator->LogMinorError();
			}
		}
		if (base_minerals == nullptr)
		{
			base_minerals = Utility::ClosestTo(mediator->GetUnits(IsUnits(MINERAL_PATCH)), target_pos);
			if (base_minerals == nullptr)
			{
				std::cerr << "Error could not find minerals close to " << std::to_string(target_pos.x) << ", " << std::to_string(target_pos.y) <<
					" in ArmyGroup::DefendLocation" << std::endl;
				mediator->LogMinorError();
			}
		}
		for (const auto &unit : all_units)
		{
			if (unit->unit_type == PROBE)
			{
				if (enemy_minerals == nullptr)
				{
					enemy_minerals = Utility::ClosestTo(mediator->GetUnits(IsUnits(MINERAL_PATCH)), mediator->GetEnemyStartLocation());
					if (enemy_minerals == nullptr)
					{
						std::cerr << "Error could not find minerals close to " << std::to_string(mediator->GetEnemyStartLocation().x) << ", " <<
							std::to_string(mediator->GetEnemyStartLocation().y) << " in ArmyGroup::DefendLocation" << std::endl;
						mediator->LogMinorError();
						continue;
					}
				}
				if (base_minerals == nullptr)
				{
					base_minerals = Utility::ClosestTo(mediator->GetUnits(IsUnits(MINERAL_PATCH)), target_pos);
					if (base_minerals == nullptr)
					{
						std::cerr << "Error could not find minerals close to " << std::to_string(target_pos.x) << ", " << std::to_string(target_pos.y) <<
							" in ArmyGroup::DefendLocation" << std::endl;
						mediator->LogMinorError();
						continue;
					}
				}
				if (unit->weapon_cooldown == 0)
				{
					Units units_in_range = Utility::GetUnitsInRange(mediator->GetUnits(Unit::Alliance::Enemy), unit, 0);
					if (units_in_range.size() > 0)
						mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK, units_in_range[0], 1);
					else if (Distance2D(unit->pos, target_pos) > 15)
						mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK, unit->pos, 0);
					else
						mediator->SetUnitCommand(unit, ABILITY_ID::SMART, enemy_minerals, 0);
				}
				else
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::SMART, base_minerals, 0);
				}
				continue;
			}
			// find closest enemy to unit and defense point
			// if within 10 of defense point and weapon off cooldown then attack unit
			// else move back to defense point
			if (unit->weapon_cooldown > 0 || Distance2D(unit->pos, target_pos) > 10)
			{
				const Unit* closest = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), unit->pos);
				if (closest == NULL || Distance2D(closest->pos, unit->pos) < Utility::GetGroundRange(unit) - 1)
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, target_pos, 0);
				else
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, closest, 0);
			}
			else
			{
				Units enemy_units = Utility::CloserThan(mediator->GetUnits(Unit::Alliance::Enemy), 20, target_pos);
				if (enemy_units.size() == 0)
					continue;
				Point2D defense_pos = target_pos;
				std::sort(enemy_units.begin(), enemy_units.end(),
					[&unit, &defense_pos](const Unit* a, const Unit* b) -> bool
				{
					return Distance2D(unit->pos, a->pos) + Distance2D(defense_pos, a->pos) <
						Distance2D(unit->pos, b->pos) + Distance2D(defense_pos, b->pos);
				});

				mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, enemy_units[0], 0);
			}
		}
	}

	void ArmyGroup::ObserverScout()
	{
		for (int i = 0; i < new_units.size(); i++)
		{
			if (new_units[i]->unit_type == OBSERVER)
			{
				AddUnit(new_units[i]);
				i--;
			}
		}
		Point2D enemy_main = Utility::PointBetween(mediator->GetEnemyStartLocation(), mediator->GetStartLocation(), 2);
		Point2D enemy_natural = Utility::PointBetween(mediator->GetEnemyNaturalLocation(), mediator->GetStartLocation(), 2);
		// update target
		if (target_pos == Point2D(0, 0))
		{
			target_pos = enemy_main;
		}
		else
		{
			Point2D obs = Utility::Center(observers);
			if (obs == Point2D(0, 0))
				return;

			double s = -1 * (enemy_main.x - enemy_natural.x) / (enemy_main.y - enemy_natural.y);
			double main_direction = s * (enemy_natural.x - enemy_main.x) - enemy_natural.y + enemy_main.y;
			double natural_direction = s * (enemy_main.x - enemy_natural.x) - enemy_main.y + enemy_natural.y;

			double outside_main = s * (obs.x - enemy_main.x) - obs.y + enemy_main.y;
			double outside_natural = s * (obs.x - enemy_natural.x) - obs.y + enemy_natural.y;

			if (target_pos == enemy_main && ((main_direction * outside_main) < 0 || Distance2D(obs, enemy_main) < 10))
				target_pos = enemy_natural;
			else if (target_pos == enemy_natural && ((natural_direction * outside_natural) < 0 || Distance2D(obs, enemy_natural) < 10))
				target_pos = enemy_main;
		}
		Units enemy_units = mediator->GetUnits(Unit::Alliance::Enemy);
		for (const auto& obs : observers)
		{
			const Unit* closest_danger = Utility::ClosestUnitTo(Utility::GetUnitsThatCanAttack(enemy_units, obs, 1), obs->pos);
			if (closest_danger == nullptr)
				mediator->SetUnitCommand(obs, ABILITY_ID::GENERAL_MOVE, target_pos, 0);
			else
				mediator->SetUnitCommand(obs, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(obs->pos, closest_danger->pos, -1), 0);
		}
	}

	void ArmyGroup::OutsideControl()
	{
		for (int i = 0; i < new_units.size(); i++)
		{
			if (state_machine->AddUnit(new_units[i]))
			{
				AddUnit(new_units[i]);
				i--;
			}
		}
	}

	void ArmyGroup::ScoutBases()
	{
		for (int i = 0; i < new_units.size(); i++)
		{
			AddUnit(new_units[i]);
			i--;
		}

		if (all_units.size() == 0)
			return;

		if (target_pos == Point2D(0, 0) && attack_path.size() > 0)
			target_pos = Utility::ClosestTo(attack_path, all_units[0]->pos);

		if (Distance2D(all_units[0]->pos, target_pos) > 7)
		{
			if (Utility::DistanceToClosest(mediator->GetUnits(IsUnits({ NEXUS, COMMAND_CENTER, COMMAND_CENTER_FLYING, ORBITAL, ORBITAL_FLYING, PLANETARY, HATCHERY, LAIR, HIVE })), target_pos) < 1)
			{
				attack_path.erase(std::remove(attack_path.begin(), attack_path.end(), target_pos), attack_path.end());
				if (attack_path.size() == 0)
				{
					attack_path = mediator->GetAllBases();
				}
				target_pos = Utility::ClosestTo(attack_path, all_units[0]->pos);
			}
			mediator->SetUnitsCommand(all_units, ABILITY_ID::MOVE_MOVE, target_pos, 0, false);
		}
		else
		{
			if (attack_path.size() == 0)
			{
				attack_path = mediator->GetAllBases();
			}
			target_pos = Utility::ClosestTo(attack_path, all_units[0]->pos);
			attack_path.erase(std::remove(attack_path.begin(), attack_path.end(), target_pos), attack_path.end());
		}

	}

	void ArmyGroup::DenyBase()
	{
		for (int i = 0; i < new_units.size(); i++)
		{
			AddUnit(new_units[i]);
			i--;
		}

		if (all_units.size() == 0)
			return;

		mediator->SetUnitsCommand(all_units, ABILITY_ID::ATTACK_ATTACK, target_pos, 1);

		if (Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ COMMAND_CENTER, COMMAND_CENTER_FLYING,
			ORBITAL, ORBITAL_FLYING, PLANETARY, NEXUS, HATCHERY, LAIR, HIVE })), target_pos) > 7)
			mediator->MarkArmyGroupForDeletion(this);
	}

	void ArmyGroup::DefendMainRamp()
	{
		for (int i = 0; i < new_units.size(); i++)
		{
			AddUnit(new_units[i]);
			i--;
		}
		Units enemy_units = mediator->GetUnits(Unit::Alliance::Enemy);

		// if supply > x, kill walloff gateway and move attack
		if (all_units.size() > 12 && 
			(enemy_units.size() == 0 ||
			mediator->JudgeFight(all_units, enemy_units, 0, 0, false) > 50 || 
			Utility::DistanceToClosest(enemy_units, target_pos) > 3))
		{
			Point2D walloff_pos = mediator->GetWallOffLocation(GATEWAY);
			const Unit* walloff_gate = Utility::ClosestUnitTo(mediator->GetUnits(Unit::Alliance::Self, IsUnits({ GATEWAY, WARP_GATE })), walloff_pos);
			if (Distance2D(walloff_pos, walloff_gate->pos) < 1)
			{
				for (const auto& unit : all_units)
				{
					if (Distance2D(unit->pos, walloff_pos) > 3)
						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, walloff_gate, 0);
					else
						mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK, walloff_gate, 0);
				}
			}
			else
			{
				mediator->CreateArmyGroup(ArmyRole::attack, { ADEPT, STALKER, SENTRY }, desired_units, max_units);
				mediator->MarkArmyGroupForDeletion(this);
				mediator->SetUnitsCommand(all_units, ABILITY_ID::ATTACK, mediator->GetEnemyStartLocation(), 1);
			}
			return;
		}

		// else 
		for (const auto& unit : all_units)
		{
			// if enemies get too close, kite away individually
			const Unit* closest_enemy = Utility::ClosestTo(enemy_units, unit->pos);
			if (Distance2D(closest_enemy->pos, unit->pos) < 3)
				mediator->SetUnitCommand(unit, ABILITY_ID::MOVE_MOVE, Utility::PointBetween(closest_enemy->pos, unit->pos, Distance2D(closest_enemy->pos, unit->pos) + 2), 1);
			else if (Distance2D(unit->pos, target_pos) > 6) // move units close to ramp
				mediator->SetUnitCommand(unit, ABILITY_ID::MOVE_MOVE, target_pos, 1);

			// attack enemies on ramp
			if (unit->weapon_cooldown == 0)
				mediator->AddUnitToAttackers(unit);
		}

		bool overcharge_active = false;
		for (const auto& battery : mediator->GetUnits(Unit::Alliance::Self, IsFinishedUnit(BATTERY)))
		{
			for (const auto& buff : battery->buffs)
			{
				if (buff == BUFF_ID::BATTERYOVERCHARGE)
				{
					overcharge_active = true;
					break;
				}
			}
		}

		// forcefield if necessary
		if (overcharge_active == false && 
			Utility::DistanceToClosest(mediator->GetUnits(IsUnit(UNIT_TYPEID::NEUTRAL_FORCEFIELD)), target_pos) > .1 &&
			enemy_units.size() > 2 &&
			Distance2D(Utility::NthClosestTo(enemy_units, target_pos, 2)->pos, target_pos) < 3)
		{
			Units available_sentries;
			bool forcefield_cast = false;
			for (const auto& sentry : sentries)
			{
				if (sentry->energy >= 50)
				{
					if (Distance2D(sentry->pos, target_pos) < 9)
					{
						mediator->SetUnitCommand(sentry, ABILITY_ID::EFFECT_FORCEFIELD, target_pos, 10);
						forcefield_cast = true;
						break;
					}
					else
					{
						available_sentries.push_back(sentry);
					}
				}
			}
			if (forcefield_cast == false && available_sentries.size() > 0)
			{
				mediator->SetUnitCommand(Utility::ClosestTo(available_sentries, target_pos), ABILITY_ID::EFFECT_FORCEFIELD, target_pos, 10);
			}
		}
	}

	void ArmyGroup::DefendCannonRush()
	{
		// if there are cannons pull workers against them
		std::map<const Unit*, Units> assigned_attackers;
		for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ PROBE, PYLON, CANNON })))
		{
			if (Distance2D(unit->pos, mediator->GetStartLocation()) < 30 || 
				Distance2D(unit->pos, mediator->GetNaturalLocation()) < 20)
				assigned_attackers[unit] = {};
		}

		if (mediator->GetCurrentTime() > 240 && assigned_attackers.size() == 0)
		{
			mediator->SetUnitsCommand(all_units, ABILITY_ID::STOP, 10);
			for (const auto& probe : all_units)
			{
				if (probe->unit_type == PROBE)
					mediator->PlaceWorker(probe);
			}
			mediator->MarkArmyGroupForDeletion(this);
		}

		Units available_probes;

		for (const auto& probe : all_units)
		{
			if (probe->orders.size() > 0 && probe->orders[0].ability_id == ABILITY_ID::ATTACK)
			{
				const Unit* probe_target = mediator->GetUnit(probe->orders[0].target_unit_tag);
				if (probe_target != nullptr && assigned_attackers.find(probe_target) != assigned_attackers.end())
					assigned_attackers[probe_target].push_back(probe);
				else
					available_probes.push_back(probe);
			}
			else
			{
				available_probes.push_back(probe);
			}
		}

		for (auto& enemy : assigned_attackers)
		{
			int needed_probes = 0;
			switch (enemy.first->unit_type.ToType())
			{
			case PROBE:
				needed_probes = 2;
				break;
			case CANNON:
				needed_probes = 3;
				break;
			case PYLON:
				needed_probes = 3;
				break;
			default:
				std::cerr << "Unknown unit found in ActionCheckBaseForCannons " << UnitTypeToName(enemy.first->unit_type) << std::endl;
				needed_probes = 0;
				break;
			}
			if (enemy.second.size() == needed_probes)
			{
				continue;
			}
			else if (enemy.second.size() < needed_probes)
			{
				while (enemy.second.size() < needed_probes)
				{
					if (available_probes.size() > 0)
					{
						const Unit* probe = available_probes.back();
						enemy.second.push_back(probe);
						available_probes.pop_back();
					}
					else
					{
						enemy.second.push_back(nullptr);
					}
				}
			}
			else if (enemy.second.size() > needed_probes)
			{
				while (enemy.second.size() > needed_probes)
				{
					const Unit* probe = enemy.second.back();
					available_probes.push_back(probe);
					enemy.second.pop_back();
				}
			}
		}

		for (auto& enemy : assigned_attackers)
		{
			for (auto& probe : enemy.second)
			{
				if (probe == nullptr)
				{
					if (available_probes.size() > 0)
					{
						const Unit* new_probe = available_probes.back();
						probe = new_probe;
						available_probes.pop_back();
					}
					else
					{
						const Unit* new_probe = mediator->GetBuilder(enemy.first->pos);
						if (new_probe == nullptr)
						{
							// no more probes available
							continue;
						}
						mediator->RemoveWorker(new_probe);
						AddUnit(new_probe);
						probe = new_probe;
					}
				}
				mediator->SetUnitCommand(probe, ABILITY_ID::ATTACK, enemy.first, 0);
			}
		}

		for (const auto& probe : available_probes)
		{
			if (probe->unit_type == PROBE)
			{
				RemoveUnit(probe);
				mediator->PlaceWorker(probe);
			}
		}
	}

	// returns: 0 - normal operation, 1 - all units dead or in standby, 2 - reached the end of the attack path
	int ArmyGroup::AttackLine(float dispersion, float target_range, std::vector<std::vector<UNIT_TYPEID>> target_priority, bool limit_advance)
	{
		if (mediator->GetEnemyRace() == Race::Zerg)
		{
			for (int i = 0; i < new_units.size(); i++)
			{
				const Unit* unit = new_units[i];
				if (unit->orders.size() == 0 || unit->orders[0].ability_id == ABILITY_ID::BUILD_INTERCEPTORS)
				{
					for (const auto& point : attack_path)
					{
						if (unit->unit_type == PRISM)
						{
							if (all_units.size() == 0)
								mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, point, 0, true);
							else
								mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(all_units), 0);
						}
						else
						{
							mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, point, 0, true);
						}
					}
				}
				if ((all_units.size() > 0 && Distance2D(unit->pos, Utility::MedianCenter(all_units)) < 5) || (all_units.size() == 0 && Utility::DistanceToClosest(attack_path, unit->pos) < 2))
				{
					if (state_machine)
					{
						if (state_machine->AddUnit(unit))
						{
							AddUnit(unit);
							i--;
						}
					}
					else
					{
						AddUnit(unit);
						i--;
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < new_units.size(); i++)
			{
				const Unit* unit = new_units[i];
				if (all_units.size() == 0)
				{
					if (Distance2D(attack_path[0], unit->pos) < 2)
					{
						if (state_machine)
						{
							if (state_machine->AddUnit(unit))
							{
								AddUnit(unit);
								i--;
							}
						}
						else
						{
							AddUnit(unit);
							i--;
						}
					}
					else
					{
						mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, attack_path[0], 0);
					}
				}
				if (all_units.size() > 0)
				{
					if (Distance2D(unit->pos, Utility::MedianCenter(all_units)) < 5)
					{
						if (state_machine)
						{
							if (state_machine->AddUnit(unit))
							{
								AddUnit(unit);
								i--;
							}
						}
						else
						{
							AddUnit(unit);
							i--;
						}
					}
					else if (unit->weapon_cooldown == 0)
					{
						if (unit->unit_type == PRISM)
							mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(all_units), 0);
						else
							mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, Utility::MedianCenter(all_units), 0);
					}
					else
					{
						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(all_units), 0);
					}
				}
			}
		}

		if (all_units.size() == 0)
		{
			if (new_units.size() == 0 && standby_units.size() == 0)
				mediator->MarkArmyGroupForDeletion(this);
			return 0;
		}

		// Find current units to micro
		Units basic_units;
		if (using_standby)
		{
			std::vector<UNIT_TYPEID> types = { STALKER, SENTRY, ADEPT, ARCHON, IMMORTAL };
			for (const auto& unit : all_units)
			{
				if (std::find(types.begin(), types.end(), unit->unit_type) != types.end() &&
					std::find(standby_units.begin(), standby_units.end(), unit) == standby_units.end())
					basic_units.push_back(unit);
			}
		}
		else
		{
			std::vector<UNIT_TYPEID> types = { STALKER, SENTRY, ADEPT, ARCHON, IMMORTAL };
			for (const auto& unit : all_units)
			{
				if (std::find(types.begin(), types.end(), unit->unit_type) != types.end())
					basic_units.push_back(unit);
			}
		}

		if (new_units.size() == 0 && basic_units.size() == 0 && standby_units.size() == 0)
		{
			mediator->MarkArmyGroupForDeletion(this);
			return 0;
		}

		OraclesDefendArmy(basic_units);

		if (static_cast<float>(basic_units.size()) / all_units.size() < .25)
			return 1;

		int return_value = 0;

		Point2D limit = attack_path_line.GetEndPoint();
		if (limit_advance)
		{
			limit = FindLimitToAdvance();
		}

		mediator->agent->Debug()->DebugSphereOut(mediator->ToPoint3D(limit), 1.5, Color(255, 255, 255));

		if (FindUnitPositions(basic_units, warp_prisms, dispersion, target_range, attack_path_line.GetPointFrom(limit, 1, false)))
			return_value = 2;

		/*for (const auto& pos : agent->locations->attack_path_line.GetPoints())
		{
			agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .5, Color(255, 255, 255));
		}
		for (const auto& pos : unit_position_asignments)
		{
			agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos.second), .625, Color(255, 0, 0));
		}*/


		// Find units that can attack and those that cant
		Units units_ready;
		Units units_not_ready;
		FindReadyUnits(basic_units, units_ready, units_not_ready);

		float percent_units_needed = .25;

		//if units are shooting then theres a reason to stay
		if ((double)units_not_ready.size() / (double)basic_units.size() > percent_units_needed)
			return_value = 0;


			
		
		MicroReadyUnits(units_ready, target_priority, percent_units_needed, (int)basic_units.size());
		std::vector<std::pair<const Unit*, UnitDanger>> units_requesting_pickup = MicroNonReadyUnits(units_not_ready);

		for (const auto& request : units_requesting_pickup)
		{
			std::stringstream str;
			str << request.second.unit_prio << ": " << std::fixed << std::setprecision(1) << request.second.damage_value;
			//agent->Debug()->DebugTextOut(str.str(), request.first->pos, Color(0, 255, 0), 14);
		}

		MicroWarpPrisms(units_requesting_pickup);


		if (using_standby)
		{
			for (int i = 0; i < standby_units.size(); i++)
			{
				bool in_cargo = false;
				for (const auto& cargo : units_in_cargo)
				{
					if (cargo.first == standby_units[i])
					{
						in_cargo = true;
						break;
					}
				}
				if (!in_cargo)
					mediator->SetUnitCommand(standby_units[i], ABILITY_ID::GENERAL_MOVE, standby_pos, 0);

				if (standby_units[i]->shield == standby_units[i]->shield_max)
				{
					standby_units.erase(standby_units.begin() + i);
					i--;
				}
			}
		}
		return return_value;
	}

	// returns: 0 - normal operation, 1 - all units dead or in standby, 2 - reached the end of the attack path
	int ArmyGroup::AttackLine(Units units, float dispersion, float target_range, std::vector<std::vector<UNIT_TYPEID>> target_priority, bool limit_advance)
	{
		if (mediator->GetEnemyRace() == Race::Zerg)
		{
			for (int i = 0; i < new_units.size(); i++)
			{
				const Unit* unit = new_units[i];
				if (unit->orders.size() == 0 || unit->orders[0].ability_id == ABILITY_ID::BUILD_INTERCEPTORS)
				{
					for (const auto& point : attack_path)
					{
						if (unit->unit_type == PRISM)
						{
							if (units.size() == 0)
								mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, point, 0, true);
							else
								mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(units), 0);
						}
						else
						{
							mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, point, 0, true);
						}
					}
				}
				if ((units.size() > 0 && Distance2D(unit->pos, Utility::MedianCenter(units)) < 5) || (units.size() == 0 && Utility::DistanceToClosest(attack_path, unit->pos) < 2))
				{
					if (state_machine)
					{
						if (state_machine->AddUnit(unit))
						{
							AddUnit(unit);
							i--;
						}
					}
					else
					{
						AddUnit(unit);
						i--;
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < new_units.size(); i++)
			{
				const Unit* unit = new_units[i];
				if (units.size() == 0)
				{
					if (Distance2D(attack_path[0], unit->pos) < 2)
					{
						if (state_machine)
						{
							if (state_machine->AddUnit(unit))
							{
								AddUnit(unit);
								i--;
							}
						}
						else
						{
							AddUnit(unit);
							i--;
						}
					}
					else
					{
						mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, attack_path[0], 0);
					}
				}
				if (units.size() > 0)
				{
					if (Distance2D(unit->pos, Utility::MedianCenter(units)) < 5)
					{
						if (state_machine)
						{
							if (state_machine->AddUnit(unit))
							{
								AddUnit(unit);
								i--;
							}
						}
						else
						{
							AddUnit(unit);
							i--;
						}
					}
					else if (unit->weapon_cooldown == 0)
					{
						if (unit->unit_type == PRISM)
							mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(units), 0);
						else
							mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, Utility::MedianCenter(units), 0);
					}
					else
					{
						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(units), 0);
					}
				}
			}
		}

		if (units.size() == 0)
		{
			if (new_units.size() == 0 && standby_units.size() == 0)
				mediator->MarkArmyGroupForDeletion(this);
			return 0;
		}

		// Find current units to micro
		Units basic_units;
		if (using_standby)
		{
			std::vector<UNIT_TYPEID> types = { STALKER, SENTRY, ADEPT, ARCHON, IMMORTAL };
			for (const auto& unit : units)
			{
				if (std::find(types.begin(), types.end(), unit->unit_type) != types.end() &&
					std::find(standby_units.begin(), standby_units.end(), unit) == standby_units.end())
					basic_units.push_back(unit);
			}
		}
		else
		{
			std::vector<UNIT_TYPEID> types = { STALKER, SENTRY, ADEPT, ARCHON, IMMORTAL };
			for (const auto& unit : units)
			{
				if (std::find(types.begin(), types.end(), unit->unit_type) != types.end())
					basic_units.push_back(unit);
			}
		}

		if (new_units.size() == 0 && basic_units.size() == 0 && standby_units.size() == 0)
		{
			mediator->MarkArmyGroupForDeletion(this);
			return 0;
		}

		OraclesDefendArmy(basic_units);

		if (static_cast<float>(basic_units.size()) / units.size() < .25)
			return 1;

		int return_value = 0;

		Point2D limit = attack_path_line.GetEndPoint();
		if (limit_advance)
		{
			limit = FindLimitToAdvance();
		}

		mediator->agent->Debug()->DebugSphereOut(mediator->ToPoint3D(limit), 1.5, Color(255, 255, 255));

		if (FindUnitPositions(basic_units, warp_prisms, dispersion, target_range, attack_path_line.GetPointFrom(limit, 1, false)))
			return_value = 2;

		/*for (const auto& pos : agent->locations->attack_path_line.GetPoints())
		{
			agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos), .5, Color(255, 255, 255));
		}
		for (const auto& pos : unit_position_asignments)
		{
			agent->Debug()->DebugSphereOut(agent->ToPoint3D(pos.second), .625, Color(255, 0, 0));
		}*/


		// Find units that can attack and those that cant
		Units units_ready;
		Units units_not_ready;
		FindReadyUnits(basic_units, units_ready, units_not_ready);

		float percent_units_needed = .25;

		//if units are shooting then theres a reason to stay
		if ((double)units_not_ready.size() / (double)basic_units.size() > percent_units_needed)
			return_value = 0;




		MicroReadyUnits(units_ready, target_priority, percent_units_needed, (int)basic_units.size());
		std::vector<std::pair<const Unit*, UnitDanger>> units_requesting_pickup = MicroNonReadyUnits(units_not_ready);

		for (const auto& request : units_requesting_pickup)
		{
			std::stringstream str;
			str << request.second.unit_prio << ": " << std::fixed << std::setprecision(1) << request.second.damage_value;
			//agent->Debug()->DebugTextOut(str.str(), request.first->pos, Color(0, 255, 0), 14);
		}

		MicroWarpPrisms(units_requesting_pickup);


		if (using_standby)
		{
			for (int i = 0; i < standby_units.size(); i++)
			{
				bool in_cargo = false;
				for (const auto& cargo : units_in_cargo)
				{
					if (cargo.first == standby_units[i])
					{
						in_cargo = true;
						break;
					}
				}
				if (!in_cargo)
					mediator->SetUnitCommand(standby_units[i], ABILITY_ID::GENERAL_MOVE, standby_pos, 0);

				if (standby_units[i]->shield == standby_units[i]->shield_max)
				{
					standby_units.erase(standby_units.begin() + i);
					i--;
				}
			}
		}
		return return_value;
	}

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

		Units enemy_burrowed_units = mediator->GetUnits(IsUnits(Utility::GetBurrowedUnitTypes()));

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
					mediator->SetUnitCommand(highest_over_75, ABILITY_ID::EFFECT_ORACLEREVELATION, unit_to_revelate->pos, 0);
					//agent->Debug()->DebugSphereOut(highest_over_75->pos, 2, Color(255, 0, 0));

				}
				else if (lowest_over_25 != nullptr)
				{
					mediator->SetUnitCommand(lowest_over_25, ABILITY_ID::EFFECT_ORACLEREVELATION, unit_to_revelate->pos, 0);
					//agent->Debug()->DebugSphereOut(lowest_over_25->pos, 2, Color(255, 0, 0));
				}
			}
		}


		Units enemy_lings = mediator->GetUnits(IsUnit(UNIT_TYPEID::ZERG_ZERGLING));
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
							mediator->SetUnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF, 0);
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
							mediator->SetUnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMON, 0);
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
						mediator->SetUnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF, 0);
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
				mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, center, 0);
				continue;
			}
			float now = mediator->GetGameLoop() / FRAME_TIME;
			bool weapon_ready = now - time_last_attacked[oracle] > .8; //.61

			/*agent->Debug()->DebugTextOut("weapon ready " + std::to_string(weapon_ready), Point2D(.2, .35), Color(0, 255, 0), 20);
			agent->Debug()->DebugTextOut("has attacked " + std::to_string(state_machine->has_attacked[oracle]), Point2D(.2, .37), Color(0, 255, 0), 20);
			agent->Debug()->DebugTextOut("target " + std::to_string(state_machine->target[oracle]), Point2D(.2, .39), Color(0, 255, 0), 20);*/


			if (weapon_ready)
			{
				const Unit* closest_unit = Utility::ClosestTo(enemy_lings, oracle->pos);
				if (closest_unit == nullptr || Distance2D(closest_unit->pos, oracle->pos) > 6)
				{
					mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, center, 0);
					continue;
				}


				mediator->SetUnitCommand(oracle, ABILITY_ID::ATTACK_ATTACK, closest_unit, 0);
				//agent->Debug()->DebugSphereOut(closest_unit->pos, .75, Color(0, 255, 255));

				target[oracle] = closest_unit->tag;
				time_last_attacked[oracle] = mediator->GetGameLoop() / FRAME_TIME;
				has_attacked[oracle] = false;
				//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
			}
			else if (has_attacked[oracle])
			{
				mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, center, 0);

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
			/*if (Distance2D(concave_origin, concave_target) > Distance2D(new_origin, concave_target))
				return reached_end;*/

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
			/*if (Distance2D(concave_origin, concave_target) < Distance2D(new_origin, concave_target))
				return reached_end;*/

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

	void ArmyGroup::MicroReadyUnits(Units units, std::vector<std::vector<UNIT_TYPEID>> target_priority, float percent_needed, int total_units)
	{
		// if enough units can attack something
		if (static_cast<float>(units.size()) / static_cast<float>(total_units) >= percent_needed)
		{
			// find targets for each unit
			mediator->AddUnitsToAttackers(units);
			for (const auto& unit : units)
			{
				if (unit_position_asignments.find(unit) != unit_position_asignments.end())
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit_position_asignments[unit], 0);
				else
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit->pos, 0);
			}
		}
		else
		{
			for (const auto& unit : units)
			{
				if (unit_position_asignments.find(unit) != unit_position_asignments.end())
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit_position_asignments[unit], 0);
				else
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit->pos, 0);
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
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit_position_asignments[unit], 0);
				else
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit->pos, 0);


				if (using_standby)
				{
					if (unit->shield == 0 || (unit->shield + unit->health) / (unit->shield_max + unit->health_max) < .3) // TODO this threshold should be passed in
					{
						standby_units.push_back(unit);

						if (standby_pos == Point2D(0, 0))
							mediator->SetUnitCommand(unit, ABILITY_ID::EFFECT_BLINK,
								Utility::PointBetween(unit->pos, Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), unit->pos)->pos, -4), 2);
						else
							mediator->SetUnitCommand(unit, ABILITY_ID::EFFECT_BLINK, standby_pos, 2); // TODO adjustable blink distance

						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, standby_pos, 0, true);
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
				mediator->SetUnitCommand(prism, ABILITY_ID::GENERAL_MOVE, unit_position_asignments[prism], 0);
			else
				mediator->SetUnitCommand(prism, ABILITY_ID::GENERAL_MOVE, prism->pos, 0);
			mediator->SetUnitCommand(prism, ABILITY_ID::UNLOADALLAT, prism, 0);
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
				mediator->SetUnitCommand(request.first, ABILITY_ID::SMART, units_in_cargo[request.first].prism, 0);
				continue;
			}
			for (const auto& prism : warp_prisms)
			{
				int size = Utility::GetCargoSize(request.first);
				int necessary_space = std::max(size, request.second.unit_prio * 2);
				if (8 - (prism_cargo[prism]) >= necessary_space)
				{
					unit_found_space = true;
					mediator->SetUnitCommand(request.first, ABILITY_ID::SMART, prism, 0);
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
							mediator->SetUnitCommand(request.first, ABILITY_ID::EFFECT_BLINK,
								Utility::PointBetween(request.first->pos, Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), request.first->pos)->pos, -4), 2);
						else
							mediator->SetUnitCommand(request.first, ABILITY_ID::EFFECT_BLINK, standby_pos, 2); // TODO adjustable blink distance

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

	void ArmyGroup::OnNewUnitCreatedListener(const Unit* unit)
	{
		if (std::find(unit_types.begin(), unit_types.end(), unit->unit_type) != unit_types.end())
			AddNewUnit(unit);
	}

	void ArmyGroup::AutoAddUnits(std::vector<UNIT_TYPEID> types)
	{
		std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
			this->OnUnitCreatedListener(unit);
		};
		mediator->agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);
		this->unit_types = types;
	}

#pragma warning(push)
#pragma warning(disable : 4100)
	void ArmyGroup::OnUnitCreatedListener(const Unit* unit)
	{
		
	}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4100)
	void ArmyGroup::OnUnitDamagedListener(const Unit* unit, float health_damage, float shields_damage)
	{
		for (const auto& oracle : oracles)
		{
			if (target[oracle] == unit->tag)
			{
				//agent->Debug()->DebugTextOut(UnitTypeToName(unit->unit_type.ToType()) + " took " + std::to_string(health) + " damage from orale", Point2D(.2, .4 + .02 * i), Color(0, 255, 0), 20);
				//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage from orale\n";
				has_attacked[oracle] = true;
			}
		}
	}
#pragma warning(pop)

	void ArmyGroup::OnUnitDestroyedListener(const Unit* unit)
	{
		for (const auto& oracle : oracles)
		{
			if (target[oracle] == unit->tag)
			{
				//agent->Debug()->DebugTextOut(UnitTypeToName(unit->unit_type.ToType()) + " desroyed by oracle", Point2D(.2, .45 + .02 * i), Color(0, 255, 0), 20);
				//std::cout << UnitTypeToName(unit->unit_type.ToType()) << " destroyed by orale\n";
				has_attacked[oracle] = true;
			}
		}
		if (state_machine)
			state_machine->RemoveUnit(unit);

		if (std::find(all_units.begin(), all_units.end(), unit) != all_units.end())
			RemoveUnit(unit);
		if (std::find(new_units.begin(), new_units.end(), unit) != new_units.end())
			RemoveUnit(unit);
	}

}