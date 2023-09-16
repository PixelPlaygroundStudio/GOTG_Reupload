#include <wipch.h>
#include "StateMachine/SubjugatorAttack.h"
#include "StateMachine/SubjugatorChasing.h"
#include "StateMachine/SubjugatorIdle.h"
#include "StateMachine/SubjugatorSpawn.h"
#include "StateMachine/SubjugatorDeath.h"
#include "StateMachine/SubjugatorHit.h"
#include "EnemySubjugator.h"
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/components/game/Health.h>


namespace Wiwa
{
	EnemySubjugator::EnemySubjugator()
	{
		m_CurrentState = nullptr;
		m_SpawnState = nullptr;
		m_IdleState = nullptr;
		m_ChasingState = nullptr;
		m_AttackingState = nullptr;
		m_DeathState = nullptr;
		m_HitState = nullptr;
		m_Hand1It = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Hand2It = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Hand3It = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Hand4It = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Subjugator = { WI_INVALID_INDEX, WI_INVALID_INDEX };

		//m_RangeOfAttack = 40.0f; //Add to components
		m_MinimumPath = 5;
		m_Timer = 0.0f;
	}

	EnemySubjugator::~EnemySubjugator()
	{
	}

	void EnemySubjugator::OnAwake()
	{
		EnemySystem::OnAwake();
		m_IdleState = new SubjugatorIdleState();
		m_SpawnState = new SubjugatorSpawnState();
		m_ChasingState = new SubjugatorChasingState();
		m_AttackingState = new SubjugatorAttackState();
		m_DeathState = new SubjugatorDeathState();
		m_HitState = new SubjugatorHitState();

		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		EntityId hand1Id = em.GetChildByName(m_EntityId, "Hand_1");
		m_Hand1It = GetComponentIterator<Wiwa::Transform3D>(hand1Id);

		EntityId hand2Id = em.GetChildByName(m_EntityId, "Hand_2");
		m_Hand2It = GetComponentIterator<Wiwa::Transform3D>(hand2Id);

		EntityId hand3Id = em.GetChildByName(m_EntityId, "Hand_3");
		m_Hand3It = GetComponentIterator<Wiwa::Transform3D>(hand3Id);

		EntityId hand4Id = em.GetChildByName(m_EntityId, "Hand_4");
		m_Hand4It = GetComponentIterator<Wiwa::Transform3D>(hand4Id);

		m_Subjugator = GetComponentIterator<Wiwa::Subjugator>();
		Subjugator* subjugator = GetComponentByIterator<Subjugator>(m_Subjugator);
		EnemyManager& manager = GameStateManager::GetEnemyManager();
		subjugator->bulletLifeTime = manager.m_RangedData.bulletLifeTime;;
		subjugator->bulletSpeed = manager.m_RangedData.bulletSpeed;;
	}

	void EnemySubjugator::OnInit()
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

	void EnemySubjugator::OnUpdate()
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

	void EnemySubjugator::OnDestroy()
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

	void EnemySubjugator::OnCollisionEnter(Object* body1, Object* body2)
	{
		EnemySystem::OnCollisionEnter(body1, body2);
		if (m_CurrentState)
			m_CurrentState->OnCollisionEnter(this, body1, body2);
	}

	void EnemySubjugator::ReceiveDamage(int damage)
	{
		if (m_CurrentState == m_DeathState)
			return;

		WI_INFO("EnemySubjugator hit by: {} damage", damage);

		EnemySystem::ReceiveDamage(damage);
		SwitchState(m_HitState);
		/*Wiwa::EntityManager& em = m_Scene->GetEntityManager();

		EntityId hitR_1 = em.GetChildByName(m_EntityId, "ER_Hit_1");
		EntityId hitR_2 = em.GetChildByName(m_EntityId, "ER_Hit_2");
		ParticleManager& pman = this->getScene().GetParticleManager();
		pman.EmitBatch(hitR_1);
		pman.EmitBatch(hitR_2);

		enemy->m_AnimatorSys->PlayAnimation("damage", false);*/
	}

	void EnemySubjugator::SwitchState(SubjugatorBaseState* state)
	{
		m_CurrentState->ExitState(this);
		m_CurrentState = state;
		m_CurrentState->EnterState(this);
	}
	bool EnemySubjugator::OnEnabledFromPool()
	{
		EnemySystem::OnEnabledFromPool();

		if (m_CurrentState != nullptr && m_SpawnState != nullptr)
		{
			m_CurrentState = m_SpawnState;
			m_CurrentState->EnterState(this);
		}

		return true;
	}
	bool EnemySubjugator::OnDisabledFromPool()
	{
		EnemySystem::OnDisabledFromPool();
		return false;
	}
}