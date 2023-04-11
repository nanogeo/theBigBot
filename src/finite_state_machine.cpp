#pragma once
#include "finish_state_machine.h"
#include "locations.h"

#include <iostream>
#include <string>


#include "sc2api/sc2_api.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"

#include "TossBot.h"

namespace sc2 {

#pragma region State

	void State::TickState()
	{
		std::cout << "TickState called on base State class";
	}

	void State::EnterState()
	{
		std::cout << "EnterState called on base State class";
	}

	void State::ExitState()
	{
		std::cout << "ExitState called on base State class";
	}

	State* State::TestTransitions()
	{
		std::cout << "TestTransitions called on base State class";
		return NULL;
	}

	std::string State::toString()
	{
		return "State";
	}

#pragma endregion

#pragma region OracleDefend

	std::string OracleDefend::toString()
	{
		return "Oracle Defend";
	}

	void OracleDefend::TickState()
	{

		if (agent->Observation()->GetUnits(Unit::Alliance::Enemy).size() > 0 && agent->DistanceToClosest(agent->Observation()->GetUnits(Unit::Alliance::Enemy), denfensive_position) < 10)
		{
			for (int i = 0; i < oracles.size(); i++)
			{
				const Unit* oracle = oracles[i];
				float now = agent->Observation()->GetGameLoop() / 22.4;
				bool weapon_ready = now - time_last_attacked[i] > .61;
				bool beam_active = false;
				bool beam_activatable = false;
				for (const auto & ability : agent->Query()->GetAbilitiesForUnit(oracle).abilities)
				{
					if (ability.ability_id.ToType() == ABILITY_ID::BEHAVIOR_PULSARBEAMOFF)
					{
						beam_active = true;
					}
				}

				if (!beam_active && oracle->energy >= 40)
					beam_activatable = true;

				const Unit* closest_unit = agent->ClosestTo(agent->Observation()->GetUnits(Unit::Alliance::Enemy), oracle->pos);
				if (beam_active)
				{
					if (Distance2D(oracle->pos, closest_unit->pos) > 4)
					{
						float dist = Distance2D(oracle->pos, closest_unit->pos);
						agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, agent->PointBetween(oracle->pos, closest_unit->pos, dist + 1), false);
					}
					else if (weapon_ready)
					{
						agent->Actions()->UnitCommand(oracle, ABILITY_ID::ATTACK_ATTACK, closest_unit, false);
						time_last_attacked[i] = now;
						has_attacked[i] = false;
						agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 255, 0));
					}
					else if (has_attacked[i])
					{
						agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, false);
						agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 0, 255));
					}
					else
					{
						agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(255, 0, 0));
					}
				}
				else if (beam_activatable)
				{
					if (Distance2D(oracle->pos, closest_unit->pos) < 3)
					{
						agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMON, false);
					}
					else
					{
						agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, false);
					}
				}
				else
				{
					agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, closest_unit->pos, false);
					agent->Debug()->DebugSphereOut(oracle->pos, 2, Color(0, 255, 0));
				}
				agent->Debug()->DebugTextOut(std::to_string(now - time_last_attacked[i]), Point2D(.7, .7), Color(0, 255, 255), 20);
				agent->Debug()->DebugTextOut(std::to_string(agent->Observation()->GetGameLoop()), Point2D(.7, .75), Color(0, 255, 255), 20);
			}
		}
		else
		{
			for (const auto &oracle : oracles)
			{

				for (const auto &ability : agent->Query()->GetAbilitiesForUnit(oracle).abilities)
				{
					if (ability.ability_id == ABILITY_ID::BEHAVIOR_PULSARBEAMOFF)
						agent->Actions()->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMOFF);
				}
				agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, denfensive_position);
			}
		}
	}

	void OracleDefend::EnterState()
	{
		for (const auto &oracle : oracles)
		{
			agent->Actions()->UnitCommand(oracle, ABILITY_ID::GENERAL_MOVE, denfensive_position);
		}
		std::function<void(const Unit*, float, float)> onUnitDamaged = [=](const Unit* unit, float health, float shields) {
			this->OnUnitDamagedListener(unit, health, shields);
		};
		agent->AddListenerToOnUnitDamagedEvent(onUnitDamaged);

		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		agent->AddListenerToOnUnitDestroyedEvent(onUnitDestroyed);
	}

	void OracleDefend::ExitState()
	{
		return;
	}

	State* OracleDefend::TestTransitions()
	{
		return NULL;
	}

	void OracleDefend::OnUnitDamagedListener(const Unit* unit, float health, float shields)
	{
		std::cout << agent->UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage\n";
		for (int i = 0; i < oracles.size(); i++)
		{
			if (oracles[i]->engaged_target_tag == unit->tag)
			{
				std::cout << agent->UnitTypeIdToString(unit->unit_type.ToType()) << " took " << std::to_string(health) << " damage from orale\n";
				has_attacked[i] = true;
			}
		}
	}

	void OracleDefend::OnUnitDestroyedListener(const Unit* unit)
	{
		std::cout << agent->UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed\n";
		for (int i = 0; i < oracles.size(); i++)
		{
			if (oracles[i]->engaged_target_tag == unit->tag)
			{
				std::cout << agent->UnitTypeIdToString(unit->unit_type.ToType()) << " destroyed by orale\n";
				has_attacked[i] = true;
			}
		}
	}


