#pragma once
#include "Wiwa/ecs//Systems.h"

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  OneTimeDialogSystem : public System {
	public:
		
	public:
		OneTimeDialogSystem();
		~OneTimeDialogSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;
		void OnCollisionExit(Object* body1, Object* body2) override;

		bool colliding = false;
	};
}

REGISTER_SYSTEM(Wiwa::OneTimeDialogSystem);