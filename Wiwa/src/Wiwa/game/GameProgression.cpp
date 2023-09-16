#include <wipch.h>
#include "GameProgression.h"
#include <Wiwa/core/Input.h>

#include "Wiwa/core/KeyCodes.h"
#include "Wiwa/utilities/time/Time.h"
#include "Wiwa/scene/SceneManager.h"
#include "Wiwa/core/Application.h"
#include <Wiwa/game/Items/ItemManager.h>

Wiwa::GameProgression::GameProgression()
{
	InitGame();
}

Wiwa::GameProgression::~GameProgression()
{
	Clear();
}

void Wiwa::GameProgression::Serialize(JSONDocument* doc)
{
	(*doc).AddMember("enemies_killed", m_PlayerData.m_EnemiesKilled);
	(*doc).AddMember("items_bought", m_PlayerData.m_ItemsBought);
	(*doc).AddMember("killed_ultron_counter", m_PlayerData.m_KilledUltronCount);
	(*doc).AddMember("unlocked_rocket", m_PlayerData.m_UnlockedRocket);
}

void Wiwa::GameProgression::Deserialize(JSONDocument* doc)
{
	if(doc->HasMember("enemies_killed"))
		m_PlayerData.m_EnemiesKilled = (*doc)["enemies_killed"].as_int();
	if (doc->HasMember("items_bought"))
		m_PlayerData.m_ItemsBought = (*doc)["items_bought"].as_int();
	if (doc->HasMember("killed_ultron_counter"))
		m_PlayerData.m_KilledUltronCount = (*doc)["killed_ultron_counter"].as_int();
	if (doc->HasMember("unlocked_rocket"))
		m_PlayerData.m_UnlockedRocket = (*doc)["unlocked_rocket"].as_bool();
}

void Wiwa::GameProgression::InitGame()
{
	m_PlayerData.m_EnemiesKilled = 0;
	m_PlayerData.m_ItemsBought = 0;
	m_PlayerData.m_KilledUltronCount = 0;
	m_PlayerData.m_UnlockedRocket = true;
}

void Wiwa::GameProgression::Update()
{
	//Check with the item manager if any item can be unlocked;
	std::map<std::string, ShopElement> m_ShopElementsMap = Wiwa::ItemManager::GetShopElements();

	for (const auto& shopelement : m_ShopElementsMap)
	{
		if (!shopelement.second.Unlocked)
		{
			switch (shopelement.second.unlockingMethod)
			{
			case Wiwa::ShopElementUnlockingMethod::ENEMIES_KILLED:
				if (m_PlayerData.m_EnemiesKilled >= shopelement.second.amountForUnlocking)
				{
					ItemManager::UnlockShopElement(shopelement.second.Name, true);
				}
				break;
			case Wiwa::ShopElementUnlockingMethod::ITEMS_BOUGHT:
				if (m_PlayerData.m_EnemiesKilled >= shopelement.second.amountForUnlocking)
				{
					ItemManager::UnlockShopElement(shopelement.second.Name, true);
				}
				break;
			case Wiwa::ShopElementUnlockingMethod::ULTRON_KILLED:
				if (m_PlayerData.m_EnemiesKilled >= shopelement.second.amountForUnlocking)
				{
					ItemManager::UnlockShopElement(shopelement.second.Name, true);
				}
				break;
			default:
				break;
			}
		}
	}
	if (m_PlayerData.m_KilledUltronCount >= 1)
		m_PlayerData.m_UnlockedRocket = true;
}

void Wiwa::GameProgression::Clear()
{
	
}
