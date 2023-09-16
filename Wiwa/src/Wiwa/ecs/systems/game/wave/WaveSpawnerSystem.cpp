#include <wipch.h>
#include "WaveSpawnerSystem.h"
#include "WaveSystem.h"
#include <Wiwa/ecs/components/game/wave/WaveSpawner.h>

namespace Wiwa
{
	WaveSpawnerSystem::WaveSpawnerSystem()
	{
		m_EnemySpawnerIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_LastWave = WI_INVALID_INDEX;

		m_HasWave = false;
		m_SpawnedWavesCounter = 0;
		m_TimerBetweenWaves = 0.0f;
		m_TimerForNextWave = 0.0f;
		m_StopUpdating = false;
	}

	WaveSpawnerSystem::~WaveSpawnerSystem()
	{
	}

	void WaveSpawnerSystem::OnAwake()
	{
	
	}

	void WaveSpawnerSystem::OnInit()
	{
		m_EnemySpawnerIt = GetComponentIterator<WaveSpawner>();
		WaveSpawner* enemySpawner = GetComponentByIterator<WaveSpawner>(m_EnemySpawnerIt);

		// Retreive data from enemy manager table
		EnemyManager& enemyManager = GameStateManager::GetEnemyManager();

		if (GameStateManager::GetType() != RoomType::ROOM_BOSS)
		{
			if (enemyManager.m_CurrentCombatRoomsCount >= enemyManager.m_SpawnerDataTable.size())
			{
				enemyManager.m_CurrentCombatRoomsCount = enemyManager.m_SpawnerDataTable.size() - 1;
			}
			const WaveSpawner& data = enemyManager.m_SpawnerDataTable[enemyManager.m_CurrentCombatRoomsCount].waveSpawnData;
			enemySpawner->maxEnemiesPerWave = data.maxEnemiesPerWave;
			enemySpawner->minEnemiesPerWave = data.minEnemiesPerWave;
			enemySpawner->enemySpawnRate = data.enemySpawnRate;
			enemySpawner->maxWaveCount = data.maxWaveCount;
			enemySpawner->timeBetweenWaves = data.timeBetweenWaves;
			enemySpawner->waveChangeRate = data.waveChangeRate;
		}

		m_TimerBetweenWaves = enemySpawner->timeBetweenWaves;
		enemySpawner->hasTriggered = false;
		enemySpawner->hasFinished = false;
		enemySpawner->entityId = m_EntityId;

		SpawnWave();
	}

	void WaveSpawnerSystem::OnUpdate()
	{
		if (m_StopUpdating)
			return;

		WaveSpawner* enemySpawner = GetComponentByIterator<WaveSpawner>(m_EnemySpawnerIt);
		Wave* currentWave = nullptr;

		if (enemySpawner->hasFinished)
			return;

		m_Action = QueryActiveWaves();

		if (m_TimerForNextWave >= enemySpawner->waveChangeRate && m_Action != Wiwa::WaveSpawnerAction::END_SPAWNER)
		{
			m_Action = WaveSpawnerAction::SPAWN_A_WAVE;
		}

		switch (m_Action)
		{
		case Wiwa::WaveSpawnerAction::DO_NOTHING:
		{

		}
			break;
		case Wiwa::WaveSpawnerAction::SPAWN_A_WAVE:
		{
			SpawnWave();
		}
			break;
		case Wiwa::WaveSpawnerAction::END_SPAWNER:
		{
			Wiwa::GameStateManager::IncrementPrometheanGems(20);
			enemySpawner->hasFinished = true;
			m_StopUpdating = true;
		}
			break;
		default:
			break;
		}

		m_TimerBetweenWaves += Time::GetRealDeltaTimeSeconds();
		m_TimerForNextWave += Time::GetRealDeltaTimeSeconds();
	}

	WaveSpawnerAction WaveSpawnerSystem::QueryActiveWaves()
	{
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		WaveSpawner* enemySpawner = GetComponentByIterator<WaveSpawner>(m_EnemySpawnerIt);
		
		if (m_SpawnedWavesCounter >= enemySpawner->maxWaveCount)
		{
			return WaveSpawnerAction::END_SPAWNER;
		}

		if (enemySpawner->hasTriggered == false)
		{
			return WaveSpawnerAction::SPAWN_A_WAVE;
		}

		int wavesFinished = 0;

		for (int i = 0; i < m_WavesIds.size(); i++)
		{
			Wave* wave = GetComponentByIterator<Wave>(m_WavesIts[i]);
			if (wave->hasFinished)
			{
				wavesFinished += 1;
			}
		}

		//WI_INFO("WAVES FINISHED: {}", wavesFinished);
		//WI_INFO("WAVES COUNT: {}", enemySpawner->maxWaveCount);
		//WI_INFO("Has Had All Waves: {}", HasHadAllWaves(*enemySpawner));
		//WI_INFO("Has triggered: {}", enemySpawner->hasTriggered);

		
		if (wavesFinished == m_WavesIds.size() && wavesFinished < enemySpawner->maxWaveCount)
		{
			return WaveSpawnerAction::SPAWN_A_WAVE;
		}

		return WaveSpawnerAction::DO_NOTHING;
	}

	void WaveSpawnerSystem::SpawnWave()
	{
		Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();

		WaveSpawner* wavesSpawner = GetComponentByIterator<WaveSpawner>(m_EnemySpawnerIt);
		wavesSpawner->currentWaveCount += 1;
		if (wavesSpawner->hasFinished || m_StopUpdating)
			return;

		// Create an empty entity
		std::string waveName = em.GetEntityName(m_EntityId);
		waveName += "_wave_" + std::to_string(wavesSpawner->currentWaveCount);
		EntityId waveId = em.CreateEntity(waveName.c_str());
		m_WavesIds.emplace_back(waveId);

		Transform3D* waveTransform = em.AddComponent<Transform3D>(waveId);
		//Transform3D* parent = GetComponentByIterator<Transform3D>(m_TransformIt);
		//waveTransform->localPosition = parent->localPosition;
		
		// Create a wave component and wave system
		Wave* wave = em.AddComponent<Wave>(waveId);
		std::uniform_int_distribution<> disMinMaxEnemy(wavesSpawner->minEnemiesPerWave, wavesSpawner->maxEnemiesPerWave);
		wave->maxEnemies = disMinMaxEnemy(Application::s_Gen);
		wave->hasFinished = false;
		wave->enemySpawnRate = wavesSpawner->enemySpawnRate;
		wave->entityId = waveId;

		em.ApplySystem<WaveSystem>(waveId);

		// Save the wave as current wave
		EntityManager::ComponentIterator waveIt = GetComponentIterator<Wave>(waveId);
		em.GetSystem<WaveSystem>(waveId)->SetSpawner(waveIt);
		m_WavesIts.emplace_back(waveIt);

		m_LastWave = waveId;
		m_HasWave = false;
		m_TimerForNextWave = 0.0f;
		m_TimerBetweenWaves = 0.0f;
		m_SpawnedWavesCounter++;

		wavesSpawner->hasTriggered = true;
	}
}

