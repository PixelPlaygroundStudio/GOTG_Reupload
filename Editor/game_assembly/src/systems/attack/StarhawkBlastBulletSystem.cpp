#include "StarhawkBlastBulletSystem.h"
#include "../../components/attack/SimpleBullet.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/utilities/EntityPool.h>
#include "../../components/attack/Attack.h"
#include "../../Systems.h"
#include <Wiwa/game/Items/ItemManager.h>

namespace Wiwa
{
	StarhawkBlastBulletSystem::StarhawkBlastBulletSystem()
	{
		m_BulletIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_AttackIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
	}

	StarhawkBlastBulletSystem::~StarhawkBlastBulletSystem()
	{

	}

	void StarhawkBlastBulletSystem::OnAwake()
	{
		m_AttackIt = GetComponentIterator<Attack>();
		m_BulletIt = GetComponentIterator<SimpleBullet>();

		Attack* attack = GetComponentByIterator<Attack>(m_AttackIt);
		if (attack)
		{
			std::string atStr = "STARHAWKS_BLAST";
			strcpy(attack->attackType, atStr.c_str());
			attack->isPlayerAttack = true;
			attack->isEnemyAttack = false;
		}
	}

	void StarhawkBlastBulletSystem::OnInit()
	{

	}

	void StarhawkBlastBulletSystem::InitBullet()
	{
		if (!getAwake())
			System::Awake();
		/*GameStateManager::s_PoolManager->SetScene(m_Scene);
		GameStateManager::s_PoolManager->s_SimpleBulletsPool->GetFromPool();*/

		SimpleBullet* bullet = GetComponentByIterator<SimpleBullet>(m_BulletIt);
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();
		Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();
		bullet->damage = Wiwa::ItemManager::GetAbility("Starhawk's Time Blast")->Damage;
		physicsManager.SetVelocity(obj, glm::normalize(bullet->direction) * bullet->velocity);
		//Wiwa::EntityManager& em = m_Scene->GetEntityManager();

		//PhysicsSystem* physSystem = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId);

		//if (physSystem)
		//{
		//	physSystem->DeactivateBody();
		//}


	}

	void StarhawkBlastBulletSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		SimpleBullet* bullet = GetComponentByIterator<SimpleBullet>(m_BulletIt);

		m_Timer += Time::GetDeltaTimeSeconds();

		if (m_Timer >= bullet->lifeTime)
		{
			GameStateManager::s_PoolManager->s_StarHawksBlastPool->ReturnToPool(m_EntityId);
		}
	}

	void StarhawkBlastBulletSystem::OnDestroy()
	{

	}

	void StarhawkBlastBulletSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		if (body1->id == m_EntityId)
		{
			GameStateManager::s_PoolManager->s_StarHawksBlastPool->ReturnToPool(m_EntityId);
		}
	}

	bool StarhawkBlastBulletSystem::EnableBullet()
	{
		SimpleBullet* bullet = GetComponent<SimpleBullet>();
		if (bullet)
		{
			InitBullet();
		}
		m_Timer = 0.0f;
		return true;
	}

	bool StarhawkBlastBulletSystem::OnDisabledFromPool()
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