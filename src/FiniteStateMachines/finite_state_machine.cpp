
#include "finite_state_machine.h"



namespace sc2 {

void State::TickState()
{
	//std::cout << "TickState called on base State class";
}

void State::EnterState()
{
	//std::cout << "EnterState called on base State class";
}

void State::ExitState()
{
	//std::cout << "ExitState called on base State class";
}

State* State::TestTransitions()
{
	//std::cout << "TestTransitions called on base State class";
	return nullptr;
}

std::string State::toString() const
{
	return "State";
}

void StateMachine::SetAttachedArmyGroup(ArmyGroup* army_group)
{
	attached_army_group = army_group;
}

ArmyGroup* StateMachine::GetAttachedArmyGroup() const
{
	return attached_army_group;
}


}