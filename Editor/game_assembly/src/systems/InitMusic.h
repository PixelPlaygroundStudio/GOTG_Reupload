#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class InitMusic : public System
	{
	public:

	public:
		InitMusic();
		~InitMusic();

		void OnAwake() override;

		void OnInit() override;

		void OnDestroy() override;
	};
}
REGISTER_SYSTEM(Wiwa::InitMusic);