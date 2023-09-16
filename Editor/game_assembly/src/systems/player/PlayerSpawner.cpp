#include "PlayerSpawner.h"
#include <Wiwa/game/GameStateManager.h>
#include <Wiwa/ecs/components/game/player/PlayerSpawnerData.h>

namespace Wiwa
{
	Wiwa::PlayerSpawner::PlayerSpawner()
	{
		
	}

	PlayerSpawner::~PlayerSpawner()
	{
	}

	void PlayerSpawner::OnSystemAdded()
	{
		m_PlayerSet = false;

		PlayerSpawnerData* data = GetComponent<PlayerSpawnerData>();
		if (data)
		{
			Transform3D* t3d = GetTransform();
			if (t3d)
			{
				data->Position = Math::GetWorldPosition(t3d->worldMatrix);
				m_LastAvailablePos = Math::GetWorldPosition(t3d->worldMatrix);
			}
		}
	}

	void PlayerSpawner::OnAwake()
	{
		m_DataIt = GetComponentIterator<PlayerSpawnerData>();
		if (GameStateManager::s_CurrentCharacter == STARLORD) // Starlord
		{
			m_PlayerId = m_Scene->GetEntityManager().LoadPrefab("assets\\player\\starlord.wiprefab");
			WI_INFO("Loaded StarLord player");
		}
		else
		{
			m_PlayerId = m_Scene->GetEntityManager().LoadPrefab("assets\\player\\rocket.wiprefab");
			WI_INFO("Loaded Rocket player");
		}
		GameStateManager::SetPlayerId(m_PlayerId, m_Scene);
		Transform3D* playerT3D = m_Scene->GetEntityManager().GetComponent<Transform3D>(m_PlayerId);

		PlayerSpawnerData* data = GetComponent<PlayerSpawnerData>();
		if (data)
		{
			playerT3D->localPosition = data->Position;
			playerT3D->localRotation = glm::vec3(0.f);
			playerT3D->localScale = glm::vec3(1.f);
		}
		else
		{
			playerT3D->localPosition = m_LastAvailablePos;
			playerT3D->localRotation = glm::vec3(0.f);
			playerT3D->localScale = glm::vec3(1.f);
		}
		WI_INFO("Player set");
	}
}