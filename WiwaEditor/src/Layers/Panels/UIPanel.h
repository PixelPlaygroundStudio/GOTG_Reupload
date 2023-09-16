#pragma once

#include "Panel.h"

#include <Wiwa/Ui/UiManager.h>
#include <Wiwa/Ui/UiCanvas.h>
#include <Wiwa/Ui/UiManager.h>
#include "Wiwa/Events/ApplicationEvent.h"
#include <glm/glm.hpp>

enum class GuiType
{
		BUTTON,
		TEXT,
		CHECKBOX,
		SLIDER,
		IMAGE,
		BAR,
		ABILITY,
		VIDEO
};
class UIPanel : public Panel
{
public:
	UIPanel(EditorLayer* instance);
	virtual ~UIPanel();

	void Draw() override;


	void OnEvent(Wiwa::Event& e) override;
	bool OnSceneChange(Wiwa::SceneChangeEvent& e);

	void DrawGuiElementSelection();

	void DrawGuiElementCreation(const char* current_item);

	GuiType GetSelectedElementType(const char* current_item);

	//SPECIFIC DRAWINGS FOR EACH TYPE
	void DrawButtonCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager);
	void DrawCheckboxCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager);
	void DrawSliderCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager);
	void DrawBarCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager);
	void DrawAbilityCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager);
	void DrawImageCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager);
	void DrawTextCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager);
	void DrawVideoCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager);

	void VectorEdit(std::vector<Wiwa::Rect2i> list);

	void SetSizeByTexture(const char* file);

	GuiType type;

	int canvasSelected = -1;
	int elementSelected = -1;
	glm::ivec2 position;
	glm::ivec2 size;
	float rotation;
	glm::ivec2 originPos;
	glm::ivec2 originSize;
	glm::ivec2 sliderOriginPos;
	glm::ivec2 sliderOriginSize;
	int callbackID;
	std::string pathForAsset;
	std::string pathForExtraAsset;
	std::string nameSavingWiGUI;
	std::string audioEventForButton;
	std::string audioEventFocused;
	std::string audioEventSelected;

	bool animated = false;
	float animSpeed = 0;
	std::vector<Wiwa::Rect2i> animationRects;
	EditorLayer* m_Instance;


	std::string currentWiPrefabScene;
};