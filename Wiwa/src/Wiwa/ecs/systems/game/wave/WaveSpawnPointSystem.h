#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class WI_API WaveSpawnPointSystem : public System {
	private:
		EntityManager::ComponentIterator m_SpawnPointIt;

	public:
		WaveSpawnPointSystem();
		~WaveSpawnPointSystem();

		void OnAwake() override;
	};
}

REGISTER_SYSTEM(Wiwa::WaveSpawnPointSystem);