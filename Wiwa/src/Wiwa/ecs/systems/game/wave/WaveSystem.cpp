#include <wipch.h>
#include "WaveSystem.h"
#include <Wiwa/ecs/systems/PhysicsSystem.h>
#include <Wiwa/ecs/components/game/enemy/Enemy.h>
#include <Wiwa/utilities/EntityPool.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

#include <random>
#include <Wiwa/ecs/components/game/wave/Wave.h>
#include <Wiwa/ecs/components/game/wave/WaveSpawner.h>
#include <Wiwa/ecs/components/game/wave/WaveSpawnPoint.h>
#include <Wiwa/audio/Audio.h>

namespace Wiwa
{
	WaveSystem::WaveSystem()
	{
		m_WaveIt = {WI_INVALID_INDEX, WI_INVALID_INDEX};
		m_SpawnerIt = {WI_INVALID_INDEX, WI_INVALID_INDEX};
		m_PointIndex = 0;
		m_TotalEnemiesSpawned = 0;
		m_HasTriggered = false;
		m_HasSetSpawnParticle = false;
	}

	WaveSystem::~WaveSystem()
	{
	}

	void WaveSystem::OnAwake()
	{
		physicsSystemHash = FNV1A_HASH("PhysicsSystem");
	}

	void WaveSystem::OnInit()
	{
		m_WaveIt = GetComponentIterator<Wave>();
		Wave *wave = GetComponentByIterator<Wave>(m_WaveIt);

		WaveSpawner* spawner = GetComponentByIterator<WaveSpawner>(m_SpawnerIt);
		GetSpawnPoints(m_Points);
		Wiwa::GameStateManager::IncrementCurrentWave();
		Audio::PostEvent("wave_start");
	}

	void WaveSystem::OnUpdate()
	{
		if (!getAwake() && !getInit())
			return;
		EntityManager& em = GetEntityManager();

		m_Timer += Time::GetDeltaTimeSeconds();
		m_TimerSpawnParticle += Time::GetDeltaTimeSeconds();

		Wave* wave = GetComponentByIterator<Wave>(m_WaveIt);
		// Wave has finished
		if (wave->currentEnemiesAlive <= 0.0f && m_HasTriggered && m_EnemiesIds.size() <= 0 && m_TotalEnemiesSpawned >= wave->maxEnemies)
		{
			wave->hasFinished = true;
		}

		// Define the range for the random number generation
		int min = -5;
		int max = 5;
		disSpawn.param(std::uniform_int_distribution<int>::param_type{ min, max });

		if (m_TimerSpawnParticle >= wave->enemySpawnRate - 4.0f && !m_HasSetSpawnParticle)
		{
			m_xRand = disSpawn(Application::s_Gen);
			m_zRand = disSpawn(Application::s_Gen);
			enemyRandSelection = GetEnemyFromProbabiliteis();

			if (m_PointIndex >= m_Points.size())
				m_PointIndex = 0;

			m_TimerSpawnParticle = 0.0f;
			m_HasSetSpawnParticle = true;

			EntityId spawnerParticle = EntityManager::INVALID_INDEX;

			if (enemyRandSelection == PHALANX_MELEE_GENERIC || enemyRandSelection == PHALANX_MELEE_VARIANT_A || enemyRandSelection == PHALANX_MELEE_VARIANT_B)
			{
				spawnerParticle = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\vfx_enemy_spawn_marker\\p_enemy_spawn_PhalanxMelee.wiprefab");
			}
			else if (enemyRandSelection == PHALANX_RANGED_GENERIC || enemyRandSelection == PHALANX_RANGED_VARIANT_A || enemyRandSelection == PHALANX_RANGED_VARIANT_B)
			{
				spawnerParticle = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\vfx_enemy_spawn_marker\\p_enemy_spawn_PhalanxRanged.wiprefab");
			}
			else if (enemyRandSelection == SUBJUGATOR || enemyRandSelection == SUBJUGATOR_CHIEF)
			{
				spawnerParticle = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\vfx_enemy_spawn_marker\\p_enemy_spawn_PhalanxSentinel.wiprefab");
			}
			else if (enemyRandSelection == SENTINEL)
			{
				spawnerParticle = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\vfx_enemy_spawn_marker\\p_enemy_spawn_PhalanxSentinel.wiprefab");
			}

			Transform3D* enemyTransform = em.GetComponent<Transform3D>(spawnerParticle);
			if (enemyTransform)
			{
				enemyTransform->localPosition.x = m_Points[m_PointIndex].x + m_xRand;
				enemyTransform->localPosition.z = m_Points[m_PointIndex].z + m_zRand;
				enemyTransform->localPosition.y = 0;
			}
		}

		if (m_Timer >= wave->enemySpawnRate && m_TotalEnemiesSpawned < wave->maxEnemies)
		{
			m_Timer = 0.0f;

			if (m_PointIndex >= m_Points.size())
				m_PointIndex = 0;

			if (SpawnEnemy(enemyRandSelection, m_Points[m_PointIndex], m_xRand, m_zRand))
			{
				wave->currentEnemiesAlive++;
				m_PointIndex++;
				m_TotalEnemiesSpawned++;
				m_HasTriggered = true;
				m_HasSetSpawnParticle = false;
			}
		}
	}

