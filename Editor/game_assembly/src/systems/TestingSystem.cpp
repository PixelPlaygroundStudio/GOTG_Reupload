#include "TestingSystem.h"
#include <Wiwa/ecs/systems/PhysicsSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

namespace Wiwa
{
	TestingSystem::TestingSystem()
	{
		m_SpawnOnce = false;
		m_Timer = 0.0f;
	}

	TestingSystem::~TestingSystem()
	{
	}

	void TestingSystem::OnAwake()
	{
	}

	void TestingSystem::OnInit()
	{
		SetTransform();
	}

	void TestingSystem::OnUpdate()
	{
		if (Input::IsKeyPressed(Key::P) && m_Timer >= 2.0f)
		{
			EntityManager& em = GetEntityManager();
			EntityId id = em.LoadPrefab("assets\\Prefabs\\YondusFin\\YundusFin_01.wiprefab");
			Transform3D* tr = em.GetComponent<Transform3D>(id);
			*tr = *GetTransform();
		/*	tr->localPosition.y = 2.3f;
			tr->position = tr->localPosition;
			physSys->ForceSetPosition(tr->localPosition);
			navAgentSys->OnInit();
			navAgentSys->SetPosition(tr->localPosition);*/
			m_SpawnOnce = true;
			m_Timer = 0.0f;
		}
		m_Timer += Time::GetDeltaTimeSeconds();
	}
}