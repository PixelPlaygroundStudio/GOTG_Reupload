#include <wipch.h>
#include "UIEditorPanel.h"
#include <unordered_map>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/core/Application.h>
#include <Wiwa/utilities/render/Text.h>
#include <ImGuizmo.h>
#include <glm/gtx/quaternion.hpp>

#include "../EditorLayer.h"

UIEditorPanel::UIEditorPanel(EditorLayer* instance)
	: Panel("UI editor", ICON_FK_MAGIC, instance)
{
	elementSelected = -1;
}

UIEditorPanel::~UIEditorPanel()
{

}

void UIEditorPanel::Draw()
{
	ImGui::Begin(iconName.c_str(), &active);
	if(ImGui::BeginTable("##ui_editor", 2, ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextColumn();
		ImGui::Text("UI editor panel");

		if (ImGui::Button(ICON_FK_ARROWS))
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		ImGui::SameLine();

		if (ImGui::Button(ICON_FK_EXPAND))
			m_GizmoType = ImGuizmo::OPERATION::SCALE;

		if (ImGui::Button(ICON_FK_CIRCLE))
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		
		GetSelectedCanvas();
		if (ImGui::CollapsingHeader("Edit canvas"))
		{
		
			if(canvasSelectedID < 0)ImGui::Text("Select a canvas to edit");
			if (canvasSelectedID > -1) {
				ImGui::Text("Canvas: %i", canvasSelectedID);
				ImGui::NewLine();
				DrawCanvasItems();
			
			}
		}
		ImGui::TableNextColumn();
		DrawGameWindow();
		
		ImGui::EndTable();
	}
	ImGui::End();
}
// Convert 2D screen coordinates to 2D world positions
glm::vec2 screenToWorld(glm::vec2 screenPos, glm::mat4 invProjMatrix, int screenWidth, int screenHeight) {
	// Convert screen coordinates to NDC
	float ndc_x = 2 * screenPos.x / screenWidth - 1;
	float ndc_y = 1 - 2 * screenPos.y / screenHeight;

	// Apply inverse projection matrix to get world position
	glm::vec4 ndcPos(ndc_x, ndc_y, 1, 1);
	glm::vec4 worldPos = invProjMatrix * ndcPos;
	return glm::vec2(worldPos.x / worldPos.w, worldPos.y / worldPos.w);
}

// Convert 2D world positions to 2D screen coordinates
glm::ivec2 worldToScreen(glm::vec2 worldPos, glm::mat4 projMatrix, int screenWidth, int screenHeight) {
	// Apply projection matrix to get NDC coordinates
	glm::vec4 worldPosHomog(worldPos.x, worldPos.y, 0, 1);
	glm::vec4 ndcPos = projMatrix * worldPosHomog;

	// Convert NDC coordinates to screen coordinates
	int screen_x = static_cast<int>((ndcPos.x + 1) / 2 * screenWidth);
	int screen_y = static_cast<int>((1 - ndcPos.y) / 2 * screenHeight);

	return glm::ivec2(screen_x, screen_y);
}
void UIEditorPanel::DrawGameWindow()
{
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

	Wiwa::Size2i resolution = Wiwa::Application::Get().GetTargetResolution();
	float ar = resolution.w / (float)resolution.h;
	Wiwa::Size2f scales = { viewportPanelSize.x / (float)resolution.w, viewportPanelSize.y / (float)resolution.h };

	float scale = scales.x < scales.y ? scales.x : scales.y;
	ImVec2 isize = { resolution.w * scale, resolution.h * scale };
	
	ImTextureID tex = (ImTextureID)(intptr_t)Wiwa::RenderManager::getColorTexture();
	
	ImGui::Image(tex, isize, ImVec2(0, 1), ImVec2(1, 0));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS;
			if (p.extension() == ".wiscene")
			{
				EditorLayer::Get().LoadScene(pathS);
				Wiwa::SceneChangeEvent event(0);
				Action<Wiwa::Event&> action = { &Wiwa::Application::OnEvent, &Wiwa::Application::Get() };
				action(event);
			}
			else if (p.extension() == ".wiprefab")
			{
				Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
				em.LoadPrefab(pathS.c_str());
			}
			else if (p.extension() == ".wiGUI")
			{
				Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
				gm.LoadWiUI(pathS.c_str());
			}
		}


		ImGui::EndDragDropTarget();
	}
	//if (elementSelected != -1)
	//{
	//	ImGuizmo::SetOrthographic(false);
	//	ImGuizmo::SetDrawlist();
	//	const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

	//	const Wiwa::Size2i resolution = Wiwa::Application::Get().GetTargetResolution();
	//	float ar = resolution.w / (float)resolution.h;
	//	const Wiwa::Size2f scales = { viewportPanelSize.x / (float)resolution.w, viewportPanelSize.y / (float)resolution.h };

	//	const float scale = scales.x < scales.y ? scales.x : scales.y;
	//	
	//	const ImVec2 rectPos = ImGui::GetItemRectMin();
	//	const ImVec2 isize = { resolution.w * scale, resolution.h * scale };
	//	ImGuizmo::SetRect(rectPos.x, rectPos.y, isize.x, isize.y);
	//	
	//	glm::mat4 cameraView = Wiwa::SceneManager::getActiveScene()->GetCameraManager().getActiveCamera()->getView();
	//	
	//	const glm::mat4& cameraProjection = glm::ortho(0.0f, (float)resolution.w, (float)resolution.h, 0.0f, -1.0f, 1.0f);

	//	
	//	//Snaping
	//	
	//	bool snap = Wiwa::Input::IsKeyRepeat(Wiwa::Key::LeftControl);
	//	
	//	float snapValue = 0.5f; //Snap to 0.5m for translation/scale
	//	
	//	float snapValues[3] = { snapValue, snapValue, snapValue };
	//	
	//	glm::vec2 position = screenToWorld(glm::vec2(pos[0], pos[1]), glm::inverse(cameraProjection), resolution.w, resolution.h);
	//	glm::mat4 tmpMat =
	//	glm::translate(glm::mat4(1.f), glm::vec3(position.x, position.y, 0.f)) *
	//		glm::toMat4(glm::quat(glm::vec3(0.f, 0.f, 0.f))) *
	//			glm::scale(glm::mat4(1.f), glm::vec3(size[0], size[1], 0.f));


	//	
	//	ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
	//	
	//		(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(tmpMat),
	//	
	//		nullptr, snap ? snapValues : nullptr);
	//	
	//	if (ImGuizmo::IsUsing())
	//	
	//	{
	//	
	//		float translation[3], rotation[3], scale[3];
	//	
	//		ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(tmpMat), translation, rotation, scale);

	//	
	//		switch (m_GizmoType)
	//	
	//		{
	//	
	//		case ImGuizmo::OPERATION::TRANSLATE:
	//	
	//			{
	//	
	//				
	//	
	//			}break;
	//	
	//		case ImGuizmo::OPERATION::SCALE:
	//	
	//			{
	//	
	//				
	//	
	//			}break;
	//	
	//		default:
	//	
	//			break;
	//	
	//		}
	//	
	//	}
	//}
	//ControlGuizmos();
}

