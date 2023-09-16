#pragma once
#include "BossUltronBase.h"

#define TIMER_GO_UPWARDS_LASER 0.01f
#define TIMER_GO_DOWNWARDS_LASER 0.01f

namespace Wiwa {
	class BossUltronLaserBeamAttackState : public BossUltronBaseState {
		enum class LaserState {
			SMASH_INIT,
			SMASH_GO_UP,
			SMASH_GO_DOWN,
			SMASH_EXPLOSION,
			MOVE_CENTER,
			PREPARE_LASER,
			LASER_ATTACK,
			END_STATE,
		};
	public:
		BossUltronLaserBeamAttackState();
		~BossUltronLaserBeamAttackState();

		void EnterState(BossUltron* enemy) override;
		void UpdateState(BossUltron* enemy) override;
		void ExitState(BossUltron* enemy) override;
		void OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2) override;
		void FillPremadePositionAfterLaser(BossUltron* enemy, std::vector<glm::vec3>& vec);
		glm::vec3 GetNewPositionAfterLaser();
		
	private:

		EntityId SpawnLaserBeam(BossUltron* enemy, const glm::vec3& bull_dir);

		glm::vec3 CalculateForward(const Transform3D& t3d);

		void SpawnExplosionAfterSmash(BossUltron* enemy, Transform3D* selfTransform);

		float m_TimerToRotate = 0.0f;
		float m_TimerDash = 0.0f;
		float m_TimerToStopDash = 0.0f;
		float m_TimerAfterDash = 0.0f;

		float lifetimeDash = 2.0f;

		bool initiateDash = false;
		bool m_UltronJump = false;
		

		float m_MoveUpwardsCounter = 0.0f;
		float m_MoveDownwardsCounter = 0.0f;

		LaserState laserState;
		std::vector<glm::vec3> m_AfterLaserBeamPosition;
		float m_TimerLaser;
		glm::vec3 centerPoint = glm::vec3(0.0f, 0.0f, 0.0f);

		const char* m_PreSmashMarkPath;
		EntityId m_PreSmashMarkId;

		const char* m_LaserProtectionPath;
		EntityId m_LaserProtectionId;
		bool m_ActivateLaserProtection = false;
	};
}
