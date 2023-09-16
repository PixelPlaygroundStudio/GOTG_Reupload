#pragma once
#include "MeleePhalanxBase.h"

namespace Wiwa {
	class MeleePhalanxChasingState : public MeleePhalanxBaseState {
	public:
		MeleePhalanxChasingState();
		~MeleePhalanxChasingState();

		void EnterState(EnemyMeleePhalanx* enemy) override;
		void UpdateState(EnemyMeleePhalanx* enemy) override;
		void ExitState(EnemyMeleePhalanx* enemy) override;
		void OnCollisionEnter(EnemyMeleePhalanx* enemy, const Object* body1, const Object* body2) override;

		float m_ChasingTimer = 0.0f;
	};
}

