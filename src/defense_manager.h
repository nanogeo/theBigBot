#pragma once



namespace sc2
{


class Mediator;

class DefenseManager
{
public:
	Mediator* mediator;
	std::vector<Point2D> ongoing_attacks;

	DefenseManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}


	void CheckForAttacks();
};

}