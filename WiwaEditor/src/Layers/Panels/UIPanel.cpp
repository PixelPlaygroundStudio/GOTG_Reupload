#include <wipch.h>
#include "UIPanel.h"
#include "imgui.h"
#include <Wiwa/scene/SceneManager.h>
#include "Wiwa/scene/Scene.h"
#include <Wiwa/core/Application.h>
#include "../../Utils/EditorUtils.h"

using namespace Wiwa;

UIPanel::UIPanel(EditorLayer* instance)
	: Panel("UI", ICON_FK_TELEVISION, instance), m_Instance(instance)
{
	position = glm::ivec2(0.f);
	size = glm::ivec2(0.f);
	originPos = glm::ivec2(0.f);
	originSize = glm::ivec2(0.f);

	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();

	currentWiPrefabScene = gm.GetCurrentPrefabWiGui();
}

UIPanel::~UIPanel()
{
}

void UIPanel::Draw()
{
	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();

	ImGui::Begin(iconName.c_str(), &active);
	
	ImGui::Text("UI creator panel");
	ImGui::InputText("current wiPrefab", &currentWiPrefabScene);
	ImGui::NewLine();
	ImGui::PushID("SaveSceneWiGui");
	if (ImGui::Button("Save sceneWiGui"))
	{
		std::filesystem::path file = "assets/saved_wiGUI";
		file /= currentWiPrefabScene.c_str();
		file += ".wiGUI";
		gm.SetCurrentPrefabWiGui(file.string().c_str());
	}
	ImGui::PopID();
	ImGui::NewLine();
	ImGui::InputText("name for GUI", (char*)nameSavingWiGUI.c_str(), 64);
	ImGui::NewLine();
	ImGui::PushID("saveGUI");
	if (ImGui::Button("Save GUI"))
	{
		std::filesystem::path file = "assets/saved_wiGUI";
		if (!std::filesystem::exists(file))
		{
			std::filesystem::create_directory(file);
		}
		file /= nameSavingWiGUI.c_str();
		file += ".wiGUI";
		gm.SaveWiUI(file.string().c_str());
	}
	ImGui::PopID();
	if (ImGui::CollapsingHeader("Canvas creation"))
	{
		if (ImGui::Button("Create Canvas"))
		{
			gm.CreateGuiCanvas(gm.canvas.size(), true);
			//m_GuiManager.CreateGuiControl_Simple(GuiControlType::BUTTON, 0, { 200,200,100,100 }, "assets/hola.png", nullptr, 0,1);

		}
		for (size_t i = 0; i < gm.canvas.size(); i++)
		{
 			ImGui::PushID((int)i);
			ImGui::Text("Canvas %i", i);
			ImGui::SameLine();
			if(ImGui::Button("Select canvas"))
			{
				canvasSelected = i;
				gm.SwapSelectedCanvas(gm.canvas.at(i));
			}
			ImGui::SameLine();
			if(ImGui::Button("Swap active"))
			{
				gm.canvas.at(i)->SwapActive();
			}
			ImGui::SameLine();
			if(ImGui::Button("Delete"))
			{
				gm.DestroyCanvas(gm.canvas.at(i));
				canvasSelected = -1;
			}
			ImGui::PopID();
		}
	}
	DrawGuiElementSelection();
	
	ImGui::End();
}

void UIPanel::DrawGuiElementSelection()
{
	const char* items[] = {"Button", "Slider", "CheckBox", "Image","Text","Bar","Ability","Video"};
	static const char* current_item = NULL;
	if (ImGui::CollapsingHeader("Create UI element"))
	{
		ImGui::Text("UI element type:");

		if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(items[n], is_selected))
				{
					current_item = items[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
			}
			ImGui::EndCombo();
		}
		if (current_item != NULL)
		{
			if (ImGui::CollapsingHeader(current_item))
			{
				DrawGuiElementCreation(current_item);
			}
		}
		else
		{
			TextCentered("Select an item");
		}
	}

}

