#include <wipch.h>
#include "MeleePhalanxIdle.h"
#include "../EnemyMeleePhalanx.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

namespace Wiwa
{
	MeleePhalanxIdleState::MeleePhalanxIdleState()
	{

	}

	MeleePhalanxIdleState::~MeleePhalanxIdleState()
	{

	}

	void MeleePhalanxIdleState::EnterState(EnemyMeleePhalanx* enemy)
	{
		enemy->m_Timer = 0;
	}

	void MeleePhalanxIdleState::UpdateState(EnemyMeleePhalanx* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		enemy->m_AnimatorSys->PlayAnimation("idle");

		if (enemy->m_Timer > 2)
		{
			enemy->SwitchState(enemy->m_ChasingState);
		}
	}


	void MeleePhalanxIdleState::ExitState(EnemyMeleePhalanx* enemy)
	{
	}

	void MeleePhalanxIdleState::OnCollisionEnter(EnemyMeleePhalanx* enemy, const Object* body1, const Object* body2)
	{
	}
}

