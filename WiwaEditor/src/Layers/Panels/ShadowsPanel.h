#pragma once

#include "Panel.h"

class ShadowsPanel : public Panel 
{
public:
	ShadowsPanel(EditorLayer* instance);
	virtual ~ShadowsPanel();

	void Draw() override;
	void Update() override;
private:
	float m_ZoomLevel = 0.1f;
};
