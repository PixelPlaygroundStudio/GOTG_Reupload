#pragma once
#include "SubjugatorBase.h"

namespace Wiwa {
	class SubjugatorHitState : public SubjugatorBaseState {
	public:
		SubjugatorHitState();
		~SubjugatorHitState();

		void EnterState(EnemySubjugator* enemy) override;
		void UpdateState(EnemySubjugator* enemy) override;
		void ExitState(EnemySubjugator* enemy) override;
		void OnCollisionEnter(EnemySubjugator* enemy, const Object* body1, const Object* body2) override;
	};
}
