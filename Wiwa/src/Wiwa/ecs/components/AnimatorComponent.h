#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/math/Vector3f.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/render/Animator.h>

namespace Wiwa {
	struct WI_API AnimatorComponent
	{
		char filePath[128];
		bool Play;
		bool Loop;
		bool Blend;
		bool LoopBlend;
		float weight;
		float baseAnimTime = 0;
		float layerAnimTime = 0;
		float blendDuration = 0;
		float blendTime = 0;

		Animator* animator;
	};
}

WI_REFLECTION_BEGIN(Wiwa::AnimatorComponent)
	WI_REFLECT_MEMBER(filePath)
	WI_REFLECT_MEMBER(Play)
	WI_REFLECT_MEMBER(Loop)
	WI_REFLECT_MEMBER(Blend)
	WI_REFLECT_MEMBER(weight)
	WI_REFLECT_MEMBER(baseAnimTime)
	WI_REFLECT_MEMBER(layerAnimTime)
	WI_REFLECT_MEMBER(blendDuration)
	WI_REFLECT_MEMBER(blendTime)
	WI_REFLECT_MEMBER(animator)
WI_REFLECTION_END;