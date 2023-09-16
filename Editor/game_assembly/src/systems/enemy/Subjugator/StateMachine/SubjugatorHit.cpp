#include <wipch.h>
#include "SubjugatorHit.h"
#include "../EnemySubjugator.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/ecs/components/Mesh.h>
#include <Wiwa/ecs/components/HitComponent.h>
#include <Wiwa/utilities/render/Material.h>
namespace Wiwa
{
	SubjugatorHitState::SubjugatorHitState()
	{
	}

	SubjugatorHitState::~SubjugatorHitState()
	{
	}

	void SubjugatorHitState::EnterState(EnemySubjugator* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Wiwa::HitComponent* hc = em.GetComponent<HitComponent>(enemy->GetEntity());
		if (hc)
		{
			hc->Hit = true;
		}
		enemy->m_AnimatorSys->PlayAnimation("hit");
	}

	void SubjugatorHitState::UpdateState(EnemySubjugator* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		if (enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->HasFinished())
		{
			enemy->SwitchState(enemy->m_ChasingState);
		}
	}

	void SubjugatorHitState::ExitState(EnemySubjugator* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Wiwa::HitComponent* hc = em.GetComponent<HitComponent>(enemy->GetEntity());
		if (hc)
		{
			hc->Hit = false;
		}
	}

	void SubjugatorHitState::OnCollisionEnter(EnemySubjugator* enemy, const Object* body1, const Object* body2)
	{

	}
}