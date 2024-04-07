#pragma once

#include "army_group.h"
#include "utility.h"

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

#include "sc2api/sc2_unit_filters.h"


namespace sc2
{

class TossBot;

struct ActionArgData
{
	int index;
	const Unit* unit = NULL;
	UNIT_TYPEID unitId;
	std::vector<UNIT_TYPEID> unitIds;
	UPGRADE_ID upgradeId;
	Point2D position;
	ArmyGroup* army_group;
	ActionArgData() {}
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
	TossBot* agent;
	std::vector<ActionData*> active_actions;

	ActionManager(TossBot* agent)
	{
		this->agent = agent;
	}

	void ProcessActions();

	bool ActionBuildBuilding(ActionArgData*);
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
	bool ActionWarpInAtProxy(ActionArgData*);
	bool ActionTrainFromProxyRobo(ActionArgData*);
	bool ActionContain(ActionArgData*);
	bool ActionStalkerOraclePressure(ActionArgData*);
	bool ActionZealotDoubleprong(ActionArgData*);
	bool ActionContinueWarpingInStalkers(ActionArgData*);
	bool ActionContinueVolleyWarpingInStalkers(ActionArgData*);
	bool ActionContinueVolleyWarpingInZealots(ActionArgData*);
	bool ActionContinueBuildingCarriers(ActionArgData*);
	bool ActionPullOutOfGas(ActionArgData*);
	bool ActionRemoveScoutToProxy(ActionArgData*); // tostring
	bool ActionDTHarassTerran(ActionArgData*); // tostring
	bool ActionUseProxyDoubleRobo(ActionArgData*);
	bool ActionAllIn(ActionArgData*); // tostring
	bool ActionAllInAttack(ActionArgData*);
	bool ActionScourMap(ActionArgData*);

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
			str += Utility::UnitTypeIdToString(action_arg->unitId);
		}
		else if (action == &ActionManager::ActionBuildBuildingMulti)
		{
			str += "Build a ";
			for (int i = action_arg->index; i < action_arg->unitIds.size(); i++)
			{
				str += Utility::UnitTypeIdToString(action_arg->unitIds[i]);
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
				str += Utility::UnitTypeIdToString(action_arg->unitIds[i]);
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
			str += Utility::UnitTypeIdToString(action_arg->unitId);
			str += " till finished";
		}
		else if (action == &ActionManager::ActionConstantChrono)
		{
			str += "Constant chrono on ";
			str += Utility::UnitTypeIdToString(action_arg->unitId);
		}
		else if (action == &ActionManager::ActionWarpInAtProxy)
		{
			str += "Warp in at proxy";
		}
		else if (action == &ActionManager::ActionTrainFromProxyRobo)
		{
			str += "Train units from proxy robo";
		}
		else if (action == &ActionManager::ActionContain)
		{
			str += "Contain";
		}
		else if (action == &ActionManager::ActionStalkerOraclePressure)
		{
			str += "Stalker Oracle pressure";
		}
		else if (action == &ActionManager::ActionZealotDoubleprong)
		{
			str += "Zealot double prong";
		}
		else if (action == &ActionManager::ActionContinueVolleyWarpingInStalkers)
		{
			str += "Continue warping in stalkers";
		}
		else if (action == &ActionManager::ActionContinueVolleyWarpingInZealots)
		{
			str += "Continue warping in zealots";
		}
		else if (action == &ActionManager::ActionContinueBuildingCarriers)
		{
			str += "Continue building carriers";
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
					str += Utility::UnitTypeIdToString(unit);
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
		return str;
	}
};


}