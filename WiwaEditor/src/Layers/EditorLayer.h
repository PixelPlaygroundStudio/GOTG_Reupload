#pragma once

#include <Wiwa/core/Layer.h>
#include <Wiwa/utilities/containers/List.h>

#include "Panels/Panel.h"
#include <Wiwa/utilities/functions/Action.h>
#include <Wiwa/Events/Event.h>

#include "../Events/PanelEvents.h"

#include "Panels/AboutPanel.h"
#include "Panels/ConfigurationPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/ScenePanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/AssetsPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/MeshViewPanel.h"
#include "Panels/MaterialPanel.h"
#include "Panels/ProjectPanel.h"
#include "Panels/SystemsPanel.h"
#include "Panels/GamePanel.h"
#include "Panels/CamerasPanel.h"
#include "Panels/ResourcesPanel.h"
#include "Panels/ImportPanel.h"
#include "Panels/ShaderPanel.h"
#include "Panels/AudioPanel.h"
#include "Panels/EnvoirmentPanel.h"

#include "Panels/AnimatorPanel.h"
#include "Panels/AnimationPanel.h"

#include "Panels/UIPanel.h"
#include "Panels/UIEditorPanel.h"
#include "Panels/DialogPanel.h"
#include "Panels/GameLogPanel.h"
#include "Panels/InventoryPanel.h"
#include "Panels/AIPanel.h"

#include "Panels/animation/OzzAnimationPanel.h"
#include "Panels/EnemyPanel.h"
#include "Panels/ShadowsPanel.h"

#include <Wiwa/scene/SceneManager.h>

#include <memory>

typedef void *ImTextureID;

class EditorLayer : public Wiwa::Layer
{
	typedef std::function<void(Wiwa::Event &)> EventCallbackFn;

public:

	static EditorLayer& Get() { return *s_Instance; }

	EditorLayer();
	~EditorLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnImGuiRender() override;
	void OnEvent(Wiwa::Event &e) override;

	SceneId LoadScene(const std::string& m_Path);

	inline int GetGizmo() { return m_GizmoType; }

	void SubmitToMainThread(const std::function<void()> func);

	void StopScene();
	static void RegenSol();

	std::string getOpenedScenePath() { return m_OpenedScenePath; };
private:
	struct LayoutData {
		std::string name;
		std::string path;
	};

	void MainMenuBar();
	void PlayScene();
	void OpenCloseAssetsFolder();
	void SaveProjectAs();
	void OpenProject();
	void NewProject();
	void DockSpace();

	// Scene functions
	void SaveSceneAs();
	void SaveScene();
	void OpenScene();

	void LoadPanelConfig();
	void SavePanelConfig();

	void ImportToOzz(std::string& file);

	void LoadCallback();
	void SaveCallback();

	bool OnKeyPressed(Wiwa::KeyPressedEvent &e);
	bool OnLoad(Wiwa::OnLoadEvent &e);
	bool OnSave(Wiwa::OnSaveEvent &e);
	bool OnWindowClose(Wiwa::WindowCloseEvent &e);

	void ExecuteMainThreadQueue();
	static void RegenSolutionThread();

	// Layouts
	void LoadLayout(const char* path);
	void SaveLayout(LayoutData& ldata);

	void BuildProject();
private:
	bool m_ShowConsole = false;
	bool m_ShowDemo = false;

	std::unique_ptr<Panel> m_About;
	std::unique_ptr<Panel> m_Configuration;
	std::unique_ptr<Panel> m_Console;
	std::unique_ptr<Panel> m_Scene;
	std::unique_ptr<Panel> m_Hierarchy;
	std::unique_ptr<Panel> m_Assets;
	std::unique_ptr<Panel> m_Inspector;
	std::unique_ptr<Panel> m_MeshView;
	std::unique_ptr<Panel> m_MaterialEditor;
	std::unique_ptr<Panel> m_ProjectPanel;
	std::unique_ptr<Panel> m_SystemsPanel;
	std::unique_ptr<Panel> m_GamePanel;
	std::unique_ptr<Panel> m_CamerasPanel;
	std::unique_ptr<Panel> m_ResourcesPanel;
	std::unique_ptr<Panel> m_ImportPanel;
	std::unique_ptr<Panel> m_ShaderPanel;
	std::unique_ptr<Panel> m_AudioPanel;
	std::unique_ptr<Panel> m_EnvPanel;
	std::unique_ptr<Panel>  m_AnimatorPanel;
	std::unique_ptr<Panel> m_AnimationPanel;
	std::unique_ptr<Panel> m_UiPanel;
	std::unique_ptr<Panel> m_DialogPanel;
	std::unique_ptr<Panel> m_UiEditorPanel;
	std::unique_ptr<Panel> m_GameLogPanel;
	std::unique_ptr<Panel> m_InventoryPanel;
	std::unique_ptr<Panel> m_AiPanel;
	std::unique_ptr<Panel> m_OzzAnimationPanel;

	std::unique_ptr<Panel> m_EnemyPanel;
	std::unique_ptr<Panel> m_ShadowsPanel;

	std::vector<Panel *> m_Panels;
	std::vector<Panel *> m_Settings;

	// Editor layouts
	std::string m_ActiveLayout;
	bool m_ReloadLayout;

	std::vector<LayoutData> m_CustomLayouts;

	Action<Wiwa::Event &> m_EventCallback;

	SceneId m_EditorSceneId;

	SceneId m_SimulationSceneId;

	std::string m_OpenedScenePath;

	Wiwa::Scene *m_EditorScene;

	int m_GizmoType = -1;

	std::vector<std::function<void()>> m_EditorThreadQueue;
	std::mutex m_EditorThreadMutex;

	int m_GameVolume = 0;
public:
	static std::string s_SolVersion;
	static std::string s_BuildConf;
	static bool s_BuildAutomatic;
private:
	static EditorLayer* s_Instance;
	static std::thread* s_RegenThread;

};