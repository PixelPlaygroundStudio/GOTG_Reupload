#include <wipch.h>

#include "DialogEventManager.h"
#include "DialogManager.h"

#include <Wiwa/ecs/components/game/Character.h>

#include "Wiwa/audio/Audio.h"

#include <Wiwa/core/Application.h>
//#include <Wiwa/scene/SceneManager.h>
#include <Wiwa/scene/Scene.h>

//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../vendor/stb/stb_image_write.h"

//#define STB_TRUETYPE_IMPLEMENTATION 
#include "../vendor/stb/stb_truetype.h"

#include <Wiwa/scene/SceneManager.h>

#include <Wiwa/utilities/json/JSONDocument.h>

#include <Wiwa/utilities/easings.h>
#include <Wiwa/game/Items/ItemManager.h>

namespace Wiwa
{
	DialogEventManager::DialogEventManager()
	{

	}

	DialogEventManager::~DialogEventManager()
	{
		/*Renderer2D& render = Wiwa::Application::Get().GetRenderer2D();



		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/Choice_menu/uatu_reward_menu_v1.dds"))
			render.RemoveInstance(m_Scene, choice1Opt1ImgID);

		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/Choice_menu/uatu_reward_menu_v1.dds"))
			render.RemoveInstance(m_Scene, choice1Opt2ImgID);


		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/Choice_menu/aron_reward_menu_v1.dds"))
			render.RemoveInstance(m_Scene, choice2Opt1ImgID);

		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/Choice_menu/aron_reward_menu_v1.dds"))
			render.RemoveInstance(m_Scene, choice2Opt2ImgID);

		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/Choice_menu/aron_reward_menu_v1.dds"))
			render.RemoveInstance(m_Scene, choice2Opt3ImgID);


		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/Choice_menu/ulana_reward_menu_v1.dds"))
			render.RemoveInstance(m_Scene, choice3Opt1ImgID);

		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/Choice_menu/ulana_reward_menu_v1.dds"))
			render.RemoveInstance(m_Scene, choice3Opt2ImgID);*/
	}

