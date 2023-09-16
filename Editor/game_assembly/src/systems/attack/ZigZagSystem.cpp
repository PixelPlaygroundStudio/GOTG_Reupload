#include "ZigZagSystem.h"
#include "../../components/attack/ZigZagBullet.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/utilities/EntityPool.h>
#include "../../components/attack/Attack.h"

namespace Wiwa
{
	ZigZagBulletSystem::ZigZagBulletSystem()
	{
		m_BulletIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_AttackIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
		m_TimerAmplitude = 0.0f;
		m_ZigZagTimer = 0.0f;
		m_GetMaxAmplitude = false;
	}

	ZigZagBulletSystem::~ZigZagBulletSystem()
	{

	}

	void ZigZagBulletSystem::OnAwake()
	{
		m_AttackIt = GetComponentIterator<Attack>();
		m_BulletIt = GetComponentIterator<ZigZagBullet>();

		Attack* attack = GetComponentByIterator<Attack>(m_AttackIt);
		if (attack)
		{
			std::string atStr = "ZIG_ZAG_BULLET";
			strcpy(attack->attackType, atStr.c_str());
			attack->isPlayerAttack = false;
			attack->isEnemyAttack = true;
		}
	}

	void ZigZagBulletSystem::OnInit()
	{

	}

	void ZigZagBulletSystem::InitBullet()
	{
		if (!getAwake())
			System::Awake();
		
		ZigZagBullet* bullet = GetComponentByIterator<ZigZagBullet>(m_BulletIt);
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();
		Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

		m_Timer = 0.0f;
		m_TimerAmplitude = 0.0f;
		m_ZigZagTimer = 0.0f;
		m_GetMaxAmplitude = false;

		physicsManager.SetVelocity(obj, glm::normalize(bullet->direction) * bullet->velocity);
	}

	void ZigZagBulletSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		ZigZagBullet* bullet = GetComponentByIterator<ZigZagBullet>(m_BulletIt);
		Transform3D* transform = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(m_EntityId));
		

		m_Timer += Time::GetDeltaTimeSeconds();
		m_TimerAmplitude += Time::GetDeltaTimeSeconds();
		m_ZigZagTimer += Time::GetDeltaTimeSeconds();

		if (m_TimerAmplitude >= 2.0f)
		{
			m_TimerAmplitude = 2.0f;
		}

		if (m_GetMaxAmplitude == false)
		{
			glm::vec3 updatePosition = ZigZagBulletMotion(transform->localPosition, 0.5f, 1.4f / 2, 10.0f, m_ZigZagTimer, bullet->direction, m_TimerAmplitude / 5.0f);
			transform->localPosition = updatePosition;
			em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->ForceSetPosition(updatePosition);

			m_GetMaxAmplitude = true;
		}

		if (m_GetMaxAmplitude == true)
		{
			glm::vec3 updatePosition = ZigZagBulletMotion(transform->localPosition, 0.5f, 1.4f, 10.0f, m_ZigZagTimer, bullet->direction, m_TimerAmplitude / 5.0f);
			transform->localPosition = updatePosition;
			em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->ForceSetPosition(updatePosition);
		}

		if (m_Timer >= bullet->lifeTime)
		{
			GameStateManager::s_PoolManager->s_ZigZagBulletPool->ReturnToPool(m_EntityId);
		}
	}

	void ZigZagBulletSystem::OnDestroy()
	{

	}

	void ZigZagBulletSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		if (body1->id == m_EntityId)
		{
			std::string playerStr = "PLAYER";
			if (playerStr == body2->selfTagStr)
			{
				Wiwa::EntityManager& em = getScene().GetEntityManager();
				ZigZagBullet* bullet = GetComponentByIterator<ZigZagBullet>(m_BulletIt);
				GameStateManager::DamagePlayer(bullet->damage);

				EntityId zigzagHitPlayer = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_enemybullethit_player.wiprefab");

				if (zigzagHitPlayer != EntityManager::INVALID_INDEX)
				{
					Transform3D* zigzagHitPlayerTr = em.GetComponent<Transform3D>(zigzagHitPlayer);
					Transform3D* bulletPlayerTr = em.GetComponent<Transform3D>(m_EntityId);

					if (zigzagHitPlayerTr != nullptr)
					{
						zigzagHitPlayerTr->localPosition = bulletPlayerTr->localPosition;
					}
				}
			}

			std::string wallStr = "WALL";

			if (wallStr == body2->selfTagStr)
			{
				Wiwa::EntityManager& entityManager = getScene().GetEntityManager();
				EntityId zigzagHitWall = entityManager.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_enemybullethit_wall.wiprefab");

				if (zigzagHitWall != EntityManager::INVALID_INDEX)
				{
					Transform3D* zigzagHitWallTr = entityManager.GetComponent<Transform3D>(zigzagHitWall);
					Transform3D* bulletWallTr = entityManager.GetComponent<Transform3D>(m_EntityId);

					if (zigzagHitWallTr != nullptr)
					{
						zigzagHitWallTr->localPosition = bulletWallTr->localPosition;
					}
				}
			}

			GameStateManager::s_PoolManager->s_ZigZagBulletPool->ReturnToPool(m_EntityId);
		}
	}

	bool ZigZagBulletSystem::EnableBullet()
	{
		ZigZagBullet* bullet = GetComponent<ZigZagBullet>();
		if (bullet)
		{
			InitBullet();
		}
		m_Timer = 0.0f;
		m_ZigZagTimer = 0.0f;
		return true;
	}

	bool ZigZagBulletSystem::OnDisabledFromPool()
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
		m_ZigZagTimer = 0.0f;

		return true;
	}

	glm::vec3 ZigZagBulletSystem::ZigZagBulletMotion(glm::vec3 current_position, float speed, float amplitude, float frequency, float delta_time, glm::vec3 forward_direction, float timerAmplitude)
	{		
		forward_direction = glm::normalize(forward_direction);

		glm::vec3 world_up(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::normalize(glm::cross(forward_direction, world_up));
		glm::vec3 new_up = glm::cross(right, forward_direction);

		float zigzag_offset = amplitude * sin(frequency * delta_time) * timerAmplitude;

		glm::vec3 zigzag_global = zigzag_offset * right;

		glm::vec3 new_position = current_position + forward_direction * speed * delta_time + zigzag_global;

		return new_position;		
	}
}