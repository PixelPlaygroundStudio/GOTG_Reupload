#pragma once
#include "BossUltronBase.h"

namespace Wiwa {
	class BossUltronIdleState : public BossUltronBaseState {
	public:
		BossUltronIdleState();
		~BossUltronIdleState();

		void EnterState(BossUltron* enemy) override;
		void UpdateState(BossUltron* enemy) override;
		void ExitState(BossUltron* enemy) override;
		void OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2) override;
	};
}
