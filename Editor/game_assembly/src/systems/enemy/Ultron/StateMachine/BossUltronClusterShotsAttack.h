#pragma once
#include "BossUltronBase.h"

#define TIME_BETWEEN_CLUSTER_BULLET 1.0f

namespace Wiwa {
	class BossUltronClusterShotsAttackState : public BossUltronBaseState {
		enum class ClusterState {
			FIRST_ATTACK,
			SECOND_ATTACK,
			THIRD_ATTACK,
			FOURTH_ATTACK,
			FIFTH_ATTACK,
			SIXTH_ATTACK,
			END_STATE,
		};
	public:
		BossUltronClusterShotsAttackState();
		~BossUltronClusterShotsAttackState();

		void EnterState(BossUltron* enemy) override;
		void UpdateState(BossUltron* enemy) override;
		void ExitState(BossUltron* enemy) override;
		void OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2) override;		

	private:

		ClusterState clusterState;

		bool SpawnClusterBullet(BossUltron* enemy, const glm::vec3& bull_dir);

		glm::vec3 CalculateForward(const Transform3D& t3d);

		bool LookingAtPlayer();

		float m_TimerBetweenBullet = 0.0f;
		float m_TimerToLookAtPlayer = 0.0f;
		bool m_AlreadyShot = false;
	};
}