void UIEditorPanel::GetSelectedCanvas()
{
	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();

	for (size_t i = 0; i < gm.canvas.size(); i++)
	{
		if (gm.canvas.at(i)->selected)
		{
			canvasSelectedID = (int)i;
		}
	}

	if (gm.canvas.size() <= 0) canvasSelectedID = -1;
}

void UIEditorPanel::DrawCanvasItems()
{
	Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
	Wiwa::GuiCanvas* canvas = gm.canvas.at(canvasSelectedID);
	size_t controls_size = canvas->controls.size();
	for (size_t i = 0; i < controls_size; i++)
	{
		ImGui::Text(canvas->controls.at(i)->name.c_str());
		ImGui::SameLine();
		ImGui::Text("%i", i);
		ImGui::SameLine();
		ImGui::PushID(i);
		if (ImGui::Button("Edit"))
		{
			CleanInitialValues();
			elementSelected = (int)i;
			SetInitialValues(canvas->controls.at(i));
		}
		ImGui::SameLine();
		if (ImGui::Button("Activate"))
		{
			canvas->controls.at(i)->SwapActive(Wiwa::Application::Get().GetRenderer2D());
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			gm.DestroyGuiControl(canvas->controls.at(i), canvas);
			elementSelected = -1;
		}
		ImGui::PopID();
	}
	if(elementSelected > -1)OpenEditGuiControl(canvas->controls.at(elementSelected));
	
	
}

