#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/math/Vector3f.h>
#include <Wiwa/utilities/Reflection.h>

#include <glm/glm.hpp>

namespace Wiwa {
	struct WI_API ColliderCube {
		glm::vec3 halfExtents;
	};
}

WI_REFLECTION_BEGIN(Wiwa::ColliderCube)
	WI_REFLECT_MEMBER(halfExtents)
WI_REFLECTION_END;