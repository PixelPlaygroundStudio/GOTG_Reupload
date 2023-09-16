#pragma once
#include "System.h"
#include "../components/Transform3D.h"
#include "../components/AnimatorComponent.h"
#include "../components/Mesh.h"
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class WI_API AnimatorSystem : public System {
	private:
		EntityManager::ComponentIterator m_AnimatorComponent; 
	public:
		AnimatorSystem();
		~AnimatorSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnEventFinish();

		void OnSystemAdded() override;

		void OnSystemRemoved()override;

		void Pause();

		void Play();

		void PlayAnimation(std::string name, bool loop);

		void Blend(std::string targetAnim, bool loop, float transitionTime);

		void LoopAnimation(bool loop);

		bool HasFinished();

		void SetAnimationSate(Wiwa::AnimationState state);

		void Restart();

		const std::string GetCurrentAnimName();
	};
}

WI_REGISTER_SYSTEM(Wiwa::AnimatorSystem);