#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

#include <glm/glm.hpp>
namespace Wiwa {
	struct WI_API EnemyData {
		std::string name = "NONE";
		int level = 0;
		int health = 0;
		int damage = 0;
		float maxVelocity = 0.0f;
		float maxAcceleration = 0.0f;
		float stoppingDistance = 0.0f;
		float rateOfFire = 0.0f;
		float range = 0.0f;
		int creditsDrop = 0;
	};
}

REFLECTION_BEGIN(Wiwa::EnemyData)
REFLECT_MEMBER(name)
REFLECT_MEMBER(level)
REFLECT_MEMBER(health)
REFLECT_MEMBER(damage)
REFLECT_MEMBER(maxVelocity)
REFLECT_MEMBER(maxAcceleration)
REFLECT_MEMBER(rateOfFire)
REFLECT_MEMBER(range)
REFLECT_MEMBER(creditsDrop)
REFLECTION_END;