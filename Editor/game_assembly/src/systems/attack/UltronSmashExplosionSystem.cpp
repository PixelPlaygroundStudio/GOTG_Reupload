#include "UltronSmashExplosionSystem.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/utilities/EntityPool.h>
#include "../../components/attack/Explosion.h"

namespace Wiwa
{
	UltronSmashExplosionSystem::UltronSmashExplosionSystem()
	{
		m_ExplosionIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
		m_DoSmashDamage = true;
	}

	UltronSmashExplosionSystem::~UltronSmashExplosionSystem()
	{

	}

	void UltronSmashExplosionSystem::OnAwake()
	{
		m_ExplosionIt = GetComponentIterator<Explosion>();
	}

	void UltronSmashExplosionSystem::OnInit()
	{

	}

	void UltronSmashExplosionSystem::InitExplosion()
	{
		if (!getAwake())
			System::Awake();

		Explosion* explosion = GetComponentByIterator<Explosion>(m_ExplosionIt);
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();
		Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

		m_DoSmashDamage = true;
	}

	void UltronSmashExplosionSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		Explosion* explosion = GetComponentByIterator<Explosion>(m_ExplosionIt);

		m_Timer += Time::GetDeltaTimeSeconds();

		if (m_Timer >= 0.2f)
		{
			m_DoSmashDamage = false;
		}

		if (m_Timer >= explosion->lifeTime)
		{
			GameStateManager::s_PoolManager->s_SmashExplosionPool->ReturnToPool(m_EntityId);
		}
	}

	void UltronSmashExplosionSystem::OnDestroy()
	{

	}

	void UltronSmashExplosionSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		if (body1->id == m_EntityId)
		{
			std::string playerStr = "PLAYER";
			if (playerStr == body2->selfTagStr)
			{
				if (m_DoSmashDamage)
				{
					Explosion* explosion = GetComponentByIterator<Explosion>(m_ExplosionIt);
					GameStateManager::DamagePlayer(explosion->damage);
				}
			}
		}
	}

	bool UltronSmashExplosionSystem::EnableExplosion()
	{
		Explosion* explosion = GetComponent<Explosion>();
		if (explosion)
		{
			InitExplosion();
		}
		m_Timer = 0.0f;
		return true;
	}

	bool UltronSmashExplosionSystem::OnDisabledFromPool()
	{
		Transform3D* transform = GetComponent<Transform3D>();
		if (transform)
		{
			transform->localPosition.y = 1000.0f;
		}

		Wiwa::EntityManager& em = m_Scene->GetEntityManager();

		PhysicsSystem* physSystem = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId);

		physSystem->DeleteBody();

		m_Timer = 0.0f;

		return true;
	}
}