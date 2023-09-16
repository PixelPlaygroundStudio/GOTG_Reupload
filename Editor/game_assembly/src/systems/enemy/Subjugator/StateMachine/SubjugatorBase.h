#pragma once
#include <Wiwa/physics/PhysicsManager.h>

namespace Wiwa {
	class EnemySubjugator;
	class SubjugatorBaseState {
	public:
		SubjugatorBaseState() {};
		~SubjugatorBaseState() {};

		virtual void EnterState(EnemySubjugator* enemy) {};
		virtual void UpdateState(EnemySubjugator* enemy) {};
		virtual void ExitState(EnemySubjugator* enemy) {}
		virtual void OnCollisionEnter(EnemySubjugator* enemy, const Object* body1, const Object* body2) {};
	};
}