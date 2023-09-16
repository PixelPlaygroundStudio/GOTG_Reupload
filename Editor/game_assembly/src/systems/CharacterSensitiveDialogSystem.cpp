#include "CharacterSensitiveDialogSystem.h"
#include <Wiwa/game/GameStateManager.h>

#include "Wiwa/Dialog/DialogManager.h"

namespace Wiwa
{

	Wiwa::CharacterSensitiveDialogSystem::CharacterSensitiveDialogSystem()
	{
		
	}

	CharacterSensitiveDialogSystem::~CharacterSensitiveDialogSystem()
	{
	}

	void CharacterSensitiveDialogSystem::OnAwake()
	{
	}

	void CharacterSensitiveDialogSystem::OnInit()
	{

	}

	void CharacterSensitiveDialogSystem::OnUpdate()
	{
		
	}

	void CharacterSensitiveDialogSystem::OnDestroy()
	{
	}

	void CharacterSensitiveDialogSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		std::string playerStr = "PLAYER";
		if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();
			
			_scene->GetDialogManager().characterSensitive = true;
			
		}
	}

	void CharacterSensitiveDialogSystem::OnCollisionExit(Object* body1, Object* body2)
	{
		std::string playerStr = "PLAYER";
		if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();

			_scene->GetDialogManager().characterSensitive = false;
		}
	}
}