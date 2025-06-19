
#include "observer_scout_army_group.h"
#include "mediator.h"
#include "definitions.h"

namespace sc2 {

ObserverScoutArmyGroup::ObserverScoutArmyGroup(Mediator* mediator) : ArmyGroup(mediator)
{
	this->unit_types.push_back(OBSERVER);
	desired_units = 1;
	max_units = 1;

	this->main_pos = mediator->GetEnemyStartLocation();
	current_target = main_pos;
	this->natural_pos = mediator->GetEnemyNaturalLocation();
}

ObserverScoutArmyGroup::ObserverScoutArmyGroup(Mediator* mediator, ArmyTemplate<ObserverScoutArmyGroup>* army_template) : ArmyGroup(mediator)
{
	this->unit_types.push_back(OBSERVER);
	desired_units = 1;
	max_units = 1;

	this->main_pos = mediator->GetEnemyStartLocation();
	current_target = main_pos;
	this->natural_pos = mediator->GetEnemyNaturalLocation();
}

void ObserverScoutArmyGroup::Run()
{
	// update target
	Point2D obs = Utility::Center(all_units);
	if (obs == Point2D(0, 0))
		return;

	double s = -1 * (main_pos.x - main_pos.x) / (main_pos.y - natural_pos.y);
	double main_direction = s * (natural_pos.x - main_pos.x) - natural_pos.y + main_pos.y;
	double natural_direction = s * (main_pos.x - natural_pos.x) - main_pos.y + natural_pos.y;

	double outside_main = s * (obs.x - main_pos.x) - obs.y + main_pos.y;
	double outside_natural = s * (obs.x - natural_pos.x) - obs.y + natural_pos.y;

	if (current_target == main_pos && ((main_direction * outside_main) < 0 || Distance2D(obs, main_pos) < 10))
		current_target = natural_pos;
	else if (current_target == natural_pos && ((natural_direction * outside_natural) < 0 || Distance2D(obs, natural_pos) < 10))
		current_target = main_pos;
	
	Units enemy_units = mediator->GetUnits(Unit::Alliance::Enemy);
	for (const auto& obs : all_units)
	{
		const Unit* closest_danger = Utility::ClosestUnitTo(Utility::GetUnitsThatCanAttack(enemy_units, obs, 1), obs->pos);
		if (closest_danger == nullptr)
			mediator->SetUnitCommand(obs, A_MOVE, current_target, 0);
		else
			mediator->SetUnitCommand(obs, A_MOVE, Utility::PointBetween(obs->pos, closest_danger->pos, -1), 0);
	}
}

void ObserverScoutArmyGroup::AddNewUnit(const Unit* unit)
{
	AddUnit(unit);
}

}