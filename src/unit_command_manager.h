#pragma once

#include "sc2api/sc2_interfaces.h"


namespace sc2
{

class Mediator;
class TheBigBot;

struct UnitCommand
{
    AbilityID ability;
    Point2D target_point;
    const Unit* target;
    int priority;
    UnitCommand()
    {
        target = NULL;
        target_point = Point2D(0, 0);
        priority = 0;
    }
    UnitCommand(AbilityID ability, int priority)
    {
        this->ability = ability;
        this->priority = priority;
        target = NULL;
        target_point = Point2D(0, 0);
    }
    UnitCommand(AbilityID ability, Point2D target_point, int priority)
    {
        this->ability = ability;
        this->priority = priority;
        this->target_point = target_point;
        target = NULL;
    }
    UnitCommand(AbilityID ability, const Unit* target, int priority)
    {
        this->ability = ability;
        this->priority = priority;
        this->target = target;
        target_point = Point2D(0, 0);
    }
    bool operator==(UnitCommand u)
    {
        if ((target_point == u.target_point && target == u.target))
        {
            if (ability == ABILITY_ID::SMART && 
                (u.ability == ABILITY_ID::SMART ||
                u.ability == ABILITY_ID::HARVEST_GATHER ||
                u.ability == ABILITY_ID::HARVEST_RETURN ||
                u.ability == ABILITY_ID::GENERAL_MOVE))
                    return true;
            if (u.ability == ABILITY_ID::SMART &&
                    (ability == ABILITY_ID::HARVEST_GATHER ||
                    ability == ABILITY_ID::HARVEST_RETURN ||
                    ability == ABILITY_ID::GENERAL_MOVE))
                return true;
        }
        return false;
    }
};

class UnitCommandManager
{
public:
	Mediator* mediator;
    TheBigBot* agent;
    std::map<const Unit*, UnitCommand> current_commands;
    std::map<const Unit*, std::vector<UnitCommand>> queued_commands;

	UnitCommandManager(Mediator* mediator, TheBigBot* agent)
	{
		this->mediator = mediator;
        this->agent = agent;
	}

    void SetUnitCommand(const Unit* unit, AbilityID ability, int priority, bool queued_command = false);
    void SetUnitCommand(const Unit* unit, AbilityID ability, Point2D point, int priority, bool queued_command = false);
    void SetUnitCommand(const Unit* unit, AbilityID ability, const Unit* target, int priority, bool queued_command = false);
    void SetUnitsCommand(Units units, AbilityID ability, int priority, bool queued_command = false);
    void SetUnitsCommand(Units units, AbilityID ability, Point2D point, int priority, bool queued_command = false);
    void SetUnitsCommand(Units units, AbilityID ability, const Unit* target, int priority, bool queued_command = false);
    void ParseUnitCommands();
};


}