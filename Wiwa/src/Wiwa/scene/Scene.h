#pragma once

#include <Wiwa/core/Core.h>

#include <vector>
#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/physics/PhysicsManager.h>
#include <Wiwa/utilities/render/Camera.h>
#include <Wiwa/utilities/render/CameraManager.h>

#include <Wiwa/utilities/render/InstanceRenderer.h>

namespace Wiwa {
	class LightManager;
	class GuiManager;
	class DialogManager;
	class DialogEventManager;


	class WI_API Scene {
	public:
		Scene();
		~Scene();

		enum State {
			SCENE_ENTERING,
			SCENE_LOOP,
			SCENE_LEAVING
		};

		void Start();

		void Awake();
		void Init();
		void Update();

		void ModuleUpdate();

		void Load();
		void Unload(bool unload_resources=true);

		State getState() { return m_CurrentState; }

		void ChangeScene(size_t scene, int flags);

		void SwapPauseActive() { pausedGame = !pausedGame; }

		bool IsScenePaused() { return pausedGame; }

		uint32_t CreateInstanceRenderer();
		EntityManager& GetEntityManager() { return m_EntityManager; }
		CameraManager& GetCameraManager() { return *m_CameraManager; }
		PhysicsManager& GetPhysicsManager() { return *m_PhysicsManager; }
		std::vector<InstanceRenderer*>& GetInstanceRenderers() { return m_InstanceRenderers; }
		InstanceRenderer* GetInstanceRenderer(uint32_t index) { return m_InstanceRenderers[index]; }
		LightManager& GetLightManager() { return *m_LightManager; }
		GuiManager& GetGuiManager() { return *m_GuiManager; }
		DialogManager& GetDialogManager() { return *m_DialogManager; }
		DialogEventManager& GetDialogEventManager() { return *m_DialogEventManager; }
		inline const char* getName() { return m_Name.c_str(); }
		inline void ChangeName(const char* name) { m_Name = name; }
	protected:
		virtual void ProcessInput() {}

		void UpdateEnter();
		virtual void UpdateLoop() {}
		void UpdateLeave();

		void RenderEnter();
		virtual void RenderLoop() {}
		void RenderLeave();

		size_t mMaxTimeEntering, mMaxTimeLeaving = 0;

		EntityManager m_EntityManager;
		CameraManager* m_CameraManager = nullptr;
		PhysicsManager* m_PhysicsManager = nullptr;
		LightManager* m_LightManager = nullptr;
		GuiManager* m_GuiManager = nullptr;
		DialogManager* m_DialogManager = nullptr;
		DialogEventManager* m_DialogEventManager = nullptr;

		std::vector<std::string> m_AudioBanks;
	private:
		std::vector<InstanceRenderer*> m_InstanceRenderers;

		Renderer2D::InstanceData m_TransitionInstance;

		State m_CurrentState = SCENE_ENTERING;
		size_t m_TransitionTimer = 0;
		size_t m_SceneToChange = 0;
		int m_SceneChangeFlags = 0;
		std::string m_Name = "Default scene";
		bool pausedGame = false;
	};
}