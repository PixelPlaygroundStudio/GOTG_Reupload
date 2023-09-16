#include <wipch.h>
#include "PhysicsSystem.h"
#include "Wiwa/physics/PhysicsManager.h"
#include "Wiwa/core/Resources.h"
#include "Wiwa/utilities/render/Model.h"
#include "Wiwa/ecs/components/Mesh.h"

namespace Wiwa {
	PhysicsSystem::PhysicsSystem()
	{
		obj = nullptr;
	}

	PhysicsSystem::~PhysicsSystem()
	{

	}

	void PhysicsSystem::OnAwake()
	{
	}

	void PhysicsSystem::OnInit()
	{
	}

	void PhysicsSystem::OnUpdate()
	{
		//PhysicsManager& physicsManager = Wiwa::SceneManager::getActiveScene()->GetPhysicsManager();

	/*	Wiwa::EntityManager& entityManager = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
		const char* e_name = entityManager.GetEntityName(m_EntityId);
		WI_INFO("Updating physics of --> {}", e_name);*/
	}

	void PhysicsSystem::OnDestroy()
	{
		DeleteBody();
	}

	void PhysicsSystem::OnSystemAdded()
	{
		CreateBody();
	}
	void PhysicsSystem::OnSystemRemoved()
	{
		DeleteBody();
	}

	void PhysicsSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		Wiwa::EntityManager& entityManager = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
		const char* e_name = entityManager.GetEntityName(body1->id);
		//WI_INFO("Collision enter of --> {}", e_name);
	}

	void PhysicsSystem::OnCollision(Object* body1, Object* body2)
	{
		//Wiwa::EntityManager& entityManager = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
		//const char* e_name = entityManager.GetEntityName(body1->id);
		//WI_INFO("Collision continuwus of --> {}", e_name);
	}

	void PhysicsSystem::OnCollisionExit(Object* body1, Object* body2)
	{
		Wiwa::EntityManager& entityManager = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
		const char* e_name = entityManager.GetEntityName(body1->id);
		//WI_INFO("Collision leave of --> {}", e_name);
	}

	void PhysicsSystem::ForceSetPosition(const glm::vec3& new_position)
	{
		if (!obj)
			return;

		Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
		CollisionBody* collBody = (CollisionBody*)em.GetComponentByIterator(em.GetComponentIterator<CollisionBody>(m_EntityId));
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(m_EntityId));
		m_Scene->GetPhysicsManager().UpdateSingleEngineToPhys(new_position, *selfTr, *collBody, obj);
	}

	Object* PhysicsSystem::getBody()
	{
		return obj;
	}

	void PhysicsSystem::ActivateBody()
	{
		if (obj != nullptr)
		{
			obj->collisionObject->setActivationState(ACTIVE_TAG);
		}
	}

	void PhysicsSystem::DeActivateBody()
	{
		if (obj != nullptr)
		{
			obj->collisionObject->setActivationState(DISABLE_SIMULATION);
		}
	}

	bool PhysicsSystem::OnDisabledFromPool()
	{
		if (obj != nullptr)
			obj->velocity = { 0,0,0 };
		return false;
	}

	void PhysicsSystem::DeleteBody()
	{
		PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

		if (obj != nullptr)
			physicsManager.DeleteBody(obj);

		obj = nullptr;
	}

	void PhysicsSystem::CreateBody()
	{
		Wiwa::EntityManager& entityManager = m_Scene->GetEntityManager();
		const char* e_name = entityManager.GetEntityName(m_EntityId);
	
		CollisionBody* rb = GetComponent<CollisionBody>();
		Transform3D* transform = GetComponent<Transform3D>();

		ColliderCube* cube = GetComponent<ColliderCube>();
		ColliderSphere* sphere = GetComponent<ColliderSphere>();
		ColliderCylinder* cylinder = GetComponent<ColliderCylinder>();
		ColliderCapsule* capsule = GetComponent<ColliderCapsule>();

		Mesh* mesh = GetComponent<Mesh>();

		PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();

		if (rb)
		{
			if (cube)
			{
				if (mesh)
				{
			
					//Model* m = Wiwa::Resources::GetResourceById<Wiwa::Model>(mesh->meshId);
					////fix this 
					//for (auto model : m->GetModelList())
					//{
					//	if (model->IsRoot())
					//		continue;
					//	WI_INFO("half size {0} {1} {2}", model->boundingBox.HalfSize().x, model->boundingBox.HalfSize().y, model->boundingBox.HalfSize().z);

					//	cube->halfExtents.x = model->boundingBox.HalfSize().x * transform->scale.x;
					//	cube->halfExtents.y = model->boundingBox.HalfSize().y * transform->scale.y;
					//	cube->halfExtents.z = model->boundingBox.HalfSize().z * transform->scale.z;

					//	break;
					//}
				}
				obj = physicsManager.AddBodyCube(m_EntityId, *cube, *transform, *rb);
			}
			else if (sphere)
			{
				obj = physicsManager.AddBodySphere(m_EntityId, *sphere, *transform, *rb);
			}
			else if (cylinder)
			{
				obj = physicsManager.AddBodyCylinder(m_EntityId, *cylinder, *transform, *rb);
			}
			else if (capsule)
			{
				obj = physicsManager.AddBodyCapsule(m_EntityId, *capsule, *transform, *rb);
			}
		}
	}
	void PhysicsSystem::SetBoundingBoxes(Model* root, ColliderCube* cube, Transform3D* transform)
	{
	}
}