#include "ClusterBulletSystem.h"
#include "../../components/attack/ClusterBullet.h"
#include "../../systems/attack/SimpleBulletSystem.h"
#include "../../components/attack/SimpleBullet.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/utilities/EntityPool.h>
#include <Wiwa/ecs/systems/AudioSystem.h>

Wiwa::ClusterBulletSystem::ClusterBulletSystem()
{
	m_BulletIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
	m_Timer = 0.0f;
	m_CollisionByWall = false;
	m_NotCollidingPlayer = true;
}

Wiwa::ClusterBulletSystem::~ClusterBulletSystem()
{
	
}

void Wiwa::ClusterBulletSystem::OnAwake()
{
	m_BulletIt = GetComponentIterator<ClusterBullet>();	
}

void Wiwa::ClusterBulletSystem::OnInit()
{
	
}

void Wiwa::ClusterBulletSystem::InitClusterBullet()
{
	if (!getAwake())
		System::Awake();

	ClusterBullet* bullet = GetComponentByIterator<ClusterBullet>(m_BulletIt);

	//bullet->damage = CLUSTER_BULLET_DAMAGE;
	//bullet->lifeTime = CLUSTER_BULLET_LIFETIME;

	Wiwa::EntityManager& em = m_Scene->GetEntityManager();
	Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

	Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();

	physicsManager.SetVelocity(obj, glm::normalize(bullet->direction) * bullet->velocity);

	m_HasBlown = false;
	m_CollisionByWall = false;
	m_NotCollidingPlayer = true;
}

void Wiwa::ClusterBulletSystem::OnUpdate()
{
	if (!getAwake())
		System::Awake();
	if (!getInit())
		System::Init();

	ClusterBullet* bullet = GetComponentByIterator<ClusterBullet>(m_BulletIt);

	m_Timer += Time::GetDeltaTimeSeconds();



	if (m_Timer >= bullet->lifeTime)
	{

		int randomValue = Math::RandomRange(0, 1);

		if (randomValue == 0)
		{
			BossUltronCluster->m_AudioSys->PlayAudio("boss_cluster_ball_exp");
			BlowClusterBullet01(m_EntityId);
		}
		else
		{
			BossUltronCluster->m_AudioSys->PlayAudio("boss_cluster_ball_exp");
			BlowClusterBullet02(m_EntityId);
		}

		GameStateManager::s_PoolManager->s_ClusterBulletsPool->ReturnToPool(m_EntityId);
	}

	if (m_CollisionByWall)
	{
		int pattern = Math::RandomRange(0, 1);
		if (pattern == 0)
		{
			BossUltronCluster->m_AudioSys->PlayAudio("boss_cluster_ball_exp");
			BlowClusterBullet01(m_EntityId);
		}
		else
		{
			BossUltronCluster->m_AudioSys->PlayAudio("boss_cluster_ball_exp");
			BlowClusterBullet02(m_EntityId);
		}
		GameStateManager::s_PoolManager->s_ClusterBulletsPool->ReturnToPool(m_EntityId);

		m_CollisionByWall = false;
	}
}

void Wiwa::ClusterBulletSystem::OnDestroy()
{
}

void Wiwa::ClusterBulletSystem::OnCollisionEnter(Object* body1, Object* body2)
{
	if (body1->id == m_EntityId)
	{
		std::string playerStr = "PLAYER";
		if (playerStr == body2->selfTagStr)
		{
			Wiwa::EntityManager& em = getScene().GetEntityManager();

			m_NotCollidingPlayer = false;
			ClusterBullet* bullet = GetComponentByIterator<ClusterBullet>(m_BulletIt);
			GameStateManager::DamagePlayer(bullet->damage);

			EntityId clusterHitPlayer = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_cluster_hit.wiprefab");

			if (clusterHitPlayer != EntityManager::INVALID_INDEX)
			{
				Transform3D* clusterHitPlayerTr = em.GetComponent<Transform3D>(clusterHitPlayer);
				Transform3D* bulletPlayerTr = em.GetComponent<Transform3D>(m_EntityId);

				if (clusterHitPlayerTr != nullptr)
				{
					clusterHitPlayerTr->localPosition = bulletPlayerTr->localPosition;
				}
			}

			GameStateManager::s_PoolManager->s_ClusterBulletsPool->ReturnToPool(m_EntityId);
		}
		else
		{
			Wiwa::EntityManager& em = getScene().GetEntityManager();

			EntityId clusterHitPlayer = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_cluster_hit.wiprefab");

			if (clusterHitPlayer != EntityManager::INVALID_INDEX)
			{
				Transform3D* clusterHitPlayerTr = em.GetComponent<Transform3D>(clusterHitPlayer);
				Transform3D* bulletPlayerTr = em.GetComponent<Transform3D>(m_EntityId);

				if (clusterHitPlayerTr != nullptr)
				{
					clusterHitPlayerTr->localPosition = bulletPlayerTr->localPosition;
				}
			}

			m_CollisionByWall = true;
		}		
	}
}

