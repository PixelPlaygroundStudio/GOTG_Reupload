#include <wipch.h>
#include "SubjugatorSpawn.h"
#include "../EnemySubjugator.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

namespace Wiwa
{
	SubjugatorSpawnState::SubjugatorSpawnState()
	{

	}

	SubjugatorSpawnState::~SubjugatorSpawnState()
	{

	}

	void SubjugatorSpawnState::EnterState(EnemySubjugator* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		EntityId currentEnemy = enemy->GetEntity();
	
		//SubjugatorParticles - Spawn Particles
		//SubjugatorAudio - Spawn enemy->m_AudioSys
		enemy->m_AnimatorSys->PlayAnimation("spawn");
	}

	void SubjugatorSpawnState::UpdateState(EnemySubjugator* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);

		navAgent->autoRotate = true;

		if (enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->HasFinished())
			enemy->SwitchState(enemy->m_ChasingState);
	}

	void SubjugatorSpawnState::ExitState(EnemySubjugator* enemy)
	{

	}

	void SubjugatorSpawnState::OnCollisionEnter(EnemySubjugator* enemy, const Object* body1, const Object* body2)
	{

	}
}