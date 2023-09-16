#pragma once
#include "System.h"
#include "../components/Transform3D.h"
#include "../components/AudioSource.h"

#include <Wiwa/utilities/Reflection.h>

#include <Wiwa/ecs/EntityManager.h>

namespace Wiwa {
	class WI_API AudioListenerSystem : public System {
	private:
		EntityManager::ComponentIterator m_AudioListener;
		EntityManager::ComponentIterator m_Transform;
	public:
		AudioListenerSystem();
		~AudioListenerSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

	};
}

WI_REGISTER_SYSTEM(Wiwa::AudioListenerSystem);
