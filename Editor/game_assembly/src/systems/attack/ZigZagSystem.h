#pragma once

#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  ZigZagBulletSystem : public System {
	private:
		float m_Timer = 0;
		float m_TimerAmplitude = 0.0f;
		float m_ZigZagTimer = 0.0f;
		bool m_GetMaxAmplitude = false;
		EntityManager::ComponentIterator m_AttackIt;
		EntityManager::ComponentIterator m_BulletIt;
	public:
		ZigZagBulletSystem();
		~ZigZagBulletSystem();

		void OnAwake() override;

		void OnInit() override;

		void InitBullet();

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		bool EnableBullet();

		bool OnDisabledFromPool() override;

		glm::vec3 ZigZagBulletMotion(glm::vec3 current_position, float speed, float amplitude, float frequency, float delta_time, glm::vec3 forward_direction, float timer_amplitude);
	};
}

REGISTER_SYSTEM(Wiwa::ZigZagBulletSystem);