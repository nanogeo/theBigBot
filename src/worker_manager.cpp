
#include "worker_manager.h"
#include "utility.h"
#include "mediator.h"

// temp
#include "theBigBot.h"


namespace sc2
{
	
#pragma warning(push)
#pragma warning(disable : 4702)
const Unit* WorkerManager::GetWorker()
{
	// Units mineral_fields = observation->GetUnits(IsMineralPatch());
	std::map<const Unit*, mineral_patch_data> close_patches;
	for (const auto &field : mineral_patches)
	{
		if (field.second.is_close && field.second.workers[0] != nullptr)
		{
			close_patches[field.first] = field.second;
		}
	}
	std::map<const Unit*, mineral_patch_data> close_oversaturated_patches;
	for (const auto &field : close_patches)
	{
		if (field.second.workers[2] != nullptr)
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
		if (!field.second.is_close && field.second.workers[0] != nullptr)
		{
			far_patches[field.first] = field.second;
		}
	}
	std::map<const Unit*, mineral_patch_data> far_oversaturated_patches;
	for (const auto &field : far_patches)
	{
		if (field.second.workers[2] != nullptr)
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
			if (field.second.workers[1] != nullptr && IsCarryingMinerals(*field.second.workers[1]) == false)
			{
				return field.second.workers[1];
			}
		}
		for (const auto &field : far_patches)
		{
			if (field.second.workers[0] != nullptr && IsCarryingMinerals(*field.second.workers[0]) == false)
			{
				return field.second.workers[0];
			}
		}
	}
	// if a worker wasn't found ignore the carrying minerals check
	if (far_patches.size() > 0)
	{
		for (const auto& field : far_patches)
		{
			if (field.second.workers[1] != nullptr)
			{
				return field.second.workers[1];
			}
		}
		for (const auto& field : far_patches)
		{
			if (field.second.workers[0] != nullptr)
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
			if (field.second.workers[1] != nullptr && IsCarryingMinerals(*field.second.workers[1]) == false)
			{
				return field.second.workers[1];
			}
		}
		for (const auto &field : close_patches)
		{
			if (field.second.workers[0] != nullptr && IsCarryingMinerals(*field.second.workers[0]) == false)
			{
				return field.second.workers[0];
			}
		}
	}
	// if a worker wasn't found ignore the carrying minerals check
	if (close_patches.size() > 0)
	{
		for (const auto& field : close_patches)
		{
			if (field.second.workers[1] != nullptr)
			{
				return field.second.workers[1];
			}
		}
		for (const auto& field : close_patches)
		{
			if (field.second.workers[0] != nullptr)
			{
				return field.second.workers[0];
			}
		}
	}
	std::cerr << "Error no available worker found in GetWorker" << std::endl;
	mediator->LogMinorError();
	return nullptr;
}
#pragma warning(pop)

const Unit* WorkerManager::GetBuilder(Point2D position)
{
	// TODO check for buff CARRYMINERALFIELDMINERALS
	Units mineral_fields = mediator->GetUnits(IsMineralPatch());
	Units mineral_patches_reversed_keys;
	Units far_only_mineral_patches_reversed_keys;
	for (const auto &worker : mineral_patches_reversed)
	{
		if (IsCarryingMinerals(*worker.first))
			continue;
		if (!mineral_patches[mineral_patches_reversed[worker.first].mineral].is_close)
			far_only_mineral_patches_reversed_keys.push_back(worker.first);
		mineral_patches_reversed_keys.push_back(worker.first);
	}
	const Unit* closest = nullptr;
	if (far_only_mineral_patches_reversed_keys.size() > 0) 
	{
		closest = Utility::ClosestTo(far_only_mineral_patches_reversed_keys, position);
	}
	if (mineral_patches_reversed_keys.size() > 0)
	{
		const Unit* c = Utility::ClosestTo(mineral_patches_reversed_keys, position);
		if (closest != nullptr && Distance2D(closest->pos, position) < Distance2D(c->pos, position) * 1.2)
			return closest;
		else
			return c;
	}
	std::cerr << "Error no available worker found in GetBuilder" << std::endl;
	mediator->LogMinorError();
	return nullptr;
}

