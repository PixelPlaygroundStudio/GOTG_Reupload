#include "SimpleBulletSystem.h"
#include "../../components/attack/SimpleBullet.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/utilities/EntityPool.h>
#include "../../components/attack/Attack.h"
#include "../../Systems.h"

namespace Wiwa
{
	SimpleBulletSystem::SimpleBulletSystem()
	{
		m_BulletIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_AttackIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
	}

	SimpleBulletSystem::~SimpleBulletSystem()
	{

	}

	void SimpleBulletSystem::OnAwake()
	{
		m_AttackIt = GetComponentIterator<Attack>();
		m_BulletIt = GetComponentIterator<SimpleBullet>();

		Attack* attack = GetComponentByIterator<Attack>(m_AttackIt);
		if (attack)
		{
			std::string atStr = "SIMPLE_BULLET";
			strcpy(attack->attackType, atStr.c_str());
			attack->isPlayerAttack = false;
			attack->isEnemyAttack = true;
		}
	}

	void SimpleBulletSystem::OnInit()
	{
	
	}

	void SimpleBulletSystem::InitBullet()
	{
		if (!getAwake())
			System::Awake();
		/*GameStateManager::s_PoolManager->SetScene(m_Scene);
		GameStateManager::s_PoolManager->s_SimpleBulletsPool->GetFromPool();*/

		SimpleBullet* bullet = GetComponentByIterator<SimpleBullet>(m_BulletIt);
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();
		Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

		physicsManager.SetVelocity(obj, glm::normalize(bullet->direction) * bullet->velocity);
		//Wiwa::EntityManager& em = m_Scene->GetEntityManager();

		//PhysicsSystem* physSystem = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId);

		//if (physSystem)
		//{
		//	physSystem->DeactivateBody();
		//}
	}

	void SimpleBulletSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();
		
		SimpleBullet* bullet = GetComponentByIterator<SimpleBullet>(m_BulletIt);

		m_Timer += Time::GetDeltaTimeSeconds();

		if (m_Timer >= bullet->lifeTime)
		{
			GameStateManager::s_PoolManager->s_SimpleBulletsPool->ReturnToPool(m_EntityId);
		}	
	}

	void SimpleBulletSystem::OnDestroy()
	{

	}

	void SimpleBulletSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		if (body1->id == m_EntityId)
		{
			std::string playerStr = "PLAYER";
			if (playerStr == body2->selfTagStr)
			{
				SimpleBullet* bullet = GetComponentByIterator<SimpleBullet>(m_BulletIt);
				GameStateManager::DamagePlayer(bullet->damage);

				//emit player blood
				Wiwa::EntityManager& entityManager = getScene().GetEntityManager();
				EntityId playerId = entityManager.GetEntityByName("StarLord");

				if (playerId != EntityManager::INVALID_INDEX)
				{
					EntityId p_hurt = entityManager.GetChildByName(playerId, "p_player_hurt");

					if (p_hurt != EntityManager::INVALID_INDEX)
					{
						entityManager.SetActive(p_hurt, true);

						ParticleSystem* sys_p_hurt = entityManager.GetSystem<ParticleSystem>(p_hurt);

						if (sys_p_hurt != nullptr)
							sys_p_hurt->EmitParticleBatch();
						
					}
				}

				EntityId lapsus = entityManager.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_enemybullethit_player.wiprefab");

				if (lapsus != EntityManager::INVALID_INDEX)
				{
					Transform3D* tlapsus = entityManager.GetComponent<Transform3D>(lapsus);
					Transform3D* tbulletplayer = entityManager.GetComponent<Transform3D>(m_EntityId);

					if (tlapsus != nullptr)
					{
						tlapsus->localPosition = tbulletplayer->localPosition;
					}
				}
			}

			playerStr = "WALL";

			if (playerStr == body2->selfTagStr)
			{
				Wiwa::EntityManager& entityManager = getScene().GetEntityManager();
				EntityId lapsus = entityManager.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_enemybullethit_wall.wiprefab");

				if (lapsus != EntityManager::INVALID_INDEX)
				{
					Transform3D* tlapsus = entityManager.GetComponent<Transform3D>(lapsus);
					Transform3D* tbulletplayer = entityManager.GetComponent<Transform3D>(m_EntityId);

					if (tlapsus != nullptr)
					{
						tlapsus->localPosition = tbulletplayer->localPosition;
					}
				}
			}

			GameStateManager::s_PoolManager->s_SimpleBulletsPool->ReturnToPool(m_EntityId);
		}
	}

	bool SimpleBulletSystem::EnableBullet()
	{
		SimpleBullet* bullet = GetComponent<SimpleBullet>();
		if (bullet)
		{
			InitBullet();
		}
		m_Timer = 0.0f;
		return true;
	}

	bool SimpleBulletSystem::OnDisabledFromPool()
	{
		Transform3D* transform = GetComponent<Transform3D>();
		if (transform)
		{
			transform->localPosition.y = 3000.0f;
		}

		//SimpleBullet* bullet = GetComponent<SimpleBullet>();
		//
		//if (bullet)
		//{
		//	bullet->direction = glm::vec3(0.0f);
		//	bullet->lifeTime = 0.0f;
		//	bullet->velocity = 0.0f;
		//}

		//CollisionBody* collisionBody = GetComponent<CollisionBody>();

		Wiwa::EntityManager& em = m_Scene->GetEntityManager();

		PhysicsSystem* physSystem = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId);

		physSystem->DeleteBody();

		m_Timer = 0.0f;

		return true;
	}
}
