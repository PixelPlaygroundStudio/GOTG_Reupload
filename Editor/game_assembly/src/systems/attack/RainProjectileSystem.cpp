#include "RainProjectileSystem.h"
#include "../../components/attack/RainProjectile.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include "Wiwa/ecs/systems/ParticleSystem.h"
#include <Wiwa/utilities/EntityPool.h>

Wiwa::RainProjectileSystem::RainProjectileSystem()
{
	m_BulletIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
	m_Timer = 0.0f;
	m_TimerCircularThunder = 0.0f;
	m_DamageHasBeenApplied = false;
	m_BulletSpeedStored = 1.0f;
}

Wiwa::RainProjectileSystem::~RainProjectileSystem()
{
}

void Wiwa::RainProjectileSystem::OnAwake()
{
	m_BulletIt = GetComponentIterator<RainProjectile>();
}

void Wiwa::RainProjectileSystem::OnInit()
{	
}

void Wiwa::RainProjectileSystem::InitRainProjectileBullet()
{
	if (!getAwake())
		System::Awake();

	RainProjectile* bullet = GetComponentByIterator<RainProjectile>(m_BulletIt);

	Wiwa::EntityManager& em = m_Scene->GetEntityManager();
	Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

	Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();

	physicsManager.SetVelocity(obj, glm::normalize(bullet->direction) * bullet->velocity);

}

void Wiwa::RainProjectileSystem::OnUpdate()
{
	if (!getAwake())
		System::Awake();
	if (!getInit())
		System::Init();

	RainProjectile* bullet = GetComponentByIterator<RainProjectile>(m_BulletIt);
	Wiwa::EntityManager& em = m_Scene->GetEntityManager();

	if (m_EntityId == EntityManager::INVALID_INDEX)
	{
		return;
	}

	Transform3D* transform = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(m_EntityId));

	if (transform == nullptr)
	{
		return;
	}
	
	m_Timer += Time::GetDeltaTimeSeconds();
	m_TimerCircularThunder += Time::GetDeltaTimeSeconds();

	if (m_CircularThunder)
	{
		/*bullet->velocity = 30.0f;*/

		if (m_SpawnCircularThunder == false)
		{
			//emit left muzzle
			EntityId rainproj = em.GetChildByName(m_EntityId, "p_boss_rain_projectile_01");
			EntityId layer0 = em.GetChildByName(rainproj, "p_boss_laser_layer00");
			EntityId layer1 = em.GetChildByName(rainproj, "p_boss_laser_layer01");
			EntityId core = em.GetChildByName(rainproj, "bullet_core");
			EntityId ground = em.GetChildByName(rainproj, "p_ground");
			EntityId sphere = em.GetChildByName(rainproj, "p_sphere");

			ParticleSystem* p_sys = nullptr;


			if (layer0 != EntityManager::INVALID_INDEX)
				p_sys = em.GetSystem<ParticleSystem>(layer0);
			if (p_sys != nullptr) p_sys->RestartEmitter(0.1f); else WI_CORE_ERROR("not found layer0");

			if (layer1 != EntityManager::INVALID_INDEX)
				p_sys = em.GetSystem<ParticleSystem>(layer1);
			if (p_sys != nullptr) p_sys->RestartEmitter(0.1f); else WI_CORE_ERROR("not found layer1");

			if (core != EntityManager::INVALID_INDEX)
				p_sys = em.GetSystem<ParticleSystem>(core);
			if (p_sys != nullptr) p_sys->RestartEmitter(0.1f); else WI_CORE_ERROR("not found core");

			if (ground != EntityManager::INVALID_INDEX)
				p_sys = em.GetSystem<ParticleSystem>(ground);
			if (p_sys != nullptr) p_sys->RestartEmitter(0.1f); else WI_CORE_ERROR("not found ground");

			if (sphere != EntityManager::INVALID_INDEX)
				p_sys = em.GetSystem<ParticleSystem>(sphere);
			if (p_sys != nullptr) p_sys->RestartEmitter(0.1f); else WI_CORE_ERROR("not found sphere");

			m_SpawnCircularThunder = true;
		}

		//----------------------

		float startingAngleRadians = glm::radians(m_StartingRotationPoint);
		
		// Calculate the angle for the position
		float angle = ROTATION_SPEED * static_cast<float>(m_TimerCircularThunder) * (2.0f * glm::pi<float>()) + startingAngleRadians;

		// Calculate the raw x and z coordinates for the position
		transform->localPosition.x = m_CenterPoint.x + m_RotationRadius * std::cos(angle);
		transform->localPosition.y = 0.1f;
		transform->localPosition.z = m_CenterPoint.z + m_RotationRadius * std::sin(angle);

		//------------------------

		/*if (m_CircualrThunderId != EntityManager::INVALID_INDEX)
		{
			Transform3D* p_laserT = em.GetComponent<Transform3D>(m_CircualrThunderId);

			if (p_laserT != nullptr)
			{
				p_laserT->localPosition = transform->localPosition;
			}
		}*/

		if (m_Timer >= bullet->lifeTime)
		{
			
			GameStateManager::s_PoolManager->s_RainProjectilePool->ReturnToPool(m_EntityId);
		}
	}
	else
	{
		if (transform->localPosition.y <= 0.08f)
		{
			transform->localPosition.y = 0.08f;
		}

		if (m_Timer >= bullet->lifeTime)
		{
			/*bullet->lifeTime -= RAIN_BULLET_WAITINGTIME;*/
			GameStateManager::s_PoolManager->s_RainProjectilePool->ReturnToPool(m_EntityId);
		}
	}

	//if (transform->localPosition.y <= 0.08f)
	//{
	//	transform->localPosition.y = 0.08f;
	//}

	//if (m_Timer >= bullet->lifeTime)
	//{
	//	/*bullet->lifeTime -= RAIN_BULLET_WAITINGTIME;*/
	//	GameStateManager::s_PoolManager->s_RainProjectilePool->ReturnToPool(m_EntityId);
	//}
}

