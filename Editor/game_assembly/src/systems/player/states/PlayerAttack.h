#pragma once
#include "PlayerBaseState.h"

namespace Wiwa
{
	class PlayerAttack : public PlayerBaseState
	{
	public:
		PlayerAttack(PlayerStateMachine* stateMachine, EntityId id);
		virtual ~PlayerAttack();

		virtual void EnterState() override;
		virtual void UpdateState() override;
		virtual void ExitState() override;
		virtual void OnCollisionEnter(Object* object1, Object* object2) override;

		void Fire();
		void FireStarlordUltimate();
		void FireRocketUltimate();
	private:
		float m_ShootTimer;
		glm::vec2 m_MousePos;
		bool m_FirstShoot;
		float m_audioDelay;
		float m_currentAudioDelay;

		float footstepTimer = 0.f;
		float walkStepTimer = 0.62f;
		float runStepTimer = 0.458f;
		float stepParticleTimer = 0.0f;
		float currentSteptime = 0.0f;
	};
}