#pragma endregion

#pragma region OracleCoverArmy

	std::string OracleCoverArmy::toString()
	{
		return "Cover Army";
	}

#pragma endregion

#pragma region ChargeAllInMovingToWarpinSpot

	std::string ChargeAllInMovingToWarpinSpot::toString()
	{
		float time_left = state_machine->last_warp_in_time + 20 - agent->Observation()->GetGameLoop() / 22.4;
		return "ChargeAllIn looking for warp in spot " + std::to_string((int)time_left);
	}

	void ChargeAllInMovingToWarpinSpot::TickState()
	{
		// move prism to spot
		if (Distance2D(state_machine->prism->pos, state_machine->next_warp_in_location) > 1)
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->next_warp_in_location);
		for (const auto &zealot : state_machine->zealots)
		{
			if (zealot->orders.size() == 0)
			{
				Point2D pos = agent->Observation()->GetGameInfo().enemy_start_locations[0];
				agent->Actions()->UnitCommand(zealot, ABILITY_ID::ATTACK_ATTACK, pos);
			}
		}
		return;
	}

	void ChargeAllInMovingToWarpinSpot::EnterState()
	{
		// find warp in spot
		state_machine->prism_spots_index++;
		if (state_machine->prism_spots_index >= state_machine->prism_spots.size())
			state_machine->prism_spots_index = 0;
		state_machine->next_warp_in_location = state_machine->prism_spots[state_machine->prism_spots_index];
	}

	void ChargeAllInMovingToWarpinSpot::ExitState()
	{
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE);
	}

	State* ChargeAllInMovingToWarpinSpot::TestTransitions()
	{
		float time_left = state_machine->last_warp_in_time + 20 - agent->Observation()->GetGameLoop() / 22.4;
		if (time_left < 2)
			return new ChargeAllInWarpingIn(agent, state_machine);
		return NULL;
	}

#pragma endregion

#pragma region ChargeAllInWarpingIn

	std::string ChargeAllInWarpingIn::toString()
	{
		return "ChargeAllIn warping in";
	}

	void ChargeAllInWarpingIn::TickState()
	{
		for (const auto &zealot : state_machine->zealots)
		{
			if (zealot->orders.size() == 0)
			{
				Point2D pos = agent->Observation()->GetGameInfo().enemy_start_locations[0];
				agent->Actions()->UnitCommand(zealot, ABILITY_ID::ATTACK_ATTACK, pos);
			}
		}

		bool all_gates_ready = true;
		Units gates = agent->Observation()->GetUnits(IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
		for (const auto &warpgate : gates)
		{
			bool gate_ready = false;
			for (const auto &ability : agent->Query()->GetAbilitiesForUnit(warpgate).abilities)
			{
				if (ability.ability_id == ABILITY_ID::TRAINWARP_ZEALOT)
				{
					gate_ready = true;
					break;
				}
			}
			if (!gate_ready)
			{
				all_gates_ready = false;
				break;
			}
		}
		if (gates.size() > 0 && all_gates_ready && agent->CanAfford(UNIT_TYPEID::PROTOSS_ZEALOT, gates.size()))
		{
			std::vector<Point2D> spots = agent->FindWarpInSpots(agent->Observation()->GetGameInfo().enemy_start_locations[0]);
			if (spots.size() >= gates.size())
			{
				for (int i = 0; i < gates.size(); i++)
				{
					Point3D pos = Point3D(gates[i]->pos.x, gates[i]->pos.y, agent->Observation()->TerrainHeight(gates[i]->pos));
					agent->Debug()->DebugSphereOut(pos, 1, Color(255, 0, 255));
					agent->Actions()->UnitCommand(gates[i], ABILITY_ID::TRAINWARP_ZEALOT, spots[i]);
					state_machine->last_warp_in_time = agent->Observation()->GetGameLoop() / 22.4;
				}
			}
		}
	}

	void ChargeAllInWarpingIn::EnterState()
	{
		if (state_machine->prism->unit_type != UNIT_TYPEID::PROTOSS_WARPPRISMPHASING)
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE);
	}

	void ChargeAllInWarpingIn::ExitState()
	{
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MORPH_WARPPRISMTRANSPORTMODE);
	}

	State* ChargeAllInWarpingIn::TestTransitions()
	{
		float time_left = state_machine->last_warp_in_time + 20 - agent->Observation()->GetGameLoop() / 22.4;
		if (time_left < 16 && time_left > 10)
			return new ChargeAllInMovingToWarpinSpot(agent, state_machine);
		// if last warp in time >3 and < 8
		// return new ChargeAllInLookingForWarpinSpot)(
		return NULL;
	}

