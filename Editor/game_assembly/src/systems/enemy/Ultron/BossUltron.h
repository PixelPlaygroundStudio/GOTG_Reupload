#pragma once
#include "../EnemySystem.h"

namespace Wiwa {

	class  BossUltron : public EnemySystem {
	public:
		class BossUltronBaseState* m_CurrentState;
		class BossUltronBaseState* m_SpawnState;
		class BossUltronBaseState* m_IdleState;
		class BossUltronBaseState* m_MovementState;
		class BossUltronBaseState* m_ClusterShotsAttackState;
		class BossUltronBaseState* m_LaserBeamAttackState;
		class BossUltronBaseState* m_BulletStormAttackState;
		class BossUltronBaseState* m_DeathState;
		class BossUltronBaseState* m_DashState;
		class BossUltronBaseState* m_SecondDashState;
		class BossUltronBaseState* m_ProjectileRain;
		class BossUltronBaseState* m_TransitionSecondPhaseState;

		EntityManager::ComponentIterator m_GunTransformIt;
		EntityManager::ComponentIterator m_Ultron;

		EntityId m_SecondPhaseId;

		float m_Timer;
		int m_MinimumPath = 0;
		int m_MaxHealth = 0;
		bool m_IsSecondPhaseActive = false;

		glm::vec3 m_SceneCenterPos = glm::vec3(0.0f);

		const char* m_SecondPhasePath;

	public:
		BossUltron();
		~BossUltron();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		void ReceiveDamage(int damage) override;

		void SwitchState(BossUltronBaseState* state);

		bool OnEnabledFromPool() override;

		bool OnDisabledFromPool() override;
	};
}

REGISTER_SYSTEM(Wiwa::BossUltron);