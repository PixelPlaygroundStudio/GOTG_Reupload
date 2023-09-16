#pragma once

#include <Wiwa/core/Core.h>

#include "Scene.h"

#include <Wiwa/Ui/UiManager.h>
#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/utilities/filesystem/FileSystem.h>
#include <Wiwa/game/GameStateManager.h>
#include <vector>
#include <Wiwa/utilities/containers/Memory.h>

#include <thread>

typedef size_t SceneId;

namespace Wiwa {
	class WI_API SceneManager {
	private:
		static std::vector<Scene*> m_Scenes;
		static SceneId m_ActiveScene;

		static InstanceRenderer* m_InstanceRenderer;
		static size_t m_ProgressDot1;
		static size_t m_ProgressDot2;
		static size_t m_ProgressDot3;
		static size_t m_BackGround_Starlord;
		static size_t m_BackGround_Rocket;



		static std::vector<SceneId> m_RemovedSceneIds;

		static void LoadEntity(Memory& scene_data, EntityId parent, EntityManager& em, bool is_parent);
		static void SaveEntity(File& scene_file, EntityId eid, EntityManager& em);

		static bool _loadSceneImpl(Scene* scene, Memory& scene_data);
		static bool _saveSceneImpl(Scene* scene, Memory& scene_data);

		SceneManager();

		static bool m_PlayScene;

		// For scene loading inside systems
		static bool m_LoadScene;
		static std::string m_LoadPath;
		static int m_LoadFlags;
		static std::atomic<bool> m_LoadedScene;
		static std::atomic<bool> m_UnloadedScene;
		static std::atomic<int> m_LoadingProgress;
		static std::thread m_LoadThread;
		static std::thread m_LoadScreenThread;
		static Scene* m_SceneLoading;
		static SceneId m_SceneLoadingId;

		static void LoadSceneJob(Wiwa::Memory& scene_data);
		static void LoadingScreenJob();
		static void ReLoadTransition();

	public:
		enum LoadFlags {
			// Unloads current scene
			UNLOAD_CURRENT = 1,
			// Unloads current resources (ignored if UNLOAD_CURRENT flag is disabled)
			UNLOAD_RESOURCES = 2,
			// Loads the scene in a separate scene instance
			LOAD_SEPARATE = 4,
			// Loads the scene and appends it into the current scene (ignored if LOAD_SEPARATE is enabled)
			LOAD_APPEND = 8,
			// Load but don't awake/init
			LOAD_NO_INIT = 16,
			// Default settings = UNLOAD_CURRENT | UNLOAD_RESOURCES
			LOAD_DEFAULT = UNLOAD_CURRENT | UNLOAD_RESOURCES | LOAD_SEPARATE
		};

		static void Awake();
		static void Init();
		static void Update();
		static void ModuleInit();
		static void ModuleUpdate();

		static void PlayScene() { m_PlayScene = true; }

		static void StopScene() { m_PlayScene = false; }

		static bool IsPlaying() { return m_PlayScene; }

		static void CleanUp();
		
		static void SetScene(SceneId sceneId, bool init);

		static void ChangeSceneByIndex(SceneId sceneId, int flags = LOAD_DEFAULT);
		static void ChangeSceneByName(const char* name, int flags = LOAD_DEFAULT);

		static SceneId getActiveSceneId() { return m_ActiveScene; }
		static Scene* getActiveScene() { return m_Scenes[m_ActiveScene]; }

		static size_t getSceneSize() { return m_Scenes.size(); }

		static size_t CreateScene();
		static Scene* getScene(SceneId sceneId) { return m_Scenes[sceneId]; }
		static std::vector<Scene*>& getScenes() { return m_Scenes; }

		static void SaveScene(SceneId scene_id, const char* scene_path);

		static void PauseCurrentScene();

		// Load a scene file to memory
		static SceneId LoadScene(const char* scene_path, int flags=LOAD_DEFAULT);

		// Load scene to memory by index
		static void LoadSceneByIndex(size_t scene_index, int flags = LOAD_DEFAULT);

		// Load scene to memory by name
		static void LoadSceneByName(const char* scene_name, int flags = LOAD_DEFAULT);

		// Unload a scene id
		static void UnloadScene(SceneId scene_id, bool unload_resources=true);

		static void FixSceneLoading();
		static bool isLoadingScene;
	};
}