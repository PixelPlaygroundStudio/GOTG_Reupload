#include "Reset_HubSystem.h"
void Wiwa::Reset_HubSystem::OnAwake()
{
}
void Wiwa::Reset_HubSystem::OnInit()
{
	m_ResetStats = true;
}
void Wiwa::Reset_HubSystem::OnUpdate()
{
	if (m_ResetStats)
	{
		Wiwa::GameStateManager::InitPlayerData();
		Wiwa::GameStateManager::SaveProgression();
		m_ResetStats = false;
	}
}
