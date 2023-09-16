#include "OneTimeDialogSystem.h"
#include <Wiwa/game/GameStateManager.h>

#include "Wiwa/Dialog/DialogManager.h"

namespace Wiwa
{

	Wiwa::OneTimeDialogSystem::OneTimeDialogSystem()
	{
		
	}

	OneTimeDialogSystem::~OneTimeDialogSystem()
	{
	}

	void OneTimeDialogSystem::OnAwake()
	{
	}

	void OneTimeDialogSystem::OnInit()
	{
		colliding = false;
	}

	void OneTimeDialogSystem::OnUpdate()
	{
		if ((Wiwa::Input::IsKeyPressed(Wiwa::Key::Space) || Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA)) && colliding == true)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			_scene->GetDialogManager().alreadyTalkedOnceTriggered = true;
		}
	}

	void OneTimeDialogSystem::OnDestroy()
	{
	}

	void OneTimeDialogSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		std::string playerStr = "PLAYER";
		if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();
			
			colliding = true;
		}
	}

	void OneTimeDialogSystem::OnCollisionExit(Object* body1, Object* body2)
	{
		std::string playerStr = "PLAYER";
		if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();

			colliding = false;
		}
	}
}