void WorkerManager::PlaceWorker(const Unit* worker)
{
	if (worker == nullptr)
	{
		std::cerr << "Error null worker found in PlaceWorker" << std::endl;
		return;
	}
	if ((immediatelySaturateGasses || 
		(immediatelySemiSaturateGasses && gas_spaces.size() > mediator->GetUnits(Unit::Alliance::Self, IsUnit(ASSIMILATOR)).size()))
		&& gas_spaces.size() > removed_gas_miners)
	{
		float distance = INFINITY;
		mineral_patch_space* closest = nullptr;
		for (mineral_patch_space* &space : gas_spaces)
		{
			float dist = Distance2D(worker->pos, space->mineral_patch->pos);
			if (closest == nullptr || dist < distance)
			{
				distance = dist;
				closest = space;
			}
		}
		if (closest == nullptr)
		{
			std::cerr << "Error: nullptr in gas_spaces" << std::endl;
			mediator->LogMinorError();
			return;
		}
		NewPlaceWorkerInGas(worker, closest->mineral_patch);

		return;
	}
	if (first_2_mineral_patch_spaces.size() > 0)
	{
		float distance = INFINITY;
		mineral_patch_space* closest = nullptr;
		float distance_c = INFINITY;
		mineral_patch_space* closest_c = nullptr;
		for (mineral_patch_space* &space : first_2_mineral_patch_spaces)
		{
			float dist = Distance2D(worker->pos, space->mineral_patch->pos);
			if (closest == nullptr || dist < distance)
			{
				distance = dist;
				closest = space;
			}
			if (mineral_patches[space->mineral_patch].is_close && (closest_c == nullptr || dist < distance_c))
			{
				distance_c = dist;
				closest_c = space;
			}
		}
		if (closest_c != nullptr)
		{
			(*closest_c->worker) = worker;
			NewPlaceWorkerOnMinerals(worker, closest_c->mineral_patch);
			first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), closest_c), first_2_mineral_patch_spaces.end());
		}
		else if (closest != nullptr)
		{
			(*closest->worker) = worker;
			NewPlaceWorkerOnMinerals(worker, closest->mineral_patch);
			first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), closest), first_2_mineral_patch_spaces.end());
		}
		else
		{
			std::cerr << "Error: nullptr in first_2_mineral_patch_spaces" << std::endl;
			mediator->LogMinorError();
			return;
		}
		return;
	}
	if (removed_gas_miners > 0 && removed_gas_miners == gas_spaces.size())
		removed_gas_miners--;

	if (gas_spaces.size() > removed_gas_miners)
	{
		float distance = INFINITY;
		mineral_patch_space* closest = nullptr;
		for (mineral_patch_space* &space : gas_spaces)
		{
			float dist = Distance2D(worker->pos, space->mineral_patch->pos);
			if (closest == nullptr || dist < distance)
			{
				distance = dist;
				closest = space;
			}
		}
		if (closest == nullptr)
		{
			std::cerr << "Error: nullptr in gas_spaces" << std::endl;
			mediator->LogMinorError();
			return;
		}
		NewPlaceWorkerInGas(worker, closest->mineral_patch);

		return;
	}
	if (far_3_mineral_patch_spaces.size() > 0)
	{
		float distance = INFINITY;
		mineral_patch_space* closest = nullptr;
		for (mineral_patch_space* &space : far_3_mineral_patch_spaces)
		{
			float dist = Distance2D(worker->pos, space->mineral_patch->pos);
			if (closest == nullptr || dist < distance)
			{
				distance = dist;
				closest = space;
			}
		}
		if (closest == nullptr)
		{
			std::cerr << "Error: nullptr in far_3_mineral_patch_spaces" << std::endl;
			mediator->LogMinorError();
			return;
		}
		*(closest->worker) = worker;
		NewPlaceWorkerOnMinerals(worker, closest->mineral_patch);
		far_3_mineral_patch_spaces.erase(std::remove(far_3_mineral_patch_spaces.begin(), far_3_mineral_patch_spaces.end(), closest), far_3_mineral_patch_spaces.end());
		mineral_patch_space* space = new mineral_patch_space(closest->worker, closest->mineral_patch);
		far_3_mineral_patch_extras.push_back(space);

		return;
	}
	if (close_3_mineral_patch_spaces.size() > 0)
	{
		float distance = INFINITY;
		mineral_patch_space* closest = nullptr;
		for (mineral_patch_space* &space : close_3_mineral_patch_spaces)
		{
			float dist = Distance2D(worker->pos, space->mineral_patch->pos);
			if (closest == nullptr || dist < distance)
			{
				distance = dist;
				closest = space;
			}
		}
		if (closest == nullptr)
		{
			std::cerr << "Error: nullptr in close_3_mineral_patch_spaces" << std::endl;
			mediator->LogMinorError();
			return;
		}
		(*closest->worker) = worker;
		NewPlaceWorkerOnMinerals(worker, closest->mineral_patch);
		close_3_mineral_patch_spaces.erase(std::remove(close_3_mineral_patch_spaces.begin(), close_3_mineral_patch_spaces.end(), closest), close_3_mineral_patch_spaces.end());
		close_3_mineral_patch_extras.push_back(new mineral_patch_space(closest->worker, closest->mineral_patch));
		return;
	}
	//std::cout << "Error no place for worker\n";
}

