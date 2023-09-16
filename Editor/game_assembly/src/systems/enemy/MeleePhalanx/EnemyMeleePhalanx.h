#pragma once
#include "../EnemySystem.h"

namespace Wiwa {
	class  EnemyMeleePhalanx : public EnemySystem {
	public:
		class MeleePhalanxBaseState* m_CurrentState;
		class MeleePhalanxBaseState* m_IdleState;
		class MeleePhalanxBaseState* m_SpawnState;
		class MeleePhalanxBaseState* m_ChasingState;
		class MeleePhalanxBaseState* m_AttackingState;
		class MeleePhalanxBaseState* m_DeathState;
		class MeleePhalanxBaseState* m_HitState;

		float m_Timer;
	public:
		EnemyMeleePhalanx();
		~EnemyMeleePhalanx();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		void ReceiveDamage(int damage) override;

		void SwitchState(MeleePhalanxBaseState* state);

		bool OnEnabledFromPool() override;

		bool OnDisabledFromPool() override;

	};
}

REGISTER_SYSTEM(Wiwa::EnemyMeleePhalanx);