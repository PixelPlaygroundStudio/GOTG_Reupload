#include <wipch.h>
#include "ProjectManager.h"
#include <Wiwa/utilities/json/JSONDocument.h>
#include <Wiwa/core/Application.h>
#include <Wiwa/scene/Scene.h>
#include <Wiwa/core/Resources.h>

namespace Wiwa
{
	std::filesystem::path ProjectManager::m_CurrentProject = "";
	std::string ProjectManager::m_Name;
	std::string ProjectManager::m_Version;
	std::string ProjectManager::m_Company;
	ProjectManager::Target ProjectManager::m_Target;
	std::vector<ProjectManager::SceneData> ProjectManager::m_SceneList;



	void ProjectManager::CreateProject(const char* file)
	{
		m_Name = "New project";
		m_Version = "0.1";
		m_Company = "My company";
		m_Target = Target::Windows;

		Wiwa::JSONDocument doc;
		doc.AddMember("name", m_Name.c_str());
		doc.AddMember("version", m_Version.c_str());
		doc.AddMember("company", m_Company.c_str());
		doc.AddMember("target", (int)m_Target);
		
		std::string path = file;
		path += ".wiproject";
		doc.save_file(path.c_str());
		m_CurrentProject = path.c_str();
	}
	void ProjectManager::OpenProject(const char* file)
	{
		Wiwa::JSONDocument doc(file);
		m_CurrentProject = file;
		if (doc.HasMember("name"))
			m_Name = doc["name"].as_string();
		if (doc.HasMember("version"))
			m_Version = doc["version"].as_string();
		if (doc.HasMember("company"))
			m_Company = doc["company"].get<const char*>();
		if (doc.HasMember("target"))
			m_Target = (Target)doc["target"].as_int();

		if (doc.HasMember("scene_list")) {
			JSONValue scene_list = doc["scene_list"];

			if (scene_list.IsArray()) {
				size_t size = scene_list.Size();
				SceneData sd;

				for (size_t i = 0; i < size; i++) {
					JSONValue scene = scene_list[(uint32_t)i];
					
					sd.scene_name = scene["name"].as_string();
					sd.scene_path = scene["path"].as_string();

					if (Wiwa::FileSystem::Exists(sd.scene_path.c_str())) {
						AddScene(sd.scene_name.c_str(), sd.scene_path.c_str());
					}
				}
			}
		}

		doc.save_file(file);
	}

	bool ProjectManager::SaveProject()
	{
		if (m_CurrentProject.empty())
		{
			return false;
		}
		Wiwa::JSONDocument proj_file;
		proj_file.AddMember("name", m_Name.c_str());
		proj_file.AddMember("version", m_Version.c_str());
		proj_file.AddMember("company", m_Company.c_str());
		proj_file.AddMember("target", (int)m_Target);

		Wiwa::JSONValue scene_list = proj_file.AddMemberArray("scene_list");

		size_t slsize = m_SceneList.size();

		for (size_t i = 0; i < slsize; i++) {
			Wiwa::JSONValue scene = scene_list.PushBackObject();
			scene.AddMember("name", m_SceneList[i].scene_name.c_str());
			
			scene.AddMember("path", m_SceneList[i].scene_path.c_str());
		}

		proj_file.save_file(m_CurrentProject.string().c_str());

		return true;
	}

	void ProjectManager::SaveProjectAs(const char* file)
	{
		std::string path = file;
		Wiwa::JSONDocument doc(path.c_str());
		m_CurrentProject = path;
		doc.AddMember("name", m_Name.c_str());
		doc.AddMember("version", m_Version.c_str());
		doc.AddMember("company", m_Company.c_str());
		doc.save_file(path.c_str());
	}

	void ProjectManager::AddScene(const char* name, const char* path)
	{
		std::string p = Wiwa::Resources::_assetToLibPath(path).c_str();
		std::filesystem::path pathR(p);
		
		pathR = std::filesystem::relative(pathR, std::filesystem::current_path());
		SceneData* sdata = getSceneByName(name);

		if (!sdata) {
			m_SceneList.emplace_back(SceneData{ name, pathR.string()});
		}
	}

	void ProjectManager::RemoveScene(const char* name)
	{
		size_t ind = getSceneIndexByName(name);

		if (ind != WI_INVALID_INDEX) {
			m_SceneList.erase(m_SceneList.begin() + ind);
		}
	}

	ProjectManager::SceneData* ProjectManager::getSceneByName(const char* name)
	{
		SceneData* sdata = NULL;

		size_t s = m_SceneList.size();

		for (size_t i = 0; i < s; i++) {
			if (m_SceneList[i].scene_name == name) {
				sdata = &m_SceneList[i];
				break;
			}
		}

		return sdata;
	}

	size_t ProjectManager::getSceneIndexByName(const char* name)
	{
		size_t ind = WI_INVALID_INDEX;

		size_t s = m_SceneList.size();

		for (size_t i = 0; i < s; i++) {
			if (m_SceneList[i].scene_name == name) {
				ind = i;
				break;
			}
		}

		return ind;
	}
	size_t ProjectManager::getSceneIndexByPath(const char* path)
	{
		size_t ind = WI_INVALID_INDEX;

		size_t s = m_SceneList.size();

		for (size_t i = 0; i < s; i++) {
			if (m_SceneList[i].scene_path== path) {
				ind = i;
				break;
			}
		}

		return ind;
	}
}