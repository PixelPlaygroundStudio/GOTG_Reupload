#include "InitHub.h"

Wiwa::InitHubTrigger::InitHubTrigger()
{
}

Wiwa::InitHubTrigger::~InitHubTrigger()
{
}

void Wiwa::InitHubTrigger::OnAwake()
{
	GameStateManager::InitHub();
}
