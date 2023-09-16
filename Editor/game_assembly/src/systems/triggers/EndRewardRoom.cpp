#include "EndRewardRoom.h"
#include "../../components/trigger/RewardRoomData.h"

void Wiwa::EndRewardRoom::OnAwake()
{
	m_Activated = false;
	m_Timer = 0.0f;
}
void Wiwa::EndRewardRoom::OnInit()
{
}
void Wiwa::EndRewardRoom::OnUpdate()
{
	if (!m_Activated)
		return;

	if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA)
		|| Wiwa::Input::IsKeyPressed(Key::Enter))
	{
		RewardRoomData* data = GetComponent<RewardRoomData>();
		GameStateManager::setPlayerTriggerNextRoom(true);
		if(data->isLeft)
			GameStateManager::s_NextRewardRoomReward = GameStateManager::s_DoorsReward[0];
		else
			GameStateManager::s_NextRewardRoomReward = GameStateManager::s_DoorsReward[1];
	}

	if (m_Timer >= 2.0f)
	{
		GameStateManager::setPlayerTriggerNextRoom(false);
		m_Timer = 0.0f;
	}

	m_Timer += Time::GetDeltaTimeSeconds();
}

void Wiwa::EndRewardRoom::OnDestroy()
{
	GameStateManager::setPlayerTriggerNextRoom(false);
}

void Wiwa::EndRewardRoom::OnCollisionEnter(Object* body1, Object* body2)
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	std::string playerBulletStr = "PLAYER";
	if (body1->id == m_EntityId && playerBulletStr == body2->selfTagStr)
	{
		gm.canvas.at(9)->SwapActive();
		m_Activated = true;
	}
}

void Wiwa::EndRewardRoom::OnCollisionExit(Object* body1, Object* body2)
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	std::string playerBulletStr = "PLAYER";
	if (body1->id == m_EntityId && playerBulletStr == body2->selfTagStr)
	{
		gm.canvas.at(9)->SwapActive();
		m_Activated = false;
	}
	GameStateManager::setPlayerTriggerNextRoom(false);
}