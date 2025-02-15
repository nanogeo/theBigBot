#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"

#include "army_group.h"
#include "theBigBot.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {

	ArmyGroup::ArmyGroup(Mediator* mediator, PathManager attack_line, std::vector<Point2D> attack_path, ArmyRole role, std::vector<UNIT_TYPEID> unit_types) : persistent_fire_control(mediator->agent)
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

	ArmyGroup::ArmyGroup(Mediator* mediator, std::vector<Point2D> attack_path, ArmyRole role, std::vector<UNIT_TYPEID> unit_types) : persistent_fire_control(mediator->agent)
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

	ArmyGroup::ArmyGroup(Mediator* mediator, ArmyRole role, std::vector<UNIT_TYPEID> unit_types) : persistent_fire_control(mediator->agent)
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

	ArmyGroup::ArmyGroup(Mediator* mediator, Point2D defense_point, ArmyRole role, std::vector<UNIT_TYPEID> unit_types) : persistent_fire_control(mediator->agent)
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

		this->defense_point = defense_point;
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
			time_last_attacked[unit] = 0;
			has_attacked[unit] = true;
			is_beam_active[unit] = false;
			casting[unit] = false;
			casting_energy[unit] = 0;
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
		all_units.erase(std::remove(all_units.begin(), all_units.end(), unit), all_units.end());
		new_units.erase(std::remove(new_units.begin(), new_units.end(), unit), new_units.end());
		attack_status.erase(unit);

		Units* units;
		switch (unit->unit_type.ToType())
		{
		case ZEALOT:
			units = &zealots;
			break;
		case STALKER:
		{
			units = &stalkers; // TODO why do i need this
			auto index = std::find(stalkers.begin(), stalkers.end(), unit);
			if (index == stalkers.end())
			{
				//std::cout << "Error trying to remove stalker not in stalkers in RemoveUnit\n";
			}
			else
			{
				stalkers.erase(std::remove(stalkers.begin(), stalkers.end(), unit), stalkers.end());
				if (last_time_blinked.count(unit))
					last_time_blinked.erase(unit);
			}
			break;
		}
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
		default:
			std::cerr << "Error unknown unit type in ArmyGroup::RemoveUnit" << std::endl;
			return;
		}

		units->erase(std::remove(units->begin(), units->end(), unit), units->end());
	}

	Units ArmyGroup::GetExtraUnits()
	{
		Units extra_units;
		int extra_num = all_units.size() + new_units.size() - desired_units;
		if (extra_num <= 0)
			return extra_units;

		for (int i = new_units.size() - 1; i >= 0; i --)
		{
			extra_units.push_back(new_units[i]);
			if (extra_units.size() == extra_num)
				return extra_units;
		}
		for (int i = all_units.size() - 1; i >= 0; i--)
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
		}
		return concave_points;
	}

	std::vector<Point2D> ArmyGroup::FindConcaveWithPrism(Point2D origin, Point2D fallback_point, int num_units, int num_prisms, float unit_size, float dispersion, float concave_degree, std::vector<Point2D>& prism_positions)
	{
		float min_height = mediator->ToPoint3D(origin).z - .5;
		float max_height = min_height + 1;
		float height = mediator->ToPoint3D(origin).z;
		float range = 0; //r
		float unit_radius = unit_size + dispersion; //u
		//float concave_degree = 30; //p
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
						if (point_height + .5 > max_height)
							max_height = point_height + .5;
						if (point_height - .5 < min_height)
							min_height = point_height - .5;
						if (Utility::DistanceToClosest(mediator->agent->corrosive_bile_positions, unit_position) > .5 + unit_size + .1)
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
						if (point_height + .5 > max_height)
							max_height = point_height + .5;
						if (point_height - .5 < min_height)
							min_height = point_height - .5;
						if (Utility::DistanceToClosest(mediator->agent->corrosive_bile_positions, unit_position) > .5 + unit_size + .1)
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

				arclength = (2 * unit_radius * ((6 / num_prisms) - .5)) / (range + concave_degree + (((row * 2) - 1) * unit_radius));
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
				if (point_height + .5 > max_height)
					max_height = point_height + .5;
				if (point_height - .5 < min_height)
					min_height = point_height - .5;
				if (Utility::DistanceToClosest(mediator->agent->corrosive_bile_positions, unit_position) > .5 + unit_size + .1)
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
					float point_height = mediator->ToPoint3D(unit_position).z;
					if (mediator->IsPathable(unit_position) && point_height > min_height && point_height < max_height)
					{
						if (point_height + .5 > max_height)
							max_height = point_height + .5;
						if (point_height - .5 < min_height)
							min_height = point_height - .5;
						if (Utility::DistanceToClosest(mediator->agent->corrosive_bile_positions, unit_position) > .5 + unit_size + .1)
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
						if (point_height + .5 > max_height)
							max_height = point_height + .5;
						if (point_height - .5 < min_height)
							min_height = point_height - .5;
						if (Utility::DistanceToClosest(mediator->agent->corrosive_bile_positions, unit_position) > .5 + unit_size + .1)
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

				arclength = (2 * unit_radius * ((6 / num_prisms) - .5)) / (range + concave_degree + (((row * 2) - 1) * unit_radius));
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

				attack_status[unit] = false;
				if (unit->orders.size() > 0 && unit->orders[0].ability_id == ABILITY_ID::ATTACK && unit->weapon_cooldown == 0)
					mediator->SetUnitCommand(unit, ABILITY_ID::SMART, prism.first, true);
				else
					mediator->SetUnitCommand(unit, ABILITY_ID::SMART, prism.first);
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
				bool blink_ready = false;
				for (const auto &abiliy : mediator->agent->Query()->GetAbilitiesForUnit(Funit).abilities)
				{
					if (abiliy.ability_id == ABILITY_ID::EFFECT_BLINK)
					{
						blink_ready = true;
						break;
					}
				}
				if (blink_ready && (danger > Funit->shield || danger > (Funit->shield_max / 2)))
				{
					mediator->SetUnitCommand(Funit, ABILITY_ID::EFFECT_BLINK, Funit->pos + Point2D(0, 4));
					mediator->SetUnitCommand(Funit, ABILITY_ID::ATTACK, Funit->pos - Point2D(0, 4), true);
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
			max_range = std::max(Utility::GetMaxRange(close_enemies) + 2, 6.0f);
		}

		Point2D retreat_concave_origin = attack_path_line.GetPointFrom(concave_target, max_range, false);
		if (retreat_concave_origin == Point2D(0, 0))
			retreat_concave_origin = attack_path_line.GetPointFrom(stalker_line_pos, 2 * unit_size + unit_dispersion, false);

		Point2D attack_concave_origin = attack_path_line.GetPointFrom(stalker_line_pos, 2 * unit_size + unit_dispersion, true);



		std::vector<Point2D> attack_concave_positions = FindConcaveFromBack(attack_concave_origin, (2 * attack_concave_origin) - concave_target, stalkers.size(), .625, unit_dispersion);
		std::vector<Point2D> retreat_concave_positions = FindConcave(retreat_concave_origin, (2 * retreat_concave_origin) - concave_target, stalkers.size(), .625, unit_dispersion, 30);

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
		if (enemies.size() == 0)
		{
			mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_MOVE, door_open_pos);
			return;
		}

		const Unit* closest_to_door = Utility::ClosestTo(enemies, door_closed_pos);
		double dist_to_closest = Distance2D(closest_to_door->pos, door_closed_pos);

		Point2D guard_move_to = Utility::PointBetween(door_closed_pos, closest_to_door->pos, std::min(4.0, std::max(0.0, (dist_to_closest / 2) - 1)));


		// TODO use fire control to find the best target
		const Unit* closest_to_guard = Utility::ClosestTo(enemies, guard->pos);
		if (Distance2D(closest_to_guard->pos, guard->pos) < Utility::RealGroundRange(guard, closest_to_guard))
		{
			// enemy in range
			if (Distance2D(guard->pos, guard_move_to) > .5)
			{
				mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_MOVE, guard_move_to);
			}
			else
			{
				mediator->SetUnitCommand(guard, ABILITY_ID::ATTACK_ATTACK, closest_to_guard);
			}
		}
		else
		{
			mediator->SetUnitCommand(guard, ABILITY_ID::GENERAL_MOVE, guard_move_to);
		}
	}

	void ArmyGroup::DefendThirdBase(Point2D third_base_pylon_gap)
	{
		if (new_units.size() > 0)
		{
			for (const auto& unit : new_units)
			{
				AddUnit(unit);
			}
		}
		for (const auto& unit : all_units)
		{
			if (Distance2D(unit->pos, third_base_pylon_gap) > 1)
			{
				mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, third_base_pylon_gap);
				mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_HOLDPOSITION, true);
			}
		}

		// TODO use fire control to find the best target
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
		for (const auto &tempest : tempests)
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
		}

		if (units_ready.size() > 0)
		{
			persistent_fire_control.UpdateEnemyUnitHealth();
			std::map<const Unit*, const Unit*> attacks = persistent_fire_control.FindAttacks(units_ready, 
				{ {SIEGE_TANK_SIEGED}, //TODO redo this list
					{SIEGE_TANK},
					{WIDOW_MINE},
					{CYCLONE},
					{VIKING},
					{RAVEN},
					{MEDIVAC},
					{GHOST},
					{MARINE, MARAUDER, REAPER, HELLION, HELLBAT, THOR_AP, THOR_AOE, VIKING_LANDED, BANSHEE, BATTLECRUISER, LIBERATOR, LIBERATOR_SIEGED},
					{SCV, MULE},
					{BUNKER},
					{PLANETARY},
					{MISSILE_TURRET},
					{BARRACKS_REACTOR, FACTORY_REACTOR, STARPORT_REACTOR},
					{BARRACKS_TECH_LAB, FACTORY_TECH_LAB, STARPORT_TECH_LAB},
					{REACTOR, TECH_LAB} }, 0);
			for (const auto& unit : units_ready)
			{
				if (attacks.size() == 0)
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, attack_pos);
				}
				if (attacks.count(unit) > 0)
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK, attacks[unit]);
					attack_status[unit] = true;
					//agent->Debug()->DebugLineOut(unit->pos, attacks[unit]->pos, Color(0, 255, 0));
				}
			}
			mediator->agent->PrintAttacks(attacks);
		}

		/*for (const auto& unit : persistent_fire_control.enemy_unit_hp)
		{
			agent->Debug()->DebugTextOut(std::to_string(unit.second), unit.first->pos, Color(0, 255, 0), 15);
		}

		for (const auto& unit : units_on_cd)
		{
			mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, retreat_pos);
		}*/
	}

	void ArmyGroup::ScourMap()
	{
		ImageData raw_map = mediator->GetPathingGrid();
		for (const auto& unit : mediator->GetUnits(IsFightingUnit(Unit::Alliance::Self)))
		{
			if (unit->orders.size() == 0)
			{
				std::srand(unit->tag + mediator->GetGameLoop());
				int x = std::rand() % raw_map.width;
				int y = std::rand() % raw_map.height;
				Point2D pos = Point2D(x, y);
				while (!unit->is_flying && !mediator->IsPathable(pos))
				{
					x = std::rand() % raw_map.width;
					y = std::rand() % raw_map.height;
					pos = Point2D(x, y);
				}
				mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK, pos);
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
		for (const auto& unit : all_units)
		{
			if (unit->orders.size() == 0)
			{
				for (const auto& point : attack_path)
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, point, true);
				}
			}
		}
	}

	void ArmyGroup::DefendLocation()
	{
		for (int i = 0; i < new_units.size(); i++)
		{
			mediator->SetUnitCommand(new_units[i], ABILITY_ID::ATTACK_ATTACK, defense_point);
			if (Distance2D(new_units[i]->pos, defense_point) <= 10)
			{
				AddUnit(new_units[i]);
				i--;
			}
		}
		for (const auto &unit : all_units)
		{
			// find closest enemy to unit and defense point
			// if within 10 of defense point and weapon off cooldown then attack unit
			// else move back to defense point
			if (unit->weapon_cooldown > 0 || Distance2D(unit->pos, defense_point) > 10)
			{
				mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, defense_point);
			}
			else
			{
				Units enemy_units = Utility::CloserThan(mediator->GetUnits(Unit::Alliance::Enemy), 20, defense_point);
				if (enemy_units.size() == 0)
					continue;
				Point2D defense_pos = defense_point;
				std::sort(enemy_units.begin(), enemy_units.end(),
					[&unit, &defense_pos](const Unit* a, const Unit* b) -> bool
				{
					return Distance2D(unit->pos, a->pos) + Distance2D(defense_pos, a->pos) <
						Distance2D(unit->pos, b->pos) + Distance2D(defense_pos, b->pos);
				});

				mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, enemy_units[0]);
			}
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

	// returns: 0 - normal operation, 1 - all units dead or in standby, 2 - reached the end of the attack path
	int ArmyGroup::AttackLine(float dispersion, float target_range, std::vector<std::vector<UNIT_TYPEID>> target_priority)
	{
		for (int i = 0; i < new_units.size(); i++)
		{
			const Unit* unit = new_units[i];
			if (unit->orders.size() == 0 || unit->orders[0].ability_id == ABILITY_ID::BUILD_INTERCEPTORS)
			{
				for (const auto& point : attack_path)
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, point, true);
				}
			}
			if ((all_units.size() > 0 && Distance2D(unit->pos, Utility::MedianCenter(all_units)) < 5) || (all_units.size() == 0 && Distance2D(unit->pos, attack_path[0]) < 2))
			{
				AddUnit(unit);
				i--;
			}
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

		OraclesDefendArmy(basic_units);

		if (static_cast<float>(basic_units.size()) / all_units.size() < .25)
			return 1;

		int return_value = 0;

		if (FindUnitPositions(basic_units, warp_prisms, dispersion, target_range))
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


			
		
		MicroReadyUnits(units_ready, target_priority, percent_units_needed, basic_units.size());
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
					mediator->SetUnitCommand(standby_units[i], ABILITY_ID::GENERAL_MOVE, standby_pos);

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
			if (casting[oracle])
			{
				revelation_cast = true;
				break;
			}
		}
		// revelate when units are burrowing
		if (!revelation_cast)
		{
			const Unit* unit_to_revelate = NULL;
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
			if (unit_to_revelate != NULL)
			{
				const Unit* highest_over_75 = NULL;
				const Unit* lowest_over_25 = NULL;
				for (const auto& oracle : oracles)
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
					mediator->SetUnitCommand(highest_over_75, ABILITY_ID::EFFECT_ORACLEREVELATION, unit_to_revelate->pos);
					casting[highest_over_75] = true;
					casting_energy[highest_over_75] = highest_over_75->energy;
					//agent->Debug()->DebugSphereOut(highest_over_75->pos, 2, Color(255, 0, 0));

				}
				else if (lowest_over_25 != NULL)
				{
					mediator->SetUnitCommand(lowest_over_25, ABILITY_ID::EFFECT_ORACLEREVELATION, unit_to_revelate->pos);
					casting[lowest_over_25] = true;
					casting_energy[lowest_over_25] = lowest_over_25->energy;
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
			float now = mediator->GetGameLoop() / 22.4;
			for (const auto& last_blink_time : last_time_blinked)
			{
				if (now - last_blink_time.second > 7)
					num_stalkers_with_blink++;
			}
			float percent_stalkers_with_blink = 1;
			if (last_time_blinked.size() > 0)
				percent_stalkers_with_blink = static_cast<float>(num_stalkers_with_blink) / static_cast<float>(last_time_blinked.size());

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
			for (const auto& beam_active : is_beam_active)
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
				Units oracles = Units(oracles);
				std::sort(oracles.begin(), oracles.end(), [](const Unit*& a, const Unit*& b) -> bool
				{
					return a->energy > b->energy;
				});
				for (const auto& oracle : oracles)
				{
					if (num_oracles_active == num_oracles_needed)
						break;
					if (oracle->energy > 10 && Utility::DistanceToClosest(enemy_lings, oracle->pos) > 5)
					{
						if (is_beam_active.count(oracle) && is_beam_active[oracle] == true)
						{
							is_beam_active[oracle] = false;
							mediator->SetUnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
							num_oracles_active--;
						}
					}
				}
			}
			else if (num_oracles_active < num_oracles_needed) // activate more oracles
			{
				Units oracles = Units(oracles);
				std::sort(oracles.begin(), oracles.end(), [](const Unit*& a, const Unit*& b) -> bool
				{
					return a->energy < b->energy;
				});
				for (const auto& oracle : oracles)
				{
					if (num_oracles_active == num_oracles_needed)
						break;
					if (oracle->energy > 40)
					{
						if (is_beam_active.count(oracle) && is_beam_active[oracle] == false)
						{
							is_beam_active[oracle] = true;
							mediator->SetUnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMON);
							num_oracles_active++;
						}
					}
				}
			}
		}
		else
		{
			Units oracles = Units(oracles);
			std::sort(oracles.begin(), oracles.end(), [](const Unit*& a, const Unit*& b) -> bool
			{
				return a->energy > b->energy;
			});
			for (const auto& oracle : oracles)
			{
				if (oracle->energy > 10 && (enemy_lings.size() == 0 || Utility::DistanceToClosest(enemy_lings, oracle->pos)))
				{
					if (is_beam_active.count(oracle) && is_beam_active[oracle] == true)
					{
						is_beam_active[oracle] = false;
						mediator->SetUnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
					}
				}
			}
		}
		// add oracle to volley or agnore units targetted in volley?
		// add event listeners for oracle
		for (const auto& oracle : oracles)
		{
			if (casting[oracle])
			{
				if (casting_energy[oracle] > oracle->energy || casting_energy[oracle] + 5 < oracle->energy)
				{
					casting[oracle] = false;
				}
				else
				{
					continue;
				}
			}
			if (is_beam_active[oracle] == false)
			{
				mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, center);
				continue;
			}
			float now = mediator->GetGameLoop() / 22.4;
			bool weapon_ready = now - time_last_attacked[oracle] > .8; //.61

			/*agent->Debug()->DebugTextOut("weapon ready " + std::to_string(weapon_ready), Point2D(.2, .35), Color(0, 255, 0), 20);
			agent->Debug()->DebugTextOut("has attacked " + std::to_string(state_machine->has_attacked[oracle]), Point2D(.2, .37), Color(0, 255, 0), 20);
			agent->Debug()->DebugTextOut("target " + std::to_string(state_machine->target[oracle]), Point2D(.2, .39), Color(0, 255, 0), 20);*/


			if (weapon_ready)
			{
				const Unit* closest_unit = Utility::ClosestTo(enemy_lings, oracle->pos);
				if (closest_unit == NULL || Distance2D(closest_unit->pos, oracle->pos) > 6)
				{
					mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, center);
					continue;
				}


				mediator->SetUnitCommand(oracle, ABILITY_ID::ATTACK_ATTACK, closest_unit);
				//agent->Debug()->DebugSphereOut(closest_unit->pos, .75, Color(0, 255, 255));

				target[oracle] = closest_unit->tag;
				time_last_attacked[oracle] = mediator->GetGameLoop() / 22.4;
				has_attacked[oracle] = false;
				//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
			}
			else if (has_attacked[oracle])
			{
				mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, center);

				//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 0, 255));
			}
			else
			{
				//agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
			}
		}
		// update beam status for tired oracles
		for (const auto& oracle : oracles)
		{
			if (oracle->energy <= 2)
				is_beam_active[oracle] = false;
		}
	}

	bool ArmyGroup::FindUnitPositions(Units units, Units prisms, float dispersion, float max_range)
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
		if (Distance2D(concave_origin, concave_target) < target_range - .1 || (close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, concave_origin) < 2 + unit_size))
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
			// if new origin is further away then ignore it
			if (Distance2D(concave_origin, concave_target) < Distance2D(new_origin, concave_target))
				return reached_end;

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
		std::vector<Point2D> concave_positions = FindConcaveWithPrism(concave_origin, (2 * concave_origin) - concave_target, units.size(), prisms.size(), unit_size, dispersion, concave_degree, prism_positions);

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
			if (unit->weapon_cooldown == 0 && attack_status[unit] == false)
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
			std::map<const Unit*, const Unit*> found_targets = persistent_fire_control.FindAttacks(units, target_priority, 0);
			for (const auto& unit : units)
			{
				if (found_targets.count(unit) > 0)
				{
					mediator->SetUnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, found_targets[unit]);
					if (attack_status[unit])
					{
						//std::cout << unit->tag << " second attack order given" << std::endl;
					}
					attack_status[unit] = true;
				}
				else // if a unit has no target, keep advancing
				{
					if (unit_position_asignments.find(unit) != unit_position_asignments.end())
						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit_position_asignments[unit]);
					else
						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit->pos);
				}
			}
		}
		else
		{
			for (const auto& unit : units)
			{
				if (unit_position_asignments.find(unit) != unit_position_asignments.end())
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit_position_asignments[unit]);
				else
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit->pos);
			}
		}
	}

	std::vector<std::pair<const Unit*, UnitDanger>> ArmyGroup::MicroNonReadyUnits(Units units)
	{
		std::vector<std::pair<const Unit*, UnitDanger>> incoming_damage;
		for (const auto& unit : units)
		{
			if (attack_status[unit] == false)
			{
				float damage = mediator->agent->IncomingDamage(unit);
				//agent->Debug()->DebugTextOut(std::to_string(damage), unit->pos, Color(255, 255, 255), 16);
				if (damage > 0)
				{
					float shield_damage = std::min(damage, unit->shield);
					float health_damage = damage - shield_damage;
					float total_damage = shield_damage + ((health_damage / unit->health) * unit->health_max * 1.5);
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
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit_position_asignments[unit]);
				else
					mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, unit->pos);


				if (using_standby)
				{
					if ((unit->shield + unit->health) / (unit->shield_max + unit->health_max) < .3) // TODO this threshold should be passed in
					{
						standby_units.push_back(unit);
						mediator->SetUnitCommand(unit, ABILITY_ID::EFFECT_BLINK, standby_pos);
						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, standby_pos, true);
					}
				}
			}
			else if (unit->weapon_cooldown > 0)
			{
				// attack has gone off so reset order status
				persistent_fire_control.ConfirmAttack(unit, mediator->GetUnit(unit->engaged_target_tag));
				attack_status[unit] = false;
			} // TODO maybe reimplement this if necessary
			else if (unit->orders.size() == 0/* || unit->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE || unit->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE*/)
			{
				// attack order is no longer valid
				attack_status[unit] = false;
			}
		}
		return incoming_damage;
	}

	void ArmyGroup::MicroWarpPrisms(std::vector<std::pair<const Unit*, UnitDanger>> units_requesting_pickup)
	{
		for (const auto& prism : warp_prisms)
		{

			if (unit_position_asignments.find(prism) != unit_position_asignments.end())
				mediator->SetUnitCommand(prism, ABILITY_ID::GENERAL_MOVE, unit_position_asignments[prism]);
			else
				mediator->SetUnitCommand(prism, ABILITY_ID::GENERAL_MOVE, prism->pos);
			mediator->SetUnitCommand(prism, ABILITY_ID::UNLOADALLAT, prism);
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
				mediator->SetUnitCommand(request.first, ABILITY_ID::SMART, units_in_cargo[request.first].prism);
				continue;
			}
			for (const auto& prism : warp_prisms)
			{
				int size = Utility::GetCargoSize(request.first);
				int necessary_space = std::max(size, request.second.unit_prio * 2);
				if (8 - (prism_cargo[prism]) >= necessary_space)
				{
					unit_found_space = true;
					mediator->SetUnitCommand(request.first, ABILITY_ID::SMART, prism);
					units_in_cargo[request.first] = PrismCargo(prism);
					prism_cargo[prism] += size;
					break;
				}
			}
			if (unit_found_space == false)
			{
				if (request.first->unit_type == STALKER && request.second.unit_prio >= 2)
				{
					float now = mediator->GetGameLoop() / 22.4;
					bool blink_off_cooldown = now - last_time_blinked[request.first] > 7;

					if (mediator->upgrade_manager.has_blink && blink_off_cooldown)
					{

						mediator->SetUnitCommand(request.first, ABILITY_ID::EFFECT_BLINK, Utility::PointBetween(request.first->pos, Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy), request.first->pos)->pos, -4)); // TODO adjustable blink distance

						attack_status[request.first] = false;
						last_time_blinked[request.first] = now;
					}
				}
				break;
			}
		}
	}

	void ArmyGroup::AutoAddNewUnits(std::vector<UNIT_TYPEID> unit_types)
	{
		std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
			this->OnNewUnitCreatedListener(unit);
		};
		mediator->agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);
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
		mediator->agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);
		this->unit_types = unit_types;
	}

	void ArmyGroup::OnUnitCreatedListener(const Unit* unit)
	{
		
	}

	void ArmyGroup::OnUnitDamagedListener(const Unit* unit, float health_damage, float shields_damage)
	{
		for (const auto& oracle : oracles)
		{
			if (target[oracle] == unit->tag)
			{
				//agent->Debug()->DebugTextOut(Utility::UnitTypeIdToString(unit->unit_type.ToType()) + " took " + std::to_string(health) + " damage from orale", Point2D(.2, .4 + .02 * i), Color(0, 255, 0), 20);
				//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage from orale\n";
				has_attacked[oracle] = true;
			}
		}
	}

	void ArmyGroup::OnUnitDestroyedListener(const Unit* unit)
	{
		for (const auto& oracle : oracles)
		{
			if (target[oracle] == unit->tag)
			{
				//agent->Debug()->DebugTextOut(Utility::UnitTypeIdToString(unit->unit_type.ToType()) + " desroyed by oracle", Point2D(.2, .45 + .02 * i), Color(0, 255, 0), 20);
				//std::cout << Utility::UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed by orale\n";
				has_attacked[oracle] = true;
			}
		}

		if (std::find(all_units.begin(), all_units.end(), unit) != all_units.end())
			RemoveUnit(unit);
		if (std::find(new_units.begin(), new_units.end(), unit) != new_units.end())
			RemoveUnit(unit);
	}

}