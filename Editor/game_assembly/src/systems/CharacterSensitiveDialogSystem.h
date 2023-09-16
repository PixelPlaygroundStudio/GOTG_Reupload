#pragma once
#include "Wiwa/ecs//Systems.h"

#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/ecs/systems/AnimatorSystem.h>

namespace Wiwa {
	class  CharacterSensitiveDialogSystem : public System {
	public:
		
	public:
		CharacterSensitiveDialogSystem();
		~CharacterSensitiveDialogSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;
		void OnCollisionExit(Object* body1, Object* body2) override;

	};
}

REGISTER_SYSTEM(Wiwa::CharacterSensitiveDialogSystem);