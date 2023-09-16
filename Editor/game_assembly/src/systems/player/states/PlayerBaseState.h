#pragma once
#include "PlayerStateMachine.h"
#include <Wiwa/ecs/systems/PhysicsSystem.h>
#include <LinearMath/btVector3.h>
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
namespace Wiwa
{
	typedef size_t EntityId;

	class PlayerBaseState
	{
	public:
		PlayerBaseState(PlayerStateMachine* stateMachine, EntityId id);
		virtual ~PlayerBaseState();

		virtual void EnterState() = 0;
		virtual void UpdateState() = 0;
		virtual void ExitState() = 0;
		virtual void OnCollisionEnter(Object*, Object*) = 0;
		virtual void OnCollision(Object*, Object*);
		virtual void OnCollisionExit(Object*, Object*);
	protected:
		EntityId m_Id;
		PlayerStateMachine* m_StateMachine;
	};
}

