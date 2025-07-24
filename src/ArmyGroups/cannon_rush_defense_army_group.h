#pragma once

#include "utility.h"
#include "piecewise_path.h"
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class CannonRushDefenseArmyGroup : public ArmyGroup
{
protected:
	std::map<const Unit*, Units> assigned_attackers; // enemy cannon/pylon/probe -> probes attacking
	std::map<const Unit*, const Unit*> probe_targets; // probe -> attack target

public:
	CannonRushDefenseArmyGroup(Mediator*);
	~CannonRushDefenseArmyGroup();

	void Run() override;
	void RemoveUnit(const Unit*) override;
	std::string ToString() const override
	{
		return "Cannon rush defense army group";
	}
};

}