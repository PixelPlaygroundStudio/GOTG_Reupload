#pragma once
#include "PlayerBaseState.h"

namespace Wiwa
{
	class PlayerDeath : public PlayerBaseState
	{
	public:
		PlayerDeath(PlayerStateMachine* stateMachine, EntityId id);
		virtual ~PlayerDeath();

		virtual void EnterState() override;
		virtual void UpdateState() override;
		virtual void ExitState() override;
		virtual void OnCollisionEnter(Object* object1, Object* object2) override;
	};
}