#pragma once
#include <Wiwa/ecs/systems/System.h>
#include "../enemy/Ultron/BossUltron.h"
#include <Wiwa/utilities/Reflection.h>

#define LITTLE_BULLETS_NUMBER 14

namespace Wiwa {
	class  ClusterBulletSystem : public System {
	private:
		float m_Timer = 0;
		EntityManager::ComponentIterator m_BulletIt;

		bool m_CollisionByWall = false;
		bool m_NotCollidingPlayer = true;

		bool m_HasBlown = true;

		BossUltron* BossUltronCluster;
	public:
		ClusterBulletSystem();
		~ClusterBulletSystem();

		void OnAwake() override;

		void OnInit() override;

		void InitClusterBullet();

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		void SpawnBullet(const glm::vec3& bull_dir, EntityId clusterBullet);
		void BlowClusterBullet01(EntityId bulletId);
		void BlowClusterBullet02(EntityId bulletId);

		//bool OnEnabledFromPool();
		bool EnableBullet(BossUltron* enemy);
		bool OnDisabledFromPool();
	};
}

REGISTER_SYSTEM(Wiwa::ClusterBulletSystem);