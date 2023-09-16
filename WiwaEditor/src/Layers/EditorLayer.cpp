#include "EditorLayer.h"

#include <imgui.h>
#include <stdio.h>

#include <Wiwa/core/Application.h>
#include <Wiwa/core/ProjectManager.h>
#include <Wiwa/utilities/json/JSONDocument.h>

#include <Wiwa/core/Input.h>
#include <ImGuizmo.h>

#include <imgui_internal.h>

#include <Wiwa/core/Resources.h>

#include <Wiwa/Platform/Windows/WindowsPlatformUtils.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include <Wiwa/ecs/systems/AnimatorSystem.h>
#include <Wiwa/ecs/components/Sprite.h>

#include <Wiwa/ecs/systems/PhysicsSystem.h>

#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/core/Renderer3D.h>
#include <Wiwa/audio/Audio.h>

#include <Wiwa/audio/Audio.h>


#include "../Entities.h"
EditorLayer *EditorLayer::s_Instance = nullptr;
std::string EditorLayer::s_SolVersion = "vs2022";
std::string EditorLayer::s_BuildConf = "Release";
bool EditorLayer::s_BuildAutomatic = false;
std::thread *EditorLayer::s_RegenThread;


EditorLayer::EditorLayer()
	: Layer("Editor Layer")
{
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::OnAttach()
{
	WI_CORE_ASSERT(!s_Instance, "Application already exists!");
	RegenSol();
	s_Instance = this;

	Wiwa::Time::Stop();

	// Editor scene
	m_EditorSceneId = Wiwa::SceneManager::CreateScene();
	Wiwa::SceneManager::StopScene();

	m_EditorScene = Wiwa::SceneManager::getScene(m_EditorSceneId);
	m_EditorScene->GetEntityManager().SetInitSystemsOnApply(false);
	m_EditorScene->GetEntityManager().AddSystemToWhitelist("MeshRenderer");
	m_EditorScene->GetEntityManager().AddSystemToWhitelist("OzzAnimationSystem");
	

	Wiwa::SceneManager::SetScene(m_EditorSceneId, false);

	Wiwa::Size2i &res = Wiwa::Application::Get().GetTargetResolution();
	float ar = res.w / (float)res.h;
	CameraId cam_id = m_EditorScene->GetCameraManager().CreatePerspectiveCamera(45, ar, 0.1f, 131.0f);
	Wiwa::Camera *cam = m_EditorScene->GetCameraManager().getCamera(cam_id);
	cam->setPosition(glm::vec3{-52.5f, 30.2f, 26.2f});
	cam->setRotation(glm::vec3{-26.0f, -30.2f, 0.0f});

	m_Scene = std::make_unique<ScenePanel>(this);
	m_Configuration = std::make_unique<ConfigurationPanel>(this);
	m_Console = std::make_unique<ConsolePanel>(this);
	m_Hierarchy = std::make_unique<HierarchyPanel>(this);
	m_Assets = std::make_unique<AssetsPanel>(this);
	m_Inspector = std::make_unique<InspectorPanel>(this);
	m_MeshView = std::make_unique<MeshViewPanel>(this);
	m_MaterialEditor = std::make_unique<MaterialPanel>(this);
	m_SystemsPanel = std::make_unique<SystemsPanel>(this);
	m_GamePanel = std::make_unique<GamePanel>(this);
	m_CamerasPanel = std::make_unique<CamerasPanel>(this);
	m_ResourcesPanel = std::make_unique<ResourcesPanel>(this);
	m_ImportPanel = std::make_unique<ImportPanel>(this);
	m_ShaderPanel = std::make_unique<ShaderPanel>(this);
	m_EnvPanel = std::make_unique<EnvironmentPanel>(this);
	m_UiPanel = std::make_unique<UIPanel>(this);
	m_UiEditorPanel = std::make_unique<UIEditorPanel>(this);
	m_DialogPanel = std::make_unique<DialogPanel>(this);
	m_GameLogPanel = std::make_unique<GameLogPanel>(this);
	m_InventoryPanel = std::make_unique<InventoryPanel>(this);
	m_AiPanel = std::make_unique<AIPanel>(this);
	m_OzzAnimationPanel = std::make_unique<OzzAnimationPanel>(this);
	m_EnemyPanel = std::make_unique<EnemyPanel>(this);
	m_ShadowsPanel = std::make_unique<ShadowsPanel>(this);

	m_AnimatorPanel = std::make_unique <AnimatorPanel>(this);
	m_AnimationPanel = std::make_unique<AnimationPanel>(this);

	m_AudioPanel = std::make_unique<AudioPanel>(this);

	m_ProjectPanel = std::make_unique<ProjectPanel>(this);
	m_About = std::make_unique<AboutPanel>(this);

	m_Panels.push_back(m_Configuration.get());
	m_Panels.push_back(m_Console.get());
	m_Panels.push_back(m_Scene.get());
	m_Panels.push_back(m_Hierarchy.get());
	m_Panels.push_back(m_Assets.get());
	m_Panels.push_back(m_Inspector.get());
	m_Panels.push_back(m_MeshView.get());
	m_Panels.push_back(m_MaterialEditor.get());
	m_Panels.push_back(m_SystemsPanel.get());
	m_Panels.push_back(m_GamePanel.get());
	m_Panels.push_back(m_CamerasPanel.get());
	m_Panels.push_back(m_ResourcesPanel.get());
	m_Panels.push_back(m_ImportPanel.get());
	m_Panels.push_back(m_ShaderPanel.get());
	m_Panels.push_back(m_EnvPanel.get());
	m_Panels.push_back(m_AnimatorPanel.get());
	m_Panels.push_back(m_AnimationPanel.get());
	m_Panels.push_back(m_UiPanel.get());
	m_Panels.push_back(m_UiEditorPanel.get());
	m_Panels.push_back(m_DialogPanel.get());
	m_Panels.push_back(m_AudioPanel.get());
	m_Panels.push_back(m_GameLogPanel.get());
	m_Panels.push_back(m_InventoryPanel.get());
	m_Panels.push_back(m_AiPanel.get());
	m_Panels.push_back(m_OzzAnimationPanel.get());
	m_Panels.push_back(m_ShadowsPanel.get());

	m_Panels.push_back(m_EnemyPanel.get());
	
	m_Settings.push_back(m_ProjectPanel.get());
	m_Settings.push_back(m_About.get());
	m_EventCallback = {&Wiwa::Application::OnEvent, &Wiwa::Application::Get()};

	LoadCallback();

	m_ReloadLayout = false;

	WI_TRACE("Editor layer attached!");
}

void EditorLayer::OnDetach()
{
	s_Instance = nullptr;
	m_Panels.clear();
	m_Settings.clear();
}

void EditorLayer::OnUpdate()
{
	if (m_ReloadLayout)
	{
		ImGui::LoadIniSettingsFromDisk(m_ActiveLayout.c_str());
		m_ReloadLayout = false;
	}

	for (auto &p : m_Panels)
	{
		if (p->active)
			p->Update();
	}
	for (auto &p : m_Settings)
	{
		if (p->active)
			p->Update();
	}
}

void EditorLayer::OnImGuiRender()
{
	ImGuiContext *ctx = Wiwa::Application::Get().GetImGuiContext();
	ImGui::SetCurrentContext(ctx);

	MainMenuBar();
	DockSpace();
	for (auto &p : m_Panels)
	{
		if (p->active)
			p->Draw();
	}
	for (auto &p : m_Settings)
	{
		if (p->active)
			p->Draw();
	}
	if (m_ShowDemo)
		ImGui::ShowDemoWindow(&m_ShowDemo);
}

void EditorLayer::OnEvent(Wiwa::Event &e)
{
	Wiwa::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Wiwa::KeyPressedEvent>({ &EditorLayer::OnKeyPressed, this});
	dispatcher.Dispatch<Wiwa::OnLoadEvent>({ &EditorLayer::OnLoad, this});
	dispatcher.Dispatch<Wiwa::OnSaveEvent>({ &EditorLayer::OnSave, this});
	dispatcher.Dispatch<Wiwa::WindowCloseEvent>({ &EditorLayer::OnWindowClose, this});

	for (auto it = m_Panels.end(); it != m_Panels.begin();)
	{
		(*--it)->OnEvent(e);
		if (e.Handled)
			break;
	}
}

SceneId EditorLayer::LoadScene(const std::string &m_Path)
{
	// Load scene and prepare it
	SceneId id = Wiwa::SceneManager::LoadScene(m_Path.c_str(), Wiwa::SceneManager::LoadFlags::LOAD_DEFAULT | Wiwa::SceneManager::LoadFlags::LOAD_NO_INIT);
	Wiwa::Scene *scene = Wiwa::SceneManager::getScene(id);
	scene->GetEntityManager().AddSystemToWhitelist("MeshRenderer");
	scene->GetEntityManager().AddSystemToWhitelist("OzzAnimationSystem");

	// Update editor scene references
	m_OpenedScenePath = m_Path;
	m_EditorSceneId = id;
	m_EditorScene = scene;
	return id;
}

static bool threadExec = false;
static bool finishedThread = false;
static std::mutex mutex;

void EditorLayer::RegenSolutionThread()
{
	if (!s_BuildAutomatic)
		return;
	std::string call = "call tools\\generatesol.bat ";
	call += s_SolVersion;
	call += " game_assembly/WiwaGameAssembly.sln ";
	call += s_BuildConf;
	system(call.c_str());
	mutex.lock();
	finishedThread = true;
	mutex.unlock();
}

void EditorLayer::LoadLayout(const char *path)
{
	Wiwa::JSONDocument layout(path);

	if (layout.HasMember("imgui_file"))
	{
		m_ActiveLayout = layout["imgui_file"].as_string();

		size_t psize = m_Panels.size();

		for (size_t i = 0; i < psize; i++)
		{
			Panel *p = m_Panels[i];

			if (layout.HasMember(p->GetName()))
			{
				p->active = layout[p->GetName()].as_bool();
			}
		}

		m_ReloadLayout = true;
	}
}

void EditorLayer::SaveLayout(LayoutData &ldata)
{
	Wiwa::JSONDocument layout;

	std::filesystem::path p = ldata.path;
	p.replace_extension(".ini");

	layout.AddMember("imgui_file", p.string().c_str());

	size_t psize = m_Panels.size();

	for (size_t i = 0; i < psize; i++)
	{
		Panel *panel = m_Panels[i];

		layout.AddMember(panel->GetName(), panel->active);
	}

	layout.save_file(ldata.path.c_str());

	Wiwa::FileSystem::Copy("imgui.ini", p.string().c_str());
}

void EditorLayer::BuildProject()
{
	// On save callback
	Wiwa::OnSaveEvent ev;
	Action<Wiwa::Event &> action = {&Wiwa::Application::OnEvent, &Wiwa::Application::Get()};
	action.execute(ev);

	// Build game
	system("call tools\\buildgame.bat");
}

void EditorLayer::RegenSol()
{
	if (!s_BuildAutomatic)
		return;
	if (threadExec)
	{
		mutex.lock();
		if (finishedThread)
			s_RegenThread->join();

		mutex.unlock();

		if (finishedThread)
			delete s_RegenThread;
	}

	s_RegenThread = new std::thread(RegenSolutionThread);
	threadExec = true;

	finishedThread = false;
}

void EditorLayer::SubmitToMainThread(const std::function<void()> func)
{
	std::scoped_lock<std::mutex> lock(m_EditorThreadMutex);

	m_EditorThreadQueue.emplace_back(func);
}

void EditorLayer::MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl+N"))
			{
				NewProject();
			}
			if (ImGui::MenuItem("Open", "Ctrl+O"))
			{
				OpenProject();
			}
			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				Wiwa::ProjectManager::SaveProject();
			}
			if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S"))
			{
				SaveProjectAs();
			}
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_FK_FILE " New scene", ""))
			{
				StopScene();
				Wiwa::SceneManager::UnloadScene(m_EditorSceneId);

				m_EditorSceneId = Wiwa::SceneManager::CreateScene();
				Wiwa::SceneManager::getScene(m_EditorSceneId)->GetCameraManager().CreatePerspectiveCamera(0.1f, 1000.0f);
				m_EditorScene = Wiwa::SceneManager::getScene(m_EditorSceneId);
				m_OpenedScenePath = "";

				m_EditorScene->GetEntityManager().AddSystemToWhitelist("MeshRenderer");
				m_EditorScene->GetEntityManager().SetInitSystemsOnApply(false);

				Wiwa::SceneManager::SetScene(m_EditorSceneId, false);

			}
			if (ImGui::MenuItem(ICON_FK_FOLDER " Open scene", ""))
			{
				StopScene();
				OpenScene();
			}
			if (ImGui::MenuItem(ICON_FK_FLOPPY_O " Save scene", ""))
			{
				StopScene();
				SaveScene();
			}
			if (ImGui::MenuItem(ICON_FK_FILES_O " Save scene as..."))
			{
				StopScene();
				SaveSceneAs();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Import as ozz")) {
				std::string filePath = Wiwa::FileDialogs::OpenFile("fbxozz (*.fbxozz)\0*.fbxozz\0");
				if (!filePath.empty())
				{
					WI_INFO("Importing as ozz at path {0}", filePath.c_str());

					ImportToOzz(filePath);
				}
			}
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_FK_TIMES " Close", "ALT + Q"))
				Wiwa::Application::Get().Quit();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Project"))
		{
			if (ImGui::MenuItem("Build"))
			{
				BuildProject();
			}
			if (ImGui::MenuItem("Project settings"))
				m_ProjectPanel->SwitchActive();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			int i = 1;
			for (auto &p : m_Panels)
			{
				if (ImGui::MenuItem(p->GetName(), "", p->active))
					p->SwitchActive();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Layout"))
		{
			if (ImGui::MenuItem("Scene layout"))
			{
				LoadLayout("config/layouts/scene_layout.wilayout");
			}

			/*if (ImGui::MenuItem("UI layout")) {

			}*/

			static size_t save_ind = 0;
			static LayoutData *save_ldata = nullptr;

			size_t csize = m_CustomLayouts.size();

			for (size_t i = 0; i < csize; i++)
			{
				LayoutData &ldata = m_CustomLayouts[i];

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				if (ImGui::Button(ldata.name.c_str()))
				{
					LoadLayout(ldata.path.c_str());
				}
				ImGui::PopStyleColor();

				ImGui::SameLine();

				ImGui::PushID(i);

				if (ImGui::Button("Save"))
				{
					save_ldata = &ldata;
					save_ind = i;

					ImGui::OpenPopup("save_layout");
				}

				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					std::filesystem::path p = ldata.path;
					p.replace_extension(".ini");

					Wiwa::FileSystem::Remove(ldata.path.c_str());
					Wiwa::FileSystem::Remove(p.string().c_str());

					m_CustomLayouts.erase(m_CustomLayouts.begin() + i);
					i--;
					csize--;
				}
				ImGui::PopID();
			}

			if (ImGui::Button("New layout"))
			{
				ImGui::OpenPopup("create_layout");
			}

			ImGui::PushID(save_ind);
			if (ImGui::BeginPopup("save_layout"))
			{
				ImGui::Text("Save layout");

				std::string msg = "Are you sure you want to override layout [";
				msg += save_ldata->name;
				msg += "] with the current layout setup? This change can't be undone.";

				ImGui::Text(msg.c_str());

				if (ImGui::Button("Save"))
				{
					SaveLayout(*save_ldata);
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
			ImGui::PopID();

			if (ImGui::BeginPopup("create_layout"))
			{
				static char name[64] = {0};

				ImGui::Text("Create Layout");
				ImGui::InputText("Layout name", name, 64);

				if (ImGui::Button("Create"))
				{
					std::string compact_name = name;

					size_t index = 0;

					do
					{
						index = compact_name.find(' ', index);

						if (index != compact_name.npos)
						{
							compact_name[index] = '_';
						}
					} while (index != compact_name.npos);

					std::string path = "config/layouts/";

					std::string wipath = path + compact_name + ".wilayout";

					LayoutData ldata;
					ldata.name = name;
					ldata.path = wipath;

					SaveLayout(ldata);

					m_CustomLayouts.push_back(ldata);

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Script"))
		{
			if (ImGui::MenuItem("Reload assembly", "ALT + R")) {
				

				// Reload app assembly
				Wiwa::Application::Get().UnloadGameAssembly();

				Wiwa::FileSystem::Copy("game_assembly/build/WiwaGameAssembly.dll", "resources/WiwaGameAssembly.dll");

				Wiwa::Application::Get().LoadGameAssembly();
			}
			if (ImGui::MenuItem("Open Solution"))
				system("call tools/opensln.bat game_assembly/WiwaGameAssembly.sln");
			ImGui::Checkbox("Build automatic", &s_BuildAutomatic);
			ImGui::SameLine();

			HelpMarker("Enables/Disables the automatic assembly compilation");

			


			if (ImGui::MenuItem("Compile"))			
				system("call tools/buildgameasm.bat");
			ImGui::SameLine();
			HelpMarker("Builds the solution with MsBuild, you'll need to firs add it to the PATH env var to compile");

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("GuiDemo", "", m_ShowDemo))
				m_ShowDemo = !m_ShowDemo;

			if (ImGui::MenuItem("Documentation"))
				Wiwa::Application::Get().OpenDir("https://github.com/ZaroDev/WiwaEngine/wiki");

			if (ImGui::MenuItem(ICON_FK_GITHUB " Download Latest"))
				Wiwa::Application::Get().OpenDir("https://github.com/ZaroDev/WiwaEngine/releases");

			if (ImGui::MenuItem("Report a bug"))
				Wiwa::Application::Get().OpenDir("https://github.com/ZaroDev/WiwaEngine/issues");

			if (ImGui::MenuItem("About", "", m_About->active))
				m_About->SwitchActive();

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	ImGuiViewportP *viewport = (ImGuiViewportP *)(void *)ImGui::GetMainViewport();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	float height = ImGui::GetFrameHeight();

	if (ImGui::BeginViewportSideBar("##SecondaryMenuBar", viewport, ImGuiDir_Up, height, window_flags))
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
			if (ImGui::Button(ICON_FK_MOUSE_POINTER))
				m_GizmoType = -1;

			if (ImGui::Button(ICON_FK_ARROWS))
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;

			if (ImGui::Button(ICON_FK_REPEAT))
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;

			if (ImGui::Button(ICON_FK_EXPAND))
				m_GizmoType = ImGuizmo::OPERATION::SCALE;


			ImGui::SetCursorPosX(Wiwa::Application::Get().GetWindow().GetWidth() / 2 - 15.0f);
			const char *play = Wiwa::Time::IsPlaying() ? ICON_FK_STOP : ICON_FK_PLAY;

			bool is_playing = Wiwa::Time::IsPlaying();

			// Play button
			if (ImGui::Button(play))
			{
				InspectorPanel::s_EntitySet = false;
				if (!is_playing)
				{	
					SaveScene();
					PlayScene();
				}
				else
				{
					StopScene();
				}


			}

			if (ImGui::Button(ICON_FK_PAUSE))
			{
				if (is_playing)
				{
					Wiwa::Time::PauseUnPause();

					if (Wiwa::SceneManager::IsPlaying())
					{
						Wiwa::SceneManager::StopScene();
					}
					else
					{
						Wiwa::SceneManager::PlayScene();
					}
				}
			}


			if (ImGui::Button(ICON_FK_STEP_FORWARD))
			{
				if (is_playing)
				{
					Wiwa::Time::Step();
				}
			}
			ImGui::PushItemWidth(200);
			if (ImGui::SliderInt("Volume", &m_GameVolume, 0, 100, "%d", 0, 17))
				Audio::ChangeMasterVolume(m_GameVolume);
			ImGui::PopItemWidth();
			
			ImGui::PopStyleColor();
			ImGui::EndMenuBar();
		}

		
		ImGui::End();
	}

	viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
	window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	height = ImGui::GetFrameHeight();
	

	if (ImGui::BeginViewportSideBar("##MainStatusBar", viewport, ImGuiDir_Down, height, window_flags))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::Button("Assets browser"))
			{
				OpenCloseAssetsFolder();
			}

			ImGuiLog log = Wiwa::Application::Get().GetImGuiLayer().GetConsole();

			ImGui::PushStyleColor(ImGuiCol_Button, { 0.0, 0, 0, 0 });
			ImGui::AlignTextToFramePadding();

			char buff[16];
			sprintf_s(buff, 16, "%i", log.infoCount);
			ImGui::Text(buff);
			ImGui::Text(ICON_FK_INFO_CIRCLE);

			sprintf_s(buff, 16, "%i", log.warnCount);
			ImGui::Text(buff);
			ImGui::Text(ICON_FK_EXCLAMATION_TRIANGLE);

			sprintf_s(buff, 16, "%i", log.errorCount);
			ImGui::Text(buff);
			ImGui::PopStyleColor();
			ImGui::Text(ICON_FK_EXCLAMATION_CIRCLE);
			if (!log.Buf.empty())
			{
				const char* beg = log.Buf.begin() + log.LineOffsets[log.LineOffsets.Size - 2];
				ImGui::TextUnformatted(beg, log.Buf.end());
			}
			
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}
	
}

void EditorLayer::PlayScene()
{

	if (m_OpenedScenePath != "")
	{
		Wiwa::Time::Play();
		Wiwa::Time::Update();

		m_SimulationSceneId = Wiwa::SceneManager::LoadScene(m_OpenedScenePath.c_str(), Wiwa::SceneManager::LOAD_SEPARATE);
		Wiwa::Scene* sc = Wiwa::SceneManager::getScene(m_SimulationSceneId);
		sc->GetEntityManager().AddSystemToWhitelist("MeshRenderer");
		sc->GetEntityManager().AddSystemToWhitelist("OzzAnimationSystem");

		// For debug purposes
		std::string ex = sc->getName();
		ex += "_execution";
		sc->ChangeName(ex.c_str());

		Wiwa::SceneManager::PlayScene();
	}
}

void EditorLayer::StopScene()
{
	if (!Wiwa::Time::IsPlaying())
		return;
	Wiwa::Time::Stop();

	Audio::StopAllEvents();

	// Wait until all events are stopped
	Sleep(10);

	// Unload simulated scene but keep resources for the editor
	Wiwa::SceneManager::UnloadScene(m_SimulationSceneId, false);

	// Set editor scene again
	Wiwa::SceneManager::SetScene(m_EditorSceneId, false);

	// Stop scene from being played
	Wiwa::SceneManager::StopScene();
}

void EditorLayer::OpenCloseAssetsFolder()
{
	m_Assets->active = !m_Assets->active;
}

void EditorLayer::SaveProjectAs()
{
	std::string filePath = Wiwa::FileDialogs::SaveFile("Wiwa Project (*.wiproject)\0*.wiproject\0");
	if (!filePath.empty())
	{
		Wiwa::ProjectManager::SaveProjectAs(filePath.c_str());
		WI_INFO("Succesfully saved project at path {0}", filePath.c_str());
	}
}

void EditorLayer::OpenProject()
{
	std::string filePath = Wiwa::FileDialogs::OpenFile("Wiwa Project (*.wiproject)\0*.wiproject\0");
	if (!filePath.empty())
	{
		Wiwa::ProjectManager::OpenProject(filePath.c_str());
		WI_INFO("Succesfully opened project at path {0}", filePath.c_str());
	}
}

void EditorLayer::NewProject()
{
	std::string filePath = Wiwa::FileDialogs::SaveFile("Wiwa Project (*.wiproject)\0*.wiproject\0");
	if (!filePath.empty())
	{
		Wiwa::ProjectManager::CreateProject(filePath.c_str());
		WI_INFO("Succesfully created project at path {0}", filePath.c_str());
	}
}

void EditorLayer::SaveSceneAs()
{
	std::string filePath = Wiwa::FileDialogs::SaveFile("Wiwa Scene (*.wiscene)\0*.wiscene\0");
	if (!filePath.empty())
	{
		if (filePath.find(".wiscene") == filePath.npos)
		{
			filePath += ".wiscene";
		}
		Wiwa::SceneManager::SaveScene(Wiwa::SceneManager::getActiveSceneId(), filePath.c_str());
		m_OpenedScenePath = filePath;
		std::filesystem::path name = filePath;
		name = name.filename();
		name.replace_extension();

		std::filesystem::path curpath = std::filesystem::current_path();

		std::filesystem::path path = filePath;
		path = path.lexically_relative(curpath);

		Wiwa::ProjectManager::AddScene(name.string().c_str(), path.string().c_str());
		WI_INFO("Succesfully saved scene at path {0}", filePath.c_str());
	}
}

void EditorLayer::SaveScene()
{
	if (m_OpenedScenePath.empty())
	{
		SaveSceneAs();
	}
	else
	{
		Wiwa::SceneManager::SaveScene(Wiwa::SceneManager::getActiveSceneId(), m_OpenedScenePath.c_str());
	}
}

void EditorLayer::OpenScene()
{
	std::string filePath = Wiwa::FileDialogs::OpenFile("Wiwa Scene (*.wiscene)\0*.wiscene\0");
	if (!filePath.empty())
	{
		LoadScene(filePath);

		WI_INFO("Succesfully opened scene at path {0}", filePath.c_str());
		InspectorPanel::s_EntitySet = false;
	}
}

void EditorLayer::DockSpace()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground;

	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	static bool show = false;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &show, window_flags);

	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);
	// Submit the DockSpace
	ImGuiIO &io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();
}