	void WaveSystem::OnDestroy()
	{
	
	}

	bool WaveSystem::SpawnEnemy(Pool_Type enemy_type, const glm::vec3& spawn_point, int rand_x, int rand_z)
	{
		// Create an enemy from prefab
		Wiwa::EntityManager &entityManager = m_Scene->GetEntityManager();
		EntityId newEnemyId = EntityManager::INVALID_INDEX;

		GameStateManager::s_PoolManager->SetScene(m_Scene);

		switch (enemy_type)
		{
		case Pool_Type::PHALANX_MELEE_GENERIC:
		{
			newEnemyId = GameStateManager::s_PoolManager->s_PhalanxMeleeGenericPool->GetFromPool();
		}
		break;
		case Pool_Type::PHALANX_MELEE_VARIANT_A:
		{
			newEnemyId = GameStateManager::s_PoolManager->s_PhalanxMeleeVariantAPool->GetFromPool();
		}
		break;
		case Pool_Type::PHALANX_MELEE_VARIANT_B:
		{
			newEnemyId = GameStateManager::s_PoolManager->s_PhalanxMeleeVariantBPool->GetFromPool();
		}
		break;
		case Pool_Type::PHALANX_RANGED_GENERIC:
		{
			newEnemyId = GameStateManager::s_PoolManager->s_PhalanxRangedGenericPool->GetFromPool();
		}
		break;
		case Pool_Type::PHALANX_RANGED_VARIANT_A:
		{
			newEnemyId = GameStateManager::s_PoolManager->s_PhalanxRangedVariantAPool->GetFromPool();
		}
		break;
		case Pool_Type::PHALANX_RANGED_VARIANT_B:
		{
			newEnemyId = GameStateManager::s_PoolManager->s_PhalanxRangedVariantBPool->GetFromPool();
		}
		break;
		case Pool_Type::SENTINEL:
		{
			newEnemyId = GameStateManager::s_PoolManager->s_SentinelPool->GetFromPool();
		}
		break;
		case Pool_Type::SUBJUGATOR:
		{
			newEnemyId = GameStateManager::s_PoolManager->s_SubjugatorPool->GetFromPool();
		}
		break;
		case Pool_Type::SUBJUGATOR_CHIEF:
		{
			newEnemyId = GameStateManager::s_PoolManager->s_SubjugatorChiefPool->GetFromPool();
		}
		break;
		default:
			break;
		}

		if (newEnemyId == EntityManager::INVALID_INDEX)
		{
			return false;
		}

		PhysicsSystem* physSys = entityManager.GetSystem<PhysicsSystem>(newEnemyId);
		physSys->DeleteBody();

		Transform3D* enemyTransform = entityManager.GetComponent<Transform3D>(newEnemyId);

		if (!enemyTransform)
			return false;

		enemyTransform->localPosition.x = spawn_point.x + rand_x;
		enemyTransform->localPosition.z = spawn_point.z + rand_z;
		enemyTransform->localPosition.y = 0;

		NavAgentSystem* navAgentSys = entityManager.GetSystem<NavAgentSystem>(newEnemyId);

		//// Set the correspondent tag
		//CollisionBody *collBodyPtr = entityManager.GetComponent<CollisionBody>(newEnemyId);
		//collBodyPtr->selfTag = m_Scene->GetPhysicsManager().GetFilterTag("ENEMY");
		//collBodyPtr->filterBits |= 1 << m_Scene->GetPhysicsManager().GetFilterTag("WALL");
		//collBodyPtr->filterBits |= 1 << m_Scene->GetPhysicsManager().GetFilterTag("PLAYER");

		// Save the enemy component in the list
		EntityManager::ComponentIterator enemyIt = GetComponentIterator<EnemyState>(newEnemyId);
		
		EnemyState* enemy = (EnemyState*)entityManager.GetComponentByIterator(enemyIt);
		enemy->hasFinished = false;
		enemy->enemyType = (int)enemy_type;
		enemy->waveId = (int)m_EntityId;

		navAgentSys->OnInit();
		navAgentSys->SetPosition(enemyTransform->localPosition);
		physSys->CreateBody();

		m_EnemiesIds.emplace_back(newEnemyId);
		return true;
	}

