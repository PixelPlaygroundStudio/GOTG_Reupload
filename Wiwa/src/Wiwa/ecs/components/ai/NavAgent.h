#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	struct WI_API NavAgent
	{
		float radius;						///< Agent radius. [Limit: >= 0]
		float height;						///< Agent height. [Limit: > 0]
		float maxAcceleration;				///< Maximum allowed acceleration. [Limit: >= 0]
		float maxSpeed;						///< Maximum allowed speed. [Limit: >= 0]
		float separationWeight; /// How aggresive the agent manager should be at avoiding collisions with this agent. [Limit: >= 0]
		float stoppingDistance;
		bool autoRotate;
		bool agentSliding;
	};
}

REFLECTION_BEGIN(Wiwa::NavAgent)
REFLECT_MEMBER(radius)
REFLECT_MEMBER(height)
REFLECT_MEMBER(maxAcceleration)
REFLECT_MEMBER(maxSpeed)
REFLECT_MEMBER(separationWeight)
REFLECT_MEMBER(stoppingDistance)
REFLECT_MEMBER(autoRotate)
REFLECTION_END;