void WorkerManager::PlaceWorkerInGas(const Unit* worker, const Unit* gas, int index)
{
	if (assimilators.find(gas) == assimilators.end())
	{
		assimilators[gas] = assimilator_data();
	}
	assimilators[gas].workers[index] = worker;
	Point2D assimilator_position = gas->pos;
	Units townhalls = mediator->GetUnits(IsFriendlyUnit(NEXUS));
	const Unit* closest_nexus = Utility::ClosestTo(townhalls, assimilator_position);
	if (closest_nexus == nullptr)
		return;
	Point2D vector = assimilator_position - closest_nexus->pos;
	Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
	Point2D drop_off_point = closest_nexus->pos + normal_vector * 3.2f;
	Point2D pick_up_point = assimilator_position - normal_vector * 2.1f;
	assimilator_reversed_data data;
	data.assimilator = gas;
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
	for (size_t i = 0; i < 3; i++)
	{
		if (assimilators[gas].workers[i] == nullptr)
		{
			assimilators[gas].workers[i] = worker;
			for (size_t j = 0; j < gas_spaces.size(); j++)
			{
				if (*(gas_spaces[j]->worker) == worker)
				{
					gas_spaces.erase(gas_spaces.begin() + j);
					break;
				}
			}
			break;
		}
	}
	Point2D assimilator_position = gas->pos;
	Units townhalls = mediator->GetUnits(IsFriendlyUnit(NEXUS));
	const Unit* closest_nexus = Utility::ClosestTo(townhalls, assimilator_position);
	if (closest_nexus == nullptr)
		return;
	Point2D vector = assimilator_position - closest_nexus->pos;
	Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
	Point2D drop_off_point = closest_nexus->pos + normal_vector * 3.2f;
	Point2D pick_up_point = assimilator_position - normal_vector * 2.1f;
	assimilator_reversed_data data;
	data.assimilator = gas;
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
	Units townhalls = mediator->GetUnits(IsFriendlyUnit(NEXUS));
	const Unit* closest_nexus = Utility::ClosestTo(townhalls, mineral_position);
	if (closest_nexus == nullptr)
		return;
	Point2D vector = mineral_position - closest_nexus->pos;
	Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
	Point2D drop_off_point = closest_nexus->pos + normal_vector * 3.2f;
	Point2D pick_up_point = mineral_position - normal_vector * 1.325f;

	std::vector<const Unit*> closest_minerals = Utility::NClosestUnits(mediator->GetUnits(Unit::Alliance::Neutral), pick_up_point, 2);
	if (closest_minerals.size() == 2)
	{
		const Unit* closest = closest_minerals[0];
		if (Distance2D(closest->pos, mineral_position) == 0)
			closest = closest_minerals[1];

		if (closest != nullptr && Distance2D(closest->pos, pick_up_point) < 1.325)
		{
			Point2D new_pick_up_point = Utility::ClosestIntersectionTo(closest->pos, 1.325, mineral_position, 1.325, pick_up_point);
			pick_up_point = new_pick_up_point;
			mediator->agent->Debug()->DebugSphereOut(mediator->ToPoint3D(new_pick_up_point), .25, Color(255, 0, 255));
		}
	}

	mineral_patch_reversed_data data;
	data.mineral = mineral;
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
	Units townhalls = mediator->GetUnits(IsFriendlyUnit(NEXUS));
	const Unit* closest_nexus = Utility::ClosestTo(townhalls, mineral_position);
	if (closest_nexus == nullptr)
		return;
	Point2D vector = mineral_position - closest_nexus->pos;
	Point2D normal_vector = vector / sqrt(vector.x * vector.x + vector.y * vector.y);
	Point2D drop_off_point = closest_nexus->pos + normal_vector * 3.2f;
	Point2D pick_up_point = mineral_position - normal_vector * 1.325f;

	std::vector<const Unit*> closest_minerals = Utility::NClosestUnits(mediator->GetUnits(Unit::Alliance::Neutral), pick_up_point, 2);
	if (closest_minerals.size() == 2)
	{
		const Unit* closest = closest_minerals[0];
		if (Distance2D(closest->pos, mineral_position) == 0)
			closest = closest_minerals[1];

		if (closest != nullptr && Distance2D(closest->pos, pick_up_point) < 1.325)
		{
			Point2D new_pick_up_point = Utility::ClosestIntersectionTo(closest->pos, 1.325, mineral_position, 1.325, pick_up_point);
			pick_up_point = new_pick_up_point;
			mediator->agent->Debug()->DebugSphereOut(mediator->ToPoint3D(new_pick_up_point), .25, Color(255, 0, 255));
		}
	}

	mineral_patch_reversed_data data;
	data.mineral = mineral;
	data.drop_off_point = drop_off_point;
	data.pick_up_point = pick_up_point;
	mineral_patches_reversed[worker] = data;
}

