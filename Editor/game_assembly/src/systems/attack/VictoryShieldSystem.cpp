#include "VictoryShieldSystem.h"
#include "../../components/attack/VictoryShield.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/game/Items/ItemManager.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>

namespace Wiwa
{
	VictoryShieldSystem::VictoryShieldSystem()
	{
		m_ShieldIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
	}

	VictoryShieldSystem::~VictoryShieldSystem()
	{

	}

	void VictoryShieldSystem::OnAwake()
	{
		m_ShieldIt = GetComponentIterator<VictoryShield>();
		m_ShieldTransfromIt = GetComponentIterator<Transform3D>();
	}

	void VictoryShieldSystem::OnInit()
	{
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		VictoryShield* shield = GetComponentByIterator<VictoryShield>(m_ShieldIt);
		shield->velocity = 200.f;
		shield->lifeTime = Wiwa::ItemManager::GetBuff("Major Victory's Shield")->Duration;
		EntityId player = GameStateManager::GetPlayerId();
		
		m_PlayerTransformIt = em.GetComponentIterator<Transform3D>(player);
		EntityId child = em.GetChildByName(m_EntityId, "Colider");
		m_ColliderTransformIt = em.GetComponentIterator<Transform3D>(child);
	}

	void VictoryShieldSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		VictoryShield* shield = GetComponentByIterator<VictoryShield>(m_ShieldIt);

		Wiwa::Transform3D* tranformShield = GetComponentByIterator<Transform3D>(m_ShieldTransfromIt);
		Wiwa::Transform3D* transformPlayer = GetComponentByIterator<Transform3D>(m_PlayerTransformIt);
		Wiwa::Transform3D* transformCol = GetComponentByIterator<Transform3D>(m_ColliderTransformIt);

		tranformShield->localPosition.x = transformPlayer->localPosition.x;
		tranformShield->localPosition.z = transformPlayer->localPosition.z;
		tranformShield->localRotation.y += shield->velocity * Time::GetDeltaTimeSeconds();
		transformCol->localPosition.y = 0.f;
		m_Timer += Time::GetDeltaTimeSeconds();
		if (m_Timer >= shield->lifeTime)
		{
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			em.DestroyEntity(m_EntityId);
		}

	}


	void VictoryShieldSystem::OnDestroy()
	{

	}

	void VictoryShieldSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		/*if (body1->id == m_EntityId && strcmp(body2->selfTagStr, "ENEMY_BULLET") == 0)
		{
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			em.DestroyEntity(body2->id);
		}*/
	}

}
