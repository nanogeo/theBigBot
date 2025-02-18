#pragma once
#include "utility.h"

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

#include "sc2api/sc2_unit_filters.h"


namespace sc2
{

class Mediator;


struct BuildOrderConditionArgData
{
	float time;
	UNIT_TYPEID unitId;
	int amount;
	BuildOrderConditionArgData() {};
	BuildOrderConditionArgData(float x)
	{
		time = x;
	}
	BuildOrderConditionArgData(UNIT_TYPEID x)
	{
		unitId = x;
	}
	BuildOrderConditionArgData(int x)
	{
		amount = x;
	}
	BuildOrderConditionArgData(UNIT_TYPEID x, int y)
	{
		unitId = x;
		amount = y;
	}
};

struct BuildOrderResultArgData
{
	UPGRADE_ID upgradeId;
	UNIT_TYPEID unitId;
	std::vector<UNIT_TYPEID> unitIds;
	int amount;
	BuildOrderResultArgData() {};
	BuildOrderResultArgData(UPGRADE_ID x)
	{
		upgradeId = x;
	}
	BuildOrderResultArgData(UNIT_TYPEID x)
	{
		unitId = x;
	}
	BuildOrderResultArgData(std::vector<UNIT_TYPEID> x)
	{
		unitIds = x;
	}
	BuildOrderResultArgData(int x)
	{
		amount = x;
	}
	BuildOrderResultArgData(UNIT_TYPEID x, int y)
	{
		unitId = x;
		amount = y;
	}
};

struct BuildOrderData;

enum BuildOrder {
	blank,
	testing,
	blink_proxy_robo_pressure,
	oracle_gatewayman_pvz,
	four_gate_blink,
	chargelot_allin,
	chargelot_allin_old,
	four_gate_adept_pressure,
	fastest_dts,
	proxy_double_robo,
	recessed_cannon_rush,
	cannon_rush_terran,
	three_gate_robo
};



class BuildOrderManager
{
public:
	Mediator* mediator;
	BuildOrder current_build_order;
	std::vector<BuildOrderData> build_order;
	int build_order_step = 0;

	BuildOrderManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void CheckBuildOrder();
	// Build order condition functions
	bool TimePassed(BuildOrderConditionArgData);
	bool NumWorkers(BuildOrderConditionArgData);
	bool HasBuilding(BuildOrderConditionArgData);
	bool HasBuildingStarted(BuildOrderConditionArgData);
	bool IsResearching(BuildOrderConditionArgData);
	bool HasGas(BuildOrderConditionArgData);
	bool HasUnits(BuildOrderConditionArgData);
	bool ReadyToScour(BuildOrderConditionArgData); // to string

