#include "OzzAnimationPanel.h"

#include "imgui.h"

#include <Wiwa/core/Application.h>

#include <Wiwa/Platform/Windows/WindowsPlatformUtils.h>
#include "../../../Utils/EditorUtils.h"
#include <Wiwa/utilities/render/Camera.h>

void OzzAnimationPanel::DrawTopbar()
{
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New animator")) {
				if (m_ActiveAnimator) {
					delete m_ActiveAnimator;
				}

				m_ActiveAnimator = new Wiwa::OzzAnimator();
				m_ActiveAnimatorPath = "";
			}

			if (ImGui::MenuItem("Open animator")) {
				std::string filePath = Wiwa::FileDialogs::OpenFile("Wiwa Ozz Animator (*.wiozzanimator)\0*.wiozzanimator\0");
				if (!filePath.empty())
				{
					if (filePath.find(".wiozzanimator") == filePath.npos)
					{
						filePath += ".wiozzanimator";
					}

					if (m_ActiveAnimator) {
						delete m_ActiveAnimator;
					}

					m_ActiveAnimator = Wiwa::OzzAnimator::LoadAnimator(filePath.c_str());
					m_ActiveAnimatorPath = filePath;
				}
			}

			if (ImGui::MenuItem("Save animator")) {
				if (m_ActiveAnimator) {
					if (m_ActiveAnimatorPath != "") {
						Wiwa::OzzAnimator::SaveAnimator(m_ActiveAnimator, m_ActiveAnimatorPath.c_str());
					}
					else {
						std::string filePath = Wiwa::FileDialogs::SaveFile("Wiwa Ozz Animator (*.wiozzanimator)\0*.wiozzanimator\0");

						if (!filePath.empty()) {
							if (filePath.find(".wiozzanimator") == filePath.npos)
							{
								filePath += ".wiozzanimator";
							}

							m_ActiveAnimatorPath = filePath;

							Wiwa::OzzAnimator::SaveAnimator(m_ActiveAnimator, m_ActiveAnimatorPath.c_str());
						}
					}
				}
			}

			if (ImGui::MenuItem("Save animator as...")) {
				if (m_ActiveAnimator) {
					std::string filePath = Wiwa::FileDialogs::SaveFile("Wiwa Ozz Animator (*.wiozzanimator)\0*.wiozzanimator\0");

					if (!filePath.empty()) {
						if (filePath.find(".wiozzanimator") == filePath.npos)
						{
							filePath += ".wiozzanimator";
						}

						m_ActiveAnimatorPath = filePath;

						Wiwa::OzzAnimator::SaveAnimator(m_ActiveAnimator, m_ActiveAnimatorPath.c_str());
					}
				}
			}

			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}
}

void OzzAnimationPanel::DrawBody()
{
	if (!m_ActiveAnimator) {
		ImGui::Text("Create or open an animator");
		return;
	}

	const char* animator_path = "Not saved.";

	if (m_ActiveAnimatorPath != "") {
		animator_path = m_ActiveAnimatorPath.c_str();
	}

	ImGui::Text("Animator file: %s", animator_path);

	if (ImGui::BeginTable("##anim_table", 2, ImGuiTableFlags_Resizable)) {
		ImGui::TableNextColumn();

		DrawMeshContainer();
		DrawMaterialContainer();
		DrawSkeletonContainer();

		bool blend = m_ActiveAnimator->getBlendOnTransition();

		if (ImGui::Checkbox("Blend on transition", &blend)) {
			m_ActiveAnimator->setBlendOnTransition(blend);
		}

		float transtime = m_ActiveAnimator->getTransitionTime();

		if (blend) {
			
			if (ImGui::InputFloat("Transition time", &transtime)) {
				m_ActiveAnimator->setTransitionTime(transtime);
			}
		}
		else {
			ImGui::BeginDisabled();
			ImGui::InputFloat("Transition time", &transtime);
			ImGui::EndDisabled();
		}

		DrawAnimations();

		ImGui::TableNextColumn();

		DrawAnimationViewer();

		ImGui::EndTable();
	}
}

void OzzAnimationPanel::DrawAnimationViewer()
{
	m_Camera->frameBuffer->Clear({0.1f, 0.1f, 0.1f, 1.0f});

	// Render animation
	m_ActiveAnimator->Update(Wiwa::Time::GetRealDeltaTimeSeconds());
	m_ActiveAnimator->Render(m_Camera, m_Transform);

	// ImGui draw
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

	Wiwa::Size2i resolution = Wiwa::Application::Get().GetTargetResolution();
	float ar = resolution.w / (float)resolution.h;
	Wiwa::Size2f scales = { viewportPanelSize.x / (float)resolution.w, viewportPanelSize.y / (float)resolution.h };

	float scale = scales.x < scales.y ? scales.x : scales.y;

	ImVec2 isize = { resolution.w * scale, resolution.h * scale };

	ImTextureID tex = (ImTextureID)(intptr_t)m_Camera->frameBuffer->getColorBufferTexture();
	ImGui::Image(tex, isize, ImVec2(0, 1), ImVec2(1, 0));
}

