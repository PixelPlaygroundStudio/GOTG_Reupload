#include <wipch.h>
#include "MeleePhalanxSpawn.h"
#include "../EnemyMeleePhalanx.h"

#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>

namespace Wiwa
{
	MeleePhalanxSpawnState::MeleePhalanxSpawnState()
	{

	}

	MeleePhalanxSpawnState::~MeleePhalanxSpawnState()
	{

	}

	void MeleePhalanxSpawnState::EnterState(EnemyMeleePhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		EntityId currentEnemy = enemy->GetEntity();
		enemy->m_AudioSys->PlayAudio("melee_spawn");
		enemy->m_AnimatorSys->PlayAnimation("spawn");
	}

	void MeleePhalanxSpawnState::UpdateState(EnemyMeleePhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);
		navAgent->autoRotate = true;

		if (enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->HasFinished())
			enemy->SwitchState(enemy->m_ChasingState);
	}

	void MeleePhalanxSpawnState::ExitState(EnemyMeleePhalanx* enemy)
	{
	}

	void MeleePhalanxSpawnState::OnCollisionEnter(EnemyMeleePhalanx* enemy, const Object* body1, const Object* body2)
	{
	}
}