	// Build order results
	bool BuildBuilding(BuildOrderResultArgData);
	bool BuildFirstPylon(BuildOrderResultArgData);
	bool BuildBuildingMulti(BuildOrderResultArgData);
	bool Scout(BuildOrderResultArgData);
	bool CannonRushProbe1(BuildOrderResultArgData);
	bool CutWorkers(BuildOrderResultArgData);
	bool UncutWorkers(BuildOrderResultArgData);
	bool ImmediatelySaturateGasses(BuildOrderResultArgData);
	bool CancelImmediatelySaturateGasses(BuildOrderResultArgData);
	bool ImmediatelySemiSaturateGasses(BuildOrderResultArgData);
	bool CancelImmediatelySemiSaturateGasses(BuildOrderResultArgData);
	bool TrainStalker(BuildOrderResultArgData);
	bool TrainAdept(BuildOrderResultArgData);
	bool TrainZealot(BuildOrderResultArgData);
	bool TrainOracle(BuildOrderResultArgData);
	bool TrainPrism(BuildOrderResultArgData);
	bool TrainObserver(BuildOrderResultArgData);
	bool ChronoBuilding(BuildOrderResultArgData);
	bool ResearchWarpgate(BuildOrderResultArgData);
	bool BuildProxy(BuildOrderResultArgData);
	bool BuildProxyMulti(BuildOrderResultArgData);
	bool ResearchBlink(BuildOrderResultArgData);
	bool ResearchCharge(BuildOrderResultArgData);
	bool ResearchGlaives(BuildOrderResultArgData);
	bool ResearchDTBlink(BuildOrderResultArgData);
	bool ChronoTillFinished(BuildOrderResultArgData);
	bool ContinueBuildingPylons(BuildOrderResultArgData);
	bool ContinueMakingWorkers(BuildOrderResultArgData);
	bool ContinueUpgrades(BuildOrderResultArgData);
	bool ContinueChronos(BuildOrderResultArgData);
	bool ContinueExpanding(BuildOrderResultArgData);
	bool TrainFromProxy(BuildOrderResultArgData);
	bool ContinueChronoProxyRobo(BuildOrderResultArgData);
	bool Contain(BuildOrderResultArgData);
	bool StalkerOraclePressure(BuildOrderResultArgData);
	bool ZealotDoubleprong(BuildOrderResultArgData);
	bool ZealotDoubleprongLarge(BuildOrderResultArgData);
	bool MicroOracles(BuildOrderResultArgData);
	bool OracleHarass(BuildOrderResultArgData);
	bool SpawnUnits(BuildOrderResultArgData);
	bool ResearchAttackOne(BuildOrderResultArgData);
	bool ResearchAttackTwo(BuildOrderResultArgData);
	bool ResearchShieldsOne(BuildOrderResultArgData);
	bool ResearchAirAttackOne(BuildOrderResultArgData);
	bool WarpInUnits(BuildOrderResultArgData);
	bool PullOutOfGas(BuildOrderResultArgData);
	bool IncreaseExtraPylons(BuildOrderResultArgData);
	bool MicroChargelotAllin(BuildOrderResultArgData);
	bool RemoveScoutToProxy(BuildOrderResultArgData);
	bool SafeRallyPoint(BuildOrderResultArgData);
	bool DTHarass(BuildOrderResultArgData);
	bool UseProxyDoubleRobo(BuildOrderResultArgData);
	bool MicroImmortalDrop(BuildOrderResultArgData);
	bool ProxyDoubleRoboAllIn(BuildOrderResultArgData);
	bool DefendThirdBase(BuildOrderResultArgData);
	bool SetDoorGuard(BuildOrderResultArgData);
	bool AdeptDefendBaseTerran(BuildOrderResultArgData);
	bool StalkerDefendBaseTerran(BuildOrderResultArgData);
	bool StartFourGateBlinkPressure(BuildOrderResultArgData);
	bool SendCannonRushTerranProbe1(BuildOrderResultArgData);
	bool SendCannonRushTerranProbe2(BuildOrderResultArgData);
	bool CannonRushAttack(BuildOrderResultArgData);
	bool SendAllInAttack(BuildOrderResultArgData);
	bool SendAdeptHarassProtoss(BuildOrderResultArgData);
	bool ScourMap(BuildOrderResultArgData);
	bool CheckForEarlyPool(BuildOrderResultArgData);
	bool BuildNaturalDefensiveBuilding(BuildOrderResultArgData);
	bool ReturnToMainBuild(BuildOrderResultArgData);
	bool SetUnitProduction(BuildOrderResultArgData);
	bool CancelWarpgateUnitProduction(BuildOrderResultArgData);
	bool SetWarpInAtProxy(BuildOrderResultArgData);
	bool AddToNaturalDefense(BuildOrderResultArgData);
	bool CheckTankCount(BuildOrderResultArgData);

	bool SpawnArmy(BuildOrderResultArgData); // testing only // to string
	bool AttackLine(BuildOrderResultArgData); // testing only // to string

	bool RemoveProbe(BuildOrderResultArgData);

