#pragma once
#include "PlayerBaseState.h"

namespace Wiwa
{
	class PlayerDash : public PlayerBaseState
	{
	public:
		PlayerDash(PlayerStateMachine* stateMachine, EntityId id);
		virtual ~PlayerDash();

		virtual void EnterState() override;
		virtual void UpdateState() override;
		virtual void ExitState() override;
		virtual void OnCollisionEnter(Object* object1, Object* object2) override;
		virtual void OnCollision(Object* object1, Object* object2) override;
	private:
		float m_DashTimer = 0.f;

		glm::vec3 m_TargetPoint;
		glm::vec3 m_DashDirection;
		
		float m_MaxDashTime = 1.5f;

		EntityId m_DashVFX;
	};
}