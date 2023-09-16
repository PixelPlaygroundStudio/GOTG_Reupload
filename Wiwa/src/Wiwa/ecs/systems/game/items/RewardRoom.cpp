#include <wipch.h>
#include "RewardRoom.h"
#include <Wiwa/scene/SceneManager.h>

void Wiwa::RewardRoom::OnInit()
{
	GameStateManager::SpawnRandomItem(GetTransform()->position, GameStateManager::s_NextRewardRoomReward);
}
