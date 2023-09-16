#include "YondusFinSystem.h"
#include "../../components/attack/YondusFin.h"
#include <Wiwa/ecs/components/game/enemy/Enemy.h>
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/game/Items/ItemManager.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/ecs/components/game/wave/WaveSpawner.h>
#include <Wiwa/ecs/systems/game/wave/WaveSpawnerSystem.h>
#include <Wiwa/ecs/systems/game/wave/WaveSystem.h>
#include <Wiwa/ecs/components/ai/NavAgent.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include "../enemy/EnemySystem.h"
#include "../enemy/MeleePhalanx/EnemyMeleePhalanx.h"
#include "../enemy/RangedPhalanx/EnemyRangedPhalanx.h"
#include "../enemy/Sentinel/EnemySentinel.h"
#include "../enemy/Subjugator/EnemySubjugator.h"
#include "../../components/attack/Attack.h"
namespace Wiwa
{
	YondusFinSystem::YondusFinSystem()
	{
		m_YondusFinIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
		m_Timer = 0.0f;
		currentWantedEnemies = 0;
	}

	YondusFinSystem::~YondusFinSystem()
	{

	}

	void YondusFinSystem::OnAwake()
	{
		m_YondusFinIt = GetComponentIterator<YondusFin>();
	}

	void YondusFinSystem::OnInit()
	{
		if (!getAwake())
			System::Awake();

		YondusFin* yondus_fin = GetComponentByIterator<YondusFin>(m_YondusFinIt);
		Attack* attack = GetComponentByIterator<Attack>(GetComponentIterator<Attack>(m_EntityId));
		if (attack)
		{
			strcpy(attack->attackType, "YONDUS_FIN");
		}

		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		yondus_fin->damage = Wiwa::ItemManager::GetAbility("Yondu's Fin")->Damage;
		yondus_fin->lifeTime = 5.0f;

		EntityId player = GameStateManager::GetPlayerId();

		m_PlayerTransformIt = em.GetComponentIterator<Transform3D>(player);
		m_NavAgentIt = GetComponentIterator<NavAgent>();

		// Get the first and only spawner in scene
		size_t size = 0;
		Wiwa::WaveSpawner* waveSpawner = nullptr;
		waveSpawner = em.GetComponents<WaveSpawner>(&size);
		if (waveSpawner){
			if (em.IsComponentRemoved<WaveSpawner>(0))
				return;

			waveSpawner = &waveSpawner[0];
			if (waveSpawner){

				// Check for all the active waves in that spawner.
				WaveSpawnerSystem* waveSpawnerSystem = em.GetSystem<WaveSpawnerSystem>(waveSpawner->entityId);
				if (waveSpawnerSystem) {
					const std::vector<EntityId>& waveIds = waveSpawnerSystem->getWaveIds();
					for (int i = 0; i < waveIds.size(); i++)
					{
						if (currentWantedEnemies >= maximumWantedEnemies)
							break;

						Wave* wave = em.GetComponent<Wave>(waveIds[i]);
						if (wave && wave->currentEnemiesAlive > 0) {

							// Store all the enemies alive in that wave
							WaveSystem* waveSystem = em.GetSystem<WaveSystem>(waveIds[i]);
							if (waveSystem) {

								const std::vector<EntityId>& enemiesIds = waveSystem->getEnemiesIds();
								for (int j = 0; j < enemiesIds.size(); j++)
								{
									if (currentWantedEnemies >= maximumWantedEnemies)
										break;

									m_EnemiesIds.emplace_back(enemiesIds[j]);
									m_EnemiesTransformIt.emplace_back(em.GetComponentIterator<Transform3D>(enemiesIds[j]));
									m_EnemiesStateIt.emplace_back(em.GetComponentIterator<EnemyState>(enemiesIds[j]));

									currentWantedEnemies++;
								}
							}
						}
					}
				}
			}
		}

		Transform3D* playerTransform = GetComponentByIterator<Transform3D>(m_PlayerTransformIt);
		Transform3D* selfTr = GetTransform();
		*selfTr = *playerTransform;

		// Re order enemies transforms from closest to farhtest
		int totalEnemies = m_EnemiesTransformIt.size();
		for (int i = 1; i < totalEnemies; ++i) {
			glm::vec3 key = Math::GetWorldPosition(GetComponentByIterator<Transform3D>(m_EnemiesTransformIt[i])->worldMatrix);

			EntityManager::ComponentIterator keyIt = m_EnemiesTransformIt[i];

			const glm::vec3 position = Math::GetWorldPosition(selfTr->worldMatrix);
			int j = i - 1;
			while (j >= 0 && glm::distance(position, Math::GetWorldPosition(GetComponentByIterator<Transform3D>(m_EnemiesTransformIt[j])->worldMatrix)) > glm::distance(position, key)) {
				m_EnemiesTransformIt[j + 1] = m_EnemiesTransformIt[j];
				m_EnemiesIds[j + 1] = m_EnemiesIds[j];
				m_EnemiesStateIt[j + 1] = m_EnemiesStateIt[j];
				j--;
			}

			m_EnemiesTransformIt[j + 1] = keyIt;
			m_EnemiesIds[j + 1] = m_EnemiesIds[i];
			m_EnemiesStateIt[j + 1] = m_EnemiesStateIt[i];
		}

		AudioSystem* audio = em.GetSystem<Wiwa::AudioSystem>(m_EntityId);
		if (audio)
			audio->PlayAudio("yondus_fin");
	}

	void YondusFinSystem::OnUpdate()
	{
		if (!getAwake())
			System::Awake();
		if (!getInit())
			System::Init();

		if (m_EnemiesTransformIt.size() <= 0)
			return;

		EntityManager& em = GetEntityManager();
		YondusFin* yondusFin = GetComponentByIterator<YondusFin>(m_YondusFinIt);
		Transform3D* playerTransform = GetComponentByIterator<Transform3D>(m_PlayerTransformIt);
		Transform3D* selfTr = GetTransform();

		if (m_Timer >= yondusFin->lifeTime)
		{
			em.DestroyEntity(m_EntityId);
			return;
		}
		
		NavAgentSystem* agentSys = em.GetSystem<NavAgentSystem>(m_EntityId);
		NavAgent* agent = GetComponentByIterator<NavAgent>(m_NavAgentIt);
		agent->agentSliding = false;

		glm::vec3 currentEnemyPosition = Math::GetWorldPosition(GetComponentByIterator<Transform3D>(m_EnemiesTransformIt[0])->worldMatrix);
		if (Math::IsPointNear(selfTr->localPosition, currentEnemyPosition, 1.0f))
		{
		/*	agentSys->StopAgent();
			agentSys->SetPosition(selfTr->localPosition);*/
			m_EnemiesIds.erase(m_EnemiesIds.begin());
			m_EnemiesTransformIt.erase(m_EnemiesTransformIt.begin());
			m_EnemiesStateIt.erase(m_EnemiesStateIt.begin());
			currentEnemyPosition = Math::GetWorldPosition(GetComponentByIterator<Transform3D>(m_EnemiesTransformIt[0])->worldMatrix);
		}

		if (agentSys && m_EnemiesTransformIt.size() > 0) {
			agentSys->m_RotationValue = 60.0f;
			agentSys->SetDestination(currentEnemyPosition);
		}

		if (m_EnemiesTransformIt.size() <= 0) {
			em.DestroyEntity(m_EntityId);
		}

		m_Timer += Time::GetDeltaTimeSeconds();
	}

	void YondusFinSystem::OnDestroy()
	{

	}

	void YondusFinSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		WI_INFO("Yondusss collision!!!");
	}

}