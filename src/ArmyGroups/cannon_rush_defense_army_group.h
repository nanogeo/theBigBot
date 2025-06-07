#pragma once

#include "utility.h"
#include "path_manager.h"
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class CannonRushDefenseArmyGroup : public ArmyGroup
{
public:
	std::map<const Unit*, Units> assigned_attackers; // enemy cannon/pylon/probe -> probes attacking
	std::map<const Unit*, const Unit*> probe_targets; // probe -> attack target

	CannonRushDefenseArmyGroup(Mediator*);
	~CannonRushDefenseArmyGroup();

	void Run() override;
	void RemoveUnit(const Unit*) override;
	std::string ToString() override
	{
		return "Cannon rush defense army group";
	}
};

}