RemoveWorkerResult WorkerManager::RemoveWorker(const Unit* worker)
{
	/*auto rem = [&](mineral_patch_space* space) -> bool
	{
		return *(space->worker) == worker;
	};*/
	if (mineral_patches_reversed.find(worker) != mineral_patches_reversed.end())
	{
		const Unit* mineral = mineral_patches_reversed[worker].mineral;
		mineral_patches_reversed.erase(worker);
		bool is_close = mineral_patches[mineral].is_close;

		auto rem = [&](mineral_patch_space* space) -> bool
		{
			return *(space->worker) == mineral_patches[mineral].workers[2];
		};

		if (mineral_patches[mineral].workers[0] == worker)
		{
			if (mineral_patches[mineral].workers[1] == nullptr)
			{
				mineral_patches[mineral].workers[0] = nullptr;
				first_2_mineral_patch_spaces.push_back(new mineral_patch_space(&mineral_patches[mineral].workers[0], mineral));
			}
			else
			{
				mineral_patches[mineral].workers[0] = mineral_patches[mineral].workers[1];
				if (mineral_patches[mineral].workers[2] == nullptr)
				{
					mineral_patches[mineral].workers[1] = nullptr;
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
					mineral_patches[mineral].workers[2] = nullptr;
				}
			}
		}
		else if (mineral_patches[mineral].workers[1] == worker)
		{
			if (mineral_patches[mineral].workers[2] == nullptr)
			{
				mineral_patches[mineral].workers[1] = nullptr;
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
				mineral_patches[mineral].workers[2] = nullptr;
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
			mineral_patches[mineral].workers[2] = nullptr;
		}
		return RemoveWorkerResult::REMOVED_FROM_MINERALS;
	}
	else if (assimilators_reversed.find(worker) != assimilators_reversed.end())
	{
		const Unit* assimilator = assimilators_reversed[worker].assimilator;
		assimilators_reversed.erase(worker);
		if (assimilators[assimilator].workers[0] == worker)
		{
			if (assimilators[assimilator].workers[1] == nullptr)
			{
				assimilators[assimilator].workers[0] = nullptr;
				gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[0], assimilator));
			}
			else
			{
				assimilators[assimilator].workers[0] = assimilators[assimilator].workers[1];
				if (assimilators[assimilator].workers[2] == nullptr)
				{
					assimilators[assimilator].workers[1] = nullptr;
					gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[1], assimilator));
				}
				else
				{
					assimilators[assimilator].workers[1] = assimilators[assimilator].workers[2];
					assimilators[assimilator].workers[2] = nullptr;
					gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[2], assimilator));
				}
			}
		}
		else if (assimilators[assimilator].workers[1] == worker)
		{
			if (assimilators[assimilator].workers[2] == nullptr)
			{
				assimilators[assimilator].workers[1] = nullptr;
				gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[1], assimilator));
			}
			else
			{
				assimilators[assimilator].workers[1] = assimilators[assimilator].workers[2];
				assimilators[assimilator].workers[2] = nullptr;
				gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[2], assimilator));
			}
		}
		else if (assimilators[assimilator].workers[2] == worker)
		{
			assimilators[assimilator].workers[2] = nullptr;
			gas_spaces.push_back(new mineral_patch_space(&assimilators[assimilator].workers[2], assimilator));
		}
		return RemoveWorkerResult::REMOVED_FROM_GAS;
	}
	else
	{
		return RemoveWorkerResult::NOT_FOUND;
	}
}

