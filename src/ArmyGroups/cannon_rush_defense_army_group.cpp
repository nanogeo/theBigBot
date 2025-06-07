
#include "cannon_rush_defense_army_group.h"
#include "mediator.h"
#include "utility.h"
#include "finite_state_machine.h"
#include "locations.h"
#include "definitions.h"

namespace sc2 {


	CannonRushDefenseArmyGroup::CannonRushDefenseArmyGroup(Mediator* mediator) : ArmyGroup(mediator)
	{
		desired_units = 30;
	}

	CannonRushDefenseArmyGroup::~CannonRushDefenseArmyGroup()
	{
		for (const auto& probe : probe_targets)
		{
			mediator->PlaceWorker(probe.first);
		}
	}
	
	void CannonRushDefenseArmyGroup::Run() // TODO fix
	{
		// if there are cannons pull workers against them
		for (const auto& unit : mediator->GetUnits(Unit::Alliance::Enemy, IsUnits({ PROBE, PYLON, CANNON })))
		{
			if (unit->display_type != Unit::DisplayType::Visible)
				continue;

			if (assigned_attackers.find(unit) != assigned_attackers.end())
				continue;

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
			if (probe_targets.find(probe) == probe_targets.end())
			{
				available_probes.push_back(probe);
			}
		}

		for (auto itr = assigned_attackers.begin(); itr != assigned_attackers.end();)
		{
			if (itr->first == nullptr || itr->first->is_alive == false)
			{
				for (const auto& probe : itr->second)
				{
					if (probe != nullptr)
					{
						probe_targets.erase(probe);
						available_probes.push_back(probe);
					}
				}
				itr = assigned_attackers.erase(itr);
			}
			else
			{
				itr++;
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
				std::cerr << "Unknown unit found in CannonRushDefenseArmyGroup::Run " << UnitTypeToName(enemy.first->unit_type) << std::endl;
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
						probe_targets[new_probe] = enemy.first;
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

	void CannonRushDefenseArmyGroup::RemoveUnit(const Unit* unit)
	{
		ArmyGroup::RemoveUnit(unit);

		probe_targets.erase(unit);
		for (auto& assignment : assigned_attackers)
		{
			for (auto& probe : assignment.second)
			{
				if (probe == unit)
				{
					assignment.second.erase(std::remove(assignment.second.begin(), assignment.second.end(), unit), assignment.second.end());
					return;
				}
			}
		}
	}

}