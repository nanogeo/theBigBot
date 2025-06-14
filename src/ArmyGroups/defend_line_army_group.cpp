
#include "defend_line_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {


	DefendLineArmyGroup::DefendLineArmyGroup(Mediator* mediator, Point2D start, Point2D end, std::vector<UNIT_TYPEID> unit_types, 
		uint16_t desired_units, uint16_t max_units) : ArmyGroup(mediator)
	{
		for (const auto& type : unit_types)
		{
			this->unit_types.push_back(type);
		}

		this->start = start;
		this->end = end;
		this->desired_units = desired_units;
		this->max_units = max_units;
	}

	void DefendLineArmyGroup::Run()
	{
		Units enemy_ground_units = mediator->GetUnits(Unit::Alliance::Enemy, IsNotFlyingUnit());
		Units enemy_units = mediator->GetUnits(Unit::Alliance::Enemy);

		// remove any enemies too far from defense location
		for (int i = 0; i < enemy_ground_units.size(); i++)
		{
			Point2D closest_pos = Utility::ClosestPointOnLine(enemy_ground_units[i]->pos, start, end);
			if (Distance2D(enemy_ground_units[i]->pos, closest_pos) > leash_range)
			{
				enemy_ground_units.erase(enemy_ground_units.begin() + i);
				i--;
			}
		}
		for (int i = 0; i < enemy_units.size(); i++)
		{
			Point2D closest_pos = Utility::ClosestPointOnLine(enemy_units[i]->pos, start, end);
			if (Distance2D(enemy_units[i]->pos, closest_pos) > leash_range)
			{
				enemy_units.erase(enemy_units.begin() + i);
				i--;
			}
		}

		if (enemy_ground_units.size() > 0)
		{
			if (oracles.size() > 0)
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
			}

			// attack with orcales
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
					if (Distance2D(oracle->pos, closest_unit->pos) < 2 && enemy_ground_units.size() > 5) // TODO use battle sim to determine this better
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

				double dist_to_start = Distance2D(oracle->pos, start);
				double dist_to_end = Distance2D(oracle->pos, end);

				if (oracle->orders.size() > 0 &&
					(oracle->orders[0].target_pos == start || oracle->orders[0].target_pos == end))
				{
					continue;
				}
				else if (dist_to_start < 1)
				{
					mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, end, 0);
				}
				else if (dist_to_end < 1)
				{
					mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, start, 0);
				}
				else
				{
					if (dist_to_end < dist_to_start)
						mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, end, 0);
					else
						mediator->SetUnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, start, 0);
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
					double dist_to_start = Distance2D(unit->pos, start);
					double dist_to_end = Distance2D(unit->pos, end);

					if (unit->orders.size() > 0 &&
						(unit->orders[0].target_pos == start || unit->orders[0].target_pos == end))
					{
						continue;
					}
					else if (dist_to_start < 1)
					{
						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, end, 0);
					}
					else if (dist_to_end < 1)
					{
						mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, start, 0);
					}
					else
					{
						if (dist_to_end < dist_to_start)
							mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, end, 0);
						else
							mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, start, 0);
					}
				}
			}
		}
	}

	void DefendLineArmyGroup::AddNewUnit(const Unit* unit)
	{
		AddUnit(unit);
	}

	void DefendLineArmyGroup::AddUnit(const Unit* unit)
	{
		if (unit->unit_type == ORACLE)
		{
			if (std::find(oracles.begin(), oracles.end(), unit) == oracles.end())
				oracles.push_back(unit);
		}
		ArmyGroup::AddUnit(unit);
	}

	void DefendLineArmyGroup::RemoveUnit(const Unit* unit)
	{
		oracles.erase(std::remove(oracles.begin(), oracles.end(), unit), oracles.end());
		ArmyGroup::RemoveUnit(unit);
	}

}