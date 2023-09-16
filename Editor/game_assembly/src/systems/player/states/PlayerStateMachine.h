#pragma once
#include "../PlayerController.h"


namespace Wiwa
{
	class PlayerBaseState;

	class PlayerStateMachine : public PlayerController
	{
	public:
		PlayerStateMachine();
		virtual ~PlayerStateMachine();

	public:
		virtual void OnAwake() override;
		virtual void OnInit() override;
		virtual void OnUpdate() override;
		virtual void OnCollisionEnter(Object* body1, Object* body2) override;

		WI_HARD_INL bool CanDash() { return (Input::IsKeyPressed(Key::LeftShift) || Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadLeftBumper)) && IsDashEnable(); }
		WI_HARD_INL bool CanMove() { return  GetInput() != glm::vec2(0.f); }
		WI_HARD_INL bool IsAiming() { return GetShootInput() != glm::vec2(0.f); }
		WI_HARD_INL bool CanAttack() { return Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadRigthBumper) || Input::IsKeyPressed(Key::Space); }
		WI_HARD_INL bool CanUltimate() 
		{
			return (Wiwa::Input::GetRawAxis(Gamepad::GamePad1, Gamepad::LeftTrigger, 0.2f) >= 0.f 
				&& Wiwa::Input::GetRawAxis(Gamepad::GamePad1, Gamepad::RightTrigger, 0.2f) >= 0.f)
				|| (Input::IsKeyRepeat(Key::Q) && Input::IsKeyRepeat(Key::E));
		}
		
		void UpdateMovement(const float speed);
		void UpdateRotation();

		void SwitchState(PlayerBaseState* state);
		void CheckHealth();
		void DashCooldown();
		void ResetCooldown();

		virtual void OnCollision(Object*, Object*) override;
		virtual void OnCollisionExit(Object* obj1, Object* obj2) override;
	public:
		class PlayerBaseState* m_IdleState;
		class PlayerBaseState* m_MoveState;
		class PlayerBaseState* m_AttackState;
		class PlayerBaseState* m_DashState;
		class PlayerBaseState* m_DeathState;

	private:
		PlayerBaseState* m_CurrentState;
		bool m_Colliding = false;
	};
}

REGISTER_SYSTEM(Wiwa::PlayerStateMachine)