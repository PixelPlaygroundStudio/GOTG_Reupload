#pragma once
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa
{
	struct Attack
	{
		char attackType[64];
		bool isPlayerAttack;
		bool isEnemyAttack;
	};
}

REFLECTION_BEGIN(Wiwa::Attack)
REFLECT_MEMBER(attackType)
REFLECT_MEMBER(isPlayerAttack)
REFLECT_MEMBER(isEnemyAttack)
REFLECTION_END;