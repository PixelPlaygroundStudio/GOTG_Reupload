#include <wipch.h>
#include "SentinelAttack.h"
#include "../EnemySentinel.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include "../../../attack/SentinelExplosionSystem.h"

#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>


namespace Wiwa
{
	SentinelAttackState::SentinelAttackState()
	{
		m_TimerExplosion = 0.0f;
		prepareAttack = false;
	}

	SentinelAttackState::~SentinelAttackState()
	{

	}

	void SentinelAttackState::EnterState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);
		/*Character* stats = (Character*)em.GetComponentByIterator(enemy->m_StatsIt);*/
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);

		navAgent->agentSliding = false;

		enemy->m_AnimatorSys->PlayAnimation("atackdeath");
	}

	void SentinelAttackState::UpdateState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();

		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);
		//Character* stats = (Character*)em.GetComponentByIterator(enemy->m_StatsIt);
		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);

		if (glm::distance(selfTr->localPosition, playerTr->localPosition) < 2.0f)
		{
			navAgent->stoppingDistance = 1.0f;
			enemy->m_NavAgentSys->StopAgent();
		}

		float dt = Time::GetDeltaTimeSeconds();

		m_TimerExplosion += dt;

		if (m_TimerExplosion >= 0.7f)
		{
			enemy->SwitchState(enemy->m_DeathState);
		}
	}

	void SentinelAttackState::ExitState(EnemySentinel* enemy)
	{

	}

	void SentinelAttackState::OnCollisionEnter(EnemySentinel* enemy, const Object* body1, const Object* body2)
	{
	
	}
}