#pragma once

#include "Panel.h"

class GameLogPanel : public Panel
{
public:
	GameLogPanel(EditorLayer* instance);
	virtual ~GameLogPanel();

	void Draw() override;

	void DrawStateInfo();

	void DrawRoomSpawnersInfo();

	void DrawRoomVariables();
};