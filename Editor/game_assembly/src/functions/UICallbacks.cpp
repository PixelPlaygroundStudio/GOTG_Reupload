#include "UICallbacks.h"
#include <Wiwa/scene/SceneManager.h>
#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/Ui/UiManager.h>
#include <Wiwa/core/Application.h>
#include <Wiwa/core/Window.h>
#include <Wiwa/audio/Audio.h>
#include <Wiwa/game/GameMusicManager.h>
#include <Wiwa/game/Items/Inventory.h>
#include <Wiwa/game/Items/ItemManager.h>
#include <Wiwa/core/Renderer3D.h>
#include "../systems/ships/ship_main_menu.h"
#include <iostream>

GoToMilanoHub_ GoToMilanoHub()
{
	Wiwa::GameStateManager::s_CanContinue = false;
	Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
	EntityId parentID = em.GetEntityByName("Parent");
	EntityId shipID = em.GetChildByName(parentID, "Ship");
	Wiwa::ShipMainMenu* ship = em.GetSystem<Wiwa::ShipMainMenu>(shipID);
	ship->SetPanToCamera(true);
	gm.canvas.at(0)->SwapActive();
	Wiwa::GameMusicManager::StartGame();
	Wiwa::Application::Get().GetRenderer3D().EnableSkybox(true);
	return GoToMilanoHub_::hola;
}

GoMainMenu_ GoMainMenu()
{
	Wiwa::SceneManager::ChangeSceneByIndex(2);
	Wiwa::GameStateManager::ResetWave();
	Wiwa::GameMusicManager::MainMenu();
	Wiwa::Application::Get().GetRenderer3D().EnableSkybox(true);
	return GoMainMenu_::hola;
}

Quit_ Quit()
{
	Wiwa::Application::Get().Quit();
	return Quit_();
}

Resume_PauseGame_ Resume_PauseGame()
{
	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
	gm.canvas.at(1)->SwapActive();
	gm.canvas.at(0)->SwapActive();
	Wiwa::SceneManager::PauseCurrentScene();
	return Resume_PauseGame_();
}

ActivateOptionsCanvas_ ActivateOptionsCanvas()
{
	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
	gm.canvas.at(1)->SwapActive();
	gm.canvas.at(2)->SwapActive();
	return ActivateOptionsCanvas_();
}

DeActivateOptionsCanvas_ DeActivateOptionsCanvas()
{
	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
	gm.canvas.at(2)->SwapActive();
	gm.canvas.at(1)->SwapActive();
	return DeActivateOptionsCanvas_();
}

SetFullScreen_ SetFullScreen(bool ret)
{
	Wiwa::Application::Get().GetWindow().SetFullScreen(ret);
	return SetFullScreen_();
}

SetVsync_ SetVsync(bool ret)
{
	Wiwa::Application::Get().GetWindow().SetVSync(ret);
	return SetVsync_();
}

SetVolumeMusic_ SetVolumeMusic(float level)
{
	Audio::ChangeMusicVolume((int)level);
	return SetVolumeMusic_();
}

SetVolumeFX_ SetVolumeFX(float level)
{
	Audio::ChangeSFXVolume((int)level);
	return SetVolumeFX_();
}

PlaceHolderButton_ PlaceHolderButton()
{
	return PlaceHolderButton_();
}

ActivateOptionsMenu_ ActivateOptionsMenu()
{
	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
	gm.canvas.at(0)->SwapActive();
	gm.canvas.at(1)->SwapActive();
	return ActivateOptionsMenu_();
}

DeActivateOptionsMenu_ DeActivateOptionsMenu()
{
	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
	gm.canvas.at(1)->SwapActive();
	gm.canvas.at(0)->SwapActive();
	return DeActivateOptionsMenu_();
}

GoToVideo_ GoToIntroLoreVideo()
{
	Wiwa::SceneManager::ChangeSceneByIndex(1);
	return GoToVideo_::hola;
}

AddHealthCapacitor_ AddHealthCapacitor()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Health Capacitor")->Costs.at(Wiwa::ItemManager::GetShopElement("Health Capacitor")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Health Capacitor"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddHealthCapacitor_();
}

AddNanooBoost_ AddNanooBoost()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Nano Boost")->Costs.at(Wiwa::ItemManager::GetShopElement("Nano Boost")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Nano Boost"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddNanooBoost_();
}

AddEasyTrigger_ AddEasyTrigger()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Easy Trigger")->Costs.at(Wiwa::ItemManager::GetShopElement("Easy Trigger")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Easy Trigger"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddEasyTrigger_();
}

AddFancyBoots_ AddFancyBoots()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Fancy Boots")->Costs.at(Wiwa::ItemManager::GetShopElement("Fancy Boots")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Fancy Boots"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddFancyBoots_();
}

AddLethalShooter_ AddLethalShooter()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Lethal shooter")->Costs.at(Wiwa::ItemManager::GetShopElement("Lethal shooter")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Lethal shooter"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddLethalShooter_();
}

AddShieldFan_ AddShieldFan()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Shield Fan")->Costs.at(Wiwa::ItemManager::GetShopElement("Shield Fan")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Shield Fan"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddShieldFan_();
}

AddNanoMachines_ AddNanoMachines()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Nanomachines")->Costs.at(Wiwa::ItemManager::GetShopElement("Nanomachines")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Nanomachines"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddNanoMachines_();
}

AddRecoveryShield_ AddRecoveryShield()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Recovery Shield")->Costs.at(Wiwa::ItemManager::GetShopElement("Recovery Shield")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Recovery Shield"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddRecoveryShield_();
}

AddDevourer_ AddDevourer()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Devourer")->Costs.at(Wiwa::ItemManager::GetShopElement("Devourer")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Devourer"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddDevourer_();
}

AddFanatic_ AddFanatic()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Fanatic")->Costs.at(Wiwa::ItemManager::GetShopElement("Fanatic")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Fanatic"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddFanatic_();
}

AddRecoveryHealth_ AddRecoveryHealth()
{
	Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();
	size_t tokensHoward = inventory.GetTokensHoward();
	size_t currentCost = Wiwa::ItemManager::GetShopElement("Recovery health")->Costs.at(Wiwa::ItemManager::GetShopElement("Recovery health")->CurrentStep);
	if (tokensHoward >= currentCost)
	{
		inventory.AddShopPassive(*Wiwa::ItemManager::GetShopElement("Recovery health"));
		inventory.SubstractTokensHoward(currentCost);
	}
	return AddRecoveryHealth_();
}

GoToHub_ GoToHub()
{
	Wiwa::GameStateManager::ResetWave();
	Wiwa::SceneManager::ChangeSceneByIndex(3);
	Wiwa::GameMusicManager::MainHub();
	Wiwa::Application::Get().GetRenderer3D().EnableSkybox(true);
	return GoToHub_();
}

GoToOutro_ GoToOutro()
{
	Wiwa::SceneManager::ChangeSceneByIndex(20);
	Wiwa::GameMusicManager::Outro();
	return GoToOutro_();
}

Continue_ Continue()
{
	Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
	EntityId parentID = em.GetEntityByName("Parent");
	EntityId shipID = em.GetChildByName(parentID, "Ship");
	Wiwa::ShipMainMenu* ship = em.GetSystem<Wiwa::ShipMainMenu>(shipID);
	ship->SetPanToCamera(true);
	gm.canvas.at(0)->SwapActive();
	Wiwa::GameMusicManager::StartGame();
	Wiwa::GameStateManager::Continue();
	return Continue_();
}

