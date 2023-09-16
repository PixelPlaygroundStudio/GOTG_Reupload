#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <glm/glm.hpp>

namespace Wiwa
{
	struct StarLordShooter
	{
		bool ShootRight;
	};
}
REFLECTION_BEGIN(Wiwa::StarLordShooter)
	REFLECT_MEMBER(ShootRight)
REFLECTION_END;