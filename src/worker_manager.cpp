#pragma once
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_unit.h"
#include "sc2api/sc2_unit_filters.h"

#include "worker_manager.h"
#include "utility.h"
#include "TossBot.h"

namespace sc2
{

const Unit* WorkerManager::GetWorker()
{
	// Units mineral_fields = observation->GetUnits(IsMineralPatch());
	std::map<const Unit*, mineral_patch_data> close_patches;
	for (const auto &field : mineral_patches)
	{
		if (field.second.is_close && field.second.workers[0] != NULL)
		{
			close_patches[field.first] = field.second;
		}
	}
	std::map<const Unit*, mineral_patch_data> close_oversaturated_patches;
	for (const auto &field : close_patches)
	{
		if (field.second.workers[2] != NULL)
		{
			close_oversaturated_patches[field.first] = field.second;
		}
	}
	// check close mineral patches with 3 assigned harvesters
	if (close_oversaturated_patches.size() > 0)
	{
		for (const auto &field : close_oversaturated_patches)
		{
			return field.second.workers[2];
		}
	}

	std::map<const Unit*, mineral_patch_data> far_patches;
	for (const auto &field : mineral_patches)
	{
		if (!field.second.is_close && field.second.workers[0] != NULL)
		{
			far_patches[field.first] = field.second;
		}
	}
	std::map<const Unit*, mineral_patch_data> far_oversaturated_patches;
	for (const auto &field : far_patches)
	{
		if (field.second.workers[2] != NULL)
		{
			far_oversaturated_patches[field.first] = field.second;
		}
	}
	// check far mineral patches with 3 assigned harvesters
	if (far_oversaturated_patches.size() > 0)
	{
		for (const auto &field : far_oversaturated_patches)
		{
			return field.second.workers[2];
		}
	}
	// check far mineral patches with <3 assigned harvesters
	if (far_patches.size() > 0)
	{
		for (const auto &field : far_patches)
		{
			if (field.second.workers[1] != NULL)
			{
				return field.second.workers[1];
			}
		}
		for (const auto &field : far_patches)
		{
			if (field.second.workers[0] != NULL)
			{
				return field.second.workers[0];
			}
		}
	}
	// check close mineral patches with <3 assigned harvesters
	if (close_patches.size() > 0)
	{
		for (const auto &field : close_patches)
		{
			if (field.second.workers[1] != NULL)
			{
				return field.second.workers[1];
			}
		}
		for (const auto &field : close_patches)
		{
			if (field.second.workers[0] != NULL)
			{
				return field.second.workers[0];
			}
		}
	}
	std::cout << "Error no available worker found";
	return NULL;
}

const Unit* WorkerManager::GetBuilder(Point2D position)
{
	// TODO check for buff CARRYMINERALFIELDMINERALS
	Units mineral_fields = agent->Observation()->GetUnits(IsMineralPatch());
	Units mineral_patches_reversed_keys;
	Units far_only_mineral_patches_reversed_keys;
	for (const auto &patch : mineral_patches_reversed)
	{
		if (!mineral_patches[mineral_patches_reversed[patch.first].mineral_tag].is_close)
			far_only_mineral_patches_reversed_keys.push_back(patch.first);
		mineral_patches_reversed_keys.push_back(patch.first);

	}
	const Unit* closest;
	if (far_only_mineral_patches_reversed_keys.size() > 0)
	{
		closest = Utility::ClosestTo(far_only_mineral_patches_reversed_keys, position);
	}
	if (mineral_patches_reversed_keys.size() > 0)
	{
		const Unit* c = Utility::ClosestTo(mineral_patches_reversed_keys, position);
		if (Distance2D(closest->pos, position) < Distance2D(c->pos, position) * 1.2) // TODO check this
			return closest;
		else
			return c;
	}
	std::cout << "Error mineral patches reversed is empty in GetBuilder";
	return NULL;
}

void WorkerManager::PlaceWorker(const Unit* worker)
{
	if (worker == NULL)
		std::cout << "why";
	if (first_2_mineral_patch_spaces.size() > 0)
	{
		float distance = INFINITY;
		mineral_patch_space* closest = NULL;
		float distance_c = INFINITY;
		mineral_patch_space* closest_c = NULL;
		for (mineral_patch_space* &space : first_2_mineral_patch_spaces)
		{
			float dist = Distance2D(worker->pos, space->mineral_patch->pos);
			if (closest == NULL || dist < distance)
			{
				distance = dist;
				closest = space;
			}
			if (mineral_patches[space->mineral_patch].is_close && (closest_c == NULL || dist < distance_c))
			{
				distance_c = dist;
				closest_c = space;
			}
		}
		if (closest_c != NULL)
		{
			(*closest_c->worker) = worker;
			NewPlaceWorkerOnMinerals(worker, closest_c->mineral_patch);
			first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), closest_c), first_2_mineral_patch_spaces.end());
		}
		else
		{
			(*closest->worker) = worker;
			NewPlaceWorkerOnMinerals(worker, closest->mineral_patch);
			first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), closest), first_2_mineral_patch_spaces.end());
		}
		return;
	}
	if (gas_spaces.size() > removed_gas_miners)
	{
		float distance = INFINITY;
		mineral_patch_space* closest = NULL;
		for (mineral_patch_space* &space : gas_spaces)
		{
			float dist = Distance2D(worker->pos, space->mineral_patch->pos);
			if (closest == NULL || dist < distance)
			{
				distance = dist;
				closest = space;
			}
		}
		*(closest->worker) = worker;
		NewPlaceWorkerInGas(worker, closest->mineral_patch);
		gas_spaces.erase(std::remove(gas_spaces.begin(), gas_spaces.end(), closest), gas_spaces.end());

		return;
	}
	if (far_3_mineral_patch_spaces.size() > 0)
	{
		float distance = INFINITY;
		mineral_patch_space* closest = NULL;
		for (mineral_patch_space* &space : far_3_mineral_patch_spaces)
		{
			float dist = Distance2D(worker->pos, space->mineral_patch->pos);
			if (closest == NULL || dist < distance)
			{
				distance = dist;
				closest = space;
			}
		}
		*(closest->worker) = worker;
		NewPlaceWorkerOnMinerals(worker, closest->mineral_patch);
		far_3_mineral_patch_spaces.erase(std::remove(far_3_mineral_patch_spaces.begin(), far_3_mineral_patch_spaces.end(), closest), far_3_mineral_patch_spaces.end());
		mineral_patch_space* space = new mineral_patch_space(closest->worker, closest->mineral_patch);
		if (worker == NULL)
			std::cout << "null";
		far_3_mineral_patch_extras.push_back(space);

		return;
	}
	if (close_3_mineral_patch_spaces.size() > 0)
	{
		float distance = INFINITY;
		mineral_patch_space* closest = NULL;
		for (mineral_patch_space* &space : close_3_mineral_patch_spaces)
		{
			float dist = Distance2D(worker->pos, space->mineral_patch->pos);
			if (closest == NULL || dist < distance)
			{
				distance = dist;
				closest = space;
			}
		}
		(*closest->worker) = worker;
		NewPlaceWorkerOnMinerals(worker, closest->mineral_patch);
		close_3_mineral_patch_spaces.erase(std::remove(close_3_mineral_patch_spaces.begin(), close_3_mineral_patch_spaces.end(), closest), close_3_mineral_patch_spaces.end());
		close_3_mineral_patch_extras.push_back(new mineral_patch_space(closest->worker, closest->mineral_patch));
		return;
	}
	std::cout << "Error no place for worker\n";
}

