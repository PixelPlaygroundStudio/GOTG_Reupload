#include "RewardRoomDisplay.h"
#include "../../components/trigger/RewardRoomData.h"

namespace Wiwa
{
	void RewardRoomDisplay::OnInit()
	{
		m_DataIt = GetComponentIterator<RewardRoomData>();
		RewardRoomData* data = GetComponentByIterator<RewardRoomData>(m_DataIt);
		if (data)
		{
			if (data->isLeft)
			{
				data->num = GameStateManager::s_DoorsReward[0];
			}
			else
			{
				data->num = GameStateManager::s_DoorsReward[1];
			}

			EntityId particleId = EntityManager::INVALID_INDEX;
			EntityManager& em = GetEntityManager();
			switch (data->num)
			{
			case 0: // Active
			{
				particleId = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\doors\\p_door_active.wiprefab");
			}
			break;
			case 1: // Passive
			{
				particleId = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\doors\\p_door_passive.wiprefab");
			}
			break;
			case 2: // Buff
			{
				particleId = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\doors\\p_door_buff.wiprefab");
			}
			break;
			case 3: // NPC room
			{
				particleId = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\doors\\p_door_utau.wiprefab");
			}
			break;
			default:
				break;
			}

			if (GameStateManager::s_RoomsToShop == 1)
			{
				// particleId shop particle
				particleId = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\doors\\p_door_shop.wiprefab");
			}
			else if (GameStateManager::s_RoomsToBoss == 0)
			{
				// particleId boss particle
				particleId = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\doors\\p_door_boss.wiprefab");
			}

			if (particleId != EntityManager::INVALID_INDEX)
			{
				Transform3D* tr = em.GetComponent<Transform3D>(particleId);
				*tr = *GetTransform();
				tr->localPosition.y += 7;
				tr->localPosition.z += 2;
			}
		}
	}
}