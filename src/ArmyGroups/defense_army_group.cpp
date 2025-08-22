
#include "defense_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {

void DefenseArmyGroup::AllocateUnitsToDefensiveGroups(std::vector<EnemyArmyGroup> incoming_army_groups)
{
	for (auto itr = defensive_groups.begin(); itr != defensive_groups.end();)
	{
		if (std::find_if(incoming_army_groups.begin(), incoming_army_groups.end(),
			[itr](const EnemyArmyGroup& enemy_group) { return enemy_group.attack_pos == itr->defensive_pos; }) == incoming_army_groups.end())
		{
			// no attack at locations
			unassigned_units.insert(unassigned_units.end(), itr->friendly_units.begin(), itr->friendly_units.end());
			itr = defensive_groups.erase(itr);
		}
		else
		{
			itr->enemy_units.clear();
			itr++;
		}
	}
	// clear enemy units from all and replace them
	
	for (auto itr = incoming_army_groups.begin(); itr != incoming_army_groups.end(); itr++)
	{
		auto group = std::find_if(defensive_groups.begin(), defensive_groups.end(),
			[itr](const DefensiveGroup& defensive_group) { return defensive_group.defensive_pos == itr->attack_pos; });
		if (group == defensive_groups.end())
		{
			// attack at location not being defended
			defensive_groups.push_back(DefensiveGroup(itr->attack_pos, {}, itr->units, new PointPath(mediator->FindPath(mediator->GetStartLocation(), itr->attack_pos))));
		}
		else
		{
			for (const auto& unit : itr->units)
			{
				if (std::find(group->enemy_units.begin(), group->enemy_units.end(), unit) == group->enemy_units.end())
					group->enemy_units.push_back(unit);
			}
		}
	}

	if (defensive_groups.size() == 0)
		return;

	for (auto itr = defensive_groups.begin(); itr != defensive_groups.end(); itr++)
	{
		itr->status = mediator->JudgeFight(itr->friendly_units, itr->enemy_units, 0, 0, false);
		while (unassigned_units.size() > 0 && itr->status < 0)
		{
			itr->AddNewUnit(unassigned_units.back());
			unassigned_units.pop_back();
			itr->status = mediator->JudgeFight(itr->friendly_units, itr->enemy_units, 0, 0, false);
		}
	}

	DefensiveGroup* worst_status = nullptr;
	for (auto itr = defensive_groups.begin(); itr != defensive_groups.end(); itr++)
	{
		if (worst_status == nullptr || itr->status < worst_status->status)
		{
			worst_status = &(*itr);
		}
	}
	while (worst_status->status < 0 || unassigned_units.size() > 0)
	{
		if (unassigned_units.size() > 0)
		{
			worst_status->AddNewUnit(unassigned_units.back());
			unassigned_units.pop_back();
			worst_status->status = mediator->JudgeFight(worst_status->friendly_units, worst_status->enemy_units, 0, 0, false);
			for (auto itr = defensive_groups.begin(); itr != defensive_groups.end(); itr++)
			{
				if (worst_status == nullptr || itr->status < worst_status->status)
				{
					worst_status = &(*itr);
				}
			}
			continue;
		}
		DefensiveGroup* best_status = nullptr;
		for (auto itr = defensive_groups.begin(); itr != defensive_groups.end(); itr++)
		{
			if (itr->status > 0 && (best_status == nullptr || itr->status > best_status->status))
			{
				best_status = &(*itr);
			}
		}
		if (best_status != nullptr)
		{
			const Unit* extra_unit = best_status->friendly_units.back();
			best_status->RemoveUnit(extra_unit);
			float new_status = mediator->JudgeFight(best_status->friendly_units, best_status->enemy_units, 0, 0, false);
			if (new_status < 0)
			{
				best_status->AddNewUnit(extra_unit);
				break;
			}
			else
			{
				worst_status->AddNewUnit(extra_unit);
				worst_status->status = mediator->JudgeFight(worst_status->friendly_units, worst_status->enemy_units, 0, 0, false);
				for (auto itr = defensive_groups.begin(); itr != defensive_groups.end(); itr++)
				{
					if (worst_status == nullptr || itr->status < worst_status->status)
					{
						worst_status = &(*itr);
					}
				}
				continue;
			}
		}
		break;
	}
}

DefenseArmyGroup::DefenseArmyGroup(Mediator* mediator) : AttackArmyGroup(mediator)
{
	unit_types = ALL_ARMY_UNITS;
	central_pos = mediator->GetCentralBasePos();
}
	
void DefenseArmyGroup::SetUp()
{
	ready = true;
}