void WorkerManager::PlaceWorkerInGas(const Unit* worker, const Unit* gas, int index)
{
	if (assimilators.find(gas) == assimilators.end())
	{
		assimilators[gas] = assimilator_data();
	}
	assimilators[gas].workers[index] = worker;
	Point2D assimilator_position = gas->pos;
	Units townhalls = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS));
	const Unit* closest_nexus = Utility::ClosestTo(townhalls, assimilator_position);
	Point2D vector = assimilator_position - closest_nexus->pos;
	Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
	Point2D drop_off_point = closest_nexus->pos + normal_vector * 2;
	Point2D pick_up_point = assimilator_position - normal_vector * .5;
	assimilator_reversed_data data;
	data.assimilator_tag = gas;
	data.drop_off_point = drop_off_point;
	data.pick_up_point = pick_up_point;
	assimilators_reversed[worker] = data;
}

void WorkerManager::NewPlaceWorkerInGas(const Unit* worker, const Unit* gas)
{
	if (assimilators.find(gas) == assimilators.end())
	{
		assimilators[gas] = assimilator_data();
	}
	Point2D assimilator_position = gas->pos;
	Units townhalls = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS));
	const Unit* closest_nexus = Utility::ClosestTo(townhalls, assimilator_position);
	Point2D vector = assimilator_position - closest_nexus->pos;
	Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
	Point2D drop_off_point = closest_nexus->pos + normal_vector * 2;
	Point2D pick_up_point = assimilator_position - normal_vector * .5;
	assimilator_reversed_data data;
	data.assimilator_tag = gas;
	data.drop_off_point = drop_off_point;
	data.pick_up_point = pick_up_point;
	assimilators_reversed[worker] = data;
}

