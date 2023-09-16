#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>

#include <glm/glm.hpp>
#define MAP_TILES_MAX_SIZE 20000

namespace Wiwa {
	struct WI_API MapAI {
		glm::vec2 startingPosition;
		int width;
		int	height;
		float tileWidth; // Cannot be less than 1!
		float tileHeight;// Cannot be less than 1!
		unsigned char map[MAP_TILES_MAX_SIZE];
	};
}

WI_REFLECTION_BEGIN(Wiwa::MapAI)
	WI_REFLECT_MEMBER(startingPosition)
	WI_REFLECT_MEMBER(width)
	WI_REFLECT_MEMBER(height)
	WI_REFLECT_MEMBER(tileWidth)
	WI_REFLECT_MEMBER(tileHeight)
	WI_REFLECT_MEMBER(map)
WI_REFLECTION_END;