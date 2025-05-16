#pragma once
#include "utility.h"


namespace sc2
{

class Mediator;


struct BuildOrderConditionArgData
{
	float time = 0;
	UNIT_TYPEID unitId = UNIT_TYPEID::INVALID;
	int amount = 0;
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
	UPGRADE_ID upgradeId = UPGRADE_ID::INVALID;
	UNIT_TYPEID unitId = UNIT_TYPEID::INVALID;
	std::vector<UNIT_TYPEID> unitIds;
	int amount = 0;
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
	three_gate_robo,
	pvp_openner
};



class BuildOrderManager
{
public:
	Mediator* mediator;
	BuildOrder current_build_order = BuildOrder::blank;
	std::vector<BuildOrderData> build_order;
	int build_order_step = 0;
	bool run_build_order = true;

	BuildOrderManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void CheckBuildOrder();
	void PauseBuildOrder();
	void UnpauseBuildOrder();
	// Build order condition functions
	bool TimePassed(BuildOrderConditionArgData);
	bool NumWorkers(BuildOrderConditionArgData);
	bool HasBuilding(BuildOrderConditionArgData);
	bool HasBuildingStarted(BuildOrderConditionArgData);
	bool IsResearching(BuildOrderConditionArgData);
	bool HasGas(BuildOrderConditionArgData);
	bool HasUnits(BuildOrderConditionArgData);

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
	bool BalanceIncome(BuildOrderResultArgData);
	bool TrainUnit(BuildOrderResultArgData);
	bool TrainStalker(BuildOrderResultArgData);
	bool TrainAdept(BuildOrderResultArgData);
	bool TrainZealot(BuildOrderResultArgData);
	bool TrainSentry(BuildOrderResultArgData);
	bool TrainOracle(BuildOrderResultArgData);
	bool TrainPrism(BuildOrderResultArgData);
	bool TrainImmortal(BuildOrderResultArgData);
	bool TrainObserver(BuildOrderResultArgData);
	bool ChronoBuilding(BuildOrderResultArgData);
	bool OptionalChronoBuilding(BuildOrderResultArgData);
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
	bool ZealotSimpleAttack(BuildOrderResultArgData);
	bool ZealotDoubleprong(BuildOrderResultArgData);
	bool ZealotDoubleprongLarge(BuildOrderResultArgData);
	bool MicroOracles(BuildOrderResultArgData);
	bool SpawnUnits(BuildOrderResultArgData);
	bool ResearchAttackOne(BuildOrderResultArgData);
	bool ResearchAttackTwo(BuildOrderResultArgData);
	bool ResearchShieldsOne(BuildOrderResultArgData);
	bool ResearchAirAttackOne(BuildOrderResultArgData);
	bool WarpInUnits(BuildOrderResultArgData);
	bool PullOutOfGas(BuildOrderResultArgData);
	bool IncreaseExtraPylons(BuildOrderResultArgData);
	bool StartChargelotAllIn(BuildOrderResultArgData);
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
	bool CheckForEarlyPool(BuildOrderResultArgData);
	bool BuildNaturalDefensiveBuilding(BuildOrderResultArgData);
	bool ReturnToOracleGatewaymanPvZ(BuildOrderResultArgData);
	bool ReturnTo4GateBlink(BuildOrderResultArgData);
	bool SetUnitProduction(BuildOrderResultArgData);
	bool CancelWarpgateUnitProduction(BuildOrderResultArgData);
	bool CancelStargateUnitProduction(BuildOrderResultArgData);
	bool SetWarpInAtProxy(BuildOrderResultArgData);
	bool AddToNaturalDefense(BuildOrderResultArgData);
	bool CheckTankCount(BuildOrderResultArgData);
	bool CheckForProxyRax(BuildOrderResultArgData);
	bool CheckProtossOpenning(BuildOrderResultArgData);
	bool ScoutBases(BuildOrderResultArgData);

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
	void SetPvPOpenner();
	void Set2GateProxyRobo();
	void Set1GateExpand();

	void SetRecessedCannonRush();


	void SetEarlyPoolInterrupt();
	void SetChargeAllInInterrupt();
	void SetChargeTransition();
	void SetMinorProxyRaxResponse();
	void SetMajorProxyRaxResponse();
	void SetWorkerRushDefense();

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
			int mins = (int)std::floor(condition_arg.time / 60);
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
			str += UnitTypeToName(condition_arg.unitId);
			str += " is built, ";
		}
		else if (condition == &BuildOrderManager::HasBuildingStarted)
		{
			str += "a ";
			str += UnitTypeToName(condition_arg.unitId);
			str += " has started building, ";
		}
		else if (condition == &BuildOrderManager::IsResearching)
		{
			str += UnitTypeToName(condition_arg.unitId);
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
			str += UnitTypeToName(condition_arg.unitId);
			str += "s are made, ";
		}
		else
		{
			str += "unknown condition, ";
		}



