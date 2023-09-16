#pragma once
#include "Wiwa/ecs//Systems.h"

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class InitHubTrigger : public System 
	{
	public:
		InitHubTrigger();
		~InitHubTrigger();

		void OnAwake() override;
	};
}

REGISTER_SYSTEM(Wiwa::InitHubTrigger);