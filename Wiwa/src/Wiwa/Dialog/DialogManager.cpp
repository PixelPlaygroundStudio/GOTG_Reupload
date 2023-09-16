#include <wipch.h>

#include "DialogManager.h"
#include "DialogEventManager.h"

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



namespace Wiwa
{
	DialogManager::DialogManager()
	{
		
	}

	DialogManager::~DialogManager()
	{
		/*Renderer2D& render = Wiwa::Application::Get().GetRenderer2D();


		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/UI_SpeechTriangle_01.dds"))
			render.RemoveInstance(m_Scene, continueImgID);
		
		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/UI_SpeechTriangle_02.dds"))
			render.RemoveInstance(m_Scene, continueImgID2);

		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/UI_TalkButton01.dds"))
			render.RemoveInstance(m_Scene, talkIndicatorImgID);

		if (totalLoadNum >= 3)
		{
			for (int i = 0; (i < MAX_CONVERSATIONS) && conversations[i].occupied == true; i++)
			{
				render.RemoveInstance(m_Scene, conversations[i].characterImgID);
				render.RemoveInstance(m_Scene, conversations[i].dialogImgID);

				for (int j = 0; (j < MAX_CONVERSATION_NODES) && conversations[i].nodes[j].occupied == true; j++)
				{
					render.RemoveInstance(m_Scene, conversations[i].nodes[j].text1_imgModeID);
				}
			}
		}

		for (int l = 0; l < MAX_CONVERSATIONS; l++)
		{
			conversations[l].occupied = false;

			for (int m = 0; m < MAX_CONVERSATION_NODES; m++)
			{
				conversations[l].nodes[m].occupied = false;
			}
		}*/
	}