void WorkerManager::PlaceWorkerOnMinerals(const Unit* worker, const Unit* mineral, int index)
{
	if (mineral_patches.find(mineral) == mineral_patches.end())
	{
		mineral_patches[mineral] = mineral_patch_data(mineral->mineral_contents == 1800);
	}
	mineral_patches[mineral].workers[index] = worker;
	Point2D mineral_position = mineral->pos;
	Units townhalls = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS));
	const Unit* closest_nexus = Utility::ClosestTo(townhalls, mineral_position);
	Point2D vector = mineral_position - closest_nexus->pos;
	Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
	Point2D drop_off_point = closest_nexus->pos + normal_vector * 2;
	Point2D pick_up_point = mineral_position - normal_vector * .5;
	mineral_patch_reversed_data data;
	data.mineral_tag = mineral;
	data.drop_off_point = drop_off_point;
	data.pick_up_point = pick_up_point;
	mineral_patches_reversed[worker] = data;
}

void WorkerManager::NewPlaceWorkerOnMinerals(const Unit* worker, const Unit* mineral)
{
	if (mineral_patches.find(mineral) == mineral_patches.end())
	{
		mineral_patches[mineral] = mineral_patch_data(mineral->mineral_contents == 1800);
	}
	Point2D mineral_position = mineral->pos;
	Units townhalls = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS));
	const Unit* closest_nexus = Utility::ClosestTo(townhalls, mineral_position);
	Point2D vector = mineral_position - closest_nexus->pos;
	Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
	Point2D drop_off_point = closest_nexus->pos + normal_vector * 2;
	Point2D pick_up_point = mineral_position - normal_vector * .5;
	mineral_patch_reversed_data data;
	data.mineral_tag = mineral;
	data.drop_off_point = drop_off_point;
	data.pick_up_point = pick_up_point;
	mineral_patches_reversed[worker] = data;
}

