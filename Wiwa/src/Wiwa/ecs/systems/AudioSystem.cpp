#include <wipch.h>
#include "AudioSystem.h"
#include <Wiwa/audio/Audio.h>

namespace Wiwa {
	AudioSystem::AudioSystem()
	{
		m_AudioSource = { WI_INVALID_INDEX, WI_INVALID_INDEX };
	}

	AudioSystem::~AudioSystem()
	{

	}

	void AudioSystem::OnAwake()
	{
		Audio::RegisterGameObject(m_EntityId);

		m_AudioSource = GetComponentIterator<AudioSource>();
		m_Transform = GetComponentIterator<Transform3D>();

		if (m_AudioSource.c_id == WI_INVALID_INDEX) return;

		AudioSource* asrc = GetComponentByIterator<AudioSource>(m_AudioSource);

		m_CurrentEvent = asrc->eventName;


		if(asrc->isDefaultListener){
			Audio::AddDefaultListener(m_EntityId);
		}
	}

	void AudioSystem::OnInit()
	{
		if (m_AudioSource.c_id == WI_INVALID_INDEX) return;


		//Set the go listener, we use the global world listener, the player.
		Audio::SetWorldListener(m_EntityId);

		AudioSource* asrc = GetComponentByIterator<AudioSource>(m_AudioSource);

		if (asrc->playOnAwake)
		{
			if (!Audio::PostEvent(asrc->eventName, m_EntityId, { &AudioSystem::OnEventFinish, this })) {
				WI_CORE_ERROR("Audio couldn't post event [{}]", Audio::GetLastError());
			}

			asrc->isPlaying = true;
		}
	}

	void AudioSystem::OnUpdate()
	{
		if (m_AudioSource.c_id == WI_INVALID_INDEX) return;

		Transform3D* t3d = GetComponentByIterator<Transform3D>(m_Transform);		

		if (!Audio::SetPositionAndOrientation(m_EntityId, t3d->position, Math::Forward(t3d->worldMatrix), glm::vec3(0,1,0))) {
			WI_CORE_ERROR("Audio couldn't set position [{}]", Audio::GetLastError());
		}
	}

	void AudioSystem::OnDestroy()
	{
		if (m_AudioSource.c_id == WI_INVALID_INDEX) return;

		AudioSource* asrc = GetComponentByIterator<AudioSource>(m_AudioSource);

		if (asrc->isPlaying) {
			if (!Audio::StopEvent(m_CurrentEvent, m_EntityId)) {
				WI_CORE_ERROR("Audio couldn't stop event [{}]", Audio::GetLastError());
			}

			asrc->isPlaying = false;

			Audio::CancelAudioAllCallback(play_id);

			Audio::Update();

			// Wait to update audio engine thread so we can destroy the system
			Sleep(100);

			Audio::Update();

			Audio::CancelAudioAllCallback(play_id);
			// Wait to update audio engine thread so we can destroy the system
			Sleep(100);

			Audio::Update();
		}

		Audio::UnregisterGameObject(m_EntityId);

		destroyed = true;
	}

	void AudioSystem::OnEventFinish(const char* ev_name)
	{
		if (!this || m_AudioSource.c_id == WI_INVALID_INDEX)
			return;

		if (destroyed)
			return;
		

		AudioSource* asrc = GetComponentByIterator<AudioSource>(m_AudioSource);

		if (asrc != nullptr && strcmp(ev_name, m_CurrentEvent) == 0) {
			asrc->isPlaying = false;
		}
	}

	void AudioSystem::PlayAudio(const char* ev_name)
	{
		if (m_AudioSource.c_id == WI_INVALID_INDEX) return;

		AudioSource* asrc = GetComponentByIterator<AudioSource>(m_AudioSource);

		m_CurrentEvent = ev_name;
		
		if (!Audio::PostEvent(m_CurrentEvent, m_EntityId, { &AudioSystem::OnEventFinish, this }, play_id)) {
			WI_CORE_ERROR("Audio couldn't post event [{}]", Audio::GetLastError());
			asrc->isPlaying = false;
		}
		else
		{
			asrc->isPlaying = true;
		}
	}
	void AudioSystem::StopEvent(const char* ev_name)
	{
		if (!Audio::StopEvent(m_CurrentEvent, m_EntityId)) {
			WI_CORE_ERROR("Audio couldn't stop event [{}]", Audio::GetLastError());
		}
	}
}