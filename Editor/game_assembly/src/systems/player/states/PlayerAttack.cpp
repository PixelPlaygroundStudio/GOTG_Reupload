#include "PlayerAttack.h"
#include "../../../components/player/StarLordShooter.h"
#include <Wiwa/ecs/components/game/Character.h>


Wiwa::PlayerAttack::PlayerAttack(PlayerStateMachine* stateMachine, EntityId id)
	: PlayerBaseState(stateMachine, id) {
	m_ShootTimer = 0.f;
	m_audioDelay = 5.0f;
	m_FirstShoot = false;
	m_currentAudioDelay = m_audioDelay;
}

Wiwa::PlayerAttack::~PlayerAttack() {}

void Wiwa::PlayerAttack::EnterState()
{
	WI_WARN("Player attack");
	if(m_StateMachine->GetAnimator()->getAnimator()->getActiveAnimationName() != "aim")
		m_StateMachine->GetAnimator()->getAnimator()->PlayAnimation("aim");
}

void Wiwa::PlayerAttack::UpdateState()
{
	if (!m_StateMachine->GetCharacter()->CanMove)
	{
		m_StateMachine->SwitchState(m_StateMachine->m_IdleState);
		return;
	}
	if (!m_StateMachine->CanAttack() && !m_StateMachine->IsAiming() && !m_StateMachine->CanMove())
	{
		m_StateMachine->SwitchState(m_StateMachine->m_IdleState);
		return;
	}
	if (!m_StateMachine->CanAttack() && !m_StateMachine->IsAiming() && m_StateMachine->CanMove())
	{
		m_StateMachine->SwitchState(m_StateMachine->m_MoveState);
		return;
	}
	if (m_StateMachine->CanDash())
	{
		m_StateMachine->SwitchState(m_StateMachine->m_DashState);
		return;
	}

	m_ShootTimer += Time::GetDeltaTimeSeconds();

	Character* character = m_StateMachine->GetCharacter();
	if (m_StateMachine->CanAttack())
	{
		Fire();
	}
	else if (m_StateMachine->CanUltimate() && character->Shield >= character->MaxShield)
	{
		if(GameStateManager::s_CurrentCharacter == 0)
			FireStarlordUltimate();
		else if (GameStateManager::s_CurrentCharacter == 1)
			FireRocketUltimate();
	}

	if (m_StateMachine->CanMove())
	{
		m_StateMachine->UpdateMovement(m_StateMachine->GetCharacter()->Speed);
		m_StateMachine->UpdateRotation();
		float shootDirection = m_StateMachine->GetTransform()->localRotation.y;
		float movementDirection = Math::AngleFromVec2(m_StateMachine->GetInput());
		float difference = glm::abs(shootDirection - movementDirection);

		//WI_INFO("Diff angle {}", difference);
		if (IN_BETWEEN(difference, 0.f, 45.f))
		{
			m_StateMachine->GetAnimator()->PlayAnimation("walk_aim");
		}
		else if (IN_BETWEEN(difference, 170.f, 289.f))
		{	
			m_StateMachine->GetAnimator()->PlayAnimation("walk_aim_back");
		}
		else if (IN_BETWEEN(difference, 45.0f, 169.f))
		{
			m_StateMachine->GetAnimator()->PlayAnimation("walk_aim_left");

		}
		else if (IN_BETWEEN(difference, 290.0f, 360.0f))
		{
			m_StateMachine->GetAnimator()->PlayAnimation("walk_aim_right");

		}

		//audio
		currentSteptime -= Time::GetDeltaTimeSeconds();
		if (currentSteptime < 0)
		{
			m_StateMachine->GetAudio()->PlayAudio("player_walk");
			currentSteptime = runStepTimer;
		}

	}
	else
	{
		if (m_StateMachine->GetAnimator()->getAnimator()->getActiveAnimationName() != "aim")
			m_StateMachine->GetAnimator()->PlayAnimation("aim");
		m_StateMachine->UpdateMovement(0.f);
		m_StateMachine->UpdateRotation();
	}

	m_currentAudioDelay -= Time::GetDeltaTimeSeconds();
}

void Wiwa::PlayerAttack::ExitState()
{
}

void Wiwa::PlayerAttack::OnCollisionEnter(Object* object1, Object* object2)
{
}

void Wiwa::PlayerAttack::Fire()
{

	float timeBetweenShots = 1.0f / m_StateMachine->GetCharacter()->RateOfFire;
	if (GameStateManager::s_CurrentCharacter == 0)
	{
		Transform3D* spawnPoint;
		StarLordShooter* shooter = m_StateMachine->GetStarLord();
		//Decide wich hand is going next

		if (m_ShootTimer >= timeBetweenShots)
		{
			m_ShootTimer = 0;
			if (shooter->ShootRight)
			{
				spawnPoint = m_StateMachine->GetFirePosition("RightPos");
				//m_StateMachine->GetAnimator()->Blend("shoot_right", false, 0.1f);
			}
			else
			{
				spawnPoint = m_StateMachine->GetFirePosition("LeftPos"); ;
				//m_StateMachine->GetAnimator()->Blend("shoot_left", false, 0.1f);
			}
  			shooter->ShootRight = !shooter->ShootRight;
			m_StateMachine->SpawnStarLordBullet(*spawnPoint, *m_StateMachine->GetCharacter());
			m_StateMachine->GetAudio()->PlayAudio("player_shoot");
			

		
			if ((int)GameStateManager::GetType() ==  2 || (int) GameStateManager::GetRoomType() == 4)
			{
				if (!m_FirstShoot)
				{
					m_StateMachine->GetAudio()->PlayAudio("combat_start");
					m_FirstShoot = true;
					
				}
			}
			else {
				if (m_currentAudioDelay < 0 && !m_FirstShoot)
				{
					//m_StateMachine->GetAudio()->PlayAudio("combat_start");
					m_FirstShoot = false;
					m_currentAudioDelay = m_audioDelay;
				}
			}
		}
	}
	else
	{
		Transform3D* spawnPoint = m_StateMachine->GetFirePosition("FirePos");
		RocketShooter* shooter = m_StateMachine->GetRocket();

		//TODO: blend with current animation
		if (m_ShootTimer >= timeBetweenShots)
		{
			m_ShootTimer = 0;
			m_StateMachine->SpawnRocketBullet(*spawnPoint, *m_StateMachine->GetCharacter());
		}
	}
}

void Wiwa::PlayerAttack::FireStarlordUltimate()
{
	Transform3D* spawnPoint;
	Character* character = m_StateMachine->GetCharacter();
	character->Shield = 0;
	character->ShieldRegeneration = 2;
	character->ShieldRegenerationMult = 1;
	spawnPoint = m_StateMachine->GetFirePosition("RightPos");
	m_StateMachine->GetAnimator()->PlayAnimation("ultimate");
	
	m_StateMachine->SpawnStarLordUltimate(*spawnPoint, *m_StateMachine->GetCharacter());
	m_StateMachine->GetAudio()->PlayAudio("player_shoot");
}

void Wiwa::PlayerAttack::FireRocketUltimate()
{
	Transform3D* spawnPoint;
	Character* character = m_StateMachine->GetCharacter();
	character->Shield = 0;
	character->ShieldRegeneration = 2;
	character->ShieldRegenerationMult = 1;
	spawnPoint = m_StateMachine->GetFirePosition("FirePos");
	m_StateMachine->GetAnimator()->PlayAnimation("ultimate");

	m_StateMachine->SpawnStarLordUltimate(*spawnPoint, *m_StateMachine->GetCharacter());
	m_StateMachine->GetAudio()->PlayAudio("player_shoot");
}