#pragma endregion

#pragma region ScoutZInitialMove

	void ScoutZInitialMove::TickState()
	{
		if (state_machine->scout->orders.size() == 0 || state_machine->scout->orders[0].target_pos != state_machine->current_target)
			agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::MOVE_MOVE, state_machine->current_target);
	}

	void ScoutZInitialMove::EnterState()
	{
		state_machine->current_target = state_machine->enemy_main;
	}

	void ScoutZInitialMove::ExitState()
	{
		return;
	}

	State* ScoutZInitialMove::TestTransitions()
	{
		if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 1)
			return new ScoutZScoutMain(agent, state_machine);
		return NULL;
	}

	std::string ScoutZInitialMove::toString()
	{
		return "initial move";
	}

#pragma endregion

#pragma region ScoutZScoutMain

	void ScoutZScoutMain::TickState()
	{
		if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 3)
		{
			state_machine->index++;
			if (state_machine->index < state_machine->main_scout_path.size())
				state_machine->current_target = state_machine->main_scout_path[state_machine->index];
		}
		agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::MOVE_MOVE, state_machine->current_target);
	}

	void ScoutZScoutMain::EnterState()
	{
		state_machine->index = 0;
		state_machine->current_target = state_machine->main_scout_path[0];
	}

	void ScoutZScoutMain::ExitState()
	{
		return;
	}

	State* ScoutZScoutMain::TestTransitions()
	{
		if (state_machine->index >= state_machine->main_scout_path.size())
			return new ScoutZScoutNatural(agent, state_machine);
		return NULL;
	}

	std::string ScoutZScoutMain::toString()
	{
		return "scout main";
	}

#pragma endregion

#pragma region ScoutZScoutNatural

	void ScoutZScoutNatural::TickState()
	{
		if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 3)
		{
			state_machine->index++;
			if (state_machine->index < state_machine->natural_scout_path.size())
				state_machine->current_target = state_machine->natural_scout_path[state_machine->index];
		}
		agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::MOVE_MOVE, state_machine->current_target);
	}

	void ScoutZScoutNatural::EnterState()
	{
		state_machine->index = 0;
		state_machine->current_target = state_machine->natural_scout_path[0];
	}

	void ScoutZScoutNatural::ExitState()
	{
		return;
	}

	State* ScoutZScoutNatural::TestTransitions()
	{
		if (state_machine->index >= state_machine->natural_scout_path.size())
			return new ScoutZLookFor3rd(agent, state_machine);
		return NULL;
	}

	std::string ScoutZScoutNatural::toString()
	{
		return "scout natural";
	}

#pragma endregion

#pragma region ScoutZLookFor3rd

	void ScoutZLookFor3rd::TickState()
	{
		if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 8)
		{
			state_machine->index++;
			if (state_machine->index < state_machine->possible_3rds.size())
				state_machine->current_target = state_machine->possible_3rds[state_machine->index];
		}
		agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::MOVE_MOVE, state_machine->current_target);
	}

	void ScoutZLookFor3rd::EnterState()
	{
		state_machine->index = 0;
		state_machine->current_target = state_machine->possible_3rds[0];
	}

	void ScoutZLookFor3rd::ExitState()
	{
		return;
	}
	
	State* ScoutZLookFor3rd::TestTransitions()
	{
		if (state_machine->index >= state_machine->possible_3rds.size())
			return new ScoutZScoutMain(agent, state_machine);
		return NULL;
	}

	std::string ScoutZLookFor3rd::toString()
	{
		return "look for 3rd";
	}

