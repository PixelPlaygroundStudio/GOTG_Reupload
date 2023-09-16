#pragma once
#include <Wiwa/utilities/Reflection.h>
#include <glm/glm.hpp>

namespace Wiwa
{
	struct CameraAgent
	{
		bool Active;
		glm::vec3 Position;
	};
}

REFLECTION_BEGIN(Wiwa::CameraAgent)
	REFLECT_MEMBER(Active)
	REFLECT_MEMBER(Position)
REFLECTION_END;
