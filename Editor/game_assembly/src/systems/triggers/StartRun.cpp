#include "StartRun.h"
#include "../../components/trigger/Trigger.h"
#include "EndRoom.h"
#include "../../components/trigger/RewardRoomData.h"

Wiwa::StartRunTrigger::StartRunTrigger()
{
    m_DataIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
	m_Activated = false;
}

Wiwa::StartRunTrigger::~StartRunTrigger()
{
}

void Wiwa::StartRunTrigger::OnInit()
{
	m_DataIt = GetComponentIterator<RewardRoomData>();
	RewardRoomData* data = GetComponentByIterator<RewardRoomData>(m_DataIt);
	if (data)
	{
		if (data->isLeft)
		{
			data->num = GameStateManager::s_DoorsReward[0];
		}
		else
		{
			data->num = GameStateManager::s_DoorsReward[1];
		}
	}
}

void Wiwa::StartRunTrigger::OnAwake()
{
}

void Wiwa::StartRunTrigger::OnUpdate()
{
	if (!m_Activated)
		return;

	if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA)
		|| Wiwa::Input::IsKeyPressed(Key::Enter))
	{
		WI_INFO("----------- Starting a new run -----------");
		RewardRoomData* data = GetComponentByIterator<RewardRoomData>(m_DataIt);
		GameStateManager::s_NextRewardRoomReward = data->num;
		GameStateManager::StartRun();
	}
}

void Wiwa::StartRunTrigger::OnCollisionEnter(Object* body1, Object* body2)
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	std::string playerTag = "PLAYER";

	if (body1->id == m_EntityId && body2->selfTagStr == playerTag)
	{
		gm.canvas.at(9)->SwapActive();
		m_Activated = true;
	}
}

void Wiwa::StartRunTrigger::OnCollisionExit(Object* body1, Object* body2)
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	std::string playerTag = "PLAYER";

	if (body1->id == m_EntityId && body2->selfTagStr == playerTag)
	{
		gm.canvas.at(9)->SwapActive();
		m_Activated = false;
	}
}
