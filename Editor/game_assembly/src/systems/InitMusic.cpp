#include "wipch.h"
#include "InitMusic.h"
#include "Wiwa/game/GameMusicManager.h"

namespace Wiwa {
	InitMusic::InitMusic()
	{
	}
	InitMusic::~InitMusic()
	{
	}
	void InitMusic::OnAwake()
	{
	}
	void InitMusic::OnInit()
	{
		GameMusicManager::Init();
	}
	void InitMusic::OnDestroy()
	{
	}
}