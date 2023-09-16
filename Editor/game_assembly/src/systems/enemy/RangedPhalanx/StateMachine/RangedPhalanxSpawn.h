#pragma once
#include "RangedPhalanxBase.h"

namespace Wiwa {
	class RangedPhalanxSpawnState : public RangedPhalanxBaseState {
	public:
		RangedPhalanxSpawnState();
		~RangedPhalanxSpawnState();

		void EnterState(EnemyRangedPhalanx* enemy) override;
		void UpdateState(EnemyRangedPhalanx* enemy) override;
		void ExitState(EnemyRangedPhalanx* enemy) override;
		void OnCollisionEnter(EnemyRangedPhalanx* enemy, const Object* body1, const Object* body2) override;
	};
}