void WorkerManager::SplitWorkers()
{
	Units workers = mediator->GetUnits(IsFriendlyUnit(PROBE));
	Units close_patches;
	Units far_patches;
	for (const auto &mineral_patch : mineral_patches)
	{
		if (mineral_patch.second.is_close)
			close_patches.push_back(mineral_patch.first);
		else
			far_patches.push_back(mineral_patch.first);
	}
	for (const auto &patch : close_patches)
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
	for (const auto& patch : far_patches)
	{
		const Unit* closest_worker = Utility::ClosestTo(workers, patch->pos);
		PlaceWorkerOnMinerals(closest_worker, patch, 0);
		for (mineral_patch_space*& space : first_2_mineral_patch_spaces)
		{
			if (patch == space->mineral_patch)
			{
				first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), space), first_2_mineral_patch_spaces.end());
				break;
			}
		}
		workers.erase(std::remove(workers.begin(), workers.end(), closest_worker), workers.end());
	}
	for (const auto& patch : close_patches)
	{
		const Unit* closest_worker = Utility::FurthestFrom(workers, patch->pos);
		PlaceWorkerOnMinerals(closest_worker, patch, 1);
		for (mineral_patch_space*& space : first_2_mineral_patch_spaces)
		{
			if (patch == space->mineral_patch)
			{
				first_2_mineral_patch_spaces.erase(std::remove(first_2_mineral_patch_spaces.begin(), first_2_mineral_patch_spaces.end(), space), first_2_mineral_patch_spaces.end());
				break;
			}
		}
		workers.erase(std::remove(workers.begin(), workers.end(), closest_worker), workers.end());
	}

	for (const auto& worker : mineral_patches_reversed)
	{
		mediator->SetUnitCommand(worker.first, A_SMART, worker.second.mineral, 0);
	}
}

void WorkerManager::SaturateGas(const Unit* gas)
{
	for (int i = 0; i < 3; i++)
	{
		const Unit* worker = GetWorker();
		if (worker == nullptr)
			return;
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
		if (worker == nullptr)
			return;
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
	Units minerals = mediator->GetUnits(IsMineralPatch());
	Units close_minerals = Utility::CloserThan(minerals, 10, new_base->pos);
	for (const auto &mineral_field : close_minerals)
	{
		if (mineral_field->display_type == Unit::Snapshot)
		{
			return;
		}
	}
	if (close_minerals.size() > 0)
		mediator->SetUnitCommand(new_base, A_SMART, close_minerals[0], 0);
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
		new_base = nullptr;
	}
}

void WorkerManager::RemoveBase(const Unit* nexus)
{
	Units minerals = mediator->GetUnits(IsMineralPatch());
	for (auto itr = mineral_patches.begin(); itr != mineral_patches.end();)
	{
		if (Distance2D(nexus->pos, itr->first->pos) < 10)
		{
			RemoveMineralPatch((itr++)->first);
		}
		else
		{
			itr++;
		}
	}
	// TODO remove assimilators but make sure to add them back if if base gets rebuilt

}

