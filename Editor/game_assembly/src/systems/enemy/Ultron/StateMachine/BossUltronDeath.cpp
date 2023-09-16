#include <wipch.h>
#include "BossUltronDeath.h"
#include "../BossUltron.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/components/game/enemy/Enemy.h>
#include <Wiwa/ecs/systems/game/wave/WaveSystem.h>
#include <Wiwa/Dialog/DialogManager.h>
#include <Wiwa/ecs/systems/AudioSystem.h>

namespace Wiwa
{
	BossUltronDeathState::BossUltronDeathState()
	{
		m_DeathExplosionPath1 = "assets/Enemy/Explosions/DeathExplosions_01.wiprefab";
		m_DeathExplosionPath2 = "assets/Enemy/Explosions/DeathExplosions_02.wiprefab";
		m_DeathExplosionPath3 = "assets/Enemy/Explosions/DeathExplosions_03.wiprefab";
		m_DeathExplosionPath4 = "assets/vfx/prefabs/vfx_finals/boss_Ultron/p_boss_death_explosion.wiprefab";
	}

	BossUltronDeathState::~BossUltronDeathState()
	{

	}

	void BossUltronDeathState::EnterState(BossUltron* enemy)
	{
		Wiwa::Scene& scene = enemy->getScene();
		
		GameStateManager::setFinishRoom(true);
		GameStateManager::setCanPassNextRoom(true);

		m_TimerDeathExplosions = 0.0f;
		m_DeathTimer = 0.0f;

		m_DeathState = DeathState::DEATH_INIT;
	}

