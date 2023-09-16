#include "wipch.h"
#include "PlayerGUISystem.h"
#include "Wiwa/game/Items/ItemManager.h"
#include "Wiwa/scene/SceneManager.h"
#include "Wiwa/core/Input.h"
#include "Wiwa/audio/Audio.h"
#include <Wiwa/Dialog/DialogManager.h>
#include <Wiwa/Dialog/DialogEventManager.h>

void Wiwa::PlayerGUISystem::OnInit()
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
	for (size_t i = 0; i < 18; i++)
	{
		int index_passives = 15 + i;
		int index_passives_death = 12 + i;
		gm.canvas.at(PauseHUD)->controls.at(index_passives)->SetNextFrame(0, &r2d);
		gm.canvas.at(DeathHUD)->controls.at(index_passives_death)->SetNextFrame(0, &r2d);
	}
	gm.canvas.at(OptionsHUD)->controls.at(2)->SetInitialSliderValue(Wiwa::Application::Get().ReturnMusicVolume());
	gm.canvas.at(OptionsHUD)->controls.at(3)->SetInitialSliderValue(Wiwa::Application::Get().ReturnSFXVolume());
	gm.canvas.at(OptionsHUD)->controls.at(0)->SetChecked(Wiwa::Application::Get().GetWindow().IsVSync());
	gm.canvas.at(OptionsHUD)->controls.at(1)->SetChecked(Wiwa::Application::Get().GetWindow().GetFullScreen());
	m_CurrentTime = 0;
	//frequency of the animation movement
	m_Frequency = .001f;

	////init start scale values here because its not working in the constructor
	for (int i = 0; i < gm.canvas.at(CanvasHUD)->controls.size(); i++)
	{
		GuiControl* control = gm.canvas.at(CanvasHUD)->controls[i];
		control->startPos = control->position;
	}
}

void Wiwa::PlayerGUISystem::OnUpdate()
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	Character* character = Wiwa::GameStateManager::GetPlayerCharacterComp();
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
	CharacterInventory* characterInventory = m_Scene->GetEntityManager().GetSystem<CharacterInventory>(m_EntityId);
	m_CurrentTime += Time::GetDeltaTime();
	PlayerElements(gm, character);
	HandleCurrentCanvas(gm);
	if (CurrentWave != Wiwa::GameStateManager::GetCurrentWave())
	{
		HandleWaves(gm);
		CurrentWave = Wiwa::GameStateManager::GetCurrentWave();
	}

	Wiwa::OzzAnimationSystem* animator = m_Scene->GetEntityManager().GetSystem<Wiwa::OzzAnimationSystem>(m_EntityId);
	if (animator->getAnimator()->getAnimationByName("death")->HasFinished() && !deathHud)
	{
		if (!GameStateManager::ReturnFanaticEffect())
		{
			DeathHud(gm);
			if (Wiwa::GameStateManager::s_CurrentCharacter == 0)
			{
				//STARLORD
				gm.canvas.at(DeathHUD)->controls.at(1)->SwapToNewTexture("library/hudimages/menus/gameovermenu_02/ui_speechmenustarlordwithshadows_01.dds",Wiwa::Application::Get().GetRenderer2D());
				r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(DeathHUD)->controls.at(1)->id_quad_normal, 1);
				r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(DeathHUD)->controls.at(0)->id_quad_normal, 2);



			}
			else if (Wiwa::GameStateManager::s_CurrentCharacter == 1)
			{
				//ROCKET
				gm.canvas.at(DeathHUD)->controls.at(1)->SwapToNewTexture("library/hudimages/menus/gameovermenu_02/ui_speechmenurocket_01.dds", Wiwa::Application::Get().GetRenderer2D());
				r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(DeathHUD)->controls.at(1)->id_quad_normal, 1);
				r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(DeathHUD)->controls.at(0)->id_quad_normal, 2);


			}
			deathHud = true;
		}
	}
	if (characterInventory != NULL)
	{
		if (characterInventory->GetShopActive() && updateShopHUD)
		{
			ShopElementsHUD(characterInventory->GetCurrentShopItem(),gm);
		}
		if (characterInventory->PlayerUpdated())
		{
			HandlePlayerStatistics(character->MaxHealth, character->MaxShield, 10.0f, character->Damage, character->Speed, character->RateOfFire, gm);
		}
	}
}

