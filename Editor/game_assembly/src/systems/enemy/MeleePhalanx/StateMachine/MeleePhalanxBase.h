#pragma once
#include <Wiwa/physics/PhysicsManager.h>

namespace Wiwa {
	class EnemyMeleePhalanx;
	class MeleePhalanxBaseState {
	public:
		MeleePhalanxBaseState() {};
		~MeleePhalanxBaseState() {};

		virtual void EnterState(EnemyMeleePhalanx* enemy) {};
		virtual void UpdateState(EnemyMeleePhalanx* enemy) {};
		virtual void ExitState(EnemyMeleePhalanx* enemy) {}
		virtual void OnCollisionEnter(EnemyMeleePhalanx* enemy, const Object* body1, const Object* body2) {};
	};
}