	void SetBuildOrder(BuildOrder);
	void SetBlank();
	void SetTesting();
	void SetBlinkProxyRoboPressureBuild();
	void SetOracleGatewaymanPvZ();
	void Set4GateBlink();
	void SetChargelotAllin();
	void SetChargelotAllinOld();
	void Set4GateAdept();
	void SetFastestDTsPvT();
	void SetProxyDoubleRobo();
	void SetCannonRushTerran();
	void SetThreeGateRobo();

	void SetRecessedCannonRush();


	void SetEarlyPoolInterrupt();
	void SetChargeTransition();

};


struct BuildOrderData
{
	bool(sc2::BuildOrderManager::*condition)(BuildOrderConditionArgData);
	BuildOrderConditionArgData condition_arg;
	bool(sc2::BuildOrderManager::*result)(BuildOrderResultArgData);
	BuildOrderResultArgData result_arg;
	BuildOrderData(bool(sc2::BuildOrderManager::*x)(BuildOrderConditionArgData), BuildOrderConditionArgData y, bool(sc2::BuildOrderManager::*z)(BuildOrderResultArgData), BuildOrderResultArgData a)
	{
		condition = x;
		condition_arg = y;
		result = z;
		result_arg = a;
	}
	std::string toString()
	{
		// Condition
		std::string str = "When ";
		if (condition == &BuildOrderManager::TimePassed)
		{
			str = "At ";
			int mins = std::floor(condition_arg.time / 60);
			int seconds = (int)condition_arg.time % 60;
			str += std::to_string(mins);
			str += ':';
			if (seconds < 10)
				str += '0';
			str += std::to_string(seconds);
			str += ", ";
		}
		else if (condition == &BuildOrderManager::NumWorkers)
		{
			str += "worker count = ";
			str += std::to_string(condition_arg.amount);
			str += ", ";
		}
		else if (condition == &BuildOrderManager::HasBuilding)
		{
			str += "a ";
			str += Utility::UnitTypeIdToString(condition_arg.unitId);
			str += " is built, ";
		}
		else if (condition == &BuildOrderManager::HasBuildingStarted)
		{
			str += "a ";
			str += Utility::UnitTypeIdToString(condition_arg.unitId);
			str += " has started building, ";
		}
		else if (condition == &BuildOrderManager::IsResearching)
		{
			str += Utility::UnitTypeIdToString(condition_arg.unitId);
			str += " is researching, ";
		}
		else if (condition == &BuildOrderManager::HasGas)
		{
			str += "vespene >= ";
			str += std::to_string(condition_arg.amount);
			str += ", ";
		}
		else if (condition == &BuildOrderManager::HasUnits)
		{
			str += std::to_string(condition_arg.amount);
			str += " ";
			str += Utility::UnitTypeIdToString(condition_arg.unitId);
			str += "s are made, ";
		}
		else if (condition == &BuildOrderManager::ReadyToScour)
		{
			str += "ready to scour, ";
		}
		else
		{
			str += "unknown condition, ";
		}



		// Result
		if (result == &BuildOrderManager::BuildBuilding)
		{
			str += "build a ";
			str += Utility::UnitTypeIdToString(result_arg.unitId);
		}
		else if (result == &BuildOrderManager::BuildFirstPylon)
		{
			str += "build first pylon";
		}
		else if (result == &BuildOrderManager::BuildBuildingMulti)
		{
			str += "build a ";
			for (const auto &building : result_arg.unitIds)
			{
				str += Utility::UnitTypeIdToString(building);
				str += ", ";
			}
			str.pop_back();
			str.pop_back();
		}
		else if (result == &BuildOrderManager::Scout)
		{
			str += "send scout";
		}
		else if (result == &BuildOrderManager::CannonRushProbe1)
		{
			str += "send cannon rush probe 1";
		}
		else if (result == &BuildOrderManager::CutWorkers)
		{
			str += "stop building probes";
		}
		else if (result == &BuildOrderManager::UncutWorkers)
		{
			str += "start building probes again";
		}
		else if (result == &BuildOrderManager::ImmediatelySaturateGasses)
		{
			str += "immediately saturate gasses";
		}
		else if (result == &BuildOrderManager::CancelImmediatelySaturateGasses)
		{
			str += "cancel immediately saturate gasses";
		}
		else if (result == &BuildOrderManager::ImmediatelySemiSaturateGasses)
		{
			str += "immediately semi-saturate gasses";
		}
		else if (result == &BuildOrderManager::CancelImmediatelySemiSaturateGasses)
		{
			str += "cancel immediately semi-saturate gasses";
		}
		else if (result == &BuildOrderManager::TrainStalker)
		{
			str += "build a stalker";
		}
		else if (result == &BuildOrderManager::TrainAdept)
		{
			str += "build ";
			str += result_arg.amount;
			str += " adept";
			if (result_arg.amount > 1)
				str += 's';
		}
		else if (result == &BuildOrderManager::TrainOracle)
		{
			str += "build an oracle";
		}
		else if (result == &BuildOrderManager::TrainPrism)
		{
			str += "build a warp prism";
		}
		else if (result == &BuildOrderManager::ChronoBuilding)
		{
			str += "chrono ";
			str += Utility::UnitTypeIdToString(result_arg.unitId);
		}
		else if (result == &BuildOrderManager::ResearchWarpgate)
		{
			str += "research warpgate";
		}
		else if (result == &BuildOrderManager::BuildProxy)
		{
			str += "build a proxy ";
			str += Utility::UnitTypeIdToString(result_arg.unitId);
		}
		else if (result == &BuildOrderManager::BuildProxyMulti)
		{
			str += "build a proxy ";
			for (const auto &building : result_arg.unitIds)
			{
				str += Utility::UnitTypeIdToString(building);
				str += ", ";
			}
			str.pop_back();
			str.pop_back();
		}
		else if (result == &BuildOrderManager::ResearchBlink)
		{
			str += "research blink";
		}
		else if (result == &BuildOrderManager::ResearchCharge)
		{
			str += "research charge";
		}
		else if (result == &BuildOrderManager::ResearchGlaives)
		{
			str += "research glaives";
		}
		else if (result == &BuildOrderManager::ResearchDTBlink)
		{
			str += "research dt blink";
		}
		else if (result == &BuildOrderManager::ChronoTillFinished)
		{
			str += "chrono ";
			str += Utility::UnitTypeIdToString(result_arg.unitId);
			str += " till it's finished";
		}
		else if (result == &BuildOrderManager::ContinueBuildingPylons)
		{
			str += "macro pylons";
		}
		else if (result == &BuildOrderManager::ContinueMakingWorkers)
		{
			str += "macro workers";
		}
		else if (result == &BuildOrderManager::ContinueUpgrades)
		{
		str += "macro upgrades";
		}
		else if (result == &BuildOrderManager::ContinueChronos)
		{
		str += "macro chrono";
		}
		else if (result == &BuildOrderManager::ContinueExpanding)
		{
		str += "macro expanding";
		}
		else if (result == &BuildOrderManager::TrainFromProxy)
		{
			str += "train units from proxy ";
			str += Utility::UnitTypeIdToString(result_arg.unitId);
		}
		else if (result == &BuildOrderManager::ContinueChronoProxyRobo)
		{
			str += "continue chronoing proxy robo";
		}
		else if (result == &BuildOrderManager::Contain)
		{
			str += "contain";
		}
		else if (result == &BuildOrderManager::StalkerOraclePressure)
		{
			str += "stalker oracle pressure";
		}
		else if (result == &BuildOrderManager::ZealotDoubleprong)
		{
		str += "zealot double prong";
		}
		else if (result == &BuildOrderManager::MicroOracles)
		{
			str += "micro oracles";
		}
		else if (result == &BuildOrderManager::OracleHarass)
		{
		str += "oracle harass";
		}
		else if (result == &BuildOrderManager::SpawnUnits)
		{
			str += "cheat in units ";
			//add what units
		}
		else if (result == &BuildOrderManager::ResearchAttackOne)
		{
			str += "research +1 attack";
		}
		else if (result == &BuildOrderManager::ResearchAttackTwo)
		{
		str += "research +2 attack";
		}
		else if (result == &BuildOrderManager::ResearchShieldsOne)
		{
		str += "research +1 shields";
		}
		else if (result == &BuildOrderManager::ResearchAirAttackOne)
		{
		str += "research +1 air weapons";
		}
		else if (result == &BuildOrderManager::WarpInUnits)
		{
			str += "warp in ";
			str += std::to_string(result_arg.amount);
			str += " ";
			str += Utility::UnitTypeIdToString(result_arg.unitId);
			str += 's';
		}
		else if (result == &BuildOrderManager::PullOutOfGas)
		{
			str += "pull ";
			str += std::to_string(result_arg.amount);
			str += " out of gas";
		}
		else if (result == &BuildOrderManager::IncreaseExtraPylons)
		{
			str += "increase extra pylons by ";
			str += std::to_string(result_arg.amount);
		}
		else if (result == &BuildOrderManager::MicroChargelotAllin)
		{
			str += "micro chargelot allin";
		}
		else if (result == &BuildOrderManager::RemoveScoutToProxy)
		{
			str += "remove scout and send to proxy";
		}
		else if (result == &BuildOrderManager::SafeRallyPoint)
		{
			str += "change rally point to safe pos";
		}
		else if (result == &BuildOrderManager::DTHarass)
		{
			str += "start DT harass";
		}
		else if (result == &BuildOrderManager::UseProxyDoubleRobo)
		{
			str += "use proxy double robo";
		}
		else if (result == &BuildOrderManager::MicroImmortalDrop)
		{
			str += "micro immortal drop";
		}
		else if (result == &BuildOrderManager::DefendThirdBase)
		{
			str += "defend third base";
		}
		else if (result == &BuildOrderManager::SetDoorGuard)
		{
			str += "set door guard";
		}
		else if (result == &BuildOrderManager::AdeptDefendBaseTerran)
		{
			str += "set adept base defense vs terran";
		}
		else if (result == &BuildOrderManager::StalkerDefendBaseTerran)
		{
			str += "set stalker base defense vs terran";
		}
		else if (result == &BuildOrderManager::StartFourGateBlinkPressure)
		{
			str += "start 4 gate blink attack";
		}
		else if (result == &BuildOrderManager::SendCannonRushTerranProbe1)
		{
			str += "send cannon rush terran probe 1";
		}
		else if (result == &BuildOrderManager::SendCannonRushTerranProbe2)
		{
			str += "send cannon rush terran probe 2";
		}
		else if (result == &BuildOrderManager::CannonRushAttack)
		{
			str += "start cannon rush attack";
		}
		else if (result == &BuildOrderManager::SendAllInAttack)
		{
			str += "send all in attack";
		}
		else if (result == &BuildOrderManager::SendAdeptHarassProtoss)
		{
			str += "send adept harass vs protoss";
		}
		else if (result == &BuildOrderManager::ScourMap)
		{
			str += "scour map";
		}
		else if (result == &BuildOrderManager::CheckForEarlyPool)
		{
			str += "check for early pool";
		}
		else if (result == &BuildOrderManager::BuildNaturalDefensiveBuilding)
		{
			str += "build defensive building at natural";
		}
		else if (result == &BuildOrderManager::ReturnToMainBuild)
		{
			str += "return to main build";
		}
		else if (result == &BuildOrderManager::SetUnitProduction)
		{
			str += "set unit production";
		}
		else if (result == &BuildOrderManager::CancelWarpgateUnitProduction)
		{
			str += "cancel warpgate unit production";
		}
		else if (result == &BuildOrderManager::SetWarpInAtProxy)
		{
			str += "set warp in a t proxy";
		}
		else if (result == &BuildOrderManager::AddToNaturalDefense)
		{
			str += "add to natural defense";
		}
		else if (result == &BuildOrderManager::CheckTankCount)
		{
			str += "check tank count";
		}

		return str;
	}
};

}