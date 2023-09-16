#include <wipch.h>
#include "RangedPhalanxDeath.h"
#include "../EnemyRangedPhalanx.h"
#include <Wiwa/ecs/systems/game/wave/WaveSystem.h>
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/components/game/enemy/Enemy.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include <Wiwa/ecs/systems/PhysicsSystem.h>

namespace Wiwa
{
	RangedPhalanxDeathState::RangedPhalanxDeathState()
	{
	}

	RangedPhalanxDeathState::~RangedPhalanxDeathState()
	{

	}

	void RangedPhalanxDeathState::EnterState(EnemyRangedPhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		enemy->m_AudioSys->PlayAudio("ranged_dead");
		
		enemy->m_AnimatorSys->PlayAnimation("death");
		m_TimerToDie = 0.0f;

		if (enemy->m_NavAgentSys)
			enemy->m_NavAgentSys->StopAgent();

		PhysicsSystem* physSys = em.GetSystem<PhysicsSystem>(enemy->GetEntity());
		physSys->DeleteBody();
	}

	void RangedPhalanxDeathState::UpdateState(EnemyRangedPhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);

		enemy->m_NavAgentSys->StopAgent();
		navAgent->autoRotate = false;

		if (enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->HasFinished() && m_TimerToDie > m_TimeToDie)
		{
			EnemyState* self = (EnemyState*)em.GetComponentByIterator(enemy->m_EnemyStateIt);
			self->hasFinished = true;
			if (self->waveId != -1)
			{
				PhalanxRanged* Pr = em.GetComponent<Wiwa::PhalanxRanged>(enemy->GetEntity());
				if (Pr->variant == 1)
				{
					GameStateManager::s_PoolManager->s_PhalanxRangedVariantAPool->ReturnToPool(enemy->GetEntity());
				}
				else if (Pr->variant == 2)
				{
					GameStateManager::s_PoolManager->s_PhalanxRangedVariantBPool->ReturnToPool(enemy->GetEntity());
				}
				else
				{
					GameStateManager::s_PoolManager->s_PhalanxRangedGenericPool->ReturnToPool(enemy->GetEntity());
				}
				Wiwa::WaveSystem* waveSys = em.GetSystem<Wiwa::WaveSystem>(self->waveId);
				waveSys->DestroyEnemy(enemy->GetEntity());
			}
			else
			{
				em.DestroyEntity(enemy->GetEntity());
			}
			m_TimerToDie = 0.0f;
		}

		m_TimerToDie += Time::GetDeltaTimeSeconds();
	}

	void RangedPhalanxDeathState::ExitState(EnemyRangedPhalanx* enemy)
	{
	}

	void RangedPhalanxDeathState::OnCollisionEnter(EnemyRangedPhalanx* enemy, const Object* body1, const Object* body2)
	{
	}
}

