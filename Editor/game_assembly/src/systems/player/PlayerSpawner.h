#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>
#include <glm/glm.hpp>
namespace Wiwa {
	class PlayerSpawner : public System 
	{

	public:
		PlayerSpawner();
		~PlayerSpawner();
		void OnSystemAdded() override;
		void OnAwake() override;
	private:
		EntityManager::ComponentIterator m_DataIt;

		glm::vec3 m_LastAvailablePos;
		bool m_PlayerSet = false;
		EntityId m_PlayerId = -1;
	};
}

REGISTER_SYSTEM(Wiwa::PlayerSpawner);