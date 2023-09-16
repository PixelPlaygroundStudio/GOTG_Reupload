#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

#include <glm/glm.hpp>
namespace Wiwa {
	struct Explosion {
		float lifeTime;
		int damage;
		bool isFromPool = false;
	};
}

REFLECTION_BEGIN(Wiwa::Explosion)
REFLECT_MEMBER(lifeTime)
REFLECT_MEMBER(damage)
REFLECT_MEMBER(isFromPool)
REFLECTION_END;