void Wiwa::RainProjectileSystem::OnDestroy()
{
}

void Wiwa::RainProjectileSystem::OnCollisionEnter(Object* body1, Object* body2)
{
	if (body1->id == m_EntityId)
	{
		if (m_DamageHasBeenApplied == false)
		{
			std::string playerStr = "PLAYER";
			if (playerStr == body2->selfTagStr)
			{

				RainProjectile* bullet = GetComponentByIterator<RainProjectile>(m_BulletIt);
				GameStateManager::DamagePlayer(bullet->damage);

				m_DamageHasBeenApplied = true;
			}
		}	
		
		std::string wallStr = "WALL";
		if (wallStr == body2->selfTagStr)
		{
			GameStateManager::s_PoolManager->s_RainProjectilePool->ReturnToPool(m_EntityId);
		}

	}
}

bool Wiwa::RainProjectileSystem::EnableBullet()
{
	RainProjectile* rainProjectile = GetComponent<RainProjectile>();
	if (rainProjectile)
	{
		m_CircularThunder = false;

		InitRainProjectileBullet();
	}

	return true;
}

bool Wiwa::RainProjectileSystem::EnableCircularThunder(BossUltron* enemy, glm::vec3 centerPoint, float startingPoint, float rotationRadius)
{
	RainProjectile* rainProjectile = GetComponent<RainProjectile>();
	if (rainProjectile)
	{
		m_CircularThunder = true;

		InitRainProjectileBullet();
	}

	BossUltronEnemy = enemy;
	m_CenterPoint = centerPoint;
	m_StartingRotationPoint = startingPoint;
	m_RotationRadius = rotationRadius;

	return true;
}

bool Wiwa::RainProjectileSystem::OnDisabledFromPool()
{
	Transform3D* transform = GetComponent<Transform3D>();
	if (transform)
	{
		transform->localPosition.y = 4100.0f;
		//WI_INFO("transform local pos");
	}

	Wiwa::EntityManager& em = m_Scene->GetEntityManager();
	//WI_INFO("em");
	PhysicsSystem* physSystem = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId);
	//WI_INFO("physsys");
	physSystem->DeleteBody();
	//WI_INFO("delete body");

	m_Timer = 0.0f;
	m_TimerCircularThunder = 0.0f;
	m_DamageHasBeenApplied = false;
	m_SpawnCircularThunder = false;

	return true;
}
