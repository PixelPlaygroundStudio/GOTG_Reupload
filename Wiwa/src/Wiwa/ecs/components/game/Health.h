#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

#include <glm/glm.hpp>
namespace Wiwa {
	struct WI_API Health {
		int health;
	};
}

REFLECTION_BEGIN(Wiwa::Health)
REFLECT_MEMBER(health)
REFLECTION_END;