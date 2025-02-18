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
    const Unit* target_tag;
    UnitCommand()
    {
        target_tag = NULL;
        target_point = Point2D(0, 0);
    }
    UnitCommand(AbilityID ability)
    {
        this->ability = ability;
        target_tag = NULL;
        target_point = Point2D(0, 0);
    }
    UnitCommand(AbilityID ability, Point2D target)
    {
        this->ability = ability;
        target_point = target;
        target_tag = NULL;
    }
    UnitCommand(AbilityID ability, const Unit* target)
    {
        this->ability = ability;
        target_tag = target;
        target_point = Point2D(0, 0);
    }
    bool operator==(UnitCommand u)
    {
        if (ability == u.ability && (target_point == u.target_point && target_tag == u.target_tag)) 
            return true;
        else
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

    void SetUnitCommand(const Unit* unit, AbilityID ability, bool queued_command = false);
    void SetUnitCommand(const Unit* unit, AbilityID ability, Point2D point, bool queued_command = false);
    void SetUnitCommand(const Unit* unit, AbilityID ability, const Unit* target, bool queued_command = false);
    void SetUnitsCommand(Units units, AbilityID ability, bool queued_command = false);
    void SetUnitsCommand(Units units, AbilityID ability, Point2D point, bool queued_command = false);
    void SetUnitsCommand(Units units, AbilityID ability, const Unit* target, bool queued_command = false);
    void ParseUnitCommands();
};


}