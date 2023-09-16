#include "StarlordUltimateSystem.h"
#include <Wiwa/ecs/systems/PhysicsSystem.h>

namespace Wiwa
{
	StarlordUltimateSystem::StarlordUltimateSystem()
	{
	}

	StarlordUltimateSystem::~StarlordUltimateSystem()
	{
	}

	void StarlordUltimateSystem::OnAwake()
	{
	}

	void StarlordUltimateSystem::OnInit()
	{
		SetTransform();
	}

	void StarlordUltimateSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		EntityManager& em = GetEntityManager();
		EntityId explosionId = em.LoadPrefab("assets\\Prefabs\\StarlordUltimate\\P_StarLordUltimateExplosion.wiprefab");
		Transform3D* selfTr = GetTransform();
		Transform3D* explosionTr = (Transform3D*)em.GetComponentByIterator(GetComponentIterator<Transform3D>(explosionId));

		*explosionTr = *selfTr;
		glm::vec3 newPos = Math::GetWorldPosition(selfTr->worldMatrix);
		newPos.y = 0.1f;
		explosionTr->localPosition = newPos;
		explosionTr->localRotation = glm::vec3(-90.0f, 0.0f, 90.0f);
		explosionTr->localScale = selfTr->localScale;

		/*EntityId explosionVfxId = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_ExplosionStarlordUlti.wiprefab");
		Transform3D* explosionVfxTr = (Transform3D*)em.GetComponentByIterator(GetComponentIterator<Transform3D>(explosionVfxId));

		*explosionVfxTr = *selfTr;
		explosionVfxTr->localPosition = Math::GetWorldPosition(selfTr->worldMatrix);
		explosionVfxTr->localRotation = glm::vec3(-90.0f, 0.0f, 90.0f);
		explosionVfxTr->localScale = selfTr->localScale;*/

		PhysicsSystem* physSys = em.GetSystem<PhysicsSystem>(explosionId);
		physSys->ForceSetPosition(Math::GetWorldPosition(explosionTr->worldMatrix));
	}
}
