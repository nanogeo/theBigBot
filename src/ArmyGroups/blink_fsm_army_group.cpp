
#include "blink_fsm_army_group.h"
#include "blink_stalker_attack_terran_state_machine.h"
#include "mediator.h"
#include "utility.h"
#include "definitions.h"

#include "theBigBot.h"

namespace sc2 {

std::vector<std::pair<const Unit*, UnitDanger>> BlinkFSMArmyGroup::CalculateUnitDanger(Units units)
{
	std::vector<std::pair<const Unit*, UnitDanger>> incoming_damage;
	for (const auto& unit : units)
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

bool BlinkFSMArmyGroup::FindNewConcaveOrigin(Units units, bool with_prism)
{
	// first concave origin can just be the closest points on the line
	if (concave_origin == Point2D(0, 0))
	{
		concave_origin = attack_path.FindClosestPoint(Utility::MedianCenter(units));
	}

	float desired_range = CalculateDesiredRange(units);

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
	concave_origin = CalculateNewConcaveOrigin(close_enemies, desired_range, limit, (int)units.size(), with_prism ? 1 : 0);

	if (Utility::AnyUnitWithin(mediator->GetUnits(Unit::Alliance::Enemy), concave_origin, CLOSE_RANGE) == false && Distance2D(concave_origin, attack_path.GetEndPoint()) < CLOSE_RANGE)
		return true;
	return false;
}

Point2D BlinkFSMArmyGroup::CalculateNewConcaveOrigin(Units close_enemies, float desired_range, Point2D limit, int num_basic_units, int num_prisms)
{
	Point2D new_origin;

	// too close
	if (Distance2D(concave_origin, concave_target) < desired_range - .1f ||
		(close_enemies.size() > 0 && Utility::DistanceToClosest(close_enemies, concave_origin) < 2 + unit_size) ||
		concave_origin == attack_path.GetFurthestForward({ concave_origin, limit }))
	{
		// if were already retreating and we have the correct number of points then check if we need a new concave
		if (!advancing && unit_position_asignments.size() == num_basic_units + num_prisms)
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
			if (total_dist / (num_basic_units + num_prisms) > 1)
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
		if (advancing && unit_position_asignments.size() == num_basic_units + num_prisms)
		{
			float total_dist = 0;
			for (const auto& pos : unit_position_asignments)
			{
				// maybe ignore very far off units
				total_dist += Distance2D(pos.first->pos, pos.second);
			}
			if (total_dist / (num_basic_units + num_prisms) > 1)
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

BlinkFSMArmyGroup::BlinkFSMArmyGroup(Mediator* mediator, BlinkStalkerAttackTerran* state_machine, int desired_units, int max_units, int required_units, int min_reinfore_group_size) :
	AttackArmyGroup(mediator, mediator->GetDirectAttackLine(), { STALKER, PRISM, COLOSSUS, IMMORTAL }, desired_units, max_units, required_units, min_reinfore_group_size)
{
	this->state_machine = state_machine;
	main_attack_path = mediator->GetLocations().blink_main_attack_path_lines;
	natural_attack_path = mediator->GetLocations().blink_nat_attack_path_line;
	third_attack_path = mediator->GetLocations().blink_third_attack_path_lines;
	dispersion = .2f;
	default_range = 7;
}
	
void BlinkFSMArmyGroup::SetUp()
{
		
}

void BlinkFSMArmyGroup::Run()
{
	// handled by state machine
}

void BlinkFSMArmyGroup::ScourMap()
{
	ConvertToRegularAttackArmyGroup();
}

void BlinkFSMArmyGroup::AddUnit(const Unit* unit)
{
	AttackArmyGroup::AddUnit(unit);

	if (state_machine->AddUnit(unit) == false)
	{
		ConvertToRegularAttackArmyGroup();
	}
}

void BlinkFSMArmyGroup::RemoveUnit(const Unit* unit)
{
	state_machine->RemoveUnit(unit);
	AttackArmyGroup::RemoveUnit(unit);
}

// returns true when group is close enough to add into main army
bool BlinkFSMArmyGroup::MobilizeNewUnits(Units units)
{
	if (Distance2D(Utility::MedianCenter(units), Utility::MedianCenter(all_units)) < 5)
		return true;
	for (const auto& unit : units)
	{
		if (mediator->GetAttackStatus(unit))
			continue;
		if (unit->weapon_cooldown == 0)
			mediator->AddUnitToAttackers(unit);

		mediator->SetUnitCommand(unit, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(all_units), CommandPriorty::low);
		// TODO make sure units stay grouped up
	}
	return false;
}

void BlinkFSMArmyGroup::GroupUpNewUnits()
{
	// TODO check for enemies in range
	mediator->SetUnitsCommand(new_units, ABILITY_ID::GENERAL_MOVE, Utility::MedianCenter(new_units), CommandPriorty::low);

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

Point2D BlinkFSMArmyGroup::GetConcaveOrigin() const
{
	return concave_origin;
}

void BlinkFSMArmyGroup::ResetConcaveOrigin()
{
	concave_origin = Point2D(0, 0);
}

void BlinkFSMArmyGroup::SetAttackPath(PiecewisePath path)
{
	attack_path = path;
}

const PiecewisePath& BlinkFSMArmyGroup::GetAttackPath() const
{
	return attack_path;
}

void BlinkFSMArmyGroup::SetUseStandby(bool value)
{
	using_standby = value;
}

void BlinkFSMArmyGroup::SetStandbyPos(Point2D pos)
{
	standby_pos = pos;
}

AttackLineResult BlinkFSMArmyGroup::AttackLine(Units units)
{
	if (units.size() == 0)
	{
		return AttackLineResult::all_units_dead;
	}

	AttackLineResult result = AttackLineResult::normal;
	// update unit size TODO maybe move to addunit/removeunit
	unit_size = Utility::GetLargestUnitSize(units);

	// move concave origin if necessary
	if (FindNewConcaveOrigin(units, false))
		result = AttackLineResult::reached_end_of_path;

	standby_pos = attack_path.GetPointFrom(concave_origin, 8, false);

	// Find positions
	std::vector<Point2D> prism_positions;
	std::vector<Point2D> concave_positions = FindConcaveWithPrism(prism_positions, (int)units.size(), 0);

	// assign units to positions
	unit_position_asignments = AssignUnitsToPositions(units, concave_positions);

	// avoid danger
	std::vector<std::pair<const Unit*, UnitDanger>> incoming_damage = CalculateUnitDanger(units);
	Units escaping_units = EvadeDamage(incoming_damage);


	// move units to positions
	for (const auto& assignment : unit_position_asignments)
	{
		if (mediator->GetAttackStatus(assignment.first) == false)
			mediator->SetUnitCommand(assignment.first, A_MOVE, assignment.second, CommandPriorty::low);
	}

	// if units that can attack > threshold -> add units to attackers
	// ignore units blinking/casting
	Units ready_to_attack;
	for (const auto& unit : units)
	{
		if (std::find(escaping_units.begin(), escaping_units.end(), unit) != escaping_units.end())
			continue;
		if (unit->weapon_cooldown > 0 || mediator->GetAttackStatus(unit))
			continue;

		ready_to_attack.push_back(unit);
	}

	if ((float)ready_to_attack.size() / units.size() >= attack_threshold)
	{
		mediator->AddUnitsToAttackers(ready_to_attack);
	}

	return result;
}

AttackLineResult BlinkFSMArmyGroup::AttackLine(Units units, const Unit* prism)
{
	if (units.size() == 0)
	{
		return AttackLineResult::all_units_dead;
	}

	AttackLineResult result = AttackLineResult::normal;
	// update unit size TODO maybe move to addunit/removeunit
	unit_size = Utility::GetLargestUnitSize(units);

	// move concave origin if necessary
	if (FindNewConcaveOrigin(units, true))
		result = AttackLineResult::reached_end_of_path;

	standby_pos = attack_path.GetPointFrom(concave_origin, 8, false);

	// Find positions
	std::vector<Point2D> prism_positions;
	std::vector<Point2D> concave_positions = FindConcaveWithPrism(prism_positions, (int)units.size(), 1);

	// assign units to positions
	unit_position_asignments = AssignUnitsToPositions(units, concave_positions);
	if (prism != nullptr)
	{
		std::map<const Unit*, Point2D> prism_position_assignments = AssignUnitsToPositions(warp_prisms, prism_positions);
		for (const auto& assignment : prism_position_assignments)
		{
			unit_position_asignments[assignment.first] = assignment.second;
		}
	}

	// avoid danger
	std::vector<std::pair<const Unit*, UnitDanger>> incoming_damage = CalculateUnitDanger(units);
	Units escaping_units = EvadeDamage(incoming_damage);

	std::vector<Tag> units_in_cargo;
	// if prisms -> pickup units in enough danger
	if (prism != nullptr)
	{
		mediator->ForceUnitCommand(prism, A_UNLOAD_AT, prism);
		for (const auto& passanger : prism->passengers)
		{
			units_in_cargo.push_back(passanger.tag);
			mediator->RemoveAllAttacksAtUnit(mediator->GetUnit(passanger.tag));
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

	if ((float)ready_to_attack.size() / units.size() >= attack_threshold)
	{
		mediator->AddUnitsToAttackers(ready_to_attack);
	}

	return result;
}

void BlinkFSMArmyGroup::ConvertToRegularAttackArmyGroup()
{
	mediator->MarkStateMachineForDeletion(state_machine);
	mediator->MarkArmyGroupForDeletion(this);
	mediator->DeleteFourGateBlinkFSM();
}

}