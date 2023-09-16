#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

namespace Wiwa
{
	struct PlayerSpawnerData
	{
		Vector3f Position;
	};
}
REFLECTION_BEGIN(Wiwa::PlayerSpawnerData)
	REFLECT_MEMBER(Position)
REFLECTION_END;

