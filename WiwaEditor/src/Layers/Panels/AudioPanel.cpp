#include "AudioPanel.h"

#include "imgui.h"

#include <Wiwa/audio/Audio.h>
#include <Wiwa/Platform/Windows/WindowsPlatformUtils.h>
#include <Wiwa/utilities/filesystem/FileSystem.h>
#include <Wiwa/core/Resources.h>

AudioPanel::AudioPanel(EditorLayer* instance) 
	: Panel("Audio", ICON_FK_HEADPHONES, instance)
{
	bank_list = std::vector<Audio::BankData>();
	event_list = std::vector<Audio::EventData>();
}

AudioPanel::~AudioPanel()
{
}

void AudioPanel::Draw()
{
	ImGui::Begin(iconName.c_str(), &active);

	loaded_project = Audio::LoadedProject();

	if (loaded_project) {
		ImGui::TextColored(ImVec4(0, 255, 0, 1), "Loaded");
	}
	else {
		ImGui::TextColored(ImVec4(255, 252, 127, 1), "Not loaded");
	}

	ImGui::SameLine();
	if (ImGui::Button("Load project##audio")) {
		filename = Wiwa::FileDialogs::OpenFile("Wwise bank (*.bnk)\0*.bnk\0");

		if (filename != "") {
			filename = Wiwa::Resources::_assetToLibPath(filename);

			std::filesystem::path p = std::filesystem::relative(filename);
			filename = p.string();

			ret = Audio::LoadProject(filename.c_str());

			if (!ret) {
				WI_ERROR("Error loading project [{}]", Audio::GetLastError());
			}
		}
	}

	if (ImGui::CollapsingHeader("Banks##audio")) {
		bank_list = Audio::GetLoadedBankList();

		b_size = bank_list.size();

		del_bank_id = Audio::INVALID_ID;

		for (size_t i = 0; i < b_size; i++) {
			ImGui::Text(bank_list[i].name.c_str());
			ImGui::SameLine();
			if (ImGui::Button("Delete##audio")) {
				del_bank_id = i;
			}
		}

		if (del_bank_id != Audio::INVALID_ID) {
			Audio::UnloadBank(bank_list[del_bank_id].name.c_str());
		}

		if (ImGui::Button("Load bank##audio")) {
			filename = Wiwa::FileDialogs::OpenFile("Wwise bank (*.bnk)\0*.bnk\0");

			if (filename != "") {
				filename = Wiwa::Resources::_assetToLibPath(filename);
				std::filesystem::path p = std::filesystem::relative(filename);
				filename = p.string();
				ret = Audio::LoadBank(filename.c_str());

				if (!ret) {
					WI_ERROR("Error loading project [{}]", Audio::GetLastError());
				}
			}
		}
	}

	if (ImGui::CollapsingHeader("Events##audio")) {
		event_list = Audio::GetLoadedEventList();

		e_size = event_list.size();

		for (size_t i = 0; i < e_size; i++) {
			ImGui::Text(event_list[i].name.c_str());
			ImGui::SameLine();
			ImGui::PushID(i);
			if (ImGui::Button("Play")) {
				Audio::PostEvent(event_list[i].name.c_str());
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop")) {
				Audio::StopEvent(event_list[i].name.c_str());
			}
			ImGui::PopID();
		}

		if (ImGui::Button("Load event##audio")) {
			ImGui::OpenPopup("Load event##audio");
		}
	}

	if (ImGui::BeginPopup("Load event##audio")) {
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Load event").x) / 2.f);
		ImGui::Text("Load event");
		buffer[64] = { 0 };
		ImGui::Text("Event name");
		ImGui::SameLine();
		ImGui::InputText("##inputEvent", buffer, IM_ARRAYSIZE(buffer));

		if (ImGui::Button("Load##audio")) {
			ret = Audio::LoadEvent(buffer);

			if (!ret) {
				WI_ERROR("Error loading event [{}]", Audio::GetLastError());
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel##audio")) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::Button("Reload project##audio")) {
		ret = Audio::ReloadProject();

		if (!ret) {
			WI_ERROR("Error reloading audio [{}]", Audio::GetLastError());
		}
	}

	ImGui::Text("Save or load a preset for the current secene");
	ImGui::NewLine();

	if (ImGui::Button("Save preset##audio")) {
		std::string filename = Wiwa::FileDialogs::SaveFile("Wwise preset (*.json)\0*.json\0");

		if (filename != "") {

			std::filesystem::path p = std::filesystem::relative(filename);
			
			p = p.replace_extension(".json");

			filename = p.generic_string();
			

			bool ret = Audio::SavePreset(filename.c_str());

			if (!ret) {
				WI_ERROR("Error saving preset [{}]", Audio::GetLastError());
			}
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Load preset##audio")) {
		std::string filename = Wiwa::FileDialogs::OpenFile("Wwise preset (*.json)\0*.json\0");

		if (filename != "") {

			std::filesystem::path p = std::filesystem::relative(filename);
			filename = p.generic_string();

			ret = Audio::LoadPreset(filename.c_str());

			if (!ret) {
				WI_ERROR("Error loading preset [{}]", Audio::GetLastError());
			}
		}
	}

	ImGui::End();
}

void AudioPanel::OnEvent(Wiwa::Event& e)
{
	Wiwa::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Wiwa::SceneChangeEvent>({ &AudioPanel::OnSceneChange, this });
}

bool AudioPanel::OnSceneChange(Wiwa::SceneChangeEvent& e)
{
	loaded_project = false;
	filename = "";
	ret = false;
	bank_list = std::vector<Audio::BankData>();
	event_list = std::vector<Audio::EventData>();
	b_size = 0;
	e_size = 0;
	del_bank_id = 0;
	buffer[64] = { 0 };

	return true;
}

