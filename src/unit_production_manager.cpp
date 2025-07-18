
#include "definitions.h"
#include "mediator.h"
#include "unit_production_manager.h"


namespace sc2
{

void UnitProductionManager::ChooseUnitProduction()
{
	std::map<UNIT_TYPEID, uint16_t> units;
	for (const auto& unit : mediator->GetUnits(Unit::Alliance::Self))
	{
		if (units.find(unit->unit_type) != units.end())
			units[unit->unit_type] = units[unit->unit_type] + 1;
		else
			units[unit->unit_type] = 1;
	}

	std::map<UNIT_TYPEID, uint16_t> needed_units;

	for (const auto& unit_type : target_unit_comp)
	{
		if (unit_type.second > units[unit_type.first])
			needed_units[unit_type.first] = unit_type.second - units[unit_type.first];
	}


}

void UnitProductionManager::SetWarpgateProduction(UNIT_TYPEID unit_type)
{
	// TODO check if this is a unit made by the warp gate
	warpgate_production = unit_type;
	std::cerr << "Warpgate production changed to " << UnitTypeToName(unit_type) << std::endl;
}

void UnitProductionManager::SetRoboProduction(UNIT_TYPEID unit_type)
{
	// TODO check if this is a unit made by the robo
	robo_production = unit_type;
	std::cerr << "Robo production changed to " << UnitTypeToName(unit_type) << std::endl;
}

void UnitProductionManager::SetStargateProduction(UNIT_TYPEID unit_type)
{
	// TODO check if this is a unit made by the stargate
	stargate_production = unit_type;
	std::cerr << "Stargate production changed to " << UnitTypeToName(unit_type) << std::endl;
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
	case GATEWAY:
		gateways.push_back(building);
		break;
	}
}

void UnitProductionManager::RunUnitProduction()
{
	UpdateWarpgateStatus();

	if (automatic_unit_production)
		RunAutomaticUnitProduction();
	else
		RunSpecificUnitProduction();

	
}

void UnitProductionManager::RunAutomaticUnitProduction()
{
	std::map<UNIT_TYPEID, uint16_t> units;
	for (const auto& unit : mediator->GetUnits(Unit::Alliance::Self))
	{
		if (units.find(unit->unit_type) != units.end())
			units[unit->unit_type] = units[unit->unit_type] + 1;
		else
			units[unit->unit_type] = 1;
	}

	std::map<UNIT_TYPEID, uint16_t> needed_gateway_units;
	std::map<UNIT_TYPEID, uint16_t> needed_robo_units;
	std::map<UNIT_TYPEID, uint16_t> needed_stargate_units;

	for (const auto& unit_type : target_unit_comp)
	{
		if (unit_type.second > units[unit_type.first])
		{
			switch (Utility::GetBuildStructure(unit_type.first))
			{
			case GATEWAY:
				needed_gateway_units[unit_type.first] = unit_type.second - units[unit_type.first];
				break;
			case STARGATE:
				needed_stargate_units[unit_type.first] = unit_type.second - units[unit_type.first];
				break;
			case ROBO:
				needed_robo_units[unit_type.first] = unit_type.second - units[unit_type.first];
				break;
			default:
				std::cerr << "Error, unknown build structure in RunAutomaticUnitProduction for unit type: " << UnitTypeToName(unit_type.first) << std::endl;
				break;
			}
		}
	}

	for (const auto& stargate : stargates)
	{
		if (stargate->orders.size() != 0 && stargate->is_powered == false)
			continue;
		for (auto itr = needed_stargate_units.begin(); itr != needed_stargate_units.end(); itr++)
		{
			TryActionResult result = mediator->TryTrainUnit(stargate, itr->first);
			if (result == TryActionResult::success)
			{
				if (itr->second == 1)
				{
					itr = needed_stargate_units.erase(itr);
				}
				else
				{
					itr->second = itr->second - 1;
				}
				break;
			}
		}
	}

	for (const auto& robo : robos)
	{
		if (robo->orders.size() != 0 || robo->is_powered == false)
			continue;
		
		for (auto itr = needed_robo_units.begin(); itr != needed_robo_units.end(); itr++)
		{
			TryActionResult result = mediator->TryTrainUnit(robo, itr->first);
			if (result == TryActionResult::success)
			{
				if (itr->second == 1)
				{
					itr = needed_robo_units.erase(itr);
				}
				else
				{
					itr->second = itr->second - 1;
				}
				break;
			}
		}
	}

	if (mediator->CheckUpgrade(UPGRADE_ID::WARPGATERESEARCH))
	{
		std::vector<Point2D> spots = FindWarpInSpots(mediator->GetEnemyStartLocation());
		for (const auto& warpgate : warpgates)
		{
			if (warpgate_status[warpgate].frame_ready != 0 || warpgate->is_powered == false)
				continue;
			if (spots.size() == 0)
				break;

			for (auto itr = needed_gateway_units.begin(); itr != needed_gateway_units.end(); itr++)
			{
				TryActionResult result = mediator->TryWarpIn(warpgate, itr->first, spots.back());
				while (result == TryActionResult::invalid_position && spots.size() > 1)
				{
					spots.pop_back();
					result = mediator->TryWarpIn(warpgate, itr->first, spots.back());
				}
				if (result == TryActionResult::success)
				{
					warpgate_status[warpgate].used = true;
					warpgate_status[warpgate].frame_ready = mediator->GetGameLoop() + (int)round(Utility::GetWarpCooldown(itr->first) * FRAME_TIME);
					spots.pop_back();
					if (itr->second == 1)
					{
						itr = needed_gateway_units.erase(itr);
					}
					else
					{
						itr->second = itr->second - 1;
					}
					break;
				}
				else if (result == TryActionResult::invalid_position)
				{
					break;
				}
			}
		}
	}
	else
	{
		for (const auto& gateway : gateways)
		{
			if (gateway->orders.size() != 0 && gateway->is_powered == false)
				continue;
			for (auto itr = needed_gateway_units.begin(); itr != needed_gateway_units.end(); itr++)
			{
				TryActionResult result = mediator->TryTrainUnit(gateway, itr->first);
				if (result == TryActionResult::success)
				{
					if (itr->second == 1)
					{
						itr = needed_gateway_units.erase(itr);
					}
					else
					{
						itr->second = itr->second - 1;
					}
					break;
				}
			}
		}
	}
}

