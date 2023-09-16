#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	struct WI_API Wave {
		int maxEnemies;
		int currentEnemiesAlive;
		float enemySpawnRate;
		bool hasFinished;
		size_t entityId;
	};
}

REFLECTION_BEGIN(Wiwa::Wave)
REFLECT_MEMBER(maxEnemies)
REFLECT_MEMBER(currentEnemiesAlive)
REFLECT_MEMBER(enemySpawnRate)
REFLECT_MEMBER(hasFinished)
REFLECT_MEMBER(entityId)
REFLECTION_END;