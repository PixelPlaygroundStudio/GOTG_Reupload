#pragma once
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa
{
	struct RewardRoomData
	{
		int num;
		bool isLeft;
	};
}

REFLECTION_BEGIN(Wiwa::RewardRoomData)
REFLECT_MEMBER(num)
REFLECT_MEMBER(isLeft)
REFLECTION_END;