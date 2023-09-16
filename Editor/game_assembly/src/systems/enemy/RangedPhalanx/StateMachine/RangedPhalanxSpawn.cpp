#include <wipch.h>
#include "RangedPhalanxSpawn.h"
#include "../EnemyRangedPhalanx.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>

namespace Wiwa
{
	RangedPhalanxSpawnState::RangedPhalanxSpawnState()
	{

	}

	RangedPhalanxSpawnState::~RangedPhalanxSpawnState()
	{

	}

	void RangedPhalanxSpawnState::EnterState(EnemyRangedPhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();

		enemy->m_AudioSys->PlayAudio("ranged_spawn");
		EntityId currentEnemy = enemy->GetEntity();
		enemy->m_AnimatorSys->PlayAnimation("spawn");
	}

	void RangedPhalanxSpawnState::UpdateState(EnemyRangedPhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);

		navAgent->autoRotate = true;
		//if (enemy->m_AnimatorSys->HasFinished())
			enemy->SwitchState(enemy->m_ChasingState);
	}

	void RangedPhalanxSpawnState::ExitState(EnemyRangedPhalanx* enemy)
	{
	}

	void RangedPhalanxSpawnState::OnCollisionEnter(EnemyRangedPhalanx* enemy, const Object* body1, const Object* body2)
	{
	}
}

