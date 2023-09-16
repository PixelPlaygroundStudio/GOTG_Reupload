#pragma once
#include "SentinelBase.h"

namespace Wiwa {
	class SentinelHitState : public SentinelBaseState {
	public:
		SentinelHitState();
		~SentinelHitState();

		void EnterState(EnemySentinel* enemy) override;
		void UpdateState(EnemySentinel* enemy) override;
		void ExitState(EnemySentinel* enemy) override;
		void OnCollisionEnter(EnemySentinel* enemy, const Object* body1, const Object* body2) override;
	};
}