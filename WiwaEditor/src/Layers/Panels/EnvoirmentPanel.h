#pragma once

#include "Panel.h"

class EnvironmentPanel : public Panel
{
public:
	EnvironmentPanel(EditorLayer* instance);
	virtual ~EnvironmentPanel();

	void Draw() override;
};