	bool DialogManager::Init(Scene* scene)
	{
		m_Scene = scene;

		Renderer2D& render = Wiwa::Application::Get().GetRenderer2D();

		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/UI_ButtonA_01.dds"))
		{
			ResourceId textID = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/UI_ButtonA_01.dds");
			Image* continueImg = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID);

			if (continueImg)
			{
				continueImgID = render.CreateInstancedQuadTex(m_Scene, continueImg->GetTextureId(), continueImg->GetSize(), { 1500,870 }, { 256,256 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				render.DisableInstance(m_Scene, continueImgID);
			}
		}
		else
		{
			WI_CRITICAL("UI_ButtonA_01.dds Does not exist");
		}
		
		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/UI_ButtonA_02.dds"))
		{
			ResourceId textID2 = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/UI_ButtonA_02.dds");
			Image* continueImg2 = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID2);

			if (continueImg2)
			{
				continueImgID2 = render.CreateInstancedQuadTex(m_Scene, continueImg2->GetTextureId(), continueImg2->GetSize(), { 1500,870 }, { 256,256 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				render.DisableInstance(m_Scene, continueImgID2);
			}
		}
		else
		{
			WI_CRITICAL("UI_ButtonA_02.dds Does not exist");
		}
		
		if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/UI_TalkButton01.dds"))
		{
			ResourceId textID3 = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/UI_TalkButton01.dds");
			Image* talkIndicatorImg = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID3);

			if (talkIndicatorImg)
			{
				talkIndicatorImgID = render.CreateInstancedQuadTex(m_Scene, talkIndicatorImg->GetTextureId(), talkIndicatorImg->GetSize(), { 800,750 }, { 256,256 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				render.DisableInstance(m_Scene, talkIndicatorImgID);
			}
		}
		else
		{
			WI_CRITICAL("UI_TalkButton01.dds Does not exist");
		}

		

		characterID = GameStateManager::s_CurrentCharacter;

		//conversations[0].conversationName = "NPC_1";

		//SetDialogText("I am not to interfere, Guardian.", "However, I will tell you this: ", "the Phalanx are a formidable species,", "library/Fonts/Jade_Smile.ttf", 0, 0);
		//SetDialogText("constantly updating their data banks.", "With them the architecture, too, gets", "updated. ", "library/Fonts/Jade_Smile.ttf", 0, 1);
		//SetDialogText("Surprisingly, some of the collector’s", "inventory of weaponsandgadgets", "has been seen in the vicinity.", "library/Fonts/Jade_Smile.ttf", 0, 2);
		//SetDialogText("Do not get lost in the labyrinth of", "machinery, Guardian.", "And good luck.", "library/Fonts/Jade_Smile.ttf", 0, 3);

		//SetDialogBubbleImage("library/HUD_Images/menus/speech menu/ui_speech_menu_starlord_bubble-01.dds", 0);
		//SetCharacterImage("library/HUD_Images/menus/speech menu/ui_speech_menu_starlord_withshadows-01.dds", 0);

		//conversations[0].occupied = true;

		totalLoadNum = 0;

		if (std::filesystem::exists("library"))
		{
			if (std::filesystem::exists("library/fonts/Jade_Smile.ttf"))
			{
				LoadAllDialogs();
			}
		}

		actualConversationState = 2;
		currentNode = 0;

		keyPressRefreshTimer = 0;

		convGroup = -1;
		convOrder = -1;

		collidingWithNpc = false;

		characterImgPos.x = -50;
		characterImgPos.y = 100; // <--
		bubbleImgPos.x = 640; // <--
		bubbleImgPos.y = 300;

		forcedDialogHappened = false;
		alreadyTalkedOnceTriggered = false;
		canNoLongerTalk = false;

		for (int e = 0; e < MAX_CONVERSATIONS && conversations[e].occupied == true; e++)
		{
			editorConversations[e].bubbleImagePath = conversations[e].bubbleImagePath;
			editorConversations[e].characterImagePath = conversations[e].characterImagePath;
			editorConversations[e].conversationName = conversations[e].conversationName;
			editorConversations[e].occupied = conversations[e].occupied;

			editorConversations[e].isInOppositeSide = conversations[e].isInOppositeSide;
			editorConversations[e].isRandom = conversations[e].isRandom;
			editorConversations[e].detectsCharacter = conversations[e].detectsCharacter;
			editorConversations[e].group.groupID = conversations[e].group.groupID;
			editorConversations[e].group.order = conversations[e].group.order;
			editorConversations[e].eventName = conversations[e].eventName;

			for (int f = 0; f < MAX_CONVERSATION_NODES && conversations[e].nodes[f].occupied == true; f++)
			{
				editorConversations[e].nodes[f].text1 = conversations[e].nodes[f].text1;

				editorConversations[e].nodes[f].audioEventName = conversations[e].nodes[f].audioEventName;

				editorConversations[e].nodes[f].occupied = conversations[e].nodes[f].occupied;

			}


		}

		return true;
	}


	bool DialogManager::Update()
	{
		Renderer2D& render = Wiwa::Application::Get().GetRenderer2D();

		Wiwa::GuiManager& ui = m_Scene->GetGuiManager(); // 0 == se dibuja; 0 != no se dibuja

		characterID = GameStateManager::s_CurrentCharacter;

		Character* character = GameStateManager::GetPlayerCharacterComp();

		if (actualConversationState == 1 || triggerEvent == true)
		{
			character->CanMove = false;
		}
		else if (actualConversationState != 1 && triggerEvent == false)
		{
			character->CanMove = true;
		}

		if (((((Wiwa::Input::IsKeyPressed(Wiwa::Key::Space) || Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA)) && actualConversationState != 1 && keyPressRefreshTimer > 120 && collidingWithNpc == true)
			|| (forceStartConversation == true && forcedDialogHappened == false)) && triggerEvent == false && canNoLongerTalk == false) && ui.getCurrentCanvas() == 0)
		{
			if (collidingWithNpc == true)
			{
				conversationToPlayName = NpcConversationTag.c_str();

				if (characterSensitive == true)
				{
					if (characterID == 0)
					{
						conversationToPlayName = conversationToPlayName + "_Starlord";
					}
					else if (characterID == 1)
					{
						conversationToPlayName = conversationToPlayName + "_Rocket";
					}
				}
			}
			else if (forceStartConversation == true)
			{
				conversationToPlayName = forcedConversationTag.c_str();
				forcedDialogHappened = true;

				if (characterSensitive == true)
				{
					if (characterID == 0)
					{
						conversationToPlayName = conversationToPlayName + "_Starlord";
					}
					else if (characterID == 1)
					{
						conversationToPlayName = conversationToPlayName + "_Rocket";
					}
				}
			}
			actualConversationState = 0;

			keyPressRefreshTimer = 0;
			finishedRandomizing = false;
		}

		if (collidingWithNpc == true && actualConversationState == 2 && talkIndicatorImgEnabled == false && triggerEvent == false && canNoLongerTalk == false && ui.getCurrentCanvas() == 0)
		{
			render.EnableInstance(m_Scene, talkIndicatorImgID);

			talkIndicatorImgEnabled = true;
		}
		else if ((actualConversationState != 2 && talkIndicatorImgEnabled == true) || collidingWithNpc == false || ui.getCurrentCanvas() != 0)
		{
			render.DisableInstance(m_Scene, talkIndicatorImgID);

			talkIndicatorImgEnabled = false;
		}

		if (actualConversationState == 0 || actualConversationState == 1)
		{
			for (int i = 0; (i < MAX_CONVERSATIONS) && conversations[i].occupied == true; i++)
			{
				if (!strcmp(conversations[i].conversationName.c_str(), conversationToPlayName.c_str()) && conversations[i].isRandom == false)
				{
					if (ui.getCurrentCanvas() == 0) UpdateConversation(i, &Wiwa::Application::Get().GetRenderer2D());
					forceStartConversation = false;

					if (std::stoi(conversations[i].group.groupID) != -1 && actualConversationState == 2)
					{
						convGroup = std::stoi(conversations[i].group.groupID);
						convOrder = std::stoi(conversations[i].group.order) + 1;
					}
					else if (std::stoi(conversations[i].group.groupID) == -1 && actualConversationState == 2)
					{
						convGroup = -1;
						convOrder = -1;
					}
				}
				else if (!strcmp(conversations[i].conversationName.c_str(), conversationToPlayName.c_str()) && conversations[i].isRandom == true && std::stoi(conversations[i].group.groupID) != -1)
				{
					if (finishedRandomizing == false)
					{
						convGroup = std::stoi(conversations[i].group.groupID);
						int randomizerMaxValue = 0;
						int randomizerResult = 0;

						for (int y = 0; (y < MAX_CONVERSATIONS) && conversations[y].occupied == true; y++)
						{
							if (convGroup == std::stoi(conversations[y].group.groupID))
							{
								randomizerMaxValue++;
							}
						}

						std::random_device rdev;
						std::mt19937 rgen(rdev());
						std::uniform_int_distribution<int> idist(0, randomizerMaxValue);
						randomizerResult = idist(rgen);

						for (int x = 0; (x < MAX_CONVERSATIONS) && conversations[x].occupied == true; x++)
						{
							if (convGroup == std::stoi(conversations[x].group.groupID) && randomizerResult == std::stoi(conversations[x].group.order))
							{
								conversationToPlayName = conversations[x].conversationName;
								actualConversationState = 0;
							}
						}
					}

					for (int j = 0; (j < MAX_CONVERSATIONS) && conversations[j].occupied == true; j++)
					{
						if (!strcmp(conversations[j].conversationName.c_str(), conversationToPlayName.c_str()))
						{
							if(ui.getCurrentCanvas() == 0) UpdateConversation(j, &Wiwa::Application::Get().GetRenderer2D());
						}
					}

					forceStartConversation = false;
					finishedRandomizing = true;

					convGroup = -1;
					convOrder = -1;
				}
			}
		}

		if (convGroup != -1 && convOrder != -1 && actualConversationState == 2)
		{
			for (int z = 0; (z < MAX_CONVERSATIONS) && conversations[z].occupied == true; z++)
			{
				if (convGroup == std::stoi(conversations[z].group.groupID) && convOrder == std::stoi(conversations[z].group.order))
				{
					conversationToPlayName = conversations[z].conversationName;
					actualConversationState = 0;
				}
			}
		}

		if (alreadyTalkedOnceTriggered == true)
		{
			canNoLongerTalk = true;
		}

		keyPressRefreshTimer += Time::GetDeltaTime();

		return true;
	}

	bool DialogManager::Draw()
	{
		
		
		return false;
	}

	void DialogManager::OnDestroy()
	{
		
	}
	
	bool DialogManager::CleanUp()
	{
		

		return true;
	}

	void DialogManager::UpdateConversation(int conversationNumber, Renderer2D* render)
	{
		if (actualConversationState == 0)
		{
			actualConversationState = 1;
			firstTime = true;
			endTime = false;
			firstTimeTimer = 0;
			endTimeTimer = 0;

			characterImgPos.x = -150;
			characterImgPos.y = 100; // <--
			if (conversations[conversationNumber].isInOppositeSide == true) bubbleImgPos.x = 500; // <--
			else bubbleImgPos.x = 640; // <--
			bubbleImgPos.y = 0;

			render->EnableInstance(m_Scene, conversations[conversationNumber].characterImgID);
			render->EnableInstance(m_Scene, conversations[conversationNumber].dialogImgID);
		}

		if (actualConversationState == 1)
		{
			render->UpdateInstancedQuadTexColor(m_Scene, conversations[conversationNumber].nodes[currentNode].text1_imgModeID, BLACK);
			/*InstanceRenderer& instanceRenderer = m_Scene->GetInstanceRenderer(conversations[conversationNumber].nodes[currentNode].text1_imgModeID.renderer_id);
			instanceRenderer.UpdateInstanceColor(conversations[conversationNumber].nodes[currentNode].text1_imgModeID.instance_id, BLACK);*/

			if (((keyPressRefreshTimer / 450) % 2) == 0)
			{
				if (firstTime == false && endTime == false)
				{
					render->EnableInstance(m_Scene, continueImgID2);
					render->DisableInstance(m_Scene, continueImgID);

					if (conversations[conversationNumber].isInOppositeSide == true)
					{
						render->UpdateInstancedQuadTexPosition(m_Scene, continueImgID2, { 1350,220 }, Wiwa::Renderer2D::Pivot::UPLEFT);
					}
					else
					{
						render->UpdateInstancedQuadTexPosition(m_Scene, continueImgID2, { 1500,870 }, Wiwa::Renderer2D::Pivot::UPLEFT);
					}
				}
			}
			else
			{
				if (firstTime == false && endTime == false)
				{
					render->DisableInstance(m_Scene, continueImgID2);
					render->EnableInstance(m_Scene, continueImgID);

					if (conversations[conversationNumber].isInOppositeSide == true)
					{
						render->UpdateInstancedQuadTexPosition(m_Scene, continueImgID, { 1350,220 }, Wiwa::Renderer2D::Pivot::UPLEFT);
					}
					else
					{
						render->UpdateInstancedQuadTexPosition(m_Scene, continueImgID, { 1500,870 }, Wiwa::Renderer2D::Pivot::UPLEFT);
					}
				}

			}

			if (firstTime == false && endTime == false)
			{
				render->EnableInstance(m_Scene, conversations[conversationNumber].nodes[currentNode].text1_imgModeID);

				if (conversations[conversationNumber].isInOppositeSide == true)
				{
					render->UpdateInstancedQuadTexPosition(m_Scene, conversations[conversationNumber].nodes[currentNode].text1_imgModeID, { 600,140 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				}
				else
				{
					render->UpdateInstancedQuadTexPosition(m_Scene, conversations[conversationNumber].nodes[currentNode].text1_imgModeID, { 730,840 }, Wiwa::Renderer2D::Pivot::UPLEFT);
				}
			}
			else if (firstTime == true && firstTimeTimer > 850 && endTime == false)
			{
				render->EnableInstance(m_Scene, conversations[conversationNumber].nodes[currentNode].text1_imgModeID);
			}

			if (firstTime == true)
			{
				firstTimeTimer += Time::GetDeltaTime();

				if (conversations[conversationNumber].isInOppositeSide == true)
				{
					characterImgPos.x = EaseBackOut(firstTimeTimer, 1800, 1100 - 1800, 450);
					bubbleImgPos.y = EaseExpoOut(firstTimeTimer, -1080, -250 + 1080, 450);
				}
				else
				{
					characterImgPos.x = EaseBackOut(firstTimeTimer, -1500, -50 + 1500, 450);
					bubbleImgPos.y = EaseExpoOut(firstTimeTimer, 1080, 300 - 1080, 450);
				}

				render->UpdateInstancedQuadTexPosition(m_Scene, conversations[conversationNumber].characterImgID, characterImgPos, Wiwa::Renderer2D::Pivot::UPLEFT);
				render->UpdateInstancedQuadTexPosition(m_Scene, conversations[conversationNumber].dialogImgID, bubbleImgPos, Wiwa::Renderer2D::Pivot::UPLEFT);

				if (firstTimeTimer >= 450)
				{
					firstTime = false;

					if (conversations[conversationNumber].detectsCharacter == true)
					{
						if (characterID == 0)
						{
							conversations[conversationNumber].nodes[currentNode].audioEventName = conversations[conversationNumber].nodes[currentNode].audioEventName + "_Starlord";
						}
						else if (characterID == 1)
						{
							conversations[conversationNumber].nodes[currentNode].audioEventName = conversations[conversationNumber].nodes[currentNode].audioEventName + "_Rocket";
						}
					}

					Audio::PostWorldEvent(conversations[conversationNumber].nodes[currentNode].audioEventName.c_str());

					if (conversations[conversationNumber].isInOppositeSide == true)
					{
						characterImgPos.x = 1100;
						characterImgPos.y = 100; // <--
						bubbleImgPos.x = 500; // <--
						bubbleImgPos.y = -250;
					}
					else
					{
						characterImgPos.x = -50;
						characterImgPos.y = 100; // <--
						bubbleImgPos.x = 640; // <--
						bubbleImgPos.y = 300;
					}

					render->UpdateInstancedQuadTexPosition(m_Scene, conversations[conversationNumber].characterImgID, characterImgPos, Wiwa::Renderer2D::Pivot::UPLEFT);
					render->UpdateInstancedQuadTexPosition(m_Scene, conversations[conversationNumber].dialogImgID, bubbleImgPos, Wiwa::Renderer2D::Pivot::UPLEFT);
				}
			}
			else if (endTime == true)
			{
				endTimeTimer += Time::GetDeltaTime();
				/*
				Example:

				currentPositionX = EaseSineIn(currentTime, startPositionX, finalPositionX - startPositionX, duration);
				currentTime++;
				*/
				if (conversations[conversationNumber].isInOppositeSide == true)
				{
					characterImgPos.x = EaseBackIn(endTimeTimer, 1100, 1800 - 1100, 450);
					bubbleImgPos.y = EaseExpoIn(endTimeTimer, -250, -1080 + 250, 450);
				}
				else
				{
					characterImgPos.x = EaseBackIn(endTimeTimer, -50, -1500 + 50, 450);
					bubbleImgPos.y = EaseExpoIn(endTimeTimer, 300, 1080 - 300, 450);
				}

				render->UpdateInstancedQuadTexPosition(m_Scene, conversations[conversationNumber].characterImgID, characterImgPos, Wiwa::Renderer2D::Pivot::UPLEFT);
				render->UpdateInstancedQuadTexPosition(m_Scene, conversations[conversationNumber].dialogImgID, bubbleImgPos, Wiwa::Renderer2D::Pivot::UPLEFT);
				if (endTimeTimer >= 450)
				{

					endTime = false;

					if (conversations[conversationNumber].isInOppositeSide == true)
					{
						characterImgPos.x = 1800;
						characterImgPos.y = 100; // <--
						bubbleImgPos.x = 500; // <--
						bubbleImgPos.y = 0;
					}
					else
					{
						characterImgPos.x = -150;
						characterImgPos.y = 100; // <--
						bubbleImgPos.x = 640; // <--
						bubbleImgPos.y = 0;
					}

					render->DisableInstance(m_Scene, conversations[conversationNumber].characterImgID);
					render->DisableInstance(m_Scene, conversations[conversationNumber].dialogImgID);

					dialogEventToTrigger = conversations[conversationNumber].eventName;

					if (!strcmp(dialogEventToTrigger.c_str(), ""))
					{
						triggerEvent = false;
					}
					else
					{
						triggerEvent = true;
					}

					actualConversationState = 2;
				}
			}

			if ((Wiwa::Input::IsKeyPressed(Wiwa::Key::Space) || Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA)) && (keyPressRefreshTimer > 850 && firstTime == false && endTime == false))
			{
				render->DisableInstance(m_Scene, conversations[conversationNumber].nodes[currentNode].text1_imgModeID);
				Audio::StopWorldEvent(conversations[conversationNumber].nodes[currentNode].audioEventName.c_str());
				keyPressRefreshTimer = 0;

				currentNode++;
				if (currentNode >= MAX_CONVERSATION_NODES || conversations[conversationNumber].nodes[currentNode].occupied == false)
				{
					render->DisableInstance(m_Scene, continueImgID);
					render->DisableInstance(m_Scene, continueImgID2);

					currentNode = 0;
					firstTime = false;
					endTime = true;
				}
				else
				{
					if (conversations[conversationNumber].detectsCharacter == true)
					{
						if (characterID == 0)
						{
							conversations[conversationNumber].nodes[currentNode].audioEventName = conversations[conversationNumber].nodes[currentNode].audioEventName + "_Starlord";
						}
						else if (characterID == 1)
						{
							conversations[conversationNumber].nodes[currentNode].audioEventName = conversations[conversationNumber].nodes[currentNode].audioEventName + "_Rocket";
						}
					}

					Audio::PostWorldEvent(conversations[conversationNumber].nodes[currentNode].audioEventName.c_str());
				}
			}
		}
	}

	void DialogManager::SetDialogText(char* line1Text, const char* fontPath, int conversationNumber, int nodeNumber)
	{
		Wiwa::GuiManager& gm = m_Scene->GetGuiManager();

		Text* text1_imgMode = gm.InitFontForDialog(fontPath, line1Text, 950);

		conversations[conversationNumber].nodes[nodeNumber].text1 = (std::string)line1Text;

		Renderer2D& render = Wiwa::Application::Get().GetRenderer2D();

		conversations[conversationNumber].nodes[nodeNumber].text1_imgModeID = render.CreateInstancedQuadTex(m_Scene, text1_imgMode->GetTextureId(), text1_imgMode->GetSize(), {730,840}, {1000,650}, Wiwa::Renderer2D::Pivot::UPLEFT);
		render.DisableInstance(m_Scene, conversations[conversationNumber].nodes[nodeNumber].text1_imgModeID);

		conversations[conversationNumber].nodes[nodeNumber].occupied = true;

		totalLoadNum++;
	}

	void DialogManager::SetDialogBubbleImage(const char* path, int conversationNumber)
	{
		Renderer2D& render = Wiwa::Application::Get().GetRenderer2D();

		conversations[conversationNumber].bubbleImagePath = (std::string)path;

		ResourceId textID;
		Image* dialogImg = nullptr;
		if (conversations[conversationNumber].detectsCharacter == true)
		{
			if (characterID == 0)
			{
				if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/UI_SpeechMenuStarlordBubble_01.dds"))
				{
					textID = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/UI_SpeechMenuStarlordBubble_01.dds");
					dialogImg = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID);

					if (dialogImg)
					{
						conversations[conversationNumber].dialogImgID = render.CreateInstancedQuadTex(m_Scene, dialogImg->GetTextureId(), dialogImg->GetSize(), { 640,100 }, { 1080,1080 }, Wiwa::Renderer2D::Pivot::UPLEFT);
						render.DisableInstance(m_Scene, conversations[conversationNumber].dialogImgID);

						totalLoadNum++;
					}
				}
				else
				{
					WI_CRITICAL("UI_TalkButton01.dds Does not exist");
				}
				
			}
			else if (characterID == 1)
			{
				if (std::filesystem::exists("library/HudImages/Menus/SpeechMenu/UI_SpeechMenuRocketBubble_01.dds"))
				{
					textID = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/UI_SpeechMenuRocketBubble_01.dds");
					dialogImg = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID);

					conversations[conversationNumber].dialogImgID = render.CreateInstancedQuadTex(m_Scene, dialogImg->GetTextureId(), dialogImg->GetSize(), { 640,100 }, { 1080,1080 }, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.DisableInstance(m_Scene, conversations[conversationNumber].dialogImgID);

					totalLoadNum++;
				}
				else
				{
					WI_CRITICAL("UI_TalkButton01.dds Does not exist");
				}
			}
			else
			{
				if (std::filesystem::exists(path))
				{
					std::filesystem::path libpath = Wiwa::Resources::_assetToLibPath(path).c_str();
					libpath.replace_extension(".dds");

					textID = Wiwa::Resources::Load<Wiwa::Image>(libpath.string().c_str());
					dialogImg = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID);

					conversations[conversationNumber].dialogImgID = render.CreateInstancedQuadTex(m_Scene, dialogImg->GetTextureId(), dialogImg->GetSize(), { 640,100 }, { 1080,1080 }, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.DisableInstance(m_Scene, conversations[conversationNumber].dialogImgID);

					totalLoadNum++;
				}
				else
				{
					WI_CRITICAL("UI_TalkButton01.dds Does not exist");
				}
			}
		}
		else
		{
			if (std::filesystem::exists(path))
			{
				std::filesystem::path libpath = Wiwa::Resources::_assetToLibPath(path).c_str();
				libpath.replace_extension(".dds");

				textID = Wiwa::Resources::Load<Wiwa::Image>(libpath.string().c_str());
				dialogImg = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID);

				if (dialogImg)
				{
					conversations[conversationNumber].dialogImgID = render.CreateInstancedQuadTex(m_Scene, dialogImg->GetTextureId(), dialogImg->GetSize(), { 640,100 }, { 1080,1080 }, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.DisableInstance(m_Scene, conversations[conversationNumber].dialogImgID);

					totalLoadNum++;
				}
			}
			else
			{
				WI_CRITICAL("UI_TalkButton01.dds Does not exist");
			}
		}
	}

	void DialogManager::SetCharacterImage(const char* path, int conversationNumber)
	{
		Renderer2D& render = Wiwa::Application::Get().GetRenderer2D();

		conversations[conversationNumber].characterImagePath = (std::string)path;

		ResourceId textID;
		Image* characterImg = nullptr;
		if (conversations[conversationNumber].detectsCharacter == true)
		{
			if (characterID == 0)
			{
				if (std::filesystem::exists("library/HUDImages/Menus/SpeechMenu/UI_Starlord_01.dds"))
				{
					textID = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/UI_Starlord_01.dds");
					characterImg = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID);

					if (characterImg)
					{
						conversations[conversationNumber].characterImgID = render.CreateInstancedQuadTex(m_Scene, characterImg->GetTextureId(), characterImg->GetSize(), { -50,100 }, { 1024,1024 }, Wiwa::Renderer2D::Pivot::UPLEFT);
						render.DisableInstance(m_Scene, conversations[conversationNumber].characterImgID);

						totalLoadNum++;
					}
				}
				else
				{
					WI_CRITICAL("UI_TalkButton01.dds Does not exist");
				}
			}
			else if (characterID == 1)
			{
				if (std::filesystem::exists("library/HUDImages/Menus/SpeechMenu/UI_Rocket_01.dds"))
				{
					textID = Wiwa::Resources::Load<Wiwa::Image>("library/HUDImages/Menus/SpeechMenu/UI_Rocket_01.dds");
					characterImg = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID);

					conversations[conversationNumber].characterImgID = render.CreateInstancedQuadTex(m_Scene, characterImg->GetTextureId(), characterImg->GetSize(), { -50,100 }, { 1024,1024 }, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.DisableInstance(m_Scene, conversations[conversationNumber].characterImgID);

					totalLoadNum++;
				}
				else
				{
					WI_CRITICAL("UI_TalkButton01.dds Does not exist");
				}
			}
			else
			{
				if (std::filesystem::exists(path))
				{
					std::filesystem::path libpath = Wiwa::Resources::_assetToLibPath(path).c_str();
					libpath.replace_extension(".dds");

					textID = Wiwa::Resources::Load<Wiwa::Image>(libpath.string().c_str());
					characterImg = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID);

					conversations[conversationNumber].characterImgID = render.CreateInstancedQuadTex(m_Scene, characterImg->GetTextureId(), characterImg->GetSize(), { -50,100 }, { 1024,1024 }, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.DisableInstance(m_Scene, conversations[conversationNumber].characterImgID);

					totalLoadNum++;
				}
				else
				{
					WI_CRITICAL("UI_TalkButton01.dds Does not exist");
				}
			}
		}
		else
		{
			if (std::filesystem::exists(path))
			{
				std::filesystem::path libpath = Wiwa::Resources::_assetToLibPath(path).c_str();
				libpath.replace_extension(".dds");

				textID = Wiwa::Resources::Load<Wiwa::Image>(libpath.string().c_str());
				characterImg = Wiwa::Resources::GetResourceById<Wiwa::Image>(textID);

				if (characterImg)
				{
					conversations[conversationNumber].characterImgID = render.CreateInstancedQuadTex(m_Scene, characterImg->GetTextureId(), characterImg->GetSize(), { -50,100 }, { 1024,1024 }, Wiwa::Renderer2D::Pivot::UPLEFT);
					render.DisableInstance(m_Scene, conversations[conversationNumber].characterImgID);

					totalLoadNum++;
				}
			}
			else
			{
				WI_CRITICAL("UI_TalkButton01.dds Does not exist");
			}
		}
	}

	void DialogManager::ForceDialogStart(std::string forcedConversationToStart, bool isCharacterSensitive)
	{
		forcedDialogHappened = false;
		forceStartConversation = true;
		characterSensitive = isCharacterSensitive;

		forcedConversationTag = forcedConversationToStart;
	}

	void DialogManager::RestartForceDialogState()
	{
		forcedDialogHappened = false;
	}

	void DialogManager::SaveAllDialogs()
	{
		Wiwa::JSONDocument doc;

		for (int i = 0; (i < MAX_CONVERSATIONS) && conversations[i].occupied == true; i++)
		{
			std::string s = std::to_string(i);

			std::string memberNameConversation = "Name_Conversation" + s;
			std::string memberNameBubbleImage = "BubbleImagePath_Conversation" + s;
			std::string memberNameCharacterImage = "CharacterImagePath_Conversation" + s;

			std::string memberNameGroupId = "GroupID_Conversation" + s;
			std::string memberNameGroupOrder = "GroupOrder_Conversation" + s;

			std::string memberNameOppositeSide = "IsOppositeSide_Conversation" + s;
			std::string memberNameIsRandom = "IsRandom_Conversation" + s;
			std::string memberNameDetectsCharacter = "DetectsCharacter_Conversation" + s;

			std::string memberNameEvent = "Event_Conversation" + s;


			doc.AddMember(memberNameConversation.c_str(), conversations[i].conversationName.c_str());

			std::filesystem::path libpath2 = Wiwa::Resources::_assetToLibPath(conversations[i].bubbleImagePath).c_str();
			libpath2.replace_extension(".dds");
			doc.AddMember(memberNameBubbleImage.c_str(), libpath2.string().c_str());

			std::filesystem::path libpath3 = Wiwa::Resources::_assetToLibPath(conversations[i].characterImagePath).c_str();
			libpath3.replace_extension(".dds");
			doc.AddMember(memberNameCharacterImage.c_str(), libpath3.string().c_str());

			doc.AddMember(memberNameGroupId.c_str(), conversations[i].group.groupID.c_str());
			doc.AddMember(memberNameGroupOrder.c_str(),conversations[i].group.order.c_str());

			doc.AddMember(memberNameOppositeSide.c_str(), (bool)conversations[i].isInOppositeSide);
			doc.AddMember(memberNameIsRandom.c_str(), (bool)conversations[i].isRandom);
			doc.AddMember(memberNameDetectsCharacter.c_str(), (bool)conversations[i].detectsCharacter);

			doc.AddMember(memberNameEvent.c_str(), conversations[i].eventName.c_str());


			for (int j = 0; j < MAX_CONVERSATION_NODES && conversations[i].nodes[j].occupied == true; j++)
			{
				std::string s2 = std::to_string(j);

				std::string conv = "Conversation" + s;

				std::string memberNameLine1 = "_Line1_Node" + s2;
				std::string memberNameLine4 = "_AudioEvent_Node" + s2;

				memberNameLine1 = conv + memberNameLine1;
				memberNameLine4 = conv + memberNameLine4;

				doc.AddMember(memberNameLine1.c_str(), conversations[i].nodes[j].text1.c_str());

				doc.AddMember(memberNameLine4.c_str(), conversations[i].nodes[j].audioEventName.c_str());

			}
		}

		std::string path = "config/conversations";
		path += ".json";
		doc.save_file(path.c_str());
	}

	void DialogManager::LoadAllDialogs()
	{

		bool breakConversationsLoop = false;
		bool breakNodesLoop = false;

		Wiwa::JSONDocument doc("config/conversations.json");
		if (doc.IsObject())
		{
			for (int i = 0; (i < MAX_CONVERSATIONS) && breakConversationsLoop == false; i++)
			{
				std::string s = std::to_string(i);

				std::string memberNameConversation = "Name_Conversation" + s;
				std::string memberNameBubbleImage = "BubbleImagePath_Conversation" + s;
				std::string memberNameCharacterImage = "CharacterImagePath_Conversation" + s;

				std::string memberNameGroupId = "GroupID_Conversation" + s;
				std::string memberNameGroupOrder = "GroupOrder_Conversation" + s;

				std::string memberNameOppositeSide = "IsOppositeSide_Conversation" + s;
				std::string memberNameIsRandom = "IsRandom_Conversation" + s;
				std::string memberNameDetectsCharacter = "DetectsCharacter_Conversation" + s;

				std::string memberNameEvent = "Event_Conversation" + s;


				if (doc.HasMember(memberNameConversation.c_str())
					&& doc.HasMember(memberNameBubbleImage.c_str())
					&& doc.HasMember(memberNameCharacterImage.c_str())
					&& doc.HasMember(memberNameGroupId.c_str())
					&& doc.HasMember(memberNameGroupOrder.c_str())
					&& doc.HasMember(memberNameOppositeSide.c_str())
					&& doc.HasMember(memberNameEvent.c_str()))
				{
					breakNodesLoop = false;
				}

				for (int j = 0; (j < MAX_CONVERSATION_NODES) && breakNodesLoop == false; j++)
				{
					std::string s2 = std::to_string(j);

					std::string conv = "Conversation" + s;

					std::string memberNameLine1 = "_Line1_Node" + s2;

					std::string memberNameLine4 = "_AudioEvent_Node" + s2;

					memberNameLine1 = conv + memberNameLine1;

					memberNameLine4 = conv + memberNameLine4;

					if (doc.HasMember(memberNameLine1.c_str())
						&& doc.HasMember(memberNameLine4.c_str()))
					{
						SetDialogText((char*)doc[memberNameLine1.c_str()].as_string(), "library/Fonts/Jade_Smile.ttf", i, j);
						conversations[i].nodes[j].audioEventName = doc[memberNameLine4.c_str()].as_string();
					}
					else
					{
						breakNodesLoop = true;
					}
				}

				if (doc.HasMember(memberNameConversation.c_str())
					&& doc.HasMember(memberNameBubbleImage.c_str())
					&& doc.HasMember(memberNameCharacterImage.c_str())
					&& doc.HasMember(memberNameGroupId.c_str())
					&& doc.HasMember(memberNameGroupOrder.c_str())
					&& doc.HasMember(memberNameOppositeSide.c_str())
					&& doc.HasMember(memberNameIsRandom.c_str())
					&& doc.HasMember(memberNameDetectsCharacter.c_str())
					&& doc.HasMember(memberNameEvent.c_str()))
				{
					conversations[i].detectsCharacter = doc[memberNameDetectsCharacter.c_str()].as_bool();

					conversations[i].conversationName = doc[memberNameConversation.c_str()].as_string();
					SetDialogBubbleImage(doc[memberNameBubbleImage.c_str()].as_string(), i);
					SetCharacterImage(doc[memberNameCharacterImage.c_str()].as_string(), i);

					conversations[i].group.groupID = doc[memberNameGroupId.c_str()].as_string();
					conversations[i].group.order = doc[memberNameGroupOrder.c_str()].as_string();

					conversations[i].isInOppositeSide = doc[memberNameOppositeSide.c_str()].as_bool();
					conversations[i].isRandom = doc[memberNameIsRandom.c_str()].as_bool();

					conversations[i].eventName = doc[memberNameEvent.c_str()].as_string();

					conversations[i].occupied = true;

				}
				else
				{
					breakConversationsLoop = true;
				}

			}
		}

		doc.save_file("config/conversations.json");
	}
	
}
