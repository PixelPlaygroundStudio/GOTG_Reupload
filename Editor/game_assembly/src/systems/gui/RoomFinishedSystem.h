#pragma once
#include "Wiwa/ecs//Systems.h"

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  RoomFinishedSystem : public System {
	private:
		bool m_FinishedRoom;
		bool m_FinishedAnim;
		float m_TimerAnim;
		float m_TimerIn;
	public:
		RoomFinishedSystem();
		~RoomFinishedSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;
	};
}

REGISTER_SYSTEM(Wiwa::RoomFinishedSystem);