void UnitProductionManager::RunSpecificUnitProduction()
{
	if (stargate_production != UNIT_TYPEID::INVALID)
	{
		// use stargates
		for (const auto& stargate : stargates)
		{
			if (stargate->orders.size() != 0 || stargate->is_powered == false)
				continue;
		
			TryActionResult result = mediator->TryTrainUnit(stargate, stargate_production);
			if (result == TryActionResult::cannot_afford || result == TryActionResult::low_tech)
			{
				break;
			}
		}
	}

	if (robo_production != UNIT_TYPEID::INVALID)
	{
		// use robos
		for (const auto& robo : robos)
		{
			if (robo->orders.size() != 0 || robo->is_powered == false)
				continue;

			TryActionResult result = mediator->TryTrainUnit(robo, robo_production);
			if (result == TryActionResult::cannot_afford || result == TryActionResult::low_tech)
			{
				break;
			}
		}
	}
	if (mediator->CheckUpgrade(UPGRADE_ID::WARPGATERESEARCH))
	{
		if (warpgate_production != UNIT_TYPEID::INVALID)
		{
			// use warpgates
			std::vector<Point2D> spots = FindWarpInSpots(mediator->GetEnemyStartLocation());
			for (const auto& warpgate : warpgates)
			{
				if (warpgate_status[warpgate].frame_ready != 0 || warpgate->is_powered == false)
					continue;
				if (spots.size() == 0)
					break;

				TryActionResult result = mediator->TryWarpIn(warpgate, warpgate_production, spots.back());
				while (result == TryActionResult::invalid_position && spots.size() > 1)
				{
					spots.pop_back();
					result = mediator->TryWarpIn(warpgate, warpgate_production, spots.back());
				}
				if (result == TryActionResult::success)
				{
					warpgate_status[warpgate].used = true;
					warpgate_status[warpgate].frame_ready = mediator->GetGameLoop() + (int)round(Utility::GetWarpCooldown(warpgate_production) * FRAME_TIME);
					spots.pop_back();
				}
				else if (result == TryActionResult::invalid_position || result == TryActionResult::low_tech || result == TryActionResult::cannot_afford)
				{
					break;
				}
			}
		}
	}
	else
	{
		if (warpgate_production != UNIT_TYPEID::INVALID)
		{
			// use gateways
			for (const auto& gateway : gateways)
			{
				if (gateway->orders.size() != 0 || gateway->is_powered == false)
					continue;
				TryActionResult result = mediator->TryTrainUnit(gateway, warpgate_production);
				if (result == TryActionResult::cannot_afford || result == TryActionResult::low_tech)
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
	for (const auto& pylon : mediator->GetUnits(Unit::Alliance::Self, IsFinishedUnit(PYLON)))
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

				Point2D pos = Point2D(pylon->pos.x + i + .5f, pylon->pos.y + j + .5f);
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
						if (building->unit_type == PYLON || building->unit_type == CANNON || building->unit_type == BATTERY)
						{
							if (building->pos.x - .5 <= pos.x && building->pos.x + .5 >= pos.x && building->pos.y - .5 <= pos.y && building->pos.y + .5 >= pos.y)
							{
								blocked = true;
								break;
							}
						}
						else if (building->unit_type == NEXUS)
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

	for (const auto& prism : mediator->GetUnits(Unit::Alliance::Self, IsFinishedUnit(PRISM_SIEGED)))
	{
		for (int i = -4; i <= 4; i += 1)
		{
			for (int j = -4; j <= 4; j += 1)
			{
				Point2D pos = Point2D(prism->pos.x + i + .5f, prism->pos.y + j + .5f);
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

std::vector<Point2D> UnitProductionManager::FindWarpInSpotsAt(Point2D pos)
{
	std::vector<Point2D> spots = FindWarpInSpots(pos);
	for (size_t i = 0; i < spots.size(); i++)
	{
		if (Distance2D(spots[i], pos) > 10)
		{
			spots.erase(spots.begin() + i);
			i--;
		}
	}
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

int UnitProductionManager::NumWarpgatesReady()
{
	int gates_ready = 0;
	for (const auto& status : warpgate_status)
	{
		if (status.second.frame_ready == 0)
			gates_ready++;
	}
	return gates_ready;
}

UnitCost UnitProductionManager::CalculateCostOfProduction()
{
	float mineral_cost = 0;
	float gas_cost = 0;
	if (warpgate_production != UNIT_TYPEID::INVALID)
	{
		UnitCost warpgate_cost = Utility::GetCost(warpgate_production) * 
			(mediator->GetNumBuildActions(GATEWAY) + (int)mediator->GetUnits(Unit::Alliance::Self, IsUnits({ GATEWAY, WARP_GATE })).size());
		mineral_cost += (float)warpgate_cost.mineral_cost * 60.0f / Utility::GetTrainingTime(warpgate_production);
		gas_cost += (float)warpgate_cost.vespene_cost * 60.0f / Utility::GetTrainingTime(warpgate_production);
	}
	if (robo_production != UNIT_TYPEID::INVALID)
	{
		UnitCost robo_cost = Utility::GetCost(robo_production) *
			(mediator->GetNumBuildActions(ROBO) + (int)mediator->GetUnits(Unit::Alliance::Self, IsUnit(ROBO)).size());
		mineral_cost += (float)robo_cost.mineral_cost * 60.0f / Utility::GetTrainingTime(robo_production);
		gas_cost += (float)robo_cost.vespene_cost * 60.0f / Utility::GetTrainingTime(robo_production);
	}
	if (stargate_production != UNIT_TYPEID::INVALID)
	{
		UnitCost stargate_cost = Utility::GetCost(stargate_production) *
			(mediator->GetNumBuildActions(STARGATE) + (int)mediator->GetUnits(Unit::Alliance::Self, IsUnit(STARGATE)).size());
		mineral_cost += (float)stargate_cost.mineral_cost * 60.0f / Utility::GetTrainingTime(stargate_production);
		gas_cost += (float)stargate_cost.vespene_cost * 60.0f / Utility::GetTrainingTime(stargate_production);
	}

	if (mediator->CheckBuildWorkers())
	{
		mineral_cost += mediator->GetUnits(Unit::Alliance::Self, IsUnit(NEXUS)).size() * 250.0f; // 50 minerals per probe, 5 probes per minute
	}

	return UnitCost((int)mineral_cost, (int)gas_cost, 0);
}

void UnitProductionManager::WarpInUnit(UNIT_TYPEID unit_type, Point2D pos)
{
	if (mediator->CanAfford(unit_type, 1))
	{
		for (const auto& warpgate : warpgates)
		{
			if (warpgate_status[warpgate].frame_ready == 0 && mediator->TryWarpIn(warpgate, unit_type, pos) == TryActionResult::success)
			{
				warpgate_status[warpgate].used = true;
				warpgate_status[warpgate].frame_ready = mediator->GetGameLoop() + (int)round(Utility::GetWarpCooldown(unit_type) * FRAME_TIME);
				return;
			}
		}
	}
}

bool UnitProductionManager::WarpInUnits(UNIT_TYPEID unit_type, int num, Point2D pos)
{
	std::vector<Point2D> spots = FindWarpInSpots(pos);
	if (spots.size() < num || mediator->CanAfford(unit_type, num) == false || NumWarpgatesReady() < num)
		return false;

	int successful_warpins = 0;
	for (const auto& warpgate : warpgates)
	{
		if (warpgate_status[warpgate].frame_ready != 0)
			continue;
		if (spots.size() == 0)
			break;
		TryActionResult result = mediator->TryWarpIn(warpgate, unit_type, spots.back());
		while (result == TryActionResult::invalid_position && spots.size() > 1)
		{
			spots.pop_back();
			result = mediator->TryWarpIn(warpgate, unit_type, spots.back());
		}
		if (result == TryActionResult::success)
		{
			warpgate_status[warpgate].used = true;
			warpgate_status[warpgate].frame_ready = mediator->GetGameLoop() + (int)round(Utility::GetWarpCooldown(unit_type) * FRAME_TIME);
			spots.pop_back();
			successful_warpins++;
		}
		else if (result == TryActionResult::invalid_position || result == TryActionResult::low_tech || result == TryActionResult::cannot_afford)
		{
			break;
		}
		if (successful_warpins >= num)
			return true;
	}
	return false;
}

bool UnitProductionManager::WarpInUnitsAt(UNIT_TYPEID unit_type, int num, Point2D pos)
{
	std::vector<Point2D> spots = FindWarpInSpotsAt(pos);
	if (spots.size() < num || mediator->CanAfford(unit_type, num) == false || NumWarpgatesReady() < num)
		return false;

	int successful_warpins = 0;
	for (const auto& warpgate : warpgates)
	{
		if (warpgate_status[warpgate].frame_ready != 0)
			continue;
		if (spots.size() == 0)
			break;
		TryActionResult result = mediator->TryWarpIn(warpgate, unit_type, spots.back());
		while (result == TryActionResult::invalid_position && spots.size() > 1)
		{
			spots.pop_back();
			result = mediator->TryWarpIn(warpgate, unit_type, spots.back());
		}
		if (result == TryActionResult::success)
		{
			warpgate_status[warpgate].used = true;
			warpgate_status[warpgate].frame_ready = mediator->GetGameLoop() + (int)round(Utility::GetWarpCooldown(unit_type) * FRAME_TIME);
			spots.pop_back();
			successful_warpins++;
		}
		else if (result == TryActionResult::invalid_position || result == TryActionResult::low_tech || result == TryActionResult::cannot_afford)
		{
			break;
		}
		if (successful_warpins >= num)
			return true;
	}
	return false;
}


std::map<UNIT_TYPEID, uint16_t> UnitProductionManager::GetTargetUnitComp()
{
	return target_unit_comp;
}

void UnitProductionManager::IncreaseUnitAmountInTargetComposition(UNIT_TYPEID unit_type, uint16_t amount)
{
	if (target_unit_comp.find(unit_type) != target_unit_comp.end())
		target_unit_comp[unit_type] = target_unit_comp[unit_type] + amount;
	else
		target_unit_comp[unit_type] = amount;
}

void UnitProductionManager::DecreaseUnitAmountInTargetComposition(UNIT_TYPEID unit_type, uint16_t amount)
{
	if (target_unit_comp.find(unit_type) != target_unit_comp.end())
	{
		if (target_unit_comp[unit_type] > amount)
			target_unit_comp[unit_type] = target_unit_comp[unit_type] - amount;
		else
			target_unit_comp[unit_type] = 0;
	}
}

}