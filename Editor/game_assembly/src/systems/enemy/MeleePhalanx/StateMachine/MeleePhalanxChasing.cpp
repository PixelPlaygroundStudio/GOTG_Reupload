#include <wipch.h>
#include "MeleePhalanxChasing.h"
#include "../EnemyMeleePhalanx.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include <Wiwa/audio/Audio.h>

namespace Wiwa
{
	MeleePhalanxChasingState::MeleePhalanxChasingState()
	{

	}

	MeleePhalanxChasingState::~MeleePhalanxChasingState()
	{

	}

	void MeleePhalanxChasingState::EnterState(EnemyMeleePhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		enemy->m_AnimatorSys->PlayAnimation("walk");

		enemy->m_Timer = 0;
	}
	
	void MeleePhalanxChasingState::UpdateState(EnemyMeleePhalanx* enemy)
	{
		m_ChasingTimer += Time::GetDeltaTime();

		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);
		float distance = glm::distance(selfTr->localPosition, playerTr->localPosition);

		if (enemy->m_NavAgentSys != nullptr)
		{
			Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);

			glm::vec3 offset = { 1.0f,0.0f,1.0f };
			offset.x = playerTr->localPosition.x + 1.1f;
			offset.z = playerTr->localPosition.z + 1.1f;

			enemy->m_NavAgentSys->SetDestination(offset);
		}
		
		if (enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->HasFinished())//&& !IsPlaying(enemy->GetEntity())
		{
			
		}

		
		if (distance <= 2.5f && distance > 1.0f)
		{
			enemy->SwitchState(enemy->m_AttackingState);
		}
	}
	
	void MeleePhalanxChasingState::ExitState(EnemyMeleePhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		//enemy->m_AnimatorSys->Blend("idle", true, true, 0.5f);
		//enemy->m_AnimatorSys->Pause();
	}
	
	void MeleePhalanxChasingState::OnCollisionEnter(EnemyMeleePhalanx* enemy, const Object* body1, const Object* body2)
	{
	}
}