#include "NpcSystem.h"
#include <Wiwa/game/GameStateManager.h>

#include "Wiwa/Dialog/DialogManager.h"

namespace Wiwa
{

	Wiwa::NpcSystem::NpcSystem()
	{
		
	}

	NpcSystem::~NpcSystem()
	{
		
	}

	void NpcSystem::OnAwake()
	{
	}

	void NpcSystem::OnInit()
	{
		
	}

	void NpcSystem::OnUpdate()
	{
		
	}

	void NpcSystem::OnDestroy()
	{
		
	}

	void NpcSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		std::string playerStr = "PLAYER";
		if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();
			
			_scene->GetDialogManager().collidingWithNpc = true;
			_scene->GetDialogManager().NpcConversationTag = em.GetEntityName(body1->id);
		}
	}

	void NpcSystem::OnCollisionExit(Object* body1, Object* body2)
	{
		std::string playerStr = "PLAYER";
		if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();

			_scene->GetDialogManager().collidingWithNpc = false;
		}
	}
}