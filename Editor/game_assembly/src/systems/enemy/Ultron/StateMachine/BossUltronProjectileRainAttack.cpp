#include <wipch.h>
#include "BossUltronProjectileRainAttack.h"
#include "../../../../components/attack/RainProjectile.h"
#include "../../../attack/RainProjectileSystem.h"
#include "../BossUltron.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ParticleSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

Wiwa::BossUltronProjectileRainAttackState::BossUltronProjectileRainAttackState()
{
	m_ThunderMarkPath = "assets\\Enemy\\RainProjectile\\p_boss_storm_marker.wiprefab";
}

Wiwa::BossUltronProjectileRainAttackState::~BossUltronProjectileRainAttackState()
{

}

void Wiwa::BossUltronProjectileRainAttackState::EnterState(BossUltron* enemy)
{
	m_TimerRain = 0.0f;
	m_RainProjectileCounter = 0;
	m_RainState = ProjectileRainState::PREPARE_RAIN;
	Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();

	
	NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);
	if (navAgent)
	{
		navAgent->autoRotate = true;
	}

	//EntityId currentEnemy = enemy->GetEntity();

	//pman.EmitBatch(currentEnemy);
}

void Wiwa::BossUltronProjectileRainAttackState::UpdateState(BossUltron* enemy)
{
	Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();	

	switch (m_RainState)
	{
	case ProjectileRainState::PREPARE_RAIN:
	{
		//WI_INFO("Prepare rain");

		//Posar anim de preparacio

		/*enemy->m_AnimatorSys->PlayAnimation("fiveshot_anticipation");*/
		enemy->m_AnimatorSys->PlayAnimation("bulletrain_together");

		enemy->m_NavAgentSys->StopAgent();

		m_RainState = ProjectileRainState::RAIN_ATTACK;
		m_TimerRain = 0.0f;
		m_RainProjectileCounter = 0;
		m_TimerThundersSpawn = 0.0f;
		m_TimerThundersMark = 0.0f;

	}break;
	case ProjectileRainState::RAIN_ATTACK:
	{
		m_TimerThundersSpawn += Time::GetDeltaTimeSeconds();
		m_TimerThundersMark += Time::GetDeltaTimeSeconds();

		if (m_TimerThundersSpawn >= 0.1f)
		{
			m_TimerRain = 0.0f;

			if (!m_RandomPositionSetted)
			{
				for (int i = 1; i <= NUMBER_OF_THUNDERS; ++i) {
					EntityId thunderMarkId = em.LoadPrefab(m_ThunderMarkPath);
					m_ThunderMarkIds.push_back(thunderMarkId); // Add the ID to the vector
					Transform3D* thunderMarkTr = em.GetComponent<Transform3D>(thunderMarkId);
					glm::vec3 thunderPos = RandomPoint();
					m_ThunderPositions.push_back(thunderPos); // Add the position to the vector
					thunderMarkTr->localPosition.x = thunderPos.x;
					thunderMarkTr->localPosition.y = 0.1f;
					thunderMarkTr->localPosition.z = thunderPos.z;
				}

				m_RandomPositionSetted = true;
			}

			if (m_TimerThundersMark >= 1.1f)
			{
				/*enemy->m_AnimatorSys->PlayAnimation("fiveshot_attack");*/
				enemy->m_AnimatorSys->PlayAnimation("bulletrain_together");

				for (int i = 0; i < m_ThunderMarkIds.size(); ++i) {
					em.DestroyEntity(m_ThunderMarkIds[i]);
					SpawnThunderStorm(enemy, m_ThunderPositions[i], { 0.0f, -1.0f, 0.0f });
				}

				// Clear the vectors
				m_ThunderMarkIds.clear();
				m_ThunderPositions.clear();
				

				m_TimerThundersMark = 0.0f;
				m_TimerThundersSpawn = 0.0f;
				m_RandomPositionSetted = false;
				m_RainProjectileCounter++;
			}
		}

		if (NUMBER_WAVES <= m_RainProjectileCounter)
		{
			m_PreviousPoints.clear();
			m_RainState = ProjectileRainState::END_STATE;
		}


	}break;
	case ProjectileRainState::END_STATE:
	{
		//WI_INFO("End rain");
		enemy->SwitchState(enemy->m_MovementState);

	}break;
	}	
	m_TimerRain += Time::GetDeltaTimeSeconds();
}

void Wiwa::BossUltronProjectileRainAttackState::ExitState(BossUltron* enemy)
{
}

glm::vec3 Wiwa::BossUltronProjectileRainAttackState::GetRandomPositionInRange(const glm::vec3& position, float range)
{
	float theta = 2.0f * PI_F * (float(RANDF(0.0f, RAND_MAX)) / float(RAND_MAX));
	float r = range * std::sqrt(float(RANDF(0.0f, RAND_MAX)) / float(RAND_MAX));

	glm::vec3 randomOffset;
	randomOffset.x = r * std::cos(theta);
	randomOffset.z = r * std::sin(theta);
	randomOffset.y = 0.0f;

	return position + randomOffset;
}

