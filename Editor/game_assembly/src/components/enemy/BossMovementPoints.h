#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

#include <glm/glm.hpp>

namespace Wiwa {
	struct BossMovementPoints {
		glm::vec3 point;
		bool isCenter;
	};
}

REFLECTION_BEGIN(Wiwa::BossMovementPoints)
REFLECT_MEMBER(point)
REFLECTION_END;