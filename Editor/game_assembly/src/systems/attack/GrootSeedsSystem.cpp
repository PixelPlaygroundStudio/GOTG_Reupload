#include "GrootSeedsSystem.h"
#include "../../components/attack/GrootSeeds.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include "Wiwa/ecs/systems/AudioSystem.h"
#include "../../components/attack/Attack.h"
#include <Wiwa/game/Items/ItemManager.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
namespace Wiwa
{
	GrootSeedsSystem::GrootSeedsSystem()
	{
		m_GrootSeedsIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
	}

	GrootSeedsSystem::~GrootSeedsSystem()
	{

	}

	void GrootSeedsSystem::OnAwake()
	{
		m_GrootSeedsIt = GetComponentIterator<GrootSeeds>();
		m_GrootSeedsTransformIt = GetComponentIterator<Transform3D>();

	}

	void GrootSeedsSystem::OnInit()
	{
		GrootSeeds* grootSeeds = GetComponentByIterator<GrootSeeds>(m_GrootSeedsIt);
		Transform3D* grootSeedsTransform = GetComponentByIterator<Transform3D>(m_GrootSeedsTransformIt);
		Attack* attack = GetComponentByIterator<Attack>(GetComponentIterator<Attack>(m_EntityId));
		if (attack)
		{
			strcpy(attack->attackType, "GROOTS_SEEDS");
		}
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		grootSeeds->damage = Wiwa::ItemManager::GetAbility("Groot's Seeds")->Damage;
		grootSeeds->lifeTime = 5.0f;

		EntityId player = GameStateManager::GetPlayerId();

		m_PlayerTransformIt = em.GetComponentIterator<Transform3D>(player);
		Transform3D* playerTransform = GetComponentByIterator<Transform3D>(m_PlayerTransformIt);

		grootSeedsTransform->localRotation = playerTransform->localRotation;

		grootSeedsTransform->localPosition = playerTransform->localPosition;
	
		grootSeedsTransform->localPosition.y = 0.1f;

		//grootSeedsTransform->localScale.x = 10.0f;
		//grootSeedsTransform->localScale.z = 10.0f;

		AudioSystem* audio = em.GetSystem<Wiwa::AudioSystem>(m_EntityId);
		if(audio)
			audio->PlayAudio("groot_seed");

	}

	void GrootSeedsSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		GrootSeeds* grootSeeds = GetComponentByIterator<GrootSeeds>(m_GrootSeedsIt);

		Transform3D* playerTransform = GetComponentByIterator<Transform3D>(m_PlayerTransformIt);
		Transform3D* grootSeedsTransform = GetComponentByIterator<Transform3D>(m_GrootSeedsTransformIt);

		m_Timer += Time::GetDeltaTimeSeconds();


		if (m_Timer >= grootSeeds->lifeTime)
		{
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			em.DestroyEntity(m_EntityId);
		}

	}


	void GrootSeedsSystem::OnDestroy()
	{

	}

	void GrootSeedsSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
	}


}