void Wiwa::PlayerGUISystem::HandleCurrentCanvas(Wiwa::GuiManager& gm)
{
	CharacterInventory* characterInventory = m_Scene->GetEntityManager().GetSystem<CharacterInventory>(m_EntityId);
	Wiwa::DialogManager& dm = m_Scene->GetDialogManager();
	Wiwa::DialogEventManager& dEm = m_Scene->GetDialogEventManager();

	CurrentHUD = gm.getCurrentCanvas();

	if (CurrentHUD == CanvasHUD)
	{

		if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadStart) && dm.actualConversationState == 2 && dEm.eventState == 2)
		{
			pauseGame = true;

		}
		if (Wiwa::Input::IsButtonReleased(Gamepad::GamePad1, Key::GamepadStart) && pauseGame)
		{
			if (Audio::FindEvent("game_pause") != Audio::INVALID_ID)
			{
				Audio::PostEvent("game_pause");
			}
			Wiwa::Application::Get().GetRenderer3D().EnableSkybox(true);
			gm.canvas.at(CanvasHUD)->SwapActive();
			gm.canvas.at(PauseHUD)->SwapActive();
			m_Scene->SwapPauseActive();

			pauseGame = false;
		}

		if (Wiwa::Input::IsKeyPressed(Key::Escape) && dm.actualConversationState == 2 && dEm.eventState == 2)
		{
			pauseGame = true;
			Wiwa::Application::Get().GetRenderer3D().EnableSkybox(pauseGame);
		}
		if (Wiwa::Input::IsKeyReleased(Key::Escape) && pauseGame)
		{
			if (Audio::FindEvent("game_pause") != Audio::INVALID_ID)
			{
				Audio::PostEvent("game_pause");
			}
			Wiwa::Application::Get().GetRenderer3D().EnableSkybox(true);
			gm.canvas.at(CanvasHUD)->SwapActive();
			gm.canvas.at(PauseHUD)->SwapActive();
			m_Scene->SwapPauseActive();
			pauseGame = false;
		}


	}
	else if (CurrentHUD == PauseHUD)
	{
		if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadB))
		{
			returnToHUD = true;
		}
		if (returnToHUD && Wiwa::Input::IsButtonReleased(Gamepad::GamePad1, Key::GamepadB))
		{
			gm.canvas.at(PauseHUD)->SwapActive();
			gm.canvas.at(CanvasHUD)->SwapActive();
			m_Scene->SwapPauseActive();
			returnToHUD = false;
			Wiwa::Application::Get().GetRenderer3D().EnableSkybox(false);
		}

		if (Wiwa::Input::IsKeyPressed(Key::Backspace))
		{
			returnToHUD = true;
		}
		if (returnToHUD && Wiwa::Input::IsKeyReleased(Key::Backspace))
		{
			gm.canvas.at(PauseHUD)->SwapActive();
			gm.canvas.at(CanvasHUD)->SwapActive();
			m_Scene->SwapPauseActive();
			returnToHUD = false;
			Wiwa::Application::Get().GetRenderer3D().EnableSkybox(false);
		}
	}
	else if (CurrentHUD == OptionsHUD)
	{
		if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadB))
		{
			returnToPauseHUD = true;
		}
		if (returnToPauseHUD && Wiwa::Input::IsButtonReleased(Gamepad::GamePad1, Key::GamepadB))
		{
			gm.canvas.at(OptionsHUD)->SwapActive();
			gm.canvas.at(PauseHUD)->SwapActive();
			OnSaveEvent event;
			Wiwa::Application::Get().OnEvent(event);
			returnToPauseHUD = false;
		}

		if (Wiwa::Input::IsKeyPressed(Key::Backspace))
		{
			returnToPauseHUD = true;
		}
		if (returnToPauseHUD && Wiwa::Input::IsKeyReleased(Key::Backspace))
		{
			gm.canvas.at(OptionsHUD)->SwapActive();
			gm.canvas.at(PauseHUD)->SwapActive();
			OnSaveEvent event;
			Wiwa::Application::Get().OnEvent(event);
			returnToPauseHUD = false;
		}
	}
	else if (CurrentHUD == ShopHUD)
	{
		if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA))
		{
			buyItem = true;
		}
		if (buyItem && Wiwa::Input::IsButtonReleased(Gamepad::GamePad1, Key::GamepadA))
		{
			if (Audio::FindEvent("action_accepted") != Audio::INVALID_ID)
			{
				Audio::PostEvent("action_accepted");
			}
			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			characterInventory->ShopElement(characterInventory->GetCurrentShopItem());
			buyItem = false;
			em.DestroyEntity(characterInventory->GetShopItemId());
			gm.canvas.at(ShopHUD)->SwapActive();
			gm.canvas.at(CanvasHUD)->SwapActive();
		}

		if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadB))
		{
			gm.canvas.at(ShopHUD)->SwapActive();
			gm.canvas.at(CanvasHUD)->SwapActive();
		}


		if (Wiwa::Input::IsKeyPressed(Key::Space))
		{
			buyItem = true;
		}
		if (buyItem && Wiwa::Input::IsKeyReleased(Key::Space))
		{

			Wiwa::EntityManager& em = m_Scene->GetEntityManager();
			if (characterInventory->ShopElement(characterInventory->GetCurrentShopItem()))
			{
				if (Audio::FindEvent("action_accepted") != Audio::INVALID_ID)
				{
					Audio::PostEvent("action_accepted");
				}
				em.DestroyEntity(characterInventory->GetShopItemId());
			}
			else {
				if (Audio::FindEvent("action_denied") != Audio::INVALID_ID)
				{
					Audio::PostEvent("action_denied");
				}
				buyItem = false;
				gm.canvas.at(ShopHUD)->SwapActive();
				gm.canvas.at(CanvasHUD)->SwapActive();
			}

		}

		if (Wiwa::Input::IsKeyPressed(Key::Escape))
		{
			gm.canvas.at(ShopHUD)->SwapActive();
			gm.canvas.at(CanvasHUD)->SwapActive();
		}
	}

}

