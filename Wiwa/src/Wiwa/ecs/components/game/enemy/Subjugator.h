#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

#include <glm/glm.hpp>
namespace Wiwa {
	struct WI_API Subjugator {
		int variant;
		float bulletSpeed;
		float bulletLifeTime;
	};
}

REFLECTION_BEGIN(Wiwa::Subjugator)
REFLECT_MEMBER(variant)
REFLECT_MEMBER(bulletSpeed)
REFLECT_MEMBER(bulletLifeTime)
REFLECTION_END;