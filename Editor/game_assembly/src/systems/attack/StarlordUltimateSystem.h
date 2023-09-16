#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  StarlordUltimateSystem : public System {
	public:
		StarlordUltimateSystem();
		~StarlordUltimateSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

	};
}

REGISTER_SYSTEM(Wiwa::StarlordUltimateSystem);