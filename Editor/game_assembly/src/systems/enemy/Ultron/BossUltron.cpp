#include <wipch.h>
#include "StateMachine/BossUltronSpawn.h"
#include "StateMachine/BossUltronIdle.h"
#include "StateMachine/BossUltronMovement.h"
#include "StateMachine/BossUltronClusterShotsAttack.h"
#include "StateMachine/BossUltronLaserBeamAttack.h"
#include "StateMachine/BossUltronBulletStormAttack.h"
#include "StateMachine/BossUltronDeath.h"
#include "StateMachine/BossUltronDash.h"
#include "StateMachine/BossUltronSecondDash.h"
#include "StateMachine/BossUltronProjectileRainAttack.h"
#include "StateMachine/TransitionSecondPhase.h"
#include "BossUltron.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

#include <Wiwa/ecs/systems/AudioSystem.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/ecs/components/game/Health.h>

namespace Wiwa
{
	BossUltron::BossUltron()
	{
		m_CurrentState = nullptr;
		m_SpawnState = nullptr;
		m_IdleState = nullptr;
		m_DeathState = nullptr;
		m_MovementState = nullptr;
		m_ClusterShotsAttackState = nullptr;
		m_LaserBeamAttackState = nullptr;
		m_BulletStormAttackState = nullptr;
		m_DashState = nullptr;
		m_SecondDashState = nullptr;
		m_ProjectileRain = nullptr;
		m_TransitionSecondPhaseState = nullptr;
		
		m_GunTransformIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Ultron = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
		m_MaxHealth = 0;
		m_SceneCenterPos = glm::vec3(0.0f);

		m_SecondPhasePath = "assets/vfx/prefabs/vfx_finals/boss_Ultron/ultron_second_phase.wiprefab";
	}

	BossUltron::~BossUltron()
	{
	}

	void BossUltron::OnAwake()
	{
		EnemySystem::OnAwake();

		m_SpawnState = new BossUltronSpawnState();
		m_IdleState = new BossUltronIdleState();
		m_MovementState = new BossUltronMovementState();
		m_ClusterShotsAttackState = new BossUltronClusterShotsAttackState();
		m_LaserBeamAttackState = new BossUltronLaserBeamAttackState();
		m_BulletStormAttackState = new BossUltronBulletStormAttackState();
		m_DeathState = new BossUltronDeathState();
		m_DashState = new BossUltronDashState();
		m_SecondDashState = new BossUltronSecondDashState();
		m_ProjectileRain = new BossUltronProjectileRainAttackState();
		m_TransitionSecondPhaseState = new BossUltronSecondPhaseState();
		
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();

		EntityId gunId = em.GetChildByName(m_EntityId, "gun");
		m_GunTransformIt = GetComponentIterator<Wiwa::Transform3D>(gunId);

		m_Ultron = GetComponentIterator<Wiwa::Ultron>();
		Ultron* ultron = GetComponentByIterator<Ultron>(m_Ultron);
		EnemyManager& manager = GameStateManager::GetEnemyManager();
		*ultron = manager.m_UltronData;
	}

	void BossUltron::OnInit()
	{
		if (!getAwake())
			System::Awake();

		EnemySystem::OnInit();

		NavAgent* navAgent = GetComponentByIterator<NavAgent>(m_NavAgentIt);
		if (navAgent) {
			navAgent->autoRotate = true;
		}

		Health* stats = GetComponentByIterator<Health>(m_Health);
		m_MaxHealth = stats->health;

		m_CurrentState = m_SpawnState;
		m_CurrentState->EnterState(this);
	}

