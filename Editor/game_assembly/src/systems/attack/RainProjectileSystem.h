#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>
#include "../enemy/Ultron/BossUltron.h"

#define RAIN_BULLET_WAITINGTIME 2.0f
#define ROTATION_RADIUS 12.0f
#define ROTATION_SPEED 0.67f

namespace Wiwa {
	class  RainProjectileSystem : public System {
	private:
		EntityManager::ComponentIterator m_BulletIt;
		EntityId m_CircualrThunderId;
		EntityId m_ThunderMarkId;

		BossUltron* BossUltronEnemy;

		const char* m_ThunderMarkPath;

		bool m_CircularThunder = false;
		bool m_SpawnMark = false;
		bool m_SpawnCircularThunder = false;
		bool m_DamageHasBeenApplied = false;

		int m_IndexAngle = 1;
		
		float m_StartingRotationPoint = 0.0f;
		float m_Timer = 0.0f;
		float m_TimerCircularThunder = 0.0f;
		float m_BulletSpeedStored;
		float m_RotationRadius = 0.0f;

		glm::vec3 m_CenterPoint = glm::vec3(0.0f);

	public:
		RainProjectileSystem();
		~RainProjectileSystem();

		void OnAwake() override;

		void OnInit() override;

		void InitRainProjectileBullet();

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		bool EnableBullet();
		bool EnableCircularThunder(BossUltron* enemy, glm::vec3 centerPoint, float startingPoint, float rotationRadius);
		bool OnDisabledFromPool();
	};
}

REGISTER_SYSTEM(Wiwa::RainProjectileSystem);