#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/math/Math.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	struct WI_API Transform2D {
		Vector2i position;
		float rotation;
		Vector2f scale;
	};
}

WI_REFLECTION_BEGIN(Wiwa::Transform2D)
	WI_REFLECT_MEMBER(position)
	WI_REFLECT_MEMBER(rotation)
	WI_REFLECT_MEMBER(scale)
WI_REFLECTION_END;