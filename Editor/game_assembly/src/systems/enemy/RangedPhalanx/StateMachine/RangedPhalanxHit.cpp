#include <wipch.h>
#include "RangedPhalanxHit.h"
#include "../EnemyRangedPhalanx.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/ecs/components/Mesh.h>
#include <Wiwa/ecs/components/HitComponent.h>
#include <Wiwa/utilities/render/Material.h>

namespace Wiwa
{
	RangedPhalanxHitState::RangedPhalanxHitState()
	{

	}

	RangedPhalanxHitState::~RangedPhalanxHitState()
	{

	}

	void RangedPhalanxHitState::EnterState(EnemyRangedPhalanx* enemy)
	{

		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Wiwa::HitComponent* hc = em.GetComponent<HitComponent>(enemy->GetEntity());
		if (hc)
		{
			hc->Hit = true;
		}
		enemy->m_AnimatorSys->PlayAnimation("damage");

		enemy->m_AudioSys->PlayAudio("ranged_dead");
	}

	void RangedPhalanxHitState::UpdateState(EnemyRangedPhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		if (enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->HasFinished())
		{
			enemy->SwitchState(enemy->m_ChasingState);
		}
	}

	void RangedPhalanxHitState::ExitState(EnemyRangedPhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Wiwa::HitComponent* hc = em.GetComponent<HitComponent>(enemy->GetEntity());
		if (hc)
		{
			hc->Hit = false;
		}
	}

	void RangedPhalanxHitState::OnCollisionEnter(EnemyRangedPhalanx* enemy, const Object* body1, const Object* body2)
	{
	}
}