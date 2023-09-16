#pragma once
#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
namespace Wiwa {
	struct WI_API HitComponent {
		bool Hit;
	};
}

REFLECTION_BEGIN(Wiwa::HitComponent)
REFLECT_MEMBER(Hit)
REFLECTION_END;