void UIEditorPanel::CleanInitialValues()
{
	pos[0] = 0;
	pos[1] = 0;
	size[0] = 0;
	size[1] = 0;
	originPos[0] = 0;
	originPos[1] = 0;
	originSize[0] = 0;
	originSize[1] = 0;
	priority = 0;
	callbackID = WI_INVALID_INDEX;
	audioEventForButton = "";
	animated = false;
	rotation = 0.0f;
	
	animSpeed = 0.0f;
	animationRects = empty;
	

	extraOriginPos[0] = 0;
	extraOriginPos[1] = 0;
	extraOriginSize[0] = 0;
	extraOriginSize[1] = 0;

	pathForAsset = "";
	pathForExtraAsset = "";
	text = "";
}

void UIEditorPanel::SetInitialValues(Wiwa::GuiControl* control)
{
	pos[0] = control->position.x;
	pos[1] = control->position.y;
	size[0] = control->position.width;
	size[1] = control->position.height;
	originPos[0] = control->texturePosition.x;
	originPos[1] = control->texturePosition.y;
	originSize[0] = control->texturePosition.width;
	originSize[1] = control->texturePosition.height;
	callbackID = control->callbackID;
	audioEventForButton = control->audioEventForButton;
	animated = control->animatedControl;
	rotation = control->rotation;
	if (animated)
	{
		animSpeed = control->animSpeed;
		animationRects = control->positionsForAnimations;
	}
	if (control->type == Wiwa::GuiControlType::SLIDER)
	{
		extraOriginPos[0] = control->extraTexturePosition.x;
		extraOriginPos[1] = control->extraTexturePosition.y;
		extraOriginSize[0] = control->extraTexturePosition.width;
		extraOriginSize[1] = control->extraTexturePosition.height;
	}
	text = control->text;
	if(control->type != Wiwa::GuiControlType::VIDEO) pathForAsset = Wiwa::Resources::getResourcePathById<Wiwa::Image>(control->textId1);
	if(control->type == Wiwa::GuiControlType::VIDEO)  pathForAsset = Wiwa::Resources::getResourcePathById<Wiwa::Video>(control->textId1);
	pathForExtraAsset = Wiwa::Resources::getResourcePathById<Wiwa::Image>(control->textId2);
}
void UIEditorPanel::OpenEditGuiControl(Wiwa::GuiControl* control)
{
	if (ImGui::CollapsingHeader("Edit control"))
	{
		ImGui::Text(control->name.c_str());
		ImGui::SameLine();
		ImGui::Text("%i", control->id);
		ImGui::NewLine();
		if(ImGui::DragInt2("position", pos))
		{
			UpdateElements(control,control->type);
		}
		if (ImGui::DragInt("priority", &priority))
		{
			UpdateElements(control, control->type);
		}
		if(ImGui::DragInt2("size", size))
		{
			UpdateElements(control,control->type);
		}
		if (ImGui::SliderFloat("rotation", &rotation, 0.0f, 360.f, "%.1f"))
		{
			UpdateElements(control,control->type);
		}

		switch (control->type)
		{
		case Wiwa::GuiControlType::ABILITY:
			ImGui::DragInt2("origin position", originPos);
			ImGui::DragInt2("origin size", originSize);
			ImGui::Text("Animations");
			ImGui::Checkbox("animated", &animated);
			ImGui::InputFloat("Animation speed", &animSpeed);
			VectorEdit(animationRects);
			AssetContainerPath();
			break;
		case Wiwa::GuiControlType::BAR:
			ImGui::DragInt2("origin position", originPos);
			ImGui::DragInt2("origin size", originSize);
			ImGui::Text("Animations");
			ImGui::Checkbox("animated", &animated);
			ImGui::InputFloat("Animation speed", &animSpeed);
			VectorEdit(animationRects);
			AssetContainerPath();
			break;
		case Wiwa::GuiControlType::BUTTON:
			ImGui::DragInt2("origin position", originPos);
			ImGui::DragInt2("origin size", originSize);
			ImGui::InputText("audio event:", &audioEventForButton);
			ImGui::InputText("audio selected:", &audioEventSelected);
			ImGui::InputText("audio focused:", &audioEventFocused);
			CallbackElements(control);
			ImGui::Text("Animations");
			ImGui::Checkbox("animated", &animated);
			VectorEdit(animationRects);
			AssetContainerPath();
			break;
		case Wiwa::GuiControlType::CHECKBOX:
			ImGui::DragInt2("origin position", originPos);
			ImGui::DragInt2("origin size", originSize);
			ImGui::InputText("audio event:", &audioEventForButton);
			ImGui::InputText("audio selected:", &audioEventSelected);
			ImGui::InputText("audio focused:", &audioEventFocused);
			VectorEdit(animationRects);
			CallbackElements(control);
			AssetContainerPath();
			break;
		case Wiwa::GuiControlType::IMAGE:
			ImGui::DragInt2("origin position", originPos);
			ImGui::DragInt2("origin size", originSize);
			
			ImGui::Text("Animations");
			ImGui::Checkbox("animated", &animated);
			ImGui::InputFloat("Animation speed", &animSpeed);
			VectorEdit(animationRects);
			AssetContainerPath();
			break;
		case Wiwa::GuiControlType::SLIDER:
			ImGui::DragInt2("origin position", originPos);
			ImGui::DragInt2("origin size", originSize);
			ImGui::DragInt2("origin position slider", extraOriginPos);
			ImGui::DragInt2("origin size slider", extraOriginSize);
			ImGui::InputText("audio event:", &audioEventForButton);
			ImGui::InputText("audio selected:", &audioEventSelected);
			ImGui::InputText("audio focused:", &audioEventFocused);
			CallbackElements(control);
			AssetContainerPath();
			AssetContainerExtraPath();
			break;
		case Wiwa::GuiControlType::TEXT:
			ImGui::InputText("text", &text);
			ImGui::DragInt2("origin position", originPos);
			ImGui::DragInt2("origin size", originSize);
			break;
		case Wiwa::GuiControlType::VIDEO:
			AssetContainerPath();
			CallbackElements(control);
			break;
		default:
			break;
		}

		if (ImGui::Button("Update"))
		{
			UpdateElements(control, control->type);
		}
	}
	
}