#pragma endregion


#pragma region ScoutTInitialMove

	void ScoutTInitialMove::TickState()
	{
		if (state_machine->scout->orders.size() == 0 || state_machine->scout->orders[0].target_pos != state_machine->current_target)
			agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::MOVE_MOVE, state_machine->current_target);
	}

	void ScoutTInitialMove::EnterState()
	{
		state_machine->current_target = state_machine->enemy_main;
	}

	void ScoutTInitialMove::ExitState()
	{
		return;
	}

	State* ScoutTInitialMove::TestTransitions()
	{
		if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 1)
			return new ScoutTScoutMain(agent, state_machine);
		return NULL;
	}

	std::string ScoutTInitialMove::toString()
	{
		return "initial move";
	}

#pragma endregion

#pragma region ScoutZScoutMain

	void ScoutTScoutMain::TickState()
	{
		if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 3)
		{
			state_machine->index++;
			if (state_machine->index < state_machine->main_scout_path.size())
				state_machine->current_target = state_machine->main_scout_path[state_machine->index];
		}
		agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::MOVE_MOVE, state_machine->current_target);
	}

	void ScoutTScoutMain::EnterState()
	{
		state_machine->index = 0;
		state_machine->current_target = state_machine->main_scout_path[0];
	}

	void ScoutTScoutMain::ExitState()
	{
		return;
	}

	State* ScoutTScoutMain::TestTransitions()
	{
		if (state_machine->index >= state_machine->main_scout_path.size())
			return new ScoutTScoutNatural(agent, state_machine);
		return NULL;
	}

	std::string ScoutTScoutMain::toString()
	{
		return "scout main";
	}

#pragma endregion

#pragma region ScoutTScoutNatural

	void ScoutTScoutNatural::TickState()
	{
		if (Distance2D(state_machine->scout->pos, state_machine->current_target) < 3)
		{
			state_machine->index++;
			if (state_machine->index < state_machine->natural_scout_path.size())
				state_machine->current_target = state_machine->natural_scout_path[state_machine->index];
		}
		agent->Actions()->UnitCommand(state_machine->scout, ABILITY_ID::MOVE_MOVE, state_machine->current_target);
	}

	void ScoutTScoutNatural::EnterState()
	{
		state_machine->index = 0;
		state_machine->current_target = state_machine->natural_scout_path[0];
	}

	void ScoutTScoutNatural::ExitState()
	{
		return;
	}

	State* ScoutTScoutNatural::TestTransitions()
	{
		if (state_machine->index >= state_machine->natural_scout_path.size())
			return new ScoutTScoutMain(agent, state_machine);
		return NULL;
	}

	std::string ScoutTScoutNatural::toString()
	{
		return "scout natural";
	}

#pragma endregion


#pragma region ImmortalDropWaitForImmortals

	void ImmortalDropWaitForImmortals::TickState()
	{
		if (state_machine->prism == NULL)
		{
			for (const auto &prism : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_WARPPRISM)))
			{
				state_machine->prism = prism;
				break;
			}
		}
		if (state_machine->immortal1 == NULL || state_machine->immortal2 == NULL)
		{
			for (const auto &immortal : agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_IMMORTAL)))
			{
				if (state_machine->immortal1 == NULL && immortal != state_machine->immortal2)
					state_machine->immortal1 = immortal;
				else if (state_machine->immortal2 == NULL && immortal != state_machine->immortal1)
					state_machine->immortal2 = immortal;
			}
		}
	}

	void ImmortalDropWaitForImmortals::EnterState()
	{
		return;
	}

	void ImmortalDropWaitForImmortals::ExitState()
	{
		agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::SMART, state_machine->prism);
		agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::SMART, state_machine->prism);
		return;
	}

	State* ImmortalDropWaitForImmortals::TestTransitions()
	{
		if (state_machine->prism != NULL && state_machine->immortal1 != NULL  && state_machine->immortal2 != NULL)
			return new ImmortalDropInitialMove(agent, state_machine);
		return NULL;
	}

	std::string ImmortalDropWaitForImmortals::toString()
	{
		return "wait for immortals";
	}

