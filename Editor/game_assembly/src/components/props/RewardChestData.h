#pragma once
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa
{
	struct RewardChestData
	{
		bool Active;
	};
}

REFLECTION_BEGIN(Wiwa::RewardChestData)
	REFLECT_MEMBER(Active)
REFLECTION_END;