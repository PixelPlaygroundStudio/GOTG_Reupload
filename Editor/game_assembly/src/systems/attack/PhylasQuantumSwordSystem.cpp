#include "PhylasQuantumSwordSystem.h"
#include "../../components/attack/PhylasQuantumSword.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include "Wiwa/ecs/systems/AudioSystem.h"
#include "../../components/attack/Attack.h"
#include <Wiwa/game/Items/ItemManager.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
namespace Wiwa
{
	PhylasQuantumSwordSystem::PhylasQuantumSwordSystem()
	{
		m_PhylasSwordIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
	}

	PhylasQuantumSwordSystem::~PhylasQuantumSwordSystem()
	{

	}

	void PhylasQuantumSwordSystem::OnAwake()
	{
		m_PhylasSwordIt = GetComponentIterator<PhylasQuantumSword>();
		m_PhylasSwordTransformIt = GetComponentIterator<Transform3D>();

	}

	void PhylasQuantumSwordSystem::OnInit()
	{
		PhylasQuantumSword* PhylasSword = GetComponent<PhylasQuantumSword>();
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		Transform3D* PhylasSwordTransform = em.GetComponent<Transform3D>(m_EntityId);
		Attack* attack = GetComponentByIterator<Attack>(GetComponentIterator<Attack>(m_EntityId));
		if (attack)
		{
			strcpy(attack->attackType, "PHYLAS_QUANTUM_SWORD");
		}

		PhylasSword->damage = Wiwa::ItemManager::GetAbility("Phyla's Quantum Sword")->Damage;
		PhylasSword->lifeTime = 30.0f;
		PhylasSword->velocity = 40.0f;
		
		EntityId player = Wiwa::GameStateManager::GetPlayerId();
		Transform3D* playerTransform = em.GetComponent<Transform3D>(player);
		PhylasSword->direction = Math::CalculateForward(playerTransform);
		PhylasSwordTransform->localRotation = playerTransform->localRotation;
		PhylasSwordTransform->localPosition.x = playerTransform->localPosition.x;
		PhylasSwordTransform->localPosition.z = playerTransform->localPosition.z;
		PhylasSwordTransform->localPosition.y = playerTransform->localPosition.y;

		PhylasSwordTransform->localScale.x = 1.5f;
		PhylasSwordTransform->localScale.z = 1.5f;
		PhylasSwordTransform->localScale.y = 1.5f;


		AudioSystem* audio = em.GetSystem<Wiwa::AudioSystem>(m_EntityId);
		if (audio)
			audio->PlayAudio("phyla_sword");
	}

	void PhylasQuantumSwordSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		PhylasQuantumSword* PhylasSword = GetComponent<PhylasQuantumSword>();
		Transform3D* PhylasSwordTransform = GetComponent<Transform3D>();

		PhylasSwordTransform->localPosition += PhylasSword->direction * PhylasSword->velocity * Time::GetDeltaTimeSeconds();

		m_Timer += Time::GetDeltaTimeSeconds();

		
		if (m_Timer >= PhylasSword->lifeTime)
		{
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			em.DestroyEntity(m_EntityId);
		}

	}


	void PhylasQuantumSwordSystem::OnDestroy()
	{

	}

	void PhylasQuantumSwordSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		std::string tag_wall = "WALL";
		std::string tag_column = "COLUMN";
		if (body1->id == m_EntityId && (tag_wall == body2->selfTagStr || tag_column == body2->selfTagStr))
		{
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			em.DestroyEntity(m_EntityId);
		}
	}
}