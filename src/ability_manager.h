#pragma once

#include "utility.h"



namespace sc2
{

class Mediator;



class AbilityManager
{
public:
	Mediator* mediator;
	// oracles
	std::map<const Unit*, bool> oracle_beam_status;
	std::map<const Unit*, int> previous_oracle_energy;
	std::map<const Unit*, ABILITY_ID> oracle_order;
	std::map<const Unit*, bool> oracle_casting;
	std::map<const Unit*, bool> oracle_revelation_off_cooldown;
	std::map<const Unit*, float> last_time_oracle_revealed;
	// TODO maybe add oracle attack tracker

	// stalkers
	std::map<const Unit*, Point2D> previous_stalker_position;
	std::map<const Unit*, int> stalkers_ordered_to_blink;
	std::map<const Unit*, bool> stalker_blink_off_cooldown;
	std::map<const Unit*, float> last_time_stalker_blinked;
	// add sentry, high templar, dark temlplar, phoenix, mothership, nexus
	float last_time_nexus_recalled = 0;
	float last_time_nexus_energy_recharged = 0;

	AbilityManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	bool IsOracleBeamOn(const Unit*);
	bool IsOracleCasting(const Unit*);
	void UpdateOracleInfo();
	void SetOracleOrder(const Unit*, ABILITY_ID);
	void TurnOffOracle(const Unit*);

	bool IsStalkerBlinkOffCooldown(const Unit*);
	void UpdateStalkerInfo();
	void SetStalkerOrder(const Unit*);

	bool NexusRecallOffCooldown();
	bool NexusEnergyRechargeOffCooldown();
	void SetNexusRecallCooldown(float);
	void SetEnergyRechargeCooldown(float);

	void OnUnitCreated(const Unit*);
	void OnUnitDestroyed(const Unit*);
	void OnUpgradeCompleted(UPGRADE_ID);
};


}