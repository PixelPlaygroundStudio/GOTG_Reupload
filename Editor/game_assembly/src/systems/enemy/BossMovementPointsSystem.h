#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class BossMovementPointsSystems : public System {
	private:
		EntityManager::ComponentIterator m_PointIt;

	public:
		BossMovementPointsSystems();
		~BossMovementPointsSystems();

		void OnAwake() override;
	};
}

REGISTER_SYSTEM(Wiwa::BossMovementPointsSystems);