void EditorLayer::LoadPanelConfig()
{
	Wiwa::JSONDocument config("config/panels.json");
	if (!config.IsObject())
		return;
	if (config.HasMember("sol_version"))
		s_SolVersion = config["sol_version"].as_string();
	if (config.HasMember("build_conf"))
		s_BuildConf = config["build_conf"].as_string();
	if (config.HasMember("build_auto"))
		s_BuildAutomatic = config["build_auto"].as_bool();

	if (config.HasMember("custom_layouts"))
	{
		Wiwa::JSONValue clayouts = config["custom_layouts"];

		uint32_t size = clayouts.Size();

		for (uint32_t i = 0; i < size; i++)
		{
			Wiwa::JSONValue layout = clayouts[i];

			LayoutData ldata;
			ldata.name = layout["name"].as_string();
			ldata.path = layout["path"].as_string();

			m_CustomLayouts.push_back(ldata);
		}
	}

	size_t psize = m_Panels.size();

	for (size_t i = 0; i < psize; i++)
	{
		const auto &p = m_Panels[i];

		if (config.HasMember(p->GetName()))
		{
			p->active = config[p->GetName()];
		}
	}
}

void EditorLayer::SavePanelConfig()
{
	Wiwa::JSONDocument config;

	config.AddMember("sol_version", s_SolVersion.c_str());
	config.AddMember("build_conf", s_BuildConf.c_str());
	config.AddMember("build_auto", s_BuildAutomatic);

	Wiwa::JSONValue clayouts = config.AddMemberArray("custom_layouts");

	size_t lsize = m_CustomLayouts.size();

	for (size_t i = 0; i < lsize; i++)
	{
		LayoutData &ldata = m_CustomLayouts[i];

		Wiwa::JSONValue val = clayouts.PushBackObject();
		val.AddMember("name", ldata.name.c_str());
		val.AddMember("path", ldata.path.c_str());
	}

	size_t psize = m_Panels.size();

	for (size_t i = 0; i < psize; i++)
	{
		const auto &p = m_Panels[i];

		config.AddMember(p->GetName(), p->active);
	}

	config.save_file("config/panels.json");
}

