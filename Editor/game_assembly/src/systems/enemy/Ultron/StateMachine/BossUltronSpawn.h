#pragma once
#include "BossUltronBase.h"

namespace Wiwa {
	class BossUltronSpawnState : public BossUltronBaseState {
	public:
		BossUltronSpawnState();
		~BossUltronSpawnState();

		void EnterState(BossUltron* enemy) override;
		void UpdateState(BossUltron* enemy) override;
		void ExitState(BossUltron* enemy) override;
		void OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2) override;

	private: 
		float m_TimerSpawn = 0.0f;
	};
}
