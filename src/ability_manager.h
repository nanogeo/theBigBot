#pragma once

#include "utility.h"



namespace sc2
{

class Mediator;



class AbilityManager
{
private:
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
	std::map<const Unit*, uint32_t> stalkers_ordered_to_blink;
	std::map<const Unit*, bool> stalker_blink_off_cooldown;
	std::map<const Unit*, float> last_time_stalker_blinked;

	// adept
	std::map<const Unit*, std::pair<const Unit*, uint32_t>> adept_shade_info; // adept -> <shade, frame shade expires>
	std::map<const Unit*, float> last_time_adept_shaded;

	// add sentry, high templar, dark temlplar, phoenix, mothership, nexus
	float last_time_nexus_recalled = 0;
	float last_time_nexus_energy_recharged = 0;

	void UpdateOracleInfo();
	void UpdateStalkerInfo();

public:
	AbilityManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void UpdatedAbilityInfo();

	bool IsOracleBeamOn(const Unit*) const;
	bool IsOracleCasting(const Unit*) const;
	void SetOracleOrder(const Unit*, ABILITY_ID);
	void TurnOffOracle(const Unit*);

	bool IsStalkerBlinkOffCooldown(const Unit*) const;
	void SetStalkerOrder(const Unit*);

	bool IsAdeptShadeOffCooldown(const Unit*) const;
	void SetAdeptShaded(const Unit*, const Unit*);
	std::pair<const Unit*, uint32_t> GetAdeptShadeInfo(const Unit*) const;

	bool NexusRecallOffCooldown() const;
	bool NexusEnergyRechargeOffCooldown() const;
	void SetNexusRecallCooldown(float);
	void SetEnergyRechargeCooldown(float);

	void OnUnitCreated(const Unit*);
	void OnUnitDestroyed(const Unit*);
	void OnUpgradeCompleted(UPGRADE_ID);
};


}