void UIPanel::DrawGuiElementCreation(const char* current_item)
{
	type = GetSelectedElementType(current_item);

	switch (type)
	{
	case GuiType::BUTTON:
		DrawButtonCreation(canvasSelected, Wiwa::SceneManager::getActiveScene()->GetGuiManager());
		
		break;
	case GuiType::TEXT:
		DrawTextCreation(canvasSelected, Wiwa::SceneManager::getActiveScene()->GetGuiManager());
		break;
	case GuiType::CHECKBOX:
		DrawCheckboxCreation(canvasSelected, Wiwa::SceneManager::getActiveScene()->GetGuiManager());
		break;
	case GuiType::SLIDER:
		DrawSliderCreation(canvasSelected, Wiwa::SceneManager::getActiveScene()->GetGuiManager());
		break;
	case GuiType::BAR:
		DrawBarCreation(canvasSelected, Wiwa::SceneManager::getActiveScene()->GetGuiManager());
		break;
	case GuiType::IMAGE:
		DrawImageCreation(canvasSelected, Wiwa::SceneManager::getActiveScene()->GetGuiManager());
		break;
	case GuiType::ABILITY:
		DrawAbilityCreation(canvasSelected, Wiwa::SceneManager::getActiveScene()->GetGuiManager());
		break;
	case GuiType::VIDEO:
		DrawVideoCreation(canvasSelected, Wiwa::SceneManager::getActiveScene()->GetGuiManager());
		break;
	default:
		break;
	}
}

GuiType UIPanel::GetSelectedElementType(const char* current_item)
{
	if (current_item == "Button")
	{
		return GuiType::BUTTON;
	}
	if (current_item == "Slider")
	{
		return GuiType::SLIDER;
	}
	if (current_item == "CheckBox")
	{
		return GuiType::CHECKBOX;
	}
	if (current_item == "Image")
	{
		return GuiType::IMAGE;
	}
	if (current_item == "Text")
	{
		return GuiType::TEXT;
	}
	if (current_item == "Bar")
	{
		return GuiType::BAR;
	}
	if (current_item == "Ability")
	{
		return GuiType::ABILITY;
	}
	if (current_item == "Video")
	{
		return GuiType::VIDEO;
	}

	return GuiType::TEXT;
}



