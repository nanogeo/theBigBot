
#include "unit_command_manager.h"
#include "theBigBot.h"
#include "locations.h"

#include <map>
#include <fstream>
#include <iostream>


namespace sc2
{
	// TODO do not queue blink or oracle beam etc or deal with it

void UnitCommandManager::SetUnitCommand(const Unit* unit, AbilityID ability, int priority, bool queued_command)
{
	UnitCommand new_command = UnitCommand(ability, priority);
	if (queued_command)
	{
		queued_commands[unit].push_back(new_command);
		return;
	}
	if (unit->orders.size() > 0) // command already being followed
	{
		if (ability == unit->orders[0].ability_id ||
			(ability == ABILITY_ID::SMART &&
			(unit->orders[0].ability_id == ABILITY_ID::SMART ||
				unit->orders[0].ability_id == ABILITY_ID::HARVEST_GATHER ||
				unit->orders[0].ability_id == ABILITY_ID::HARVEST_RETURN ||
				unit->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE)))
			return;
		if (unit->orders[0].ability_id == ABILITY_ID::SMART &&
			(ability == ABILITY_ID::HARVEST_GATHER ||
				ability == ABILITY_ID::HARVEST_RETURN ||
				ability == ABILITY_ID::GENERAL_MOVE))
			return;
	}
	if (current_commands.count(unit) == 0) // new command
		current_commands[unit] = new_command;
	else if (current_commands[unit] == new_command) // duplicate command
		return;
	else
	{
		// commands conflict
		if (new_command.priority > current_commands[unit].priority)
			current_commands[unit] = new_command;
		return;
	}
}

void UnitCommandManager::SetUnitCommand(const Unit* unit, AbilityID ability, Point2D point, int priority, bool queued_command)
{
	UnitCommand new_command = UnitCommand(ability, point, priority);
	if (queued_command)
	{
		queued_commands[unit].push_back(new_command);
		return;
	}
	if (unit->orders.size() > 0 && Distance2D(unit->orders[0].target_pos, point) < .01) // command already being followed
	{
		if (ability == unit->orders[0].ability_id ||
			(ability == ABILITY_ID::SMART &&
			(unit->orders[0].ability_id == ABILITY_ID::SMART ||
				unit->orders[0].ability_id == ABILITY_ID::HARVEST_GATHER ||
				unit->orders[0].ability_id == ABILITY_ID::HARVEST_RETURN ||
				unit->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE)))
			return;
		if (unit->orders[0].ability_id == ABILITY_ID::SMART &&
			(ability == ABILITY_ID::HARVEST_GATHER ||
				ability == ABILITY_ID::HARVEST_RETURN ||
				ability == ABILITY_ID::GENERAL_MOVE))
			return;
	}
	if (unit->orders.size() == 0 && ability == ABILITY_ID::GENERAL_MOVE && Distance2D(unit->pos, point) < .01) // ignore move commands to the units current position
		return;
	if (current_commands.count(unit) == 0) // new command
		current_commands[unit] = new_command;
	else if (current_commands[unit] == new_command) // duplicate command
		return;
	else
	{
		// commands conflict
		if (new_command.priority > current_commands[unit].priority)
			current_commands[unit] = new_command;
		return;
	}
}

void UnitCommandManager::SetUnitCommand(const Unit* unit, AbilityID ability, const Unit* target, int priority, bool queued_command)
{
	UnitCommand new_command = UnitCommand(ability, target, priority);
	if (queued_command)
	{
		queued_commands[unit].push_back(new_command);
		return;
	}
	if (unit->orders.size() > 0 && unit->orders[0].target_unit_tag == target->tag) // command already being followed
	{
		if (ability == unit->orders[0].ability_id || 
			(ability == ABILITY_ID::SMART &&
			(unit->orders[0].ability_id == ABILITY_ID::HARVEST_GATHER ||
				unit->orders[0].ability_id == ABILITY_ID::HARVEST_RETURN ||
				unit->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE)))
			return;
		if (ability == unit->orders[0].ability_id ||
			(unit->orders[0].ability_id == ABILITY_ID::SMART &&
			(ability == ABILITY_ID::HARVEST_GATHER ||
				ability == ABILITY_ID::HARVEST_RETURN ||
				ability == ABILITY_ID::GENERAL_MOVE)))
			return;
	}
	if (current_commands.count(unit) == 0) // new command
		current_commands[unit] = new_command;
	else if (current_commands[unit] == new_command) // duplicate command
		return;
	else
	{
		// commands conflict
		if (new_command.priority > current_commands[unit].priority)
			current_commands[unit] = new_command;
		return;
	}
}

void UnitCommandManager::SetUnitsCommand(Units units, AbilityID ability, int priority, bool queued_command)
{
	for (const auto &unit : units)
	{
		SetUnitCommand(unit, ability, priority, queued_command);
	}
}

void UnitCommandManager::SetUnitsCommand(Units units, AbilityID ability, Point2D point, int priority, bool queued_command)
{
	for (const auto& unit : units)
	{
		SetUnitCommand(unit, ability, point, priority, queued_command);
	}
}

void UnitCommandManager::SetUnitsCommand(Units units, AbilityID ability, const Unit* target, int priority, bool queued_command)
{
	for (const auto& unit : units)
	{
		SetUnitCommand(unit, ability, target, priority, queued_command);
	}
}

void UnitCommandManager::ParseUnitCommands()
{
	int actions_this_frame = 0;
#ifndef BUILD_FOR_LADDER
	std::ofstream file;
	file.open("unit_commands.txt", std::ios::app);
#endif
	// TODO check if move command is the same place the unit is
	for (std::map<const Unit*, UnitCommand>::iterator itr = current_commands.begin(); itr != current_commands.end(); itr++)
	{
#ifndef BUILD_FOR_LADDER
		file << mediator->GetGameLoop() << ", " << itr->first->tag << ", " << Utility::UnitTypeIdToString(itr->first->unit_type) << ", ";
		if (itr->first->orders.size() > 0)
			file << Utility::AbilityIdToString(itr->first->orders[0].ability_id.ToType()) << ", " << itr->first->orders[0].target_pos.x << " " << 
			itr->first->orders[0].target_pos.y << ", " << itr->first->orders[0].target_unit_tag << ", ";
		else
			file << "None, 0 0, 0, ";
#endif
		if (itr->second.target_point == Point2D(0, 0))
		{
			if (itr->second.target == NULL)
			{
				if (itr->second.ability.ToType() == ABILITY_ID::BEHAVIOR_PULSARBEAMON)
					mediator->SetOracleOrder(itr->first, ABILITY_ID::BEHAVIOR_PULSARBEAMON);
				else if (itr->second.ability.ToType() == ABILITY_ID::BEHAVIOR_PULSARBEAMOFF)
					mediator->SetOracleOrder(itr->first, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
				else if (itr->second.ability.ToType() == ABILITY_ID::EFFECT_ORACLEREVELATION)
					mediator->SetOracleOrder(itr->first, ABILITY_ID::EFFECT_ORACLEREVELATION);

				agent->Actions()->UnitCommand(itr->first, itr->second.ability);
				actions_this_frame++;
#ifndef BUILD_FOR_LADDER
				file << Utility::AbilityIdToString(itr->second.ability.ToType()) << ", 0 0, 0, " << std::endl;
#endif
			}
			else
			{
				if (itr->second.ability.ToType() == ABILITY_ID::BATTERYOVERCHARGE)
				{
					if (itr->first->energy >= 50 && Distance2D(itr->first->pos, itr->second.target->pos) < 8)
						mediator->SetBatteryOverchargeCooldown();
				}
				agent->Actions()->UnitCommand(itr->first, itr->second.ability, itr->second.target);
				actions_this_frame++;
#ifndef BUILD_FOR_LADDER
				file << Utility::AbilityIdToString(itr->second.ability.ToType()) << ", 0 0, " << itr->second.target->tag << ", " << std::endl;
#endif
			}
		}
		else
		{
			if (itr->second.ability.ToType() == ABILITY_ID::EFFECT_BLINK)
				mediator->SetStalkerOrder(itr->first);
			else if (itr->second.ability.ToType() == ABILITY_ID::EFFECT_MASSRECALL_NEXUS)
			{
				if (itr->first->energy >= 50)
					mediator->SetNexusRecallCooldown();
			}

			agent->Actions()->UnitCommand(itr->first, itr->second.ability, itr->second.target_point);
			actions_this_frame++;
#ifndef BUILD_FOR_LADDER
			file << Utility::AbilityIdToString(itr->second.ability.ToType()) << ", " << itr->second.target_point.x << " " << itr->second.target_point.y << ", 0, " << std::endl;
#endif
		}
	}

	for (std::map<const Unit*, std::vector<UnitCommand>>::iterator itr = queued_commands.begin(); itr != queued_commands.end(); itr++)
	{
		for (const auto& command : itr->second)
		{
			if (command.target_point == Point2D(0, 0))
			{
				if (command.target == NULL)
				{
					agent->Actions()->UnitCommand(itr->first, command.ability, true);
					actions_this_frame++;
				}
				else
				{
					agent->Actions()->UnitCommand(itr->first, command.ability, command.target, true);
					actions_this_frame++;
				}
			}
			else
			{
				agent->Actions()->UnitCommand(itr->first, command.ability, command.target_point, true);
				actions_this_frame++;
			}
		}
	}

#ifndef BUILD_FOR_LADDER
	file.close();
#endif

	if (actions_this_frame > 75)
		consecutive_high_action_frames++;
	else
		consecutive_high_action_frames = 0;

	if (consecutive_high_action_frames > 22)
	{
		mediator->SendChat("Tag: high_apm_warning_" + std::to_string(mediator->GetCurrentTime()), ChatChannel::Team);
	}
	else if (consecutive_high_action_frames > 45)
	{
		mediator->SendChat("Tag: high_apm_interrupt_" + std::to_string(mediator->GetCurrentTime()), ChatChannel::All);
		mediator->ScourMap();
	}

	current_commands.clear();
	queued_commands.clear();

}

}