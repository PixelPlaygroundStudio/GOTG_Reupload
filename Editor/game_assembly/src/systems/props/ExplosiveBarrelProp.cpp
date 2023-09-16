#include "ExplosiveBarrelProp.h"
#include "Wiwa/ecs/systems/PhysicsSystem.h"
#include <glm/gtx/quaternion.hpp>
#include "../attack/ExplosiveBarrelSystem.h"
#include <Wiwa/game/GameStateManager.h>
#include "../../components/attack/Explosion.h"

namespace Wiwa
{

	Wiwa::ExplosiveBarrelProp::ExplosiveBarrelProp()
	{
		m_SpawnExplosion = false;
		m_ExplosionMarkPath = "assets\\Enemy\\Explosions\\ExplosionRemnants_01.wiprefab";
	}

	ExplosiveBarrelProp::~ExplosiveBarrelProp()
	{
	}

	void ExplosiveBarrelProp::OnAwake()
	{
	}

	void ExplosiveBarrelProp::OnInit()
	{
		m_SpawnExplosion = false;
	}

	void ExplosiveBarrelProp::OnUpdate()
	{
		Wiwa::EntityManager& em = this->getScene().GetEntityManager();
		Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(this->m_TransformIt);

		if (m_SpawnExplosion)
		{
			SpawnExplosiveBarrelExplosion(this, selfTr);

			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();

			m_ExplosionMarkId = em.LoadPrefab(m_ExplosionMarkPath);
			Transform3D* explosionMarkTr = em.GetComponent<Transform3D>(m_ExplosionMarkId);
			explosionMarkTr->localPosition.x = selfTr->localPosition.x;
			explosionMarkTr->localPosition.y = selfTr->localPosition.y + 0.1f;
			explosionMarkTr->localPosition.z = selfTr->localPosition.z;

			//Destroy the barrel
			em.DestroyEntity(m_EntityId);
		}
	}

	void ExplosiveBarrelProp::OnDestroy()
	{

	}

	void ExplosiveBarrelProp::OnCollisionEnter(Object* body1, Object* body2)
	{
		std::string playerBulletStr = "PLAYER_ATTACK";
		if (body1->id == m_EntityId && playerBulletStr == body2->selfTagStr)
		{
			m_SpawnExplosion = true;
		}
	}

	void ExplosiveBarrelProp::SpawnExplosiveBarrelExplosion(ExplosiveBarrelProp* enemy, Transform3D* selfTransform)
	{
		/*if (GameStateManager::s_PoolManager->s_ExplosiveBarrel->getCountDisabled() <= 0)
			return;*/

		Wiwa::EntityManager& entityManager = enemy->getScene().GetEntityManager();
		GameStateManager::s_PoolManager->SetScene(&enemy->getScene());
		EntityId newExplosionId = EntityManager::INVALID_INDEX;
		newExplosionId = GameStateManager::s_PoolManager->s_ExplosiveBarrel->GetFromPool();

		if (newExplosionId == EntityManager::INVALID_INDEX)
			return;

		ExplosiveBarrelSystem* explosionBarrelSys = entityManager.GetSystem<ExplosiveBarrelSystem>(newExplosionId);
		PhysicsSystem* physSys = entityManager.GetSystem<PhysicsSystem>(newExplosionId);
		physSys->DeleteBody();

		// Set intial positions
		//Transform3D* playerTr = (Transform3D*)entityManager.GetComponentByIterator(enemy->m_TransformIt); //Transform
		Transform3D* explosiveBarrelTr = (Transform3D*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Transform3D>(newExplosionId));
		Explosion* explosion = (Explosion*)entityManager.GetComponentByIterator(entityManager.GetComponentIterator<Explosion>(newExplosionId));
		explosion->isFromPool = true;

		if (!explosiveBarrelTr)
			return;

		explosiveBarrelTr->localPosition = Math::GetWorldPosition(selfTransform->worldMatrix);
		explosiveBarrelTr->localScale = GetTransform()->localScale;

		physSys->CreateBody();

		explosionBarrelSys->EnableExplosion();
	}
}