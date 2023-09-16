#include "PlayerBaseState.h"


Wiwa::PlayerBaseState::PlayerBaseState(PlayerStateMachine* stateMachine, EntityId id)
	: m_StateMachine(stateMachine), m_Id(id)
{
}

Wiwa::PlayerBaseState::~PlayerBaseState()
{
}

void Wiwa::PlayerBaseState::OnCollision(Object*, Object*)
{
}

void Wiwa::PlayerBaseState::OnCollisionExit(Object*, Object*)
{
}

