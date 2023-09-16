#pragma once

#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/utilities/render/Text.h>
#include <vector>
#include <string>
#include <Wiwa/core/Input.h>

#define MAX_CONVERSATIONEVENTS 25

namespace Wiwa
{
	struct ConversationEvent
	{
		std::string eventName;
	};

	class WI_API DialogEventManager
	{
	public:
		Scene* m_Scene;
	public:

		// Constructor
		DialogEventManager();

		// Destructor
		~DialogEventManager();

	
		// Init gui manager
		bool Init(Scene* scene);

		// Update gui manager
		bool Update();

		bool Draw();

		void OnDestroy();

		// Called before quitting
		bool CleanUp();

		void UatuReward(int optionNumber);
		void AronReward(int optionNumber);
		void UlanaReward(int optionNumber);

	public:

		Wiwa::Renderer2D::InstanceData choice1Opt1ImgID;
		Wiwa::Renderer2D::InstanceData choice1Opt2ImgID;

		Wiwa::Renderer2D::InstanceData choice2Opt1ImgID;
		Wiwa::Renderer2D::InstanceData choice2Opt2ImgID;
		Wiwa::Renderer2D::InstanceData choice2Opt3ImgID;

		Wiwa::Renderer2D::InstanceData choice3Opt1ImgID;
		Wiwa::Renderer2D::InstanceData choice3Opt2ImgID;

		int selector = 0;

		bool eventStarted = false;
		bool eventFinished = false;

		bool selected = false;

		int eventState = 2; // 0 = started event, 1 = running event, 2 = finished event

		int keyPressTimer = 0;

		Vector2i ImgPos1 = { 0 }; //Will be controlled with the easings
		Vector2i ImgPos2 = { 0 }; //Will be controlled with the easings
		Vector2i ImgPos3 = { 0 }; //Will be controlled with the easings

		int endTimer1 = 0;
		int endTimer2 = 0;
		int endTimer3 = 0;

		int startTimer1 = 0;
		int startTimer2 = 0;
		int startTimer3 = 0;

	};
}