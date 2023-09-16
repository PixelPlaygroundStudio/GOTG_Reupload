#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/math/Math.h>

#include <Wiwa/core/Resources.h>


#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	struct WI_API Sprite {
		Size2i sizeScreen;
		ResourceId spriteResource;
		Rect2i rect;
		
	};
}

WI_REFLECTION_BEGIN(Wiwa::Sprite);
	WI_REFLECT_MEMBER(sizeScreen);
	WI_REFLECT_MEMBER(spriteResource);
	WI_REFLECT_MEMBER(rect);
WI_REFLECTION_END;