void EditorLayer::ImportToOzz(std::string& file)
{
	std::filesystem::path file_path = file;

	std::filesystem::path filename = file_path.filename();
	filename.replace_extension();

	// Skeleton output path
	std::filesystem::path skeleton_path = file_path;
	skeleton_path.replace_filename(filename.string() + "_skeleton");
	skeleton_path.replace_extension(".skeleton");

	// Animations output path
	std::filesystem::path anim_out_path = file_path;
	anim_out_path.replace_filename(filename.string() + "_*");
	anim_out_path.replace_extension(".anim");

	// Mesh output path
	std::filesystem::path mesh_out_path = file_path;
	mesh_out_path.replace_filename(filename.string() + "_mesh");
	mesh_out_path.replace_extension(".mesh");

	// === Setup ozz import json config ===
	Wiwa::JSONDocument anim_import;
	
		// Skeleton settings
		Wiwa::JSONValue skeleton_obj = anim_import.AddMemberObject("skeleton");
			
			// Skeleton filename
			skeleton_obj.AddMember("filename", skeleton_path.string().c_str());
	
			// Skeleton import settings
			Wiwa::JSONValue skeleton_import_obj = skeleton_obj.AddMemberObject("import");
			skeleton_import_obj.AddMember("enable", true);
			skeleton_import_obj.AddMember("raw", false);

		// Animation settings
		Wiwa::JSONValue animations_obj = anim_import.AddMemberArray("animations");
			Wiwa::JSONValue anim_obj = animations_obj.PushBackObject();
			anim_obj.AddMember("clip", "*");
			anim_obj.AddMember("filename", anim_out_path.string().c_str());

	anim_import.save_file("config\\import_anim.json");

	// === Import ozz ===
	std::string cmd = "tools\\import_anim.bat";
	cmd += " \"";
	cmd += file_path.string().c_str(); // Path of file to import (.fbx)
	cmd += "\" \"";
	cmd += mesh_out_path.string().c_str(); // Path of output mesh
	cmd += "\" \"";
	cmd += skeleton_path.string().c_str(); // Path of skeleton to import mesh
	cmd += "\"";
	system(cmd.c_str());
}

