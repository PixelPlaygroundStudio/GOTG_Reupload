#include "PlayerStateMachine.h"
#include "PlayerBaseState.h"
#include "PlayerIdle.h"
#include "PlayerAttack.h"
#include "PlayerMove.h"
#include "PlayerDash.h"
#include "PlayerDeath.h"
#include <Wiwa/ecs/components/game/Character.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>

Wiwa::PlayerStateMachine::PlayerStateMachine()
{
	m_IdleState = new PlayerIdle(this, m_EntityId);
	m_MoveState = new PlayerMove(this, m_EntityId);
	m_AttackState = new PlayerAttack(this, m_EntityId);
	m_DashState = new PlayerDash(this, m_EntityId);
	m_DeathState = new PlayerDeath(this, m_EntityId);

}

Wiwa::PlayerStateMachine::~PlayerStateMachine()
{
	delete m_IdleState;
	delete m_MoveState;
	delete m_AttackState;
	delete m_DashState;
	delete m_DeathState;
}

void Wiwa::PlayerStateMachine::OnAwake()
{
	PlayerController::OnAwake();
	m_CurrentState = m_IdleState;
}

void Wiwa::PlayerStateMachine::OnInit()
{
	PlayerController::OnInit();
	m_CurrentState->EnterState();

	if (GameStateManager::s_CurrentCharacter == 1)
	{
		GetTransform()->localScale = glm::vec3(1.75, 1.75, 1.75);
	}
}

void Wiwa::PlayerStateMachine::OnUpdate()
{

	PlayerController::OnUpdate();
	m_CurrentState->UpdateState();
	CheckHealth();
	DashCooldown();
}

void Wiwa::PlayerStateMachine::OnCollisionEnter(Object* body1, Object* body2)
{
	if (body1 == body2)
		return;
	m_Colliding = true;
	PlayerController::OnCollisionEnter(body1, body2);
	m_CurrentState->OnCollisionEnter(body1, body2);
	GetPhysics()->getBody()->velocity = btVector3(0.f, 0.f, 0.f);
}


void Wiwa::PlayerStateMachine::UpdateMovement(const float speed)
{
	m_Direction = Math::AngleFromVec2(m_MovementInput);
	 
	m_CurrentVelocity = m_MovementInput * speed;
	
	GetPhysics()->getBody()->velocity = Math::ToBulletVector3(glm::vec3(m_CurrentVelocity.x, 0.f, m_CurrentVelocity.y));

	SetPlayerRotation(m_Direction);
}

void Wiwa::PlayerStateMachine::UpdateRotation()
{
	if(m_ShootInput != glm::vec2(0.f))
		m_Direction = Math::AngleFromVec2(m_ShootInput);

	SetPlayerRotation(m_Direction);
}

void Wiwa::PlayerStateMachine::SwitchState(PlayerBaseState* state)
{
	//WI_INFO("Changing state");
	m_CurrentState->ExitState();
	m_CurrentState = state;
	m_CurrentState->EnterState();
}

void Wiwa::PlayerStateMachine::CheckHealth()
{
	if (GetCharacter()->Health <= 0 && !Wiwa::GameStateManager::ReturnFanaticEffect() && m_CurrentState != m_DeathState)
	{
		SwitchState(m_DeathState);
		// play audio
		if (GameStateManager::s_CurrentCharacter == STARLORD)
		{
			Audio::PostEvent("vo_starlord_death");
		}
		else if (GameStateManager::s_CurrentCharacter == ROCKET)
		{
			Audio::PostEvent("vo_rocket_death");
		}
	}
}

void Wiwa::PlayerStateMachine::DashCooldown()
{
	if (!IsDashEnable())
	{
		//count cooldown
		m_CooldownTimer -= Time::GetDeltaTimeSeconds();
		if (m_CooldownTimer <= 0)
		{
			SetDashEnable(true);
			//WI_INFO("Dash enabled");
		}
	}
}

void Wiwa::PlayerStateMachine::ResetCooldown()
{
	m_CooldownTimer = GetCharacter()->DashCooldown;
}

void Wiwa::PlayerStateMachine::OnCollision(Object* obj1, Object* obj2)
{
	if (obj1 == obj2)
		return;

	PlayerController::OnCollision(obj1, obj2);
	m_CurrentState->OnCollision(obj1, obj2);
}

void Wiwa::PlayerStateMachine::OnCollisionExit(Object* obj1, Object* obj2)
{
	if (obj1 == obj2)
		return;

	PlayerController::OnCollision(obj1, obj2);
	m_CurrentState->OnCollision(obj1, obj2);
}

