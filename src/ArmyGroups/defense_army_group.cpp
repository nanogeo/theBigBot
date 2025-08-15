
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
			defensive_groups.push_back(DefensiveGroup(itr->attack_pos, {}, itr->units));
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
			itr->friendly_units.push_back(unassigned_units.back());
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
	while (worst_status->status < 0)
	{
		if (unassigned_units.size() > 0)
		{
			worst_status->friendly_units.push_back(unassigned_units.back());
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
			best_status->friendly_units.pop_back();
			float new_status = mediator->JudgeFight(best_status->friendly_units, best_status->enemy_units, 0, 0, false);
			if (new_status < 0)
			{
				best_status->friendly_units.push_back(extra_unit);
				break;
			}
			else
			{
				worst_status->friendly_units.push_back(extra_unit);
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

DefenseArmyGroup::DefenseArmyGroup(Mediator* mediator) : ArmyGroup(mediator)
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
	for (const auto& group : defensive_groups)
	{
		for (const auto& unit : group.friendly_units)
		{
			if (mediator->GetAttackStatus(unit))
				continue;
			if (unit->weapon_cooldown == 0)
				mediator->AddUnitToAttackers(unit);
		}
		mediator->SetUnitsCommand(group.friendly_units, A_MOVE, group.defensive_pos, CommandPriorty::low);
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
			mediator->SetUnitCommand(unit, A_MOVE, central_pos, CommandPriorty::low);
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
	ArmyGroup::AddUnit(unit);
}

void DefenseArmyGroup::RemoveUnit(const Unit* unit)
{
	unassigned_units.erase(std::remove(unassigned_units.begin(), unassigned_units.end(), unit), unassigned_units.end());
	for (auto& group : defensive_groups)
	{
		for (auto itr = group.friendly_units.begin(); itr != group.friendly_units.end(); itr++)
		{
			if ((*itr) == unit)
			{
				group.friendly_units.erase(itr);
				break;
			}
		}
	}
	ArmyGroup::RemoveUnit(unit);
}

}