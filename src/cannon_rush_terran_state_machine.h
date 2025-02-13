#pragma once

#include "finite_state_machine.h"


namespace sc2
{

class Mediator;

class CannonRushTerran;
class TheBigBot;
class ArmyGroup;

#pragma region CannonRushTerran

class CannonRushTerranMoveAcross : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	CannonRushTerranMoveAcross(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranFindAvaibleCorner : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	int curr_index;
	Point2D current_target;
	CannonRushTerranFindAvaibleCorner(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe, int curr_index)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
		this->curr_index = curr_index;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranScout : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	int index;
	std::vector<Point2D> main_scout_path;
	Point2D current_target;
	bool gas_stolen;
	CannonRushTerranScout(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe, int index, std::vector<Point2D> main_scout_path, bool gas_stolen)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
		this->index = index;
		this->main_scout_path = main_scout_path;
		this->gas_stolen = gas_stolen;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranFindThreePylonWallOff : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	CannonRushTerranFindThreePylonWallOff(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranGasSteal : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	int scouting_index;
	const Unit* gas;
	CannonRushTerranGasSteal(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe, int scouting_index, const Unit* gas)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
		this->scouting_index = scouting_index;
		this->gas = gas;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranMoveAcross2 : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	CannonRushTerranMoveAcross2(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranFindWallOffSpot : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	int index;
	CannonRushTerranFindWallOffSpot(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe, int index)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
		this->index = index;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranWallOff : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	Point2D cannon_pos;
	Point2D cannon_move_to;
	std::vector<BuildingPlacement> wall_pos;
	int index = 0;
	CannonRushTerranWallOff(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe, Point2D cannon_pos,
		Point2D cannon_move_to, std::vector<BuildingPlacement> wall_pos)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
		this->cannon_pos = cannon_pos;
		this->cannon_move_to = cannon_move_to;
		this->wall_pos = wall_pos;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranCannonFirstWallOff : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	Point2D cannon_pos;
	std::vector<BuildingPlacement> pylon_wall_pos;
	std::vector<BuildingPlacement> gateway_wall_pos;
	bool cannon_placed = false;
	std::vector<BuildingPlacement> wall = {};
	bool wall_set = false;
	CannonRushTerranCannonFirstWallOff(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe, Point2D cannon_pos,
		std::vector<BuildingPlacement> pylon_wall_pos, std::vector<BuildingPlacement> gateway_wall_pos)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
		this->cannon_pos = cannon_pos;
		this->pylon_wall_pos = pylon_wall_pos;
		this->gateway_wall_pos = gateway_wall_pos;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranStandBy : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	Point2D stand_by_spot;
	CannonRushTerranStandBy(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe, Point2D stand_by_spot)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
		this->stand_by_spot = stand_by_spot;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranStandByLoop : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	std::vector<Point2D> loop_path;
	int index = 0;
	CannonRushTerranStandByLoop(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe, std::vector<Point2D> loop_path)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
		this->loop_path = loop_path;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class CannonRushTerranExtraPylon : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	Point2D pylon_pos;
	CannonRushTerranExtraPylon(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
	Point2D FindPylonPlacement();
};

class CannonRushTerranExtraCannon : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	Point2D cannon_pos;
	CannonRushTerranExtraCannon(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
	Point2D FindCannonPlacement();
};

class CannonRushTerranBuildGateway : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	Point2D gate_pos;
	CannonRushTerranBuildGateway(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
	Point2D FindGatewayPlacement();
};

class CannonRushTerranBuildStargate : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	Point2D stargate_pos;
	CannonRushTerranBuildStargate(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
	Point2D FindStargatePlacement();
};

class CannonRushTerranStandByPhase2 : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	bool probe_busy = false;
	Point2D stand_by_spot;
	UNIT_TYPEID next_unit;
	CannonRushTerranStandByPhase2(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* probe, Point2D stand_by_spot)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->probe = probe;
		this->stand_by_spot = stand_by_spot;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
	Point2D FindBuildingPlacement();
	Point2D FindBatteryPlacement();
	Point2D FindCannonPlacement();
	Point2D FindPylonPlacement();
};

class CannonRushTerranUnitMicro : public State
{
public:
	class CannonRushTerran* state_machine;
	ArmyGroup* army;
	int event_id;
	CannonRushTerranUnitMicro(TheBigBot* agent, CannonRushTerran* state_machine, const Unit* zealot);
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

#pragma endregion


class CannonRushTerran : public StateMachine
{
public:
	const Unit* probe;
	Units pylons;
	Units cannons;
	Units batteries;
	Units gateways;
	Units stargates;
	std::vector<Point2D> cannon_places;
	std::vector<Point2D> gateway_places;

	int event_id;
	CannonRushTerran(TheBigBot* agent, std::string name, const Unit* probe, int variation);

	~CannonRushTerran();

	virtual void RunStateMachine() override;


	void OnUnitCreatedListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);
	void SmallBuildingBlock(Point2D);
	void BigBuildingBlock(Point2D);

};


}