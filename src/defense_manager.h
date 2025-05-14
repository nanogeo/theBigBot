#pragma once



namespace sc2
{


class Mediator;

struct OngoingAttack
{
	Point2D location;
	float status;
	std::vector<const Unit*> pulled_workers;
	OngoingAttack(Point2D location, float status, std::vector<const Unit*> pulled_workers)
	{
		this->location = location;
		this->status = status;
		this->pulled_workers = pulled_workers;
	}
	bool operator==(const OngoingAttack u) const
	{
		return location == u.location && status == u.status && pulled_workers == u.pulled_workers;
	}
};

class DefenseManager
{
public:
	Mediator* mediator;
	std::vector<OngoingAttack> ongoing_attacks;
	bool temp_unit_production = false;
	bool reset_warpgate_production = false;
	UNIT_TYPEID prev_warpgate_production = UNIT_TYPEID::INVALID;
	bool reset_robo_production = false;
	UNIT_TYPEID prev_robo_production = UNIT_TYPEID::INVALID;
	bool reset_stargate_production = false;
	UNIT_TYPEID prev_stargate_production = UNIT_TYPEID::INVALID;

	DefenseManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}


	void CheckForAttacks();
	void UpdateOngoingAttacks();
	float JudgeFight(Units, Units, float, float, bool);

	void UseBatteryOvercharge(Point2D);  // BATTERY_OVERCHARGE
	void RemoveOngoingAttackAt(Point2D);
};

}