void WorkerManager::DistributeWorkers()
{
	BalanceWorkers();

	Units workers;
	for (const auto &worker : mineral_patches_reversed)
	{
		if (worker.first != nullptr)
		{
			workers.push_back(worker.first);
		}
		else
		{
			std::cerr << "found nullptr worker on minerals" << std::endl;
			mediator->LogMinorError();
		}
	}
	for (const auto &worker : workers)
	{
		mediator->agent->Debug()->DebugSphereOut(mediator->ToPoint3D(mineral_patches_reversed[worker].pick_up_point), .25, Color(255, 0, 0));
		mediator->agent->Debug()->DebugSphereOut(mineral_patches_reversed[worker].mineral->pos, 1.325f, Color(255, 255, 0));
		if (worker->weapon_cooldown == 0)
		{
			const Unit* enemy_unit = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy, IsNotFlyingUnit()), worker->pos);
			if (enemy_unit != nullptr && Distance2D(worker->pos, enemy_unit->pos) <= Utility::RealRange(worker, enemy_unit))
			{
				mediator->SetUnitCommand(worker, A_ATTACK, enemy_unit, 0);
				continue;
			}
		}
		if (worker->orders.size() >= 2)
			continue;

		if (IsCarryingMinerals(*worker) && worker->orders.size() <= 1)
		{
			// close to nexus then return the mineral
			std::vector<const Unit*> nexi = mediator->GetUnits(IsFinishedUnit(NEXUS));
			if (nexi.size() == 0)
				return;

			const Unit* closest_nexus = Utility::ClosestTo(nexi, worker->pos);
			/*if (DistanceSquared2D(closest_nexus->pos, worker->pos) < 9.75)
			{
				mediator->SetUnitCommand(worker, A_SMART, closest_nexus, 0);
				//mediator->SetUnitCommand(worker, A_MOVE, mineral_patches_reversed[worker].pick_up_point, true, 0);
				continue;
			}
			else*/
			if (Distance2D(worker->pos, mineral_patches_reversed[worker].drop_off_point) > .5 &&
				Distance2D(worker->pos, mineral_patches_reversed[worker].drop_off_point) < 2)
			{
				mediator->SetUnitCommand(worker, A_MOVE, mineral_patches_reversed[worker].drop_off_point, 0);
				mediator->SetUnitCommand(worker, A_SMART, closest_nexus, 0, true);
			}
			else if (Distance2D(worker->pos, mineral_patches_reversed[worker].drop_off_point) >= 2)
			{
				mediator->SetUnitCommand(worker, A_SMART, closest_nexus, 0);
			}
		}
		else if (!IsCarryingMinerals(*worker) && worker->orders.size() <= 1)
		{
			const Unit* mineral_patch = mineral_patches_reversed[worker].mineral;

			if (mineral_patch != nullptr)
			{
				if (Distance2D(worker->pos, mineral_patches_reversed[worker].pick_up_point) > .5 && 
					Distance2D(worker->pos, mineral_patches_reversed[worker].pick_up_point) < 2 && 
					Distance2D(worker->pos, mineral_patches_reversed[worker].mineral->pos) > 1.325)
				{
					mediator->SetUnitCommand(worker, A_MOVE, mineral_patches_reversed[worker].pick_up_point, 0);
					mediator->SetUnitCommand(worker, A_GATHER_RESOURCE, mineral_patches_reversed[worker].mineral, 0, true);
				}
				else if (Distance2D(worker->pos, mineral_patches_reversed[worker].pick_up_point) >= 2)
				{
					mediator->SetUnitCommand(worker, A_GATHER_RESOURCE, mineral_patches_reversed[worker].mineral, 0);
				}
			}
			else
			{
				//std::cout << "Error nullptr mineral patching in DistributeWorkers 2";
			}
		}
	}

	Units gas_workers;
	for (const auto &worker : assimilators_reversed)
	{
		if (worker.first != nullptr)
		{
			gas_workers.push_back(worker.first);
		}
		else
		{
			std::cerr << "found nullptr worker on gas" << std::endl;
			mediator->LogMinorError();
		}
	}
	for (const auto &worker : gas_workers)
	{
		if (worker->weapon_cooldown == 0)
		{
			const Unit* enemy_unit = Utility::ClosestTo(mediator->GetUnits(Unit::Alliance::Enemy, IsNotFlyingUnit()), worker->pos);
			if (enemy_unit != nullptr && Distance2D(worker->pos, enemy_unit->pos) <= Utility::RealRange(worker, enemy_unit))
			{
				mediator->SetUnitCommand(worker, A_ATTACK, enemy_unit, 0);
				continue;
			}
		}
		const Unit* assimilator = assimilators_reversed[worker].assimilator;
		if (assimilators[assimilator].workers[1] != nullptr)
		{
			// 2 or 3 workers assigned to gas
			if (worker->orders.size() == 0 || worker->orders[0].ability_id == A_ATTACK)
			{
				mediator->SetUnitCommand(worker, A_SMART, assimilator, 0);
			}
			else
			{
				UnitOrder current_order = worker->orders[0];
				if (current_order.ability_id == A_GATHER_RESOURCE && current_order.target_unit_tag != assimilator->tag)
				{
					mediator->SetUnitCommand(worker, A_SMART, assimilator, 0);
				}
				else
				{
					continue;
				}
			}
		}
		else if (worker->orders.size() == 2)
		{
			continue;
		}
		else if (IsCarryingVespene(*worker) && worker->orders.size() <= 1)
		{
			// close to nexus then return the mineral
			std::vector<const Unit*> nexi = mediator->GetUnits(IsFriendlyUnit(NEXUS));
			if (nexi.size() == 0)
				return;

			const Unit* closest_nexus = Utility::ClosestTo(nexi, worker->pos);

			if (Distance2D(worker->pos, assimilators_reversed[worker].drop_off_point) > .5 &&
				Distance2D(worker->pos, assimilators_reversed[worker].drop_off_point) < 2)
			{
				mediator->SetUnitCommand(worker, A_MOVE, assimilators_reversed[worker].drop_off_point, 0);
				mediator->SetUnitCommand(worker, A_SMART, closest_nexus, 0, true);
			}
			else if (Distance2D(worker->pos, assimilators_reversed[worker].drop_off_point) >= 2)
			{
				mediator->SetUnitCommand(worker, A_SMART, closest_nexus, 0);
			}
		}
		else if (!IsCarryingVespene(*worker) && worker->orders.size() <= 1)
		{
			const Unit* mineral_patch = assimilators_reversed[worker].assimilator;
			if (mineral_patch != nullptr)
			{
				if (Distance2D(worker->pos, assimilators_reversed[worker].pick_up_point) > .5 &&
					Distance2D(worker->pos, assimilators_reversed[worker].pick_up_point) < 2)
				{
					mediator->SetUnitCommand(worker, A_MOVE, assimilators_reversed[worker].pick_up_point, 0);
					mediator->SetUnitCommand(worker, A_SMART, assimilators_reversed[worker].assimilator, 0, true);
				}
				else if (Distance2D(worker->pos, assimilators_reversed[worker].pick_up_point) >= 2)
				{
					mediator->SetUnitCommand(worker, A_SMART, assimilators_reversed[worker].assimilator, 0);
				}
			}
			else
			{
				//std::cout << "Error nullptr mineral patching in DistributeWorkers 2";
			}
		}
	}
}

