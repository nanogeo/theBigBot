
#include "definitions.h"
#include "mediator.h"
#include "unit_production_manager.h"


namespace sc2
{

void UnitProductionManager::SetWarpgateProduction(UNIT_TYPEID unit_type)
{
	// TODO check if this is a unit made by the warp gate
	warpgate_production = unit_type;
}

void UnitProductionManager::SetRoboProduction(UNIT_TYPEID unit_type)
{
	// TODO check if this is a unit made by the robo
	robo_production = unit_type;
}

void UnitProductionManager::SetStargateProduction(UNIT_TYPEID unit_type)
{
	// TODO check if this is a unit made by the stargate
	stargate_production = unit_type;
}

void UnitProductionManager::OnBuildingConstructionComplete(const Unit* building)
{
	switch (building->unit_type.ToType())
	{
	case WARP_GATE:
		warpgates.push_back(building);
		warpgate_status[building] = WarpgateStatus(mediator->GetGameLoop());
		break;
	case ROBO:
		robos.push_back(building);
		break;
	case STARGATE:
		stargates.push_back(building);
		break;
	}
}

void UnitProductionManager::RunUnitProduction()
{
	UpdateWarpgateStatus();

	resources_used_this_frame = UnitCost(0, 0, 0);

	if (stargate_production != UNIT_TYPEID::BALL)
	{
		// use stargates
		for (const auto& stargate : stargates)
		{
			if (stargate->orders.size() == 0)
			{
				if (mediator->CanAffordAfter(stargate_production, resources_used_this_frame))
				{
					resources_used_this_frame += Utility::GetCost(stargate_production);
					mediator->SetUnitCommand(stargate, Utility::GetTrainAbility(stargate_production), 0);
				}
				else
				{
					break;
				}
			}
		}
	}

	if (robo_production != UNIT_TYPEID::BALL)
	{
		// use robos
		for (const auto& robo : robos)
		{
			if (robo->orders.size() == 0)
			{
				if (mediator->CanAffordAfter(robo_production, resources_used_this_frame))
				{
					resources_used_this_frame += Utility::GetCost(robo_production);
					mediator->SetUnitCommand(robo, Utility::GetTrainAbility(robo_production), 0);
				}
				else
				{
					break;
				}
			}
		}
	}

	if (warpgate_production != UNIT_TYPEID::BALL)
	{
		// use warpgates
		std::vector<Point2D> spots = FindWarpInSpots(mediator->GetEnemyStartLocation());
		for (const auto& warpgate : warpgates)
		{
			if (warpgate_status[warpgate].frame_ready == 0)
			{
				if (mediator->CanAffordAfter(warpgate_production, resources_used_this_frame) && spots.size() > 0)
				{
					mediator->SetUnitCommand(warpgate, Utility::GetWarpAbility(warpgate_production), spots.back(), 0);
					warpgate_status[warpgate].used = true;
					warpgate_status[warpgate].frame_ready = mediator->GetGameLoop() + round(Utility::GetWarpCooldown(warpgate_production) * 22.4);
					spots.pop_back();
				}
				else
				{
					break;
				}
			}
		}
	}
}

void UnitProductionManager::SetWarpInAtProxy(bool status)
{
	warp_in_at_proxy = status;
}

std::vector<Point2D> UnitProductionManager::FindWarpInSpots(Point2D close_to)
{
	// order by worst to best

	std::vector<Point2D> spots;
	for (const auto& pylon : mediator->GetUnits(Unit::Alliance::Self, IsFinishedUnit(UNIT_TYPEID::PROTOSS_PYLON)))
	{
		// ignore slow warpins
		if (!Utility::AnyUnitWithin(mediator->GetUnits(Unit::Alliance::Self, IsUnits({ NEXUS, WARP_GATE })), pylon->pos, 6.5))
		{
			// allow slow warp ins for proxy pylons
			if (!warp_in_at_proxy || Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS)), pylon->pos) < 20)
				continue;
		}
		if (Utility::AnyUnitWithin(mediator->GetUnits(Unit::Alliance::Enemy), pylon->pos, 10))
			continue;
		for (int i = -7; i <= 6; i += 1)
		{
			for (int j = -7; j <= 6; j += 1)
			{

				Point2D pos = Point2D(pylon->pos.x + i + .5, pylon->pos.y + j + .5);
				if (mediator->IsPathable(pos) && Distance2D(pos, pylon->pos) <= 6 && mediator->ToPoint3D(pos).y > pylon->pos.y + .5)
				{

					bool blocked = false;
					for (const auto& avoid_spot : mediator->GetBadWarpInSpots())
					{
						if (Distance2D(pos, avoid_spot) < 3)
						{
							blocked = true;
							break;
						}
					}
					for (const auto& building : mediator->GetUnits(Unit::Alliance::Self, IsBuilding()))
					{
						if (building->unit_type == UNIT_TYPEID::PROTOSS_PYLON || building->unit_type == UNIT_TYPEID::PROTOSS_PHOTONCANNON || building->unit_type == UNIT_TYPEID::PROTOSS_SHIELDBATTERY)
						{
							if (building->pos.x - .5 <= pos.x && building->pos.x + .5 >= pos.x && building->pos.y - .5 <= pos.y && building->pos.y + .5 >= pos.y)
							{
								blocked = true;
								break;
							}
						}
						else if (building->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
						{
							if (building->pos.x - 2 <= pos.x && building->pos.x + 2 >= pos.x && building->pos.y - 2 <= pos.y && building->pos.y + 2 >= pos.y)
							{
								blocked = true;
								break;
							}
						}
						else
						{
							if (building->pos.x - 1 <= pos.x && building->pos.x + 1 >= pos.x && building->pos.y - 1 <= pos.y && building->pos.y + 1 >= pos.y)
							{
								blocked = true;
								break;
							}
						}
					}

					for (const auto& spot : spots)
					{
						if (Distance2D(pos, spot) < 1.5)
						{
							blocked = true;
							break;
						}
					}
					if (!blocked && !Utility::AnyUnitWithin(mediator->GetUnits(Unit::Alliance::Self), pos, 1.5) && !Utility::AnyUnitWithin(mediator->GetUnits(Unit::Alliance::Neutral), pos, 1.5))
						spots.push_back(pos);


				}
				else
				{

				}
				//if (spots.size() >= 10)
				//	break;
			}
			//if (spots.size() >= 10)
			//	break;
		}
	}


	std::vector<Point2D> prism_spots;

	for (const auto& prism : mediator->GetUnits(Unit::Alliance::Self, IsFinishedUnit(UNIT_TYPEID::PROTOSS_WARPPRISMPHASING)))
	{
		for (int i = -4; i <= 4; i += 1)
		{
			for (int j = -4; j <= 4; j += 1)
			{
				Point2D pos = Point2D(prism->pos.x + i + .5, prism->pos.y + j + .5);
				if (mediator->IsPathable(pos) && Distance2D(pos, prism->pos) <= 3.75 && Utility::DistanceToClosest(mediator->GetUnits(Unit::Alliance::Self), pos) > 1)
				{
					prism_spots.push_back(pos);
				}
			}
		}
	}


	sort(spots.begin(), spots.end(),
		[close_to](const Point2D& a, const Point2D& b) -> bool
	{
		return Distance2D(a, close_to) > Distance2D(b, close_to);
	});

	spots.insert(spots.end(), prism_spots.begin(), prism_spots.end());


	return spots;
}

void UnitProductionManager::UpdateWarpgateStatus()
{
	for (const auto& warpgate : mediator->GetUnits(Unit::Alliance::Self, IsUnit(WARP_GATE)))
	{
		if (warpgate_status.count(warpgate) == 0)
		{
			warpgate_status[warpgate] = WarpgateStatus();
			continue;
		}
		WarpgateStatus* status = &warpgate_status[warpgate];
		if (status->used)
		{
			bool gate_ready = false;
			for (const auto& ability : mediator->GetAbilitiesForUnit(warpgate).abilities)
			{
				if (ability.ability_id == ABILITY_ID::TRAINWARP_ZEALOT)
				{
					gate_ready = true;
					break;
				}
			}
			if (gate_ready)
			{
				std::cerr << "Warpgate reset" << std::endl;
				status->frame_ready = 0;
			}
			status->used = false;
		}
		if (status->frame_ready > 0 && status->frame_ready <= mediator->GetGameLoop())
			status->frame_ready = 0;
	}
}




}