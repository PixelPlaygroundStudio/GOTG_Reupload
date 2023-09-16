#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

#include <glm/glm.hpp>
namespace Wiwa {
	struct  UltronLaserBeam {		
		float lifeTime;
		int damagePerSecond;
		glm::vec3 direction;
	};
}

REFLECTION_BEGIN(Wiwa::UltronLaserBeam)
REFLECT_MEMBER(lifeTime)
REFLECT_MEMBER(damagePerSecond)
REFLECT_MEMBER(direction)
REFLECTION_END;