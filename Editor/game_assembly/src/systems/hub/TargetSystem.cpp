#include "TargetSystem.h"

TargetSystem::TargetSystem() : 
	m_HitTimer(0.0f),
	m_HitTime(1.0f),
	m_RotationSpeed(1.0f),
	m_Hit(false)
{
	m_ChildTransformIt = { WI_INVALID_INDEX, WI_INVALID_INDEX, WI_INVALID_INDEX };
}

TargetSystem::~TargetSystem()
{

}

void TargetSystem::OnInit()
{
	Wiwa::EntityManager& em = m_Scene->GetEntityManager();

	EntityId child = em.GetChildByName(m_EntityId, "target_body");

	m_ChildTransformIt = GetComponentIterator<Wiwa::Transform3D>(child);
}

void TargetSystem::OnUpdate()
{
	if (!m_ChildTransformIt) return;

	Wiwa::Transform3D* childt3d = GetComponentByIterator<Wiwa::Transform3D>(m_ChildTransformIt);

	if (m_Hit) {
		m_HitTimer += Wiwa::Time::GetDeltaTimeSeconds();

		childt3d->localRotation.z += m_RotationSpeed * Wiwa::Time::GetDeltaTime();

		if (m_HitTimer >= m_HitTime) {
			m_Hit = false;
			m_HitTimer = 0.0f;
		}
	}
}

void TargetSystem::OnCollisionEnter(Wiwa::Object* body1, Wiwa::Object* body2)
{
	std::string playerBulletStr = "PLAYER_ATTACK";
	if (body1->id == m_EntityId && body2->selfTagStr == playerBulletStr) {
		m_Hit = true;
		m_HitTimer = 0.0f;
	}
}