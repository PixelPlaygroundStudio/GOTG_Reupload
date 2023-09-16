#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/math/Vector3f.h>
#include <Wiwa/utilities/Reflection.h>

#include <glm/glm.hpp>

namespace Wiwa {
	struct WI_API RayCast {
		glm::vec3 rayFromWorld;
		glm::vec3 rayToWorld;
		bool doRayCasting;
	};
}

WI_REFLECTION_BEGIN(Wiwa::RayCast)
	WI_REFLECT_MEMBER(rayFromWorld)
	WI_REFLECT_MEMBER(rayToWorld)
WI_REFLECTION_END;
