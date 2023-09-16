#pragma once
#include <Wiwa/physics/PhysicsManager.h>

namespace Wiwa {
	class BossUltron;
	class BossUltronBaseState {
	public:
		BossUltronBaseState() {};
		~BossUltronBaseState() {};

		virtual void EnterState(BossUltron* enemy) {};
		virtual void UpdateState(BossUltron* enemy) {};
		virtual void ExitState(BossUltron* enemy) {}
		virtual void OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2) {};
	};
}
