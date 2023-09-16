#include <wipch.h>
#include "TransitionSecondPhase.h"
#include "../BossUltron.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/PhysicsSystem.h>
#include <Wiwa/ecs/components/game/Health.h>
#include "../../../../components/enemy/BossMovementPoints.h"
#include "../../../../systems/enemy/EnemySystem.h"

namespace Wiwa
{
	BossUltronSecondPhaseState::BossUltronSecondPhaseState()
	{
		m_WallPrefabPath = "assets\\Enemy\\Ultron\\UltronRegenWall_01.wiprefab";
		m_EnemySpawnerPath = "assets\\Enemy\\WaveSpawner\\UltronWaveSpawner_01.wiprefab";
		m_RegenPath = "assets/vfx/prefabs/vfx_finals/boss_Ultron/p_boss_regen.wiprefab";
		/*m_SecondPhasePath = "assets/vfx/prefabs/vfx_finals/boss_Ultron/ultron_second_phase.wiprefab";*/
	}

	BossUltronSecondPhaseState::~BossUltronSecondPhaseState()
	{

	}

	void BossUltronSecondPhaseState::EnterState(BossUltron* enemy)
	{
		m_TimerSecondPhase = 0.0f;
		m_TimerHealing = 0.0f;
		m_TimerSpawnerEnemies = 0.0f;
		m_SpawnEnemies = false;
		m_SpawnerDestroyed = false;

		m_AfterRegenPosition.clear();
		FillPremadePositionAfterRegen(enemy, m_AfterRegenPosition);
		m_SecondPhaseState = Wiwa::BossUltronSecondPhaseState::SecondPhaseState::MOVE_CENTER;
	}

	void BossUltronSecondPhaseState::UpdateState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);
		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);
		Health* stats = (Health*)em.GetComponentByIterator(enemy->m_Health);

		switch (m_SecondPhaseState)
		{
		case Wiwa::BossUltronSecondPhaseState::SecondPhaseState::MOVE_CENTER:
		{
			glm::vec3 m_CenterPositionBoss = { 0.0f, 6.0f,0.0f };
			glm::vec3 m_CenterPositionRegenWall = { 0.0f, 13.0f,0.0f };
			enemy->m_NavAgentSys->StopAgent();
			enemy->m_NavAgentSys->RemoveAgent();
			selfTr->localPosition = m_CenterPositionBoss;

			m_RegenWallPrefabId = em.LoadPrefab(m_WallPrefabPath);
			Transform3D* regenWallPrefabTr = em.GetComponent<Transform3D>(m_RegenWallPrefabId);
			

			PhysicsSystem* physSysWallRegen = em.GetSystem<PhysicsSystem>(m_RegenWallPrefabId);
			physSysWallRegen->DeleteBody();

			enemy->m_AnimatorSys->PlayAnimation("idle");

			selfTr->localRotation = glm::vec3(0.0f, 180.0f, 0.0f);

			if (!regenWallPrefabTr || !playerTr)
				return;

			regenWallPrefabTr->localPosition = m_CenterPositionRegenWall;
			regenWallPrefabTr->localRotation = glm::vec3(-90.0f, 0.0f, playerTr->localRotation.y + 90.0f);
			regenWallPrefabTr->localScale = selfTr->localScale;

			physSysWallRegen->CreateBody();

			m_TimerSecondPhase = 0.0f;

			m_SpawnEnemies = true;

			//Particle Regeneration
			m_RegenPrefabId = em.LoadPrefab(m_RegenPath);
			Transform3D* regenTr = em.GetComponent<Transform3D>(m_RegenPrefabId);
			regenTr->localPosition.x = selfTr->localPosition.x;
			regenTr->localPosition.y = selfTr->localPosition.y + 9.0f;
			regenTr->localPosition.z = selfTr->localPosition.z;


			m_SecondPhaseState = SecondPhaseState::REGENERATE;
		}
		break;
		case Wiwa::BossUltronSecondPhaseState::SecondPhaseState::REGENERATE:
		{
			m_TimerSecondPhase += Time::GetDeltaTimeSeconds();
			m_TimerHealing += Time::GetDeltaTimeSeconds();
			m_TimerSpawnerEnemies += Time::GetDeltaTimeSeconds();

			if (m_TimerHealing >= 1.0f)
			{
				if (enemy->m_MaxHealth <= stats->health)
				{
					stats->health = enemy->m_MaxHealth;
				}
				else
				{
					stats->health = stats->health + 30.0f;
				}

				m_TimerHealing = 0.0f;
			}

			if (m_SpawnEnemies)
			{
				//Add enemies
				m_SpawnerPrefabId = em.LoadPrefab(m_EnemySpawnerPath);
				m_SpawnEnemies = false;
			}

			if (m_TimerSpawnerEnemies >= 10.0f && m_SpawnerDestroyed == false)
			{
				em.DestroyEntity(m_SpawnerPrefabId);
				m_SpawnerDestroyed = true;
			}

			if (m_TimerSecondPhase >= 20.0f)
			{
				m_TimerSecondPhase = 0.0f;
				m_SecondPhaseState = SecondPhaseState::END_STATE;
			}
		}
		break;
		case Wiwa::BossUltronSecondPhaseState::SecondPhaseState::END_STATE:
		{

			//Destroy wall
			em.DestroyEntity(m_RegenWallPrefabId);

			selfTr->localPosition = GetNewPositionAfterRegen();
			enemy->m_NavAgentSys->RegisterWithCrowd();
			enemy->m_NavAgentSys->SetPosition(selfTr->localPosition);
			enemy->m_NavAgentSys->StopAgent();

			////Create Second Phase Particle
			//enemy->m_SecondPhaseId = em.LoadPrefab(m_SecondPhasePath);
			//Transform3D* secondPhaseTr = em.GetComponent<Transform3D>(enemy->m_SecondPhaseId);
			//secondPhaseTr->localPosition = selfTr->localPosition;

			enemy->SwitchState(enemy->m_MovementState);
		}
		break;
		
		default:
			break;
		}
	}

	void BossUltronSecondPhaseState::ExitState(BossUltron* enemy)
	{
	}

	void BossUltronSecondPhaseState::OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2)
	{
	}

	void BossUltronSecondPhaseState::FillPremadePositionAfterRegen(BossUltron* enemy, std::vector<glm::vec3>& vec)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		size_t size = 0;
		Wiwa::BossMovementPoints* pointsList = nullptr;
		pointsList = em.GetComponents<BossMovementPoints>(&size);
		if (pointsList)
		{
			for (int i = 0; i < size; i++)
			{
				if (em.IsComponentRemoved<BossMovementPoints>(i)) {
				}
				else {
					Wiwa::BossMovementPoints* point = &pointsList[i];
					if (point)
					{
						vec.emplace_back(point->point);

						if (point->isCenter)
							enemy->m_SceneCenterPos = point->point;
					}
				}
			}
		}
	}

	glm::vec3 BossUltronSecondPhaseState::GetNewPositionAfterRegen()
	{
		std::uniform_int_distribution<> disEnemies(0, m_AfterRegenPosition.size() - 1);
		int randomNum = disEnemies(Application::s_Gen);
		return m_AfterRegenPosition[randomNum];
	}
}