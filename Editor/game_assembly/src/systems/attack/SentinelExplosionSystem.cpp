#include "SentinelExplosionSystem.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include "../../components/attack/Explosion.h"
#include "../../components/attack/Attack.h"

namespace Wiwa
{
	SentinelExplosionSystem::SentinelExplosionSystem()
	{
		m_ExplosionIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
		m_DoSentinelDamage = true;
	}

	SentinelExplosionSystem::~SentinelExplosionSystem()
	{

	}

	void SentinelExplosionSystem::OnAwake()
	{
		m_ExplosionIt = GetComponentIterator<Explosion>();
	}

	void SentinelExplosionSystem::OnInit()
	{
		InitExplosion();
	}

	void SentinelExplosionSystem::InitExplosion()
	{
		if (!getAwake())
			System::Awake();

		Explosion* explosion = GetComponentByIterator<Explosion>(m_ExplosionIt);

		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

		Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();

		m_DoSentinelDamage = true;

		//TODO: spawn prefab not from the pool

		EntityId m_ExplosionMarkId = em.LoadPrefab("library\\Enemy\\Explosions\\ExplosionRemnants_01.wiprefab");
		Transform3D* explosionMarkTr = em.GetComponent<Transform3D>(m_ExplosionMarkId);
		explosionMarkTr->localPosition = GetComponent<Transform3D>()->localPosition;
		explosionMarkTr->localPosition.y = 0.15;
		//EntityId explosion_prefab = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_explosion_remnants.wiprefab");
		//Wiwa::Transform3D* t3d = em.GetComponent<Transform3D>(explosion_prefab);
		//glm::vec3 position = GetTransform()->worldMatrix[3];
		//WI_INFO("position {0},{1},{2}", position.x,position.y,position.z);

		//t3d->localPosition = Math::GetWorldPosition(GetTransform()->worldMatrix);
		//WI_INFO("new position {0},{1},{2}", t3d->localPosition.x, t3d->localPosition.y, t3d->localPosition.z);
	}

	void SentinelExplosionSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		Explosion* explosion = GetComponentByIterator<Explosion>(m_ExplosionIt);

		m_Timer += Time::GetDeltaTimeSeconds();

		if (m_Timer >= 0.2f)
		{
			m_DoSentinelDamage = false;
		}

		if (m_Timer >= explosion->lifeTime)
		{
			if (explosion->isFromPool)
				GameStateManager::s_PoolManager->s_SentinelExplosion->ReturnToPool(m_EntityId);
			else
				GetEntityManager().DestroyEntity(m_EntityId);
		}

	}


	void SentinelExplosionSystem::OnDestroy()
	{

	}

	void SentinelExplosionSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		if (body1->id == m_EntityId)
		{
			std::string playerStr = "PLAYER";
			if (playerStr == body2->selfTagStr)
			{
				EntityManager& em = GetEntityManager();
				Attack* attack = GetComponentByIterator<Attack>(em.GetComponentIterator<Attack>(m_EntityId));
				Explosion* explosion = GetComponentByIterator<Explosion>(m_ExplosionIt);

				if (attack)
				{
					if (attack->isPlayerAttack)
						return;

					if (m_DoSentinelDamage)
					{
						GameStateManager::DamagePlayer(explosion->damage);
					}
					
				}
				else
				{

					if (m_DoSentinelDamage)
					{
						GameStateManager::DamagePlayer(explosion->damage);
					}
				}
			}

	/*		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			em.DestroyEntity(m_EntityId);*/
		}
	}

	void SentinelExplosionSystem::EnableExplosion()
	{
		InitExplosion();
	}

	bool SentinelExplosionSystem::OnDisabledFromPool()
	{
		Transform3D* transform = GetComponent<Transform3D>();
		if (transform)
		{
			transform->localPosition.y = 20000.0f;
		}

		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		PhysicsSystem* physSystem = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId);
		physSystem->DeleteBody();

		m_Timer = 0.0f;
		return true;
	}

}