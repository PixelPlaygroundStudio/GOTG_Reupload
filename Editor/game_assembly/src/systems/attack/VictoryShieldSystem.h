#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  VictoryShieldSystem : public System {
	private:
		float m_Timer = 0;
		EntityManager::ComponentIterator m_ShieldIt;
		EntityManager::ComponentIterator m_ShieldTransfromIt;
		EntityManager::ComponentIterator m_PlayerTransformIt;
		EntityManager::ComponentIterator m_ColliderTransformIt;

	public:
		VictoryShieldSystem();
		~VictoryShieldSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

	};
}

REGISTER_SYSTEM(Wiwa::VictoryShieldSystem);