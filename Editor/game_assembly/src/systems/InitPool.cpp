#include "InitPool.h"

Wiwa::InitPool::InitPool()
{
}

Wiwa::InitPool::~InitPool()
{
}

void Wiwa::InitPool::OnAwake()
{
	GameStateManager::s_PoolManager->LoadAllPools(m_Scene);
}

void Wiwa::InitPool::OnInit()
{
}

void Wiwa::InitPool::OnDestroy()
{
	GameStateManager::s_PoolManager->UnloadAllPools();
}