	bool DialogEventManager::Init(Scene* scene)
	{
		m_Scene = scene;

		Renderer2D& render = Wiwa::Application::Get().GetRenderer2D();

		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/Choice_menu/uatu_reward_menu_final.dds"))
		{
			ResourceId textID01 = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/Choice_menu/uatu_reward_menu_final.dds");
			Image* choice1Opt1Img = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID01);

			if (choice1Opt1Img)
			{
				choice1Opt1ImgID = render.CreateInstancedQuadTex(m_Scene, choice1Opt1Img->GetTextureId(), choice1Opt1Img->GetSize(), { 500,250 }, { 400,400 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				render.DisableInstance(m_Scene, choice1Opt1ImgID);
			}

			ResourceId textID02 = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/Choice_menu/uatu_reward_menu_final.dds");
			Image* choice1Opt2Img = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID02);

			if (choice1Opt2Img)
			{
				choice1Opt2ImgID = render.CreateInstancedQuadTex(m_Scene, choice1Opt2Img->GetTextureId(), choice1Opt2Img->GetSize(), { 1000,250 }, { 400,400 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				render.DisableInstance(m_Scene, choice1Opt2ImgID);
			}
		}
		else
		{
			WI_CRITICAL("uatu_reward_menu_final.dds Does not exist");
		}
		

		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/Choice_menu/aron_reward_menu_final.dds"))
		{
			ResourceId textID = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/Choice_menu/aron_reward_menu_final.dds");
			Image* choice2Opt1Img = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID);

			if (choice2Opt1Img)
			{
				choice2Opt1ImgID = render.CreateInstancedQuadTex(m_Scene, choice2Opt1Img->GetTextureId(), choice2Opt1Img->GetSize(), { 300,250 }, { 400,400 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				render.DisableInstance(m_Scene, choice2Opt1ImgID);
			}

			ResourceId textID2 = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/Choice_menu/aron_reward_menu_final.dds");
			Image* choice2Opt2Img = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID2);

			if (choice2Opt2Img)
			{
				choice2Opt2ImgID = render.CreateInstancedQuadTex(m_Scene, choice2Opt2Img->GetTextureId(), choice2Opt2Img->GetSize(), { 800,250 }, { 400,400 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				render.DisableInstance(m_Scene, choice2Opt2ImgID);
			}

			ResourceId textID3 = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/Choice_menu/aron_reward_menu_final.dds");
			Image* choice2Opt3Img = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID3);

			if (choice2Opt3Img)
			{
				choice2Opt3ImgID = render.CreateInstancedQuadTex(m_Scene, choice2Opt3Img->GetTextureId(), choice2Opt3Img->GetSize(), { 1300,250 }, { 400,400 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				render.DisableInstance(m_Scene, choice2Opt3ImgID);
			}
		}
		else
		{
			WI_CRITICAL("aron_reward_menu_final.dds Does not exist");
		}

		
		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/Choice_menu/ulana_reward_menu_final.dds"))
		{
			ResourceId textID11 = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/Choice_menu/ulana_reward_menu_final.dds");
			Image* choice3Opt1Img = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID11);

			if (choice3Opt1Img)
			{
				choice3Opt1ImgID = render.CreateInstancedQuadTex(m_Scene, choice3Opt1Img->GetTextureId(), choice3Opt1Img->GetSize(), { 500,250 }, { 400,400 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				render.DisableInstance(m_Scene, choice3Opt1ImgID);
			}

			ResourceId textID12 = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/Choice_menu/ulana_reward_menu_final.dds");
			Image* choice3Opt2Img = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID12);

			if (choice3Opt2Img)
			{
				choice3Opt2ImgID = render.CreateInstancedQuadTex(m_Scene, choice3Opt2Img->GetTextureId(), choice3Opt2Img->GetSize(), { 1000,250 }, { 400,400 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				render.DisableInstance(m_Scene, choice3Opt2ImgID);
			}
		}
		else
		{
			WI_CRITICAL("ulana_reward_menu_final.dds Does not exist");
		}


		eventStarted = false;
		eventFinished = false;
		selected = false;
		selector = 0;
		eventState = 2;
		keyPressTimer = 0;

		endTimer1 = 0;
		endTimer2 = 0;
		endTimer3 = 0;

		startTimer1 = 0;
		startTimer2 = 0;
		startTimer3 = 0;

		return true;
	}

	bool DialogEventManager::Update()
	{
		Renderer2D& render = Wiwa::Application::Get().GetRenderer2D();

		//DialogManager* dm = Wiwa::Application::Get();
		Wiwa::Scene * _scene = (Wiwa::Scene*)m_Scene;

		Wiwa::GuiManager& ui = m_Scene->GetGuiManager(); // 0 == se dibuja; 0 != no se dibuja

		if (_scene->GetDialogManager().triggerEvent == true && eventState == 2)
		{
			startTimer1 = 0;
			startTimer2 = 0;
			startTimer3 = 0;

			endTimer1 = 0;
			endTimer2 = 0;
			endTimer3 = 0;

			eventState = 0;
			selected = false;
		}

		if (eventState != 2 && ui.getCurrentCanvas() == 0)
		{

			if (!strcmp(_scene->GetDialogManager().dialogEventToTrigger.c_str(), "Choice_Uatu"))
			{
				if (eventState == 0)
				{
					render.EnableInstance(m_Scene, choice1Opt1ImgID);
					render.EnableInstance(m_Scene, choice1Opt2ImgID);

					ImgPos1.x = 500;
					ImgPos1.y = 1100;
					ImgPos2.x = 1000;
					ImgPos2.y = 1100;
					eventStarted = true;

					eventState = 1;
				}

				if (eventStarted == true)
				{
					startTimer1 += Time::GetDeltaTime();

					/*
						Example:

						currentPositionX = EaseSineIn(currentTime, startPositionX, finalPositionX - startPositionX, duration);
						currentTime++;
					*/
					if (startTimer1 <= 450)
					{
						ImgPos1.y = EaseBackOut(startTimer1, 1100, 250 - 1100, 450);
					}
					if (startTimer1 <= 550 && startTimer1 > 100)
					{
						ImgPos2.y = EaseBackOut(startTimer2, 1100, 250 - 1100, 450);
						startTimer2 += Time::GetDeltaTime();
					}

					render.UpdateInstancedQuadTexPosition(m_Scene, choice1Opt1ImgID, ImgPos1, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.UpdateInstancedQuadTexPosition(m_Scene, choice1Opt2ImgID, ImgPos2, Wiwa::Renderer2D::Pivot::UPLEFT);

					if (startTimer1 > 550)
					{
						eventStarted = false;
					}

				}

				if (selector == 0)
				{
					if (selected == false) render.UpdateInstancedQuadTexClip(m_Scene, choice1Opt1ImgID, { 4500, 4500 }, { 1500, 0, 1500, 1500 });
					render.UpdateInstancedQuadTexClip(m_Scene, choice1Opt2ImgID, { 4500, 4500 }, { 0, 1500, 1500, 1500 });
				}
				else if (selector == 1)
				{
					render.UpdateInstancedQuadTexClip(m_Scene, choice1Opt1ImgID, { 4500, 4500 }, { 0, 0, 1500, 1500 });
					if (selected == false) render.UpdateInstancedQuadTexClip(m_Scene, choice1Opt2ImgID, { 4500, 4500 }, { 1500, 1500, 1500, 1500 });
				}

				if ((Wiwa::Input::IsKeyPressed(Wiwa::Key::Space) || Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadDPadRight) ||
					Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).x > 0.5f) && keyPressTimer > 250)
				{
					keyPressTimer = 0;

					selector++;

					if (selector >= 2) selector = 0;
				}

				if ((Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadDPadLeft) ||
					Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).x < -0.5f) && keyPressTimer > 250)
				{
					keyPressTimer = 0;

					selector--;

					if (selector < 0) selector = 1;
				}

				if ((Wiwa::Input::IsKeyPressed(Wiwa::Key::Enter) || Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA)) && keyPressTimer > 650)
				{
					selected = true;
					keyPressTimer = 0;

					if (selector == 0)
					{
						render.UpdateInstancedQuadTexClip(m_Scene, choice1Opt1ImgID, { 4500, 4500 }, { 3000, 0, 1500, 1500 });
						UatuReward(1);
					}
					else if (selector == 1)
					{
						render.UpdateInstancedQuadTexClip(m_Scene, choice1Opt2ImgID, { 4500, 4500 }, { 3000, 1500, 1500, 1500 });
						UatuReward(2);
					}

					eventFinished = true;
				}

				if (eventFinished == true)
				{
					endTimer1 += Time::GetDeltaTime();

					/*
						Example:

						currentPositionX = EaseSineIn(currentTime, startPositionX, finalPositionX - startPositionX, duration);
						currentTime++;
					*/
					if (endTimer1 <= 450)
					{
						ImgPos1.y = EaseBackIn(endTimer1, 250, 1100 - 250, 450);
					}
					if (endTimer1 <= 550 && endTimer1 > 100)
					{
						ImgPos2.y = EaseBackIn(endTimer2, 250, 1100 - 250, 450);
						endTimer2 += Time::GetDeltaTime();
					}

					render.UpdateInstancedQuadTexPosition(m_Scene, choice1Opt1ImgID, ImgPos1, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.UpdateInstancedQuadTexPosition(m_Scene, choice1Opt2ImgID, ImgPos2, Wiwa::Renderer2D::Pivot::UPLEFT);

					if (endTimer1 > 550)
					{
						ImgPos1.x = 500;
						ImgPos1.y = 1100;
						ImgPos2.x = 1000;
						ImgPos2.y = 1100;

						render.DisableInstance(m_Scene, choice1Opt1ImgID);
						render.DisableInstance(m_Scene, choice1Opt2ImgID);

						eventState = 2;

						eventFinished = false;
					}
				}
			}
			else if (!strcmp(_scene->GetDialogManager().dialogEventToTrigger.c_str(), "Choice_Aron"))
			{
				if (eventState == 0)
				{
					render.EnableInstance(m_Scene, choice2Opt1ImgID);
					render.EnableInstance(m_Scene, choice2Opt2ImgID);
					render.EnableInstance(m_Scene, choice2Opt3ImgID);

					ImgPos1.x = 300;
					ImgPos1.y = 1100;
					ImgPos2.x = 800;
					ImgPos2.y = 1100;
					ImgPos3.x = 1300;
					ImgPos3.y = 1100;
					eventStarted = true;

					eventState = 1;
				}

				if (eventStarted == true)
				{
					startTimer1 += Time::GetDeltaTime();

					/*
						Example:

						currentPositionX = EaseSineIn(currentTime, startPositionX, finalPositionX - startPositionX, duration);
						currentTime++;
					*/
					if (startTimer1 <= 450)
					{
						ImgPos1.y = EaseBackOut(startTimer1, 1100, 250 - 1100, 450);
					}
					if (startTimer1 <= 550 && startTimer1 > 100)
					{
						ImgPos2.y = EaseBackOut(startTimer2, 1100, 250 - 1100, 450);
						startTimer2 += Time::GetDeltaTime();
					}
					if (startTimer1 <= 650 && startTimer1 > 200)
					{
						ImgPos3.y = EaseBackOut(startTimer3, 1100, 250 - 1100, 450);
						startTimer3 += Time::GetDeltaTime();
					}

					render.UpdateInstancedQuadTexPosition(m_Scene, choice2Opt1ImgID, ImgPos1, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.UpdateInstancedQuadTexPosition(m_Scene, choice2Opt2ImgID, ImgPos2, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.UpdateInstancedQuadTexPosition(m_Scene, choice2Opt3ImgID, ImgPos3, Wiwa::Renderer2D::Pivot::UPLEFT);

					if (startTimer1 > 650)
					{
						eventStarted = false;
					}
					
				}

				if (selector == 0)
				{
					if(selected == false) render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt1ImgID, { 4500, 4500 }, { 1500, 0, 1500, 1500 });
					render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt2ImgID, { 4500, 4500 }, { 0, 1500, 1500, 1500 });
					render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt3ImgID, { 4500, 4500 }, { 0, 3000, 1500, 1500 });
				}
				else if (selector == 1)
				{
					render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt1ImgID, { 4500, 4500 }, { 0, 0, 1500, 1500 });
					if (selected == false) render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt2ImgID, { 4500, 4500 }, { 1500, 1500, 1500, 1500 });
					render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt3ImgID, { 4500, 4500 }, { 0, 3000, 1500, 1500 });
				}
				else if (selector == 2)
				{
					render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt1ImgID, { 4500, 4500 }, { 0, 0, 1500, 1500 });
					render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt2ImgID, { 4500, 4500 }, { 0, 1500, 1500, 1500 });
					if (selected == false) render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt3ImgID, { 4500, 4500 }, { 1500, 3000, 1500, 1500 });
				}

