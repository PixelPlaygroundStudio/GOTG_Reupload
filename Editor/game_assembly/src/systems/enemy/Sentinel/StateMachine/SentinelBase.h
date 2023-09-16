#pragma once
#include <Wiwa/physics/PhysicsManager.h>

namespace Wiwa {
	class EnemySentinel;
	class SentinelBaseState {
	public:
		SentinelBaseState() {};
		~SentinelBaseState() {};

		virtual void EnterState(EnemySentinel* enemy) {};
		virtual void UpdateState(EnemySentinel* enemy) {};
		virtual void ExitState(EnemySentinel* enemy) {}
		virtual void OnCollisionEnter(EnemySentinel* enemy, const Object* body1, const Object* body2) {};
	};
}