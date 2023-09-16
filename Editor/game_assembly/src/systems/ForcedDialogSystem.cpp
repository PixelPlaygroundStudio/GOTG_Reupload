#include "ForcedDialogSystem.h"
#include <Wiwa/game/GameStateManager.h>

#include "Wiwa/Dialog/DialogManager.h"

namespace Wiwa
{

	Wiwa::ForcedDialogSystem::ForcedDialogSystem()
	{
		
	}

	ForcedDialogSystem::~ForcedDialogSystem()
	{
	}

	void ForcedDialogSystem::OnAwake()
	{
	}

	void ForcedDialogSystem::OnInit()
	{

	}

	void ForcedDialogSystem::OnUpdate()
	{
		
	}

	void ForcedDialogSystem::OnDestroy()
	{
	}

	void ForcedDialogSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		std::string playerStr = "PLAYER";
		if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();
			
			_scene->GetDialogManager().forceStartConversation = true;
			_scene->GetDialogManager().forcedConversationTag = em.GetEntityName(body1->id);
		}
	}

	void ForcedDialogSystem::OnCollisionExit(Object* body1, Object* body2)
	{
		std::string playerStr = "PLAYER";
		if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();

			_scene->GetDialogManager().forceStartConversation = false;
		}
	}
}