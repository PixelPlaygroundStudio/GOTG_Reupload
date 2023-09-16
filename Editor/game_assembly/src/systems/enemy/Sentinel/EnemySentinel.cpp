#include <wipch.h>
#include "StateMachine/SentinelAttack.h"
#include "StateMachine/SentinelChasing.h"
#include "StateMachine/SentinelIdle.h"
#include "StateMachine/SentinelSpawn.h"
#include "StateMachine/SentinelDeath.h"
#include "StateMachine/SentinelHit.h"
#include "EnemySentinel.h"
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/components/game/Health.h>


namespace Wiwa
{
	EnemySentinel::EnemySentinel()
	{
		m_CurrentState = nullptr;
		m_SpawnState = nullptr;
		m_IdleState = nullptr;
		m_ChasingState = nullptr;
		m_AttackingState = nullptr;
		m_DeathState = nullptr;
		m_HitState = nullptr;
	}

	EnemySentinel::~EnemySentinel()
	{
	}

	void EnemySentinel::OnAwake()
	{
		EnemySystem::OnAwake();
		m_IdleState = new SentinelIdleState();
		m_SpawnState = new SentinelSpawnState();
		m_ChasingState = new SentinelChasingState();
		m_AttackingState = new SentinelAttackState();
		m_DeathState = new SentinelDeathState();
		m_HitState = new SentinelHitState();
	}

	void EnemySentinel::OnInit()
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

	void EnemySentinel::OnUpdate()
	{
		if (!getAwake() && !getInit())
			return;
		EnemySystem::OnUpdate();
		m_CurrentState->UpdateState(this);
		m_TimerSentinel += Time::GetDeltaTimeSeconds();

		Health* stats = GetComponentByIterator<Health>(m_Health);
		if (stats->health <= 0 && m_CurrentState != m_DeathState)
		{
			SwitchState(m_DeathState);
		}
	}

	void EnemySentinel::OnDestroy()
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
		m_HitState = nullptr;
	}

	void EnemySentinel::OnCollisionEnter(Object* body1, Object* body2)
	{
		EnemySystem::OnCollisionEnter(body1, body2);

		/*std::string playerStr = "PLAYER";
		if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
		{
			SwitchState(m_DeathState);
		}*/

		if (m_CurrentState)
			m_CurrentState->OnCollisionEnter(this, body1, body2);
	}

	void EnemySentinel::ReceiveDamage(int damage)
	{
		if (m_CurrentState == m_DeathState)
			return;

		WI_INFO("EnemyRangedPhalanx hit by: {} damage", damage);

		EnemySystem::ReceiveDamage(damage);
		SwitchState(m_HitState);
		/*Wiwa::EntityManager& em = m_Scene->GetEntityManager();

		EntityId hit_1 = em.GetChildByName(m_EntityId, "E_Hit_1");
		EntityId hit_2 = em.GetChildByName(m_EntityId, "E_Hit_2");
		ParticleManager& pman = this->getScene().GetParticleManager();
		pman.EmitBatch(hit_1);
		pman.EmitBatch(hit_2);


		enemy->m_AnimatorSys->PlayAnimation("hit", false);*/
		//PlaySound(ScriptEngine::CreateString("melee_hit"), m_PlayerId);
	}

	void EnemySentinel::SwitchState(SentinelBaseState* state)
	{
		m_CurrentState->ExitState(this);
		m_CurrentState = state;
		m_CurrentState->EnterState(this);
	}

	bool EnemySentinel::OnEnabledFromPool()
	{
		EnemySystem::OnEnabledFromPool();

		if (m_CurrentState != nullptr && m_SpawnState != nullptr)
		{
			m_CurrentState = m_SpawnState;
			m_CurrentState->EnterState(this);
		}

		return false;
	}

	bool EnemySentinel::OnDisabledFromPool()
	{
		EnemySystem::OnDisabledFromPool();
		return false;
	}
}