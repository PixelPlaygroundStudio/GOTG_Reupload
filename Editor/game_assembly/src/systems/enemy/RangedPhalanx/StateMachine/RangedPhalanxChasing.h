#pragma once
#include "RangedPhalanxBase.h"

namespace Wiwa {
	class RangedPhalanxChasingState : public RangedPhalanxBaseState {
	public:
		RangedPhalanxChasingState();
		~RangedPhalanxChasingState();

		void EnterState(EnemyRangedPhalanx* enemy) override;
		void UpdateState(EnemyRangedPhalanx* enemy) override;
		void ExitState(EnemyRangedPhalanx* enemy) override;
		void OnCollisionEnter(EnemyRangedPhalanx* enemy, const Object* body1, const Object* body2) override;

		float m_ChasingTimer = 0.0f;
		//bool m_HasTargetPoint = false;
		glm::vec3 m_TargetPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	};
}

