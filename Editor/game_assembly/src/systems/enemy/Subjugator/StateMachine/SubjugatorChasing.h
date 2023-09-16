#pragma once
#include "SubjugatorBase.h"

namespace Wiwa {
	class SubjugatorChasingState : public SubjugatorBaseState {
	public:
		SubjugatorChasingState();
		~SubjugatorChasingState();

		void EnterState(EnemySubjugator* enemy) override;
		void UpdateState(EnemySubjugator* enemy) override;
		void ExitState(EnemySubjugator* enemy) override;
		void OnCollisionEnter(EnemySubjugator* enemy, const Object* body1, const Object* body2) override;

		float m_ChasingTimer = 0.0f;
		//bool m_HasTargetPoint = false;
		glm::vec3 m_TargetPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	};
}