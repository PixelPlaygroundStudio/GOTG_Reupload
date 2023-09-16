#pragma once
#include <Wiwa/physics/PhysicsManager.h>

namespace Wiwa {
	class EnemyRangedPhalanx;
	class RangedPhalanxBaseState {
	public:
		RangedPhalanxBaseState() {};
		~RangedPhalanxBaseState() {};

		virtual void EnterState(EnemyRangedPhalanx* enemy) {};
		virtual void UpdateState(EnemyRangedPhalanx* enemy) {};
		virtual void ExitState(EnemyRangedPhalanx* enemy) {}
		virtual void OnCollisionEnter(EnemyRangedPhalanx* enemy, const Object* body1, const Object* body2) {};
	};
}