	void BossUltron::OnUpdate()
	{
		/*if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();*/
		if (!getAwake() && !getInit())
			return;
		EnemySystem::OnUpdate();
		m_CurrentState->UpdateState(this);
		m_Timer += Time::GetDeltaTimeSeconds();

		Wiwa::EntityManager& em = this->getScene().GetEntityManager();
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(this->m_TransformIt);
		Health* stats = GetComponentByIterator<Health>(m_Health);

		if (stats->health <= (m_MaxHealth * 0.5f) && m_IsSecondPhaseActive == false)
		{
			m_IsSecondPhaseActive = true;

			//Create Second Phase Particle
			m_SecondPhaseId = em.LoadPrefab(m_SecondPhasePath);

			SwitchState(m_TransitionSecondPhaseState);
		}

		if (m_IsSecondPhaseActive && stats->health > 0)
		{
			Transform3D* secondPhaseTr = em.GetComponent<Transform3D>(m_SecondPhaseId);
			secondPhaseTr->localPosition = selfTr->localPosition;
		}

		if (stats->health <= 0 && m_CurrentState != m_DeathState)
		{
			em.DestroyEntity(m_SecondPhaseId);

			SwitchState(m_DeathState);
		}

	}

	void BossUltron::OnDestroy()
	{
		if (m_IdleState != nullptr)
			delete m_IdleState;

		if (m_SpawnState != nullptr)
			delete m_SpawnState;

		if (m_DeathState != nullptr)
			delete m_DeathState;
		
		if (m_MovementState != nullptr)
			delete m_MovementState;

		if (m_ClusterShotsAttackState != nullptr)
			delete m_ClusterShotsAttackState;

		if (m_LaserBeamAttackState != nullptr)
			delete m_LaserBeamAttackState;

		if (m_BulletStormAttackState != nullptr)
			delete m_BulletStormAttackState;

		if (m_DashState != nullptr)
			delete m_DashState;

		if (m_SecondDashState != nullptr)
			delete m_SecondDashState;

		if (m_ProjectileRain != nullptr)
			delete m_ProjectileRain;

		if (m_TransitionSecondPhaseState != nullptr)
			delete m_TransitionSecondPhaseState;

		m_CurrentState = nullptr;
		m_SpawnState = nullptr;
		m_IdleState = nullptr;
		m_DeathState = nullptr;
		m_MovementState = nullptr;
		m_ClusterShotsAttackState = nullptr;
		m_LaserBeamAttackState = nullptr;
		m_BulletStormAttackState = nullptr;
		m_DashState = nullptr;
		m_SecondDashState = nullptr;
		m_ProjectileRain = nullptr;
		m_TransitionSecondPhaseState = nullptr;
	}

	void BossUltron::OnCollisionEnter(Object* body1, Object* body2)
	{
		EnemySystem::OnCollisionEnter(body1, body2);
		if (m_CurrentState)
			m_CurrentState->OnCollisionEnter(this, body1, body2);
	}

	void BossUltron::ReceiveDamage(int damage)
	{
		if (m_CurrentState == m_DeathState)
			return;

		WI_INFO("BossUltron hit by: {} damage", damage);

		EnemySystem::ReceiveDamage(damage);

		Wiwa::EntityManager& em = this->getScene().GetEntityManager();

		std::uniform_int_distribution<> audioHitSound(1, 100);
		int randomNum = audioHitSound(Application::s_Gen);
		if (randomNum <= 20) // 20 % probability
		{
			m_AudioSys->PlayAudio("vo_boss_hit");
			m_AnimatorSys->PlayAnimation("hit");
		}

		
	}

	void BossUltron::SwitchState(BossUltronBaseState* state)
	{
		m_CurrentState->ExitState(this);
		m_CurrentState = state;
		m_CurrentState->EnterState(this);
	}

	bool BossUltron::OnEnabledFromPool()
	{
		EnemySystem::OnEnabledFromPool();

		if (m_CurrentState != nullptr && m_SpawnState != nullptr)
		{
			m_CurrentState = m_SpawnState;
			m_CurrentState->EnterState(this);
		}

		return true;
	}

	bool BossUltron::OnDisabledFromPool()
	{
		EnemySystem::OnDisabledFromPool();
		return false;
	}
}