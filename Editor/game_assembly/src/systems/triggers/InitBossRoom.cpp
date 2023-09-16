#include "InitBossRoom.h"

namespace Wiwa
{
	Wiwa::InitBossRoom::InitBossRoom()
	{
	}

	InitBossRoom::~InitBossRoom()
	{
	}

	void InitBossRoom::OnAwake()
	{
		GameStateManager::setCanPassNextRoom(false);
		GameStateManager::s_HasFinshedRoom = false;
	}
}