#include "wipch.h"
#include "AnimatorSystem.h"

namespace Wiwa {
	AnimatorSystem::AnimatorSystem()
	{
		//m_AnimatorComponent = { WI_INVALID_INDEX, WI_INVALID_INDEX };
	}
	AnimatorSystem::~AnimatorSystem()
	{
	}
	void AnimatorSystem::OnAwake()
	{
	}
	void AnimatorSystem::OnInit()
	{

	}
	void AnimatorSystem::OnUpdate()
	{
		//m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		//if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return;

		//Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);

		//if (anim == nullptr && anim->animator != nullptr) return;
		//
		////update animator
		//anim->animator->Update(Time::GetRealDeltaTime());

	}
	void AnimatorSystem::OnDestroy()
	{
		/*Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);
		delete anim->animator;*/
	}
	void AnimatorSystem::OnEventFinish()
	{

	}
	void AnimatorSystem::OnSystemAdded()
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return;

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);
		
		anim->animator = Animator::LoadWiAnimator(anim->filePath);*/
	}
	void AnimatorSystem::OnSystemRemoved()
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return;

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);
		
		if (!anim) return;
		
		if (anim->animator != nullptr)
			delete anim->animator;*/
	}

	void AnimatorSystem::Pause()
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return;

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);
		
		if (!anim) return;
		
		anim->animator->PauseAnimation();*/
	}
	void AnimatorSystem::Play()
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return;

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);
		
		if (!anim) return;
		
		anim->animator->PlayAnimation();*/
	}
	void AnimatorSystem::PlayAnimation(std::string name, bool loop)
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return;

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);
		
		if (!anim) return;
		
		anim->animator->PlayAnimationName(name,loop);*/
	}
	void AnimatorSystem::Blend(std::string name, bool loop, float transitionTime)
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return;

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);

		if (!anim) return;

		anim->animator->PlayAnimation(name,loop,true,transitionTime);*/
	}

	void AnimatorSystem::LoopAnimation(bool loop)
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return;

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);

		if (!anim) return;

		anim->animator->Loop(loop);*/
	}
	bool AnimatorSystem::HasFinished()
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return false;

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);

		if (!anim) return false;


		return anim->animator->CurrentAnimationHasFinished();*/
		return false;
	}
	void AnimatorSystem::SetAnimationSate(Wiwa::AnimationState state)
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return;

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);
		
		if (!anim) return;
		
		anim->animator->SetAnimationState(state);*/
	}
	void AnimatorSystem::Restart()
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return;

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);
		
		if (!anim) return;
		
		anim->animator->ResetAnimation();*/
	}

	const std::string AnimatorSystem::GetCurrentAnimName()
	{
		/*m_AnimatorComponent = GetComponentIterator<AnimatorComponent>();

		if (m_AnimatorComponent.c_id == WI_INVALID_INDEX) return "noname";

		Wiwa::AnimatorComponent* anim = GetComponentByIterator<Wiwa::AnimatorComponent>(m_AnimatorComponent);

		if (!anim) "noname";

		return anim->animator->GetCurrentAnimation()->m_Name;*/

		return "tuputamadre";
	}
}