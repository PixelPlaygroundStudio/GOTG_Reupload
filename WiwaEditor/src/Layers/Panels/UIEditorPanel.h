#pragma once

#include "Panel.h"
#include <Wiwa/Ui/UiManager.h>
#include <Wiwa/scene/SceneManager.h>
#include "../../Utils/EditorUtils.h"
#include <Wiwa/Events/ApplicationEvent.h>


class UIEditorPanel : public Panel
{
public:
	UIEditorPanel(EditorLayer* instance);
	virtual ~UIEditorPanel();

	void Draw() override;

	int canvasSelectedID = -1;

	void DrawGameWindow();
	
	void GetSelectedCanvas();

	void DrawCanvasItems();

	void OpenEditGuiControl(Wiwa::GuiControl* control);

	void CleanInitialValues();
	void SetInitialValues(Wiwa::GuiControl* control);

	void UpdateElements(Wiwa::GuiControl* control,Wiwa::GuiControlType type);

	void CallbackElements(Wiwa::GuiControl* control);

	void AssetContainerPath();

	void AssetContainerExtraPath();
	void UpdateRotation(Wiwa::GuiControl* control);

	void OnEvent(Wiwa::Event& e) override;
	bool OnSceneChange(Wiwa::SceneChangeEvent& e);
	void VectorEdit(std::vector<Wiwa::Rect2i> list);

	int elementSelected;

	int m_GizmoType;

	//Variables to change
	int pos[2];
	int size[2];
	int originPos[2];
	int originSize[2];
	int extraOriginPos[2];
	int extraOriginSize[2];
	int priority;

	float rotation;
	size_t callbackID;
	std::string pathForAsset;
	std::string pathForExtraAsset;
	std::string audioEventForButton;
	std::string audioEventSelected;
	std::string audioEventFocused;
	std::string text;

	bool animated;
	float animSpeed = 0.0f;
	std::vector<Wiwa::Rect2i> animationRects;
	std::vector<Wiwa::Rect2i> empty;

};