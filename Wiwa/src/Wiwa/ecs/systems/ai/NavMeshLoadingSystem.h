#pragma once
#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/ecs/components/ai/NavMesh.h>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class WI_API NavMeshLoadingSystem : public System {
	private:

	public:
		NavMeshLoadingSystem();
		~NavMeshLoadingSystem();

		void OnSystemAdded() override;

		void OnSystemRemoved() override;

		void OnUpdate() override;
	};
}

REGISTER_SYSTEM(Wiwa::NavMeshLoadingSystem);