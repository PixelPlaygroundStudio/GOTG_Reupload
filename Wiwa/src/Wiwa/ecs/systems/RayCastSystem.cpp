#include "wipch.h"
#include "RayCastSystem.h"
#include "Wiwa/physics/PhysicsManager.h"
Wiwa::RayCastSystem::RayCastSystem()
{
}

Wiwa::RayCastSystem::~RayCastSystem()
{
}

void Wiwa::RayCastSystem::OnUpdate()
{
	Wiwa::EntityManager& entityManager = m_Scene->GetEntityManager();
	const char* e_name = entityManager.GetEntityName(m_EntityId);

	PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();
	RayCast* rayCast = GetComponent<RayCast>();
	
	if (rayCast->doRayCasting)
		physicsManager.RayTest(btVector3(rayCast->rayFromWorld.x, rayCast->rayFromWorld.y, rayCast->rayFromWorld.z), 
			btVector3(rayCast->rayToWorld.x, rayCast->rayToWorld.y, rayCast->rayToWorld.z));
}
