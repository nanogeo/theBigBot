#pragma once
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

#include "sc2api/sc2_unit_filters.h"


namespace sc2
{

class TheBigBot;

struct mineral_patch_data
{
	bool is_close;
	const Unit* workers[3];
	mineral_patch_data()
	{
		is_close = false;
		workers[0] = NULL;
		workers[1] = NULL;
		workers[2] = NULL;
	}
	mineral_patch_data(bool a)
	{
		is_close = a;
		workers[0] = NULL;
		workers[1] = NULL;
		workers[2] = NULL;
	}
	mineral_patch_data(bool a, Unit* x, Unit* y, Unit* z)
	{
		is_close = a;
		workers[0] = x;
		workers[1] = y;
		workers[2] = z;
	}
};

struct assimilator_data
{
	const Unit* workers[3];
};

struct mineral_patch_reversed_data
{
	const Unit* mineral_tag;
	Point2D drop_off_point;
	Point2D pick_up_point;
};

struct assimilator_reversed_data
{
	const Unit* assimilator_tag;
	Point2D drop_off_point;
	Point2D pick_up_point;
};

struct mineral_patch_space
{
	const Unit** worker;
	const Unit* mineral_patch;
	mineral_patch_space(const Unit** unit, const Unit* patch)
	{
		worker = unit;
		mineral_patch = patch;
	}
};

class WorkerManager
{
public:
	TheBigBot* agent;
	std::map<const Unit*, mineral_patch_data> mineral_patches;
	std::map<const Unit*, mineral_patch_reversed_data> mineral_patches_reversed;
	std::map<const Unit*, assimilator_data> assimilators;
	std::map<const Unit*, assimilator_reversed_data> assimilators_reversed;
	std::vector<mineral_patch_space*> first_2_mineral_patch_spaces;
	std::vector<mineral_patch_space*> far_3_mineral_patch_spaces;
	std::vector<mineral_patch_space*> close_3_mineral_patch_spaces;
	std::vector<mineral_patch_space*> gas_spaces;
	std::vector<mineral_patch_space*> far_3_mineral_patch_extras;
	std::vector<mineral_patch_space*> close_3_mineral_patch_extras;

	const Unit *new_base = NULL;
	bool should_build_workers = true;
	int removed_gas_miners = 0;

	WorkerManager(TheBigBot* agent)
	{
		this->agent = agent;
	}

	const Unit* GetWorker();
	const Unit* GetBuilder(Point2D);
	void PlaceWorker(const Unit*);
	void PlaceWorkerInGas(const Unit*, const Unit*, int);
	void NewPlaceWorkerInGas(const Unit*, const Unit*);
	void PlaceWorkerOnMinerals(const Unit*, const Unit*, int);
	void NewPlaceWorkerOnMinerals(const Unit*, const Unit*);
	void RemoveWorker(const Unit*);
	void SplitWorkers();
	void SaturateGas(const Unit*);
	void SemiSaturateGas(const Unit*);
	void AddNewBase();
	void DistributeWorkers();
	void BalanceWorers();
	void BuildWorkers();

	void AddAssimilator(const Unit*);
	void SetNewBase(const Unit*);
	void RemoveSpentMineralPatch(const Unit*);


};

}