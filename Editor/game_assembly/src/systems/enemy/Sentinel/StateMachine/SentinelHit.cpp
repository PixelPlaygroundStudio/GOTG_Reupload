#include <wipch.h>
#include "SentinelHit.h"
#include "../EnemySentinel.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include <Wiwa/ecs/components/Mesh.h>
#include <Wiwa/ecs/components/HitComponent.h>
#include <Wiwa/utilities/render/Material.h>
namespace Wiwa
{
	SentinelHitState::SentinelHitState()
	{
	}

	SentinelHitState::~SentinelHitState()
	{
	}

	void SentinelHitState::EnterState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Wiwa::HitComponent* hc = em.GetComponent<HitComponent>(enemy->GetEntity());
		if (hc)
		{
			hc->Hit = true;
		}
		enemy->m_AudioSys->PlayAudio("sentinel_hit");
		enemy->m_AnimatorSys->PlayAnimation("hit"); //AnimacionSentinel
	}

	void SentinelHitState::UpdateState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		if (enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->HasFinished())
		{
			enemy->SwitchState(enemy->m_ChasingState);
		}
	}

	void SentinelHitState::ExitState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Wiwa::HitComponent* hc = em.GetComponent<HitComponent>(enemy->GetEntity());
		if (hc)
		{
			hc->Hit = false;
		}
	}

	void SentinelHitState::OnCollisionEnter(EnemySentinel* enemy, const Object* body1, const Object* body2)
	{
	}
}