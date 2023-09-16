#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

#include <glm/glm.hpp>
namespace Wiwa 
{
	struct SimpleBullet
	{
		float velocity;
		float lifeTime;
		int damage;
		glm::vec3 direction;
		bool isFromPool = false;
	};
}

REFLECTION_BEGIN(Wiwa::SimpleBullet)
REFLECT_MEMBER(velocity)
REFLECT_MEMBER(lifeTime)
REFLECT_MEMBER(damage)
REFLECT_MEMBER(direction)
REFLECT_MEMBER(isFromPool)
REFLECTION_END;