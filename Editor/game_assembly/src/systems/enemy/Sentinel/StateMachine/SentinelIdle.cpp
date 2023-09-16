#include <wipch.h>
#include "SentinelIdle.h"
#include "../EnemySentinel.h"
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

namespace Wiwa
{
	SentinelIdleState::SentinelIdleState()
	{

	}

	SentinelIdleState::~SentinelIdleState()
	{

	}

	void SentinelIdleState::EnterState(EnemySentinel* enemy)
	{
		enemy->m_TimerSentinel = 0;
	}

	void SentinelIdleState::UpdateState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();

		

		/*enemy->m_AnimatorSys->PlayAnimation("idle", true);*/ //AnimacionSentinel

		if (enemy->m_TimerSentinel > 2)
		{
			enemy->SwitchState(enemy->m_ChasingState);
		}
	}

	void SentinelIdleState::ExitState(EnemySentinel* enemy)
	{

	}

	void SentinelIdleState::OnCollisionEnter(EnemySentinel* enemy, const Object* body1, const Object* body2)
	{

	}
}