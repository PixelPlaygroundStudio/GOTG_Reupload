#include <wipch.h>
#include "DialogPanel.h"
#include "imgui.h"
#include "Time.h"

#include <Wiwa/core/Application.h>

using namespace Wiwa;

DialogPanel::DialogPanel(EditorLayer* instance)
	: Panel("Dialog", ICON_FK_LIST, instance)
{
	for (int a = 0; a < MAX_CONVERSATIONS; a++)
	{
		for (int b = 0; b < MAX_CONVERSATION_NODES; b++)
		{
			currentEditingNodeIsSaved[a][b] = true;
		}
	}

	Wiwa::DialogManager& dm_onInit = Wiwa::SceneManager::getActiveScene()->GetDialogManager();
	if (!&dm_onInit)
		return;
	for (int c = 0; c < MAX_CONVERSATIONS && dm_onInit.conversations[c].occupied == true; c++)
	{
		currentEditingConversationName[c] = dm_onInit.conversations[c].conversationName;
	}
}

DialogPanel::~DialogPanel()
{
}

void DialogPanel::Draw()
{
	Wiwa::DialogManager& dm = Wiwa::SceneManager::getActiveScene()->GetDialogManager();

	ImGui::Begin(iconName.c_str(), &active);
	
	ImGui::NewLine();

	if (ImGui::CollapsingHeader("Create Conversations"))
	{
		if (creatingNewDialog == false)
		{
			if (ImGui::Button("New Conversation"))
			{
				numNodesOnCurrentCreation = 1;
				creatingNewDialog = true;
				currentCreationStep = 0;
			}
		}
		else if (creatingNewDialog == true)
		{
			if (ImGui::Button("Cancel Conversation Creation"))
			{
				numNodesOnCurrentCreation = 1;
				creatingNewDialog = false;
				currentCreationStep = 0;
			}
		}

		if (creatingNewDialog == true)
		{
			if(currentCreationStep == 0)
			{
				currentNode = 0;
				currentConversation = 0;

				for (int k = 0; (k < MAX_CONVERSATIONS) && (dm.conversations[k].occupied == true); k++)
				{
					currentConversation++;
				}

				for (int m = 0; m < MAX_CONVERSATION_NODES; m++)
				{
					currentCreatingNodeIsSaved[m] = false;
					dm.conversations[currentConversation].nodes[m].occupied = false;
					dm.conversations[currentConversation].nodes[m].text1 = "";

					dm.conversations[currentConversation].nodes[m].audioEventName = "";
				}

				dm.conversations[currentConversation].bubbleImagePath = "";
				dm.conversations[currentConversation].characterImagePath = "";
				dm.conversations[currentConversation].conversationName = "";
				dm.conversations[currentConversation].isInOppositeSide = false;
				dm.conversations[currentConversation].isRandom = false;
				dm.conversations[currentConversation].detectsCharacter = false;
				dm.conversations[currentConversation].group.groupID = "-1";
				dm.conversations[currentConversation].group.order = "-1";
				dm.conversations[currentConversation].eventName = "";
				dm.conversations[currentConversation].nodes[0].occupied = true;

				currentCreationStep = 1;
			}

			if (currentCreationStep == 1)
			{
				if(currentCreatingNodeIsSaved[currentNode] == false) ImGui::TextWrapped("Currently Editing Node # %i (Unsaved*)", currentNode);
				if (currentCreatingNodeIsSaved[currentNode] == true) ImGui::TextWrapped("Currently Editing Node # %i", currentNode);
				ImGui::NewLine();

				ImGui::TextWrapped("Text is wrapped so it will jump lines automatically. But be careful not to write more than 4 lines or you will surpass the text buble's height limit. To force a jump in line write the following char on text: | ");
				if (ImGui::InputText("Bubble text", &dm.conversations[currentConversation].nodes[currentNode].text1))
				{
					currentCreatingNodeIsSaved[currentNode] = false;
				}
				ImGui::NewLine();
				if (ImGui::InputText("Audio event name", &dm.conversations[currentConversation].nodes[currentNode].audioEventName))
				{
					currentCreatingNodeIsSaved[currentNode] = false;
				}
				ImGui::TextWrapped("Note: Leave 'Audio event name' case blank if no audio event should be played. This event is normally used for voice line audio.");
				ImGui::NewLine();

				ImGui::PushID(currentNode);
				if (currentNode > 0)
				{
					if (ImGui::Button("< Previous Node"))
					{
						currentNode--;
					}

				}
				else
				{
					if (ImGui::Button("       -       "))
					{

					}
				}
				ImGui::PopID();
				ImGui::SameLine();
				ImGui::PushID(currentNode);
				if (dm.conversations[currentConversation].nodes[currentNode + 1].occupied == true)
				{
					if (ImGui::Button("Next Node >"))
					{
						currentNode++;
					}
				}
				else
				{
					if (ImGui::Button("       -       "))
					{

					}
				}
				ImGui::PopID();
				ImGui::PushID(numNodesOnCurrentCreation);
				if (ImGui::Button("Save Node"))
				{
					dm.SetDialogText(
						(char*)dm.conversations[currentConversation].nodes[currentNode].text1.c_str(),
						"library/Fonts/Jade_Smile.ttf", currentConversation, currentNode);
					dm.conversations[currentConversation].nodes[currentNode].occupied = true;

					currentCreatingNodeIsSaved[currentNode] = true;
				}
				ImGui::PopID();
				ImGui::SameLine();
				ImGui::PushID(numNodesOnCurrentCreation);
				if (ImGui::Button("New Node"))
				{
					bool exit = false;
					for (int j = 0; j < MAX_CONVERSATION_NODES && exit == false; j++)
					{
						if (dm.conversations[currentConversation].nodes[j + 1].occupied == false)
						{
							dm.conversations[currentConversation].nodes[j + 1].occupied = true;
							exit = true;
						}
					}

					numNodesOnCurrentCreation++;
					currentNode++;
				}
				ImGui::PopID();
				ImGui::SameLine();
				ImGui::PushID(numNodesOnCurrentCreation);
				if (ImGui::Button("Delete Node"))
				{
					dm.conversations[currentConversation].nodes[currentNode].occupied = false;

					int isNodeOccupiedNumTimesFalse = 0;

					for (int l = 0; (l < MAX_CONVERSATIONS) && (isNodeOccupiedNumTimesFalse < 2); l++)
					{
						if (dm.conversations[currentConversation].nodes[l].occupied == false)
						{
							isNodeOccupiedNumTimesFalse++;
						}

						if (isNodeOccupiedNumTimesFalse == 1)
						{
							dm.conversations[currentConversation].nodes[l].text1 = dm.conversations[currentConversation].nodes[l + 1].text1;

							dm.conversations[currentConversation].nodes[l].audioEventName = dm.conversations[currentConversation].nodes[l + 1].audioEventName;

							dm.conversations[currentConversation].nodes[l].occupied = dm.conversations[currentConversation].nodes[l + 1].occupied;

						}

						if (dm.editorConversations[l].occupied == false && isNodeOccupiedNumTimesFalse == 2 && currentNode > 0)
						{
							currentNode--;
						}
					}
				}
				ImGui::PopID();

				ImGui::TextWrapped("WARNING, Only Press this button if you fully finished creating all the dialog nodes:");
				ImGui::PushID(numNodesOnCurrentCreation);
				if (ImGui::Button("Finished Creating All Dialog Nodes"))
				{
					currentCreationStep = 2;
				}
				ImGui::PopID();
			}

			if (currentCreationStep == 2)
			{

				currentCreationStep = 3;
			}

			if (currentCreationStep == 3)
			{
				ImGui::TextWrapped("Bubble image:");
				ImGui::SameLine();
				AssetContainer(dm.conversations[currentConversation].bubbleImagePath.c_str());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						std::string pathS(ws.begin(), ws.end());
						std::filesystem::path p = pathS.c_str();
						if (p.extension() == ".png")
						{
							dm.conversations[currentConversation].bubbleImagePath = pathS;
							dm.SetDialogBubbleImage(pathS.c_str(), currentConversation);
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::TextWrapped("WARNING, Only Press this button if you finished importing the bubble image:");
				if (ImGui::Button("Finished Importing Bubble Image"))
				{
					currentCreationStep = 4;
				}
			}

			if (currentCreationStep == 4)
			{
				ImGui::TextWrapped("Character image:");
				ImGui::SameLine();
				AssetContainer(dm.conversations[currentConversation].characterImagePath.c_str());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						std::string pathS(ws.begin(), ws.end());
						std::filesystem::path p = pathS.c_str();
						if (p.extension() == ".png")
						{
							dm.conversations[currentConversation].characterImagePath = pathS;
							dm.SetCharacterImage(pathS.c_str(), currentConversation);
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::TextWrapped("WARNING, Only Press this button if you finished importing the character image:");
				if (ImGui::Button("Finished Importing Character Image"))
				{
					currentCreationStep = 5;
				}
			}

			if (currentCreationStep == 5)
			{
				ImGui::InputText("Conversation name", &dm.conversations[currentConversation].conversationName);
				ImGui::TextWrapped("Note: 'Conversation name' must match with the entity tag name asigned to the NPC object.");

				ImGui::NewLine();

				ImGui::Checkbox("Is character on Right?", &dm.conversations[currentConversation].isInOppositeSide);
				ImGui::TextWrapped("Note: 'Is character on Right?' defines wether the character is standing on the left or right side of the bubble. It will also make bubble appear higher.");

				ImGui::NewLine();

				ImGui::InputText("Conversation Group", &dm.conversations[currentConversation].group.groupID);
				ImGui::TextWrapped("Note: 'Conversation Group' defines if this conversation belongs to a group. Leave it to '-1' if this conversation should not belong to a group.");

				ImGui::InputText("Group Order", &dm.conversations[currentConversation].group.order);
				ImGui::TextWrapped("Note: 'Group Order' defines the order conversations will happen in the group starting from '0'. Leave it to '-1' if this conversation does not belong to any group.");

				ImGui::Checkbox("Is Group Order Random?", &dm.conversations[currentConversation].isRandom);
				ImGui::TextWrapped("Note: 'Is Group Order Random?' will make the dialog manager pick a random conversation from the group the conversations belongs when this current specific dialog is triggered.");

				ImGui::NewLine();

				ImGui::Checkbox("Detects selected character?", &dm.conversations[currentConversation].detectsCharacter);
				ImGui::TextWrapped("Note: 'Detects selected character?' will replace the bubble and character images with the ones assigned to the current selected character.");

				ImGui::NewLine();

				ImGui::InputText("Event name", &dm.conversations[currentConversation].eventName);
				ImGui::TextWrapped("Note: 'Event name' will trigger a specific event when the conversation ends if the input written here is correct (see list of event names). Leave it blank if no event should be triggered.");

				ImGui::NewLine();

				ImGui::TextWrapped("WARNING, Only Press this button if you finished creating the conversation");
				if (ImGui::Button("Finish And Save Conversation"))
				{
					dm.conversations[currentConversation].occupied = true;
					dm.SaveAllDialogs();

					for (int m = 0; m < MAX_CONVERSATION_NODES; m++)
					{
						currentCreatingNodeIsSaved[m] = false;
						dm.editorConversations[currentConversation].nodes[m].occupied = dm.conversations[currentConversation].nodes[m].occupied;
						dm.editorConversations[currentConversation].nodes[m].text1 = dm.conversations[currentConversation].nodes[m].text1;
						dm.editorConversations[currentConversation].nodes[m].audioEventName = dm.conversations[currentConversation].nodes[m].audioEventName;

					}

					dm.editorConversations[currentConversation].bubbleImagePath = dm.conversations[currentConversation].bubbleImagePath;
					dm.editorConversations[currentConversation].characterImagePath = dm.conversations[currentConversation].characterImagePath;
					dm.editorConversations[currentConversation].conversationName = dm.conversations[currentConversation].conversationName;
					dm.editorConversations[currentConversation].nodes[0].occupied = dm.conversations[currentConversation].nodes[0].occupied;

					dm.editorConversations[currentConversation].isInOppositeSide = dm.conversations[currentConversation].isInOppositeSide;
					dm.editorConversations[currentConversation].isRandom = dm.conversations[currentConversation].isRandom;
					dm.editorConversations[currentConversation].detectsCharacter = dm.conversations[currentConversation].detectsCharacter;
					dm.editorConversations[currentConversation].group.groupID = dm.conversations[currentConversation].group.groupID;
					dm.editorConversations[currentConversation].group.order = dm.conversations[currentConversation].group.order;
					dm.editorConversations[currentConversation].eventName = dm.conversations[currentConversation].eventName;

					currentEditingConversationName[currentConversation] = dm.conversations[currentConversation].conversationName;

					currentConversation++;

					currentCreationStep = 6;
				}
			}

			if (currentCreationStep == 6)
			{
				creatingNewDialog = false;

			}
		}
	}
	ImGui::Separator();
	ImGui::TextWrapped("Edit Conversations:");
	ImGui::NewLine();

	if (dm.conversations[0].occupied == true)
	{
		for (int i = 0; i < MAX_CONVERSATIONS && dm.conversations[i].occupied == true; i++)
		{
			ImGui::PushID(i);
			if (ImGui::CollapsingHeader(currentEditingConversationName[i].c_str()))
			{
				if(currentEditingNodeIsSaved[currentEditingNode[i]][i] == true) ImGui::TextWrapped("Node %i:", currentEditingNode[i]);
				else if (currentEditingNodeIsSaved[currentEditingNode[i]][i] == false) ImGui::TextWrapped("Node %i (unsaved*):", currentEditingNode[i]);
				ImGui::NewLine();

				ImGui::TextWrapped("Text is wrapped so it will jump lines automatically. But be careful not to write more than 4 lines or you will surpass the text buble's height limit. To force a jump in line write the following char on text: | ");
				ImGui::PushID(currentEditingNode[i]);
				if (ImGui::InputText("Bubble text", &dm.editorConversations[i].nodes[currentEditingNode[i]].text1))
				{
					currentEditingNodeIsSaved[currentEditingNode[i]][i] = false;
				}
				ImGui::PopID();
				ImGui::NewLine();
				ImGui::PushID(currentEditingNode[i]);
				if (ImGui::InputText("Audio event name", &dm.editorConversations[i].nodes[currentEditingNode[i]].audioEventName))
				{
					currentEditingNodeIsSaved[currentEditingNode[i]][i] = false;
				}
				ImGui::PopID();
				ImGui::TextWrapped("Note: Leave 'Audio event name' case blank if no audio event should be played. This event is normally used for voice line audio.");
				ImGui::NewLine();
				ImGui::PushID(currentEditingNode[i]);
				if (currentEditingNode[i] > 0)
				{
					if (ImGui::Button("< Previous Node"))
					{
						currentEditingNode[i]--;
					}
					
				}
				else
				{
					if (ImGui::Button("       -       "))
					{
						
					}
				}
				ImGui::PopID();
				ImGui::SameLine();
				ImGui::PushID(currentEditingNode[i]);
				if (dm.editorConversations[i].nodes[currentEditingNode[i] + 1].occupied == true)
				{
					if (ImGui::Button("Next Node >"))
					{
						currentEditingNode[i]++;
					}
				}
				else
				{
					if (ImGui::Button("       -       "))
					{
						
					}
				}
				ImGui::PopID();
				if (ImGui::Button("New Node"))
				{
					bool exit = false;
					for (int j = 0; j < MAX_CONVERSATION_NODES && exit == false; j++)
					{
						if (dm.editorConversations[i].nodes[j + 1].occupied == false)
						{
							dm.editorConversations[i].nodes[j + 1].occupied = true;
							exit = true;
						}
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete Node"))
				{
					dm.editorConversations[i].nodes[currentEditingNode[i]].occupied = false;

					int isNodeOccupiedNumTimesFalse = 0;

					for (int l = 0; (l < MAX_CONVERSATION_NODES) && (isNodeOccupiedNumTimesFalse < 2); l++)
					{
						if (dm.editorConversations[i].nodes[l].occupied == false)
						{
							isNodeOccupiedNumTimesFalse++;
						}

						if (isNodeOccupiedNumTimesFalse == 1)
						{
							dm.editorConversations[i].nodes[l].text1 = dm.editorConversations[i].nodes[l + 1].text1;

							dm.editorConversations[i].nodes[l].audioEventName = dm.editorConversations[i].nodes[l + 1].audioEventName;

							dm.editorConversations[i].nodes[l].occupied = dm.editorConversations[i].nodes[l + 1].occupied;

							currentEditingNodeIsSaved[currentEditingNode[i]][l] = currentEditingNodeIsSaved[currentEditingNode[i]][l + 1];
						}

						if (dm.editorConversations[l].occupied == false && isNodeOccupiedNumTimesFalse == 2 && currentEditingNode[i] > 0)
						{
							currentEditingNode[i]--;
						}
					}

					
				}

				ImGui::PushID(currentEditingNode[i]);
				if (ImGui::Button(" -= SAVE NODE =- "))
				{
					dm.conversations[i].nodes[currentEditingNode[i]].text1 = dm.editorConversations[i].nodes[currentEditingNode[i]].text1;

					dm.conversations[i].nodes[currentEditingNode[i]].audioEventName = dm.editorConversations[i].nodes[currentEditingNode[i]].audioEventName;

					dm.SetDialogText(
						(char*)dm.conversations[i].nodes[currentEditingNode[i]].text1.c_str(),
						"library/Fonts/Jade_Smile.ttf", i, currentEditingNode[i]);

					currentEditingNodeIsSaved[currentEditingNode[i]][i] = true;
				}
				ImGui::PopID();

				ImGui::NewLine();

				ImGui::TextWrapped("Character image:");
				ImGui::SameLine();
				ImGui::PushID(i);
				AssetContainer(dm.editorConversations[i].characterImagePath.c_str());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						std::string pathS(ws.begin(), ws.end());
						std::filesystem::path p = pathS.c_str();
						if (p.extension() == ".png")
						{
							dm.editorConversations[i].characterImagePath = pathS;
							
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
				ImGui::NewLine();

				ImGui::TextWrapped("Bubble image:");
				ImGui::SameLine();
				ImGui::PushID(i);
				AssetContainer(dm.editorConversations[i].bubbleImagePath.c_str());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						std::string pathS2(ws.begin(), ws.end());
						std::filesystem::path p = pathS2.c_str();
						if (p.extension() == ".png")
						{
							dm.editorConversations[i].bubbleImagePath = pathS2;
							
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
				ImGui::NewLine();

				ImGui::PushID(i);
				ImGui::InputText("Conversation Name", &dm.editorConversations[i].conversationName);
				ImGui::PopID();
				ImGui::TextWrapped("Note: 'Conversation name' must match with the entity tag name asigned to the NPC object.");

				ImGui::NewLine();

				ImGui::Checkbox("Is character on Right?", &dm.editorConversations[i].isInOppositeSide);
				ImGui::TextWrapped("Note: 'Is character on Right?' defines wether the character is standing on the left or right side of the bubble. It will also make bubble appear higher.");

				ImGui::NewLine();

				ImGui::InputText("Conversation Group", &dm.editorConversations[i].group.groupID);
				ImGui::TextWrapped("Note: 'Conversation Group' defines if this conversation belongs to a group. Leave it to '-1' if this conversation should not belong to a group.");

				ImGui::InputText("Group Order", &dm.editorConversations[i].group.order);
				ImGui::TextWrapped("Note: 'Group Order' defines the order conversations will happen in the group starting from '0'. Leave it to '-1' if this conversation does not belong to any group.");

				ImGui::Checkbox("Is Group Order Random?", &dm.editorConversations[i].isRandom);
				ImGui::TextWrapped("Note: 'Is Group Order Random?' will make the dialog manager pick a random conversation from the group the conversations belongs when this current specific dialog is triggered.");

				ImGui::NewLine();

				ImGui::Checkbox("Detects selected character?", &dm.editorConversations[i].detectsCharacter);
				ImGui::TextWrapped("Note: 'Detects selected character?' will replace the bubble and character images with the ones assigned to the current selected character.");

				ImGui::NewLine();

				ImGui::InputText("Event name", &dm.editorConversations[i].eventName);
				ImGui::TextWrapped("Note: 'Event name' will trigger a specific event when the conversation ends if the input written here is correct (see list of event names). Leave it blank if no event should be triggered.");

				ImGui::NewLine();

				ImGui::TextWrapped("DONR FORGET SAVING!");
				ImGui::PushID(i);
				if (ImGui::Button(" ~ Save Conversation ~ "))
				{
					currentEditingNode[i] = 0;

					dm.conversations[i].nodes[currentEditingNode[i]].text1 = dm.editorConversations[i].nodes[currentEditingNode[i]].text1;

					dm.conversations[i].nodes[currentEditingNode[i]].audioEventName = dm.editorConversations[i].nodes[currentEditingNode[i]].audioEventName;

					dm.SetDialogText(
						(char*)dm.conversations[i].nodes[currentEditingNode[i]].text1.c_str(),
						"library/Fonts/Jade_Smile.ttf", i, currentEditingNode[i]);

					dm.conversations[i].characterImagePath = dm.editorConversations[i].characterImagePath;
					dm.SetCharacterImage(dm.conversations[i].characterImagePath.c_str(), i);
					dm.conversations[i].bubbleImagePath = dm.editorConversations[i].bubbleImagePath;
					dm.SetDialogBubbleImage(dm.conversations[i].bubbleImagePath.c_str(), i);
					dm.conversations[i].conversationName = dm.editorConversations[i].conversationName;

					dm.conversations[i].isInOppositeSide = dm.editorConversations[i].isInOppositeSide;
					dm.conversations[i].isRandom = dm.editorConversations[i].isRandom;
					dm.conversations[i].detectsCharacter = dm.editorConversations[i].detectsCharacter;
					dm.conversations[i].group.groupID = dm.editorConversations[i].group.groupID;
					dm.conversations[i].group.order = dm.editorConversations[i].group.order;
					dm.conversations[i].eventName = dm.editorConversations[i].eventName;

					dm.SaveAllDialogs();

					saved = 1;
					currentEditingConversationName[i] = "Saving...";
					savedID = i;
				}
				ImGui::PopID();
				ImGui::SameLine();
				ImGui::PushID(i);
				if (ImGui::Button(" ~ Delete Conversation ~ "))
				{
					dm.conversations[i].occupied = false;

					int isOccupiedNumTimesFalse = 0;

					for (int l = 0; (l < MAX_CONVERSATIONS) && (isOccupiedNumTimesFalse < 2); l++)
					{
						if (dm.conversations[l].occupied == false)
						{
							isOccupiedNumTimesFalse++;
						}

						if (isOccupiedNumTimesFalse == 1)
						{
							dm.conversations[l].bubbleImagePath = dm.conversations[l + 1].bubbleImagePath;
							dm.conversations[l].characterImagePath = dm.conversations[l + 1].characterImagePath;
							dm.conversations[l].conversationName = dm.conversations[l + 1].conversationName;

							dm.conversations[l].isInOppositeSide = dm.conversations[l + 1].isInOppositeSide;
							dm.conversations[l].isRandom = dm.conversations[l + 1].isRandom;
							dm.conversations[l].detectsCharacter = dm.conversations[l + 1].detectsCharacter;
							dm.conversations[l].group.groupID = dm.conversations[l + 1].group.groupID;
							dm.conversations[l].group.order = dm.conversations[l + 1].group.order;
							dm.conversations[l].eventName = dm.conversations[l + 1].eventName;

							for (int m = 0; m < MAX_CONVERSATION_NODES; m++)
							{
								dm.conversations[l].nodes[m].text1 = dm.conversations[l + 1].nodes[m].text1;

								dm.conversations[l].nodes[m].audioEventName = dm.conversations[l + 1].nodes[m].audioEventName;

								dm.conversations[l].nodes[m].occupied = dm.conversations[l + 1].nodes[m].occupied;

								currentEditingNodeIsSaved[currentEditingNode[i]][m] = true;
							}

							dm.conversations[l].occupied = dm.conversations[l + 1].occupied;

						}
					}

					for (int d = 0; d < MAX_CONVERSATIONS && dm.conversations[d].occupied == true; d++)
					{
						currentEditingConversationName[d] = dm.conversations[d].conversationName;
					}

					dm.editorConversations[i].occupied = false;

					int isOccupiedNumTimesFalse2 = 0;

					for (int l = 0; (l < MAX_CONVERSATIONS) && (isOccupiedNumTimesFalse2 < 2); l++)
					{
						if (dm.editorConversations[l].occupied == false)
						{
							isOccupiedNumTimesFalse2++;
						}

						if (isOccupiedNumTimesFalse2 == 1)
						{
							dm.editorConversations[l].bubbleImagePath = dm.editorConversations[l + 1].bubbleImagePath;
							dm.editorConversations[l].characterImagePath = dm.editorConversations[l + 1].characterImagePath;
							dm.editorConversations[l].conversationName = dm.editorConversations[l + 1].conversationName;

							dm.editorConversations[l].isInOppositeSide = dm.editorConversations[l + 1].isInOppositeSide;
							dm.editorConversations[l].isRandom = dm.editorConversations[l + 1].isRandom;
							dm.editorConversations[l].detectsCharacter = dm.editorConversations[l + 1].detectsCharacter;
							dm.editorConversations[l].group.groupID = dm.editorConversations[l + 1].group.groupID;
							dm.editorConversations[l].group.order = dm.editorConversations[l + 1].group.order;
							dm.editorConversations[l].eventName = dm.editorConversations[l + 1].eventName;

							for (int m = 0; m < MAX_CONVERSATION_NODES; m++)
							{
								dm.editorConversations[l].nodes[m].text1 = dm.editorConversations[l + 1].nodes[m].text1;

								dm.editorConversations[l].nodes[m].audioEventName = dm.editorConversations[l + 1].nodes[m].audioEventName;

								dm.editorConversations[l].nodes[m].occupied = dm.editorConversations[l + 1].nodes[m].occupied;

								currentEditingNodeIsSaved[currentEditingNode[i]][m] = true;
							}

							dm.editorConversations[l].occupied = dm.editorConversations[l + 1].occupied;

						}
					}

					for (int d = 0; d < MAX_CONVERSATIONS && dm.editorConversations[d].occupied == true; d++)
					{
						currentEditingConversationName[d] = dm.editorConversations[d].conversationName;
					}

					currentEditingNode[i] = 0;
					dm.SaveAllDialogs();
				}
				ImGui::PopID();
			}
			ImGui::PopID();

			if (saved == 65)
			{
				currentEditingConversationName[savedID] = dm.editorConversations[savedID].conversationName;
				dm.conversations[savedID].conversationName = dm.editorConversations[savedID].conversationName;
				saved = 0;
			}
			else if (saved >= 1 && saved < 65) saved++;
		}
	}
	else
	{
		ImGui::TextWrapped("No conversations have been created yet.");

		ImGui::NewLine();
		ImGui::TextWrapped("WARNING:");
		ImGui::TextWrapped("If you did create conversations and they are not showing here, reload the engine.");

	}
	
	ImGui::End();
}