#pragma once
#include "RangedPhalanxBase.h"

namespace Wiwa {
	class RangedPhalanxDeathState : public RangedPhalanxBaseState {
	public:
		RangedPhalanxDeathState();
		~RangedPhalanxDeathState();

		void EnterState(EnemyRangedPhalanx* enemy) override;
		void UpdateState(EnemyRangedPhalanx* enemy) override;
		void ExitState(EnemyRangedPhalanx* enemy) override;
		void OnCollisionEnter(EnemyRangedPhalanx* enemy, const Object* body1, const Object* body2) override;

		const float m_TimeToDie = 1.5f;
		float m_TimerToDie;
	};
}
