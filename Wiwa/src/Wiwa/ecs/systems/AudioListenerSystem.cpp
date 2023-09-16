#include "wipch.h"
#include "AudioListenerSystem.h"
#include <Wiwa/audio/Audio.h>
#include <Wiwa/ecs/components/AudioListener.h>

namespace Wiwa {
	AudioListenerSystem::AudioListenerSystem()
	{
		m_AudioListener = { WI_INVALID_INDEX, WI_INVALID_INDEX };
	}
	AudioListenerSystem::~AudioListenerSystem()
	{
	}
	void AudioListenerSystem::OnAwake()
	{
		m_AudioListener = GetComponentIterator<AudioListener>();
		m_Transform = GetComponentIterator<Transform3D>();

		if (m_AudioListener.c_id == WI_INVALID_INDEX) return;

		//register as world listener, in our case the player
		//we only have one listener

		Audio::RegisterAsWorldListener(m_EntityId);

		Transform3D* t3d = GetComponentByIterator<Transform3D>(m_Transform);

		if (!Audio::SetPositionAndOrientation(m_EntityId, t3d->position, Vector3F::FRONT, Vector3F::UP)) {
			WI_CORE_ERROR("Audio couldn't set position [{}]", Audio::GetLastError());
		}

	}
	void AudioListenerSystem::OnInit()
	{
		if (m_AudioListener.c_id == WI_INVALID_INDEX) return;
	}
	void AudioListenerSystem::OnUpdate()
	{
		if (m_AudioListener.c_id == WI_INVALID_INDEX) return;

		Transform3D* t3d = GetComponentByIterator<Transform3D>(m_Transform);

		if (t3d == nullptr)
			return;

		if (!Audio::SetPositionAndOrientation(m_EntityId, t3d->position, Math::Forward(t3d->worldMatrix), Math::Up(t3d->worldMatrix))) {
			WI_CORE_ERROR("Audio couldn't set position [{}]", Audio::GetLastError());
		}
	}
	void AudioListenerSystem::OnDestroy()
	{
		if (m_AudioListener.c_id == WI_INVALID_INDEX) return;

		Audio::UnregisterGameObject(m_EntityId);
	}
}