void UIPanel::DrawButtonCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager)
{
	ImGui::PushID("CreateButton");
	if (canvas_id > -1)ImGui::Text("Canvas: %i", canvas_id);
	if (canvas_id < 0)ImGui::Text("Please select a canvas");
	ImGui::DragInt2("Origin position", glm::value_ptr(originPos));
	ImGui::DragInt2("Origin size", glm::value_ptr(originSize));
	ImGui::DragInt2("Position", glm::value_ptr(position));
	ImGui::DragInt2("Size", glm::value_ptr(size));
	ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.f, "%.1f");
	AssetContainer(pathForAsset.c_str());
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
				pathForAsset = pathS;
				SetSizeByTexture(pathS.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}
	
	static size_t current_item = WI_INVALID_INDEX;

	Wiwa::Application& app = Wiwa::Application::Get();

	size_t cbcount = app.GetCallbacksCount();

	if (current_item == WI_INVALID_INDEX) {
		for (size_t i = 0; i < cbcount; i++) {
			const Func* cb = app.getCallbackAt(i);

			if (cb->params.size() == 1 && cb->params.at(0).hash == FNV1A_HASH("void")) {
				current_item = i;
			}
		}
	}

	if (cbcount > 0) {
		const Func* current_cb = app.getCallbackAt(current_item);

		ImGui::Text("Callback type:");

		if (ImGui::BeginCombo("##combo", current_cb->name.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (size_t n = 0; n < cbcount; n++)
			{
				bool is_selected = n == current_item; // You can store your selection however you want, outside or inside your objects
				current_cb = app.getCallbackAt(n);
				if (current_cb->params.size() == 1 && current_cb->params.at(0).hash == FNV1A_HASH("void")) {
					if (ImGui::Selectable(current_cb->name.c_str(), is_selected))
					{
						current_item = n;
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	ImGui::Text("Animations");
	ImGui::Checkbox("animated", &animated);
	ImGui::InputFloat("Animation speed", &animSpeed);
	VectorEdit(animationRects);
	callbackID = current_item;
	ImGui::InputText("Audio event name", (char*)audioEventForButton.c_str(), 64);
	if (ImGui::Button("Create button"))
	{
		Wiwa::Rect2i rect;
		rect.x = position.x;
		rect.y = position.y;
		rect.width = size.x;
		rect.height = size.y;
		Wiwa::Rect2i originRect;
		originRect.x = originPos.x;
		originRect.y = originPos.y;
		originRect.width = originSize.x;
		originRect.height = originSize.y;
		if (canvas_id > -1)
		{
			m_GuiManager.CreateGuiControl_Simple(GuiControlType::BUTTON, m_GuiManager.canvas.at(canvas_id)->controls.size(), rect, pathForAsset.c_str(), nullptr, canvas_id, callbackID, originRect, audioEventForButton.c_str(),audioEventFocused.c_str(),audioEventSelected.c_str(),true, animated, animSpeed, animationRects, rotation);
		}
	}

	ImGui::PopID();
}
void UIPanel::DrawSliderCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager)
{
	ImGui::PushID("CreateSlider");
	if (canvas_id > -1)ImGui::Text("Canvas: %i", canvas_id);
	if (canvas_id < 0)ImGui::Text("Please select a canvas");
	ImGui::DragInt2("Origin position", glm::value_ptr(originPos));
	ImGui::DragInt2("Origin size", glm::value_ptr(originSize));
	ImGui::DragInt2("Slider origin position", glm::value_ptr(sliderOriginPos));
	ImGui::DragInt2("Slider origin size", glm::value_ptr(sliderOriginSize));
	ImGui::DragInt2("Position", glm::value_ptr(position));
	ImGui::DragInt2("Size", glm::value_ptr(size));
	ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.f, "%.1f");
	AssetContainer(pathForAsset.c_str());
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
				pathForAsset = pathS;
				SetSizeByTexture(pathS.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}
	AssetContainer(pathForExtraAsset.c_str());
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
				pathForExtraAsset = pathS;

			}
		}

		ImGui::EndDragDropTarget();
	}

	static size_t current_item = WI_INVALID_INDEX;

	Wiwa::Application& app = Wiwa::Application::Get();

	size_t cbcount = app.GetCallbacksCount();

	if (current_item == WI_INVALID_INDEX) {
		for (size_t i = 0; i < cbcount; i++) {
			const Func* cb = app.getCallbackAt(i);

			if (cb->params.size() == 1 && cb->params.at(0).hash == FNV1A_HASH("float")) {
				current_item = i;
				
			}
		}
	}

	if (cbcount > 0) {
		const Func* current_cb = app.getCallbackAt(current_item);

		ImGui::Text("Callback type:");

		if (ImGui::BeginCombo("##combo", current_cb->name.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (size_t n = 0; n < cbcount; n++)
			{
				bool is_selected = n == current_item; // You can store your selection however you want, outside or inside your objects
				current_cb = app.getCallbackAt(n);
				if (current_cb->params.size() == 1 && current_cb->params.at(0).hash == FNV1A_HASH("float")) {
					if (ImGui::Selectable(current_cb->name.c_str(), is_selected))
					{
						current_item = n;
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	ImGui::InputText("Audio event name", (char*)audioEventForButton.c_str(), 64);
	callbackID = current_item;

	if (ImGui::Button("Create slider"))
	{
		Wiwa::Rect2i rect;
		rect.x = position[0];
		rect.y = position[1];
		rect.width = size[0];
		rect.height = size[1];
		Wiwa::Rect2i rect2;
		rect2.x = rect.x;
		rect2.y = rect.y;
		rect2.width = (rect.width / 100);
		rect2.height = rect.height;

		Wiwa::Rect2i originRect;
		originRect.x = originPos[0];
		originRect.y = originPos[1];
		originRect.width = originSize[0];
		originRect.height = originSize[1];

		Wiwa::Rect2i sliderOriginRect;
		sliderOriginRect.x = sliderOriginPos[0];
		sliderOriginRect.y = sliderOriginPos[1];
		sliderOriginRect.width = sliderOriginSize[0];
		sliderOriginRect.height = sliderOriginSize[1];
		if (canvas_id > -1) m_GuiManager.CreateGuiControl(GuiControlType::SLIDER, m_GuiManager.canvas.at(canvas_id)->controls.size(), rect, pathForAsset.c_str(), pathForExtraAsset.c_str(),rect2, canvas_id, callbackID,originRect,sliderOriginRect, audioEventForButton.c_str(), true, rotation);
	}

	ImGui::PopID();
}
void UIPanel::DrawBarCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager)
{
	ImGui::PushID("CreateBar");
	if (canvas_id > -1)ImGui::Text("Canvas: %i", canvas_id);
	if (canvas_id < 0)ImGui::Text("Please select a canvas");
	ImGui::DragInt2("Origin position", glm::value_ptr(originPos));
	ImGui::DragInt2("Origin size", glm::value_ptr(originSize));
	ImGui::DragInt2("Position", glm::value_ptr(position));
	ImGui::DragInt2("Size", glm::value_ptr(size));
	ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.f, "%.1f");
	AssetContainer(pathForAsset.c_str());
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
				pathForAsset = pathS;
				SetSizeByTexture(pathS.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}
	if (ImGui::Button("Create bar"))
	{
		Wiwa::Rect2i rect;
		rect.x = position[0];
		rect.y = position[1];
		rect.width = size[0];
		rect.height = size[1];
		Wiwa::Rect2i rect2;
		rect2.x = rect.x;
		rect2.y = rect.y;
		rect2.width = rect.width;
		rect2.height = rect.height;

		Wiwa::Rect2i originRect;
		originRect.x = originPos[0];
		originRect.y = originPos[1];
		originRect.width = originSize[0];
		originRect.height = originSize[1];

		Wiwa::Rect2i sliderOriginRect;
		sliderOriginRect.x = originPos[0];
		sliderOriginRect.y = originPos[1];
		sliderOriginRect.width = originSize[0];
		sliderOriginRect.height = originSize[1];
		if (canvas_id > -1) m_GuiManager.CreateGuiControl(GuiControlType::BAR, m_GuiManager.canvas.at(canvas_id)->controls.size(), rect, pathForAsset.c_str(), pathForExtraAsset.c_str(), rect2, canvas_id, 0, originRect, sliderOriginRect, audioEventForButton.c_str(), true, rotation);
	}

	ImGui::PopID();
}
void UIPanel::DrawAbilityCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager)
{
	ImGui::PushID("CreateAbility");
	if (canvas_id > -1)ImGui::Text("Canvas: %i", canvas_id);
	if (canvas_id < 0)ImGui::Text("Please select a canvas");
	ImGui::DragInt2("Origin position", glm::value_ptr(originPos));
	ImGui::DragInt2("Origin size", glm::value_ptr(originSize));
	ImGui::DragInt2("Position", glm::value_ptr(position));
	ImGui::DragInt2("Size", glm::value_ptr(size));
	ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.f, "%.1f");
	AssetContainer(pathForAsset.c_str());
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
				pathForAsset = pathS;
				SetSizeByTexture(pathS.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}

	static size_t current_item = WI_INVALID_INDEX;

	Wiwa::Application& app = Wiwa::Application::Get();

	size_t cbcount = app.GetCallbacksCount();

	if (current_item == WI_INVALID_INDEX) {
		for (size_t i = 0; i < cbcount; i++) {
			const Func* cb = app.getCallbackAt(i);

			if (cb->params.size() == 0) {
				current_item = i;
			}
		}
	}

	callbackID = WI_INVALID_INDEX;
	ImGui::Text("Animations");
	ImGui::Checkbox("animated", &animated);
	VectorEdit(animationRects);

	if (ImGui::Button("Create Ability"))
	{
		Wiwa::Rect2i rect;
		rect.x = position[0];
		rect.y = position[1];
		rect.width = size[0];
		rect.height = size[1];
		Wiwa::Rect2i originRect;
		originRect.x = originPos[0];
		originRect.y = originPos[1];
		originRect.width = originSize[0];
		originRect.height = originSize[1];
		if (canvas_id > -1) m_GuiManager.CreateGuiControl_Ability(GuiControlType::ABILITY, m_GuiManager.canvas.at(canvas_id)->controls.size(),canvas_id,rect, pathForAsset.c_str(),callbackID,originRect,active,animated,animationRects, rotation);
	}

	ImGui::PopID();
}
void UIPanel::DrawImageCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager)
{
	ImGui::PushID("CreateImage");
	if (canvas_id > -1)ImGui::Text("Canvas: %i", canvas_id);
	if (canvas_id < 0)ImGui::Text("Please select a canvas");
	ImGui::DragInt2("Origin position", glm::value_ptr(originPos));
	ImGui::DragInt2("Origin size", glm::value_ptr(originSize));
	ImGui::DragInt2("Position", glm::value_ptr(position));
	ImGui::DragInt2("Size", glm::value_ptr(size));
	ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.f, "%.1f");
	AssetContainer(pathForAsset.c_str());
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
				pathForAsset = pathS;
				SetSizeByTexture(pathS.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}

	callbackID = WI_INVALID_INDEX;

	ImGui::Text("Animations");
	ImGui::Checkbox("animated", &animated);
	ImGui::InputFloat("Animation speed", &animSpeed);
	VectorEdit(animationRects);

	if (ImGui::Button("Create Image"))
	{
		Wiwa::Rect2i rect;
		rect.x = position[0];
		rect.y = position[1];
		rect.width = size[0];
		rect.height = size[1];
		Wiwa::Rect2i originRect;
		originRect.x = originPos[0];
		originRect.y = originPos[1];
		originRect.width = originSize[0];
		originRect.height = originSize[1];
		if (canvas_id > -1) m_GuiManager.CreateGuiControl_Simple(GuiControlType::IMAGE, m_GuiManager.canvas.at(canvas_id)->controls.size(), rect, pathForAsset.c_str(), nullptr, canvas_id, callbackID,originRect, audioEventForButton.c_str(),audioEventFocused.c_str(),audioEventSelected.c_str(), true,animated,animSpeed, animationRects, rotation);
	}

	ImGui::PopID();
}
void UIPanel::DrawTextCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager)
{

	ImGui::PushID("CreateText");
	if (canvas_id > -1)ImGui::Text("Canvas: %i", canvas_id);
	if (canvas_id < 0)ImGui::Text("Please select a canvas");
	ImGui::DragInt2("Position", glm::value_ptr(position));
	ImGui::DragInt2("Size", glm::value_ptr(size));
	ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.f, "%.1f");
	ImGui::InputText("String",&pathForAsset);

	if (ImGui::Button("Create Text"))
	{
		Wiwa::Rect2i rect;
		rect.x = position[0];
		rect.y = position[1];
		rect.width = size[0];
		rect.height = size[1];
		if (canvas_id > -1)
			m_GuiManager.CreateGuiControl_Text(GuiControlType::TEXT, m_GuiManager.canvas.at(canvas_id)->controls.size(), rect, pathForAsset.c_str(),canvas_id, true, rotation);
	}
	ImGui::PopID();
}
void UIPanel::DrawVideoCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager)
{
	ImGui::PushID("CreateVideo");
	if (canvas_id > -1)ImGui::Text("Canvas: %i", canvas_id);
	if (canvas_id < 0)ImGui::Text("Please select a canvas");
	ImGui::DragInt2("Position", glm::value_ptr(position));
	ImGui::DragInt2("Size", glm::value_ptr(size));

	AssetContainer(pathForAsset.c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".mp4")
			{
				pathForAsset = pathS;
				SetSizeByTexture(pathS.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}

	static size_t current_item = WI_INVALID_INDEX;

	Wiwa::Application& app = Wiwa::Application::Get();

	size_t cbcount = app.GetCallbacksCount();

	if (current_item == WI_INVALID_INDEX) {
		for (size_t i = 0; i < cbcount; i++) {
			const Func* cb = app.getCallbackAt(i);

			if (cb->params.size() == 1 && cb->params.at(0).hash == FNV1A_HASH("void")) {
				current_item = i;
			}
		}
	}

	if (cbcount > 0) {
		const Func* current_cb = app.getCallbackAt(current_item);

		ImGui::Text("Callback type:");

		if (ImGui::BeginCombo("##combo", current_cb->name.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (size_t n = 0; n < cbcount; n++)
			{
				bool is_selected = n == current_item; // You can store your selection however you want, outside or inside your objects
				current_cb = app.getCallbackAt(n);

				if (current_cb->params.size() == 1 && current_cb->params.at(0).hash == FNV1A_HASH("void")) {
					if (ImGui::Selectable(current_cb->name.c_str(), is_selected))
					{
						current_item = n;
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
					}
				}

			}
			ImGui::EndCombo();
		}
	}

	callbackID = current_item;

	if (ImGui::Button("Create video"))
	{
		Wiwa::Rect2i rect;
		rect.x = position[0];
		rect.y = position[1];
		rect.width = size[0];
		rect.height = size[1];
		if (canvas_id > -1)
			m_GuiManager.CreateGuiControl_Video(GuiControlType::VIDEO, m_GuiManager.canvas.at(canvas_id)->controls.size(), canvas_id, rect, pathForAsset.c_str(),true,callbackID);
	}
	ImGui::PopID();
}
void UIPanel::DrawCheckboxCreation(int canvas_id, Wiwa::GuiManager& m_GuiManager)
{
	ImGui::PushID("CreateCheckbox");
	if (canvas_id > -1)ImGui::Text("Canvas: %i", canvas_id);
	if (canvas_id < 0)ImGui::Text("Please select a canvas");
	ImGui::DragInt2("Origin position", glm::value_ptr(originPos));
	ImGui::DragInt2("Origin size", glm::value_ptr(originSize));
	ImGui::DragInt2("Position", glm::value_ptr(position));
	ImGui::DragInt2("Size", glm::value_ptr(size));
	ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.f, "%.1f");
	AssetContainer(pathForAsset.c_str());
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
				pathForAsset = pathS;
				SetSizeByTexture(pathS.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}
	static size_t current_item = WI_INVALID_INDEX;

	Wiwa::Application& app = Wiwa::Application::Get();

	size_t cbcount = app.GetCallbacksCount();

	if (current_item == WI_INVALID_INDEX) {
		for (size_t i = 0; i < cbcount; i++) {
			const Func* cb = app.getCallbackAt(i);

			if (cb->params.size() == 1 && cb->params.at(0).hash == FNV1A_HASH("bool")) {
				
				current_item = i;
				
			}
		}
	}

	if (cbcount > 0) {
		const Func* current_cb = app.getCallbackAt(current_item);

		ImGui::Text("Callback type:");

		if (ImGui::BeginCombo("##combo", current_cb->name.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (size_t n = 0; n < cbcount; n++)
			{
				bool is_selected = n == current_item; // You can store your selection however you want, outside or inside your objects
				current_cb = app.getCallbackAt(n);
				if (current_cb->params.size() == 1 &&  current_cb->params.at(0).hash == FNV1A_HASH("bool")) {
					
					if (ImGui::Selectable(current_cb->name.c_str(), is_selected))
					{
						current_item = n;
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
					}
					
				}
			}
			ImGui::EndCombo();
		}
	}

	callbackID = (int)current_item;
	ImGui::InputText("Audio event name", (char*)audioEventForButton.c_str(), 64);

	ImGui::Text("Animations");
	ImGui::Checkbox("animated", &animated);
	ImGui::InputFloat("Animation speed", &animSpeed);
	VectorEdit(animationRects);

	if (ImGui::Button("Create checkbox"))
	{
		Wiwa::Rect2i rect;
		rect.x = position[0];
		rect.y = position[1];
		rect.width = size[0];
		rect.height = size[1];
		Wiwa::Rect2i originRect;
		originRect.x = originPos[0];
		originRect.y = originPos[1];
		originRect.width = originSize[0];
		originRect.height = originSize[1];
		const char* audioEvent = audioEventForButton.c_str();
		if (canvas_id > -1) m_GuiManager.CreateGuiControl_Simple(GuiControlType::CHECKBOX, m_GuiManager.canvas.at(canvas_id)->controls.size(), rect, pathForAsset.c_str(), pathForExtraAsset.c_str(), canvas_id, callbackID, originRect, audioEvent,audioEventFocused.c_str(),audioEventSelected.c_str(), true, animated, animSpeed, animationRects, rotation);
	}

	ImGui::PopID();
}

void UIPanel::VectorEdit(std::vector<Wiwa::Rect2i> list)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		ImGui::PushID(i);
		ImGui::Text("Animation frame: %d", i);
		ImGui::InputInt("AnimPos_x", &list.at(i).x);
		ImGui::InputInt("AnimPos_y", &list.at(i).y);
		ImGui::InputInt("AnimPos_width", &list.at(i).width);
		ImGui::InputInt("AnimPos_height", &list.at(i).height);
		ImGui::PopID();
	}
	if (ImGui::Button("+"))
	{
		list.push_back({0,0,0,0});
	}
	ImGui::SameLine();
	if (ImGui::Button("-"))
	{
		if (list.size() > 0)list.erase(list.begin() + list.size() - 1);
	}
	
	animationRects = list;
}
void UIPanel::SetSizeByTexture(const char* file)
{
	/*const ResourceId id = Resources::Load<Image>(file);
	Image* img = Resources::GetResourceById<Image>(id);
	size.x = img->GetSize().x;
	size.y = img->GetSize().y;
	originSize.x = img->GetSize().x;
	originSize.y = img->GetSize().y;*/
}
void UIPanel::OnEvent(Wiwa::Event& e)
{
	Wiwa::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<SceneChangeEvent>({ &UIPanel::OnSceneChange, this });

}

bool UIPanel::OnSceneChange(SceneChangeEvent& e)
{
	canvasSelected = -1;
	elementSelected = -1;
	position[0] = 0; position[1] = 0;
	size[0] = 0; size[1] = 0;
	originPos[0] = 0; originPos[1] = 0;
	originSize[0] = 0; originSize[1] = 0;
	sliderOriginPos[0] = 0; sliderOriginPos[1] = 0;
	sliderOriginSize[0] = 0; sliderOriginSize[1] = 0;
	callbackID = WI_INVALID_INDEX;
	pathForAsset = "";
	pathForExtraAsset = "";
	nameSavingWiGUI = "";
	audioEventForButton = "";
	return true;
}