void WorkerManager::RemoveWorker(const Unit* worker)
{
	auto rem = [&](mineral_patch_space* space) -> bool
	{
		return *(space->worker) == worker;
	};
	if (mineral_patches_reversed.find(worker) != mineral_patches_reversed.end())
	{
		const Unit* mineral = mineral_patches_reversed[worker].mineral_tag;
		mineral_patches_reversed.erase(worker);
		bool is_close = mineral_patches[mineral].is_close;

		if (mineral_patches[mineral].workers[0] == worker)
		{
			if (mineral_patches[mineral].workers[1] == NULL)
			{
				mineral_patches[mineral].workers[0] = NULL;
				first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[0], mineral));
			}
			else
			{
				mineral_patches[mineral].workers[0] = mineral_patches[mineral].workers[1];
				if (mineral_patches[mineral].workers[2] == NULL)
				{
					mineral_patches[mineral].workers[1] = NULL;
					first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[1], mineral));
				}
				else
				{
					if (is_close)
					{
						close_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
						close_3_mineral_patch_extras.erase(std::remove_if(close_3_mineral_patch_extras.begin(), close_3_mineral_patch_extras.end(), rem), close_3_mineral_patch_extras.end());
					}
					else
					{
						far_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
						far_3_mineral_patch_extras.erase(std::remove_if(far_3_mineral_patch_extras.begin(), far_3_mineral_patch_extras.end(), rem), far_3_mineral_patch_extras.end());
					}
					mineral_patches[mineral].workers[1] = mineral_patches[mineral].workers[2];
					mineral_patches[mineral].workers[2] = NULL;
				}
			}
		}
		else if (mineral_patches[mineral].workers[1] == worker)
		{
			if (mineral_patches[mineral].workers[2] == NULL)
			{
				mineral_patches[mineral].workers[1] = NULL;
				first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[1], mineral));
			}
			else
			{
				if (is_close)
				{
					close_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
					close_3_mineral_patch_extras.erase(std::remove_if(close_3_mineral_patch_extras.begin(), close_3_mineral_patch_extras.end(), rem), close_3_mineral_patch_extras.end());
				}
				else
				{
					far_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
					far_3_mineral_patch_extras.erase(std::remove_if(far_3_mineral_patch_extras.begin(), far_3_mineral_patch_extras.end(), rem), far_3_mineral_patch_extras.end());
				}
				mineral_patches[mineral].workers[1] = mineral_patches[mineral].workers[2];
				mineral_patches[mineral].workers[2] = NULL;
			}
		}
		else if (mineral_patches[mineral].workers[2] == worker)
		{
			if (is_close)
			{
				close_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
				close_3_mineral_patch_extras.erase(std::remove_if(close_3_mineral_patch_extras.begin(), close_3_mineral_patch_extras.end(), rem), close_3_mineral_patch_extras.end());
			}
			else
			{
				far_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[2], mineral));
				far_3_mineral_patch_extras.erase(std::remove_if(far_3_mineral_patch_extras.begin(), far_3_mineral_patch_extras.end(), rem), far_3_mineral_patch_extras.end());
			}
			mineral_patches[mineral].workers[2] = NULL;
		}
	}
	else if (assimilators_reversed.find(worker) != assimilators_reversed.end())
	{
		const Unit* assimilator = assimilators_reversed[worker].assimilator_tag;
		assimilators_reversed.erase(worker);
		if (assimilators[assimilator].workers[0] == worker)
		{
			if (assimilators[assimilator].workers[1] == NULL)
			{
				assimilators[assimilator].workers[0] = NULL;
				gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[0], assimilator));
			}
			else
			{
				assimilators[assimilator].workers[0] = assimilators[assimilator].workers[1];
				if (assimilators[assimilator].workers[2] == NULL)
				{
					assimilators[assimilator].workers[1] = NULL;
					gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[1], assimilator));
				}
				else
				{
					assimilators[assimilator].workers[1] = assimilators[assimilator].workers[2];
					assimilators[assimilator].workers[2] = NULL;
					gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[2], assimilator));
				}
			}
		}
		else if (assimilators[assimilator].workers[1] == worker)
		{
			if (assimilators[assimilator].workers[2] == NULL)
			{
				assimilators[assimilator].workers[1] = NULL;
				gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[1], assimilator));
			}
			else
			{
				assimilators[assimilator].workers[1] = assimilators[assimilator].workers[2];
				assimilators[assimilator].workers[2] = NULL;
				gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[2], assimilator));
			}
		}
		else if (assimilators[assimilator].workers[2] == worker)
		{
			assimilators[assimilator].workers[2] = NULL;
			gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[2], assimilator));
		}
	}
}

void WorkerManager::SplitWorkers()
{
	Units workers = agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_PROBE));
	Units patches;
	for (const auto &mineral_patch : mineral_patches)
	{
		patches.push_back(mineral_patch.first);
	}
	for (const auto &patch : patches)
	{
		const Unit* closest_worker = Utility::ClosestTo(workers, patch->pos);
		PlaceWorkerOnMinerals(closest_worker, patch, 0);
		for (mineral_patch_space* &space : first_2_mineral_patch_spaces)
		{
			if (patch == space->mineral_patch)
			{
				first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), space), first_2_mineral_patch_spaces.end());
				break;
			}
		}
		workers.erase(std::remove(workers.begin(), workers.end(), closest_worker), workers.end());
	}
	for (const auto &patch : patches)
	{
		if (patch->mineral_contents == 1800)
		{
			const Unit* closest_worker = Utility::ClosestTo(workers, patch->pos);
			PlaceWorkerOnMinerals(closest_worker, patch, 1);
			for (mineral_patch_space* &space : first_2_mineral_patch_spaces)
			{
				if (patch == space->mineral_patch)
				{
					first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), space), first_2_mineral_patch_spaces.end());
					break;
				}
			}
			workers.erase(std::remove(workers.begin(), workers.end(), closest_worker), workers.end());
		}
	}
}

void WorkerManager::SaturateGas(const Unit* gas)
{
	for (int i = 0; i < 3; i++)
	{
		const Unit* worker = GetWorker();
		RemoveWorker(worker);
		PlaceWorkerInGas(worker, gas, i);
		for (const auto &space : gas_spaces)
		{
			if (*space->worker == worker)
			{
				gas_spaces.erase(std::remove(gas_spaces.begin(), gas_spaces.end(), space), gas_spaces.end());
				break;
			}
		}
	}
}