void UIEditorPanel::VectorEdit(std::vector<Wiwa::Rect2i> list)
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
		list.push_back({ 0,0,0,0 });
	}
	ImGui::SameLine();
	if (ImGui::Button("-"))
	{
		if(list.size()>0)list.erase(list.begin() + list.size() - 1);
	}

	animationRects = list;
}

void UIEditorPanel::UpdateElements(Wiwa::GuiControl* control, Wiwa::GuiControlType type)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
	Wiwa::Rect2i originTexRect = { originPos[0],originPos[1],originSize[0],originSize[1] };
	Wiwa::Rect2i extraOriginTexRect = { extraOriginPos[0],extraOriginPos[1],extraOriginSize[0],extraOriginSize[1] };
	control->position.x = pos[0];
	control->position.y = pos[1];
	control->position.width = size[0];
	control->position.height = size[1];
	control->texturePosition = { originPos[0], originPos[1],originSize[0],originSize[1]};
	control->audioEventForButton = audioEventForButton.c_str();
	control->audioEventFocused = audioEventFocused.c_str();
	control->audioEventSelected = audioEventSelected.c_str();
	control->animatedControl = animated;
	control->rotation = rotation;
	if (control->animatedControl)
	{
		control->positionsForAnimations = animationRects;
		control->animSpeed = animSpeed;
	}
	if (callbackID != WI_INVALID_INDEX)
	{
		control->callbackID = callbackID;
		control->callback = Wiwa::Application::Get().getCallbackAt(callbackID);
	}

	r2d.UpdateInstancedQuadTexPriority(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, priority);


	switch (type)
	{
	case Wiwa::GuiControlType::BUTTON:
	{
		control->textId1 = Wiwa::Resources::Load<Wiwa::Image>(pathForAsset.c_str());
		control->texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(control->textId1);
		r2d.UpdateInstancedQuadTexSize(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, { pos[0], pos[1] }, { size[0],size[1] }, Wiwa::Renderer2D::Pivot::CENTER);
		r2d.UpdateInstancedQuadTexClip(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetSize(), originTexRect);
		r2d.UpdateInstancedQuadTexTexture(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetTextureId());
		r2d.UpdateInstancedQuadTexRotation(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, rotation);

	}
	break;
	case Wiwa::GuiControlType::TEXT:
	{
		if (text != control->text)
		{
			control->text = text.c_str();
			Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
			Wiwa::Text* newText = gm.InitFont("library/Fonts/Jade_Smile.ttf", (char*)text.c_str());
			r2d.UpdateInstancedQuadTexClip(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, newText->GetSize(), originTexRect);
			r2d.UpdateInstancedQuadTexTexture(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, newText->GetTextureId());

			delete newText;
		}
		originPos[0] = 0;
		originPos[1] = 0;
		originSize[0] = 512;
		originSize[1] = 512;
		r2d.UpdateInstancedQuadTexRotation(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, rotation);
		r2d.UpdateInstancedQuadTexPosition(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, { pos[0],pos[1] });
		r2d.UpdateInstancedQuadTexSize(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, { pos[0], pos[1] }, { size[0],size[1] }, Wiwa::Renderer2D::Pivot::CENTER);
	}
	break;
	case Wiwa::GuiControlType::CHECKBOX:
	{
		control->textId1 = Wiwa::Resources::Load<Wiwa::Image>(pathForAsset.c_str());
		control->texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(control->textId1);
		r2d.UpdateInstancedQuadTexSize(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, { pos[0], pos[1] }, { size[0],size[1] }, Wiwa::Renderer2D::Pivot::CENTER);
		r2d.UpdateInstancedQuadTexClip(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetSize(), originTexRect);
		r2d.UpdateInstancedQuadTexTexture(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetTextureId());
		r2d.UpdateInstancedQuadTexRotation(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, rotation);

	}
	break;
	case Wiwa::GuiControlType::SLIDER:
	{
		control->textId1 = Wiwa::Resources::Load<Wiwa::Image>(pathForAsset.c_str());
		control->texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(control->textId1);
		r2d.UpdateInstancedQuadTexSize(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, { pos[0], pos[1] }, { size[0],size[1] }, Wiwa::Renderer2D::Pivot::UPLEFT);
		r2d.UpdateInstancedQuadTexClip(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetSize(), originTexRect);
		r2d.UpdateInstancedQuadTexTexture(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetTextureId());
		r2d.UpdateInstancedQuadTexRotation(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, rotation);
		control->textId2 = Wiwa::Resources::Load<Wiwa::Image>(pathForAsset.c_str());
		control->extraTexture = Wiwa::Resources::GetResourceById<Wiwa::Image>(control->textId2);
		r2d.UpdateInstancedQuadTexSize(Wiwa::SceneManager::getActiveScene(), control->id_quad_extra, { pos[0], pos[1] }, { size[0],size[1] }, Wiwa::Renderer2D::Pivot::UPLEFT);
		r2d.UpdateInstancedQuadTexClip(Wiwa::SceneManager::getActiveScene(), control->id_quad_extra, control->texture->GetSize(), originTexRect);
		r2d.UpdateInstancedQuadTexRotation(Wiwa::SceneManager::getActiveScene(), control->id_quad_extra, rotation);
		r2d.UpdateInstancedQuadTexTexture(Wiwa::SceneManager::getActiveScene(), control->id_quad_extra, control->texture->GetTextureId());

		control->extraTexturePosition = originTexRect;

	}
	break;
	case Wiwa::GuiControlType::IMAGE:
	{
		control->textId1 = Wiwa::Resources::Load<Wiwa::Image>(pathForAsset.c_str());
		control->texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(control->textId1);
		r2d.UpdateInstancedQuadTexRotation(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, rotation);
		r2d.UpdateInstancedQuadTexSize(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, { pos[0], pos[1] }, { size[0],size[1] }, Wiwa::Renderer2D::Pivot::CENTER);
		r2d.UpdateInstancedQuadTexClip(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetSize(), originTexRect);
		r2d.UpdateInstancedQuadTexTexture(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetTextureId());
	}	
	break;
	case Wiwa::GuiControlType::BAR:
	{
		control->textId1 = Wiwa::Resources::Load<Wiwa::Image>(pathForAsset.c_str());
		control->texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(control->textId1);
		r2d.UpdateInstancedQuadTexSize(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, { pos[0], pos[1] }, { size[0],size[1] }, Wiwa::Renderer2D::Pivot::UPLEFT);
		r2d.UpdateInstancedQuadTexClip(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetSize(), originTexRect);
		r2d.UpdateInstancedQuadTexTexture(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetTextureId());
		r2d.UpdateInstancedQuadTexRotation(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, rotation);

	}
	break;
	case Wiwa::GuiControlType::VIDEO:
	{
		r2d.UpdateInstancedQuadTexSize(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, { pos[0], pos[1] }, { size[0],size[1] }, Wiwa::Renderer2D::Pivot::UPLEFT);
	}
	break;
	case Wiwa::GuiControlType::ABILITY:
	{
		control->textId1 = Wiwa::Resources::Load<Wiwa::Image>(pathForAsset.c_str());
		control->texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(control->textId1);
		r2d.UpdateInstancedQuadTexSize(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, { pos[0], pos[1] }, { size[0],size[1] }, Wiwa::Renderer2D::Pivot::CENTER);
		r2d.UpdateInstancedQuadTexClip(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetSize(), originTexRect);
		r2d.UpdateInstancedQuadTexTexture(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, control->texture->GetTextureId());
		r2d.UpdateInstancedQuadTexRotation(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, rotation);

	}
	break;
	default:
		break;
	}
}
void UIEditorPanel::CallbackElements(Wiwa::GuiControl* control)
{
	Wiwa::Application& app = Wiwa::Application::Get();
	size_t cbcount = app.GetCallbacksCount();
	if (callbackID == WI_INVALID_INDEX) {
		for (size_t i = 0; i < cbcount; i++) {
			const Func* cb = app.getCallbackAt(i);
			switch (control->type)
			{
			case Wiwa::GuiControlType::BUTTON:
				if (cb->params.size() == 1 && cb->params.at(0).hash == FNV1A_HASH("void")) {

					callbackID = i;

				}
				break;
			case Wiwa::GuiControlType::VIDEO:
				if (cb->params.size() == 1 && cb->params.at(0).hash == FNV1A_HASH("void")) {

					callbackID = i;

				}
				break;
			case Wiwa::GuiControlType::CHECKBOX:
				if (cb->params.size() == 1 && cb->params.at(0).hash == FNV1A_HASH("bool")) {

					callbackID = i;

				}
				break;
			case Wiwa::GuiControlType::SLIDER:
				if (cb->params.size() == 1 && cb->params.at(0).hash == FNV1A_HASH("float")) {

					callbackID = i;

				}
				break;
			default:
				break;
			}
			
		}
	}

	if (cbcount > 0) {
		const Func* current_cb = app.getCallbackAt(callbackID);

		ImGui::Text("Callback type:");

		if (ImGui::BeginCombo("##combo", current_cb->name.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (size_t n = 0; n < cbcount; n++)
			{
				bool is_selected = n == callbackID; // You can store your selection however you want, outside or inside your objects
				current_cb = app.getCallbackAt(n);
				switch (control->type)
				{
				case Wiwa::GuiControlType::BUTTON:
					if (current_cb->params.size() == 1 && current_cb->params.at(0).hash == FNV1A_HASH("void")) {
						if (ImGui::Selectable(current_cb->name.c_str(), is_selected))
						{
							callbackID = n;
							if (is_selected)
								ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
						}
					}
					break;
				case Wiwa::GuiControlType::CHECKBOX:
					if (current_cb->params.size() == 1) {
						if (current_cb->params.at(0).hash == FNV1A_HASH("bool")) {
							if (ImGui::Selectable(current_cb->name.c_str(), is_selected))
							{
								callbackID = n;
								if (is_selected)
									ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
							}
						}
					}
					break;
				case Wiwa::GuiControlType::SLIDER:
					if (current_cb->params.size() == 1) {
						if (current_cb->params.at(0).hash == FNV1A_HASH("float")) {
							if (ImGui::Selectable(current_cb->name.c_str(), is_selected))
							{
								callbackID = n;
								if (is_selected)
									ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
							}
						}
					}
					break;
				case Wiwa::GuiControlType::VIDEO:
					if (current_cb->params.size() == 1) {
						if (current_cb->params.at(0).hash == FNV1A_HASH("void")) {
							if (ImGui::Selectable(current_cb->name.c_str(), is_selected))
							{
								callbackID = n;
								if (is_selected)
									ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
							}
						}
					}
					break;
				default:
					break;
				}
			}
			ImGui::EndCombo();
		}
	}
	
}

void UIEditorPanel::AssetContainerPath()
{
	AssetContainer(pathForAsset.c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".png" || p.extension() == ".mp4")
			{
				pathForAsset = pathS;
			}
		}

		ImGui::EndDragDropTarget();
	}
}

void UIEditorPanel::AssetContainerExtraPath()
{
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
}


void UIEditorPanel::UpdateRotation(Wiwa::GuiControl* control)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
	r2d.UpdateInstancedQuadTexRotation(Wiwa::SceneManager::getActiveScene(), control->id_quad_normal, rotation);
}


void UIEditorPanel::OnEvent(Wiwa::Event& e)
{
	Wiwa::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Wiwa::SceneChangeEvent>({ &UIEditorPanel::OnSceneChange, this });

}
bool UIEditorPanel::OnSceneChange(Wiwa::SceneChangeEvent& e)
{
	elementSelected = -1;
	pos[0] = 0; pos[1] = 0;
	size[0] = 0; size[1] = 0;
	originPos[0] = 0; originPos[1] = 0;
	originSize[0] = 0; originSize[1] = 0;
	extraOriginPos[0] = 0; extraOriginPos[1] = 0;
	extraOriginSize[0] = 0; extraOriginSize[1] = 0;
	callbackID = WI_INVALID_INDEX;
	pathForAsset = "";
	pathForExtraAsset = "";
	audioEventForButton = "";
	return true;
}