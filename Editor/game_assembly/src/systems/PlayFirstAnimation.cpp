#include "PlayFirstAnimation.h"
#include <Wiwa/animation/OzzAnimator.h>
#include <Wiwa/ecs/components/OzzAnimatorCmp.h>
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>

void PlayFirstAnimation::OnInit()
{
	Wiwa::OzzAnimatorCmp* ACmp = GetComponent<Wiwa::OzzAnimatorCmp>();

	if (!ACmp) return;
	Wiwa::EntityManager& em = m_Scene->GetEntityManager();
	Wiwa::OzzAnimationSystem* ASys = em.GetSystem<Wiwa::OzzAnimationSystem>(m_EntityId);

	if (!ASys) return;
	Wiwa::OzzAnimator* animator = ASys->getAnimator();

	if (!animator) return;
	size_t count = animator->getAnimationCount();
	
	if (count == 0) return;

	animator->PlayAnimation(0);
}
