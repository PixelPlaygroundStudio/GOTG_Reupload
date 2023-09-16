#pragma once
#include "PlayerBaseState.h"

namespace Wiwa
{
	class PlayerIdle : public PlayerBaseState
	{
	public:
		PlayerIdle(PlayerStateMachine* stateMachine, EntityId id);
		virtual ~PlayerIdle();

		virtual void EnterState() override;
		virtual void UpdateState() override;
		virtual void ExitState() override;
		virtual void OnCollisionEnter(Object* object1, Object* object2) override;
	};
}