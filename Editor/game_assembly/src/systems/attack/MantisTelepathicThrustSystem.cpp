#include "MantisTelepathicThrustSystem.h"
#include "../../components/attack/MantisTelepathicThrust.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/game/Items/ItemManager.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include "../../components/attack/Attack.h"

namespace Wiwa
{
	MantisTelepathicThrustSystem::MantisTelepathicThrustSystem()
	{
		m_MantisTelepathicThrustIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
	}

	MantisTelepathicThrustSystem::~MantisTelepathicThrustSystem()
	{

	}

	void MantisTelepathicThrustSystem::OnAwake()
	{
		m_MantisTelepathicThrustIt = GetComponentIterator<MantisTelepathicThrust>();
		m_MantisTelepathicThrustTransformIt = GetComponentIterator<Transform3D>();
	}

	void MantisTelepathicThrustSystem::OnInit()
	{
		MantisTelepathicThrust* mantisTelepathicThrust = GetComponentByIterator<MantisTelepathicThrust>(m_MantisTelepathicThrustIt);
		Transform3D* mantisTelepathicThrustTransform = GetComponentByIterator<Transform3D>(m_MantisTelepathicThrustTransformIt);
		Attack* attack = GetComponentByIterator<Attack>(GetComponentIterator<Attack>(m_EntityId));
		if (attack)
		{
			strcpy(attack->attackType, "MANTIS_TELEPATHIC_THRUST");
		}
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		mantisTelepathicThrust->damage = Wiwa::ItemManager::GetAbility("Manti's Telepathic Thrust")->Damage;
		mantisTelepathicThrust->lifeTime = 5.0f;

		EntityId player = GameStateManager::GetPlayerId();

		m_PlayerTransformIt = em.GetComponentIterator<Transform3D>(player);
		Transform3D* playerTransform = GetComponentByIterator<Transform3D>(m_PlayerTransformIt);

		mantisTelepathicThrustTransform->localRotation = playerTransform->localRotation;

		mantisTelepathicThrustTransform->localPosition = playerTransform->localPosition;

		mantisTelepathicThrustTransform->localScale *= Wiwa::ItemManager::GetAbility("Manti's Telepathic Thrust")->Area;
		mantisTelepathicThrustTransform->localPosition.y = 0.1f;

		AudioSystem* audio = em.GetSystem<Wiwa::AudioSystem>(m_EntityId);
		if (audio)
			audio->PlayAudio("mantis");
	}

	void MantisTelepathicThrustSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		MantisTelepathicThrust* mantisTelepathicThrust = GetComponentByIterator<MantisTelepathicThrust>(m_MantisTelepathicThrustIt);

		Transform3D* playerTransform = GetComponentByIterator<Transform3D>(m_PlayerTransformIt);
		Transform3D* mantisTelepathicThrustTransform = GetComponentByIterator<Transform3D>(m_MantisTelepathicThrustTransformIt);

		m_Timer += Time::GetDeltaTimeSeconds();

		mantisTelepathicThrustTransform->localRotation = playerTransform->localRotation;

		mantisTelepathicThrustTransform->localPosition = playerTransform->localPosition;

		mantisTelepathicThrustTransform->localPosition.y = 0.1f;

		if (m_Timer >= mantisTelepathicThrust->lifeTime)
		{
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			em.DestroyEntity(m_EntityId);
		}

	}


	void MantisTelepathicThrustSystem::OnDestroy()
	{

	}

	void MantisTelepathicThrustSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		std::string enemyStr = "ENEMY";

		if (body1->id == m_EntityId && body2->selfTagStr == enemyStr)
		{
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			Wiwa::PhysicsManager& physicsManager = m_Scene->GetPhysicsManager();
			Wiwa::Object* obj = em.GetSystem<Wiwa::PhysicsSystem>(body2->id)->getBody();
			Wiwa::Transform3D* tr = em.GetComponent<Transform3D>(body2->id);
			physicsManager.SetVelocity(obj, glm::normalize(Math::CalculateForward(tr)) * 50.0f * -1.0f);
		}
	}
}