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
	bool run_defense_manager = true;
	bool temp_warpgate_production = false;
	bool temp_robo_production = false;
	bool temp_stargate_production = false;

	DefenseManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}


	void CheckForAttacks();
	void UpdateOngoingAttacks();
	float JudgeFight(Units, Units, float, float, bool);

	void UseBatteries();
	void RemoveOngoingAttackAt(Point2D);
};

}