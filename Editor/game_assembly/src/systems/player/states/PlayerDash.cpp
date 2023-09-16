#include "PlayerDash.h"
#include <Wiwa/ecs/components/game/Character.h>
#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/ecs/systems/ParticleSystem.h>
#include "../../../Systems.h"

Wiwa::PlayerDash::PlayerDash(PlayerStateMachine* stateMachine, EntityId id)
	: PlayerBaseState(stateMachine, id)
{
	m_DashTimer = 0.f;
	m_TargetPoint = glm::vec3(0.f);
	m_DashDirection = glm::vec3(0.f);
	m_MaxDashTime = 1.5f;
	
	
}

Wiwa::PlayerDash::~PlayerDash()
{
}

void Wiwa::PlayerDash::EnterState()
{
	if (!m_StateMachine->GetCharacter()->CanMove)
	{
		m_StateMachine->SwitchState(m_StateMachine->m_IdleState);
		return;
	}

	m_DashVFX = m_StateMachine->GetEntityManager().GetChildByName(m_StateMachine->GetEntity(), "p_dash");
	m_StateMachine->GetEntityManager().SetActive(m_DashVFX, true);
	m_StateMachine->GetAudio()->PlayAudio("player_dash");

	EntityManager& entityManager = GameStateManager::GetCurrentScene()->GetEntityManager();
	ParticleSystem* sys_dashParticle = entityManager.GetSystem<ParticleSystem>(m_DashVFX);

	if (sys_dashParticle != nullptr)
	{
		sys_dashParticle->SetActive(true);
	}
	
	m_StateMachine->IsDashing = true;
	m_DashTimer = m_StateMachine->GetCharacter()->DashCooldown;
	if (m_StateMachine->CanMove())
	{
		float direction = Math::AngleFromVec2(m_StateMachine->GetInput());
		m_DashDirection = Math::Vec3FromAngle(direction);
	}
	else 
	{
		m_DashDirection = Math::CalculateForward(m_StateMachine->GetTransform());
	}

	m_TargetPoint = Math::PointAlongDirection(m_StateMachine->GetTransform()->localPosition, m_DashDirection, m_StateMachine->GetCharacter()->DashDistance);

	m_StateMachine->GetAnimator()->PlayAnimation("dash");
	m_MaxDashTime = 0.3f;
	m_DashTimer = 0.f;


	//m_StateMachine->SetPlayerRotation(m_StateMachine->GetDirection());
}

void Wiwa::PlayerDash::UpdateState()
{
	float distance = Math::Distance(m_TargetPoint, m_StateMachine->GetTransform()->position);

	if (distance <= 0.5f)
	{
		m_StateMachine->SwitchState(m_StateMachine->m_IdleState);
		return;
	}
	m_DashTimer += Time::GetDeltaTimeSeconds();

	if (m_DashTimer >= m_MaxDashTime)
	{
		m_StateMachine->SwitchState(m_StateMachine->m_IdleState);
		return;
	}

	glm::vec3 velocity = m_DashDirection * m_StateMachine->GetCharacter()->Speed * m_StateMachine->GetCharacter()->DashSpeed;
	m_StateMachine->GetPhysics()->getBody()->velocity = Math::ToBulletVector3(velocity);
	// TODO: Particles and audio queue
}

void Wiwa::PlayerDash::ExitState()
{
	m_DashTimer = 0.f;
	m_DashDirection = glm::vec3(0.f);
	m_StateMachine->SetDashEnable(false);
	m_StateMachine->ResetCooldown();

	m_StateMachine->IsDashing = false;
	

	EntityManager& entityManager = GameStateManager::GetCurrentScene()->GetEntityManager();
	ParticleSystem* sys_dashParticle = entityManager.GetSystem<ParticleSystem>(m_DashVFX);

	if (sys_dashParticle != nullptr)
	{
		sys_dashParticle->SetActive(false);
	}
}

void Wiwa::PlayerDash::OnCollisionEnter(Object* object1, Object* object2)
{
	m_StateMachine->GetPhysics()->getBody()->velocity = btVector3(0.f, 0.f, 0.f);
	m_StateMachine->SwitchState(m_StateMachine->m_IdleState);
}

void Wiwa::PlayerDash::OnCollision(Object* object1, Object* object2)
{
	m_StateMachine->GetPhysics()->getBody()->velocity = btVector3(0.f, 0.f, 0.f);
	m_StateMachine->SwitchState(m_StateMachine->m_IdleState);
}
