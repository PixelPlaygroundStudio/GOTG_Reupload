#pragma once

#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class ExplosiveBarrelSystem : public System {
	private:
		float m_Timer = 0;
		bool m_DoDamage = true;
		EntityManager::ComponentIterator m_ExplosiveBarrelIt;
	public:
		ExplosiveBarrelSystem();
		~ExplosiveBarrelSystem();

		void OnAwake() override;

		void OnInit() override;

		void InitExplosion();

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		bool EnableExplosion();

		bool OnDisabledFromPool() override;

		//void OnCollision(Object* body1, Object* body2) override;
	};
}

REGISTER_SYSTEM(Wiwa::ExplosiveBarrelSystem);