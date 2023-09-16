#include <wipch.h>
#include "System.h"

namespace Wiwa {
	//-------- CONSTRUCTOR --------
	System::System() : m_EntityId(EntityManager::INVALID_INDEX)
	{
		m_Awaken = false;
		m_Inited = false;
	}

	//-------- DESTRUCTOR --------
	System::~System()
	{
	}

	//-------- Entity Functions --------

	//-------- Sub-system functions --------
	void System::Awake()
	{
		if (m_Awaken) return;

		OnAwake();
		
		m_Awaken = true;
	}

	void System::Init()
	{
		if (m_Inited) return;

		OnInit();

		m_Inited = true;
	}

	void System::Update()
	{
		if (!m_Scene->GetEntityManager().IsActive(m_EntityId))
			return;


		OnUpdate();
	}

	void System::Destroy()
	{
		OnDestroy();
	}
}