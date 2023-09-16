#include <wipch.h>
#include "BossUltronClusterShotsAttack.h"
#include "../BossUltron.h"
#include "../../../../components/attack/ClusterBullet.h"
#include "../../../attack/ClusterBulletSystem.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>

namespace Wiwa
{
	BossUltronClusterShotsAttackState::BossUltronClusterShotsAttackState()
	{

	}

	BossUltronClusterShotsAttackState::~BossUltronClusterShotsAttackState()
	{

	}

	void BossUltronClusterShotsAttackState::EnterState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();

		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);
		if (navAgent != nullptr)
		{
			navAgent->autoRotate = false;
		}

		enemy->m_NavAgentSys->StopAgent();

		m_TimerBetweenBullet = 0.0f;
		m_TimerToLookAtPlayer = 0.0f;
		m_AlreadyShot = false;

		enemy->m_AnimatorSys->PlayAnimation("onearm_together");

		clusterState = Wiwa::BossUltronClusterShotsAttackState::ClusterState::FIRST_ATTACK;
	}

	void BossUltronClusterShotsAttackState::UpdateState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);

		m_TimerBetweenBullet += Time::GetDeltaTimeSeconds();
		m_TimerToLookAtPlayer += Time::GetDeltaTimeSeconds();

		switch (clusterState)
		{
		case Wiwa::BossUltronClusterShotsAttackState::ClusterState::FIRST_ATTACK:
		{
			enemy->m_NavAgentSys->StopAgent();
			//Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
			Transform3D* gunTr = (Transform3D*)em.GetComponentByIterator(enemy->m_GunTransformIt);

			enemy->LookAt(playerTr->localPosition, 90.0f);

			if (m_TimerToLookAtPlayer >= 1.0f && m_AlreadyShot == false)
			{
				/*enemy->m_AnimatorSys->PlayAnimation("onearm_together");*/
				SpawnClusterBullet(enemy, CalculateForward(*gunTr));
				m_AlreadyShot = true;
			}

			if (m_TimerBetweenBullet >= TIME_BETWEEN_CLUSTER_BULLET)
			{
				m_TimerToLookAtPlayer = 0.0f;
				m_TimerBetweenBullet = 0.0f;
				m_AlreadyShot = false;
				clusterState = ClusterState::SECOND_ATTACK;
			}
		}
		break;
		case Wiwa::BossUltronClusterShotsAttackState::ClusterState::SECOND_ATTACK:
		{

			enemy->m_NavAgentSys->StopAgent();
			//Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
			Transform3D* gunTr = (Transform3D*)em.GetComponentByIterator(enemy->m_GunTransformIt);

			enemy->LookAt(playerTr->localPosition, 90.0f);

			if (m_TimerToLookAtPlayer >= 1.0f && m_AlreadyShot == false)
			{
				/*enemy->m_AnimatorSys->PlayAnimation("onearm_together");*/
				SpawnClusterBullet(enemy, CalculateForward(*gunTr));
				m_AlreadyShot = true;
			}

			if (m_TimerBetweenBullet >= TIME_BETWEEN_CLUSTER_BULLET)
			{
				m_TimerToLookAtPlayer = 0.0f;
				m_TimerBetweenBullet = 0.0f;
				m_AlreadyShot = false;
				clusterState = ClusterState::THIRD_ATTACK;
			}
		}
		break;
		case Wiwa::BossUltronClusterShotsAttackState::ClusterState::THIRD_ATTACK:
		{
			enemy->m_NavAgentSys->StopAgent();
			//Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
			Transform3D* gunTr = (Transform3D*)em.GetComponentByIterator(enemy->m_GunTransformIt);

			enemy->LookAt(playerTr->localPosition, 90.0f);

			if (m_TimerToLookAtPlayer >= 1.0f && m_AlreadyShot == false)
			{
				/*enemy->m_AnimatorSys->PlayAnimation("onearm_together");*/

				SpawnClusterBullet(enemy, CalculateForward(*gunTr));
				m_AlreadyShot = true;
			}

			if (m_TimerBetweenBullet >= TIME_BETWEEN_CLUSTER_BULLET)
			{
				m_TimerToLookAtPlayer = 0.0f;
				m_TimerBetweenBullet = 0.0f;
				m_AlreadyShot = false;
				clusterState = ClusterState::FOURTH_ATTACK;
			}
		}
		break;
		case Wiwa::BossUltronClusterShotsAttackState::ClusterState::FOURTH_ATTACK:
		{
			enemy->m_NavAgentSys->StopAgent();
			//Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
			Transform3D* gunTr = (Transform3D*)em.GetComponentByIterator(enemy->m_GunTransformIt);

			enemy->LookAt(playerTr->localPosition, 90.0f);

			if (m_TimerToLookAtPlayer >= 1.0f && m_AlreadyShot == false)
			{
				/*enemy->m_AnimatorSys->PlayAnimation("onearm_together");*/

				SpawnClusterBullet(enemy, CalculateForward(*gunTr));
				m_AlreadyShot = true;
			}

			if (m_TimerBetweenBullet >= TIME_BETWEEN_CLUSTER_BULLET)
			{
				m_TimerToLookAtPlayer = 0.0f;
				m_TimerBetweenBullet = 0.0f;
				m_AlreadyShot = false;
				clusterState = ClusterState::FIFTH_ATTACK;
			}
		}
		break;
		case Wiwa::BossUltronClusterShotsAttackState::ClusterState::FIFTH_ATTACK:
		{
			enemy->m_NavAgentSys->StopAgent();
			//Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
			Transform3D* gunTr = (Transform3D*)em.GetComponentByIterator(enemy->m_GunTransformIt);

			enemy->LookAt(playerTr->localPosition, 90.0f);

			if (m_TimerToLookAtPlayer >= 1.0f && m_AlreadyShot == false)
			{
				/*enemy->m_AnimatorSys->PlayAnimation("onearm_together");*/

				SpawnClusterBullet(enemy, CalculateForward(*gunTr));
				m_AlreadyShot = true;
			}

			if (m_TimerBetweenBullet >= TIME_BETWEEN_CLUSTER_BULLET)
			{
				m_TimerToLookAtPlayer = 0.0f;
				m_TimerBetweenBullet = 0.0f;
				m_AlreadyShot = false;
				clusterState = ClusterState::SIXTH_ATTACK;
			}
		}
		break;
		case Wiwa::BossUltronClusterShotsAttackState::ClusterState::SIXTH_ATTACK:
		{
			enemy->m_NavAgentSys->StopAgent();
			//Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
			Transform3D* gunTr = (Transform3D*)em.GetComponentByIterator(enemy->m_GunTransformIt);

			enemy->LookAt(playerTr->localPosition, 90.0f);

			if (m_TimerToLookAtPlayer >= 1.0f && m_AlreadyShot == false)
			{
				/*enemy->m_AnimatorSys->PlayAnimation("onearm_together");*/

				SpawnClusterBullet(enemy, CalculateForward(*gunTr));
				m_AlreadyShot = true;
			}

			if (m_TimerBetweenBullet >= TIME_BETWEEN_CLUSTER_BULLET)
			{
				clusterState = ClusterState::END_STATE;
			}
		}
		break;
		case Wiwa::BossUltronClusterShotsAttackState::ClusterState::END_STATE:
		{
			m_TimerToLookAtPlayer = 0.0f;
			m_TimerBetweenBullet = 0.0f;
			m_AlreadyShot = false;
			enemy->SwitchState(enemy->m_MovementState);
		}
		break;
		default:
			break;
		}
	}

	void BossUltronClusterShotsAttackState::ExitState(BossUltron* enemy)
	{
	}

	void BossUltronClusterShotsAttackState::OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2)
	{

	}	

	bool BossUltronClusterShotsAttackState::SpawnClusterBullet(BossUltron* enemy, const glm::vec3& bull_dir)
	{

		/*if (GameStateManager::s_PoolManager->s_ClusterBulletsPool->getCountDisabled() <= 0)
			return false;*/

		Wiwa::EntityManager& entityManager = enemy->getScene().GetEntityManager();
		GameStateManager::s_PoolManager->SetScene(&enemy->getScene());
		EntityId newBulletId = GameStateManager::s_PoolManager->s_ClusterBulletsPool->GetFromPool();

		Wiwa::ClusterBulletSystem* clusterSystem = entityManager.GetSystem<Wiwa::ClusterBulletSystem>(newBulletId);
		Wiwa::PhysicsSystem* physSys = entityManager.GetSystem<PhysicsSystem>(newBulletId);

		enemy->m_AudioSys->PlayAudio("boss_cluster_ball");

		if (physSys != nullptr)
		{
			physSys->DeleteBody();
		}

		// Set intial positions
		Transform3D* bulletTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(newBulletId));
		Transform3D* enemyTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(enemy->GetEntity()));
		Transform3D* playerTr = (Transform3D*)entityManager.GetComponentByIterator(enemy->m_PlayerTransformIt);

		if (!bulletTr || !enemyTr)
			return false;
		glm::vec3 spawnPosition = enemyTr->localPosition;
		spawnPosition.y += 3.0f;

		bulletTr->localPosition = spawnPosition;
		bulletTr->localRotation = glm::vec3(-90.0f, 0.0f, playerTr->localRotation.y + 90.0f); // this to fix the cluster bullet direction, not solving it yet since debugging this is a nightmare

		ClusterBullet* bullet = (ClusterBullet*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<ClusterBullet>(newBulletId));
		Ultron* ultron = (Ultron*)entityManager.GetComponentByIterator(enemy->m_Ultron);

		bullet->damage = 40; 
		bullet->direction = bull_dir;
		bullet->velocity = 45;
		bullet->lifeTime = ultron->bulletLifeTime;
		
		physSys->CreateBody();

		clusterSystem->EnableBullet(enemy);
		
		return true;
	}

	glm::vec3 BossUltronClusterShotsAttackState::CalculateForward(const Transform3D& t3d)
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
}