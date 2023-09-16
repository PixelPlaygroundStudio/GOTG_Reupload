#pragma once

#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/animation/animations/OzzAnimationPartialBlending.h>
#include <Wiwa/animation/OzzAnimator.h>

namespace Wiwa {
	class WI_API OzzAnimationSystem : public System {
	private:
		OzzAnimator* m_Animator;
		EntityManager::ComponentIterator m_TransformIt;
		EntityManager::ComponentIterator m_AnimatorIt;
		EntityManager::ComponentIterator m_HitIt;
	public:
		OzzAnimationSystem();
		~OzzAnimationSystem();

		void PlayAnimation(const char* name);

		OzzAnimator* getAnimator() { return m_Animator; }

		void OnAwake() override;
		void OnInit() override;
		void OnUpdate() override;
	
		void OnDestroy() override;


		void OnSystemAdded() override;
	};
}

WI_REGISTER_SYSTEM(Wiwa::OzzAnimationSystem);