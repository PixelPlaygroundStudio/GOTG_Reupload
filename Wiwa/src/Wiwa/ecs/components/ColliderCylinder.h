#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/math/Vector3f.h>
#include <Wiwa/utilities/Reflection.h>

#include <glm/glm.hpp>

namespace Wiwa {
	struct WI_API ColliderCylinder {
		float height;
		float radius;
	};
}

WI_REFLECTION_BEGIN(Wiwa::ColliderCylinder)
	WI_REFLECT_MEMBER(height)
	WI_REFLECT_MEMBER(radius)
WI_REFLECTION_END;