void OzzAnimationPanel::DrawMeshContainer()
{
	// Mesh file
	std::string mesh_label = "No mesh";

	if (m_ActiveAnimator->IsMeshLoaded()) {
		mesh_label = m_ActiveAnimator->getMeshPath();
	}

	ImGui::Text("Mesh");
	ImGui::PushID("ozz_mesh");

	AssetContainer(mesh_label.c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".mesh")
			{
				std::string libpath = Wiwa::Resources::_assetToLibPath(p.string());
				bool loaded = m_ActiveAnimator->LoadMesh(libpath);

				if (loaded) {
					WI_INFO("Loaded ozz mesh {} successfully.", p.string().c_str());
				}
				else {
					WI_INFO("Couldn't load ozz mesh {}.", p.string().c_str());
				}
			}
		}

		ImGui::EndDragDropTarget();
	}
	ImGui::PopID();
}

void OzzAnimationPanel::DrawSkeletonContainer()
{
	// Mesh file
	std::string skeleton_label = "No skeleton";

	if (m_ActiveAnimator->IsSkeletonLoaded()) {
		skeleton_label = m_ActiveAnimator->getSkeletonPath();
	}

	ImGui::Text("Skeleton");
	ImGui::PushID("ozz_skeleton");

	AssetContainer(skeleton_label.c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".skeleton")
			{
				std::string libpath = Wiwa::Resources::_assetToLibPath(p.string());
				bool loaded = m_ActiveAnimator->LoadSkeleton(libpath);

				if (loaded) {
					WI_INFO("Loaded ozz skeleton {} successfully.", p.string().c_str());
				}
				else {
					WI_INFO("Couldn't load ozz skeleton {}.", p.string().c_str());
				}
			}
		}

		ImGui::EndDragDropTarget();
	}
	ImGui::PopID();
}

void OzzAnimationPanel::DrawMaterialContainer()
{
	// Material file
	std::string material_label = "No material";

	if (m_ActiveAnimator->IsMaterialLoaded()) {
		material_label = m_ActiveAnimator->getMaterialPath();
	}

	ImGui::Text("Material");
	ImGui::PushID("material");

	AssetContainer(material_label.c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".wimaterial")
			{
				std::string libpath = Wiwa::Resources::_assetToLibPath(p.string());
				bool loaded = m_ActiveAnimator->LoadMaterial(libpath);

				if (loaded) {
					WI_INFO("Loaded material {} successfully.", p.string().c_str());
				}
				else {
					WI_INFO("Couldn't load material skeleton {}.", p.string().c_str());
				}
			}
		}

		ImGui::EndDragDropTarget();
	}
	ImGui::PopID();
}

