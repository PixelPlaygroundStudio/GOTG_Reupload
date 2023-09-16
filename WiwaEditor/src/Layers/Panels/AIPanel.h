#pragma once

#include "Panel.h"
#include <glm/glm.hpp>
#include <Wiwa/ecs/components/Mesh.h>
#include <Wiwa/AI/Support/AI_BuildSettings.h>
#include <Wiwa/Events/ApplicationEvent.h>

typedef size_t EntityId;

class AIPanel : public Panel
{
public:
	AIPanel(EditorLayer* instance);
	virtual ~AIPanel();

	void Draw() override;
	void HandleSettings();
	void ResetSettings();
	void HandleDragAndDrop();
	void HandleDebugSettings();
	void HandleRender();
	void HandleCleanUp();
	void HandleSave();
	void HandleLoad();
	//void HandleCreationObj();
	//void HandleCreationRecast();

	//void HandleRenderRecast();
	void RefreshManager();

	void OnEvent(Wiwa::Event& e) override;
	bool OnSceneChange(Wiwa::SceneChangeEvent& e);
private:
	
	bool m_DebugDraw;
	EntityId m_Id = -1;
	//InputGeom* m_Geom;
	//BuildContext m_Ctx;
};