void WorkerManager::SemiSaturateGas(const Unit* gas)
{
	for (int i = 0; i < 2; i++)
	{
		const Unit* worker = GetWorker();
		RemoveWorker(worker);
		PlaceWorkerInGas(worker, gas, i);
		for (const auto& space : gas_spaces)
		{
			if (*space->worker == worker)
			{
				gas_spaces.erase(std::remove(gas_spaces.begin(), gas_spaces.end(), space), gas_spaces.end());
				break;
			}
		}
	}
}

void WorkerManager::AddNewBase()
{
	Units minerals = agent->Observation()->GetUnits(IsMineralPatch());
	Units close_minerals = Utility::CloserThan(minerals, 10, new_base->pos);
	for (const auto &mineral_field : close_minerals)
	{
		if (mineral_field->display_type == Unit::Snapshot)
		{
			return;
		}
	}
	if (close_minerals.size() > 0)
		agent->Actions()->UnitCommand(new_base, ABILITY_ID::SMART, close_minerals[0]);
	for (const auto &mineral_field : close_minerals)
	{
		bool is_close = mineral_field->mineral_contents == 1800;
		mineral_patches[mineral_field] = mineral_patch_data(is_close);
		first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral_field].workers[0], mineral_field));
		first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral_field].workers[1], mineral_field));
		if (is_close)
			close_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral_field].workers[2], mineral_field));
		else
			far_3_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral_field].workers[2], mineral_field));
		new_base = NULL;
	}
}

