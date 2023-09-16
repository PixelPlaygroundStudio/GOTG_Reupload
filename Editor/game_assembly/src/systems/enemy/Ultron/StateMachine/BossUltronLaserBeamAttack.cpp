#include <wipch.h>
#include "BossUltronLaserBeamAttack.h"
#include "../BossUltron.h"
#include "../../../../components/attack/UltronLaserBeam.h"
#include "../../../attack/UltronLaserBeamSystem.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include "../../../../components/enemy/BossMovementPoints.h"
#include "../../../attack/UltronSmashExplosionSystem.h"
#include "../../../../components/attack/Explosion.h"
#include <Wiwa/ecs/systems/AudioSystem.h>

namespace Wiwa
{
	BossUltronLaserBeamAttackState::BossUltronLaserBeamAttackState()
	{
		m_TimerLaser = 0.0f;
		m_PreSmashMarkPath = "assets/vfx/prefabs/vfx_finals/Smash/p_boss_smash_01_marker.wiprefab";
		m_LaserProtectionPath = "assets/Enemy/UltronLaserBeam/LaserBeamProtection_01.wiprefab";
	}

	BossUltronLaserBeamAttackState::~BossUltronLaserBeamAttackState()
	{

	}

	void BossUltronLaserBeamAttackState::EnterState(BossUltron* enemy)
	{
		m_TimerLaser = 0.0f;
		m_TimerToRotate = 0.0f;
		centerPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		m_TimerDash = 0.0f;
		m_TimerToStopDash = 0.0f;
		m_TimerAfterDash = 0.0f;
		m_MoveUpwardsCounter = 0.0f;
		m_ActivateLaserProtection = false;

		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);

		enemy->m_AnimatorSys->PlayAnimation("smash_up");

		m_AfterLaserBeamPosition.clear();
		FillPremadePositionAfterLaser(enemy, m_AfterLaserBeamPosition);

