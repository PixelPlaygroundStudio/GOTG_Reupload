#include "PlayerDeath.h"
#include "Wiwa/ecs/systems/ParticleSystem.h"

Wiwa::PlayerDeath::PlayerDeath(PlayerStateMachine* stateMachine, EntityId id)
	: PlayerBaseState(stateMachine, id)
{
}

Wiwa::PlayerDeath::~PlayerDeath()
{
}

void Wiwa::PlayerDeath::EnterState()
{
	WI_INFO("Player death");
	m_StateMachine->GetAnimator()->PlayAnimation("death");
	m_StateMachine->SetPlayerRotation(m_StateMachine->GetDirection());
	m_StateMachine->GetPhysics()->getBody()->velocity = btVector3(0.f, 0.f, 0.f);

	EntityId player = GameStateManager::GetPlayerId();
	// TODO: Particles 

	EntityManager em = m_StateMachine->GetEntityManager();
	
	EntityId lapsus = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\player_abduction\\p_playerabduction_.wiprefab");
	
	if (lapsus != EntityManager::INVALID_INDEX)
	{
		Transform3D* tlapsus = em.GetComponent<Transform3D>(lapsus);
		Transform3D* tplayer = em.GetComponent<Transform3D>(player);
	
		if (tlapsus != nullptr)
		{
			tlapsus->localPosition = tplayer->localPosition;
			tlapsus->localPosition.x -= 1.5f;
		}
	}

}

void Wiwa::PlayerDeath::UpdateState()
{
	/*if (m_StateMachine->GetAnimator())
	{
		GameStateManager::Die();
		return;
	}*/
}

void Wiwa::PlayerDeath::ExitState()
{
}

void Wiwa::PlayerDeath::OnCollisionEnter(Object* object1, Object* object2)
{
}
