#include "wipch.h"
#include "GameMusicManager.h"
#include "Wiwa/audio/Audio.h"
#include "Wiwa/core/Input.h"
#include "Wiwa/core/KeyCodes.h"
#include "Wiwa/scene/SceneManager.h"
#include "Wiwa/core/ProjectManager.h"

namespace Wiwa
{
	uint64_t GameMusicManager::m_PlayEventId;
	bool GameMusicManager::m_Playing = false;
	float GameMusicManager::m_MusicInterporlation = 0.2f;

	void GameMusicManager::Init()
	{
		Logo();	

		Audio::PostEventDefaultListener("play_music", GameMusicManager::m_PlayEventId);
		m_Playing = true;
	}
	void GameMusicManager::CleanUp()
	{
		
	}
	void GameMusicManager::UpdateCombatIntesity( int numEnemies)
	{
		// Define the range of enemy counts that will result in danger levels between 0 and 100
		const float minEnemies = 1.0f;
		const float maxEnemies = 10.0f;

		// Calculate the danger level based on the number of enemies
		float dangerLevel = ((numEnemies - minEnemies) / (maxEnemies - minEnemies)) * 100.0f;

		// Calculate the target danger level based on the elapsed time and interpolation time
		float targetDangerLevel = ((numEnemies - minEnemies) / (maxEnemies - minEnemies)) * 100.0f;
		if (Time::GetDeltaTimeSeconds() < m_MusicInterporlation) {
			targetDangerLevel *= Time::GetDeltaTimeSeconds() / m_MusicInterporlation;
		}

		// Interpolate smoothly between the current danger level and the target danger level
		const float interpolationSpeed = 5.0f;
		dangerLevel += (targetDangerLevel - dangerLevel) * interpolationSpeed * Time::GetDeltaTimeSeconds();

		// Clamp the danger level to the range of 0 to 100
		if (dangerLevel < 0.0f) {
			dangerLevel = 0.0f;
		}
		else if (dangerLevel > 100.0f) {
			dangerLevel = 100.0f;
		}

		Audio::SetRTPCValue("MusicIntensity", dangerLevel);
	}
	void GameMusicManager::StopMusic()
	{
		Audio::SetState("MusicState", "None");
		Audio::StopEvent("play_music");
	}
	void GameMusicManager::Logo()
	{
		Audio::SetState("MenuScene", "Logo");
		Audio::SetState("MusicState", "Menus");
	}
	void GameMusicManager::Intro()
	{
		Audio::SetState("MenuScene", "Intro");
		Audio::SetState("MusicState", "Menus");
	}
	void GameMusicManager::Outro()
	{
		Audio::SetState("MenuScene", "Outro");
		Audio::SetState("MusicState", "Menus");
		Audio::SetState("RoomType", "None");
	}
	void GameMusicManager::MainMenu()
	{
		Audio::SetState("MenuScene", "MainMenu");
		Audio::SetState("MusicState", "Menus");
	}
	void GameMusicManager::StartGame()
	{
		Audio::SetState("MenuScene", "StartGame");
		Audio::SetState("MusicState", "Menus");
	}
	void GameMusicManager::GameOver()
	{
		Audio::SetState("MenuScene", "GameOver");
		Audio::SetState("MusicState", "Menus");
	}
	void GameMusicManager::MainHub()
	{
		Audio::SetState("RoomType", "Hub");
		Audio::SetState("MusicState", "GamePlay");
	}

	void GameMusicManager::Combat()
	{
		Audio::SetState("CombatIntensity", "Low");
		Audio::SetState("RoomType", "Combat");
		Audio::SetState("MusicState", "GamePlay");
	}
	void GameMusicManager::Rewad()
	{
		Audio::SetState("RoomType", "Reward");
		Audio::SetState("MusicState", "GamePlay");
	}
	void GameMusicManager::Shop()
	{
		Audio::SetState("RoomType", "Shop");
		Audio::SetState("MusicState", "GamePlay");
	}
	void GameMusicManager::Boss()
	{
		Audio::SetState("RoomType", "Boss");
		Audio::SetState("MusicState", "GamePlay");
	}

	void GameMusicManager::OnSceneChage(const char* scene_path)
	{
		uint32_t id = Wiwa::ProjectManager::getSceneIndexByPath(scene_path);

		switch (id)
		{
		case 0:
			Logo();
			break;
		case 1: 
			Intro();
			break;
		case 2:
			MainMenu();
			break;
		case 3:
			//hub is made on new game
			break;
		case 19:
			Boss();
			break;
		case 20:
			Outro();
			break;

		default:
			Combat();
			break;
		}
		

		if (!m_Playing)
		{
			Audio::PostEventDefaultListener("play_music", GameMusicManager::m_PlayEventId);
			m_Playing = true;
		}
	}
	
}

