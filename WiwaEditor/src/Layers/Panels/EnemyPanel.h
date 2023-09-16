#pragma once

#include "Panel.h"
#include <glm/glm.hpp>

class EnemyPanel : public Panel
{
public:
	EnemyPanel(EditorLayer* instance);
	virtual ~EnemyPanel();

	void Init();
	void Draw() override;

	void DrawTables();
	void DrawEnemyStatsTable(const char* label, const char* enemy_str);
	void DrawInputs();
	void DrawVariantsTable();

	void DrawRangedData();
	void DrawSubjugatorData();
	void DrawUltronData();
	/*void OnEvent(Wiwa::Event& e) override;
	bool OnSceneChange(Wiwa::SceneChangeEvent& e);*/
};