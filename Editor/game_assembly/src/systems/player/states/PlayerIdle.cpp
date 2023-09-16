#include "PlayerIdle.h"
#include "PlayerMove.h"
#include "PlayerAttack.h"
#include "PlayerDash.h"

Wiwa::PlayerIdle::PlayerIdle(PlayerStateMachine* stateMachine, EntityId id)
	: PlayerBaseState(stateMachine, id)
{
}

Wiwa::PlayerIdle::~PlayerIdle()
{
}

void Wiwa::PlayerIdle::EnterState()
{
	//WI_INFO("Player idle");

	m_StateMachine->GetAnimator()->PlayAnimation("idle");
	m_StateMachine->GetPhysics()->getBody()->velocity = btVector3(0.f, 0.f, 0.f);
}

void Wiwa::PlayerIdle::UpdateState()
{
	if (m_StateMachine->CanMove())
	{
		m_StateMachine->SwitchState(m_StateMachine->m_MoveState);
		return;
	}
	if (m_StateMachine->IsAiming() || m_StateMachine->CanAttack())
	{
		m_StateMachine->SwitchState(m_StateMachine->m_AttackState);
		return;
	}
	if (m_StateMachine->CanDash())
	{
		m_StateMachine->SwitchState(m_StateMachine->m_DashState);
		return;
	}
}

void Wiwa::PlayerIdle::ExitState()
{
}

void Wiwa::PlayerIdle::OnCollisionEnter(Object* object1, Object* object2)
{
}
