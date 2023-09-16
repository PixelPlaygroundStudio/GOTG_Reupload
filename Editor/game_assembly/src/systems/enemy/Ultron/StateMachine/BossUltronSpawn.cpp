#include <wipch.h>
#include "BossUltronSpawn.h"
#include "../BossUltron.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>

namespace Wiwa
{
	BossUltronSpawnState::BossUltronSpawnState()
	{

	}

	BossUltronSpawnState::~BossUltronSpawnState()
	{

	}

	void BossUltronSpawnState::EnterState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		
		//ParticleManager& pman = enemy->getScene().GetParticleManager();

		EntityId currentEnemy = enemy->GetEntity();

		//enemy->m_AnimatorSys->PlayAnimation("");
		//play particles
		//play Animation

		m_TimerSpawn = 0.0f;
	
		enemy->m_AudioSys->PlayAudio("vo_boss_spawn");
		enemy->m_AnimatorSys->PlayAnimation("entrance");
	}

	void BossUltronSpawnState::UpdateState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);

		m_TimerSpawn += Time::GetRealDeltaTimeSeconds();

		enemy->LookAt(playerTr->localPosition, 100.0f);

		/*glm::vec3 centerPosition = glm::vec3(0.0f);

		enemy->m_NavAgentSys->SetDestination(centerPosition);*/

		if (m_TimerSpawn > 3.0f)
		{
			// when animation finishes, pass to the movement state
		    /*if (enemy->m_AnimatorSys->HasFinished())*/

			enemy->SwitchState(enemy->m_MovementState);
		}
	}

	void BossUltronSpawnState::ExitState(BossUltron* enemy)
	{
	}

	void BossUltronSpawnState::OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2)
	{
	}
}