void Wiwa::BossUltronProjectileRainAttackState::SpawnThunderStorm(BossUltron* enemy, glm::vec3 thunderPosition, const glm::vec3& bull_dir)
{
	if (GameStateManager::s_PoolManager->s_RainProjectilePool->getCountDisabled() <= 0)
		return;

	Wiwa::EntityManager& entityManager = enemy->getScene().GetEntityManager();
	GameStateManager::s_PoolManager->SetScene(&enemy->getScene());
	EntityId newBulletId = EntityManager::INVALID_INDEX;
	newBulletId = GameStateManager::s_PoolManager->s_RainProjectilePool->GetFromPool();

	if (newBulletId == EntityManager::INVALID_INDEX)
		return;

	RainProjectileSystem* bulletSys = entityManager.GetSystem<RainProjectileSystem>(newBulletId);
	PhysicsSystem* physSys = entityManager.GetSystem<PhysicsSystem>(newBulletId);
	physSys->DeleteBody();

	// Set intial positions
	Transform3D* playerTr = (Transform3D*)entityManager.GetComponentByIterator(enemy->m_PlayerTransformIt);
	Transform3D* bulletTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(newBulletId));

	if (!bulletTr || !playerTr)
		return;

	bulletTr->localPosition = thunderPosition;
	bulletTr->localRotation = glm::vec3(-90.0f, 0.0f, playerTr->localRotation.y + 90.0f);
	RainProjectile* bullet = (RainProjectile*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<RainProjectile>(newBulletId));
	BossUltron* ultron = (BossUltron*)entityManager.GetComponentByIterator(enemy->m_Ultron);

	bullet->direction = bull_dir;

	physSys->CreateBody();

	bulletSys->EnableBullet();

	//emit left muzzle
	EntityId rainproj = entityManager.GetChildByName(newBulletId, "p_boss_rain_projectile_01");
	EntityId layer0 = entityManager.GetChildByName(rainproj, "p_boss_laser_layer00");
	EntityId layer1 = entityManager.GetChildByName(rainproj, "p_boss_laser_layer01");
	EntityId core = entityManager.GetChildByName(rainproj, "bullet_core");
	EntityId ground = entityManager.GetChildByName(rainproj, "p_ground");
	EntityId sphere = entityManager.GetChildByName(rainproj, "p_sphere");

	ParticleSystem* p_sys = nullptr;


	if (layer0 != EntityManager::INVALID_INDEX)
		p_sys = entityManager.GetSystem<ParticleSystem>(layer0);
	if (p_sys != nullptr) p_sys->RestartEmitter(0.1f); else WI_CORE_ERROR("not found layer0");

	if (layer1 != EntityManager::INVALID_INDEX)
		p_sys = entityManager.GetSystem<ParticleSystem>(layer1);
	if (p_sys != nullptr) p_sys->RestartEmitter(0.1f); else WI_CORE_ERROR("not found layer1");

	if (core != EntityManager::INVALID_INDEX)
		p_sys = entityManager.GetSystem<ParticleSystem>(core);
	if (p_sys != nullptr) p_sys->RestartEmitter(0.1f); else WI_CORE_ERROR("not found core");

	if (ground != EntityManager::INVALID_INDEX)
		p_sys = entityManager.GetSystem<ParticleSystem>(ground);
	if (p_sys != nullptr) p_sys->RestartEmitter(0.1f); else WI_CORE_ERROR("not found ground");

	if (sphere != EntityManager::INVALID_INDEX)
		p_sys = entityManager.GetSystem<ParticleSystem>(sphere);
	if (p_sys != nullptr) p_sys->RestartEmitter(0.1f); else WI_CORE_ERROR("not found sphere");
	

}

glm::vec3 Wiwa::BossUltronProjectileRainAttackState::RandomPoint()
{
	// Square dimensions
	float minX = -29.10f;
	float maxX = 29.10f;
	float minZ = -34.58f;
	float maxZ = 34.58f;

	// Create a random number generator
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> distributionX(minX, maxX);
	std::uniform_real_distribution<float> distributionZ(minZ, maxZ);

	while (true)
	{
		// Generate random coordinates within the square
		float x = distributionX(generator);
		float z = distributionZ(generator);

		// Check if the point is at least 8.0f away from all previously selected points
		bool isValidPoint = true;
		for (const glm::vec3& prevPoint : m_PreviousPoints)
		{
			if (glm::distance(glm::vec2(x, z), glm::vec2(prevPoint.x, prevPoint.z)) < THUNDERS_SPAWN_OFFSET)
			{
				isValidPoint = false;
				break;
			}
		}

		if (isValidPoint)
		{
			m_PreviousPoints.push_back(glm::vec3(x, ALTITUDE_THUNDERSTORM, z));
			return glm::vec3(x, ALTITUDE_THUNDERSTORM, z);
		}
			
	}
}