#pragma once
#include <vector>
#include <map>

#include "sc2api/sc2_interfaces.h"

#include "definitions.h"

namespace sc2
{

class Mediator;
class TheBigBot;

struct UnitCommand
{
    AbilityID ability;
    Point2D target_point;
    const Unit* target;
    CommandPriority priority;
    UnitCommand()
    {
        target = nullptr;
        target_point = Point2D(0, 0);
        priority = CommandPriority::low;
    }
    UnitCommand(AbilityID ability, CommandPriority priority)
    {
        this->ability = ability;
        this->priority = priority;
        target = nullptr;
        target_point = Point2D(0, 0);
    }
    UnitCommand(AbilityID ability, Point2D target_point, CommandPriority priority)
    {
        this->ability = ability;
        this->priority = priority;
        this->target_point = target_point;
        target = nullptr;
    }
    UnitCommand(AbilityID ability, const Unit* target, CommandPriority priority)
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
            if (ability == A_SMART && 
                (u.ability == A_SMART ||
                u.ability == A_GATHER_RESOURCE ||
                u.ability == A_RETURN_RESOURCE ||
                u.ability == A_MOVE))
                    return true;
            if (u.ability == A_SMART &&
                    (ability == A_GATHER_RESOURCE ||
                    ability == A_RETURN_RESOURCE ||
                    ability == A_MOVE))
                return true;
        }
        return false;
    }
};

class UnitCommandManager
{
private:
	Mediator* mediator;
    TheBigBot* agent;
    std::vector<int> actions_past_ten_frames = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int consecutive_high_action_frames = 0;
    std::map<const Unit*, UnitCommand> current_commands;
    std::map<const Unit*, std::vector<UnitCommand>> queued_commands;

public:
    UnitCommandManager(Mediator* mediator, TheBigBot* agent);

    void SetUnitCommand(const Unit* unit, AbilityID ability, CommandPriority priority, bool queued_command = false);
    void SetUnitCommand(const Unit* unit, AbilityID ability, Point2D point, CommandPriority priority, bool queued_command = false);
    void SetUnitCommand(const Unit* unit, AbilityID ability, const Unit* target, CommandPriority priority, bool queued_command = false);
    void SetUnitsCommand(Units units, AbilityID ability, CommandPriority priority, bool queued_command = false);
    void SetUnitsCommand(Units units, AbilityID ability, Point2D point, CommandPriority priority, bool queued_command = false);
    void SetUnitsCommand(Units units, AbilityID ability, const Unit* target, CommandPriority priority, bool queued_command = false);
    void ParseUnitCommands();
};


}