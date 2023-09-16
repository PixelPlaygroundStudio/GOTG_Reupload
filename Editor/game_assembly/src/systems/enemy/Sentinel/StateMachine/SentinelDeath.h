#pragma once
#include "SentinelBase.h"
#include <Wiwa/ecs/components/Transform3D.h>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class SentinelDeathState : public SentinelBaseState 
	{
	private: 
		EntityId m_ExplosionMarkId;
		const char* m_ExplosionMarkPath;
	public:
		SentinelDeathState();
		~SentinelDeathState();

		void EnterState(EnemySentinel* enemy) override;
		void UpdateState(EnemySentinel* enemy) override;
		void ExitState(EnemySentinel* enemy) override;
		void OnCollisionEnter(EnemySentinel* enemy, const Object* body1, const Object* body2) override;

		const float m_TimeToDie = 1.5f;
		float m_TimerToDie;
		/*void SpawnExplosion(EnemySentinel* enemy, Wiwa::Transform3D* transform);*/
	};
}