#include "SecondDashSystem.h"
#include "../../components/attack/DashEffect.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/utilities/EntityPool.h>
#include "../../components/attack/Attack.h"
#include "../enemy/Ultron/BossUltron.h"
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/components/game/Health.h>

namespace Wiwa
{
	SecondDashSystem::SecondDashSystem()
	{
		m_DashIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_AttackIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
	}

	SecondDashSystem::~SecondDashSystem()
	{

	}

	void SecondDashSystem::OnAwake()
	{
		m_AttackIt = GetComponentIterator<Attack>();
		m_DashIt = GetComponentIterator<DashEffect>();

		//Attack* attack = GetComponentByIterator<Attack>(m_AttackIt);
		//if (attack)
		//{
		//	std::string atStr = "ULTRON_SECOND_DASH";
		//	strcpy(attack->attackType, atStr.c_str());
		//	attack->isPlayerAttack = false;
		//	attack->isEnemyAttack = true;
		//}
	}

	void SecondDashSystem::OnInit()
	{

	}

	void SecondDashSystem::InitBullet()
	{
		if (!getAwake())
			System::Awake();

		DashEffect* dash = GetComponentByIterator<DashEffect>(m_DashIt);
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();
		Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();
		m_Timer = 0.0f;
		m_CollisionWithWall = false;

		physicsManager.SetVelocity(obj, glm::normalize(dash->direction) * dash->velocity);
	}

	void SecondDashSystem::OnUpdate()
	{   
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		DashEffect* bullet = GetComponentByIterator<DashEffect>(m_DashIt);
		Transform3D* transform = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(m_EntityId));
		Health* healthBossUltron = (Health*)em.GetComponentByIterator(em.GetComponentIterator<Health>(BossUltronEnemy->GetEntity()));

		m_Timer += Time::GetDeltaTimeSeconds();

		if (m_Timer > bullet->lifeTime)
		{
			GameStateManager::s_PoolManager->s_UltronSecondDashPool->ReturnToPool(m_EntityId);
		}

		if (m_CollisionWithWall == true)
		{
			Transform3D* transformBossUltron = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(BossUltronEnemy->GetEntity()));
			Wiwa::NavAgentSystem* agent = em.GetSystem<Wiwa::NavAgentSystem>(BossUltronEnemy->GetEntity());
			transformBossUltron->localPosition.x = transform->localPosition.x;
			transformBossUltron->localPosition.z = transform->localPosition.z;

			GameStateManager::s_PoolManager->s_UltronSecondDashPool->ReturnToPool(m_EntityId);

			agent->RegisterWithCrowd();
			agent->SetPosition(transformBossUltron->localPosition);
			agent->StopAgent();
		}

		if (m_CollisionWithWall == false)
		{
			if ((m_Timer >= bullet->lifeTime - 0.05f) && (m_Timer <= bullet->lifeTime))
			{
				if (transform->localPosition.y != 3000.0f && healthBossUltron->health > 0.0f)
				{
					Transform3D* transformBossUltron = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(BossUltronEnemy->GetEntity()));
					Wiwa::NavAgentSystem* agent = em.GetSystem<Wiwa::NavAgentSystem>(BossUltronEnemy->GetEntity());
					transformBossUltron->localPosition.x = transform->localPosition.x;
					transformBossUltron->localPosition.z = transform->localPosition.z;

					agent->RegisterWithCrowd();
					agent->SetPosition(transformBossUltron->localPosition);
					agent->StopAgent();
				}
			}
		}
		
	}

	void SecondDashSystem::OnDestroy()
	{

	}

	void SecondDashSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		if (body1->id == m_EntityId)
		{
			std::string playerStr = "PLAYER";
			if (playerStr == body2->selfTagStr)
			{
				DashEffect* dash = GetComponentByIterator<DashEffect>(m_DashIt);
				GameStateManager::DamagePlayer(dash->damage);
			}

			std::string wallStr = "WALL";
			if (wallStr == body2->selfTagStr)
			{
				DashEffect* dash = GetComponentByIterator<DashEffect>(m_DashIt);
				Wiwa::EntityManager& em = m_Scene->GetEntityManager();
				Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();
				Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

				physicsManager.SetVelocity(obj, glm::normalize(dash->direction) * 0.0f);

				m_CollisionWithWall = true;
			}

			/*GameStateManager::s_PoolManager->s_UltronSecondDashPool->ReturnToPool(m_EntityId);*/
		}
	}

	bool SecondDashSystem::EnableBullet(BossUltron* enemy)
	{
		DashEffect* dash = GetComponent<DashEffect>();
		if (dash)
		{
			InitBullet();
		}
		m_Timer = 0.0f;

		BossUltronEnemy = enemy;

		return true;
	}

	bool SecondDashSystem::OnDisabledFromPool()
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

	glm::vec3 SecondDashSystem::GetPositionAfterDash(glm::vec3 position)
	{
		return position;
	}
}