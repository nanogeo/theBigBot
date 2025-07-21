
#include "mediator.h"
#include "ability_manager.h"


namespace sc2
{

void AbilityManager::UpdatedAbilityInfo()
{
	UpdateOracleInfo();
	UpdateStalkerInfo();
}

bool AbilityManager::IsOracleBeamOn(const Unit* unit) const
{
	if (unit == nullptr || unit->unit_type != ORACLE)
		return false;
	if (oracle_beam_status.count(unit) == 0)
	{
		mediator->LogMinorError();
		std::cerr << "Error oracle not found in oracle_beam_status" << std::endl;
		return false;
	}
	return oracle_beam_status.at(unit);
}

bool AbilityManager::IsOracleCasting(const Unit* unit) const
{
	if (unit == nullptr || unit->unit_type != ORACLE)
		return false;
	if (oracle_casting.count(unit) == 0)
	{
		mediator->LogMinorError();
		std::cerr << "Error oracle not found in oracle_casting" << std::endl;
		return false;
	}
	return oracle_casting.at(unit);
}

void AbilityManager::UpdateOracleInfo() 
{
	if (previous_oracle_energy.empty())
		return;
	for (auto &oracle : previous_oracle_energy)
	{
		// check revelation cooldown
		if (last_time_oracle_revealed[oracle.first] + CD_REVELATION <= mediator->GetCurrentTime())
		{
			oracle_revelation_off_cooldown[oracle.first] = true;
		}
		if (oracle_order.count(oracle.first) > 0)
		{
			switch (oracle_order[oracle.first])
			{
			case A_ORACLE_BEAM_ON:
				if (oracle.first->energy <= oracle.second - ENERGY_COST_PULSAR_BEAM + 1) // +1 because energy is a float for some reason
				{
					oracle_beam_status[oracle.first] = true;
					oracle_order.erase(oracle.first);
				}
				break;
			case A_ORACLE_BEAM_OFF:
				oracle_beam_status[oracle.first] = false;
				oracle_order.erase(oracle.first);
				break;
			case A_REVELATION:
				if (oracle.first->energy <= oracle.second - ENERGY_COST_REVELATION)
				{
					oracle_casting[oracle.first] = false;
					oracle_order.erase(oracle.first);
					last_time_oracle_revealed[oracle.first] = mediator->GetCurrentTime();
				}
				break;
			}
		}
		else if (oracle.first->energy <= 2)
		{
			oracle_beam_status[oracle.first] = false;
		}
		oracle.second = (int)oracle.first->energy;
	}
}

void AbilityManager::TurnOffOracle(const Unit* unit)
{
	oracle_beam_status[unit] = false;
}

void AbilityManager::SetOracleOrder(const Unit* unit, ABILITY_ID ability)
{
	oracle_order[unit] = ability;
	if (ability == A_REVELATION)
		oracle_casting[unit] = true;
}

bool AbilityManager::IsStalkerBlinkOffCooldown(const Unit* unit) const
{
	if (unit == nullptr || unit->unit_type != STALKER)
		return false;
	if (stalker_blink_off_cooldown.count(unit) == 0)
	{
		mediator->LogMinorError();
		std::cerr << "Error stalker not found in stalker_blink_off_cooldown" << std::endl;
		return false;
	}
	return stalker_blink_off_cooldown.at(unit);
}

void AbilityManager::UpdateStalkerInfo()
{
	if (previous_stalker_position.empty())
		return;
	for (auto& stalker : previous_stalker_position)
	{
		// check blink cooldown
		if (mediator->CheckUpgrade(U_BLINK) && last_time_stalker_blinked[stalker.first] + 7 <= mediator->GetCurrentTime())
		{
			stalker_blink_off_cooldown[stalker.first] = true;
		}
		// check if blink was cast
		if (Distance2D(stalker.first->pos, stalker.second) > 2)
		{
			// stalker moved a long distance since last frame
			if (stalkers_ordered_to_blink[stalker.first] != 0 && 
				stalkers_ordered_to_blink[stalker.first] + 5 >= mediator->GetGameLoop()) // 5 frame buffer for blink
			{
				stalkers_ordered_to_blink[stalker.first] = 0;
				stalker_blink_off_cooldown[stalker.first] = false;
				last_time_stalker_blinked[stalker.first] = mediator->GetCurrentTime();
			}
		}
		// update stalker position
		stalker.second = stalker.first->pos;
	}
}

void AbilityManager::SetStalkerOrder(const Unit* unit)
{
	stalkers_ordered_to_blink[unit] = mediator->GetGameLoop();
}

bool AbilityManager::NexusRecallOffCooldown() const
{
	return mediator->GetCurrentTime() > last_time_nexus_recalled + CD_NEXUS_RECALL;
}

bool AbilityManager::NexusEnergyRechargeOffCooldown() const
{
	return mediator->GetCurrentTime() > last_time_nexus_energy_recharged + CD_ENERGY_RECHARGE;
}

void AbilityManager::SetNexusRecallCooldown(float time)
{
	last_time_nexus_recalled = time;
}

void AbilityManager::SetEnergyRechargeCooldown(float time)
{
	last_time_nexus_energy_recharged = time;
}

void AbilityManager::OnUnitCreated(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case ORACLE:
		oracle_beam_status[unit] = false;
		previous_oracle_energy[unit] = (int)unit->energy;
		oracle_casting[unit] = false;
		oracle_revelation_off_cooldown[unit] = true;
		last_time_oracle_revealed[unit] = 0;
		break;
	case STALKER:
		previous_stalker_position[unit] = unit->pos;
		if (mediator->CheckUpgrade(U_BLINK))
			stalker_blink_off_cooldown[unit] = true;
		else
			stalker_blink_off_cooldown[unit] = false;
		last_time_stalker_blinked[unit] = 0;
		break;
	}
}

void AbilityManager::OnUnitDestroyed(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
	case ORACLE:
		oracle_beam_status.erase(unit);
		previous_oracle_energy.erase(unit);
		oracle_order.erase(unit);
		oracle_casting.erase(unit);
		oracle_revelation_off_cooldown.erase(unit);
		last_time_oracle_revealed.erase(unit);
		break;
	case STALKER:
		previous_stalker_position.erase(unit);
		stalkers_ordered_to_blink.erase(unit);
		stalker_blink_off_cooldown.erase(unit);
		last_time_stalker_blinked.erase(unit);
		break;
	}
}

void AbilityManager::OnUpgradeCompleted(UPGRADE_ID upgrade)
{
	switch (upgrade)
	{
	case U_BLINK:
		for (auto& stalker : stalker_blink_off_cooldown)
		{
			stalker.second = true;
		}
		break;
	}
}

}