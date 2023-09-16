#include "CaptainUniverseEnergySystem.h"
#include "../../components/attack/CaptainsUniverseEnergy.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/game/Items/ItemManager.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include "../../components/attack/Attack.h"


namespace Wiwa
{
	CaptainsUniverseEnergySystem::CaptainsUniverseEnergySystem()
	{
		m_CaptainsUniverseEnergyIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
	}

	CaptainsUniverseEnergySystem::~CaptainsUniverseEnergySystem()
	{

	}

	void CaptainsUniverseEnergySystem::OnAwake()
	{
		m_CaptainsUniverseEnergyIt = GetComponentIterator<CaptainsUniverseEnergy>();
		m_CaptainsUniverseEnergyTransformIt = GetComponentIterator<Transform3D>();
	}

	void CaptainsUniverseEnergySystem::OnInit()
	{
		CaptainsUniverseEnergy* captainUniverse = GetComponent<CaptainsUniverseEnergy>();
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		Attack* attack = GetComponentByIterator<Attack>(GetComponentIterator<Attack>(m_EntityId));
		if (attack)
		{
			strcpy(attack->attackType, "CAPTAINS_UNIVERSE");
		}
		Transform3D* captainUniverseTransform = em.GetComponent<Transform3D>(m_EntityId);
		captainUniverse->damage = Wiwa::ItemManager::GetAbility("Captain Universe's Energy")->Damage;
		captainUniverse->lifeTime = 30.0f;
		captainUniverse->velocity = Wiwa::ItemManager::GetAbility("Captain Universe's Energy")->Range;

		EntityId player = Wiwa::GameStateManager::GetPlayerId();
		Transform3D* playerTransform = em.GetComponent<Transform3D>(player);
		captainUniverse->direction = Math::CalculateForward(playerTransform);
		captainUniverseTransform->localRotation = playerTransform->localRotation;
		captainUniverseTransform->localPosition = playerTransform->localPosition;



		AudioSystem* audio = em.GetSystem<Wiwa::AudioSystem>(m_EntityId);
		if (audio)
			audio->PlayAudio("captain_universe");

	}

	void CaptainsUniverseEnergySystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		CaptainsUniverseEnergy* captainsUniverse = GetComponent<CaptainsUniverseEnergy>();
		Transform3D* captainsUniverseTransform = GetComponent<Transform3D>();

		captainsUniverseTransform->localPosition += captainsUniverse->direction * captainsUniverse->velocity * Time::GetDeltaTimeSeconds();

		m_Timer += Time::GetDeltaTimeSeconds();


		if (m_Timer >= captainsUniverse->lifeTime)
		{
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			SetExplosion();
			em.DestroyEntity(m_EntityId);
		}
	}


	void CaptainsUniverseEnergySystem::OnDestroy()
	{

	}

	void CaptainsUniverseEnergySystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		SetExplosion();
		em.DestroyEntity(m_EntityId);
	}

	void CaptainsUniverseEnergySystem::SetExplosion()
	{
		EntityManager& em = GetEntityManager();
		EntityId explosionId = em.LoadPrefab("assets/Prefabs/AbilitiesPrefabs/P_CaptainUniverse_Explosion.wiprefab");
		Transform3D* selfTr = GetTransform();
		Transform3D* explosionTr = (Transform3D*)em.GetComponentByIterator(GetComponentIterator<Transform3D>(explosionId));

		*explosionTr = *selfTr;
		explosionTr->localPosition = Math::GetWorldPosition(selfTr->worldMatrix);
		explosionTr->localRotation = glm::vec3(-90.0f, 0.0f, 90.0f);
		explosionTr->localScale = selfTr->localScale;

		PhysicsSystem* physSys = em.GetSystem<PhysicsSystem>(explosionId);
		physSys->ForceSetPosition(Math::GetWorldPosition(explosionTr->worldMatrix));
	}
}