#pragma once
#include "BossUltronBase.h"

#define NUMBER_OF_RANDOM_ACTIONS 4
#define ALTITUDE_THUNDERSTORM_MOVEMENT 30.0f
#define ANGLE_OFFSET 30.0f

namespace Wiwa {
	enum class UltronAttacks
	{
		NONE = 0,
		BULLET_STORM,
		LASER_BEAM,
		CLUSTER_SHOTS,
		DASH,
		SECOND_DASH,
		RAIN_PROJECTILE
	};

	class BossUltronMovementState : public BossUltronBaseState {
	public:
		BossUltronMovementState();
		~BossUltronMovementState();

		void EnterState(BossUltron* enemy) override;
		void UpdateState(BossUltron* enemy) override;
		void ExitState(BossUltron* enemy) override;
		void OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2) override;

		UltronAttacks GetAttackFromProbabilitesFirstPhase();
		UltronAttacks GetAttackFromProbabilitesSecondPhase();

		void FillPremadePosition(BossUltron* enemy, std::vector<glm::vec3>& vec);
		glm::vec3 GetNewPosition();

		void MovementAttack(BossUltron* enemy);

		void SpawnBulletMovement(BossUltron* enemy, Wiwa::Transform3D* transform, const glm::vec3& bull_dir);
		void SpawnZigZagBulletMovement(BossUltron* enemy, Wiwa::Transform3D* transform, const glm::vec3& bull_dir);
		void SpawnClusterBulletMovement(BossUltron* enemy, const glm::vec3& bull_dir);
		void SpawnSplashZigZagBullets(BossUltron* enemy);
		void SpawnThunderStormMovement(BossUltron* enemy, glm::vec3 thunderPosition, const glm::vec3& bull_dir);
		void SpawnThunderStormCircularMovement(BossUltron* enemy, glm::vec3 thunderPosition, const glm::vec3& bull_dir, float angle, float rotationRadius);

		bool isInsideSquare(const glm::vec3& point);
		glm::vec3 GetPositionAroundPlayer(const glm::vec3& localPosition, float distance, int index);
		glm::vec3 GetPositionUpDownLeftRight(const glm::vec3& localPosition, float distance, int index);

	private:
		UltronAttacks m_NextAttack;
		std::vector<glm::vec3> m_PremadePositions;
		glm::vec3 currentDestination = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_InitialPlayerPos = glm::vec3(0.0f);
		glm::vec3 m_InitialPlayerPosCenterThunders = glm::vec3(0.0f);

		const char* m_ThunderMarkPath;
		const char* m_MiddleThunderMarkPath;

		EntityId m_MiddleThunderMarkId;
		EntityId m_CircularThunderMarkId1;

		std::vector<EntityId> m_ThunderMarkIds;
		std::vector<glm::vec3> m_ThunderPositions;
		std::vector<EntityId> m_CircularThunderMarkIds;
		std::vector<glm::vec3> m_CircularThunderPositions;

		glm::vec3 m_CircularThunderPosition1 = glm::vec3(0.0f);

		float m_TimerToAttack = 0.0f;
		float m_TimerAttackOnMoving = 0.0f;

		int m_SelectMovingRandomAttack = -1;

		bool m_DoAttack = false;
		bool m_MovementAttackFinished = false;
		bool m_IsMovingAttackSelected = false;
		bool m_MovementAttackSelected = false;
		bool m_ClusterMovementSpawned = false;
		bool m_ThunderMovementMarkSpawned = false;
		bool m_CircularThunderMovementMarkSpawned = false;
		bool m_ThunderMovementSpawned = false;
		bool m_ThunderMiddleSpawned = false;
		bool m_CircularThunderMovementSpawned = false;
		bool m_CircularMiddleThunder = false;
		bool m_SplashZigZagMovementSpawned = false;
		bool m_SplashZigZagAnimation = false;
		bool m_DoubleClusterAnimation = false;
	};
}
