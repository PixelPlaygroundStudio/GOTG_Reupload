#include "ProjectPanel.h"

#include <imgui.h>

#include <Wiwa/core/Application.h>
#include <Wiwa/core/ProjectManager.h>
#include "../EditorLayer.h"

ProjectPanel::ProjectPanel(EditorLayer *instance)
	: Panel("Project", ICON_FK_DATABASE, instance)
{
}

ProjectPanel::~ProjectPanel()
{
}

void ProjectPanel::Draw()
{
	ImGui::Begin(iconName.c_str(), &active, ImGuiWindowFlags_NoDocking);
	const char* buff = Wiwa::ProjectManager::GetProjectName();
	std::string edit = buff;

	ImGui::InputText("Project Name", (char *)edit.c_str(), 64);
	if (strcmp(edit.c_str(), buff) != 0)
		Wiwa::ProjectManager::SetProjectName(edit.c_str());

	buff = Wiwa::ProjectManager::GetProjectVersion();
	edit = buff;

	ImGui::InputText("Project Version", (char *)edit.c_str(), 64);
	if (strcmp(edit.c_str(), buff) != 0)
		Wiwa::ProjectManager::SetProjectVersion(edit.c_str());

	buff = Wiwa::ProjectManager::GetProjectCompany();
	edit = buff;

	ImGui::InputText("Company Name", (char *)edit.c_str(), 64);
	if (strcmp(edit.c_str(), buff) != 0)
		Wiwa::ProjectManager::SetProjectCompany(edit.c_str());

	ImGui::Separator();

	Wiwa::ProjectManager::Target target = Wiwa::ProjectManager::GetProjectTarget();

	const char *types[] = {"Windows", "We don't support more platforms you fool"};

	static const char *currentItem = types[(int)target];

	if (ImGui::BeginCombo("Target", currentItem))
	{
		for (int i = 0; i <= (int)Wiwa::ProjectManager::Target::None; i++)
		{
			bool isSelected = (currentItem == types[i]);
			if (ImGui::Selectable(types[i], isSelected))
			{
				currentItem = types[i];
			}
			if (isSelected)
			{
				Wiwa::ProjectManager::SetProjectTarget((Wiwa::ProjectManager::Target)i);
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
	ImGui::Separator();

	ImGui::Text("Scene list");

	std::vector<Wiwa::ProjectManager::SceneData> &scene_list = Wiwa::ProjectManager::getSceneDataList();
	size_t scene_count = scene_list.size();

	for (int i = 0; i < scene_count; i++)
	{
		Wiwa::ProjectManager::SceneData &sdata = scene_list[i];
		std::string sceneName = std::to_string(i) + " ";
		sceneName += sdata.scene_name.c_str();
		ImGui::Selectable(sceneName.c_str());

		if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
		{
			int n_next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
			if (n_next >= 0 && n_next < scene_count)
			{
				// Swap indexes
				Wiwa::ProjectManager::SceneData aux = sdata;
				scene_list[i] = scene_list[n_next];
				scene_list[n_next] = aux;

				ImGui::ResetMouseDragDelta();
			}
		}
	}

	if(ImGui::Button("Add current scene")) {
		EditorLayer& editor = EditorLayer::Get();
		std::filesystem::path scene_path = editor.getOpenedScenePath();

		if (scene_path != "") {

			std::filesystem::path name = scene_path.filename();
			name.replace_extension();

			Wiwa::ProjectManager::AddScene(name.string().c_str(), scene_path.string().c_str());
		}
	}

	ImGui::End();
}