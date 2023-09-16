#pragma once
#include "MeleePhalanxBase.h"

namespace Wiwa {
	class MeleePhalanxAttackState : public MeleePhalanxBaseState {
	public:
		MeleePhalanxAttackState();
		~MeleePhalanxAttackState();

		void EnterState(EnemyMeleePhalanx* enemy) override;
		void UpdateState(EnemyMeleePhalanx* enemy) override;
		void ExitState(EnemyMeleePhalanx* enemy) override;
		void OnCollisionEnter(EnemyMeleePhalanx* enemy, const Object* body1, const Object* body2) override;
		
		void GenerateAttack();

		float m_SoundTimer = 0.5f;
		float m_SoundCurrentTime = 0.0f;
		bool m_PlaySound = false;
		EnemyMeleePhalanx* m_Enemy;
	};
}