void Wiwa::PlayerGUISystem::DeathHud(Wiwa::GuiManager& gm)
{
	Wiwa::SceneManager::PauseCurrentScene();
	gm.canvas.at(CanvasHUD)->SwapActive();
	gm.canvas.at(DeathHUD)->SwapActive();
}

void Wiwa::PlayerGUISystem::CooldownState(Ability** ability,Wiwa::GuiManager& gm)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
	for (int i = 0; i < 2; i++)
	{
		if (ability != nullptr)
		{
			if (ability[i] != nullptr)
			{
				if (ability[i]->CurrentTime >= ability[i]->Cooldown)
				{
					ability[i]->CooldownState = CooldownState::FULLY_CHARGED;
					int index = i + 15;
					GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
				}
				else if (ability[i]->CurrentTime < ability[i]->Cooldown && ability[i]->CurrentTime >= ability[i]->Cooldown / 2)
				{
					ability[i]->CooldownState = CooldownState::MEDIUM_CHARGE;
					int index = i + 15;
					gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame((int)ability[i]->CooldownState, &r2d);
				}
				else if (ability[i]->CurrentTime < ability[i]->Cooldown / 2 && ability[i]->CurrentTime > 0)
				{
					ability[i]->CooldownState = CooldownState::STARTING_CHARGE;
					int index = i + 15;
					gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame((int)ability[i]->CooldownState, &r2d);
				}
				else if (ability[i]->CurrentTime < 0.0f)
				{
					ability[i]->CooldownState = CooldownState::NO_CHARGED;
					int index = i + 15;
					gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame((int)ability[i]->CooldownState, &r2d);
				}
				
			}
		}
		
	}
	
}

void Wiwa::PlayerGUISystem::CooldownState(Buff** buff, Wiwa::GuiManager& gm)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

	
	for (int i = 0; i < 2; i++)
	{
		if (buff != nullptr)
		{
			if (buff[i] != nullptr)
			{
				if (!buff[i]->IsActive)
				{
					if (buff[i]->CurrentTime >= buff[i]->Cooldown)
					{
						buff[i]->CooldownState = CooldownState::FULLY_CHARGED;
						int index = i + 13;
						GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
						control->SetNextFrame((int)buff[i]->CooldownState, &r2d);
					}
					else if (buff[i]->CurrentTime < buff[i]->Cooldown && buff[i]->CurrentTime >= buff[i]->Cooldown / 2)
					{
						buff[i]->CooldownState = CooldownState::MEDIUM_CHARGE;
						int index = i + 13;
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame((int)buff[i]->CooldownState, &r2d);
					}
					else if (buff[i]->CurrentTime < buff[i]->Cooldown / 2 && buff[i]->CurrentTime > 0)
					{
						buff[i]->CooldownState = CooldownState::STARTING_CHARGE;
						int index = i + 13;
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame((int)buff[i]->CooldownState, &r2d);
					}
					else if (buff[i]->CurrentTime < 0.0f)
					{
						buff[i]->CooldownState = CooldownState::NO_CHARGED;
						int index = i + 13;
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame((int)buff[i]->CooldownState, &r2d);
					}
				}
				else if(buff[i]->IsActive)
				{
					buff[i]->CooldownState = CooldownState::NO_CHARGED;
					int index = i + 13;
					gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame((int)buff[i]->CooldownState, &r2d);
				}
					
			}					
		}
	}
	
	
}