void Wiwa::ClusterBulletSystem::SpawnBullet(const glm::vec3& bull_dir, EntityId simpleBullet)
{
	if (GameStateManager::s_PoolManager->s_SimpleBulletsPool->getCountDisabled() <= 0)
		return;

	Wiwa::EntityManager& entityManager = this->getScene().GetEntityManager();
	EntityId newBulletId = GameStateManager::s_PoolManager->s_SimpleBulletsPool->GetFromPool();
	//entityManager.RemoveSystem(newBulletId, physicsSystemHash);
	PhysicsSystem* physSys = entityManager.GetSystem<PhysicsSystem>(newBulletId);

	if (physSys != nullptr)
	{
		physSys->DeleteBody();
	}

	// Set intial positions
	Transform3D* bulletTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(newBulletId));
	Transform3D* clusterBulletTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(simpleBullet));


	if (!clusterBulletTr || !bulletTr)
		return;

	bulletTr->localPosition = clusterBulletTr->localPosition;
	bulletTr->localRotation = glm::vec3(-90.0f, 0.0f, bull_dir.y + 90.0f);
	bulletTr->localScale = {1.0f,1.0f,1.0f};
	SimpleBullet* bullet = (SimpleBullet*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<SimpleBullet>(newBulletId));
	bullet->direction = bull_dir;
	bullet->damage = 20; //TODO: Add to components
	bullet->isFromPool = true;

	Wiwa::SimpleBulletSystem* simpleBulletSystem = entityManager.GetSystem<Wiwa::SimpleBulletSystem>(newBulletId);

	physSys->CreateBody();

	simpleBulletSystem->EnableBullet();
}

void Wiwa::ClusterBulletSystem::BlowClusterBullet01(EntityId bulletId)
{

	Wiwa::EntityManager& entityManager = this->getScene().GetEntityManager();

	Transform3D* bulletTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(bulletId));

	if (m_NotCollidingPlayer)
	{
		int numBullets = LITTLE_BULLETS_NUMBER;
		float degreeStep = 360.0f / numBullets;

		for (int i = 0; i < numBullets; ++i) {
			float directionAngle = i * degreeStep;
			float radian = directionAngle * (PI / 180.0f); // Convert degree to radian
			float xDir = cos(radian);
			float yDir = sin(radian);

			glm::vec3 direction(xDir, 0.0f, yDir);
			this->SpawnBullet(direction, bulletId);
		}

		m_HasBlown = true;
	}
}

void Wiwa::ClusterBulletSystem::BlowClusterBullet02(EntityId bulletId)
{

	Wiwa::EntityManager& entityManager = this->getScene().GetEntityManager();

	Transform3D* bulletTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(bulletId));

	int numGroups = LITTLE_BULLETS_NUMBER;
	int numBulletsPerGroup = 3;
	float degreeStep = 360.0f / numGroups;
	float groupDegreeStep = 10.0f; // The angle between bullets in a group

	if (m_NotCollidingPlayer)
	{
		for (int i = 0; i < numGroups; ++i) {
			for (int j = 0; j < numBulletsPerGroup; ++j) {
				float directionAngle = i * degreeStep + j * groupDegreeStep;
				float radian = directionAngle * (PI / 180.0f); // Convert degree to radian
				float xDir = cos(radian);
				float yDir = sin(radian);

				glm::vec3 direction(xDir, 0.0f, yDir);
				SpawnBullet(direction, bulletId);
			}
		}

		m_HasBlown = true;
	}
}

bool Wiwa::ClusterBulletSystem::EnableBullet(BossUltron* enemy)
{
	ClusterBullet* clusterBullet = GetComponent<ClusterBullet>();
	if (clusterBullet)
	{
		InitClusterBullet();
	}

	BossUltronCluster = enemy;

	return true;
}

bool Wiwa::ClusterBulletSystem::OnDisabledFromPool()
{
	if (m_HasBlown == false)
	{
		int pattern = RAND(1, 2);

		if (pattern == 1)
		{
			BlowClusterBullet01(m_EntityId);
		}
		else
		{
			BlowClusterBullet02(m_EntityId);
		}
		
		m_HasBlown = true;
	}

	Transform3D* transform = GetComponent<Transform3D>();
	if (transform)
	{
		transform->localPosition.y = 4000.0f;
	}

	Wiwa::EntityManager& em = m_Scene->GetEntityManager();

	PhysicsSystem* physSystem = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId);

	physSystem->DeleteBody();

	m_Timer = 0.0f;

	return true;
}
