#include "EndRoom.h"

Wiwa::EndRoomTrigger::EndRoomTrigger()
{
	m_Activated = false;
	m_Timer = 0.0f;
}

Wiwa::EndRoomTrigger::~EndRoomTrigger()
{
}

void Wiwa::EndRoomTrigger::OnDestroy()
{
	GameStateManager::setPlayerTriggerNextRoom(false);
}

void Wiwa::EndRoomTrigger::OnUpdate()
{
	if (!m_Activated)
		return;

	if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA)
		|| Wiwa::Input::IsKeyPressed(Key::Enter))
	{
		GameStateManager::setPlayerTriggerNextRoom(true);
	}

	if (m_Timer >= 2.0f)
	{
		GameStateManager::setPlayerTriggerNextRoom(false);
		m_Timer = 0.0f;
	}

	m_Timer += Time::GetDeltaTimeSeconds();
}

void Wiwa::EndRoomTrigger::OnCollisionEnter(Object* body1, Object* body2)
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	std::string playerBulletStr = "PLAYER";
	if (body1->id == m_EntityId && playerBulletStr == body2->selfTagStr)
	{
		gm.canvas.at(9)->SwapActive();
		m_Activated = true;
	}
}


void Wiwa::EndRoomTrigger::OnCollisionExit(Object* body1, Object* body2)
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
