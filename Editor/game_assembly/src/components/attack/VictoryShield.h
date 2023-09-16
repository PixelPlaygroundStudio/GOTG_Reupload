#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

#include <glm/glm.hpp>
namespace Wiwa {
	struct  VictoryShield {
		float velocity;
		float lifeTime;
	};
}

REFLECTION_BEGIN(Wiwa::VictoryShield)
	REFLECT_MEMBER(velocity)
	REFLECT_MEMBER(lifeTime)
REFLECTION_END;