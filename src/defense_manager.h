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
	bool reset_warpgate_production = false;
	bool reset_robo_production = false;
	bool reset_stargate_production = false;
	float last_time_overcharge_used = 0; // BATTERY_OVERCHARGE

	DefenseManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}


	void CheckForAttacks();
	void UpdateOngoingAttacks();
	float JudgeFight(Units, Units, float, float, bool);

	void UseBatteryOvercharge(Point2D);  // BATTERY_OVERCHARGE
};

}