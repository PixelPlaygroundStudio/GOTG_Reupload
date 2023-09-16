#pragma once

#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/utilities/render/Text.h>
#include <vector>
#include <string>
#include <Wiwa/core/Input.h>

#include "DialogNode.h"

#include <Wiwa/Ui/UiManager.h>

#define MAX_CONVERSATIONS 25
#define MAX_CONVERSATION_NODES 25
#define MAX_CONVERSATION_GROUPS 25

namespace Wiwa
{
	struct Conversation
	{

		// All nodes
		DialogNode nodes[MAX_CONVERSATION_NODES];

		// Dialog bg image, character and continue button
		Wiwa::Renderer2D::InstanceData dialogImgID;
		Wiwa::Renderer2D::InstanceData characterImgID;

		std::string conversationName;

		std::string bubbleImagePath;
		std::string characterImagePath;

		bool isInOppositeSide = false;
		bool isRandom = false;
		bool detectsCharacter = false;

		bool occupied = false;

		ConversationGroup group;

		std::string eventName;
	};

	class WI_API DialogManager
	{
	public:
		Scene* m_Scene;
	public:

		// Constructor
		DialogManager();

		// Destructor
		~DialogManager();

	
		// Init gui manager
		bool Init(Scene* scene);

		// Update gui manager
		bool Update();

		bool Draw();

		void OnDestroy();

		// Called before quitting
		bool CleanUp();

		void UpdateConversation(int conversationNumber, Renderer2D* render);

		// Set the dialog font and texts
		void SetDialogText(char* line1Text, const char* fontPath, int conversationNumber, int nodeNumber);

		// Set the dialog background images
		void SetDialogBubbleImage(const char* path, int conversationNumber);
		void SetCharacterImage(const char* path, int conversationNumber);
		//void SetContinueIndicatorImage(const char* path, int conversationNumber);

		void ForceDialogStart(std::string forcedConversationToStart, bool isCharacterSensitive = false);
		void RestartForceDialogState();

		void SaveAllDialogs();
		void LoadAllDialogs();

	public:

		std::string conversationToPlayName;
		Conversation conversations[MAX_CONVERSATIONS];
		Conversation editorConversations[MAX_CONVERSATIONS];

		int actualConversationState = 0; // 0: Not conversating; 1: Is conversating; 2: Has just finished conversating;
		int currentNode = 0;

		int keyPressRefreshTimer = 0;

		//Conversation* conversationCreator_P;

		bool collidingWithNpc = false;
		std::string NpcConversationTag;

		bool forceStartConversation = false;
		std::string forcedConversationTag;

		Wiwa::Renderer2D::InstanceData continueImgID;
		Wiwa::Renderer2D::InstanceData continueImgID2;

		Wiwa::Renderer2D::InstanceData talkIndicatorImgID;
		bool talkIndicatorImgEnabled = false;

		Color4f	BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };

		Vector2i characterImgPos = { 0 }; //Will be controlled with the easings
		Vector2i bubbleImgPos = { 0 }; //Will be controlled with the easings

		bool firstTime = false;
		bool endTime = false;
		int firstTimeTimer = 0;
		int endTimeTimer = 0;

		int convGroup = -1;
		int convOrder = -1;

		bool forcedDialogHappened = false;
		bool alreadyTalkedOnceTriggered = false;
		bool canNoLongerTalk = false;

		bool finishedRandomizing = false;

		int characterID = -1; // -1 is none, 0 is Starlord, 1 is Rocket
		bool characterSensitive = false;

		std::string dialogEventToTrigger = "";
		bool triggerEvent = false;

		int totalLoadNum = 0;
	};
}