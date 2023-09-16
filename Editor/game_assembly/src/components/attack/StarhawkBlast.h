#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

#include <glm/glm.hpp>
namespace Wiwa {
	struct  StarhawksBlast {
		float velocity;
		float lifeTime;
		int damage;
	};
}

REFLECTION_BEGIN(Wiwa::StarhawksBlast)
REFLECT_MEMBER(velocity)
REFLECT_MEMBER(lifeTime)
REFLECT_MEMBER(damage)
REFLECTION_END;