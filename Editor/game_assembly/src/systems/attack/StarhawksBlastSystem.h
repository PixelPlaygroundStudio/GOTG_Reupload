#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  StarhawksBlastSystem : public System {
	private:
		float m_Timer = 0;
		EntityManager::ComponentIterator m_StarHawksIt;
		EntityManager::ComponentIterator m_StarHawksTransfromIt;
		EntityManager::ComponentIterator m_PlayerTransformIt;
		std::vector<EntityManager::ComponentIterator> m_ColliderTransformIt;

	public:
		StarhawksBlastSystem();
		~StarhawksBlastSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		void SpawnStarhawkBullet(glm::vec3 direction);

	};
}

REGISTER_SYSTEM(Wiwa::StarhawksBlastSystem);