#include "StarLordBullet.h"
#include "../../components/attack/SimpleBullet.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include "Wiwa/ecs/systems/AudioSystem.h"
#include "Wiwa/ecs/systems/ParticleSystem.h"
#include <Wiwa/utilities/EntityPool.h>
#include "../../components/attack/Attack.h"

namespace Wiwa
{
	StarLordBulletSystem::StarLordBulletSystem()
	{
		m_BulletIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_AttackIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };

		m_Timer = 0.0f;
	}

	StarLordBulletSystem::~StarLordBulletSystem()
	{

	}

	void StarLordBulletSystem::OnAwake()
	{
		m_AttackIt = GetComponentIterator<Attack>();
		m_BulletIt = GetComponentIterator<SimpleBullet>();
		Attack* attack = GetComponentByIterator<Attack>(m_AttackIt);
		if (attack)
		{
			std::string atStr = "STARLORD_BULLET";
			strcpy(attack->attackType, atStr.c_str());
			attack->isPlayerAttack = true;
			attack->isEnemyAttack = false;
		}
	}

	void StarLordBulletSystem::OnInit()
	{

	}

	void StarLordBulletSystem::InitBullet()
	{
		if (!getAwake())
			System::Awake();

		SimpleBullet* bullet = GetComponentByIterator<SimpleBullet>(m_BulletIt);
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();
		Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

		physicsManager.SetVelocity(obj, glm::normalize(bullet->direction) * bullet->velocity);
	}

	void StarLordBulletSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		SimpleBullet* bullet = GetComponentByIterator<SimpleBullet>(m_BulletIt);

		m_Timer += Time::GetDeltaTimeSeconds();

		if (m_Timer >= bullet->lifeTime)
		{
			if (bullet->isFromPool)
				GameStateManager::s_PoolManager->s_StarLordBullets->ReturnToPool(m_EntityId);
			else
				GetEntityManager().DestroyEntity(m_EntityId);
		}
	}

	void StarLordBulletSystem::OnDestroy()
	{

	}

	void StarLordBulletSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		if (body1->id == m_EntityId)
		{
			/*std::string enemyStr = "ENEMY";
			if (enemyStr == body2->selfTagStr)
			{
				SimpleBullet* bullet = GetComponentByIterator<SimpleBullet>(m_BulletIt);
				GameStateManager::DamagePlayer(bullet->damage);
			}*/
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			Wiwa::AudioSystem* audio = em.GetSystem<Wiwa::AudioSystem>(m_EntityId);
			if (audio)
			{
				audio->PlayAudio("bullet_hit");
			}
			else {
				WI_ERROR("Couldn't get audio system: starlord bullet");
			}

			std::string tagStr = "ENEMY";
			if (tagStr == body2->selfTagStr)
			{				
				EntityId lapsus = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_playerbullethit_enemy.wiprefab");
				
				if (lapsus != EntityManager::INVALID_INDEX)
				{
					Transform3D* tlapsus = em.GetComponent<Transform3D>(lapsus);
					Transform3D* tbulletplayer = em.GetComponent<Transform3D>(m_EntityId);

					if (tlapsus != nullptr)
					{
						tlapsus->localPosition = tbulletplayer->localPosition;
					}
				}
			}
			else
			{
				EntityId lapsus = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_playerbullethit_wall.wiprefab");
				
				if (lapsus != EntityManager::INVALID_INDEX)
				{
					Transform3D* tlapsus = em.GetComponent<Transform3D>(lapsus);
					Transform3D* tbulletplayer = em.GetComponent<Transform3D>(m_EntityId);
				
					if (tlapsus != nullptr)
					{
						tlapsus->localPosition = tbulletplayer->localPosition;
						tlapsus->rotation = tbulletplayer->rotation;
						tlapsus->localRotation = tbulletplayer->localRotation;
					}
				}

				//EntityId lapsus = em.GetChildByName(m_EntityId,"p_playerbullethit_wall");
				//
				//if (lapsus != EntityManager::INVALID_INDEX)
				//{
				//	ParticleSystem* plapsus = em.GetSystem<ParticleSystem>(lapsus);
				//
				//	if (plapsus != nullptr)
				//	{
				//		plapsus->SetActive(true);
				//		plapsus->EmitParticleBatch();	
				//		plapsus->SetActive(false);
				//	}
				//}
			}

			SimpleBullet* bullet = GetComponentByIterator<SimpleBullet>(m_BulletIt);
			if (bullet->isFromPool)
				GameStateManager::s_PoolManager->s_StarLordBullets->ReturnToPool(m_EntityId);
			else
				GetEntityManager().DestroyEntity(m_EntityId);
		}
	}

	bool StarLordBulletSystem::EnableBullet()
	{
		SimpleBullet* bullet = GetComponent<SimpleBullet>();
		if (bullet)
		{
			InitBullet();
		}
		m_Timer = 0.0f;
		return true;
	}

	bool StarLordBulletSystem::OnDisabledFromPool()
	{
		Transform3D* transform = GetComponent<Transform3D>();
		if (transform)
		{
			transform->localPosition.y = 3000.0f;
		}

		Wiwa::EntityManager& em = m_Scene->GetEntityManager();

		PhysicsSystem* physSystem = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId);

		physSystem->DeleteBody();

		m_Timer = 0.0f;

		return true;
	}
}