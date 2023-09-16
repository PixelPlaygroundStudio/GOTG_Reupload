#pragma once
#include "Wiwa/ecs//Systems.h"

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class InitBossRoom : public System
	{
	public:
		InitBossRoom();
		~InitBossRoom();

		void OnAwake() override;
	};
}

REGISTER_SYSTEM(Wiwa::InitBossRoom);