void WorkerManager::BalanceWorkers()
{
	if (balance_income)
	{
		UnitCost income = mediator->CalculateIncome();
		UnitCost future_resources = mediator->GetCurrentResources() + income - 
			mediator->CalculateCostOfProduction() - mediator->CalculateCostOfCurrentBuildActions();

		bool excess_minerals = future_resources.mineral_cost > income.mineral_cost / 2;
		bool excess_gas = future_resources.vespene_cost > income.vespene_cost / 2;

		if (excess_gas && !excess_minerals && assimilators_reversed.size() > 0 && first_2_mineral_patch_spaces.size() > 0)
		{
			PullOutOfGas(1);
		}
		else if (excess_minerals && !excess_gas)
		{
			if (removed_gas_miners > 0)
			{
				removed_gas_miners--;
			}
			else
			{
				if (mediator->GetNumBuildActions(ASSIMILATOR) == 0 && mediator->HasBuildingUnderConstruction(ASSIMILATOR) == false)
					mediator->BuildBuilding(ASSIMILATOR);
			}
		}
		else if (excess_gas && excess_minerals)
		{
			// build more production
		}
		else if (future_resources.mineral_cost < 0 && future_resources.vespene_cost > 0)
		{
			PullOutOfGas(1);
		}
		else if (future_resources.mineral_cost > 0 && future_resources.vespene_cost < 0)
		{
			if (removed_gas_miners > 0)
			{
				removed_gas_miners--;
			}
			else
			{
				if (mediator->GetNumBuildActions(ASSIMILATOR) == 0 && mediator->HasBuildingUnderConstruction(ASSIMILATOR) == false)
					mediator->BuildBuilding(ASSIMILATOR);
			}
		}
	}

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
			if (worker != nullptr)
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
	while (gas_spaces.size() - removed_gas_miners > 0 && (close_3_mineral_patch_extras.size() > 0 || far_3_mineral_patch_extras.size() > 0))
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
			if (worker != nullptr)
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
		for (const auto &nexus : mediator->GetUnits(Unit::Alliance::Self, IsFinishedUnit(NEXUS)))
		{
			if (nexus->orders.size() > 0)
			{
				if (nexus->orders[0].progress > .95)
				{
					if (nexus->orders.size() == 1)
					{
						mediator->TryTrainProbe(nexus);
					}
					else if (nexus->orders.size() > 2)
					{
						mediator->agent->Actions()->UnitCommand(nexus, A_CANCEL_PRODUCTION);
					}
				}
				else
				{
					if (nexus->orders.size() > 1)
					{
						mediator->agent->Actions()->UnitCommand(nexus, A_CANCEL_PRODUCTION);
					}
				}
			}
			else if (nexus->orders.size() == 0)
			{
				mediator->TryTrainProbe(nexus);
			}
		}
	}
	else
	{
		for (const auto &nexus : mediator->GetUnits(IsFriendlyUnit(NEXUS)))
		{
			if (nexus->orders.size() > 0)
			{
				mediator->agent->Actions()->UnitCommand(nexus, A_CANCEL_PRODUCTION);
			}
		}
	}
}

void WorkerManager::PullOutOfGas()
{
	removed_gas_miners = (int)mediator->GetUnits(Unit::Alliance::Self, IsUnit(ASSIMILATOR)).size() * 3;
	Units gas_workers;
	for (const auto &worker : assimilators_reversed)
	{
		gas_workers.push_back(worker.first);
	}
	for (const auto& worker : gas_workers)
	{
		RemoveWorker(worker);
		PlaceWorker(worker);
	}
}

