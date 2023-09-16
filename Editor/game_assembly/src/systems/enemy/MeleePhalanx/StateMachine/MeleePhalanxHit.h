#pragma once
#include "MeleePhalanxBase.h"

namespace Wiwa {
	class MeleePhalanxHitState : public MeleePhalanxBaseState {
	public:
		MeleePhalanxHitState();
		~MeleePhalanxHitState();

		void EnterState(EnemyMeleePhalanx* enemy) override;
		void UpdateState(EnemyMeleePhalanx* enemy) override;
		void ExitState(EnemyMeleePhalanx* enemy) override;
		void OnCollisionEnter(EnemyMeleePhalanx* enemy, const Object* body1, const Object* body2) override;
	};
}
