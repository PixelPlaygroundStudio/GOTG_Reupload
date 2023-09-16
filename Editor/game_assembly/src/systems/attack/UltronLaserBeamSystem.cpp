#include "UltronLaserBeamSystem.h"
#include "../../components/attack/UltronLaserBeam.h"
#include "../../systems/enemy/Ultron/StateMachine/BossUltronLaserBeamAttack.h"
#include "Wiwa/ecs/systems/ParticleSystem.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/utilities/EntityPool.h>

Wiwa::UltronLaserBeamSystem::UltronLaserBeamSystem()
{
	m_LaserIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
	m_Timer = 0.0f;
}

Wiwa::UltronLaserBeamSystem::~UltronLaserBeamSystem()
{
}

void Wiwa::UltronLaserBeamSystem::OnAwake()
{
	Wiwa::EntityManager& em = m_Scene->GetEntityManager();

	m_LaserIt = GetComponentIterator<UltronLaserBeam>();
	EntityId boss = em.GetEntityByName("Ultron_01"); // TODO: Change this to get the boss entity ID from the GameStatemanager

	if (boss != WI_INVALID_INDEX)
	{
		m_BossTransformIt = em.GetComponentIterator<Transform3D>(boss);
	}
	
	EntityId player = GameStateManager::GetPlayerId();
	m_PlayerTransformIt = em.GetComponentIterator<Transform3D>(player);
}

void Wiwa::UltronLaserBeamSystem::OnInit()
{

}

void Wiwa::UltronLaserBeamSystem::OnUpdate()
{
	if (!getAwake())
		System::Awake();
	if (!getInit())
		System::Init();

	UltronLaserBeam* bullet = GetComponentByIterator<UltronLaserBeam>(m_LaserIt);
	Transform3D* laserTr = GetComponent<Transform3D>();
	Transform3D* enemyTr = GetComponentByIterator<Transform3D>(m_BossTransformIt);

	Transform3D* playerTr = GetComponentByIterator<Transform3D>(m_PlayerTransformIt);
	
	if (bullet && enemyTr && playerTr)
	{

		if (m_Timer >= bullet->lifeTime)
		{
			GameStateManager::s_PoolManager->s_UltronLaserBeamPool->ReturnToPool(m_EntityId);
		}

		// Update the laser's position to be in front of the boss
		float distanceInFront = 1.0f;
		float laserHalfLength = 20.0f; // Adjust this value based on the actual half length of the laser
		glm::vec3 bossForward = Math::CalculateForward(enemyTr);
		laserTr->localPosition = enemyTr->localPosition + (bossForward * (distanceInFront + laserHalfLength));
		laserTr->localPosition.y += 2.5f;

		// Update the boss's rotation to face the player
		glm::vec3 directionToPlayer = glm::normalize(Math::CalculateForward(enemyTr) - enemyTr->localPosition);
		float angleToPlayer = glm::degrees(atan2(directionToPlayer.x, directionToPlayer.z));
		enemyTr->localRotation.y = angleToPlayer;

		// Update the laser's rotation to match the boss's rotation
		laserTr->localRotation = glm::vec3(-90.0f, angleToPlayer, 0.0f);

		m_Timer += Time::GetDeltaTimeSeconds();
		m_TimerDamagePerSec += Time::GetDeltaTimeSeconds();
	}
	else
	{
		if (bullet == nullptr) WI_CORE_ERROR("No m_LaserIt UltronLaserBeam");
		if (enemyTr == nullptr) WI_CORE_ERROR("No m_BossTransformIt Transform3D");
		if (playerTr == nullptr) WI_CORE_ERROR("No m_PlayerTransformIt Transform3D");
	}
	
}

void Wiwa::UltronLaserBeamSystem::OnDestroy()
{
}

void Wiwa::UltronLaserBeamSystem::OnCollision(Object* body1, Object* body2)
{
	std::string playerStr = "PLAYER";

	if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
	{	
		if (m_TimerDamagePerSec >= 1.0f)
		{
			UltronLaserBeam* bullet = GetComponentByIterator<UltronLaserBeam>(m_LaserIt);
			GameStateManager::DamagePlayer(bullet->damagePerSecond);
			m_TimerDamagePerSec = 0.0f;
		}
	}
}

