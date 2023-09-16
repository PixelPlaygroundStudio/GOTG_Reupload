#include <wipch.h>
#include "MeleePhalanxDeath.h"
#include "../EnemyMeleePhalanx.h"
#include <Wiwa/ecs/components/game/enemy/Enemy.h>

#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/components/game/enemy/Enemy.h>
#include <Wiwa/ecs/systems/game/wave/WaveSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include <Wiwa/ecs/systems/PhysicsSystem.h>
#include <Wiwa/ecs/components/game/enemy/PhalanxMelee.h>

namespace Wiwa
{
	MeleePhalanxDeathState::MeleePhalanxDeathState()
	{
	}

	MeleePhalanxDeathState::~MeleePhalanxDeathState()
	{

	}

	void MeleePhalanxDeathState::EnterState(EnemyMeleePhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();

		enemy->m_AudioSys->PlayAudio("melee_dead");
		enemy->m_AnimatorSys->PlayAnimation("death");
		m_TimerToDie = 0.0f;

		if (enemy->m_NavAgentSys)
			enemy->m_NavAgentSys->StopAgent();

		PhysicsSystem* physSys = em.GetSystem<PhysicsSystem>(enemy->GetEntity());
		physSys->DeleteBody();
	}

	void MeleePhalanxDeathState::UpdateState(EnemyMeleePhalanx* enemy)
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
				PhalanxMelee* Pm = em.GetComponent<Wiwa::PhalanxMelee>(enemy->GetEntity());
				if (Pm->variant == 1)
				{
					GameStateManager::s_PoolManager->s_PhalanxMeleeVariantAPool->ReturnToPool(enemy->GetEntity());
				}
				else if (Pm->variant == 2)
				{
					GameStateManager::s_PoolManager->s_PhalanxMeleeVariantBPool->ReturnToPool(enemy->GetEntity());
				}
				else
				{
					GameStateManager::s_PoolManager->s_PhalanxMeleeGenericPool->ReturnToPool(enemy->GetEntity());
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

	void MeleePhalanxDeathState::ExitState(EnemyMeleePhalanx* enemy)
	{
	}

	void MeleePhalanxDeathState::OnCollisionEnter(EnemyMeleePhalanx* enemy, const Object* body1, const Object* body2)
	{
	}
}

