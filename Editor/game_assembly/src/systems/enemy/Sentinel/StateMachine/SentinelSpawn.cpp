#include <wipch.h>
#include "SentinelSpawn.h"
#include "../EnemySentinel.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>

namespace Wiwa
{
	SentinelSpawnState::SentinelSpawnState()
	{

	}

	SentinelSpawnState::~SentinelSpawnState()
	{

	}

	void SentinelSpawnState::EnterState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		EntityId currentEnemy = enemy->GetEntity();

		//SentinelParticles - Spawn Particles
		enemy->m_AudioSys->PlayAudio("sentinel_spawn");

		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);

		navAgent->agentSliding = true;

		enemy->m_AnimatorSys->PlayAnimation("spawn"); //AnimacionSentinel
	}

	void SentinelSpawnState::UpdateState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		if (enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->HasFinished())
			enemy->SwitchState(enemy->m_ChasingState);
	}

	void SentinelSpawnState::ExitState(EnemySentinel* enemy)
	{

	}

	void SentinelSpawnState::OnCollisionEnter(EnemySentinel* enemy, const Object* body1, const Object* body2)
	{

	}
}