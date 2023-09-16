#pragma once
#include "System.h"
#include "../components/Transform3D.h"
#include "../components/AudioSource.h"

#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/ecs/EntityManager.h>

namespace Wiwa {
	class WI_API AudioSystem : public System {
	private:
		EntityManager::ComponentIterator m_AudioSource;
		EntityManager::ComponentIterator m_Transform;
		size_t play_id;
		const char* m_CurrentEvent;
		bool destroyed = false;
	public:
		AudioSystem();
		~AudioSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnEventFinish(const char* ev_name);

		void PlayAudio(const char* ev_name);

		void StopEvent(const char* ev_name);
	};
}

WI_REGISTER_SYSTEM(Wiwa::AudioSystem);