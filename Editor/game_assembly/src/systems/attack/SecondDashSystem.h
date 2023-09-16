#pragma once
#include <Wiwa/ecs/systems/System.h>
#include "../enemy/Ultron/BossUltron.h"
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  SecondDashSystem : public System {
	private:
		float m_Timer = 0;
		bool m_HasDashed = false;
		bool m_CollisionWithWall = false;

		EntityManager::ComponentIterator m_AttackIt;
		EntityManager::ComponentIterator m_DashIt;

		BossUltron* BossUltronEnemy;

	public:
		SecondDashSystem();
		~SecondDashSystem();

		void OnAwake() override;

		void OnInit() override;

		void InitBullet();

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		bool EnableBullet(BossUltron* enemy);

		bool OnDisabledFromPool() override;

		glm::vec3 GetPositionAfterDash(glm::vec3 position);
	};
}

REGISTER_SYSTEM(Wiwa::SecondDashSystem);