	void BossUltronDeathState::UpdateState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Wiwa::Scene& ems = enemy->getScene();
		Wiwa::DialogManager& dialogManager = ems.GetDialogManager();
		EnemyState* self = (EnemyState*)em.GetComponentByIterator(enemy->m_EnemyStateIt);
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(enemy->GetEntity()));
		
		self->hasFinished = true;

		switch (m_DeathState)
		{
		case Wiwa::BossUltronDeathState::DeathState::DEATH_INIT:
		{
			enemy->m_NavAgentSys->StopAgentAndVelocity();

			/*enemy->m_AudioSys->PlayAudio("vo_boss_death");*/
			enemy->m_AnimatorSys->PlayAnimation("death");

			//for (int i = 0; i <= 6; ++i)
			//{
			//	EntityId thunderMarkId = em.LoadPrefab(m_DeathExplosionPath1);
			//	m_DeathExplosionIds.push_back(thunderMarkId); // Add the ID to the vector
			//	Transform3D* thunderMarkTr = em.GetComponent<Transform3D>(thunderMarkId);
			//	thunderMarkTr->localPosition = GetExplosionPosition(selfTr->localPosition, -2.0f, 2.0f);
			//}

			m_DeathState = DeathState::DEATH_INIT_EXPLOSION_1;
		}
		break;
		case Wiwa::BossUltronDeathState::DeathState::DEATH_INIT_EXPLOSION_1:
		{
			
			enemy->m_AudioSys->PlayAudio("explosion");

			for (int i = 0; i <= 6; ++i)
			{

				EntityId explosionId = SpawnRandomExplosion(enemy);
				Transform3D* explosionTr = em.GetComponent<Transform3D>(explosionId);
				explosionTr->localPosition = GetExplosionPosition(selfTr->localPosition, 0.0f, 6.0f);
			}

			m_TimerDeathExplosions = 0.0f;
			m_DeathState = DeathState::DEATH_INIT_EXPLOSION_2;
		}
		break;
		case Wiwa::BossUltronDeathState::DeathState::DEATH_INIT_EXPLOSION_2:
		{
			m_TimerDeathExplosions += Time::GetDeltaTimeSeconds();
			
			if (m_TimerDeathExplosions > 1.0f)
			{
				enemy->m_AudioSys->PlayAudio("explosion");

				for (int i = 0; i <= 12; ++i)
				{
					EntityId explosionId = SpawnRandomExplosion(enemy);
					Transform3D* explosionTr = em.GetComponent<Transform3D>(explosionId);
					explosionTr->localPosition = GetExplosionPosition(selfTr->localPosition, -6.0f, 2.0f);
				}

				m_TimerDeathExplosions = 0.0f;
				m_DeathState = DeathState::DEATH_INIT_EXPLOSION_3;
			}
		}
		break;
		case Wiwa::BossUltronDeathState::DeathState::DEATH_INIT_EXPLOSION_3:
		{
			m_TimerDeathExplosions += Time::GetDeltaTimeSeconds();

			if (m_TimerDeathExplosions > 1.0f)
			{
				enemy->m_AudioSys->PlayAudio("explosion");

				for (int i = 0; i <= 4; ++i)
				{
					EntityId explosionId = SpawnRandomExplosion(enemy);
					Transform3D* explosionTr = em.GetComponent<Transform3D>(explosionId);
					explosionTr->localPosition = GetExplosionPosition(selfTr->localPosition, -4.0f, 4.0f);
				}

				m_TimerDeathExplosions = 0.0f;
				m_DeathState = DeathState::DEATH_EXPLOSIONS;
			}
		}
		break;
		case Wiwa::BossUltronDeathState::DeathState::DEATH_EXPLOSIONS:
		{
			m_DeathTimer += Time::GetDeltaTimeSeconds();
			m_TimerDeathExplosions += Time::GetDeltaTimeSeconds();

			/*if (m_TimerDeathExplosions > 0.5f)
			{
				m_TimerDeathExplosions = 0.0f;

				m_DeathExplosionId = em.LoadPrefab(m_DeathExplosionPath);
				Transform3D* explosionTr = em.GetComponent<Transform3D>(m_DeathExplosionId);

				explosionTr->localPosition = GetExplosionPosition(selfTr->localPosition, -2.0f, 2.0f);
			}*/

			if (m_DeathTimer > 2.0f)
			{

				m_TimerDeathExplosions = 0.0f;

				Wiwa::DialogManager& dialogManager = ems.GetDialogManager();
				//ParticleManager& pman = enemy->getScene().GetParticleManager();
				dialogManager.ForceDialogStart("UltronIsKilled", true);

				m_DeathState = DeathState::DEATH_TALK;
			}
		}
		break;
		case Wiwa::BossUltronDeathState::DeathState::DEATH_TALK:
		{
			m_TimerDeathExplosions += Time::GetDeltaTimeSeconds();

			if (dialogManager.actualConversationState == 1) // while talking
			{
				if (m_TimerDeathExplosions > 2.0f)
				{
					enemy->m_AudioSys->PlayAudio("explosion");

					EntityId explosionId = SpawnRandomExplosion(enemy);
					Transform3D* explosionTr = em.GetComponent<Transform3D>(explosionId);
					explosionTr->localPosition = GetExplosionPosition(selfTr->localPosition, -1.0f, 1.0f);

					m_TimerDeathExplosions = 0.0f;
				}

				enemy->m_NavAgentSys->StopAgent();
			}

			if (dialogManager.actualConversationState == 2) // when dialog ends
			{
				//Spawn big final explosion
				m_FinalDeathExplosionId = em.LoadPrefab(m_DeathExplosionPath4);
			    Transform3D* explosionTr = em.GetComponent<Transform3D>(m_FinalDeathExplosionId);
			    explosionTr->localPosition.x = selfTr->localPosition.x;
				explosionTr->localPosition.y = selfTr->localPosition.y;
				explosionTr->localPosition.z = selfTr->localPosition.z;

				m_DeathTimer = 0.0f;

				m_DeathState = DeathState::DEATH_FINISH;
			}
		}
		break;
		case Wiwa::BossUltronDeathState::DeathState::DEATH_FINISH:
		{
			m_DeathTimer += Time::GetDeltaTimeSeconds();

			if (m_DeathTimer > 2.0f)
			{
				em.DestroyEntity(enemy->GetEntity());

				GameStateManager::s_HasFinshedRoom = true;
				GameStateManager::setCanPassNextRoom(true);
			}
		}
		break;
		default:
			break;
		}
	}

	void BossUltronDeathState::ExitState(BossUltron* enemy)
	{

	}

	void BossUltronDeathState::OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2)
	{

	}

	EntityId BossUltronDeathState::SpawnRandomExplosion(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();

		int randomNumber = RAND(1, 3);

		if (randomNumber == 1)
		{
			return em.LoadPrefab(m_DeathExplosionPath1);
		}
		else if (randomNumber == 2)
		{
			return em.LoadPrefab(m_DeathExplosionPath2);
		}
		else if (randomNumber == 3)
		{
			return em.LoadPrefab(m_DeathExplosionPath3);
		}
	}

	glm::vec3  BossUltronDeathState::GetExplosionPosition(const glm::vec3& center, float minDistance, float maxDistance)
	{
		// Create a random number generator
		std::random_device rd;
		std::mt19937 gen(rd());

		// Create a distribution that produces numbers between min and max
		std::uniform_real_distribution<float> dis(minDistance, maxDistance);

		// Generate a random number
		float randomNumberX = dis(gen);
		float randomNumberZ = dis(gen);

		return glm::vec3(center.x + randomNumberX, center.y , center.z + randomNumberZ);
	}
}