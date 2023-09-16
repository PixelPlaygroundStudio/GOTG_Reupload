#include "RoomFinishedSystem.h"
#include <Wiwa/game/GameStateManager.h>
#include <Wiwa/core/Input.h>
#include <Wiwa/Ui/UiManager.h>


namespace Wiwa
{

	Wiwa::RoomFinishedSystem::RoomFinishedSystem()
	{

	}

	RoomFinishedSystem::~RoomFinishedSystem()
	{
	}

	void RoomFinishedSystem::OnAwake()
	{
	}

	void RoomFinishedSystem::OnInit()
	{
		m_FinishedRoom = false;
		m_FinishedAnim = false;
	}

	void RoomFinishedSystem::OnUpdate()
	{
		if (Wiwa::GameStateManager::GetRoomState() == "STATE_FINISHED")
			m_FinishedRoom = true;


		Wiwa::GuiManager& gm = m_Scene->GetGuiManager();

		if (m_FinishedRoom && !m_FinishedAnim)
		{
			m_TimerIn++;
			//gm.canvas.at(11)->controls.at(0)->MoveGUIElement({ 451.0f,900.0f }, m_TimerAnim, { (float)gm.canvas.at(11)->controls.at(0)->position.x,(float)gm.canvas.at(11)->controls.at(0)->position.y }, GuiControlEasing::EaseBounceIn);

			if (m_TimerIn >= 600)
			{
				m_FinishedRoom = false;
				m_FinishedAnim = true;
			}
			
		}
	}

	void RoomFinishedSystem::OnDestroy()
	{
	}

	void RoomFinishedSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		
	}
}