void Wiwa::PlayerGUISystem::HandleActiveAbilities(Ability** ability, Wiwa::GuiManager& gm)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

		for (int i = 0; i < 2; i++)
		{
			int index = i + 11;
			int index_pause = i + 7;
			int index_death = i + 4;
			if (ability != nullptr)
			{
				if (ability[i] != nullptr)
				{
					switch (ability[i]->AbilityType)
					{
					case AbilityType::YONDUS_FIN:
						if (ability[i]->CooldownState != CooldownState::FULLY_CHARGED)
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(6, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(6, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(6, &r2d);						
						}
						else
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(5, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(5, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(5, &r2d);
							GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
							control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
							control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
						}
						break;
					case AbilityType::GROOTS_SEEDS:
						if (ability[i]->CooldownState != CooldownState::FULLY_CHARGED)
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(4, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(4, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(4, &r2d);
						}
						else
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(3, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(3, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(3, &r2d);
							GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
							control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
							control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
						}
						break;
					case AbilityType::PHYLAS_QUANTUM_SWORD:
						if (ability[i]->CooldownState != CooldownState::FULLY_CHARGED)
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(2, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(2, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(2, &r2d);

						}
						else
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(1, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(1, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(1, &r2d);
							GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
							control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
							control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
						}
						break;
					case AbilityType::STARHAWKS_BLAST:
						if (ability[i]->CooldownState != CooldownState::FULLY_CHARGED)
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(8, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(8, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(8, &r2d);
						
						}
						else
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(7, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(7, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(7, &r2d);
							GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
							control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
							control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
						}
						break;
					case AbilityType::CAPTAINS_UNIVERSE:
						if (ability[i]->CooldownState != CooldownState::FULLY_CHARGED)
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(11, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(11, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(11, &r2d);
						
						}
						else
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(12, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(12, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(12, &r2d);
							GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
							control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
							control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
						}
						break;
					case AbilityType::MANTIS_TELEPATHIC_THRUST:
						if (ability[i]->CooldownState != CooldownState::FULLY_CHARGED)
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(9, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(9, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(9, &r2d);
						
						}
						else
						{
							gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(10, &r2d);
							gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(10, &r2d);
							gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(10, &r2d);
							GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
							control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
							control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
						}
						break;
					default:
						break;
					}
				}
				else
				{
					gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(0, &r2d);
					gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(0, &r2d);
					gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(0, &r2d);
				}
			}
			else
			{
				gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(0, &r2d);
				gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(0, &r2d);
				gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(0, &r2d);
			}
		}
	
}

void Wiwa::PlayerGUISystem::HandleActiveBuffs(Buff** buff, Wiwa::GuiManager& gm)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

	
	for (int i = 0; i < 2; i++)
	{
		int index = i + 9;
		int index_pause = i + 5;
		int index_death = i + 2;
		if (buff != nullptr)
		{
			if (buff[i] != nullptr)
			{
				switch (buff[i]->buffType)
				{
				case BuffType::NIKKIS_TOUCH:
					if (buff[i]->CooldownState != CooldownState::FULLY_CHARGED)
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(12, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(12, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(12, &r2d);
					}
					else
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(11, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(11, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(11, &r2d);
						GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
						control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
						control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
					}
					break;
				case BuffType::MARTINEX_THERMOKINESIS:
					if (buff[i]->CooldownState != CooldownState::FULLY_CHARGED)
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(10, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(10, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(10, &r2d);
					
					}
					else
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(9, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(9, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(9, &r2d);
						GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
						control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
						control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
					}
					break;
				case BuffType::MAJOR_VICTORY_SHIELD:
					if (buff[i]->CooldownState != CooldownState::FULLY_CHARGED)
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(8, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(8, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(8, &r2d);
					
					}
					else
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(7, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(7, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(7, &r2d);
						GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
						control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
						control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
					}
					break;
				case BuffType::CHARLIE27_FIST:
					if (buff[i]->CooldownState != CooldownState::FULLY_CHARGED)
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(6, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(6, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(6, &r2d);
					
					}
					else
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(5, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(5, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(5, &r2d);
						GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
						control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
						control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
					}
					break;
				case BuffType::COSMOS_PAW:
					if (buff[i]->CooldownState != CooldownState::FULLY_CHARGED)
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(4, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(4, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(4, &r2d);
					
					}
					else
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(3, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(3, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(3, &r2d);
						GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
						control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
						control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
					}
					break;
				case BuffType::BUGS_LEGS:
					if (buff[i]->CooldownState != CooldownState::FULLY_CHARGED)
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(2, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(2, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(2, &r2d);
					
					}
					else
					{
						gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(1, &r2d);
						gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(1, &r2d);
						gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(1, &r2d);
						GuiControl* control = gm.canvas.at(CanvasHUD)->controls.at(index);
						control->position.width = control->startPos.width + (control->startPos.width * .1f * sin(m_CurrentTime * m_Frequency));
						control->position.height = control->startPos.height + (control->startPos.height * .1f * sin(m_CurrentTime * m_Frequency));
					}
					break;
				default:
					break;
				}

			}
			else
			{
				gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(0, &r2d);
				gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(0, &r2d);
				gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(0, &r2d);
			}
		}
		else
		{
			gm.canvas.at(CanvasHUD)->controls.at(index)->SetNextFrame(0, &r2d);
			gm.canvas.at(PauseHUD)->controls.at(index_pause)->SetNextFrame(0, &r2d);
			gm.canvas.at(DeathHUD)->controls.at(index_death)->SetNextFrame(0, &r2d);
		}
	}
}

void Wiwa::PlayerGUISystem::HandleActivePassives(std::vector<PassiveSkill> PassiveSkills, Wiwa::GuiManager& gm)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

	std::map<std::string, int> passiveMap = {
		{"Drax_Belt",0},
		{"Angela_Crown",1},
		{"Star_Lord_Walkman",2},
		{"Gamora_Hood",3},
		{"Ikons_Battery",4},
		{"Iron_Man_Insurance",5},
		{"Jack_Flag_Gloves",6},
		{"Xandarian_Worldmind",7},
		{"Adam_Warlock_Blessing",8},
		{"Nova_Helmet",9}
	};
	if (!PassiveSkills.empty())
	{
		size_t amountOfPassives = PassiveSkills.size();
		if (amountOfPassives > 18) amountOfPassives = 18;
		for (size_t i = 0; i < amountOfPassives; i++)
		{
			int index_passives = 15 + i;
			int index_passives_death = 12 + i;
			auto passiveID = passiveMap.find(PassiveSkills.at(i).Name);
			if (passiveID != passiveMap.end())
			{
				int idForItem = passiveID->second + 1;
				gm.canvas.at(PauseHUD)->controls.at(index_passives)->SetNextFrame(idForItem, &r2d);
				gm.canvas.at(DeathHUD)->controls.at(index_passives_death)->SetNextFrame(idForItem, &r2d);
			}
			else
			{
				gm.canvas.at(PauseHUD)->controls.at(index_passives)->SetNextFrame(0, &r2d);
				gm.canvas.at(DeathHUD)->controls.at(index_passives_death)->SetNextFrame(0, &r2d);
			}
		}
	}
}

void Wiwa::PlayerGUISystem::PlayerElements(Wiwa::GuiManager& gm, Character* character)
{
	//Update hp & shield
	gm.canvas.at(CanvasHUD)->controls.at(1)->SetValueForUIbar(character->Health, character->MaxHealth);
	gm.canvas.at(CanvasHUD)->controls.at(2)->SetValueForUIbar(character->Shield, character->MaxShield);

	abilitiesList = Wiwa::GameStateManager::GetPlayerInventory().GetAbilities();
	buffsList = Wiwa::GameStateManager::GetPlayerInventory().GetBuffs();
	passives = Wiwa::GameStateManager::GetPlayerInventory().GetPassives();

	if (lastCoins != Wiwa::GameStateManager::GetPlayerInventory().GetTokens())
	{
		Coins(Wiwa::GameStateManager::GetPlayerInventory().GetTokens(), gm);
	}

	if (lastPromethiumGems != Wiwa::GameStateManager::GetPlayerInventory().GetTokensHoward())
	{
		PromethiumGems(Wiwa::GameStateManager::GetPlayerInventory().GetTokensHoward(), gm);
	}

	CooldownState(abilitiesList, gm);
	CooldownState(buffsList, gm);
	HandleActiveAbilities(abilitiesList, gm);
	HandleActiveBuffs(buffsList, gm);
	HandleActivePassives(passives, gm);
	lastCoins = Wiwa::GameStateManager::GetPlayerInventory().GetTokens();
	lastPromethiumGems = Wiwa::GameStateManager::GetPlayerInventory().GetTokensHoward();


}

void Wiwa::PlayerGUISystem::Coins(uint32_t coins, Wiwa::GuiManager& gm)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
	std::string my_string = std::to_string(coins);

	gm.canvas.at(CanvasHUD)->controls.at(5)->text = my_string.c_str();
	gm.canvas.at(ShopHUD)->controls.at(2)->text = my_string.c_str();
	Text* newText = gm.InitFont("library/Fonts/Jade_Smile.ttf", my_string.c_str());
	r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(CanvasHUD)->controls.at(5)->id_quad_normal, 1);
	r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(CanvasHUD)->controls.at(5)->id_quad_normal, newText->GetTextureId());
	r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(CanvasHUD)->controls.at(5)->id_quad_normal, newText->GetSize(), {0,0,512,512});

	r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(2)->id_quad_normal, 1);
	r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(2)->id_quad_normal, newText->GetTextureId());
	r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(2)->id_quad_normal, newText->GetSize(), { 0,0,512,512 });
}

void Wiwa::PlayerGUISystem::PromethiumGems(uint32_t promethiumGems, Wiwa::GuiManager& gm)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
	std::string my_string = std::to_string(promethiumGems);

	gm.canvas.at(CanvasHUD)->controls.at(5)->text = my_string.c_str();
	gm.canvas.at(ShopHUD)->controls.at(2)->text = my_string.c_str();
	Text* newText = gm.InitFont("library/Fonts/Jade_Smile.ttf", my_string.c_str());
	r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(CanvasHUD)->controls.at(6)->id_quad_normal, 1);
	r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(CanvasHUD)->controls.at(6)->id_quad_normal, newText->GetTextureId());
	r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(CanvasHUD)->controls.at(6)->id_quad_normal, newText->GetSize(), { 0,0,512,512 });
}

void Wiwa::PlayerGUISystem::HandlePlayerStatistics(int max_Health, int max_Shield, float range, int attack, float speed, float rateOfFire, Wiwa::GuiManager& gm)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

	std::string maxHealthString = std::to_string(max_Health);
	gm.canvas.at(DeathHUD)->controls.at(6)->text = maxHealthString.c_str();
	gm.canvas.at(PauseHUD)->controls.at(9)->text = maxHealthString.c_str();

	std::string maxShieldString = std::to_string(max_Shield);
	gm.canvas.at(DeathHUD)->controls.at(7)->text = maxShieldString.c_str();
	gm.canvas.at(PauseHUD)->controls.at(10)->text = maxShieldString.c_str();

	std::string rangeString = std::to_string((int)range);
	gm.canvas.at(DeathHUD)->controls.at(8)->text = rangeString.c_str();
	gm.canvas.at(PauseHUD)->controls.at(11)->text = rangeString.c_str();

	std::string attackString = std::to_string(attack);
	gm.canvas.at(DeathHUD)->controls.at(9)->text = attackString.c_str();
	gm.canvas.at(PauseHUD)->controls.at(12)->text = attackString.c_str();

	std::string speedString = std::to_string((int)speed);
	gm.canvas.at(DeathHUD)->controls.at(10)->text = speedString.c_str();
	gm.canvas.at(PauseHUD)->controls.at(13)->text = speedString.c_str();

	std::string rateOfFireString = std::to_string((int)rateOfFire);
	gm.canvas.at(DeathHUD)->controls.at(11)->text = rateOfFireString.c_str();
	gm.canvas.at(PauseHUD)->controls.at(14)->text = rateOfFireString.c_str();

	std::vector<Text*> TextsForSwap;
	TextsForSwap.push_back(gm.InitFont("library/Fonts/Jade_Smile.ttf", maxHealthString.c_str()));
	TextsForSwap.push_back(gm.InitFont("library/Fonts/Jade_Smile.ttf", maxShieldString.c_str()));
	TextsForSwap.push_back(gm.InitFont("library/Fonts/Jade_Smile.ttf", rangeString.c_str()));
	TextsForSwap.push_back(gm.InitFont("library/Fonts/Jade_Smile.ttf", attackString.c_str()));
	TextsForSwap.push_back(gm.InitFont("library/Fonts/Jade_Smile.ttf", speedString.c_str()));
	TextsForSwap.push_back(gm.InitFont("library/Fonts/Jade_Smile.ttf", rateOfFireString.c_str()));

	//Updating Canvas Textures
	for (int i = 0; i < 6; i++)
	{
		int index_pause = 9 + i;
		int index_death = 6 + i;
		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(PauseHUD)->controls.at(index_pause)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(PauseHUD)->controls.at(index_pause)->id_quad_normal, TextsForSwap.at(i)->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(PauseHUD)->controls.at(index_pause)->id_quad_normal, TextsForSwap.at(i)->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(DeathHUD)->controls.at(index_death)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(DeathHUD)->controls.at(index_death)->id_quad_normal, TextsForSwap.at(i)->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(DeathHUD)->controls.at(index_death)->id_quad_normal, TextsForSwap.at(i)->GetSize(), { 0,0,512,512 });
	}
}

void Wiwa::PlayerGUISystem::ShopElementsHUD(Item* currentItem, Wiwa::GuiManager& gm)
{

	if (!currentItem)
		return;

	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

	const char* tags[] =
	{
		"Attack",
		"Projectile",
		"Aoe",
		"Debuff",
		"Homing",
	};
	if (currentItem->item_type == 0) //Ability
	{
		Ability* ability = Wiwa::ItemManager::GetAbility(currentItem->Name);

		std::string ability_name = ability->Name;
		gm.canvas.at(ShopHUD)->controls.at(0)->text = ability_name.c_str();

		std::string ability_price = std::to_string((int)ability->Price);
		gm.canvas.at(ShopHUD)->controls.at(1)->text = ability_price.c_str();
		//TODO: tags
		std::string ability_tag_1 = tags[(int)ability->itemTag[0]];
		gm.canvas.at(ShopHUD)->controls.at(3)->text = ability_tag_1.c_str();

		std::string ability_tag_2 = tags[(int)ability->itemTag[1]];
		gm.canvas.at(ShopHUD)->controls.at(4)->text = ability_tag_1.c_str();

		std::string ability_damage = std::to_string((int)ability->Damage);
		gm.canvas.at(ShopHUD)->controls.at(5)->text = ability_damage.c_str();

		std::string ability_Range = std::to_string((int)ability->Range);
		gm.canvas.at(ShopHUD)->controls.at(6)->text = ability_Range.c_str();

		std::string ability_CD = std::to_string((int)ability->Cooldown);
		gm.canvas.at(ShopHUD)->controls.at(7)->text = ability_CD.c_str();

		std::string ability_Area = std::to_string((int)ability->Area);
		gm.canvas.at(ShopHUD)->controls.at(8)->text = ability_Area.c_str();


		//TODO: Do description with wrappedText
		Text* abilityNameText = gm.InitFont("library/Fonts/Jade_Smile.ttf", ability_name.c_str());
		Text* abilityPriceText = gm.InitFont("library/Fonts/Jade_Smile.ttf", ability_price.c_str());
		Text* abilityTag1 = gm.InitFont("library/Fonts/Jade_Smile.ttf", ability_tag_1.c_str());
		Text* abilityTag2 = gm.InitFont("library/Fonts/Jade_Smile.ttf", ability_tag_2.c_str());
		Text* abilityDamageText = gm.InitFont("library/Fonts/Jade_Smile.ttf", ability_damage.c_str());
		Text* abilityRangeText = gm.InitFont("library/Fonts/Jade_Smile.ttf", ability_Range.c_str());
		Text* abilityCDText = gm.InitFont("library/Fonts/Jade_Smile.ttf", ability_CD.c_str());
		Text* AreaText = gm.InitFont("library/Fonts/Jade_Smile.ttf", ability_Area.c_str());
		//Text* description = gm.InitFont("library/Fonts/Jade_Smile.ttf", const_cast<char*>(ability->Description.c_str()));
		Text* description = gm.InitFontForDialog("library/Fonts/Jade_Smile.ttf", const_cast<char*>(ability->Description.c_str()), 950);

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(0)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(0)->id_quad_normal, abilityNameText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(0)->id_quad_normal, abilityNameText->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(1)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(1)->id_quad_normal, abilityPriceText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(1)->id_quad_normal, abilityPriceText->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(3)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(3)->id_quad_normal, abilityTag1->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(3)->id_quad_normal, abilityTag1->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(4)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(4)->id_quad_normal, abilityTag2->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(4)->id_quad_normal, abilityTag2->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(5)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(5)->id_quad_normal, abilityDamageText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(5)->id_quad_normal, abilityDamageText->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(6)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(6)->id_quad_normal, abilityRangeText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(6)->id_quad_normal, abilityRangeText->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(7)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(7)->id_quad_normal, abilityCDText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(7)->id_quad_normal, abilityCDText->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(8)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(8)->id_quad_normal, AreaText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(8)->id_quad_normal, AreaText->GetSize(), { 0,0,512,512 });

		GuiControl* control = gm.canvas.at(ShopHUD)->controls.at(9);
		control->position.x = 910;
		control->position.y = 640;
		control->position.width = 1250;
		control->position.height = 450;

		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(9)->id_quad_normal, description->GetTextureId());


	}
	else if (currentItem->item_type == 2) //Buff
	{
		Buff* buff = Wiwa::ItemManager::GetBuff(currentItem->Name);

		std::string buff_name = buff->Name;
		gm.canvas.at(ShopHUD)->controls.at(0)->text = buff_name.c_str();

		std::string buff_price = std::to_string((int)buff->Price);
		gm.canvas.at(ShopHUD)->controls.at(1)->text = buff_price.c_str();

		std::string buff_damage = std::to_string((int)buff->AttackDmgInc);
		gm.canvas.at(ShopHUD)->controls.at(5)->text = buff_damage.c_str();

		std::string buff_Range = std::to_string((int)buff->BuffPercent);
		gm.canvas.at(ShopHUD)->controls.at(6)->text = buff_Range.c_str();

		std::string buff_CD = std::to_string((int)buff->Cooldown);
		gm.canvas.at(ShopHUD)->controls.at(7)->text = buff_CD.c_str();

		std::string buff_Area = std::to_string((int)buff->RoFInc);
		gm.canvas.at(ShopHUD)->controls.at(8)->text = buff_Area.c_str();


		Text* buffNameText = gm.InitFont("library/Fonts/Jade_Smile.ttf", buff_name.c_str());
		Text* buffPriceText = gm.InitFont("library/Fonts/Jade_Smile.ttf", buff_price.c_str());
		Text* buffDamageText = gm.InitFont("library/Fonts/Jade_Smile.ttf", buff_damage.c_str());
		Text* buffRangeText = gm.InitFont("library/Fonts/Jade_Smile.ttf", buff_Range.c_str());
		Text* buffCDText = gm.InitFont("library/Fonts/Jade_Smile.ttf", buff_CD.c_str());
		Text* buffAreaText = gm.InitFont("library/Fonts/Jade_Smile.ttf", buff_Area.c_str());	
		Text* description = gm.InitFontForDialog("library/Fonts/Jade_Smile.ttf", const_cast<char*>(buff->Description.c_str()), 950);

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(0)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(0)->id_quad_normal, buffNameText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(0)->id_quad_normal, buffNameText->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(1)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(1)->id_quad_normal, buffPriceText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(1)->id_quad_normal, buffPriceText->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(5)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(5)->id_quad_normal, buffDamageText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(5)->id_quad_normal, buffDamageText->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(6)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(6)->id_quad_normal, buffRangeText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(6)->id_quad_normal, buffRangeText->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(7)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(7)->id_quad_normal, buffCDText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(7)->id_quad_normal, buffCDText->GetSize(), { 0,0,512,512 });

		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(ShopHUD)->controls.at(8)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(8)->id_quad_normal, buffAreaText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(ShopHUD)->controls.at(8)->id_quad_normal, buffAreaText->GetSize(), { 0,0,512,512 });

		GuiControl* control = gm.canvas.at(ShopHUD)->controls.at(9);
		control->position.x = 910;
		control->position.y = 640;
		control->position.width = 1250;
		control->position.height = 450;

		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(ShopHUD)->controls.at(9)->id_quad_normal, description->GetTextureId());
	}
	updateShopHUD = false;
}

void Wiwa::PlayerGUISystem::OnCollisionEnter(Object* body1, Object* body2)
{

	if (GameStateManager::GetRoomType() == "ROOM_SHOP")
	{
		std::string tag_item = "ITEM";
		if (body1->id == m_EntityId && tag_item == body2->selfTagStr)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();
			Item* item = em.GetComponent<Item>(body2->id);


			if (!item)
			{
				WI_CORE_ERROR("Item component can't be find");
				return;
			}
			if (item->item_type == 0 || item->item_type == 2)//ABILITY || BUFF
			{
				CharacterInventory* characterInventory = m_Scene->GetEntityManager().GetSystem<CharacterInventory>(m_EntityId);
				characterInventory->SetCurrentItem(item, body2->id);
				characterInventory->SetShopActive(true);
				updateShopHUD = true;
				EnableShopUI();
				return;
			}
		}
	}
}

