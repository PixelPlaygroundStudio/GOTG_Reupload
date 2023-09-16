#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/math/Vector3f.h>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	struct WI_API AudioSource {
		bool isDefaultListener;
		char eventName[32]; 
		bool playOnAwake;
		bool isPlaying;
	};
}

WI_REFLECTION_BEGIN(Wiwa::AudioSource)
	WI_REFLECT_MEMBER(isDefaultListener)
	WI_REFLECT_MEMBER(eventName)
	WI_REFLECT_MEMBER(playOnAwake)
	WI_REFLECT_MEMBER(isPlaying)
WI_REFLECTION_END;