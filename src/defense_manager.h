#pragma once



namespace sc2
{


class Mediator;

class DefenseManager
{
public:
	Mediator* mediator;
	std::vector<Point2D> ongoing_attacks;
	bool reset_warpgate_production = false;
	bool reset_robo_production = false;
	bool reset_stargate_production = false;

	DefenseManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}


	void CheckForAttacks();
	void UpdateOngoingAttacks();
	float JudgeFight(Units, Units, float, float, bool);
};

}