void DefenseArmyGroup::Run()
{
	central_pos = mediator->GetCentralBasePos();

	// update ongoing attacks
	// update incoming attacks
	std::vector<EnemyArmyGroup> incoming_army_groups = mediator->GetIncomingEnemyArmyGroups();
	// find air harassers

	// group units to defend ongoing attacks
	// group units to defend incoming attacks
	AllocateUnitsToDefensiveGroups(incoming_army_groups);
	for (auto& group : defensive_groups)
	{
		for (int i = 0; i < group.new_units.size(); i++)
		{
			if (group.origin == Point2D(0, 0))
			{
				group.origin = group.path->FindClosestPoint(group.new_units[i]->pos);
				group.AddUnit(group.new_units[i]);
				i--;
				continue;
			}
			if (Distance2D(group.new_units[i]->pos, group.origin) < MEDIUM_RANGE)
			{
				group.AddUnit(group.new_units[i]);
				i--;
				continue;
			}

			if (mediator->GetAttackStatus(group.new_units[i]))
				continue;
			if (group.new_units[i]->weapon_cooldown == 0)
				mediator->AddUnitToAttackers(group.new_units[i]);

			mediator->SetUnitCommand(group.new_units[i], A_MOVE, group.origin, CommandPriority::low);
		}
		if (group.basic_units.size() > 6) // vary this based on attacking number of units?
		{
			AttackLine(group);
		}
		else if (group.friendly_units.size() > 0)
		{
			IndividualAttack(group);
		}
		else
		{
			// micro special units only
		}
	}
	
	// position units to defend against air harassers

	// move remaining units to central position
	for (const auto& unit : unassigned_units)
	{
		if (mediator->GetAttackStatus(unit))
			continue;
		if (unit->weapon_cooldown == 0)
			mediator->AddUnitToAttackers(unit);
		if (Distance2D(unit->pos, central_pos) > 10)
			mediator->SetUnitCommand(unit, A_MOVE, central_pos, CommandPriority::low);
	}
}

void DefenseArmyGroup::AddNewUnit(const Unit* unit)
{
	AddUnit(unit);
}

void DefenseArmyGroup::AddUnit(const Unit* unit)
{
	// TODO treat obs and prisms differently and maybe air units
	unassigned_units.push_back(unit);
	mediator->SetUnitCommand(unit, A_MOVE, central_pos, CommandPriority::low);
	ArmyGroup::AddUnit(unit);
}

void DefenseArmyGroup::RemoveUnit(const Unit* unit)
{
	unassigned_units.erase(std::remove(unassigned_units.begin(), unassigned_units.end(), unit), unassigned_units.end());
	for (auto& group : defensive_groups)
	{
		if (std::find(group.friendly_units.begin(), group.friendly_units.end(), unit) != group.friendly_units.end())
		{
			group.RemoveUnit(unit);
			break;
		}
	}
	ArmyGroup::RemoveUnit(unit);
}

AttackLineResult DefenseArmyGroup::AttackLine(DefensiveGroup& group)
{
	OraclesDefendArmy(group.oracles, group.path, group.basic_units);
	return AttackArmyGroup::AttackLine(group.basic_units, group.origin, group.normal_range, group.path, false, Point2D(0, 0), group.prisms, 0, group.unit_position_assignments, group.oracles, group.advancing, .25f);
}

void DefenseArmyGroup::IndividualAttack(DefensiveGroup& group)
{
	Units enemy_units = mediator->GetUnits(Unit::Alliance::Enemy);
	for (const auto& unit : group.basic_units)
	{
		if (mediator->GetAttackStatus(unit))
			continue;
		if (unit->weapon_cooldown == 0)
			mediator->AddUnitToAttackers(unit);

		const Unit* closest_enemy = Utility::ClosestTo(enemy_units, unit->pos);
		Point2D point_on_path = group.path->FindClosestPoint(unit->pos);

		if (Distance2D(closest_enemy->pos, unit->pos) > VERY_LONG_RANGE)
		{
			mediator->SetUnitCommand(unit, A_MOVE, group.defensive_pos, CommandPriority::low);
			continue;
		}

		if (group.status < 0 && Distance2D(closest_enemy->pos, unit->pos) < Utility::RealRange(unit, closest_enemy) + 1)
		{
			Point2D away_vector = unit->pos - closest_enemy->pos;
			Point2D back_vector = group.path->GetPointFrom(point_on_path, 1, false);
			if (back_vector == Point2D(0, 0))
				back_vector = group.path->GetStartPoint() - unit->pos;
			Point2D move_vector = away_vector - back_vector;
			move_vector = Utility::NormalizeVector(move_vector);

			mediator->SetUnitCommand(unit, A_MOVE, unit->pos + move_vector, CommandPriority::low);
		}
		else
		{
			float dist = Distance2D(closest_enemy->pos, unit->pos);
			Point2D intercept_point = Utility::PointBetween(closest_enemy->pos, closest_enemy->pos + Utility::GetForwardVector(unit), dist / 2);
			mediator->SetUnitCommand(unit, A_MOVE, intercept_point, CommandPriority::low);
		}

		switch (unit->unit_type.ToType())
		{
		case ADEPT:
			if (mediator->IsAdeptShadeOffCooldown(unit))
			{
				mediator->ForceUnitCommand(unit, A_SHADE, closest_enemy->pos);
			}
			else
			{
				std::pair<const Unit*, int> info = mediator->GetAdeptShadeInfo(unit);
				if (info.first == nullptr)
					break;
				mediator->SetUnitCommand(info.first, A_MOVE, closest_enemy->pos, CommandPriority::low);
			}
			break;
		}
	}

	OraclesCastRevelation(group.oracles);
	OraclesDefendLocation(group.oracles, enemy_units, group.defensive_pos);
}

void DefenseArmyGroup::LogDebugInfo()
{
	std::string str;
	for (const auto& group : defensive_groups)
	{
		str += "defensive pos: " + std::to_string(group.defensive_pos.x) + ", " + std::to_string(group.defensive_pos.y) + '\n';
		str += "f units: " + std::to_string(group.friendly_units.size()) + '\n';
		str += "e units: " + std::to_string(group.enemy_units.size()) + '\n';
	}
	std::cerr << str;
}

}