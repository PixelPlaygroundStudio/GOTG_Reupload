#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  CaptainsUniverseEnergySystem : public System {
	private:
		float m_Timer = 0;
		EntityManager::ComponentIterator m_CaptainsUniverseEnergyIt;
		EntityManager::ComponentIterator m_CaptainsUniverseEnergyTransformIt;
		EntityManager::ComponentIterator m_PlayerTransformIt;
	public:
		CaptainsUniverseEnergySystem();
		~CaptainsUniverseEnergySystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		void SetExplosion();
	};
}

REGISTER_SYSTEM(Wiwa::CaptainsUniverseEnergySystem);