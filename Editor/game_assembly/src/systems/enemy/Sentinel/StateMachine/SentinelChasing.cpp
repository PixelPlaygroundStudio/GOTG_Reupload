#include <wipch.h>
#include "SentinelChasing.h"
#include "../EnemySentinel.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

namespace Wiwa
{
	SentinelChasingState::SentinelChasingState()
	{

	}

	SentinelChasingState::~SentinelChasingState()
	{

	}

	void SentinelChasingState::EnterState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();

		NavAgent* navAgent = (NavAgent*)em.GetComponentByIterator(enemy->m_NavAgentIt);

		navAgent->agentSliding = true;

		enemy->m_AnimatorSys->PlayAnimation("walk"); //AnimacionSentinel

		enemy->m_TimerSentinel = 0;
	}

	void SentinelChasingState::UpdateState(EnemySentinel* enemy)
	{
		m_ChasingTimer += Time::GetDeltaTime();

		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);

		float distance = glm::distance(playerTr->localPosition, selfTr->localPosition);

		if (enemy->m_NavAgentSys != nullptr)
		{
			Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
			enemy->m_NavAgentSys->SetDestination(playerTr->localPosition);
		}

		//if (enemy->m_AnimatorSys->getAnimator()->getActiveAnimation()->HasFinished())//&& !IsPlaying(enemy->GetEntity())
		//{
		//	//SentinelAudio
		//	/*PlaySound(ScriptEngine::CreateString("melee_moving"), enemy->m_PlayerId);*/
		//}


		if (glm::distance(selfTr->localPosition, playerTr->localPosition) <= enemy->m_RangeOfExplosion)
		{
			enemy->SwitchState(enemy->m_AttackingState);
		}
	}

	void SentinelChasingState::ExitState(EnemySentinel* enemy)
	{
		Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
		//enemy->m_AnimatorSys->Blend("idle", true, true, 0.5f);
		//enemy->m_AnimatorSys->Pause();
	}

	void SentinelChasingState::OnCollisionEnter(EnemySentinel* enemy, const Object* body1, const Object* body2)
	{
		/*std::string playerStr = "PLAYER";
		if (body1->id == enemy->GetEntity() && playerStr == body2->selfTagStr)
		{
			enemy->SwitchState(enemy->m_DeathState);
		}*/
	}
}