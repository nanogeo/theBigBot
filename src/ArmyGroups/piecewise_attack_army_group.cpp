
#include "piecewise_attack_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "definitions.h"

#include "theBigBot.h"

namespace sc2 {


void PiecewiseAttackArmyGroup::OraclesDefendArmy()
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

PiecewiseAttackArmyGroup::PiecewiseAttackArmyGroup(Mediator* mediator, PiecewisePath attack_path, std::vector<UNIT_TYPEID> unit_types,
	int desired_units, int max_units, int required_units, int min_reinforce_group_size) : AttackArmyGroup(mediator)
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
	limit_advance = true;
	pre_prism_limit = mediator->GetLocations().adept_scout_ramptop;
}

PiecewiseAttackArmyGroup::PiecewiseAttackArmyGroup(Mediator* mediator, ArmyTemplate<PiecewiseAttackArmyGroup>* army_template) : AttackArmyGroup(mediator)
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
	
void PiecewiseAttackArmyGroup::SetUp()
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

void PiecewiseAttackArmyGroup::Run()
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

void PiecewiseAttackArmyGroup::ScourMap()
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

void PiecewiseAttackArmyGroup::AddUnit(const Unit* unit)
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

void PiecewiseAttackArmyGroup::RemoveUnit(const Unit* unit)
{
	for (auto itr = units_on_their_way.begin(); itr != units_on_their_way.end();)
	{
		itr->erase(std::remove(itr->begin(), itr->end(), unit), itr->end());
		if (itr->size() == 0)
			itr = units_on_their_way.erase(itr);
		else
			itr++;
	}
	basic_units.erase(std::remove(basic_units.begin(), basic_units.end(), unit), basic_units.end());
	oracles.erase(std::remove(oracles.begin(), oracles.end(), unit), oracles.end());
	warp_prisms.erase(std::remove(warp_prisms.begin(), warp_prisms.end(), unit), warp_prisms.end());

	ArmyGroup::RemoveUnit(unit);
}

// returns true when group is close enough to add into main army
bool PiecewiseAttackArmyGroup::MobilizeNewUnits(Units units)
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

void PiecewiseAttackArmyGroup::GroupUpNewUnits()
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


AttackLineResult PiecewiseAttackArmyGroup::AttackLine()
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

	// micro special units
	// oracles
	if (oracles.size())
		OraclesDefendArmy();

	return AttackArmyGroup::AttackLine(basic_units, concave_origin, default_range, &attack_path, limit_advance, 
		pre_prism_limit, warp_prisms, dispersion, unit_position_asignments, oracles, advancing, attack_threshold);
	
}

}