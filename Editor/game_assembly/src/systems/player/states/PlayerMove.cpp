#include "PlayerMove.h"
#include <Wiwa/ecs/components/game/Character.h>

Wiwa::PlayerMove::PlayerMove(PlayerStateMachine* stateMachine, EntityId id)
	: PlayerBaseState(stateMachine, id)
{
}

Wiwa::PlayerMove::~PlayerMove()
{
}

void Wiwa::PlayerMove::EnterState()
{
	/*WI_INFO("Player move");
	WI_INFO("{}", m_StateMachine->GetInput().length());*/
	if(m_StateMachine->GetCharacter()->WalkTreshold <= m_StateMachine->GetInput().length())
		m_StateMachine->GetAnimator()->PlayAnimation("running");
	else
		m_StateMachine->GetAnimator()->PlayAnimation("running");
	currentSteptime = runStepTimer;
}

void Wiwa::PlayerMove::UpdateState()
{
	if (!m_StateMachine->GetCharacter()->CanMove)
	{
		m_StateMachine->SwitchState(m_StateMachine->m_IdleState);
		return;
	}
	if (!m_StateMachine->CanMove())
	{
		m_StateMachine->SwitchState(m_StateMachine->m_IdleState);
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

	currentSteptime -= Time::GetDeltaTimeSeconds();
	if (currentSteptime < 0)
	{
		m_StateMachine->GetAudio()->PlayAudio("player_walk");
		currentSteptime = runStepTimer;
	}


	m_StateMachine->UpdateMovement(m_StateMachine->GetCharacter()->Speed);
}

void Wiwa::PlayerMove::ExitState()
{
}

void Wiwa::PlayerMove::OnCollisionEnter(Object* object1, Object* object2)
{
}
