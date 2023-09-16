#include "StarhawksBlastSystem.h"
#include "../../components/attack/StarhawkBlast.h"
#include "../../components/attack/SimpleBullet.h"
#include "../../systems/attack/StarhawkBlastBulletSystem.h"
#include "../../components/attack/Attack.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/game/Items/ItemManager.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/ecs/systems/AudioSystem.h>

namespace Wiwa
{
	StarhawksBlastSystem::StarhawksBlastSystem()
	{
		m_StarHawksIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
	}

	StarhawksBlastSystem::~StarhawksBlastSystem()
	{

	}

	void StarhawksBlastSystem::OnAwake()
	{
		m_StarHawksIt = GetComponentIterator<StarhawksBlast>();
		m_StarHawksTransfromIt = GetComponentIterator<Transform3D>();
	}

	void StarhawksBlastSystem::OnInit()
	{
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		StarhawksBlast* starhawksBlast = GetComponentByIterator<StarhawksBlast>(m_StarHawksIt);
		starhawksBlast->damage = 5;
		EntityId player = GameStateManager::GetPlayerId();

		m_PlayerTransformIt = em.GetComponentIterator<Transform3D>(player);

		int numGroups = 10;
		int numBulletsPerGroup = 3;
		float degreeStep = 360.0f / numGroups;
		float groupDegreeStep = 10.0f; // The angle between bullets in a group

		
		for (int i = 0; i < numGroups; ++i) {
			for (int j = 0; j < numBulletsPerGroup; ++j) {
				float directionAngle = i * degreeStep + j * groupDegreeStep;
				float radian = directionAngle * (PI / 180.0f); // Convert degree to radian
				float xDir = cos(radian);
				float yDir = sin(radian);

				glm::vec3 direction(xDir, 0.0f, yDir);
				SpawnStarhawkBullet(direction);
			}
		}
		AudioSystem* audio = em.GetSystem<Wiwa::AudioSystem>(m_EntityId);
		if (audio)
			audio->PlayAudio("starhawk");
	}

	void StarhawksBlastSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		StarhawksBlast* starhawksBlast = GetComponentByIterator<StarhawksBlast>(m_StarHawksIt);

		m_Timer += Time::GetDeltaTimeSeconds();
		if (m_Timer >= starhawksBlast->lifeTime)
		{
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			em.DestroyEntity(m_EntityId);
		}

	}


	void StarhawksBlastSystem::OnDestroy()
	{

	}

	void StarhawksBlastSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		/*if (body1->id == m_EntityId && strcmp(body2->selfTagStr, "ENEMY_BULLET") == 0)
		{
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			em.DestroyEntity(body2->id);
		}*/
	}

	void StarhawksBlastSystem::SpawnStarhawkBullet(glm::vec3 direction)
	{
		if (GameStateManager::s_PoolManager->s_StarHawksBlastPool->getCountDisabled() <= 0)
			return;

		EntityManager& entityManager = m_Scene->GetEntityManager();
		StarhawksBlast* starhawksBlast = GetComponentByIterator<StarhawksBlast>(m_StarHawksIt);
		starhawksBlast->damage = 20;
		GameStateManager::s_PoolManager->SetScene(m_Scene);
		EntityId newBulletId = EntityManager::INVALID_INDEX;
		newBulletId = GameStateManager::s_PoolManager->s_StarHawksBlastPool->GetFromPool();
		if (newBulletId == EntityManager::INVALID_INDEX)
			return;
		StarhawkBlastBulletSystem* bulletSys = entityManager.GetSystem<StarhawkBlastBulletSystem>(newBulletId);
		PhysicsSystem* physSys = entityManager.GetSystem<PhysicsSystem>(newBulletId);
		physSys->DeleteBody();

		// Set intial positions
		Transform3D* playerTr = GetComponentByIterator<Transform3D>(m_PlayerTransformIt);
		Transform3D* bulletTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(newBulletId));

		if (!bulletTr || !playerTr)
			return;

		bulletTr->localPosition = playerTr->localPosition;
		bulletTr->localRotation = glm::vec3(0.f,direction.y, 0.f);
		bulletTr->localScale = glm::vec3(0.1f, 0.1f, 0.1f);

		SimpleBullet* bullet = (SimpleBullet*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<SimpleBullet>(newBulletId));
	
		bullet->direction = direction;
		bullet->isFromPool = true;
		bullet->damage = starhawksBlast->damage;
		physSys->CreateBody();
		bulletSys->EnableBullet();
	}


}