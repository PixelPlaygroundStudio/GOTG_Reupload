#pragma once

#include <Wiwa/core/Core.h>
#include <Wiwa/audio/Audio.h>
namespace  Wiwa {

	class WI_API GameMusicManager
	{
	public:
		static uint64_t m_PlayEventId;
		static bool m_Playing ;
		static float m_MusicInterporlation ;
		static void Init();
		static void CleanUp();
		
		void static StopMusic();

		//menu states
		void static Logo();
		void static Intro();
		void static Outro();
		void static MainMenu();
		void static StartGame();
		void static GameOver();
		//game states
		void static MainHub();
		void static Combat();
		void static Rewad();
		void static Shop();
		void static Boss();

		void static UpdateCombatIntesity(int n_enemies);

		void static OnSceneChage(const char* scene_path);
	};

}
