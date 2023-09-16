#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  GrootSeedsSystem : public System {
	private:
		float m_Timer = 0;
		EntityManager::ComponentIterator m_GrootSeedsIt;
		EntityManager::ComponentIterator m_GrootSeedsTransformIt;
		EntityManager::ComponentIterator m_PlayerTransformIt;
	public:
		GrootSeedsSystem();
		~GrootSeedsSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;
	};
}

REGISTER_SYSTEM(Wiwa::GrootSeedsSystem);