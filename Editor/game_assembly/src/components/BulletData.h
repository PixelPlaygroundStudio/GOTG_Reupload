#pragma once
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa
{
	struct BulletData
	{
		float Velocity;
		float LifeTime;
	};
}

REFLECTION_BEGIN(Wiwa::BulletData)
	REFLECT_MEMBER(Velocity)
	REFLECT_MEMBER(LifeTime)
REFLECTION_END;