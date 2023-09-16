#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	struct WI_API WaveSpawner {
		int maxEnemiesPerWave;
		int minEnemiesPerWave;
		float enemySpawnRate;
		int maxWaveCount;
		int currentWaveCount;
		float timeBetweenWaves;
		float waveChangeRate;
		bool hasFinished;
		bool hasTriggered;
		size_t entityId;
	};
}

REFLECTION_BEGIN(Wiwa::WaveSpawner)
REFLECT_MEMBER(maxEnemiesPerWave)
REFLECT_MEMBER(minEnemiesPerWave)
REFLECT_MEMBER(enemySpawnRate)
REFLECT_MEMBER(maxWaveCount)
REFLECT_MEMBER(currentWaveCount)
REFLECT_MEMBER(timeBetweenWaves)
REFLECT_MEMBER(waveChangeRate)
REFLECT_MEMBER(hasFinished)
REFLECT_MEMBER(hasTriggered)
REFLECT_MEMBER(entityId)
REFLECTION_END;