void Wiwa::UltronLaserBeamSystem::InitLaserBeam()
{
	if (!getAwake())
		System::Awake();
	
	/*GameStateManager::s_PoolManager->SetScene(m_Scene);
	GameStateManager::s_PoolManager->s_SimpleBulletsPool->GetFromPool();*/

	//UltronLaserBeamSystem* bullet = GetComponentByIterator<UltronLaserBeamSystem>(m_LaserIt);
	//Wiwa::EntityManager& em = m_Scene->GetEntityManager();
	//Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId)->getBody();
	//Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

	//physicsManager.SetVelocity(obj, glm::normalize(bullet->direction) * bullet->velocity);
	//Wiwa::EntityManager& em = m_Scene->GetEntityManager();

	//PhysicsSystem* physSystem = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId);

	//if (physSystem)
	//{
	//	physSystem->DeactivateBody();
	//}
}


bool Wiwa::UltronLaserBeamSystem::EnableLaser()
{
	UltronLaserBeamSystem* bullet = GetComponent<UltronLaserBeamSystem>();
	if (bullet)
	{
		bullet->InitLaserBeam();
	}
	m_Timer = 0.0f;
	//WI_CORE_INFO("LASER ACTIVATED WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW");
	//WI_CORE_INFO("Laser ID: {0}", (int)m_EntityId);

	Wiwa::EntityManager& em = m_Scene->GetEntityManager();


	EntityId p_boss_laser_final = em.GetChildByName(m_EntityId, "p_boss_laser_final");
	//WI_CORE_INFO("p_boss_laser_final ID: {0}", (int)p_boss_laser_final);

	if (p_boss_laser_final != EntityManager::INVALID_INDEX)
	{
		EntityId p_boss_laser_layer00 = em.GetChildByName(p_boss_laser_final, "p_boss_laser_layer00");
		//WI_CORE_INFO("p_boss_laser_layer00 ID: {0}", (int)p_boss_laser_layer00);

		EntityId p_boss_laser_layer01 = em.GetChildByName(p_boss_laser_final, "p_boss_laser_layer01");
		//WI_CORE_INFO("p_boss_laser_layer01 ID: {0}", (int)p_boss_laser_layer01);


		if (p_boss_laser_layer00 != EntityManager::INVALID_INDEX)
		{
			ParticleSystem* p_boss_laser_layer00_Sys = em.GetSystem<ParticleSystem>(p_boss_laser_layer00);
			p_boss_laser_layer00_Sys->SetTimer(0);
			p_boss_laser_layer00_Sys->DeactivateParticles();
		}
		
		if (p_boss_laser_layer01 != EntityManager::INVALID_INDEX)
		{
			ParticleSystem* p_boss_laser_layer01_Sys = em.GetSystem<ParticleSystem>(p_boss_laser_layer00);
			p_boss_laser_layer01_Sys->SetTimer(0);
			p_boss_laser_layer01_Sys->DeactivateParticles();
		}


		EntityId p_boss_laser_hit = em.GetChildByName(m_EntityId, "p_boss_laser_hit");

		if (p_boss_laser_hit != EntityManager::INVALID_INDEX)
		{
			EntityId p_bullet_plasma = em.GetChildByName(p_boss_laser_hit, "bullet_plasma");
			EntityId p_bullet_core = em.GetChildByName(p_boss_laser_hit, "bullet_core");
			EntityId p_bullet_light00 = em.GetChildByName(p_boss_laser_hit, "bullet_light00");
			EntityId p_bullet_light01 = em.GetChildByName(p_boss_laser_hit, "bullet_light01");
		}
	}
	
	
	return true;
}

bool Wiwa::UltronLaserBeamSystem::OnDisabledFromPool()
{
	Transform3D* transform = GetComponent<Transform3D>();
	if (transform)
	{
		transform->localPosition.y = 5000.0f;
	}

	//UltronLaserBeam* laserBeam = GetComponent<UltronLaserBeam>();

	Wiwa::EntityManager& em = m_Scene->GetEntityManager();

	PhysicsSystem* physSystem = em.GetSystem<Wiwa::PhysicsSystem>(m_EntityId);

	physSystem->DeleteBody();

	m_Timer = 0.0f;

	return true;
}
