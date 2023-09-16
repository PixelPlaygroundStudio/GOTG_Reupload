#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/math/Vector3f.h>
#include <Wiwa/utilities/Reflection.h>

#include <glm/glm.hpp>



namespace Wiwa {
	struct WI_API CollisionBody {
		glm::vec3 positionOffset;
		glm::vec3 scalingOffset;
		bool isTrigger;
		bool isStatic;
		bool doContinuousCollision;
		int selfTag;
		int filterBits;
	};
}

WI_REFLECTION_BEGIN(Wiwa::CollisionBody)
	WI_REFLECT_MEMBER(positionOffset)
	WI_REFLECT_MEMBER(scalingOffset)
	WI_REFLECT_MEMBER(isTrigger)
	WI_REFLECT_MEMBER(isStatic)
	WI_REFLECT_MEMBER(doContinuousCollision)
	WI_REFLECT_MEMBER(selfTag)
	WI_REFLECT_MEMBER(filterBits)
WI_REFLECTION_END;