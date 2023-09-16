#pragma once

#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	struct WI_API OzzAnimatorCmp {
		// For animator system
		char animator_path[256];
	};
}

WI_REFLECTION_BEGIN(Wiwa::OzzAnimatorCmp)
	WI_REFLECT_MEMBER(animator_path)
WI_REFLECTION_END;