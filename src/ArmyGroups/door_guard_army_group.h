#pragma once

#include "utility.h"
#include "piecewise_path.h"
#include "definitions.h"
#include "army_group.h"



namespace sc2
{

class Mediator;


class DoorGuardArmyGroup : public ArmyGroup
{
protected:
	const Unit* guard = nullptr;
	Point2D door_closed_pos;
	Point2D door_open_pos;

public:
	DoorGuardArmyGroup(Mediator*, Point2D, Point2D);

	void Run() override;
	std::string ToString() const override
	{
		return "Door guard army group";
	}

	void AddNewUnit(const Unit*) override;
	void RemoveUnit(const Unit*) override;
};

}