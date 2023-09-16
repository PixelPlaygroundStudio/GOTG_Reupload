#pragma once
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa
{
	struct Trigger
	{
		char triggerType[64];
	};
}

REFLECTION_BEGIN(Wiwa::Trigger)
REFLECT_MEMBER(triggerType)
REFLECTION_END;