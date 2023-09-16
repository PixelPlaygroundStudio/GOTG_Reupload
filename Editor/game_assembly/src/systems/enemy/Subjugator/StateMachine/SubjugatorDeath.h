#pragma once
#include "SubjugatorBase.h"

namespace Wiwa {
	class SubjugatorDeathState : public SubjugatorBaseState {
	public:
		SubjugatorDeathState();
		~SubjugatorDeathState();

		void EnterState(EnemySubjugator* enemy) override;
		void UpdateState(EnemySubjugator* enemy) override;
		void ExitState(EnemySubjugator* enemy) override;
		void OnCollisionEnter(EnemySubjugator* enemy, const Object* body1, const Object* body2) override;

		const float m_TimeToDie = 1.5f;
		float m_TimerToDie;
	};
}