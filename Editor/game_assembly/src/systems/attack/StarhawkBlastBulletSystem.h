#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

//#include "Wiwa/ecs/components/game/wave/Wave.h"

namespace Wiwa {
	class  StarhawkBlastBulletSystem : public System {
	private:
		float m_Timer = 0;
		EntityManager::ComponentIterator m_AttackIt;
		EntityManager::ComponentIterator m_BulletIt;
	public:
		StarhawkBlastBulletSystem();
		~StarhawkBlastBulletSystem();

		void OnAwake() override;

		void OnInit() override;

		void InitBullet();

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		//bool OnEnabledFromPool() override;

		bool EnableBullet();

		bool OnDisabledFromPool() override;

	};
}

REGISTER_SYSTEM(Wiwa::StarhawkBlastBulletSystem);