		laserState = Wiwa::BossUltronLaserBeamAttackState::LaserState::SMASH_INIT;
	}

	void BossUltronLaserBeamAttackState::UpdateState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);
		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);

		m_TimerLaser += Time::GetDeltaTimeSeconds();
		
		switch (laserState)
		{
		case Wiwa::BossUltronLaserBeamAttackState::LaserState::SMASH_INIT:
		{
			enemy->m_NavAgentSys->StopAgent();
			navAgent->autoRotate = false;

			m_TimerToRotate += Time::GetDeltaTimeSeconds();

			enemy->LookAt(playerTr->localPosition, 80.0f);

			if (m_TimerToRotate >= 1.6f) //Timer to look at player
			{
				m_TimerToRotate = 0.0f;
				m_MoveUpwardsCounter = 0.0f;
				m_TimerToStopDash = 0.0f;
				m_UltronJump = false;
				enemy->m_NavAgentSys->StopAgent();
				enemy->m_NavAgentSys->RemoveAgent();

				laserState = LaserState::SMASH_GO_UP;
			}
		}
		break;
		case Wiwa::BossUltronLaserBeamAttackState::LaserState::SMASH_GO_UP:
		{
			Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(enemy->GetEntity()));

			m_TimerDash += Time::GetDeltaTimeSeconds();
			m_TimerToStopDash += Time::GetDeltaTimeSeconds();

			if (m_TimerDash >= TIMER_GO_UPWARDS_LASER)
			{
				m_TimerDash = 0.0f;
				m_MoveUpwardsCounter = m_MoveUpwardsCounter + 1.0f; //Offset for the Smash
				selfTr->localPosition.y = m_MoveUpwardsCounter;
			}

			if (m_TimerToStopDash >= 1.2f) //Timer for the Ultron to go up
			{
				/*enemy->m_NavAgentSys->RegisterWithCrowd();
				enemy->m_NavAgentSys->SetPosition(playerDistance);
				enemy->m_NavAgentSys->StopAgent();
				enemy->m_NavAgentSys->RemoveAgent();*/

				selfTr->localPosition.x = centerPoint.x;
				selfTr->localPosition.z = centerPoint.z;

				m_PreSmashMarkId = em.LoadPrefab(m_PreSmashMarkPath);
				Transform3D* thunderMarkTr = em.GetComponent<Transform3D>(m_PreSmashMarkId);

				thunderMarkTr->localPosition.x = centerPoint.x;
				thunderMarkTr->localPosition.y = 0.1f;
				thunderMarkTr->localPosition.z = centerPoint.z;

				WI_INFO("INTERPOLATION DONE");
				laserState = LaserState::SMASH_GO_DOWN;
			}
		}
		break;
		case Wiwa::BossUltronLaserBeamAttackState::LaserState::SMASH_GO_DOWN:
		{
			Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(enemy->GetEntity()));

			m_TimerDash += Time::GetDeltaTimeSeconds();

			if (m_TimerDash >= TIMER_GO_DOWNWARDS_LASER)
			{
				m_TimerDash = 0.0f;
				m_MoveUpwardsCounter = m_MoveUpwardsCounter - 1.0f;
				selfTr->localPosition.y = m_MoveUpwardsCounter;
			}

			if (selfTr->localPosition.y <= 6.0f && m_ActivateLaserProtection == false)
			{
				m_LaserProtectionId = em.LoadPrefab(m_LaserProtectionPath);
				Transform3D* laserProtectionTr = em.GetComponent<Transform3D>(m_LaserProtectionId);

				laserProtectionTr->localPosition.x = centerPoint.x;
				laserProtectionTr->localPosition.y = 0.2f;
				laserProtectionTr->localPosition.z = centerPoint.z;

				m_ActivateLaserProtection = true;
			}

			if (selfTr->localPosition.y <= 0.05f)
			{
				laserState = LaserState::SMASH_EXPLOSION;
			}
		}
		break;
		case Wiwa::BossUltronLaserBeamAttackState::LaserState::SMASH_EXPLOSION:
		{
			em.DestroyEntity(m_PreSmashMarkId);

			enemy->m_AnimatorSys->PlayAnimation("smash_down");
			enemy->m_AudioSys->PlayAudio("boss_smash_impact");

			enemy->m_NavAgentSys->RegisterWithCrowd();
			enemy->m_NavAgentSys->SetPosition(centerPoint);
			enemy->m_NavAgentSys->StopAgent();

			Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
			Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);

			SpawnExplosionAfterSmash(enemy, selfTr);

			navAgent->autoRotate = true;

			laserState = LaserState::MOVE_CENTER;
		}
		break;
		case Wiwa::BossUltronLaserBeamAttackState::LaserState::MOVE_CENTER:
		{
			enemy->m_NavAgentSys->StopAgent();
			enemy->m_NavAgentSys->RemoveAgent();

			enemy->m_AnimatorSys->PlayAnimation("ray_together");

			m_TimerLaser = 0.0f;
			laserState = LaserState::PREPARE_LASER;
		}
		break;
		case Wiwa::BossUltronLaserBeamAttackState::LaserState::PREPARE_LASER:
		{
			/*enemy->m_NavAgentSys->StopAgent();*/
			enemy->LookAt(playerTr->localPosition, 80.0f);

			if (m_TimerLaser >= 1.5f)
			{
				
				enemy->m_AudioSys->PlayAudio("boss_laser");
				laserState = LaserState::LASER_ATTACK;
				m_TimerLaser = 0.0f;
			}
			
		}
		break;
		case Wiwa::BossUltronLaserBeamAttackState::LaserState::LASER_ATTACK:
		{
			enemy->LookAt(playerTr->localPosition, 50.0f);

			/*navAgent->autoRotate = false;*/
			SpawnLaserBeam(enemy, CalculateForward(*selfTr));

			if (m_TimerLaser >= 5.0f)
			{		
				laserState = LaserState::END_STATE;
				m_TimerLaser = 0.0f;
			}
		}
		break;
		case Wiwa::BossUltronLaserBeamAttackState::LaserState::END_STATE:
		{
			

			if (m_TimerLaser >= 1.0f)
			{
				/*navAgent->autoRotate = true;
				enemy->m_NavAgentSys->StopAgent();*/
				/*enemy->m_NavAgentSys->StopAgent();
				enemy->m_NavAgentSys->RemoveAgent();*/

				em.DestroyEntity(m_LaserProtectionId);

				selfTr->localPosition = GetNewPositionAfterLaser();
				/*enemy->m_NavAgentSys->SetPosition(GetNewPositionAfterLaser());*/
				m_TimerLaser = 0.0f;

				enemy->m_NavAgentSys->RegisterWithCrowd();
				enemy->m_NavAgentSys->SetPosition(selfTr->localPosition);
				enemy->m_NavAgentSys->StopAgent();
				enemy->SwitchState(enemy->m_MovementState);	
			}
		}
		break;
		default:
			break;
		}

	}

	void BossUltronLaserBeamAttackState::ExitState(BossUltron* enemy)
	{

	}

	void BossUltronLaserBeamAttackState::OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2)
	{

	}

	EntityId BossUltronLaserBeamAttackState::SpawnLaserBeam(BossUltron* enemy, const glm::vec3& bull_dir)
	{
		/*if (GameStateManager::s_PoolManager->s_UltronLaserBeamPool->getCountDisabled() <= 0)
			return EntityManager::INVALID_INDEX;*/

		Wiwa::EntityManager& entityManager = enemy->getScene().GetEntityManager();
		EntityId newBulletId = GameStateManager::s_PoolManager->s_UltronLaserBeamPool->GetFromPool();
		//entityManager.RemoveSystem(newBulletId, physicsSystemHash);

		if (newBulletId == EntityManager::INVALID_INDEX)
			return EntityManager::INVALID_INDEX;

		// Set intial positions
		Transform3D* bulletTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(newBulletId));
		Transform3D* enemyTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(enemy->GetEntity()));


		if (!bulletTr || !enemyTr)
			return newBulletId;

		//Math::CalculateForward(enemyTr);

		glm::vec3 spawnPosition = enemyTr->localPosition;
		spawnPosition.y += 3.0f;

		bulletTr->localPosition = spawnPosition;
		bulletTr->localRotation = glm::vec3(-90.0f, 0.0f, bull_dir.y + 90.0f);

		/*bulletTr->localRotation = glm::vec3(-90.0f, 0.0f, playerTr->localRotation.y + 90.0f);*/

		//bulletTr->localScale = transform->localScale;
		UltronLaserBeam* bullet = (UltronLaserBeam*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<UltronLaserBeam>(newBulletId));
		bullet->direction = bull_dir;
		Wiwa::UltronLaserBeamSystem* laserSystem = entityManager.GetSystem<Wiwa::UltronLaserBeamSystem>(newBulletId);
		Wiwa::PhysicsSystem* physSys = entityManager.GetSystem<PhysicsSystem>(newBulletId);

		physSys->CreateBody();
		laserSystem->EnableLaser();

		return newBulletId;
	}

	glm::vec3 BossUltronLaserBeamAttackState::CalculateForward(const Transform3D& t3d)
	{
		/*glm::vec4 forward = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
		glm::vec4 transformed = t3d.worldMatrix * forward;
		return glm::normalize(glm::vec3(transformed));*/
		glm::vec3 rotrad = glm::radians(t3d.rotation);

		glm::vec3 forward;

		forward.x = glm::cos(rotrad.x) * glm::sin(rotrad.y);
		forward.y = -glm::sin(rotrad.x);
		forward.z = glm::cos(rotrad.x) * glm::cos(rotrad.y);

		forward = glm::degrees(forward);

		return glm::normalize(forward);
	}

	void BossUltronLaserBeamAttackState::FillPremadePositionAfterLaser(BossUltron* enemy, std::vector<glm::vec3>& vec)
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

	glm::vec3 BossUltronLaserBeamAttackState::GetNewPositionAfterLaser()
	{
		std::uniform_int_distribution<> disEnemies(0, m_AfterLaserBeamPosition.size() - 1);
		int randomNum = disEnemies(Application::s_Gen);
		return m_AfterLaserBeamPosition[randomNum];
	}

	void BossUltronLaserBeamAttackState::SpawnExplosionAfterSmash(BossUltron* enemy, Transform3D* selfTransform)
	{
		/*if (GameStateManager::s_PoolManager->s_ExplosiveBarrel->getCountDisabled() <= 0)
			return;*/

		Wiwa::EntityManager& entityManager = enemy->getScene().GetEntityManager();
		GameStateManager::s_PoolManager->SetScene(&enemy->getScene());
		EntityId newExplosionId = EntityManager::INVALID_INDEX;
		newExplosionId = GameStateManager::s_PoolManager->s_SmashExplosionPool->GetFromPool();

		if (newExplosionId == EntityManager::INVALID_INDEX)
			return;

		UltronSmashExplosionSystem* explosionSys = entityManager.GetSystem<UltronSmashExplosionSystem>(newExplosionId);
		PhysicsSystem* physSys = entityManager.GetSystem<PhysicsSystem>(newExplosionId);
		physSys->DeleteBody();

		// Set intial positions
		//Transform3D* playerTr = (Transform3D*)entityManager.GetComponentByIterator(enemy->m_TransformIt); //Transform
		Transform3D* explosionTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(newExplosionId));
		Explosion* explosion = (Explosion*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Explosion>(newExplosionId));
		explosion->isFromPool = true;

		if (!explosionTr)
			return;

		explosionTr->localPosition.x = Math::GetWorldPosition(selfTransform->worldMatrix).x;
		explosionTr->localPosition.y = Math::GetWorldPosition(selfTransform->worldMatrix).y;
		explosionTr->localPosition.z = Math::GetWorldPosition(selfTransform->worldMatrix).z;

		physSys->CreateBody();

		explosionSys->EnableExplosion();

		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		EntityId p_explosion = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/boss_Ultron/p_boss_smash.wiprefab");

		if (p_explosion != EntityManager::INVALID_INDEX)
		{
			Transform3D* p_exT = em.GetComponent<Transform3D>(p_explosion);
			if (p_exT != nullptr)
			{
				p_exT->localPosition = explosionTr->localPosition;
			}
		}
	}
}

