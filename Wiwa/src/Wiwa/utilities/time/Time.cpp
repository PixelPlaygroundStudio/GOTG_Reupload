#include "wipch.h"
#include "Time.h"

namespace Wiwa
{

	std::chrono::time_point<std::chrono::steady_clock> Time::m_RealTimeClock = std::chrono::high_resolution_clock::now();
	uint32_t Time::m_FrameCount = 0;
	uint32_t Time::m_GameFrameCount = 0;
	float Time::m_TimeScale = 1.0f;
	bool Time::m_IsPlaying = true;
	bool Time::m_IsPaused = false;
	bool Time::m_Step = false;
	std::chrono::time_point<std::chrono::steady_clock> Time::m_GameClock = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float>  Time::m_Time;
	std::chrono::duration<float>  Time::m_DeltaTime;
	std::chrono::time_point<std::chrono::steady_clock>  Time::m_RealLastTime;
	std::chrono::time_point<std::chrono::steady_clock>  Time::m_LastTime;
	std::chrono::duration<float>  Time::m_RealTimeSinceStartup;
	std::chrono::duration<float>  Time::m_RealTimeDeltaTime;
	int Time::s_NoFrameCap = -1;
	int Time::m_TargetFPS = s_NoFrameCap;
	double Time::m_TargetDT = 1.0 / m_TargetFPS;
	void Time::Play()
	{
		m_IsPlaying = true;
		m_GameClock = std::chrono::high_resolution_clock::now();
		m_TimeScale = 1;
		m_GameFrameCount = 0;
	}
	void Time::Step()
	{
		m_Step = true;
		m_IsPlaying = true;
	}
	void Time::PauseUnPause()
	{
		m_IsPaused = !m_IsPaused;
	}
	void Time::Pause()
	{
		m_IsPaused = true;
	}
	void Time::UnPause()
	{
		m_IsPaused = false;
	}
	void Time::Update()
	{
		OPTICK_EVENT("Time Update");
		m_FrameCount++;
		m_RealTimeDeltaTime = std::chrono::high_resolution_clock::now() - m_RealLastTime;
		m_RealLastTime = std::chrono::high_resolution_clock::now();
		m_RealTimeSinceStartup = std::chrono::high_resolution_clock::now() - m_RealTimeClock;

		if (m_IsPlaying)
		{
			m_DeltaTime = (std::chrono::high_resolution_clock::now() - m_LastTime) * m_TimeScale;
			m_Time = (std::chrono::high_resolution_clock::now() - m_GameClock) * m_TimeScale;
			m_LastTime = std::chrono::high_resolution_clock::now();
			if (m_IsPaused && !m_Step)
				return;
			m_GameFrameCount++;
		}
		
	}
	void Time::PostUpdate()
	{
		if (m_Step && m_IsPlaying)
		{
			m_Step = false;
			m_IsPlaying = false;
		}
	}
	void Time::Stop()
	{
		m_IsPlaying = false;
		m_TimeScale = 0;
	}
	void Time::SetTimeScale(float value)
	{
		m_TimeScale = value;
		CLAMP(m_TimeScale, -2, 5);
	}
	
}