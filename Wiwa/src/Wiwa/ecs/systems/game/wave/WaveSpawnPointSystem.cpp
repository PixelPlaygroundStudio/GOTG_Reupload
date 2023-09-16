#include <wipch.h>
#include "WaveSpawnPointSystem.h"
#include <Wiwa/ecs/components/game/wave/WaveSpawnPoint.h>

Wiwa::WaveSpawnPointSystem::WaveSpawnPointSystem()
{
	m_SpawnPointIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
}

Wiwa::WaveSpawnPointSystem::~WaveSpawnPointSystem()
{
}

void Wiwa::WaveSpawnPointSystem::OnAwake()
{
	m_SpawnPointIt = GetComponentIterator<WaveSpawnPoint>();
	WaveSpawnPoint* spawnPoint = GetComponentByIterator<WaveSpawnPoint>(m_SpawnPointIt);
	spawnPoint->point = GetTransform()->localPosition;
}