void Wiwa::PlayerGUISystem::OnCollisionExit(Object* body1, Object* body2)
{
	std::string tag_item = "ITEM";
	if (GameStateManager::GetRoomType() == "ROOM_SHOP")
	{
		if (body1->id == m_EntityId && tag_item == body2->selfTagStr)
		{
			Wiwa::Scene* _scene = (Wiwa::Scene*)m_Scene;
			Wiwa::EntityManager& em = _scene->GetEntityManager();
			Item* item = em.GetComponent<Item>(body2->id);
			Wiwa::GuiManager& gm = m_Scene->GetGuiManager();

			if (!item)
			{
				WI_CORE_ERROR("Item component can't be find");
				return;
			}
			if (item->item_type == 0 || item->item_type == 2)//ABILITY//BUFF
			{
				CharacterInventory* characterInventory = m_Scene->GetEntityManager().GetSystem<CharacterInventory>(m_EntityId);
				characterInventory->SetCurrentItem(nullptr, -1);
				characterInventory->SetShopActive(false);
				DisableShopUI();
			}
		}
	}
}

void Wiwa::PlayerGUISystem::HandleWaves(Wiwa::GuiManager& gm)
{
	if (Wiwa::GameStateManager::GetRoomType() == "ROOM_BOSS" || Wiwa::GameStateManager::GetRoomType() == "ROOM_HUB")
		return;

		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
		std::string my_string = std::to_string(Wiwa::GameStateManager::GetCurrentWave());

		gm.canvas.at(CanvasHUD)->controls.at(18)->text = my_string.c_str();
		Text* newText = gm.InitFont("library/Fonts/Jade_Smile.ttf", my_string.c_str());
		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(CanvasHUD)->controls.at(18)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(CanvasHUD)->controls.at(18)->id_quad_normal, newText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(CanvasHUD)->controls.at(18)->id_quad_normal, newText->GetSize(), { 0,0,512,512 });
}

void Wiwa::PlayerGUISystem::EnableShopUI()
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	if (!gm.canvas.at(ShopHUD)->active)
	{
		gm.canvas.at(ShopHUD)->SwapActive();
		gm.canvas.at(CanvasHUD)->SwapActive();
	}

}

void Wiwa::PlayerGUISystem::DisableShopUI()
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	if (gm.canvas.at(ShopHUD)->active)
	{
		gm.canvas.at(ShopHUD)->SwapActive();
		gm.canvas.at(CanvasHUD)->SwapActive();
	}
}