void WorkerManager::DistributeWorkers()
{
	BalanceWorers();

	Units workers;
	for (const auto &worker : mineral_patches_reversed)
	{
		workers.push_back(worker.first);
	}
	for (const auto &worker : workers)
	{
		if (IsCarryingMinerals(*worker))
		{
			// close to nexus then return the mineral
			Point2D closest_nexus = Utility::ClosestTo(agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)), worker->pos)->pos;
			if (DistanceSquared2D(closest_nexus, worker->pos) < 10 || Utility::CloserThan(workers, .75, worker->pos).size() > 1)
			{
				agent->Actions()->UnitCommand(worker, ABILITY_ID::HARVEST_RETURN_PROBE);
				continue;
			}
			Point2D drop_off_point = mineral_patches_reversed[worker].drop_off_point;
			// don't get bugged out trying to move to the drop off point
			if (worker->orders.size() > 0 && worker->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE)
			{
				if (DistanceSquared2D(worker->pos, drop_off_point) < 1)
				{
					continue;
				}
			}
			// otherwise move to the drop off point
			const Unit *mineral_patch = mineral_patches_reversed[worker].mineral_tag;
			if (mineral_patch != NULL)
			{
				agent->Actions()->UnitCommand(worker, ABILITY_ID::GENERAL_MOVE, drop_off_point);
			}
			else
			{
				std::cout << "Error null mineral patching in DistributeWorkers 1";
			}
		}
		else
		{
			const Unit *mineral_patch = mineral_patches_reversed[worker].mineral_tag;
			if (mineral_patch != NULL)
			{
				if (DistanceSquared2D(worker->pos, mineral_patch->pos) < 4 || Utility::CloserThan(workers, .75, worker->pos).size() > 1)
				{
					if (worker->orders.size() == 0 || worker->orders[0].target_unit_tag != mineral_patch->tag)
					{
						agent->Actions()->UnitCommand(worker, ABILITY_ID::HARVEST_GATHER_PROBE, mineral_patch);
					}
				}
				else if (worker->orders.size() == 0 || worker->orders[0].ability_id != ABILITY_ID::GENERAL_MOVE || DistanceSquared2D(mineral_patches_reversed[worker].pick_up_point, worker->orders[0].target_pos) > 1)
				{
					agent->Actions()->UnitCommand(worker, ABILITY_ID::GENERAL_MOVE, mineral_patches_reversed[worker].pick_up_point);
				}
			}
			else
			{
				std::cout << "Error null mineral patching in DistributeWorkers 2";
			}
		}
	}

	Units gas_workers;
	for (const auto &worker : assimilators_reversed)
	{
		gas_workers.push_back(worker.first);
	}
	for (const auto &worker : gas_workers)
	{
		const Unit* assimilator = assimilators_reversed[worker].assimilator_tag;
		if (assimilators[assimilator].workers[1] != NULL)
		{
			// 2 or 3 workers assigned to gas
			if (worker->orders.size() == 0)
			{
				agent->Actions()->UnitCommand(worker, ABILITY_ID::SMART, assimilator);
			}
			else
			{
				UnitOrder current_order = worker->orders[0];
				if (current_order.ability_id == ABILITY_ID::HARVEST_GATHER && current_order.target_unit_tag != assimilator->tag)
				{
					agent->Actions()->UnitCommand(worker, ABILITY_ID::SMART, assimilator);
				}
				else
				{
					continue;
				}
			}
		}
		else if (IsCarryingVespene(*worker))
		{
			// close to nexus then return the vespene
			Point2D closest_nexus = Utility::ClosestTo(agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)), worker->pos)->pos;
			if (DistanceSquared2D(closest_nexus, worker->pos) < 10 || Utility::CloserThan(workers, .75, worker->pos).size() > 1)
			{
				agent->Actions()->UnitCommand(worker, ABILITY_ID::HARVEST_RETURN_PROBE);
				continue;
			}
			Point2D drop_off_point = assimilators_reversed[worker].drop_off_point;
			// don't get bugged out trying to move to the drop off point
			if (worker->orders[0].ability_id == ABILITY_ID::GENERAL_MOVE && DistanceSquared2D(worker->pos, drop_off_point) < 1)
			{
				continue;
			}
			// otherwise move to the drop off point
			const Unit *assimilator = assimilators_reversed[worker].assimilator_tag;
			if (assimilator != NULL)
			{
				agent->Actions()->UnitCommand(worker, ABILITY_ID::GENERAL_MOVE, drop_off_point);
			}
			else
			{
				std::cout << "Error null assimilator in DistributeWorkers 3";
			}
		}
		else
		{
			const Unit *assimilator = assimilators_reversed[worker].assimilator_tag;
			if (assimilator != NULL)
			{
				if (DistanceSquared2D(worker->pos, assimilator->pos) < 4 || Utility::CloserThan(workers, .75, worker->pos).size() > 1)
				{
					if (worker->orders.size() == 0 || worker->orders[0].target_unit_tag != assimilator->tag)
					{
						agent->Actions()->UnitCommand(worker, ABILITY_ID::HARVEST_GATHER_PROBE, assimilator);
					}
				}
				else if (worker->orders.size() == 0 || worker->orders[0].ability_id != ABILITY_ID::GENERAL_MOVE || DistanceSquared2D(assimilators_reversed[worker].pick_up_point, worker->orders[0].target_pos) > 1)
				{
					agent->Actions()->UnitCommand(worker, ABILITY_ID::GENERAL_MOVE, assimilators_reversed[worker].pick_up_point);
				}
			}
			else
			{
				std::cout << "Error null assimilator in DistributeWorkers 4";
			}
		}
	}
}

void WorkerManager::BalanceWorers()
{
	while (first_2_mineral_patch_spaces.size() > 0 && (close_3_mineral_patch_extras.size() > 0 || far_3_mineral_patch_extras.size() > 0))
	{
		if (close_3_mineral_patch_extras.size() > 0)
		{
			const Unit* worker = *(close_3_mineral_patch_extras[0]->worker);
			RemoveWorker(worker);
			PlaceWorker(worker);
		}
		else if (far_3_mineral_patch_extras.size() > 0)
		{
			const Unit* worker = *(far_3_mineral_patch_extras[0]->worker);
			if (worker != NULL)
			{
				RemoveWorker(worker);
				PlaceWorker(worker);
			}
			else
			{
				far_3_mineral_patch_extras.erase(far_3_mineral_patch_extras.begin());
			}
		}
	}
	while (gas_spaces.size() > 0 && (close_3_mineral_patch_extras.size() > 0 || far_3_mineral_patch_extras.size() > 0))
	{
		if (close_3_mineral_patch_extras.size() > 0)
		{
			const Unit* worker = *(close_3_mineral_patch_extras[0]->worker);
			RemoveWorker(worker);
			PlaceWorker(worker);
		}
		else if (far_3_mineral_patch_extras.size() > 0)
		{
			const Unit* worker = *(far_3_mineral_patch_extras[0]->worker);
			if (worker != NULL)
			{
				RemoveWorker(worker);
				PlaceWorker(worker);
			}
			else
			{
				far_3_mineral_patch_extras.erase(far_3_mineral_patch_extras.begin());
			}
		}
	}
	while (far_3_mineral_patch_spaces.size() > 0 && close_3_mineral_patch_extras.size() > 0)
	{
		const Unit* worker = *(close_3_mineral_patch_extras[0]->worker);
		RemoveWorker(worker);
		PlaceWorker(worker);
	}
}

