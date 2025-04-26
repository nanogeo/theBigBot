#pragma once

#include <vector>


namespace sc2
{
class Mediator;


struct TransitionTemplate;


class TransitionManager
{
public:
	Mediator* mediator;
	std::vector<TransitionTemplate> possible_transitions;
	std::vector<TransitionTemplate> active_transitions;

	TransitionManager(Mediator*);

	void CheckTransitions();

	void AddTransitionsForOracleGatewaymanPvZ();
	void AddTransitionsFor4GateBlinkPvT();

	bool WorkerRushTransitionCondition();
	bool WorkerRushTransitionRemoveCondition();
	void WorkerRushTransitionEnterAction();

	bool ScourTransitionCondition();
	bool ScourTransitionRemoveCondition();
	void ScourTransitionEnterAction();
};

struct TransitionTemplate
{
	bool(sc2::TransitionManager::* condition)();
	bool(sc2::TransitionManager::* remove_condition)();
	void(sc2::TransitionManager::* enter_action)();
	TransitionTemplate(bool(sc2::TransitionManager::* condition)(), bool(sc2::TransitionManager::* remove_condition)(), 
		void(sc2::TransitionManager::* enter_action)())
	{
		this->condition = condition;
		this->remove_condition = remove_condition;
		this->enter_action = enter_action;
	}
};


}