	void WaveSystem::DestroyEnemy(size_t id)
	{
		Wave* wave = GetComponentByIterator<Wave>(m_WaveIt);

		// Delete the enemy entity entirely
		m_EnemiesIds.erase(std::remove(m_EnemiesIds.begin(), m_EnemiesIds.end(), id), m_EnemiesIds.end());
		wave->currentEnemiesAlive--;

		//m_CurrentEnemiesDead++;

		if (m_EnemiesIds.size() <= 0 && m_TotalEnemiesSpawned >= wave->maxEnemies)
		{
			wave->hasFinished = true;
		}
	}

	void WaveSystem::SetSpawner(const EntityManager::ComponentIterator& m_WaveIt)
	{
		m_SpawnerIt = m_WaveIt;
	}

	Pool_Type WaveSystem::GetEnemyFromProbabiliteis()
	{
		EnemyManager& enemyManager = GameStateManager::GetEnemyManager();
		if (enemyManager.m_CurrentCombatRoomsCount >= enemyManager.m_SpawnerDataTable.size())
		{
			enemyManager.m_CurrentCombatRoomsCount = enemyManager.m_SpawnerDataTable.size() - 1;
		}
		const VariantData& data = enemyManager.m_VariantsTable[enemyManager.m_CurrentCombatRoomsCount];

		std::uniform_int_distribution<> disEnemies(1, 100);
		int randomNum = disEnemies(Application::s_Gen);
		if (randomNum <= 35) // 35% probability
		{
			int randomNum2 = disEnemies(Application::s_Gen);
			if (randomNum2 <= 33.33f && data.list.at("MELEE_PHALANX_GENERIC"))
			{
				return Pool_Type::PHALANX_MELEE_GENERIC;
			}
			else if (randomNum2 <= 66.66f && data.list.at("MELEE_PHALANX_REDVARIANT"))
			{
				return Pool_Type::PHALANX_MELEE_VARIANT_A;
			}
			else if (data.list.at("MELEE_PHALANX_BLUEVARIANT"))
			{
				return Pool_Type::PHALANX_MELEE_VARIANT_B;
			}
			else
				return GetEnemyFromProbabiliteis();
		}
		else if (randomNum <= 90) { // 45% probability
			int randomNum2 = disEnemies(Application::s_Gen);
			if (randomNum2 <= 33.33f && data.list.at("RANGED_PHALANX_GENERIC"))
			{
				return Pool_Type::PHALANX_RANGED_GENERIC;
			}
			else if (randomNum2 <= 66.66f && data.list.at("RANGED_PHALANX_REDVARIANT"))
			{
				return Pool_Type::PHALANX_RANGED_VARIANT_A;
			}
			else if (data.list.at("RANGED_PHALANX_BLUEVARIANT"))
			{
				return Pool_Type::PHALANX_RANGED_VARIANT_B;
			}
			else
				return GetEnemyFromProbabiliteis();
		}
		else if (randomNum <= 95) { // 5% probability
			return Pool_Type::SENTINEL;
		}
		else { 
			int randomNum2 = disEnemies(Application::s_Gen);
			if (randomNum2 <= 50 && data.list.at("SUBJUGATOR"))
			{
				return Pool_Type::SUBJUGATOR;
			}
			else if (data.list.at("SUBJUGATOR_CHIEF"))
			{
				return Pool_Type::SUBJUGATOR_CHIEF;
			}
			else
				return GetEnemyFromProbabiliteis();
		}
	}

	void WaveSystem::GetSpawnPoints(std::vector<glm::vec3>& vec)
	{
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		size_t size = 0;
		Wiwa::WaveSpawnPoint* spawnPointsList = nullptr;
		spawnPointsList = em.GetComponents<WaveSpawnPoint>(&size);
		if (spawnPointsList)
		{
			for (int i = 0; i < size; i++)
			{
				if (em.IsComponentRemoved<WaveSpawnPoint>(i)) {
				}
				else {
					Wiwa::WaveSpawnPoint* spawner = &spawnPointsList[i];
					if (spawner)
					{
						vec.emplace_back(spawner->point);
					}
				}
			}
		}
	}
}