#pragma endregion

#pragma region ImmortalDropInitialMove

	void ImmortalDropInitialMove::TickState()
	{
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->entry_pos);
	}

	void ImmortalDropInitialMove::EnterState()
	{
		return;
	}

	void ImmortalDropInitialMove::ExitState()
	{
		return;
	}

	State* ImmortalDropInitialMove::TestTransitions()
	{
		if (Distance2D(state_machine->prism->pos, state_machine->entry_pos) < 15)
			return new ImmortalDropMicroDrop(agent, state_machine);
		return NULL;
	}

	std::string ImmortalDropInitialMove::toString()
	{
		return "initial move";
	}

#pragma endregion

#pragma region ImmortalDropMicroDrop

	void ImmortalDropMicroDrop::TickState()
	{
		agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism);
		agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::MOVE_MOVE, agent->PointBetween(state_machine->immortal1->pos, state_machine->prism->pos, 1));
		agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::MOVE_MOVE, agent->PointBetween(state_machine->immortal2->pos, state_machine->prism->pos, 1));
	}


	void ImmortalDropMicroDrop::EnterState()
	{
		return;
	}

	void ImmortalDropMicroDrop::ExitState()
	{
		return;
	}

	State* ImmortalDropMicroDrop::TestTransitions()
	{
		if (state_machine->prism->cargo_space_taken == 0)
			return new ImmortalDropMicroDropDropped2(agent, state_machine);
		return NULL;
	}

	std::string ImmortalDropMicroDrop::toString()
	{
		return "micro immortal drop";
	}

#pragma endregion

#pragma region ImmortalDropMicroDropCarrying1

	void ImmortalDropMicroDropCarrying1::TickState()
	{
		// don't leave immortals behind
		if (Distance2D(state_machine->prism->pos, state_machine->immortal2->pos) > 4.5)
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->immortal2->pos);
		else
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->UpdatePrismPathing());

		if (agent->Observation()->GetGameLoop() >= entry_frame + 15)
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism);
		if (state_machine->immortal1->orders.size() == 0 || state_machine->immortal1->orders[0].ability_id != ABILITY_ID::SMART)
			agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::MOVE_MOVE, agent->PointBetween(state_machine->immortal1->pos, state_machine->prism->pos, 1));
		if (state_machine->immortal2->orders.size() == 0 || state_machine->immortal2->orders[0].ability_id != ABILITY_ID::SMART)
			agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::MOVE_MOVE, agent->PointBetween(state_machine->immortal2->pos, state_machine->prism->pos, 1));
	}

	void ImmortalDropMicroDropCarrying1::EnterState()
	{
		return;
	}

	void ImmortalDropMicroDropCarrying1::ExitState()
	{
		return;
	}

	State* ImmortalDropMicroDropCarrying1::TestTransitions()
	{
		if (state_machine->immortal1->weapon_cooldown == 0 && state_machine->immortal2->weapon_cooldown == 0 && state_machine->prism->cargo_space_taken == 0)
			return new ImmortalDropMicroDropDropped1(agent, state_machine);
		return NULL;
	}

	std::string ImmortalDropMicroDropCarrying1::toString()
	{
		return "micro immortal drop carrying 1";
	}

#pragma endregion

#pragma region ImmortalDropMicroDropCarrying2

	void ImmortalDropMicroDropCarrying2::TickState()
	{
		// don't leave immortals behind
		if (Distance2D(state_machine->prism->pos, state_machine->immortal1->pos) > 4.5)
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->immortal1->pos);
		else
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->UpdatePrismPathing());

		if (agent->Observation()->GetGameLoop() >= entry_frame + 15)
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::UNLOADALLAT, state_machine->prism);
		if (state_machine->immortal1->orders.size() == 0 || state_machine->immortal1->orders[0].ability_id != ABILITY_ID::SMART)
			agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::MOVE_MOVE, agent->PointBetween(state_machine->immortal1->pos, state_machine->prism->pos, 1));
		if (state_machine->immortal2->orders.size() == 0 || state_machine->immortal2->orders[0].ability_id != ABILITY_ID::SMART)
			agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::MOVE_MOVE, agent->PointBetween(state_machine->immortal2->pos, state_machine->prism->pos, 1));
	}

	void ImmortalDropMicroDropCarrying2::EnterState()
	{
		return;
	}

	void ImmortalDropMicroDropCarrying2::ExitState()
	{
		return;
	}

	State* ImmortalDropMicroDropCarrying2::TestTransitions()
	{
		if (state_machine->immortal1->weapon_cooldown == 0 && state_machine->immortal2->weapon_cooldown == 0 && state_machine->prism->cargo_space_taken == 0)
			return new ImmortalDropMicroDropDropped2(agent, state_machine);
		return NULL;
	}

	std::string ImmortalDropMicroDropCarrying2::toString()
	{
		return "micro immortal drop carrying 2";
	}

