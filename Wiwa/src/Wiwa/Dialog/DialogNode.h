#pragma once
#include <vector>
#include <string>

#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/utilities/render/Text.h>

struct DialogNode
{
	// The dialog text lines
	Wiwa::Renderer2D::InstanceData text1_imgModeID;

	std::string text1;

	std::string audioEventName;

	bool occupied = false;
};

struct ConversationGroup
{
	
	std::string groupID = std::to_string(-1);
	std::string order = std::to_string(-1);
};