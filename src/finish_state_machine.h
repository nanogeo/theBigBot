#pragma once
#include <string>

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_unit_filters.h"

#include "TossBot.h"
#include "utility.h"
#include "locations.h"

namespace sc2 {

class State
{
public:
    TossBot* agent;
    State() {};
    virtual std::string toString();
    virtual void TickState();
    virtual void EnterState();
    virtual void ExitState();
    virtual State* TestTransitions();
};

#pragma region Oracle

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

class OracleCoverArmy : public State {
public:
    Units oracles;
    Point2D army_center;
    float danger_level;
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

#pragma endregion

#pragma region Oracle

class OracleHarassStateMachine;

class OracleDefendLocation : public State {
public:
	OracleHarassStateMachine* state_machine;
	Point2D denfensive_position;
	int event_id;
	OracleDefendLocation(TossBot* agent, OracleHarassStateMachine* state_machine, Point2D denfensive_position)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->denfensive_position = denfensive_position;
		event_id = agent->GetUniqueId();
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;

	void OnUnitDamagedListener(const Unit*, float, float);
	void OnUnitDestroyedListener(const Unit*);
};

class OracleDefendArmyGroup : public State {
public:
	OracleHarassStateMachine* state_machine;
	int event_id;
	OracleDefendArmyGroup(TossBot* agent, OracleHarassStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		event_id = agent->GetUniqueId();
	}
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
	OracleHarassGroupUp(TossBot* agent, OracleHarassStateMachine* state_machine, Point2D consolidation_pos)
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
	OracleHarassMoveToEntrance(TossBot* agent, OracleHarassStateMachine* state_machine, Point2D entrance_pos)
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
	const Unit* target_drone = NULL;
	int event_id;
	bool lost_oracle = false;
	OracleHarassAttackMineralLine(TossBot* agent, OracleHarassStateMachine* state_machine, Point2D exit_pos)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->exit_pos = exit_pos;
		event_id = agent->GetUniqueId();
	}
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
	OracleHarassReturnToBase(TossBot* agent, OracleHarassStateMachine* state_machine, std::vector<Point2D> exfil_path)
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


#pragma endregion


#pragma region Chargelot Allin

class ChargelotAllInStateMachine;

class ChargeAllInMovingToWarpinSpot : public State {
public:
    ChargelotAllInStateMachine* state_machine;
    ChargeAllInMovingToWarpinSpot(TossBot* agent, ChargelotAllInStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ChargeAllInWarpingIn : public State {
public:
    ChargelotAllInStateMachine* state_machine;
    ChargeAllInWarpingIn(TossBot* agent, ChargelotAllInStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

#pragma endregion

#pragma region Scout Z

class ScoutZergStateMachine;

class ScoutZInitialMove : public State
{
public:
    ScoutZergStateMachine* state_machine;
    ScoutZInitialMove(TossBot* agent, ScoutZergStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutZScoutMain : public State
{
public:
    ScoutZergStateMachine* state_machine;
    ScoutZScoutMain(TossBot* agent, ScoutZergStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutZScoutNatural : public State
{
public:
    ScoutZergStateMachine* state_machine;
    ScoutZScoutNatural(TossBot* agent, ScoutZergStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutZLookFor3rd : public State
{
public:
    ScoutZergStateMachine* state_machine;
    ScoutZLookFor3rd(TossBot* agent, ScoutZergStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

#pragma endregion

#pragma region Scout T

class ScoutTerranStateMachine;

class ScoutTInitialMove : public State
{
public:
    ScoutTerranStateMachine* state_machine;
    ScoutTInitialMove(TossBot* agent, ScoutTerranStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutTScoutMain : public State
{
public:
    ScoutTerranStateMachine* state_machine;
    ScoutTScoutMain(TossBot* agent, ScoutTerranStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutTScoutNatural : public State
{
public:
    ScoutTerranStateMachine* state_machine;
    ScoutTScoutNatural(TossBot* agent, ScoutTerranStateMachine* state_machine)
    {
        this->agent = agent;
        this->state_machine = state_machine;
    }
    virtual std::string toString() override;
    void TickState() override;
    virtual void EnterState() override;
    virtual void ExitState() override;
    virtual State* TestTransitions() override;
};

class ScoutTScoutRax : public State
{
public:
	const Unit* rax;
	ScoutTerranStateMachine* state_machine;
	ScoutTScoutRax(TossBot* agent, ScoutTerranStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ScoutTReturnToBase : public State
{
public:
	ScoutTerranStateMachine* state_machine;
	ScoutTReturnToBase(TossBot* agent, ScoutTerranStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

#pragma endregion

#pragma region ImmortalDrop

class ImmortalDropStateMachine;

class ImmortalDropWaitForImmortals : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropWaitForImmortals(TossBot* agent, ImmortalDropStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ImmortalDropInitialMove : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropInitialMove(TossBot* agent, ImmortalDropStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ImmortalDropMicroDrop : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	bool first_immortal_turn = true;
	bool immortal1_has_attack_order = false;
	bool immortal2_has_attack_order = false;
	ImmortalDropMicroDrop(TossBot* agent, ImmortalDropStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ImmortalDropMicroDropCarrying1 : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	int entry_frame;
	ImmortalDropMicroDropCarrying1(TossBot* agent, ImmortalDropStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		entry_frame = agent->Observation()->GetGameLoop();
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ImmortalDropMicroDropCarrying2 : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	int entry_frame;
	ImmortalDropMicroDropCarrying2(TossBot* agent, ImmortalDropStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		entry_frame = agent->Observation()->GetGameLoop();
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ImmortalDropMicroDropDropped1 : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	bool immortal1_has_attack_order = false;
	bool immortal2_has_attack_order = false;
	ImmortalDropMicroDropDropped1(TossBot* agent, ImmortalDropStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ImmortalDropMicroDropDropped2 : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	bool immortal1_has_attack_order = false;
	bool immortal2_has_attack_order = false;
	ImmortalDropMicroDropDropped2(TossBot* agent, ImmortalDropStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ImmortalDropWaitForShields : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropWaitForShields(TossBot* agent, ImmortalDropStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ImmortalDropLeave : public State
{
public:
	class ImmortalDropStateMachine* state_machine;
	ImmortalDropLeave(TossBot* agent, ImmortalDropStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};


#pragma endregion

#pragma region Door Guard

class DoorOpen : public State
{
public:
	class DoorGuardStateMachine* state_machine;
	DoorOpen(TossBot* agent, DoorGuardStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class DoorClosed : public State
{
public:
	class DoorGuardStateMachine* state_machine;
	DoorClosed(TossBot* agent, DoorGuardStateMachine* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

#pragma endregion

#pragma region AdeptBaseDefenseTerran

class AdeptBaseDefenseTerranClearBase : public State
{
public:
	class AdeptBaseDefenseTerran* state_machine;
	bool checked_dead_space = false;
	AdeptBaseDefenseTerranClearBase(TossBot* agent, AdeptBaseDefenseTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class AdeptBaseDefenseTerranDefendFront : public State
{
public:
	class AdeptBaseDefenseTerran* state_machine;
	bool forward = true;
	AdeptBaseDefenseTerranDefendFront(TossBot* agent, AdeptBaseDefenseTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class AdeptBaseDefenseTerranMoveAcross : public State
{
public:
	class AdeptBaseDefenseTerran* state_machine;
	AdeptBaseDefenseTerranMoveAcross(TossBot* agent, AdeptBaseDefenseTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class AdeptBaseDefenseTerranScoutBase : public State
{
public:
	class AdeptBaseDefenseTerran* state_machine;
	Point2D shade_target;
	Point2D adept_scout_shade;
	Point2D adept_scout_runaway;
	Point2D adept_scout_ramptop;
	bool shields_regening = false;
	std::vector<Point2D> adept_scout_nat_path;
	std::vector<Point2D> adept_scout_base_spots;
	int base_spots_index;
	AdeptBaseDefenseTerranScoutBase(TossBot* agent, AdeptBaseDefenseTerran* state_machine, Point2D adept_scout_shade, Point2D adept_scout_runaway,
		Point2D adept_scout_ramptop, std::vector<Point2D> adept_scout_nat_path, std::vector<Point2D> adept_scout_base_spots)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->adept_scout_shade = adept_scout_shade;
		this->adept_scout_runaway = adept_scout_runaway;
		this->adept_scout_ramptop = adept_scout_ramptop;
		this->adept_scout_nat_path = adept_scout_nat_path;
		this->adept_scout_base_spots = adept_scout_base_spots;
		shade_target = adept_scout_ramptop;
		base_spots_index = 1;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
	void UpdateShadeTarget();
};

#pragma endregion

#pragma region StalkerBaseDefenseTerran

class StalkerBaseDefenseTerranDefendFront : public State
{
public:
	class StalkerBaseDefenseTerran* state_machine;
	bool forward = true;
	StalkerBaseDefenseTerranDefendFront(TossBot* agent, StalkerBaseDefenseTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class StalkerBaseDefenseTerranMoveAcross : public State
{
public:
	class StalkerBaseDefenseTerran* state_machine;
	StalkerBaseDefenseTerranMoveAcross(TossBot* agent, StalkerBaseDefenseTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class ScoutBaseDefenseTerranHarrassFront : public State
{
public:
	class StalkerBaseDefenseTerran* state_machine;
	Point2D attack_pos;
	Point2D retreat_pos;
	bool shields_regening = false;
	ScoutBaseDefenseTerranHarrassFront(TossBot* agent, StalkerBaseDefenseTerran* state_machine, Point2D adept_scout_shade, Point2D adept_scout_runaway)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		this->attack_pos = adept_scout_shade;
		this->retreat_pos = adept_scout_runaway;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};


#pragma endregion


#pragma region BlinkStalkerAttackTerran

class BlinkStalkerAttackTerranMoveAcross : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	BlinkStalkerAttackTerranMoveAcross(TossBot* agent, BlinkStalkerAttackTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class BlinkStalkerAttackTerranWarpIn : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	BlinkStalkerAttackTerranWarpIn(TossBot* agent, BlinkStalkerAttackTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class BlinkStalkerAttackTerranConsolidate : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	BlinkStalkerAttackTerranConsolidate(TossBot* agent, BlinkStalkerAttackTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class BlinkStalkerAttackTerranAttack : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	BlinkStalkerAttackTerranAttack(TossBot* agent, BlinkStalkerAttackTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class BlinkStalkerAttackTerranSnipeUnit : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	BlinkStalkerAttackTerranSnipeUnit(TossBot* agent, BlinkStalkerAttackTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class BlinkStalkerAttackTerranBlinkUp : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	Units stalkers_to_blink;
	BlinkStalkerAttackTerranBlinkUp(TossBot* agent, BlinkStalkerAttackTerran* state_machine, Units stalkers)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		stalkers_to_blink = stalkers;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class BlinkStalkerAttackTerranLeaveHighground : public State
{
public:
	class BlinkStalkerAttackTerran* state_machine;
	BlinkStalkerAttackTerranLeaveHighground(TossBot* agent, BlinkStalkerAttackTerran* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};


#pragma endregion

#pragma region CannonRushTerran

class CannonRushTerranMoveAcross : public State
{
public:
	class CannonRushTerran* state_machine;
	const Unit* probe;
	CannonRushTerranMoveAcross(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe)
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
	CannonRushTerranFindAvaibleCorner(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe, int curr_index)
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
	CannonRushTerranScout(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe, int index, std::vector<Point2D> main_scout_path, bool gas_stolen)
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
	CannonRushTerranFindThreePylonWallOff(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe)
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
	CannonRushTerranGasSteal(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe, int scouting_index, const Unit* gas)
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
	CannonRushTerranMoveAcross2(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe)
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
	CannonRushTerranFindWallOffSpot(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe, int index)
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
	CannonRushTerranWallOff(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe, Point2D cannon_pos, 
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
	CannonRushTerranCannonFirstWallOff(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe, Point2D cannon_pos, 
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
	CannonRushTerranStandBy(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe, Point2D stand_by_spot)
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
	CannonRushTerranStandByLoop(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe, std::vector<Point2D> loop_path)
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
	CannonRushTerranExtraPylon(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe)
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
	CannonRushTerranExtraCannon(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe)
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
	CannonRushTerranBuildGateway(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe)
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
	CannonRushTerranBuildStargate(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe)
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
	CannonRushTerranStandByPhase2(TossBot* agent, CannonRushTerran* state_machine, const Unit* probe, Point2D stand_by_spot)
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
	ArmyGroup army;
	int event_id;
	CannonRushTerranUnitMicro(TossBot* agent, CannonRushTerran* state_machine, const Unit* zealot) : army(agent)
	{
		this->agent = agent;
		this->state_machine = state_machine;
		army.AddUnit(zealot);
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

#pragma endregion

#pragma region AdeptHarassProtoss

class AdeptHarassProtossMoveAcross : public State
{
public:
	class AdeptHarassProtoss* state_machine;
	AdeptHarassProtossMoveAcross(TossBot* agent, AdeptHarassProtoss* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class AdeptHarassProtossConsolidate : public State
{
public:
	class AdeptHarassProtoss* state_machine;
	AdeptHarassProtossConsolidate(TossBot* agent, AdeptHarassProtoss* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class AdeptHarassProtossShadeIntoBase : public State
{
public:
	class AdeptHarassProtoss* state_machine;
	AdeptHarassProtossShadeIntoBase(TossBot* agent, AdeptHarassProtoss* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class AdeptHarassProtossShadeToOtherSide : public State
{
public:
	class AdeptHarassProtoss* state_machine;
	AdeptHarassProtossShadeToOtherSide(TossBot* agent, AdeptHarassProtoss* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};

class AdeptHarassProtossKillProbes : public State
{
public:
	class AdeptHarassProtoss* state_machine;
	AdeptHarassProtossKillProbes(TossBot* agent, AdeptHarassProtoss* state_machine)
	{
		this->agent = agent;
		this->state_machine = state_machine;
	}
	virtual std::string toString() override;
	void TickState() override;
	virtual void EnterState() override;
	virtual void ExitState() override;
	virtual State* TestTransitions() override;
};



#pragma endregion

class StateMachine
{
public:
    TossBot* agent;
    State* current_state;
    std::string name;
    StateMachine() {}
    StateMachine(TossBot* agent, State* starting_state, std::string name) {
        this->agent = agent;
        current_state = starting_state;
        this->name = name;
        current_state->EnterState();
    }
    virtual void RunStateMachine()
    {
        State* new_state = current_state->TestTransitions();
        if (new_state != NULL)
        {
            current_state->ExitState();
            delete current_state;
            current_state = new_state;
            current_state->EnterState();
        }
		if (current_state != NULL)
	        current_state->TickState();
    }

    virtual void StartStateMachine()
    {
        return;
    }

    State* GetState()
    {
        return current_state;
    }

    std::string toString()
    {
        return name + " - " + current_state->toString();
    }

	void CloseStateMachine()
	{
		delete current_state;
		current_state = NULL;
		agent->RemoveStateMachine(this);
	}
};

class OraclePvZStateMachine : public StateMachine
{
    std::vector<const Unit*> oracles;
    float last_scout_time;
};

class OracleHarassStateMachine : public StateMachine
{
public:
	Units oracles;
	std::map<const Unit*, float> time_last_attacked;
	std::map<const Unit*, bool> has_attacked;
	std::map<const Unit*, uint64_t> target;
	std::map<const Unit*, bool> is_beam_active;
	std::map<const Unit*, bool> casting;
	std::map<const Unit*, float> casting_energy;
	bool harass_direction = true;
	int harass_index = 0;
	ArmyGroup* attached_army_group = NULL;
	int event_id;
	OracleHarassStateMachine(TossBot* agent, Units oracles, Point2D denfensive_position, std::string name)
	{
		this->agent = agent;
		this->oracles = oracles;
		this->name = name;
		event_id = agent->GetUniqueId();
		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		current_state = new OracleDefendLocation(agent, this, denfensive_position);
		for (int i = 0; i < oracles.size(); i++)
		{
			time_last_attacked[oracles[i]] = 0;
			has_attacked[oracles[i]] = true;
			is_beam_active[oracles[i]] = false;
			casting[oracles[i]] = false;
			casting_energy[oracles[i]] = 0;
		}
		current_state->EnterState();
	}
	OracleHarassStateMachine(TossBot* agent, Units oracles, std::string name)
	{
		this->agent = agent;
		this->oracles = oracles;
		this->name = name;
		event_id = agent->GetUniqueId();
		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		current_state = new OracleHarassReturnToBase(agent, this, { Point2D(59, 114) });
		for (int i = 0; i < oracles.size(); i++)
		{
			time_last_attacked[oracles[i]] = 0;
			has_attacked[oracles[i]] = true;
			is_beam_active[oracles[i]] = false;
			casting[oracles[i]] = false;
			casting_energy[oracles[i]] = 0;
		}
		current_state->EnterState();
	}
	~OracleHarassStateMachine()
	{
		agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
	}
	void AddOracle(const Unit*);
	void OnUnitDestroyedListener(const Unit*);
};

class ChargelotAllInStateMachine : public StateMachine
{
public:
    Units zealots;
    const Unit* prism;
    float last_warp_in_time;
    std::vector<Point2D> prism_spots;
    int prism_spots_index;
    Point2D next_warp_in_location;
    ChargelotAllInStateMachine(TossBot* agent, std::string name, std::vector<Point2D> prism_locations, Units zealots, const Unit* prism, float last_warp_in_time) {
        this->agent = agent;
        this->name = name;
        current_state = new ChargeAllInMovingToWarpinSpot(agent, this);
        prism_spots = prism_locations;
        this->zealots = zealots;
        this->prism = prism;
        this->last_warp_in_time;
        prism_spots_index = 0;
        current_state->EnterState();
    }
};

class ScoutZergStateMachine : public StateMachine
{
public:
    const Unit* scout;
    Point2D enemy_main;
    int index;
    std::vector<Point2D> main_scout_path;
    std::vector<Point2D> natural_scout_path;
    std::vector<Point2D> possible_3rds;
    Point2D enemy_natural_pos;
    Point2D current_target;
    ScoutZergStateMachine(TossBot* agent, std::string name, const Unit* scout, Point2D enemy_main, std::vector<Point2D> main_scout_path, std::vector<Point2D> natural_scout_path, Point2D enemy_natural_pos, std::vector<Point2D> possible_3rds)
    {
        this->agent = agent;
        this->name = name;
        current_state = new ScoutZInitialMove(agent, this);
        this->scout = scout;
        this->enemy_main = enemy_main;
        this->main_scout_path = main_scout_path;
        this->natural_scout_path = natural_scout_path;
        this->enemy_natural_pos = enemy_natural_pos;
        this->possible_3rds = possible_3rds;

        current_state->EnterState();
    }

    void RunStateMachine()
    {
        StateMachine::RunStateMachine();
        CheckScoutingInfo();
    }


    void CheckScoutingInfo()
    {
        for (const auto &unit : agent->Observation()->GetUnits())
        {
            if (unit->alliance == Unit::Alliance::Enemy && unit->display_type == Unit::DisplayType::Visible)
            {
                if (unit->unit_type.ToType() == UNIT_TYPEID::ZERG_SPAWNINGPOOL && agent->scout_info_zerg.pool_timing == 0)
                {
                    std::cout << "pool built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                    agent->scout_info_zerg.pool_timing = Utility::GetTimeBuilt(unit, agent->Observation());
                }
                else if (unit->unit_type.ToType() == UNIT_TYPEID::ZERG_ROACHWARREN && agent->scout_info_zerg.roach_warren_timing == 0)
                {
                    std::cout << "roach warren built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                    agent->scout_info_zerg.roach_warren_timing = Utility::GetTimeBuilt(unit, agent->Observation());
                }
                else if (unit->unit_type.ToType() == UNIT_TYPEID::ZERG_EXTRACTOR && agent->scout_info_zerg.gas_timing == 0)
                {
                    std::cout << "gas built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                    agent->scout_info_zerg.gas_timing = Utility::GetTimeBuilt(unit, agent->Observation());
                }
                else if (unit->unit_type.ToType() == UNIT_TYPEID::ZERG_HATCHERY && agent->scout_info_zerg.natural_timing == 0)
                {
                    if (Distance2D(unit->pos, enemy_natural_pos) < 2)
                    {
                        std::cout << "natural built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                        agent->scout_info_zerg.natural_timing = Utility::GetTimeBuilt(unit, agent->Observation());
                    }
                }
                else if (unit->unit_type.ToType() == UNIT_TYPEID::ZERG_HATCHERY && agent->scout_info_zerg.third_timing == 0)
                {
                    for (const auto &pos : possible_3rds)
                    {
                        if (Distance2D(unit->pos, pos) < 2)
                        {
                            std::cout << "third built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                            agent->scout_info_zerg.third_timing = Utility::GetTimeBuilt(unit, agent->Observation());
                        }
                    }
                }
            }
        }
    }
};

class ScoutTerranStateMachine : public StateMachine
{
public:
    const Unit* scout;
    Point2D enemy_main;
    int index;
    std::vector<Point2D> main_scout_path;
    std::vector<Point2D> natural_scout_path;
    Point2D enemy_natural_pos;
    Point2D current_target;
    ScoutTerranStateMachine(TossBot* agent, std::string name, const Unit* scout, Point2D enemy_main, std::vector<Point2D> main_scout_path, std::vector<Point2D> natural_scout_path, Point2D enemy_natural_pos)
    {
        this->agent = agent;
        this->name = name;
        current_state = new ScoutTInitialMove(agent, this);
        this->scout = scout;
        this->enemy_main = enemy_main;
        this->main_scout_path = main_scout_path;
        this->natural_scout_path = natural_scout_path;
        this->enemy_natural_pos = enemy_natural_pos;

        current_state->EnterState();
    }

    void RunStateMachine()
    {
        StateMachine::RunStateMachine();
        CheckScoutingInfo();
    }

	void CloseStateMachine()
	{
		delete current_state;
		current_state = NULL;
		agent->worker_manager.PlaceWorker(scout);
		agent->RemoveStateMachine(this);
	}

    void CheckScoutingInfo()
    {
        for (const auto &unit : agent->Observation()->GetUnits())
        {
            if (unit->alliance == Unit::Alliance::Enemy && unit->display_type == Unit::DisplayType::Visible)
            {
                if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_BARRACKS && agent->scout_info_terran.barrackes_timing == 0)
                {
                    std::cout << "barracks built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                    agent->scout_info_terran.barrackes_timing = Utility::GetTimeBuilt(unit, agent->Observation());
                }
                else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_FACTORY && agent->scout_info_terran.factory_timing == 0)
                {
                    std::cout << "factory built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                    agent->scout_info_terran.factory_timing = Utility::GetTimeBuilt(unit, agent->Observation());
                }
                else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_COMMANDCENTER && agent->scout_info_terran.natural_timing == 0 && unit->build_progress < 1)
                {
                    std::cout << "natural built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
                    agent->scout_info_terran.natural_timing = Utility::GetTimeBuilt(unit, agent->Observation());
                }
                else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_REFINERY)
                {
					if (agent->scout_info_terran.gas_timing == 0)
					{
						std::cout << "gas built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
						agent->scout_info_terran.gas_timing = Utility::GetTimeBuilt(unit, agent->Observation());
						agent->scout_info_terran.gas_pos = unit->pos;
					}
					else if (agent->scout_info_terran.second_gas_timing == 0 && unit->pos != agent->scout_info_terran.gas_pos)
					{
						std::cout << "second gas built at " << std::to_string(Utility::GetTimeBuilt(unit, agent->Observation())) << std::endl;
						agent->scout_info_terran.second_gas_timing = Utility::GetTimeBuilt(unit, agent->Observation());
					}
                }
            }
        }
    }
};

class ImmortalDropStateMachine : public StateMachine
{
public:
	const Unit* immortal1;
	const Unit* immortal2;
	const Unit* prism;
	Point2D entry_pos;
	std::vector<Point2D> prism_path;
	int path_index;
	std::vector<UNIT_TYPEID> target_priority = { UNIT_TYPEID::TERRAN_CYCLONE, UNIT_TYPEID::TERRAN_THOR, UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, UNIT_TYPEID::TERRAN_SIEGETANK,
		UNIT_TYPEID::TERRAN_MULE, UNIT_TYPEID::TERRAN_MARAUDER, UNIT_TYPEID::TERRAN_MARINE, UNIT_TYPEID::TERRAN_SCV, UNIT_TYPEID::TERRAN_WIDOWMINE, UNIT_TYPEID::TERRAN_TECHLAB, UNIT_TYPEID::TERRAN_REACTOR };
	ImmortalDropStateMachine(TossBot* agent, std::string name, const Unit* immortal1, const Unit* immortal2, const Unit* prism, Point2D entry_pos, std::vector<Point2D> prism_path) {
		this->agent = agent;
		this->name = name;
		current_state = new ImmortalDropWaitForImmortals(agent, this);
		this->immortal1 = immortal1;
		this->immortal2 = immortal2;
		this->prism = prism;
		this->prism_path = prism_path;
		this->entry_pos = entry_pos;
		path_index = 0;
		current_state->EnterState();
	}

	Point2D UpdatePrismPathing()
	{
		// if theres a tank then go ontop of it
		if (agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_SIEGETANKSIEGED)).size() > 0)
		{
			return agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_SIEGETANKSIEGED))[0]->pos;
		}
		if (agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_SIEGETANK)).size() > 0)
		{
			return agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_SIEGETANK))[0]->pos;
		}
		// if theres a cyclone then go ontop of it?
		if (agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_CYCLONE)).size() > 0)
		{
			return agent->Observation()->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_CYCLONE))[0]->pos;
		}

		int prev_index = path_index - 1 >= 0 ? path_index - 1 : prism_path.size() - 1;
		int next_index = path_index + 1 < prism_path.size() ? path_index + 1 : 0;

		// update path index
		if (Distance2D(prism->pos, prism_path[prev_index]) < 2)
		{
			path_index = prev_index;
			prev_index = path_index - 1 >= 0 ? path_index - 1 : prism_path.size() - 1;
			next_index = path_index + 1 < prism_path.size() ? path_index + 1 : 0;
		}
		else if (Distance2D(prism->pos, prism_path[next_index]) < 2)
		{
			path_index = next_index;
			prev_index = path_index - 1 >= 0 ? path_index - 1 : prism_path.size() - 1;
			next_index = path_index + 1 < prism_path.size() ? path_index + 1 : 0;
		}

		// move away from danger?
		std::cout << Utility::DangerLevelAt(prism, prism_path[prev_index], agent->Observation()) << std::endl;
		if (Utility::DangerLevelAt(prism, prism_path[prev_index], agent->Observation()) > 0 && Utility::DangerLevelAt(prism, prism_path[prev_index], agent->Observation()) < Utility::DangerLevelAt(prism, prism_path[next_index], agent->Observation()))
			return prism_path[prev_index];
		else
			return prism_path[next_index];

	}

};

class DoorGuardStateMachine : public StateMachine
{
public:
	const Unit* guard;
	Point2D door_open_pos;
	Point2D door_closed_pos;
	DoorGuardStateMachine(TossBot* agent, std::string name, const Unit* guard, Point2D door_open_pos, Point2D door_closed_pos) {
		this->agent = agent;
		this->name = name;
		current_state = new DoorClosed(agent, this);
		this->guard = guard;
		this->door_open_pos = door_open_pos;
		this->door_closed_pos = door_closed_pos;
		current_state->EnterState();
	}
};

class AdeptBaseDefenseTerran : public StateMachine
{
public:
	const Unit* adept;
	int frame_shade_used = 0;
	bool attack_status = false;
	const Unit* shade = NULL;
	Point2D dead_space_spot;
	const Unit* target = NULL;
	std::vector<Point2D> front_of_base;
	int event_id;
	AdeptBaseDefenseTerran(TossBot* agent, std::string name, const Unit* adept, Point2D dead_space_spot, std::vector<Point2D> front_of_base) {
		this->agent = agent;
		this->name = name;
		current_state = new AdeptBaseDefenseTerranClearBase(agent, this);
		this->adept = adept;
		this->dead_space_spot = dead_space_spot;
		this->front_of_base = front_of_base;

		event_id = agent->GetUniqueId();
		std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
			this->OnUnitCreatedListener(unit);
		};
		agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);

		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		current_state->EnterState();
	}
	~AdeptBaseDefenseTerran()
	{
		agent->RemoveListenerToOnUnitCreatedEvent(event_id);
		agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
	}
	void OnUnitCreatedListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);
};

class StalkerBaseDefenseTerran : public StateMachine
{
public:
	const Unit* stalker;
	bool attack_status = false;
	const Unit* target = NULL;
	std::vector<Point2D> front_of_base;
	int event_id;
	StalkerBaseDefenseTerran(TossBot* agent, std::string name, const Unit* stalker, std::vector<Point2D> front_of_base) {
		this->agent = agent;
		this->name = name;
		current_state = new StalkerBaseDefenseTerranDefendFront(agent, this);
		this->stalker = stalker;
		this->front_of_base = front_of_base;

		event_id = agent->GetUniqueId();
		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		current_state->EnterState();
	}
	~StalkerBaseDefenseTerran()
	{
		agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
	}
	void OnUnitDestroyedListener(const Unit*);
};

class BlinkStalkerAttackTerran : public StateMachine
{
public:
	ArmyGroup* army_group;
	const Unit* prism;
	bool attacking_main = false;
	Point2D consolidation_pos;
	Point2D prism_consolidation_pos;
	Point2D blink_up_pos;
	Point2D blink_down_pos;
	bool warping_in = false;
	int warp_in_time;


	int event_id;
	BlinkStalkerAttackTerran(TossBot* agent, std::string name, ArmyGroup* army, Point2D consolidation_pos, Point2D prism_consolidation_pos, Point2D blink_up_pos, Point2D blink_down_pos) {
		this->agent = agent;
		this->name = name;
		this->army_group = army;
		this->prism = army->warp_prisms[0];
		this->consolidation_pos = consolidation_pos;
		this->prism_consolidation_pos = prism_consolidation_pos;
		this->blink_up_pos = blink_up_pos;
		this->blink_down_pos = blink_down_pos;
		current_state = new BlinkStalkerAttackTerranMoveAcross(agent, this);

		event_id = agent->GetUniqueId();
		std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
			this->OnUnitCreatedListener(unit);
		};
		agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);

		current_state->EnterState();
	}

	~BlinkStalkerAttackTerran()
	{
		agent->RemoveListenerToOnUnitCreatedEvent(event_id);
	}
	void OnUnitCreatedListener(const Unit*);
};

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
	CannonRushTerran(TossBot* agent, std::string name, const Unit* probe, int variation) {
		this->agent = agent;
		this->name = name;
		this->probe = probe;
		if (variation == 1)
			current_state = new CannonRushTerranMoveAcross(agent, this, probe);
		else if (variation == 2)
			current_state = new CannonRushTerranMoveAcross2(agent, this, probe);
		else
			current_state = new CannonRushTerranUnitMicro(agent, this, probe);


		event_id = agent->GetUniqueId();
		std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
			this->OnUnitCreatedListener(unit);
		};
		agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);

		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		current_state->EnterState();
	}

	~CannonRushTerran()
	{
		agent->RemoveListenerToOnUnitCreatedEvent(event_id);
		agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
	}

	virtual void RunStateMachine() override;
	

	void OnUnitCreatedListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);
	void SmallBuildingBlock(Point2D);
	void BigBuildingBlock(Point2D);

};

class AdeptHarassProtoss : public StateMachine
{
public:
	Units adepts;
	std::vector<Point2D> consolidation_points;
	int frame_shade_used = 0;
	bool attack_status;
	Units shades;
	const Unit* target = NULL;
	int index = 0;
	int event_id;

	AdeptHarassProtoss(TossBot* agent, std::string name, Units adepts, std::vector<Point2D> consolidation_points) {
		this->agent = agent;
		this->name = name;
		this->adepts = adepts;
		this->consolidation_points = consolidation_points;

		attack_status = false;
		
		current_state = new AdeptHarassProtossMoveAcross(agent, this);

		event_id = agent->GetUniqueId();
		std::function<void(const Unit*)> onUnitCreated = [=](const Unit* unit) {
			this->OnUnitCreatedListener(unit);
		};
		agent->AddListenerToOnUnitCreatedEvent(event_id, onUnitCreated);

		std::function<void(const Unit*)> onUnitDestroyed = [=](const Unit* unit) {
			this->OnUnitDestroyedListener(unit);
		};
		agent->AddListenerToOnUnitDestroyedEvent(event_id, onUnitDestroyed);

		current_state->EnterState();
	}

	~AdeptHarassProtoss()
	{
		agent->RemoveListenerToOnUnitCreatedEvent(event_id);
		agent->RemoveListenerToOnUnitDestroyedEvent(event_id);
	}

	void OnUnitCreatedListener(const Unit*);
	void OnUnitDestroyedListener(const Unit*);
};

}