#pragma endregion

#pragma region ImmortalDropMicroDropDropped1

	void ImmortalDropMicroDropDropped1::TickState()
	{
		// don't leave immortals behind
		if (Distance2D(state_machine->prism->pos, state_machine->immortal1->pos) > 4.5)
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->immortal1->pos);
		else if (Distance2D(state_machine->prism->pos, state_machine->immortal2->pos) > 4.5)
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->immortal2->pos);
		else
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->UpdatePrismPathing());

		if ((!immortal1_has_attack_order || state_machine->immortal1->weapon_cooldown > 0) && (!immortal2_has_attack_order || state_machine->immortal2->weapon_cooldown > 0))
			agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::SMART, state_machine->prism);
	}

	void ImmortalDropMicroDropDropped1::EnterState()
	{
		std::map<const Unit*, const Unit*> attacks = agent->FindTargets({ state_machine->immortal1, state_machine->immortal2 }, state_machine->target_priority, 0);
		if (attacks.count(state_machine->immortal1))
		{
			agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::ATTACK, attacks[state_machine->immortal1]);
			immortal1_has_attack_order = true;
		}

		if (attacks.count(state_machine->immortal2))
		{
			agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::ATTACK, attacks[state_machine->immortal2]);
			immortal2_has_attack_order = true;
		}
	}

	void ImmortalDropMicroDropDropped1::ExitState()
	{
		
	}

	State* ImmortalDropMicroDropDropped1::TestTransitions()
	{
		if (state_machine->prism->cargo_space_taken == 4)
			return new ImmortalDropMicroDropCarrying2(agent, state_machine);
		return NULL;
	}

	std::string ImmortalDropMicroDropDropped1::toString()
	{
		return "micro immortal drop dropped 1";
	}

#pragma endregion

#pragma region ImmortalDropMicroDropDropped2

	void ImmortalDropMicroDropDropped2::TickState()
	{
		// don't leave immortals behind
		if (Distance2D(state_machine->prism->pos, state_machine->immortal1->pos) > 4.5)
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->immortal1->pos);
		else if (Distance2D(state_machine->prism->pos, state_machine->immortal2->pos) > 4.5)
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->immortal2->pos);
		else
			agent->Actions()->UnitCommand(state_machine->prism, ABILITY_ID::MOVE_MOVE, state_machine->UpdatePrismPathing());

		if ((!immortal1_has_attack_order || state_machine->immortal1->weapon_cooldown > 0) && (!immortal2_has_attack_order || state_machine->immortal2->weapon_cooldown > 0))
			agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::SMART, state_machine->prism);
	}

	void ImmortalDropMicroDropDropped2::EnterState()
	{
		std::map<const Unit*, const Unit*> attacks = agent->FindTargets({ state_machine->immortal1, state_machine->immortal2 }, state_machine->target_priority, 0);
		if (attacks.count(state_machine->immortal1))
		{
			agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::ATTACK, attacks[state_machine->immortal1]);
			immortal1_has_attack_order = true;
		}

		if (attacks.count(state_machine->immortal2))
		{
			agent->Actions()->UnitCommand(state_machine->immortal2, ABILITY_ID::ATTACK, attacks[state_machine->immortal2]);
			immortal2_has_attack_order = true;
		}
	}

	void ImmortalDropMicroDropDropped2::ExitState()
	{
		agent->Actions()->UnitCommand(state_machine->immortal1, ABILITY_ID::SMART, state_machine->prism);
	}

	State* ImmortalDropMicroDropDropped2::TestTransitions()
	{
		if (state_machine->prism->cargo_space_taken == 4)
			return new ImmortalDropMicroDropCarrying1(agent, state_machine);
		return NULL;
	}

	std::string ImmortalDropMicroDropDropped2::toString()
	{
		return "micro immortal drop dropped 2";
	}

#pragma endregion

}