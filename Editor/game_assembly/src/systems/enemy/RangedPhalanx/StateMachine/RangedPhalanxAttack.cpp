#include <wipch.h>
#include "RangedPhalanxAttack.h"
#include "../EnemyRangedPhalanx.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <glm/gtx/quaternion.hpp>
#include "../../../attack/SimpleBulletSystem.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include "../../../../components/attack/SimpleBullet.h"

#include <Wiwa/ecs/systems/AudioSystem.h>
#include <Wiwa/ecs/systems/ParticleSystem.h>

namespace Wiwa
{
	RangedPhalanxAttackState::RangedPhalanxAttackState()
	{
		m_Enemy = nullptr;
	}

	RangedPhalanxAttackState::~RangedPhalanxAttackState()
	{
		m_Enemy = nullptr;
	}

	void RangedPhalanxAttackState::EnterState(EnemyRangedPhalanx* enemy)
	{
		m_Enemy = enemy;

		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);

		Wiwa::OzzAnimator* animator = enemy->m_AnimatorSys->getAnimator();
		OzzAnimation* animation = animator->getAnimationByName("shot");
		animation->addKeyAction({ &RangedPhalanxAttackState::SpawnBullet, this }, 0.3f);

		EnemyData* stats = (EnemyData*)em.GetComponentByIterator(enemy->m_StatsIt);
		enemy->m_AnimatorSys->PlayAnimation("shot");
		animator->getActiveAnimation()->setPlaybackSpeed(stats->rateOfFire);

		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);
		if (navAgent) {
			navAgent->autoRotate = false;
			enemy->LookAt(playerTr->localPosition, ROTATION_SPEED);
		}
	}
	
	void RangedPhalanxAttackState::UpdateState(EnemyRangedPhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();		
		EnemyData* stats = (EnemyData*)em.GetComponentByIterator(enemy->m_StatsIt);

		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);

		float dist2Player = glm::distance(selfTr->localPosition, playerTr->localPosition);
		enemy->LookAt(playerTr->localPosition, ROTATION_SPEED);

		bool hasFinished = enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->HasFinished();
		if ((dist2Player > stats->range || enemy->m_NavAgentSys->Raycast(selfTr->localPosition, playerTr->localPosition) == false) && hasFinished)
		{
			enemy->SwitchState(enemy->m_ChasingState);
		}
		else if (hasFinished)
		{
			enemy->m_AnimatorSys->PlayAnimation("shot");
			enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->setPlaybackSpeed(stats->rateOfFire);
		}
	}
	
	void RangedPhalanxAttackState::ExitState(EnemyRangedPhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);
		if (navAgent) {
			navAgent->autoRotate = true;
		}

		Wiwa::OzzAnimator* animator = enemy->m_AnimatorSys->getAnimator();
		OzzAnimation* animation = animator->getAnimationByName("shot");
		animation->removeKeyAction({ &RangedPhalanxAttackState::SpawnBullet, this });

		m_Enemy = nullptr;
	}
	
	void RangedPhalanxAttackState::OnCollisionEnter(EnemyRangedPhalanx* enemy, const Object* body1, const Object* body2)
	{

	}

	void RangedPhalanxAttackState::SpawnBullet()
	{
		if (GameStateManager::s_PoolManager->s_SimpleBulletsPool->getCountDisabled() <= 0 || m_Enemy == nullptr)
			return;

		Wiwa::EntityManager& em = m_Enemy->getScene().GetEntityManager();
		m_Enemy->m_AudioSys->PlayAudio("ranged_attack");

		Wiwa::EntityManager& entityManager = m_Enemy->getScene().GetEntityManager();
		GameStateManager::s_PoolManager->SetScene(&m_Enemy->getScene());
		EntityId newBulletId = EntityManager::INVALID_INDEX;
		newBulletId = GameStateManager::s_PoolManager->s_SimpleBulletsPool->GetFromPool();

		if (newBulletId == EntityManager::INVALID_INDEX)
			return;

		SimpleBulletSystem* bulletSys = entityManager.GetSystem<SimpleBulletSystem>(newBulletId);
		PhysicsSystem* physSys = entityManager.GetSystem<PhysicsSystem>(newBulletId);		
		physSys->DeleteBody();

		// Set intial positions
		Transform3D* playerTr = (Transform3D*)entityManager.GetComponentByIterator(m_Enemy->m_PlayerTransformIt);
		Transform3D* bulletTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(newBulletId));
		Transform3D* gunTr = (Transform3D*)em.GetComponentByIterator(m_Enemy->m_GunTransformIt);

		if (!bulletTr || !playerTr)
			return;

		bulletTr->localPosition = Math::GetWorldPosition(gunTr->worldMatrix);
		bulletTr->localRotation = glm::vec3(-90.0f, 0.0f, playerTr->localRotation.y + 90.0f);
		bulletTr->localScale = gunTr->localScale;

		SimpleBullet* bullet = (SimpleBullet*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<SimpleBullet>(newBulletId));
		PhalanxRanged* phalanx = (PhalanxRanged*)entityManager.GetComponentByIterator(m_Enemy->m_PhalanxIt);
		EnemyData* stats = (EnemyData*)em.GetComponentByIterator(m_Enemy->m_StatsIt);

		bullet->direction = CalculateForward(*gunTr);
		bullet->velocity = phalanx->bulletSpeed;
		bullet->lifeTime = phalanx->bulletLifeTime;
		bullet->damage = stats->damage;
		bullet->isFromPool = true;

		physSys->CreateBody();

		bulletSys->EnableBullet();


		//emit left muzzle
		EntityId shotMuzzleLeft = entityManager.GetChildByName(m_Enemy->GetEntity(), "p_shot_muzzle_ranged_phalanx");
		EntityId shotMuzzleLeftImpact = entityManager.GetChildByName(shotMuzzleLeft, "vfx_impact");
		EntityId shotMuzzleLeftFlash = entityManager.GetChildByName(shotMuzzleLeft, "vfx_flash");


		if (shotMuzzleLeft != EntityManager::INVALID_INDEX &&
			shotMuzzleLeftImpact != EntityManager::INVALID_INDEX &&
			shotMuzzleLeftFlash != EntityManager::INVALID_INDEX)
		{
			entityManager.SetActive(shotMuzzleLeft, true);

			ParticleSystem* sys_shotMuzzleLeftImpact = entityManager.GetSystem<ParticleSystem>(shotMuzzleLeftImpact);
			ParticleSystem* sys_shotMuzzleLeftFlash = entityManager.GetSystem<ParticleSystem>(shotMuzzleLeftFlash);

			if (sys_shotMuzzleLeftImpact != nullptr && sys_shotMuzzleLeftFlash != nullptr)
			{
				sys_shotMuzzleLeftImpact->EmitParticleBatch(1);
				sys_shotMuzzleLeftFlash->EmitParticleBatch(1);
			}
		}
		

	}

	glm::vec3 RangedPhalanxAttackState::CalculateForward(const Transform3D& t3d)
	{
		glm::vec3 rotrad = glm::radians(t3d.rotation);

		glm::vec3 forward;

		forward.x = glm::cos(rotrad.x) * glm::sin(rotrad.y);
		forward.y = -glm::sin(rotrad.x);
		forward.z = glm::cos(rotrad.x) * glm::cos(rotrad.y);

		forward = glm::degrees(forward);
		
		return glm::normalize(forward);
	}
}