#pragma once

#include "adept_harass_protoss_state_machine.h"
#include "theBigBot.h"


namespace sc2 {


#pragma region AdeptHarassProtossMoveAcross

void AdeptHarassProtossMoveAcross::TickState()
{
	/*if (state_machine->target == NULL)
	{
		for (const auto& unit : agent->Observation()->GetUnits(Unit::Alliance::Enemy))
		{
			if (Distance2D(state_machine->adept->pos, unit->pos) < 8)
			{
				state_machine->target = unit;
				break;
			}
		}
		if (state_machine->target == NULL)
		{
			agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, agent->locations->adept_scout_runaway);
			if (state_machine->frame_shade_used + 254 < agent->Observation()->GetGameLoop())
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, agent->locations->adept_scout_runaway);

			if (state_machine->shade != NULL)
				agent->Actions()->UnitCommand(state_machine->shade, ABILITY_ID::GENERAL_MOVE, agent->locations->adept_scout_runaway);
		}
	}
	else
	{
		if (Distance2D(state_machine->adept->pos, state_machine->target->pos) > 8)
		{
			state_machine->target = NULL;
			return;
		}

		if (state_machine->attack_status == false)
		{
			// TODO move infront of units based on distance away
			if (Distance2D(state_machine->target->pos, state_machine->adept->pos) <= 4 && state_machine->adept->weapon_cooldown == 0)
			{
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::ATTACK_ATTACK, state_machine->target);
				state_machine->attack_status = true;
			}
			else
			{
				agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::GENERAL_MOVE, state_machine->target->pos);
				if (state_machine->frame_shade_used + 254 < agent->Observation()->GetGameLoop())
					agent->Actions()->UnitCommand(state_machine->adept, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, agent->locations->adept_scout_runaway);

			}
		}
		else if (state_machine->adept->weapon_cooldown > 0)
		{
			state_machine->attack_status = false;
		}
		if (state_machine->shade != NULL)
		{
			agent->Actions()->UnitCommand(state_machine->shade, ABILITY_ID::GENERAL_MOVE, agent->locations->adept_scout_runaway);
		}
	}*/

	agent->Actions()->UnitCommand(state_machine->adepts, ABILITY_ID::GENERAL_MOVE, state_machine->consolidation_points[0]);

}

void AdeptHarassProtossMoveAcross::EnterState()
{
	return;
}

void AdeptHarassProtossMoveAcross::ExitState()
{
	return;
}

State* AdeptHarassProtossMoveAcross::TestTransitions()
{
	for (const auto adept : state_machine->adepts)
	{
		if (Distance2D(adept->pos, state_machine->consolidation_points[0]) > 3)
			return NULL;
	}
	return new AdeptHarassProtossConsolidate(agent, state_machine);
}

std::string AdeptHarassProtossMoveAcross::toString()
{
	return "adepts move across";
}

#pragma endregion

#pragma region AdeptHarassProtossConsolidate

void AdeptHarassProtossConsolidate::TickState()
{
	Point2D adept_center = Utility::MedianCenter(state_machine->adepts);
	const Unit* closest_enemy = Utility::ClosestUnitTo(agent->Observation()->GetUnits(Unit::Alliance::Enemy), adept_center);
	for (const auto& adept : state_machine->adepts)
	{
		float dist = Distance2D(adept_center, closest_enemy->pos);
		if (dist < 9/*Utility::GetRange(closest_enemy) + 2*/)
		{
			agent->Actions()->UnitCommand(adept, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(closest_enemy->pos, adept_center, dist + 2));
		}
		else
		{
			agent->Actions()->UnitCommand(adept, ABILITY_ID::GENERAL_MOVE, state_machine->consolidation_points[state_machine->index]);
		}
	}
}

void AdeptHarassProtossConsolidate::EnterState()
{
	return;
}

void AdeptHarassProtossConsolidate::ExitState()
{
	return;
}

State* AdeptHarassProtossConsolidate::TestTransitions()
{
	if (state_machine->frame_shade_used + 255 < agent->Observation()->GetGameLoop())
	{
		if (Distance2D(Utility::MedianCenter(state_machine->adepts), state_machine->consolidation_points[state_machine->index]) < 1)
		{
			return new AdeptHarassProtossShadeIntoBase(agent, state_machine);
		}
		else if (Utility::DistanceToClosest(agent->Observation()->GetUnits(Unit::Alliance::Enemy), Utility::MedianCenter(state_machine->adepts)) < 8)
		{
			return new AdeptHarassProtossShadeToOtherSide(agent, state_machine);
		}
	}
	return NULL;
}

std::string AdeptHarassProtossConsolidate::toString()
{
	return "adepts consolidate";
}

#pragma endregion

#pragma region AdeptHarassProtossShadeIntoBase

void AdeptHarassProtossShadeIntoBase::TickState()
{
	Point2D adept_center = Utility::MedianCenter(state_machine->adepts);
	const Unit* closest_enemy = Utility::ClosestUnitTo(agent->Observation()->GetUnits(Unit::Alliance::Enemy), adept_center);
	for (const auto& adept : state_machine->adepts)
	{
		float dist = Distance2D(adept_center, closest_enemy->pos);
		if (dist < 8/*Utility::GetRange(closest_enemy) + 2*/)
		{
			agent->Actions()->UnitCommand(adept, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(closest_enemy->pos, adept_center, dist + 1));
		}
		else
		{
			agent->Actions()->UnitCommand(adept, ABILITY_ID::GENERAL_MOVE, state_machine->consolidation_points[state_machine->index]);
		}
	}
}

void AdeptHarassProtossShadeIntoBase::EnterState()
{
	agent->Actions()->UnitCommand(state_machine->adepts, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, agent->Observation()->GetGameInfo().enemy_start_locations[0]);
	state_machine->frame_shade_used = agent->Observation()->GetGameLoop();
	return;
}

void AdeptHarassProtossShadeIntoBase::ExitState()
{
	return;
}

State* AdeptHarassProtossShadeIntoBase::TestTransitions()
{
	if (state_machine->frame_shade_used + 155 < agent->Observation()->GetGameLoop())
	{
		// last chance to cancel shade
		Point3D enemy_start_location = agent->ToPoint3D(agent->Observation()->GetGameInfo().enemy_start_locations[0]);
		for (const auto& shade : state_machine->shades)
		{
			if (shade->pos.z < enemy_start_location.z - .1 || shade->pos.z > enemy_start_location.z + .1)
			{
				for (const auto& adept : state_machine->adepts)
				{
					agent->Actions()->UnitCommand(adept, ABILITY_ID::CANCEL_ADEPTPHASESHIFT);
				}
				return new AdeptHarassProtossConsolidate(agent, state_machine);
			}
		}
		return new AdeptHarassProtossKillProbes(agent, state_machine);
	}
	return NULL;
}

std::string AdeptHarassProtossShadeIntoBase::toString()
{
	return "adepts shade into base";
}

#pragma endregion

#pragma region AdeptHarassProtossShadeToOtherSide

void AdeptHarassProtossShadeToOtherSide::TickState()
{
	Point2D adept_center = Utility::MedianCenter(state_machine->adepts);
	const Unit* closest_enemy = Utility::ClosestUnitTo(agent->Observation()->GetUnits(Unit::Alliance::Enemy), adept_center);
	for (const auto& adept : state_machine->adepts)
	{
		float dist = Distance2D(adept_center, closest_enemy->pos);
		if (dist < 9/*Utility::GetRange(closest_enemy) + 2*/)
		{
			agent->Actions()->UnitCommand(adept, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(closest_enemy->pos, adept_center, dist + 2));
		}
		else
		{
			agent->Actions()->UnitCommand(adept, ABILITY_ID::GENERAL_MOVE, state_machine->consolidation_points[state_machine->index]);
		}
	}
}

void AdeptHarassProtossShadeToOtherSide::EnterState()
{
	agent->Actions()->UnitCommand(state_machine->adepts, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, state_machine->consolidation_points[(state_machine->index * -1) + 1]);
	state_machine->frame_shade_used = agent->Observation()->GetGameLoop();
	return;
}

void AdeptHarassProtossShadeToOtherSide::ExitState()
{
	return;
}

State* AdeptHarassProtossShadeToOtherSide::TestTransitions()
{
	if (state_machine->frame_shade_used + 155 < agent->Observation()->GetGameLoop())
	{
		int swap_sides = 0;
		for (const auto& unit : agent->Observation()->GetUnits(Unit::Alliance::Enemy))
		{
			if (unit->unit_type != PROBE)
			{
				if (Distance2D(unit->pos, state_machine->consolidation_points[0]) < 6)
				{
					if (state_machine->index == 0)
						swap_sides++;
					else
						swap_sides--;
				}
				else if (Distance2D(unit->pos, state_machine->consolidation_points[1]) < 6)
				{
					if (state_machine->index == 1)
						swap_sides++;
					else
						swap_sides--;
				}
			}
		}
		if (swap_sides > 0)
		{
			state_machine->index = (state_machine->index * -1) + 1;
			return new AdeptHarassProtossConsolidate(agent, state_machine);
		}
		else
		{
			agent->Actions()->UnitCommand(state_machine->adepts, ABILITY_ID::CANCEL_ADEPTPHASESHIFT);
			return new AdeptHarassProtossConsolidate(agent, state_machine);
		}
	}
	return NULL;
}

std::string AdeptHarassProtossShadeToOtherSide::toString()
{
	return "adepts shade across";
}

#pragma endregion

#pragma region AdeptHarassProtossKillProbes

void AdeptHarassProtossKillProbes::TickState()
{
	if (state_machine->attack_status)
	{
		for (const auto& adept : state_machine->adepts)
		{
			if (adept->weapon_cooldown == 0)
				return;
		}
		state_machine->attack_status = false;
	}
	Units enemy_probes = agent->Observation()->GetUnits(IsEnemyUnit(PROBE));
	Point2D center = Utility::MedianCenter(state_machine->adepts);
	if (enemy_probes.size() > 0)
	{
		if (agent->Observation()->GetGameLoop() > state_machine->frame_shade_used + 255)
		{
			agent->Actions()->UnitCommand(state_machine->adepts, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, Utility::FurthestFrom(enemy_probes, center)->pos);
		}
		if (state_machine->shades.size() > 0)
		{
			agent->Actions()->UnitCommand(state_machine->shades, ABILITY_ID::GENERAL_MOVE, Utility::FurthestFrom(enemy_probes, center)->pos);
		}
		const Unit* closest_probe = Utility::ClosestTo(enemy_probes, center);
		if (Utility::DistanceToFurthest(state_machine->adepts, closest_probe->pos) < 4)
		{
			agent->Actions()->UnitCommand(state_machine->adepts, ABILITY_ID::ATTACK_ATTACK, closest_probe);
			state_machine->attack_status = true;
		}
		else
		{
			agent->Actions()->UnitCommand(state_machine->adepts, ABILITY_ID::GENERAL_MOVE, closest_probe->pos);
		}
	}
	else
	{
		Point2D enemy_start_pos = agent->Observation()->GetGameInfo().enemy_start_locations[0];
		float dist = Distance2D(center, enemy_start_pos);
		if (agent->Observation()->GetGameLoop() > state_machine->frame_shade_used + 255)
		{
			agent->Actions()->UnitCommand(state_machine->adepts, ABILITY_ID::EFFECT_ADEPTPHASESHIFT, Utility::PointBetween(center, enemy_start_pos, dist + 10));
		}
		if (state_machine->shades.size() > 0)
		{
			agent->Actions()->UnitCommand(state_machine->shades, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(center, enemy_start_pos, dist + 10));
		}
		agent->Actions()->UnitCommand(state_machine->adepts, ABILITY_ID::GENERAL_MOVE, Utility::PointBetween(center, enemy_start_pos, dist + 6));
	}
}

void AdeptHarassProtossKillProbes::EnterState()
{
	return;
}

void AdeptHarassProtossKillProbes::ExitState()
{
	return;
}

State* AdeptHarassProtossKillProbes::TestTransitions()
{
	return NULL;
}

std::string AdeptHarassProtossKillProbes::toString()
{
	return "adepts kill probes";
}

#pragma endregion


#pragma region AdeptHarassProtoss

AdeptHarassProtoss::AdeptHarassProtoss(TheBigBot* agent, std::string name, Units adepts, std::vector<Point2D> consolidation_points) {
	this->agent = agent;
	this->name = name;
	this->adepts = adepts;
	this->consolidation_points = consolidation_points;

	attack_status = false;

	current_state = new AdeptHarassProtossMoveAcross(agent, this);

	event_id = agent->GetUniqueId();
	std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
		this->OnUnitCreatedListener(unit);
	};
	agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);

	std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
		this->OnUnitDestroyedListener(unit);
	};
	agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

	current_state->EnterState();
}

AdeptHarassProtoss::~AdeptHarassProtoss()
{
	agent->RemoveListenerToOnUnitCreatedEvent(event_id);
	agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
}

void AdeptHarassProtoss::OnUnitCreatedListener(const Unit* unit)
{
	if (unit->unit_type == UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT && Utility::DistanceToClosest(adepts, unit->pos) < .5)
	{
		frame_shade_used = agent->Observation()->GetGameLoop();
		shades.push_back(unit);
	}
}

void AdeptHarassProtoss::OnUnitDestroyedListener(const Unit* unit)
{
	if (unit == target)
	{
		target = NULL;
	}
	else
	{
		auto shade = std::find(shades.begin(), shades.end(), unit);
		if (shade != shades.end())
			shades.erase(shade);
	}
}

#pragma endregion

}