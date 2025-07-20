#pragma once
#pragma warning(push)
#pragma warning(disable : 26495)
#pragma warning(disable : 26439)
#pragma warning(disable : 4189)
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)
#pragma warning(disable : 6001)
#pragma warning(disable : 6385)
#pragma warning(disable : 6386)
#include "sc2api/sc2_common.h"
#include "sc2api/sc2_unit.h"
#pragma warning(pop)

#include <map>
#include <vector>

#include "definitions.h"


namespace sc2
{

class Mediator;

struct mineral_patch_data
{
	bool is_close;
	const Unit* workers[3];
	mineral_patch_data()
	{
		is_close = false;
		workers[0] = nullptr;
		workers[1] = nullptr;
		workers[2] = nullptr;
	}
	mineral_patch_data(bool a)
	{
		is_close = a;
		workers[0] = nullptr;
		workers[1] = nullptr;
		workers[2] = nullptr;
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
	const Unit* mineral = nullptr;
	Point2D drop_off_point;
	Point2D pick_up_point;
};

struct assimilator_reversed_data
{
	const Unit* assimilator = nullptr;
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

enum RemoveWorkerResult
{
	REMOVED_FROM_MINERALS,
	REMOVED_FROM_GAS,
	NOT_FOUND
};

class WorkerManager
{
private:
	Mediator* mediator;
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

	bool immediatelySaturateGasses = false;
	bool immediatelySemiSaturateGasses = false;

	const Unit *new_base = nullptr;
	bool should_build_workers = true;
	int removed_gas_miners = 0;
	bool balance_income = false;

public:
	WorkerManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}
	void DisplayWorkerStatus();

	void SetBuildWorkers(bool);
	bool CheckBuildWorkers() const;
	void SetImmediatlySaturateGasses(bool);
	void SetImmediatlySemiSaturateGasses(bool);
	void SetBalanceIncome(bool);
	int GetExtraWorkersOnMinerals() const;

	const Unit* GetWorker() const;
	const Unit* GetBuilder(Point2D) const;
	void PlaceWorker(const Unit*);
	void PlaceWorkerInGas(const Unit*, const Unit*, int);
	void NewPlaceWorkerInGas(const Unit*, const Unit*);
	void PlaceWorkerOnMinerals(const Unit*, const Unit*, int);
	void NewPlaceWorkerOnMinerals(const Unit*, const Unit*);
	RemoveWorkerResult RemoveWorker(const Unit*);
	void SplitWorkers();
	void SaturateGas(const Unit*);
	void SemiSaturateGas(const Unit*);
	void AddNewBase();
	void RemoveBase(const Unit*);
	void DistributeWorkers();
	void BalanceWorkers();
	void BuildWorkers();
	bool PullOutOfGas();
	void PullOutOfGas(int);
	UnitCost CalculateIncome() const;

	void AddAssimilator(const Unit*);
	void SetNewBase(const Unit*);
	const Unit* GetNewBase() const;
	void RemoveMineralPatch(const Unit*);

	void OnUnitDestroyed(const Unit*);
};

}