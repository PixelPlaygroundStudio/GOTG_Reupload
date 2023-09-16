#include <wipch.h>
#include "ShopRoom.h"
#include <Wiwa/scene/SceneManager.h>

void Wiwa::ShopRoom::OnInit()
{
	uint32_t randomNum = RAND(0, 2);
	WI_INFO(randomNum);
	Transform3D* t3d = SceneManager::getActiveScene()->GetEntityManager().GetComponent<Transform3D>(m_EntityId);
	GameStateManager::SpawnShopRandomItem(t3d->position, randomNum,m_EntityId);
	
}
