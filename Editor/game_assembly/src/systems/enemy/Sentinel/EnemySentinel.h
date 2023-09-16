#pragma once
#include "../EnemySystem.h"

namespace Wiwa 
{
	class  EnemySentinel : public EnemySystem {
	public:
		class SentinelBaseState* m_CurrentState;
		class SentinelBaseState* m_IdleState;
		class SentinelBaseState* m_SpawnState;
		class SentinelBaseState* m_ChasingState;
		class SentinelBaseState* m_AttackingState;
		class SentinelBaseState* m_DeathState;
		class SentinelBaseState* m_HitState;

		float m_TimerSentinel; 
		float m_RangeOfExplosion = 3.0f;
	public:
		EnemySentinel();
		~EnemySentinel();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		void ReceiveDamage(int damage) override;

		void SwitchState(SentinelBaseState* state);

		bool OnEnabledFromPool() override;

		bool OnDisabledFromPool() override;

	};
}

REGISTER_SYSTEM(Wiwa::EnemySentinel);