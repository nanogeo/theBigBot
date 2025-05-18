#pragma once

#include "army_group.h"
#include "utility.h"


namespace sc2
{

class Mediator;

struct ActionArgData
{
	int index = 0;
	const Unit* unit = nullptr;
	UNIT_TYPEID unitId = UNIT_TYPEID::INVALID;
	std::vector<UNIT_TYPEID> unitIds;
	UPGRADE_ID upgradeId = UPGRADE_ID::INVALID;
	Point2D position = Point2D(0, 0);
	ArmyGroup* army_group = nullptr;
	ActionArgData() {}
	ActionArgData(UNIT_TYPEID y, Point2D z)
	{
		unitId = y;
		position = z;
	}
	ActionArgData(const Unit* x, UNIT_TYPEID y, Point2D z)
	{
		unit = x;
		unitId = y;
		position = z;
	}
	ActionArgData(const Unit* x)
	{
		unit = x;
	}
	ActionArgData(const Unit* x, UNIT_TYPEID y)
	{
		unit = x;
		unitId = y;
	}
	ActionArgData(ArmyGroup* x)
	{
		army_group = x;
	}
	ActionArgData(const Unit* x, std::vector<UNIT_TYPEID> y, Point2D z, int i)
	{
		unit = x;
		unitIds = y;
		position = z;
		index = i;
	}
	ActionArgData(const Unit* x, UNIT_TYPEID y, Point2D z, int time)
	{
		unit = x;
		unitId = y;
		position = z;
		index = time;
	}
	ActionArgData(std::vector<UNIT_TYPEID> x)
	{
		unitIds = x;
	}
	ActionArgData(int x)
	{
		index = x;
	}
};

struct ActionData;


class ActionManager
{
public:
	Mediator* mediator;
	std::vector<ActionData*> active_actions;

	ActionManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void ProcessActions();
	void AddAction(ActionData*);

	bool ActionBuildBuilding(ActionArgData*);
	bool ActionBuildBuildingWhenSafe(ActionArgData*);
	bool ActionBuildBuildingMulti(ActionArgData*);
	bool ActionBuildProxyMulti(ActionArgData*);
	bool ActionScoutZerg(ActionArgData*);
	bool ActionContinueMakingWorkers(ActionArgData*);
	bool ActionContinueBuildingPylons(ActionArgData*);
	bool ActionContinueUpgrades(ActionArgData*);
	bool ActionContinueChronos(ActionArgData*);
	bool ActionContinueExpanding(ActionArgData*);
	bool ActionChronoTillFinished(ActionArgData*);
	bool ActionConstantChrono(ActionArgData*);
	bool ActionTrainFromProxyRobo(ActionArgData*);
	bool ActionZealotDoubleprong(ActionArgData*);
	bool ActionPullOutOfGas(ActionArgData*);
	bool ActionRemoveScoutToProxy(ActionArgData*); // tostring
	bool ActionDTHarassTerran(ActionArgData*); // tostring
	bool ActionUseProxyDoubleRobo(ActionArgData*);
	bool ActionAllIn(ActionArgData*); // tostring
	bool ActionAllInAttack(ActionArgData*);
	bool ActionScourMap(ActionArgData*);
	bool ActionCheckBaseForCannons(ActionArgData*);

	bool ActionAttackLine(ActionArgData*); // tostring // for testing
};

struct ActionData
{
	bool(sc2::ActionManager::*action)(ActionArgData*);
	ActionArgData* action_arg;
	ActionData(bool(sc2::ActionManager::*x)(ActionArgData*), ActionArgData* y)
	{
		action = x;
		action_arg = y;
	}
	std::string toString()
	{
		std::string str;
		if (action == &ActionManager::ActionBuildBuilding)
		{
			str += "Build a ";
			str += UnitTypeToName(action_arg->unitId);
		}
		if (action == &ActionManager::ActionBuildBuildingWhenSafe)
		{
			str += "Build a ";
			str += UnitTypeToName(action_arg->unitId);
			str += " when safe";
		}
		else if (action == &ActionManager::ActionBuildBuildingMulti)
		{
			str += "Build a ";
			for (int i = action_arg->index; i < action_arg->unitIds.size(); i++)
			{
				str += UnitTypeToName(action_arg->unitIds[i]);
				str += ", ";
			}
			str.pop_back();
			str.pop_back();
		}
		else if (action == &ActionManager::ActionBuildProxyMulti)
		{
			str += "Build a proxy ";
			for (int i = action_arg->index; i < action_arg->unitIds.size(); i++)
			{
				str += UnitTypeToName(action_arg->unitIds[i]);
				str += ", ";
			}
			str.pop_back();
			str.pop_back();
		}
		else if (action == &ActionManager::ActionScoutZerg)
		{
			str += "Scout zerg UNUSED";
		}
		else if (action == &ActionManager::ActionContinueMakingWorkers)
		{
			str += "Continue making workers";
		}
		else if (action == &ActionManager::ActionContinueBuildingPylons)
		{
			str += "Continue building pylons";
		}
		else if (action == &ActionManager::ActionContinueUpgrades)
		{
			str += "Continue getting upgrades";
		}
		else if (action == &ActionManager::ActionContinueChronos)
		{
			str += "Continue chronoing";
		}
		else if (action == &ActionManager::ActionContinueExpanding)
		{
			str += "Continue expanding";
		}
		else if (action == &ActionManager::ActionChronoTillFinished)
		{
			str += "Chrono ";
			str += UnitTypeToName(action_arg->unitId);
			str += " till finished";
		}
		else if (action == &ActionManager::ActionConstantChrono)
		{
			str += "Constant chrono on ";
			str += UnitTypeToName(action_arg->unitId);
		}
		else if (action == &ActionManager::ActionTrainFromProxyRobo)
		{
			str += "Train units from proxy robo";
		}
		else if (action == &ActionManager::ActionZealotDoubleprong)
		{
			str += "Zealot double prong";
		}
		else if (action == &ActionManager::ActionPullOutOfGas)
		{
			str += "pull out of gas";
		}
		else if (action == &ActionManager::ActionUseProxyDoubleRobo)
		{
			str += "Build ";
			if (action_arg->unitIds.size() > 0)
			{
				for (const auto &unit : action_arg->unitIds)
				{
					str += UnitTypeToName(unit);
					str += ", ";
				}
			}
			else
			{
				str += "immortals ";
			}
			str += "from proxy robos";
		}
		else if (action == &ActionManager::ActionAllInAttack)
		{
			str += "All in attack";
		}
		else if (action == &ActionManager::ActionScourMap)
		{
			str += "Scour map";
		}
		else if (action == &ActionManager::ActionCheckBaseForCannons)
		{
			str += "Check base for cannons";
		}
		return str;
	}
};


}