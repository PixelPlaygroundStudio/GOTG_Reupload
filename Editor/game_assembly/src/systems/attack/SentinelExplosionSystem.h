#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  SentinelExplosionSystem : public System {
	private:
		float m_Timer = 0;
		bool m_DoSentinelDamage = true;
		EntityManager::ComponentIterator m_ExplosionIt;
	public:
		SentinelExplosionSystem();
		~SentinelExplosionSystem();

		void OnAwake() override;

		void OnInit() override;

		void InitExplosion();

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		void EnableExplosion();

		//bool OnEnabledFromPool();

		bool OnDisabledFromPool();
	};
}

REGISTER_SYSTEM(Wiwa::SentinelExplosionSystem);