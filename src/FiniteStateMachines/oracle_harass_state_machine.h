#pragma once

#include "finite_state_machine.h"

namespace sc2
{

class Mediator;
class OracleHarassStateMachine;
class TheBigBot;
class ArmyGroup;

class OracleScout : public State {
public:
	Units oracles;
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class OracleHarass : public State {
public:
	Units oracles;
	Point2D pos;
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class OracleDefendLocation : public State {
public:
	OracleHarassStateMachine* state_machine;
	Point2D denfensive_position;
	int event_id;
	OracleDefendLocation(TheBigBot* agent, OracleHarassStateMachine* state_machine, Point2D denfensive_position);
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;

	void OnUnitDamagedListener(const Unit*, float, float);
	void OnUnitDestroyedListener(const Unit*);
};

class OracleDefendLine : public State {
public:
	OracleHarassStateMachine* state_machine;
	LineSegmentLinearX* line;
	int event_id;
	OracleDefendLine(TheBigBot* agent, OracleHarassStateMachine* state_machine, Point2D start, Point2D end);
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;

	void OnUnitDamagedListener(const Unit*, float, float);
	void OnUnitDestroyedListener(const Unit*);
	void OnUnitCreatedListener(const Unit*);
};

class OracleDefendArmyGroup : public State {
public:
	OracleHarassStateMachine* state_machine;
	int event_id;
	OracleDefendArmyGroup(TheBigBot* agent, OracleHarassStateMachine* state_machine);
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;

	void OnUnitDamagedListener(const Unit*, float, float);
	void OnUnitDestroyedListener(const Unit*);
};

class OracleHarassGroupUp : public State {
public:
	OracleHarassStateMachine* state_machine;
	Point2D consolidation_pos;
	OracleHarassGroupUp(TheBigBot* agent, OracleHarassStateMachine* state_machine, Point2D consolidation_pos)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->consolidation_pos = consolidation_pos;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class OracleHarassMoveToEntrance : public State {
public:
	OracleHarassStateMachine* state_machine;
	Point2D entrance_pos;
	OracleHarassMoveToEntrance(TheBigBot* agent, OracleHarassStateMachine* state_machine, Point2D entrance_pos)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->entrance_pos = entrance_pos;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class OracleHarassAttackMineralLine : public State {
public:
	OracleHarassStateMachine* state_machine;
	Point2D exit_pos;
	const Unit* target_drone = nullptr;
	int event_id;
	bool lost_oracle = false;
	OracleHarassAttackMineralLine(TheBigBot* agent, OracleHarassStateMachine* state_machine, Point2D exit_pos);
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;

	void OnUnitDestroyedListener(const Unit*);
};

class OracleHarassReturnToBase : public State {
public:
	OracleHarassStateMachine* state_machine;
	std::vector<Point2D> exfil_path;
	OracleHarassReturnToBase(TheBigBot* agent, OracleHarassStateMachine* state_machine, std::vector<Point2D> exfil_path)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->exfil_path = exfil_path;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};



class OracleHarassStateMachine : public StateMachine
{
public:
	Units oracles;
	Point2D third_base_pos, door_guard_pos;
	bool sent_harass = false;
	std::map<const Unit*, float> time_last_attacked;
	std::map<const Unit*, bool> has_attacked;
	std::map<const Unit*, uint64_t> target;
	bool harass_direction = true;
	int harass_index = 0;
	int event_id;
	OracleHarassStateMachine(TheBigBot* agent, Units oracles, Point2D third_base_pos, Point2D door_guard_pos, std::string name);
	OracleHarassStateMachine(TheBigBot* agent, Units oracles, std::string name);

	~OracleHarassStateMachine();

	void AddOracle(const Unit*);
	bool AddUnit(const Unit*) override;
	void OnUnitDestroyedListener(const Unit*);
};

}