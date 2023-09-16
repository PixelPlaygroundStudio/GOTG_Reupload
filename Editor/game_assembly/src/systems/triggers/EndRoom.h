#pragma once
#include "Wiwa/ecs//Systems.h"

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class EndRoomTrigger : public System {
	public:
		EntityManager::ComponentIterator triggerIt;

	public:
		EndRoomTrigger();
		~EndRoomTrigger();

		void OnDestroy() override;
		void OnUpdate() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;
		void OnCollisionExit(Object* body1, Object* body2) override;
		bool m_Activated = false;
		float m_Timer = 0.0f;
	};
}

REGISTER_SYSTEM(Wiwa::EndRoomTrigger);