#include <wipch.h>
#include "BossUltronIdle.h"
#include "../BossUltron.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

namespace Wiwa
{
	BossUltronIdleState::BossUltronIdleState()
	{

	}

	BossUltronIdleState::~BossUltronIdleState()
	{

	}

	void BossUltronIdleState::EnterState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		EntityId currentEnemy = enemy->GetEntity();

		//pman.EmitBatch(currentEnemy);

	}

	void BossUltronIdleState::UpdateState(BossUltron* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		//if (enemy->m_AnimatorSys->HasFinished())
		//enemy->SwitchState(enemy->m_ChasingState);
	}

	void BossUltronIdleState::ExitState(BossUltron* enemy)
	{
	}

	void BossUltronIdleState::OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2)
	{
	}
}