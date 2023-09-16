#include "ContinueLoading.h"
#include <Wiwa/game/GameStateManager.h>

#include "Wiwa/game/Items/ItemManager.h"

namespace Wiwa
{

	Wiwa::ContinueLoading::ContinueLoading()
	{

	}

	ContinueLoading::~ContinueLoading()
	{
	}

	void ContinueLoading::OnAwake()
	{
	}

	void ContinueLoading::OnInit()
	{

	}

	void ContinueLoading::OnUpdate()
	{
		if (GameStateManager::s_CanContinue && !StatsLoaded)
		{
			Wiwa::GameStateManager::GetPlayerInventory().UseAllPassives();

			StatsLoaded = true;
		}
	}

	void ContinueLoading::OnDestroy()
	{
	}

	void ContinueLoading::OnCollisionEnter(Object* body1, Object* body2)
	{
		
	}

	void ContinueLoading::OnCollisionExit(Object* body1, Object* body2)
	{
	}
}