		// Result
		if (result == &BuildOrderManager::BuildBuilding)
		{
			str += "build a ";
			str += UnitTypeToName(result_arg.unitId);
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
				str += UnitTypeToName(building);
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
		else if (result == &BuildOrderManager::BalanceIncome)
		{
			str += "balance income";
		}
		else if (result == &BuildOrderManager::TrainUnit)
		{
			str += "build ";
			str += std::to_string(result_arg.amount);
			str += ' ';
			str += UnitTypeToName(result_arg.unitId);
			if (result_arg.amount > 1)
				str += 's';
		}
		else if (result == &BuildOrderManager::TrainStalker)
		{
			str += "build ";
			str += std::to_string(result_arg.amount);
			str += " stalker";
			if (result_arg.amount > 1)
				str += 's';
		}
		else if (result == &BuildOrderManager::TrainAdept)
		{
			str += "build ";
			str += std::to_string(result_arg.amount);
			str += " adept";
			if (result_arg.amount > 1)
				str += 's';
		}
		else if (result == &BuildOrderManager::TrainZealot)
		{
			str += "build ";
			str += std::to_string(result_arg.amount);
			str += " zealot";
			if (result_arg.amount > 1)
				str += 's';
		}
		else if (result == &BuildOrderManager::TrainSentry)
		{
			str += "build ";
			str += std::to_string(result_arg.amount);
			str += " sentry";
			if (result_arg.amount > 1)
				str += 's';
		}
		else if (result == &BuildOrderManager::TrainAdept)
		{
			str += "build ";
			str += std::to_string(result_arg.amount);
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
		else if (result == &BuildOrderManager::TrainImmortal)
		{
			str += "build an immortal";
		}
		else if (result == &BuildOrderManager::ChronoBuilding)
		{
			str += "chrono ";
			str += UnitTypeToName(result_arg.unitId);
		}
		else if (result == &BuildOrderManager::OptionalChronoBuilding)
		{
			str += "chrono ";
			str += UnitTypeToName(result_arg.unitId);
			str += "if energy is available";
			}
		else if (result == &BuildOrderManager::ResearchWarpgate)
		{
			str += "research warpgate";
		}
		else if (result == &BuildOrderManager::BuildProxy)
		{
			str += "build a proxy ";
			str += UnitTypeToName(result_arg.unitId);
		}
		else if (result == &BuildOrderManager::BuildProxyMulti)
		{
			str += "build a proxy ";
			for (const auto &building : result_arg.unitIds)
			{
				str += UnitTypeToName(building);
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
			str += UnitTypeToName(result_arg.unitId);
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
			str += UnitTypeToName(result_arg.unitId);
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
			str += UnitTypeToName(result_arg.unitId);
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
		else if (result == &BuildOrderManager::StartChargelotAllIn)
		{
			str += "start chargelot all in";
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
		else if (result == &BuildOrderManager::CheckForEarlyPool)
		{
			str += "check for early pool";
		}
		else if (result == &BuildOrderManager::BuildNaturalDefensiveBuilding)
		{
			str += "build defensive building at natural";
		}
		else if (result == &BuildOrderManager::ReturnToOracleGatewaymanPvZ)
		{
			str += "return to oracle gatewayman PvZ";
		}
		else if (result == &BuildOrderManager::ReturnTo4GateBlink)
		{
			str += "return to 4 gate blink";
			}
		else if (result == &BuildOrderManager::SetUnitProduction)
		{
			str += "set unit production";
		}
		else if (result == &BuildOrderManager::CancelWarpgateUnitProduction)
		{
			str += "cancel warpgate unit production";
		}
		else if (result == &BuildOrderManager::CancelStargateUnitProduction)
		{
			str += "cancel stargate unit production";
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
		else if (result == &BuildOrderManager::CheckForProxyRax)
		{
			str += "check for proxy rax";
		}
		else if (result == &BuildOrderManager::CheckProtossOpenning)
		{
			str += "check protoss openning";
		}
		else if (result == &BuildOrderManager::ScoutBases)
		{
			str += "scout bases";
			}

		return str;
	}
};

}