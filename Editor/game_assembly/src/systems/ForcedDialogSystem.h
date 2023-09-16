#pragma once
#include "Wiwa/ecs//Systems.h"

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  ForcedDialogSystem : public System {
	public:
		
	public:
		ForcedDialogSystem();
		~ForcedDialogSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;
		void OnCollisionExit(Object* body1, Object* body2) override;

	};
}

REGISTER_SYSTEM(Wiwa::ForcedDialogSystem);