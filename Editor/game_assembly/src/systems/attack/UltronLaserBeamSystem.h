#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

//#include "Wiwa/ecs/components/game/wave/Wave.h"

//#define ULTRON_LASER_LIFETIME 6.0f
//#define ULTRON_LASER_DAMAGE 30 // per second

namespace Wiwa {
	class  UltronLaserBeamSystem : public System {
	private:

		float m_Timer = 0.0f;
		float m_TimerDamagePerSec = 0.0f;
		EntityManager::ComponentIterator m_LaserIt;
		EntityManager::ComponentIterator m_PlayerTransformIt;
		EntityManager::ComponentIterator m_BossTransformIt;


	public:
		UltronLaserBeamSystem();
		~UltronLaserBeamSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		/*void OnCollisionEnter(Object* body1, Object* body2) override;*/
		void OnCollision(Object* body1, Object* body2) override;

		void InitLaserBeam();
		//bool OnEnabledFromPool();
		bool EnableLaser();
		bool OnDisabledFromPool();

	};
}

REGISTER_SYSTEM(Wiwa::UltronLaserBeamSystem);