				if ((Wiwa::Input::IsKeyPressed(Wiwa::Key::Space) || Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadDPadRight) ||
					Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).x > 0.5f) && keyPressTimer > 250)
				{
					keyPressTimer = 0;

					selector++;

					if (selector >= 3) selector = 0;
				}

				if ((Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadDPadLeft) ||
					Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).x < -0.5f) && keyPressTimer > 250)
				{
					keyPressTimer = 0;

					selector--;

					if (selector < 0) selector = 2;
				}

				if ((Wiwa::Input::IsKeyPressed(Wiwa::Key::Enter) || Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA)) && keyPressTimer > 650)
				{
					selected = true;
					keyPressTimer = 0;

					if (selector == 0)
					{
						render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt1ImgID, { 4500, 4500 }, { 3000, 0, 1500, 1500 });
						AronReward(1);
					}
					else if (selector == 1)
					{
						render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt2ImgID, { 4500, 4500 }, { 3000, 1500, 1500, 1500 });
						AronReward(2);
					}
					else if (selector == 2)
					{
						render.UpdateInstancedQuadTexClip(m_Scene, choice2Opt3ImgID, { 4500, 4500 }, { 3000, 3000, 1500, 1500 });
						AronReward(3);
					}

					eventFinished = true;
				}

				if (eventFinished == true)
				{
					endTimer1 += Time::GetDeltaTime();

					/*
						Example:

						currentPositionX = EaseSineIn(currentTime, startPositionX, finalPositionX - startPositionX, duration);
						currentTime++;
					*/
					if (endTimer1 <= 450)
					{
						ImgPos1.y = EaseBackIn(endTimer1, 250, 1100 - 250, 450);
					}
					if (endTimer1 <= 550 && endTimer1 > 100)
					{
						ImgPos2.y = EaseBackIn(endTimer2, 250, 1100 - 250, 450);
						endTimer2 += Time::GetDeltaTime();
					}
					if (endTimer1 <= 650 && endTimer1 > 200)
					{
						ImgPos3.y = EaseBackIn(endTimer3, 250, 1100 - 250, 450);
						endTimer3 += Time::GetDeltaTime();
					}

					render.UpdateInstancedQuadTexPosition(m_Scene, choice2Opt1ImgID, ImgPos1, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.UpdateInstancedQuadTexPosition(m_Scene, choice2Opt2ImgID, ImgPos2, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.UpdateInstancedQuadTexPosition(m_Scene, choice2Opt3ImgID, ImgPos3, Wiwa::Renderer2D::Pivot::UPLEFT);

					if (endTimer1 > 650)
					{
						ImgPos1.x = 300;
						ImgPos1.y = 1100;
						ImgPos2.x = 800;
						ImgPos2.y = 1100;
						ImgPos3.x = 1300;
						ImgPos3.y = 1100;

						render.DisableInstance(m_Scene, choice2Opt1ImgID);
						render.DisableInstance(m_Scene, choice2Opt2ImgID);
						render.DisableInstance(m_Scene, choice2Opt3ImgID);

						eventState = 2;

						eventFinished = false;
					}
				}
			}
			else if (!strcmp(_scene->GetDialogManager().dialogEventToTrigger.c_str(), "Choice_Ulana"))
			{
				if (eventState == 0)
				{
					render.EnableInstance(m_Scene, choice3Opt1ImgID);
					render.EnableInstance(m_Scene, choice3Opt2ImgID);

					ImgPos1.x = 500;
					ImgPos1.y = 1100;
					ImgPos2.x = 1000;
					ImgPos2.y = 1100;
					eventStarted = true;

					eventState = 1;
				}

				if (eventStarted == true)
				{
					startTimer1 += Time::GetDeltaTime();

					/*
						Example:

						currentPositionX = EaseSineIn(currentTime, startPositionX, finalPositionX - startPositionX, duration);
						currentTime++;
					*/
					if (startTimer1 <= 450)
					{
						ImgPos1.y = EaseBackOut(startTimer1, 1100, 250 - 1100, 450);
					}
					if (startTimer1 <= 550 && startTimer1 > 100)
					{
						ImgPos2.y = EaseBackOut(startTimer2, 1100, 250 - 1100, 450);
						startTimer2 += Time::GetDeltaTime();
					}

					render.UpdateInstancedQuadTexPosition(m_Scene, choice3Opt1ImgID, ImgPos1, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.UpdateInstancedQuadTexPosition(m_Scene, choice3Opt2ImgID, ImgPos2, Wiwa::Renderer2D::Pivot::UPLEFT);

					if (startTimer1 > 550)
					{
						eventStarted = false;
					}

				}

				if (selector == 0)
				{
					if (selected == false) render.UpdateInstancedQuadTexClip(m_Scene, choice3Opt1ImgID, { 4500, 4500 }, { 1500, 0, 1500, 1500 });
					render.UpdateInstancedQuadTexClip(m_Scene, choice3Opt2ImgID, { 4500, 4500 }, { 0, 1500, 1500, 1500 });
				}
				else if (selector == 1)
				{
					render.UpdateInstancedQuadTexClip(m_Scene, choice3Opt1ImgID, { 4500, 4500 }, { 0, 0, 1500, 1500 });
					if (selected == false) render.UpdateInstancedQuadTexClip(m_Scene, choice3Opt2ImgID, { 4500, 4500 }, { 1500, 1500, 1500, 1500 });
				}

				if ((Wiwa::Input::IsKeyPressed(Wiwa::Key::Space) || Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadDPadRight) ||
					Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).x > 0.5f) && keyPressTimer > 250)
				{
					keyPressTimer = 0;

					selector++;

					if (selector >= 2) selector = 0;
				}

				if ((Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadDPadLeft) ||
					Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).x < -0.5f) && keyPressTimer > 250)
				{
					keyPressTimer = 0;

					selector--;

					if (selector < 0) selector = 1;
				}

				if ((Wiwa::Input::IsKeyPressed(Wiwa::Key::Enter) || Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA)) && keyPressTimer > 650)
				{
					selected = true;
					keyPressTimer = 0;

					if (selector == 0)
					{
						render.UpdateInstancedQuadTexClip(m_Scene, choice3Opt1ImgID, { 4500, 4500 }, { 3000, 0, 1500, 1500 });
						UlanaReward(1);
					}
					else if (selector == 1)
					{
						render.UpdateInstancedQuadTexClip(m_Scene, choice3Opt2ImgID, { 4500, 4500 }, { 3000, 1500, 1500, 1500 });
						UlanaReward(2);
					}

					eventFinished = true;
				}

				if (eventFinished == true)
				{
					endTimer1 += Time::GetDeltaTime();

					/*
						Example:

						currentPositionX = EaseSineIn(currentTime, startPositionX, finalPositionX - startPositionX, duration);
						currentTime++;
					*/
					if (endTimer1 <= 450)
					{
						ImgPos1.y = EaseBackIn(endTimer1, 250, 1100 - 250, 450);
					}
					if (endTimer1 <= 550 && endTimer1 > 100)
					{
						ImgPos2.y = EaseBackIn(endTimer2, 250, 1100 - 250, 450);
						endTimer2 += Time::GetDeltaTime();
					}

					render.UpdateInstancedQuadTexPosition(m_Scene, choice3Opt1ImgID, ImgPos1, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.UpdateInstancedQuadTexPosition(m_Scene, choice3Opt2ImgID, ImgPos2, Wiwa::Renderer2D::Pivot::UPLEFT);

					if (endTimer1 > 550)
					{
						ImgPos1.x = 500;
						ImgPos1.y = 1100;
						ImgPos2.x = 1000;
						ImgPos2.y = 1100;

						render.DisableInstance(m_Scene, choice3Opt1ImgID);
						render.DisableInstance(m_Scene, choice3Opt2ImgID);

						eventState = 2;

						eventFinished = false;
					}
				}
			}
			else
			{
				eventState = 2;
			}
		}

		if (eventState == 2)
		{
			_scene->GetDialogManager().triggerEvent = false;
		}

		keyPressTimer += Time::GetDeltaTime();

		return true;
	}

	bool DialogEventManager::Draw()
	{


		return false;
	}

	void DialogEventManager::OnDestroy()
	{

	}

	bool DialogEventManager::CleanUp()
	{


		return true;
	}

	void DialogEventManager::UatuReward(int optionNumber)
	{
		Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();

		if (optionNumber == 1) // 1000 credits
		{
			inventory.AddTokens(1000);
		}
		else if (optionNumber == 2) // 100 prometheum
		{
			inventory.AddTokensHoward(100);
		}
	}

	void DialogEventManager::AronReward(int optionNumber)
	{
		Wiwa::Inventory& inventory = Wiwa::GameStateManager::GetPlayerInventory();

		if (optionNumber == 1) // Random active
		{
			int randomizerMaxValue = 3;
			int randomizerResult = 0;

			std::random_device rdev;
			std::mt19937 rgen(rdev());
			std::uniform_int_distribution<int> idist(0, randomizerMaxValue);
			randomizerResult = idist(rgen);

			if (randomizerResult == 0)
			{
				inventory.AddAbility(Wiwa::ItemManager::GetAbility("Groot's Seeds"));
			}
			else if (randomizerResult == 1)
			{
				inventory.AddAbility(Wiwa::ItemManager::GetAbility("Phyla's Quantum Sword"));
			}
			else if (randomizerResult == 2)
			{
				inventory.AddAbility(Wiwa::ItemManager::GetAbility("Starhawk's Time Blast"));
			}
			else if (randomizerResult == 3)
			{
				inventory.AddAbility(Wiwa::ItemManager::GetAbility("Yondu's Fin"));
			}
		}
		else if (optionNumber == 2) // Random buff
		{
			int randomizerMaxValue = 5;
			int randomizerResult = 0;

			std::random_device rdev;
			std::mt19937 rgen(rdev());
			std::uniform_int_distribution<int> idist(0, randomizerMaxValue);
			randomizerResult = idist(rgen);

			if (randomizerResult == 0)
			{
				inventory.AddBuff(Wiwa::ItemManager::GetBuff("Bug's Leg"));
			}
			else if (randomizerResult == 1)
			{
				inventory.AddBuff(Wiwa::ItemManager::GetBuff("Charlie-27's Fist"));
			}
			else if (randomizerResult == 2)
			{
				inventory.AddBuff(Wiwa::ItemManager::GetBuff("Cosmo's Paw"));
			}
			else if (randomizerResult == 3)
			{
				inventory.AddBuff(Wiwa::ItemManager::GetBuff("Major Victory's Shield"));
			}
			else if (randomizerResult == 4)
			{
				inventory.AddBuff(Wiwa::ItemManager::GetBuff("Martinex's Thermokinesis"));
			}
			else if (randomizerResult == 5)
			{
				inventory.AddBuff(Wiwa::ItemManager::GetBuff("Nikki's Touch"));
			}
		}
		else if (optionNumber == 3) // Random passive
		{
			int randomizerMaxValue = 9;
			int randomizerResult = 0;

			std::random_device rdev;
			std::mt19937 rgen(rdev());
			std::uniform_int_distribution<int> idist(0, randomizerMaxValue);
			randomizerResult = idist(rgen);

			if (randomizerResult == 0)
			{
				inventory.AddPassive(*Wiwa::ItemManager::GetPassive("Adam_Warlock_Blessing"));
			}
			else if (randomizerResult == 1)
			{
				inventory.AddPassive(*Wiwa::ItemManager::GetPassive("Angela_Crown"));
			}
			else if (randomizerResult == 2)
			{
				inventory.AddPassive(*Wiwa::ItemManager::GetPassive("Drax_Belt"));
			}
			else if (randomizerResult == 3)
			{
				inventory.AddPassive(*Wiwa::ItemManager::GetPassive("Gamora_Hood"));
			}
			else if (randomizerResult == 4)
			{
				inventory.AddPassive(*Wiwa::ItemManager::GetPassive("Ikons_Battery"));
			}
			else if (randomizerResult == 5)
			{
				inventory.AddPassive(*Wiwa::ItemManager::GetPassive("Iron_Man_Insurance"));
			}
			else if (randomizerResult == 6)
			{
				inventory.AddPassive(*Wiwa::ItemManager::GetPassive("Jack_Flag_Gloves"));
			}
			else if (randomizerResult == 7)
			{
				inventory.AddPassive(*Wiwa::ItemManager::GetPassive("Nova_Helmet"));
			}
			else if (randomizerResult == 8)
			{
				inventory.AddPassive(*Wiwa::ItemManager::GetPassive("Star_Lord_Walkman"));
			}
			else if (randomizerResult == 9)
			{
				inventory.AddPassive(*Wiwa::ItemManager::GetPassive("Xandarian_Worldmind"));
			}
		}
	}

	void DialogEventManager::UlanaReward(int optionNumber)
	{
		Character* character = GameStateManager::GetPlayerCharacterComp();

		if (optionNumber == 1) // Full HP
		{
			character->Health = character->MaxHealth;
		}
		else if (optionNumber == 2) // Full Ult-Shield
		{
			character->Shield = character->MaxShield;
		}
	}
}