void WorkerManager::PullOutOfGas(int num)
{
	removed_gas_miners = std::min(removed_gas_miners + num, (int)mediator->GetUnits(Unit::Alliance::Self, IsFinishedUnit(ASSIMILATOR)).size() * 3);
	Units workers_to_remove;
	int removed = 0;

	// remove from gasses with 3 workers
	for (const auto& gas : assimilators)
	{
		if (gas.second.workers[2] == nullptr)
			continue;

		const Unit* worker = nullptr;
		for (int i = 0; i < 3; i++)
		{
			bool carrying_gas = false;
			for (const auto& buff : gas.second.workers[i]->buffs)
			{
				if (buff == BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGASPROTOSS) // check if in the geiser
				{
					carrying_gas = true;
					break;
				}
			}
			if (carrying_gas && i != 2)
				continue;
			worker = gas.second.workers[i];
			break;
		}
		if (worker != nullptr)
		{
			workers_to_remove.push_back(worker);
			removed++;
			if (removed >= num)
				break;
		}
	}
	if (removed < num)
	{
		// we didnt find enough 3rds so find any worker to remove
		for (const auto& worker : assimilators_reversed)
		{
			if (std::find(workers_to_remove.begin(), workers_to_remove.end(), worker.first) != workers_to_remove.end())
				continue;
			workers_to_remove.push_back(worker.first);
			removed++;
			if (removed >= num)
				break;
		}
	}

	for (const auto& worker : workers_to_remove)
	{
		RemoveWorker(worker);
		PlaceWorker(worker);
	}
}

UnitCost WorkerManager::CalculateIncome()
{
	int mineral_income = 0;
	int gas_income = 0;
	for (const auto &patch : mineral_patches)
	{
		if (patch.second.is_close)
		{
			for (int i = 0; i < 3; i++)
			{
				if (patch.second.workers[i])
				{
					if (i == 2)
						mineral_income += 7;
					else
						mineral_income += 66;
				}
			}
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				if (patch.second.workers[i])
				{
					if (i == 2)
						mineral_income += 23;
					else
						mineral_income += 60;
				}
			}
		}
	}


	for (const auto &gas : assimilators)
	{
		for (int i = 0; i < 3; i++)
		{
			if (gas.second.workers[i])
			{
				if (i == 2)
					gas_income += 39;
				else
					gas_income += 62;
			}
		}
	}

	return UnitCost(mineral_income, gas_income, 0);
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

void WorkerManager::RemoveMineralPatch(const Unit* patch)
{
	if (mineral_patches.count(patch) > 0)
	{
		Units assigned_workers;
		mineral_patch_data patch_data = mineral_patches[patch];

		for (int i = 0; i < 3; i++)
		{
			const Unit* worker = patch_data.workers[i];
			if (worker != nullptr)
			{
				assigned_workers.push_back(worker);
				mineral_patches_reversed.erase(worker);
			}
			else
			{
				break;
			}
		}
		for (int i = 0; i < first_2_mineral_patch_spaces.size(); i++)
		{
			if (first_2_mineral_patch_spaces[i]->mineral_patch == patch)
			{
				first_2_mineral_patch_spaces.erase(first_2_mineral_patch_spaces.begin() + i);
				i--;
			}
		}
		if (patch_data.is_close)
		{
			for (const auto& space : close_3_mineral_patch_spaces)
			{
				if (space->mineral_patch == patch)
				{
					close_3_mineral_patch_spaces.erase(std::remove(close_3_mineral_patch_spaces.begin(), close_3_mineral_patch_spaces.end(), space), close_3_mineral_patch_spaces.end());
					break;
				}
			}
			for (const auto& space : close_3_mineral_patch_extras)
			{
				if (space->mineral_patch == patch)
				{
					close_3_mineral_patch_extras.erase(std::remove(close_3_mineral_patch_extras.begin(), close_3_mineral_patch_extras.end(), space), close_3_mineral_patch_extras.end());
					break;
				}
			}
		}
		else
		{
			for (const auto& space : far_3_mineral_patch_spaces)
			{
				if (space->mineral_patch == patch)
				{
					far_3_mineral_patch_spaces.erase(std::remove(far_3_mineral_patch_spaces.begin(), far_3_mineral_patch_spaces.end(), space), far_3_mineral_patch_spaces.end());
					break;
				}
			}
			for (const auto& space : far_3_mineral_patch_extras)
			{
				if (space->mineral_patch == patch)
				{
					far_3_mineral_patch_extras.erase(std::remove(far_3_mineral_patch_extras.begin(), far_3_mineral_patch_extras.end(), space), far_3_mineral_patch_extras.end());
					break;
				}
			}
		}
			

		mineral_patches.erase(patch);
		for (const auto &worker : assigned_workers)
		{
			PlaceWorker(worker);
		}
	}
}

void WorkerManager::OnUnitDestroyed(const Unit* unit)
{
	if (unit->mineral_contents > 0)
		RemoveMineralPatch(unit);
	else if (unit->unit_type == PROBE)
		RemoveWorker(unit);
	else if (unit->unit_type == NEXUS)
		RemoveBase(unit);
}

}