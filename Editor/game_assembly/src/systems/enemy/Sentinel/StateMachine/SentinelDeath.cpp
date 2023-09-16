#include <wipch.h>
#include "SentinelDeath.h"
#include "../EnemySentinel.h"
#include <Wiwa/ecs/components/game/wave/Wave.h>
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include <Wiwa/ecs/components/game/enemy/Enemy.h>

#include "../../../attack/SentinelExplosionSystem.h"
#include <Wiwa/ecs/systems/game/wave/WaveSystem.h>
#include "../../../../components/attack/Attack.h"
#include "../../../../components/attack/Explosion.h"

namespace Wiwa
{
	SentinelDeathState::SentinelDeathState()
	{
		//m_ExplosionMarkPath = "assets\\Enemy\\Explosions\\ExplosionRemnants_01.wiprefab";
	}

	SentinelDeathState::~SentinelDeathState()
	{

	}

	void SentinelDeathState::EnterState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		

		/*enemy->m_AnimatorSys->PlayAnimation("atackdeath", false);*/ //AnimacionSentinel

		if (enemy->m_NavAgentSys)
			enemy->m_NavAgentSys->StopAgent();

		PhysicsSystem* physSys = em.GetSystem<PhysicsSystem>(enemy->GetEntity());
		physSys->DeleteBody();
	}

	void SentinelDeathState::UpdateState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);
		
		enemy->m_AudioSys->PlayAudio("explosion");

		//Sentinel Explosion
		GameStateManager::s_PoolManager->SetScene(&enemy->getScene());
		EntityId newExplosionId = GameStateManager::s_PoolManager->s_SentinelExplosion->GetFromPool();
		SentinelExplosionSystem* explosionSys = em.GetSystem<SentinelExplosionSystem>(newExplosionId);

		Attack* attack = (Attack*)em.GetComponentByIterator(em.GetComponentIterator<Attack>(newExplosionId));
		attack->isEnemyAttack = true;
		attack->isPlayerAttack = false;

		Explosion* explosion = (Explosion*)em.GetComponentByIterator(em.GetComponentIterator<Explosion>(newExplosionId));
		explosion->isFromPool = true;

		PhysicsSystem* physSys = em.GetSystem<PhysicsSystem>(newExplosionId);
		physSys->DeleteBody();

		// Set intial positions
		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
		Transform3D* explosionTr = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(newExplosionId));

		if (!explosionTr || !playerTr)
			return;

		explosionTr->localPosition = Math::GetWorldPosition(selfTr->worldMatrix);
		explosionTr->localRotation = glm::vec3(-90.0f, 0.0f, playerTr->localRotation.y + 90.0f);
		explosionTr->localScale = selfTr->localScale;

		physSys->CreateBody();
		explosionSys->EnableExplosion();

		/*m_ExplosionMarkId = em.LoadPrefab(m_ExplosionMarkPath);
		Transform3D* explosionMarkTr = em.GetComponent<Transform3D>(m_ExplosionMarkId);
		explosionMarkTr->localPosition = selfTr->localPosition;*/

		// Destroy enemy
		EnemyState* self = (EnemyState*)em.GetComponentByIterator(enemy->m_EnemyStateIt);
		self->hasFinished = true;
		if (self->waveId != -1)
		{
			Wiwa::WaveSystem* waveSys = em.GetSystem<Wiwa::WaveSystem>(self->waveId);
			waveSys->DestroyEnemy(enemy->GetEntity());
			GameStateManager::s_PoolManager->s_SentinelPool->ReturnToPool(enemy->GetEntity());
		}
		else
		{
			em.DestroyEntity(enemy->GetEntity());
		}
			
		//m_TimerToDie += Time::GetDeltaTimeSeconds();
	}

	void SentinelDeathState::ExitState(EnemySentinel* enemy)
	{
	}

	void SentinelDeathState::OnCollisionEnter(EnemySentinel* enemy, const Object* body1, const Object* body2)
	{
	}
}