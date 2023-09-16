#include <wipch.h>
#include "BossUltronSecondDash.h"
#include "../BossUltron.h"
#include "../../../../components/attack/DashEffect.h"
#include "../../../attack/SecondDashSystem.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

namespace Wiwa
{
	BossUltronSecondDashState::BossUltronSecondDashState()
	{

	}

	BossUltronSecondDashState::~BossUltronSecondDashState()
	{

	}

	void BossUltronSecondDashState::EnterState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);
		if (navAgent != nullptr)
		{
			navAgent->autoRotate = false;
		}

		enemy->m_NavAgentSys->StopAgent();

		m_SpawnDashEffect = true;
		m_CollisionWall = false;
		m_TimerToLookAtPlayer = 0.0f;
		m_TimerAfterDash = 0.0f;
		m_TimerOfDashAction = 0.0f;

		enemy->m_AnimatorSys->PlayAnimation("dash");

		m_SecondDashState = Wiwa::BossUltronSecondDashState::SecondDashState::PREPARE_DASH;
	}

	void BossUltronSecondDashState::UpdateState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);

		switch (m_SecondDashState)
		{
		case Wiwa::BossUltronSecondDashState::SecondDashState::PREPARE_DASH:
		{
			m_TimerToLookAtPlayer += Time::GetDeltaTimeSeconds();

			enemy->LookAt(playerTr->localPosition, 300.0f);

			if (m_TimerToLookAtPlayer >= 0.4f) //Time to Look At player
			{
				enemy->m_NavAgentSys->StopAgent();
				navAgent->autoRotate = false;

				m_TimerToLookAtPlayer = 0.0f;
				m_TimerOfDashAction = 0.0f;
				m_SecondDashState = SecondDashState::PLAY_DASH;
			}
		}
		break;
		case Wiwa::BossUltronSecondDashState::SecondDashState::PLAY_DASH:
		{
			Transform3D* gunTr = (Transform3D*)em.GetComponentByIterator(enemy->m_GunTransformIt);
			Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(em.GetComponentIterator<Transform3D>(enemy->GetEntity()));

			m_TimerOfDashAction += Time::GetDeltaTimeSeconds();

			//First Play Animation
			if (m_SpawnDashEffect)
			{
				enemy->m_NavAgentSys->StopAgent();

				SpawnDashEffect(enemy, CalculateForward(*gunTr));

				/*glm::vec3 outOfScene = glm::vec3(100.0f, 100.0f, 100.0f);*/
				enemy->m_NavAgentSys->RemoveAgent();

				selfTr->localPosition.x = 100.0f;
	
				m_SpawnDashEffect = false;
			}
			
			if (m_TimerOfDashAction >= 1.0f || m_CollisionWall) //Bullet Lifetime
			{
				m_TimerOfDashAction = 0.0f;
				m_SecondDashState = SecondDashState::END_DASH;
			}
		}
		break;
		case Wiwa::BossUltronSecondDashState::SecondDashState::END_DASH:
		{
			
			navAgent->autoRotate = true;

			m_TimerAfterDash = 0.0f;
			enemy->SwitchState(enemy->m_MovementState);
		}
		break;
		default:
			break;
		}
	}

	void BossUltronSecondDashState::ExitState(BossUltron* enemy)
	{
	}

	void BossUltronSecondDashState::OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2)
	{
		std::string wallStr = "WALL";
		if (body1->id == enemy->GetEntity() && wallStr == body2->selfTagStr)
		{
			m_CollisionWall = true;
		}
	}

	bool BossUltronSecondDashState::SpawnDashEffect(BossUltron* enemy, const glm::vec3& bull_dir)
	{

		if (GameStateManager::s_PoolManager->s_UltronSecondDashPool->getCountDisabled() <= 0)
			return false;

		Wiwa::EntityManager& entityManager = enemy->getScene().GetEntityManager();
		GameStateManager::s_PoolManager->SetScene(&enemy->getScene());
		EntityId newBulletId = GameStateManager::s_PoolManager->s_UltronSecondDashPool->GetFromPool();
		//entityManager.RemoveSystem(newBulletId, physicsSystemHash);
		Wiwa::SecondDashSystem* dashSystem = entityManager.GetSystem<Wiwa::SecondDashSystem>(newBulletId);
		Wiwa::PhysicsSystem* physSys = entityManager.GetSystem<PhysicsSystem>(newBulletId);

		/*enemy->m_AnimatorSys->PlayAnimation("dash");*/

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
		//bulletTr->localScale = transform->localScale;
		DashEffect* bullet = (DashEffect*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<DashEffect>(newBulletId));
		bullet->damage = 40; //TODO: Add to components
		bullet->direction = bull_dir;

		physSys->CreateBody();

		dashSystem->EnableBullet(enemy);

		return true;
	}

	glm::vec3 BossUltronSecondDashState::CalculateForward(const Transform3D& t3d)
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