void WorkerManager::BuildWorkers()
{
	if (should_build_workers)
	{
		for (const auto &nexus : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
		{
			if (nexus->orders.size() > 0)
			{
				if (nexus->orders[0].progress > .95)
				{
					if (nexus->orders.size() == 1)
					{
						agent->Actions()->UnitCommand(nexus, ABILITY_ID::TRAIN_PROBE);
					}
					else if (nexus->orders.size() > 2)
					{
						agent->Actions()->UnitCommand(nexus, ABILITY_ID::CANCEL_LAST);
					}
				}
				else
				{
					if (nexus->orders.size() > 1)
					{
						agent->Actions()->UnitCommand(nexus, ABILITY_ID::CANCEL_LAST);
					}
				}
			}
			else if (nexus->orders.size() == 0)
			{
				agent->Actions()->UnitCommand(nexus, ABILITY_ID::TRAIN_PROBE);
			}
		}
	}
	else
	{
		for (const auto &nexus : agent->Observation()->GetUnits(IsFriendlyUnit(UNIT_TYPEID::PROTOSS_NEXUS)))
		{
			if (nexus->orders.size() > 0)
			{
				agent->Actions()->UnitCommand(nexus, ABILITY_ID::CANCEL_LAST);
			}
		}
	}
}

void WorkerManager::AddAssimilator(const Unit* assimilator)
{
	assimilators[assimilator] = assimilator_data();
	gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[0], assimilator));
	gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[1], assimilator));
	gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[2], assimilator));

}

void WorkerManager::SetNewBase(const Unit* nexus)
{
	new_base = nexus;
}

void WorkerManager::RemoveSpentMineralPatch(const Unit* spent_patch)
{
	if (mineral_patches.count(spent_patch) > 0)
	{
		Units assigned_workers;
		mineral_patch_data spent_patch_data = mineral_patches[spent_patch];
		for (int i = 0; i < 3; i++)
		{
			const Unit* worker = spent_patch_data.workers[i];
			if (worker != NULL)
			{
				assigned_workers.push_back(worker);
				mineral_patches_reversed.erase(worker);
			}
			else
			{
				if (i < 2)
				{
					for (const auto &space : first_2_mineral_patch_spaces)
					{
						if (space->mineral_patch == spent_patch)
						{
							first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), space), first_2_mineral_patch_spaces.end());
							break;
						}
					}
				}
				else if (spent_patch_data.is_close)
				{
					for (const auto &space : close_3_mineral_patch_spaces)
					{
						if (space->mineral_patch == spent_patch)
						{
							close_3_mineral_patch_spaces.erase(std::remove(close_3_mineral_patch_spaces.begin(), close_3_mineral_patch_spaces.end(), space), close_3_mineral_patch_spaces.end());
							break;
						}
					}
					for (const auto &space : close_3_mineral_patch_extras)
					{
						if (space->mineral_patch == spent_patch)
						{
							close_3_mineral_patch_extras.erase(std::remove(close_3_mineral_patch_extras.begin(), close_3_mineral_patch_extras.end(), space), close_3_mineral_patch_extras.end());
							break;
						}
					}
				}
				else 
				{
					for (const auto &space : far_3_mineral_patch_spaces)
					{
						if (space->mineral_patch == spent_patch)
						{
							far_3_mineral_patch_spaces.erase(std::remove(far_3_mineral_patch_spaces.begin(), far_3_mineral_patch_spaces.end(), space), far_3_mineral_patch_spaces.end());
							break;
						}
					}
					for (const auto &space : far_3_mineral_patch_extras)
					{
						if (space->mineral_patch == spent_patch)
						{
							far_3_mineral_patch_extras.erase(std::remove(far_3_mineral_patch_extras.begin(), far_3_mineral_patch_extras.end(), space), far_3_mineral_patch_extras.end());
							break;
						}
					}
				}
			}
		}

		mineral_patches.erase(spent_patch);
		for (const auto &worker : assigned_workers)
		{
			PlaceWorker(worker);
		}
	}
}

}