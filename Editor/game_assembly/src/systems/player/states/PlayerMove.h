#pragma once
#include "PlayerBaseState.h"

namespace Wiwa
{
	class PlayerMove : public PlayerBaseState
	{
	public:
		PlayerMove(PlayerStateMachine* stateMachine, EntityId id);
		virtual ~PlayerMove();

		virtual void EnterState() override;
		virtual void UpdateState() override;
		virtual void ExitState() override;
		virtual void OnCollisionEnter(Object* object1, Object* object2) override;
	private:
		float footstepTimer = 0.f;
		float walkStepTimer = 0.62f;
		float runStepTimer = 0.458f;
		float stepParticleTimer = 0.0f;
		float currentSteptime  =0.0f;
	};
}