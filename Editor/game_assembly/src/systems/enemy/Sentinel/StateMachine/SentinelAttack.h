#pragma once
#include "SentinelBase.h"
#include <Wiwa/ecs/components/Transform3D.h>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class SentinelAttackState : public SentinelBaseState {
	public:
		SentinelAttackState();
		~SentinelAttackState();

		void EnterState(EnemySentinel* enemy) override;
		void UpdateState(EnemySentinel* enemy) override;
		void ExitState(EnemySentinel* enemy) override;
		void OnCollisionEnter(EnemySentinel* enemy, const Object* body1, const Object* body2) override;

		float m_TimerExplosion = 0.0f;
		bool prepareAttack;
	};
}