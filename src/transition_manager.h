#pragma once

#include <vector>


namespace sc2
{
class Mediator;


struct TransitionTemplate;


class TransitionManager
{
private:
	Mediator* mediator;
	std::vector<TransitionTemplate> possible_transitions;
	std::vector<TransitionTemplate> active_transitions;

public:
	TransitionManager(Mediator*);

	void CheckTransitions();

	void AddTransitionsForOracleGatewaymanPvZ();
	void AddTransitionsFor4GateBlinkPvT();
	// TODO can these be private
	bool WorkerRushTransitionCondition() const;
	bool WorkerRushTransitionRemoveCondition() const;
	void WorkerRushTransitionEnterAction();

	bool ScourTransitionCondition() const;
	bool ScourTransitionRemoveCondition() const;
	void ScourTransitionEnterAction();

	bool FixEarlySupplyBlockCondition() const;
	bool FixEarlySupplyBlockRemoveCondition() const;
	void FixEarlySupplyBlockEnterAction();

};

struct TransitionTemplate
{
	bool(sc2::TransitionManager::* condition)() const;
	bool(sc2::TransitionManager::* remove_condition)() const;
	void(sc2::TransitionManager::* enter_action)();
	TransitionTemplate(bool(sc2::TransitionManager::* condition)() const, bool(sc2::TransitionManager::* remove_condition)() const,
		void(sc2::TransitionManager::* enter_action)())
	{
		this->condition = condition;
		this->remove_condition = remove_condition;
		this->enter_action = enter_action;
	}
};


}