void OzzAnimationPanel::DrawAnimations()
{
	// Add animation button
	if (ImGui::Button("Add animation")) {
		ImGui::OpenPopup("add_anim");
	}

	// Add animation popup
	if (ImGui::BeginPopup("add_anim")) {
		ImGui::Text("Add animation");

		static std::string animation_name;

		ImGui::InputText("Name", &animation_name);

		const char* items[] = { "Partial blending", "Simple animation"};

		static int current_item = 0;

		if (ImGui::BeginCombo("anim_type", items[current_item])) {
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (current_item == n); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(items[n], is_selected))
				{
					current_item = n;
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
			}

			ImGui::EndCombo();
		}

		static std::string error_msg;

		if (error_msg != "") {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), error_msg.c_str());
		}

		if (ImGui::Button("Add")) {
			size_t anim_id = 0;

			switch (current_item) {
				case Wiwa::AnimationType::AT_PARTIAL_BLEND:
					anim_id = m_ActiveAnimator->CreatePartialAnimation(animation_name);
					break;
				case Wiwa::AnimationType::AT_SIMPLE:
					anim_id = m_ActiveAnimator->CreateSimpleAnimation(animation_name);
					break;
				default:
					break;
			}

			if (anim_id == WI_INVALID_INDEX) {
				error_msg = "Animation name already exists.";
			}
			else {
				error_msg = "";
				animation_name = "";

				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			error_msg = "";
			animation_name = "";

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// Show animation list
	if (ImGui::CollapsingHeader("Animation list")) {
		size_t anim_count = m_ActiveAnimator->getAnimationCount();
		
		for (size_t i = 0; i < anim_count; i++) {
			Wiwa::OzzAnimator::AnimationData& a_data = m_ActiveAnimator->getAnimationAt(i);

			if (!a_data.animation) continue;

			ImGui::PushID(i);

			if (ImGui::TreeNode(a_data.name.c_str())) {
				Wiwa::OzzAnimation* animation = a_data.animation;
				Wiwa::AnimationType a_type = animation->getAnimationType();
				ImGui::Text("Status: ");
				ImGui::SameLine();

				Wiwa::OzzAnimation::Status a_status = animation->getStatus();
				
				a_status == Wiwa::OzzAnimation::Status::VALID ?
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Valid") :
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid");

				float playback_speed = animation->getPlaybackSpeed();

				if (ImGui::InputFloat("Playback speed", &playback_speed)) {
					animation->setPlaybackSpeed(playback_speed);
				}

				bool loop = animation->getLoop();

				if (ImGui::Checkbox("Loop", &loop)) {
					animation->setLoop(loop);
				}

				switch (a_type) {
					case Wiwa::AnimationType::AT_PARTIAL_BLEND:
						DrawPartialBlendingAnimation((Wiwa::OzzAnimationPartialBlending*)animation);
						break;
					case Wiwa::AnimationType::AT_SIMPLE:
						DrawSimpleAnimation((Wiwa::OzzAnimationSimple*)animation);
						break;
					default:
						break;
				}

				float time_ratio = animation->getTimeRatio();

				if (ImGui::SliderFloat("Time", &time_ratio, 0.0f, 1.0f)) {
					animation->setTimeRatio(time_ratio);
				}

				if (ImGui::Button("Play")) {
					m_ActiveAnimator->PlayAnimation(i);
				}

				ImGui::SameLine();

				if (ImGui::Button("Stop")) {
					m_ActiveAnimator->StopAnimation(false);
				}

				if (ImGui::Button("Delete")) {
					m_ActiveAnimator->RemoveAnimationAt(i);
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}
}

void OzzAnimationPanel::DrawPartialBlendingAnimation(Wiwa::OzzAnimationPartialBlending* partial_animation)
{
	// Upper body root
	static int ubr = 0;
	static int prevubr = 0;

	ubr = partial_animation->GetUpperBodyRoot();
	prevubr = ubr;

	ozz::span<const char* const> bone_names = m_ActiveAnimator->getSkeletonBoneNames();

	if (ImGui::BeginCombo("Upper body root", bone_names[ubr])) {
		for (int n = 0; n < bone_names.size(); n++)
		{
			bool is_selected = (ubr == n); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(bone_names[n], is_selected))
			{
				ubr = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
		}

		ImGui::EndCombo();
	}

	if (ubr != prevubr) {
		partial_animation->SetUpperBodyRoot(ubr);
	}

	// Lower body animation
	const char* lower = "No lower animation";

	if (partial_animation->IsLowerAnimationLoaded()) {
		lower = partial_animation->getLowerBodyFile();
	}

	ImGui::Text("Lower body animation");
	ImGui::PushID("lower_body");
	AssetContainer(lower);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".anim")
			{
				std::string libpath = Wiwa::Resources::_assetToLibPath(p.string());
				partial_animation->LoadLowerAnimation(libpath.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}
	ImGui::PopID();

	// Upper body animation
	const char* upper = "No upper animation";

	if (partial_animation->IsUpperAnimationLoaded()) {
		upper = partial_animation->getUpperBodyFile();
	}

	ImGui::Text("Upper body animation");
	ImGui::PushID("upper_body");
	AssetContainer(upper);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".anim")
			{
				std::string libpath = Wiwa::Resources::_assetToLibPath(p.string());
				partial_animation->LoadUpperAnimation(libpath.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}
	ImGui::PopID();
}

void OzzAnimationPanel::DrawSimpleAnimation(Wiwa::OzzAnimationSimple* simple_animation)
{
	const char* anim_file = "No animation";

	if (simple_animation->IsAnimationLoaded()) {
		anim_file = simple_animation->getAnimationPath();
	}

	ImGui::Text("Animation file");
	ImGui::PushID("animation_file");
	AssetContainer(anim_file);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".anim")
			{
				std::string libpath = Wiwa::Resources::_assetToLibPath(p.string());
				simple_animation->LoadAnimation(libpath.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}
	ImGui::PopID();
}

OzzAnimationPanel::OzzAnimationPanel(EditorLayer* instance)
	: Panel("Ozz Animation Panel", ICON_FK_INFO, instance),
	m_ActiveAnimator(nullptr),
	m_Camera(nullptr)
{
	m_Camera = new Wiwa::Camera();

	Wiwa::Size2i& size = Wiwa::Application::Get().GetTargetResolution();
	float aratio = size.w / (float)size.h;

	m_Camera->SetPerspective(45.0f, aratio);

	m_Camera->setPosition(glm::vec3(5.0f, 6.5f, 5.0f));
	m_Camera->lookat({ 0.0f, 2.5f, 0.0f });

	m_Transform = glm::mat4(1.0f);

	m_Transform = glm::scale(m_Transform, glm::vec3(100.0f, 100.0f, 100.0f));
}

OzzAnimationPanel::~OzzAnimationPanel()
{
	delete m_Camera;
}

void OzzAnimationPanel::Draw()
{
	ImGui::Begin("Ozz Animator", &active, ImGuiWindowFlags_MenuBar);

	DrawTopbar();
	DrawBody();	

	ImGui::End();
}
