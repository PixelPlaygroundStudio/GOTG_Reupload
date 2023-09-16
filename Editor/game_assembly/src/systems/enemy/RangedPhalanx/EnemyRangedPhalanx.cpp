#include <wipch.h>
#include "StateMachine/RangedPhalanxAttack.h"
#include "StateMachine/RangedPhalanxChasing.h"
#include "StateMachine/RangedPhalanxIdle.h"
#include "StateMachine/RangedPhalanxSpawn.h"
#include "StateMachine/RangedPhalanxDeath.h"
#include "StateMachine/RangedPhalanxHit.h"
#include "EnemyRangedPhalanx.h"
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/components/game/Health.h>


namespace Wiwa
{
	EnemyRangedPhalanx::EnemyRangedPhalanx()
	{
		m_CurrentState = nullptr;
		m_SpawnState = nullptr;
		m_IdleState = nullptr;
		m_ChasingState = nullptr;
		m_AttackingState = nullptr;
		m_DeathState = nullptr;
		m_HitState = nullptr;
		m_GunTransformIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_PhalanxIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		//m_RangeOfAttack = 20.0f;
		m_MinimumPath = 5;
		m_Timer = 0.0f;
	}

	EnemyRangedPhalanx::~EnemyRangedPhalanx()
	{
	}

	void EnemyRangedPhalanx::OnAwake()
	{
		EnemySystem::OnAwake();
		m_IdleState = new RangedPhalanxIdleState();
		m_SpawnState = new RangedPhalanxSpawnState();
		m_ChasingState = new RangedPhalanxChasingState();
		m_AttackingState = new RangedPhalanxAttackState();
		m_DeathState = new RangedPhalanxDeathState();
		m_HitState = new RangedPhalanxHitState();

		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		EntityId gunId = em.GetChildByName(m_EntityId, "gun");
		m_GunTransformIt = GetComponentIterator<Wiwa::Transform3D>(gunId);
		m_PhalanxIt = GetComponentIterator<Wiwa::PhalanxRanged>();
		PhalanxRanged* phalanx = GetComponentByIterator<PhalanxRanged>(m_PhalanxIt);
		EnemyManager& manager = GameStateManager::GetEnemyManager();
		phalanx->bulletLifeTime = manager.m_RangedData.bulletLifeTime;
		phalanx->bulletSpeed = manager.m_RangedData.bulletSpeed;
	}

	void EnemyRangedPhalanx::OnInit()
	{
		if (!getAwake())
			System::Awake();

		EnemySystem::OnInit();

		NavAgent* navAgent = GetComponentByIterator<NavAgent>(m_NavAgentIt);
		if (navAgent) {
			navAgent->autoRotate = true;
		}

		m_CurrentState = m_SpawnState;
		m_CurrentState->EnterState(this);
	}

	void EnemyRangedPhalanx::OnUpdate()
	{
		if (!getAwake() && !getInit())
			return;

		EnemySystem::OnUpdate();
		m_CurrentState->UpdateState(this);
		m_Timer += Time::GetDeltaTimeSeconds();

		Health* stats = GetComponentByIterator<Health>(m_Health);
		if (stats->health <= 0 && m_CurrentState != m_DeathState)
		{
			SwitchState(m_DeathState);
		}
	}

	void EnemyRangedPhalanx::OnDestroy()
	{
		if (m_IdleState != nullptr)
			delete m_IdleState;

		if (m_SpawnState != nullptr)
			delete m_SpawnState;

		if (m_ChasingState != nullptr)
			delete m_ChasingState;

		if (m_AttackingState != nullptr)
			delete m_AttackingState;

		if (m_DeathState != nullptr)
			delete m_DeathState;

		if (m_HitState != nullptr)
			delete m_HitState;

		m_CurrentState = nullptr;
		m_SpawnState = nullptr;
		m_IdleState = nullptr;
		m_ChasingState = nullptr;
		m_AttackingState = nullptr;
		m_DeathState = nullptr;
		m_HitState = nullptr;
	}

	void EnemyRangedPhalanx::OnCollisionEnter(Object* body1, Object* body2)
	{
		EnemySystem::OnCollisionEnter(body1, body2);
		if (m_CurrentState)
			m_CurrentState->OnCollisionEnter(this, body1, body2);
	}

	void EnemyRangedPhalanx::ReceiveDamage(int damage)
	{
		if (m_CurrentState == m_DeathState)
			return;

		WI_INFO("EnemyRangedPhalanx hit by: {} damage", damage);

		EnemySystem::ReceiveDamage(damage);
		SwitchState(m_HitState);
	}

	void EnemyRangedPhalanx::SwitchState(RangedPhalanxBaseState* state)
	{
		m_CurrentState->ExitState(this);
		m_CurrentState = state;
		m_CurrentState->EnterState(this);
	}
	bool EnemyRangedPhalanx::OnEnabledFromPool()
	{
		EnemySystem::OnEnabledFromPool();

		if (m_CurrentState != nullptr && m_SpawnState != nullptr)
		{
			m_CurrentState = m_SpawnState;
			m_CurrentState->EnterState(this);
		}

		return true;
	}
	bool EnemyRangedPhalanx::OnDisabledFromPool()
	{
		EnemySystem::OnDisabledFromPool();
		return false;
	}
}