void EditorLayer::LoadCallback()
{
	Wiwa::OnLoadEvent event;
	m_EventCallback(event);
}

void EditorLayer::SaveCallback()
{
	Wiwa::OnSaveEvent event;
	m_EventCallback(event);
}

bool EditorLayer::OnKeyPressed(Wiwa::KeyPressedEvent &e)
{
	// Shortcuts
	if (e.IsRepeat())
		return false;

	bool control = Wiwa::Input::IsKeyPressed(Wiwa::Key::LeftControl) || Wiwa::Input::IsKeyPressed(Wiwa::Key::RightControl);
	bool shift = Wiwa::Input::IsKeyPressed(Wiwa::Key::LeftShift) || Wiwa::Input::IsKeyPressed(Wiwa::Key::RightShift);
	bool alt = Wiwa::Input::IsKeyPressed(Wiwa::Key::LeftAlt) || Wiwa::Input::IsKeyPressed(Wiwa::Key::RightAlt);
	if (Wiwa::Input::IsMouseButtonPressed(1) || Wiwa::Input::IsMouseButtonPressed(0))
		return false;
	switch (e.GetKeyCode())
	{
	case Wiwa::Key::N:
	{
		if (control)
		{
			NewProject();
		} // NewScene();

		break;
	}
	case Wiwa::Key::O:
	{
		if (control)
		{
			OpenProject();
		} // OpenScene();

		break;
	}
	case Wiwa::Key::S:
	{
		if (control)
		{
			if (shift)
				SaveProjectAs();
			else
				Wiwa::ProjectManager::SaveProject();
		}

		break;
	}

	// Scene Commands
	case Wiwa::Key::D:
	{
		if (control)
		{

		} // OnDuplicateEntity();

		break;
	}

	// Gizmos
	case Wiwa::Key::Q:
	{
		if (alt)
			Wiwa::Application::Get().Quit();
		if (!ImGuizmo::IsUsing())
		{
			m_GizmoType = -1;
		}
		break;
	}
	case Wiwa::Key::W:
	{
		if (Wiwa::Input::IsMouseButtonPressed(1))
		{
			break;
		}
		if (!ImGuizmo::IsUsing())
		{
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		}
		break;
	}
	case Wiwa::Key::E:
	{
		if (!ImGuizmo::IsUsing())
		{
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		}
		break;
	}
	case Wiwa::Key::R:
	{
		if (!ImGuizmo::IsUsing())
		{
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
		}
		break;
	}
	case Wiwa::Key::Space:
	{
		if (control)
			OpenCloseAssetsFolder();
	}
	}
	return false;
}

bool EditorLayer::OnLoad(Wiwa::OnLoadEvent &e)
{
	LoadPanelConfig();

	return false;
}

bool EditorLayer::OnSave(Wiwa::OnSaveEvent &e)
{
	SavePanelConfig();

	return false;
}

bool EditorLayer::OnWindowClose(Wiwa::WindowCloseEvent &e)
{
	SaveCallback();
	return false;
}

void EditorLayer::ExecuteMainThreadQueue()
{
	for (auto &func : m_EditorThreadQueue)
		func();

	m_EditorThreadQueue.clear();
}
