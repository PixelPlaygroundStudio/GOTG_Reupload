#pragma once
#include "SubjugatorBase.h"

namespace Wiwa {
	class SubjugatorIdleState : public SubjugatorBaseState {
	public:
		SubjugatorIdleState();
		~SubjugatorIdleState();

		void EnterState(EnemySubjugator* enemy) override;
		void UpdateState(EnemySubjugator* enemy) override;
		void ExitState(EnemySubjugator* enemy) override;
		void OnCollisionEnter(EnemySubjugator* enemy, const Object* body1, const Object* body2) override;
	};
}