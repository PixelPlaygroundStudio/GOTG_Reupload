#include <wipch.h>
#include "SceneManager.h"
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/ecs/systems/ParticleSystem.h>
#include <Wiwa/events/Event.h>
#include <Wiwa/core/Resources.h>
#include <Wiwa/events/ApplicationEvent.h>
#include <Wiwa/audio/Audio.h>
#include <Wiwa/utilities/render/LightManager.h>
#include <Wiwa/core/ProjectManager.h>
#include <Wiwa/AI/AI_Crowd.h>
#include <Wiwa/game/GameMusicManager.h>

#include <glew.h>

namespace Wiwa
{
	std::vector<Scene*> SceneManager::m_Scenes;
	SceneId SceneManager::m_ActiveScene = -1;
	bool SceneManager::m_PlayScene = true;
	bool SceneManager::isLoadingScene = false;

	// For loading inside systems
	bool SceneManager::m_LoadScene = false;
	std::string SceneManager::m_LoadPath = "";
	int SceneManager::m_LoadFlags = 0;

	std::vector<SceneId> SceneManager::m_RemovedSceneIds;

	std::atomic<bool> SceneManager::m_LoadedScene = false;
	std::atomic<bool> SceneManager::m_UnloadedScene = false;
	std::atomic<int> SceneManager::m_LoadingProgress = 0;
	std::thread SceneManager::m_LoadThread;
	std::thread SceneManager::m_LoadScreenThread;
	Scene* SceneManager::m_SceneLoading = nullptr;
	SceneId SceneManager::m_SceneLoadingId = -1;

	InstanceRenderer* SceneManager::m_InstanceRenderer = nullptr;
	size_t SceneManager::m_ProgressDot1 = WI_INVALID_INDEX;
	size_t SceneManager::m_ProgressDot2 = WI_INVALID_INDEX;
	size_t SceneManager::m_ProgressDot3 = WI_INVALID_INDEX;
	size_t SceneManager::m_BackGround_Starlord = WI_INVALID_INDEX;
	size_t SceneManager::m_BackGround_Rocket = WI_INVALID_INDEX;


	void SceneManager::Awake()
	{
		m_Scenes[m_ActiveScene]->Awake();
	}

	void SceneManager::Init()
	{
		m_Scenes[m_ActiveScene]->Init();
	}

	void SceneManager::Update()
	{
		m_Scenes[m_ActiveScene]->Update();
	}

	void SceneManager::ModuleInit()
	{
		m_InstanceRenderer = new InstanceRenderer(10);

		m_InstanceRenderer->Init("resources/shaders/instanced_tex_color");
		ReLoadTransition();
	}

	void SceneManager::ModuleUpdate()
	{
		OPTICK_EVENT("Scene Update");

		if (m_LoadScene) {
			m_ActiveScene = LoadScene(m_LoadPath.c_str(), m_LoadFlags);
			GameMusicManager::OnSceneChage(m_LoadPath.c_str());

			m_LoadScene = false;
		}

		m_Scenes[m_ActiveScene]->ModuleUpdate();

		if (m_PlayScene)
		{
			Update();
			GameStateManager::UpdateRoomState();
		}
	}

	void SceneManager::CleanUp()
	{
		size_t scsize = m_Scenes.size();

		for (size_t i = 0; i < scsize; i++)
		{
			delete m_Scenes[i];
		}

		delete CameraManager::editorCamera;
		delete m_InstanceRenderer;
	}

	SceneId SceneManager::CreateScene()
	{
		Scene* sc = new Scene();
		sc->Start();

		SceneId scene_id;

		if (m_RemovedSceneIds.size() > 0)
		{
			scene_id = m_RemovedSceneIds[m_RemovedSceneIds.size() - 1];
			m_RemovedSceneIds.pop_back();

			m_Scenes[scene_id] = sc;
		}
		else
		{
			scene_id = m_Scenes.size();
			m_Scenes.push_back(sc);
		}

		return scene_id;
	}

	void SceneManager::LoadEntity(Memory& scene_data, EntityId parent, EntityManager& em, bool is_parent)
	{
		size_t e_name_len;
		char* e_name_c;
		std::string e_name;

		// Read entity name
		scene_data.Read(&e_name_len, sizeof(size_t));
		e_name_c = new char[e_name_len];
		scene_data.Read(e_name_c, e_name_len);
		e_name = e_name_c;
		delete[] e_name_c;

		EntityId eid;

		if (is_parent)
		{
			eid = em.CreateEntity(e_name.c_str());
		}
		else
		{
			eid = em.CreateEntity(e_name.c_str(), parent);
		}

		size_t component_size;

		// Read component count
		scene_data.Read(&component_size, sizeof(size_t));

		// For each component in entity
		for (size_t i = 0; i < component_size; i++)
		{
			ComponentHash c_hash;
			size_t c_size;
			byte* data;

			// Read component hash, size and data
			scene_data.Read(&c_hash, sizeof(size_t));
			scene_data.Read(&c_size, sizeof(size_t));
			data = new byte[c_size];
			scene_data.Read(data, c_size);

			byte* component = em.AddComponent(eid, c_hash, data);
			delete[] data;

			size_t mesh_hash = em.GetComponentType(em.GetComponentId<Mesh>())->hash;

			if (c_hash == mesh_hash)
			{
				Mesh* mesh = (Mesh*)component;

				size_t meshpath_size = strlen(mesh->mesh_path);
				if (meshpath_size > 0)
					mesh->meshId = Resources::Load<Model>(mesh->mesh_path);

				size_t matpath_size = strlen(mesh->mat_path);
				if (matpath_size > 0)
					mesh->materialId = Resources::Load<Material>(mesh->mat_path);
			}
		}

		size_t system_count;

		// Read system count
		scene_data.Read(&system_count, sizeof(size_t));

		if (system_count > 0)
		{
			SystemHash* system_hashes = new SystemHash[system_count];

			// Read system hashes
			scene_data.Read(system_hashes, system_count * sizeof(SystemHash));

			for (size_t i = 0; i < system_count; i++)
			{
				em.ApplySystem(eid, system_hashes[i]);
			}
		}

		// Check for child entities
		size_t children_size;

		// Read children size (size >= 0)
		scene_data.Read(&children_size, sizeof(size_t));

		for (size_t i = 0; i < children_size; i++)
		{
			LoadEntity(scene_data, eid, em, false);
		}
	}

	void SceneManager::SaveEntity(File& scene_file, EntityId eid, EntityManager& em)
	{
		const char* e_name = em.GetEntityName(eid);
		size_t e_name_len = strlen(e_name) + 1;

		// Save entity name
		scene_file.Write(&e_name_len, sizeof(size_t));
		scene_file.Write(e_name, e_name_len);

		std::map<ComponentId, size_t>& components = em.GetEntityComponents(eid);
		std::map<ComponentId, size_t>::iterator c_it;
		size_t component_size = components.size();

		// Save component count for this entity
		scene_file.Write(&component_size, sizeof(size_t));

		// For each component in entity
		for (c_it = components.begin(); c_it != components.end(); c_it++)
		{
			ComponentId c_id = c_it->first;
			const Type* c_type = em.GetComponentType(c_id);
			size_t c_size = c_type->size;
			byte* c_data = em.GetComponent(eid, c_id, c_size);

			// Save component hash, size and data
			scene_file.Write(&c_type->hash, sizeof(size_t));
			scene_file.Write(&c_size, sizeof(size_t));
			scene_file.Write(c_data, c_size);
		}

		// Save entity systems
		const std::vector<SystemHash>& system_list = em.GetEntitySystemHashes(eid);
		size_t system_count = system_list.size();

		// Save system count
		scene_file.Write(&system_count, sizeof(size_t));

		if (system_count > 0)
		{
			// Save system hashes
			scene_file.Write(system_list.data(), system_count * sizeof(SystemHash));
		}

		// Check for child entities
		std::vector<EntityId>* children = em.GetEntityChildren(eid);
		size_t children_size = children->size();

		// Save children size (size >= 0)
		scene_file.Write(&children_size, sizeof(size_t));

		// Recursively save each child entity
		for (size_t i = 0; i < children_size; i++)
		{
			SaveEntity(scene_file, children->at(i), em);
		}
	}

	bool SceneManager::_loadSceneImpl(Scene* scene, Memory& scene_data)
	{
		// Load GuiControls
		GuiManager& gm = scene->GetGuiManager();
		if (&gm)
		{
			char* textGui_c;
			size_t textGuiLen;
			scene_data.Read(&textGuiLen, sizeof(size_t));

			if (textGuiLen > 0) {
				textGui_c = new char[textGuiLen];
				scene_data.Read(textGui_c, textGuiLen);

				gm.SetCurrentPrefabWiGui(textGui_c);

				gm.LoadWiUI(textGui_c);
			}
		}

		m_LoadingProgress += 1; // 2

		isLoadingScene = true;

		// Load cameras
		CameraManager& cm = scene->GetCameraManager();
		size_t camera_count;

		// Read camera count
		scene_data.Read(&camera_count, sizeof(size_t));
		for (size_t i = 0; i < camera_count; i++)
		{
			Wiwa::Camera::CameraType camtype;
			float fov;
			float ar;
			float nplane;
			float fplane;
			Vector3f pos;
			glm::vec3 rot;
			bool is_active;

			// Read camera active
			scene_data.Read(&is_active, 1);

			// Read camera type
			scene_data.Read(&camtype, sizeof(Wiwa::Camera::CameraType));

			// Read camera FOV
			scene_data.Read(&fov, sizeof(float));

			// Read camera aspect ratio
			scene_data.Read(&ar, sizeof(float));

			// Write camera near plane distance
			scene_data.Read(&nplane, sizeof(float));

			// Write camera far plane distance
			scene_data.Read(&fplane, sizeof(float));

			CameraId cam_id = -1;

			if (camtype == Wiwa::Camera::CameraType::PERSPECTIVE)
			{
				cam_id = cm.CreatePerspectiveCamera(fov, ar, nplane, fplane, false);
			}
			else
			{
				// TODO: LOAD ORTHOGRAPHIC
			}

			if (is_active)
				cm.setActiveCamera(cam_id);

			Camera* camera = cm.getCamera(cam_id);

			// Read camera cull
			scene_data.Read(&camera->cull, 1);

			// Read camera draw BB
			scene_data.Read(&camera->drawBoundingBoxes, 1);

			// Read camera draw frustums
			scene_data.Read(&camera->drawFrustrums, 1);

			// Read camera position
			scene_data.Read(&pos, sizeof(Vector3f));

			// Read camera rotation
			scene_data.Read(glm::value_ptr(rot), sizeof(glm::vec3));

			camera->setPosition(pos);
			camera->setRotation(rot);
		}

		m_LoadingProgress += 2; // 4

		// Load audio
		bool loaded_audio;

		// Save if audio loaded
		scene_data.Read(&loaded_audio, sizeof(bool));

		if (loaded_audio)
		{
			size_t n_len;
			std::string project_path;

			scene_data.Read(&n_len, sizeof(size_t));

			project_path.resize(n_len - 1);

			scene_data.Read(&project_path[0], n_len);

			Audio::LoadProject(project_path.c_str());

			size_t bank_size;
			std::string bank_path;

			// Save bank size
			scene_data.Read(&bank_size, sizeof(size_t));

			if (bank_size > 0)
			{
				for (size_t i = 0; i < bank_size; i++)
				{
					size_t n_size;

					scene_data.Read(&n_size, sizeof(size_t));

					bank_path.resize(n_size - 1);

					scene_data.Read(&bank_path[0], n_size);

					Audio::LoadBank(bank_path.c_str());
				}
			}

			size_t event_size;
			std::string event_path;

			// Save event size
			scene_data.Read(&event_size, sizeof(size_t));

			if (event_size > 0)
			{
				for (size_t i = 0; i < event_size; i++)
				{
					size_t n_size;
					scene_data.Read(&n_size, sizeof(size_t));

					event_path.resize(n_size - 1);

					scene_data.Read(&event_path[0], n_size);

					Audio::LoadEvent(event_path.c_str());
				}
			}
		}

		m_LoadingProgress += 3; // 7		

		// Load entities
		EntityManager& em = scene->GetEntityManager();

		size_t entity_count;
		size_t p_entity_count;

		// Read total entity count
		scene_data.Read(&entity_count, sizeof(size_t));
		// Read parent entity count
		scene_data.Read(&p_entity_count, sizeof(size_t));

		em.ReserveEntities(entity_count);

		for (size_t i = 0; i < p_entity_count; i++)
		{
			LoadEntity(scene_data, i, em, true);
		}

		m_LoadingProgress += 5; // 12

		isLoadingScene = false;

		return true;
	}

	bool SceneManager::_saveSceneImpl(Scene* scene, Memory& scene_data)
	{
		return false;
	}

	void SceneManager::LoadSceneJob(Wiwa::Memory& scene_data)
	{
		while (!m_UnloadedScene);

		// Bind extra context for loading
		Wiwa::Application::Get().GetWindow().BindExtra();

		std::filesystem::path path = m_LoadPath;

		m_SceneLoading->ChangeName(path.filename().stem().string().c_str());

		m_SceneLoading->GetEntityManager().SetInitSystemsOnApply(!(m_LoadFlags & LOAD_NO_INIT));
		m_SceneLoading->GetEntityManager().AddSystemToWhitelist<Wiwa::MeshRenderer>();
		m_SceneLoading->GetEntityManager().AddSystemToWhitelist<Wiwa::ParticleSystem>();

		// Load Physics Manager json Data
		m_SceneLoading->GetPhysicsManager().OnLoad(path.filename().stem().string().c_str());

		m_LoadingProgress += 1;

		// Load actual scene
		_loadSceneImpl(m_SceneLoading, scene_data);

		// Unbind extra context
		Wiwa::Application::Get().GetWindow().UnbindExtra();

		m_LoadedScene = true;
	}

	void SceneManager::LoadingScreenJob()
	{

		// Bind context to this thread
		Wiwa::Application::Get().GetWindow().Bind();

		bool romw = Wiwa::RenderManager::GetRenderOnMainWindow();

		Wiwa::RenderManager::SetRenderOnMainWindow(true);

		m_InstanceRenderer->DisableInstance(m_BackGround_Rocket);
		m_InstanceRenderer->DisableInstance(m_BackGround_Starlord);
		m_InstanceRenderer->DisableInstance(m_ProgressDot1);
		m_InstanceRenderer->DisableInstance(m_ProgressDot2);
		m_InstanceRenderer->DisableInstance(m_ProgressDot3);

		if (GameStateManager::s_CurrentCharacter == 0)
		{
			m_InstanceRenderer->DisableInstance(m_BackGround_Rocket);
			m_InstanceRenderer->EnableInstance(m_BackGround_Starlord);
		}
		else if (GameStateManager::s_CurrentCharacter == 1)
		{
			m_InstanceRenderer->DisableInstance(m_BackGround_Starlord);
			m_InstanceRenderer->EnableInstance(m_BackGround_Rocket);
		}

		GL(ClearColor(0.1f, 0.1f, 0.1f, 1.0f));
		GL(Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		Wiwa::Application::Get().GetRenderer2D().RenderInstanced(*m_InstanceRenderer);
		Wiwa::RenderManager::UpdateSingle(1);
		// Swap buffers
		Wiwa::Application::Get().GetWindow().OnUpdate();

		if (m_LoadFlags & UNLOAD_CURRENT)
		{
			UnloadScene(m_ActiveScene, m_LoadFlags & UNLOAD_RESOURCES);
		}
		SceneId sceneid = -1;

		if (m_LoadFlags & LOAD_SEPARATE)
		{
			sceneid = CreateScene();
		}
		else if (m_LoadFlags & LOAD_APPEND)
		{
			sceneid = m_ActiveScene;
		}
		
		m_SceneLoading = m_Scenes[sceneid];
		m_SceneLoadingId = sceneid;
		
		m_UnloadedScene = true;

		while (!m_LoadedScene) {
			
			// Clear screen
			GL(ClearColor(0.1f, 0.1f, 0.1f, 1.0f));
			GL(Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			// Render loading screen
			/*int progress = m_LoadingProgress;

			float progperc = progress / 12.0f;*/
			
			if (m_LoadingProgress > 3 && m_LoadingProgress <6)
				m_InstanceRenderer->EnableInstance(m_ProgressDot1);
			if(m_LoadingProgress >6 && m_LoadingProgress <= 9)
				m_InstanceRenderer->EnableInstance(m_ProgressDot2);
			if (m_LoadingProgress >9 && m_LoadingProgress <= 12)
				m_InstanceRenderer->EnableInstance(m_ProgressDot3);
			
			Wiwa::Application::Get().GetRenderer2D().RenderInstanced(*m_InstanceRenderer);
			Wiwa::RenderManager::UpdateSingle(1);
			// Swap buffers
			Wiwa::Application::Get().GetWindow().OnUpdate();
		}

		m_InstanceRenderer->DisableInstance(m_ProgressDot1);
		m_InstanceRenderer->DisableInstance(m_ProgressDot2);
		m_InstanceRenderer->DisableInstance(m_ProgressDot3);

		Wiwa::RenderManager::SetRenderOnMainWindow(romw);

		// Unbind context from this thread
		Wiwa::Application::Get().GetWindow().Unbind();
	}

	void SceneManager::SaveScene(SceneId scene_id, const char* scene_path)
	{
		File scene_file = FileSystem::Open(scene_path, FileSystem::OM_OUT | FileSystem::OM_BINARY);

		if (scene_file.IsOpen())
		{
			Scene* sc = m_Scenes[scene_id];

			// Save scene data
			// TODO: Save scene info??
			// Scene name, etc
			// Save GuiControls
			GuiManager& gm = sc->GetGuiManager();
			if (&gm)
			{
				const char* prefab = gm.GetCurrentPrefabWiGui();
				size_t textGuiLen = strlen(prefab) + 1;

				scene_file.Write(&textGuiLen, sizeof(size_t));
				scene_file.Write(prefab, textGuiLen);
			}

			// Iterate through all controls

			// Save cameras
			CameraManager& cm = sc->GetCameraManager();
			std::vector<CameraId>& cameras = cm.getCameras();
			size_t camera_count = cameras.size();

			// Save camera count
			scene_file.Write(&camera_count, sizeof(size_t));

			CameraId active_cam = cm.getActiveCameraId();

			for (size_t i = 0; i < camera_count; i++)
			{
				CameraId cam_id = cameras[i];
				Camera* camera = cm.getCamera(cam_id);

				Wiwa::Camera::CameraType camtype = camera->GetCameraType();
				float fov = camera->getFOV();
				float ar = camera->getAspectRatio();
				float nplane = camera->getNear();
				float fplane = camera->getFar();
				glm::vec3 pos = camera->getPosition();
				glm::vec3 rot = camera->getRotation();

				bool is_active = cam_id == active_cam;

				// Write camera active
				scene_file.Write(&is_active, 1);

				// Write camera type
				scene_file.Write(&camtype, sizeof(Wiwa::Camera::CameraType));

				// Write camera FOV
				scene_file.Write(&fov, sizeof(float));

				// Write camera aspect ratio
				scene_file.Write(&ar, sizeof(float));

				// Write camera near plane distance
				scene_file.Write(&nplane, sizeof(float));

				// Write camera far plane distance
				scene_file.Write(&fplane, sizeof(float));

				// Write camera cull
				scene_file.Write(&camera->cull, 1);

				// Write camera draw BB
				scene_file.Write(&camera->drawBoundingBoxes, 1);

				// Write camera draw frustums
				scene_file.Write(&camera->drawFrustrums, 1);

				// Write camera position
				scene_file.Write(glm::value_ptr(pos), sizeof(glm::vec3));

				// Write camera rotation
				scene_file.Write(glm::value_ptr(rot), sizeof(glm::vec3));
			}

			// Save audio
			bool loaded_audio = Audio::LoadedProject();

			// Save if audio loaded
			scene_file.Write(&loaded_audio, sizeof(bool));

			if (loaded_audio)
			{
				// Save project path
				std::string project_path = Audio::GetProjectPath();

				size_t n_len = project_path.size() + 1;

				scene_file.Write(&n_len, sizeof(size_t));
				scene_file.Write(project_path.data(), n_len);

				const std::vector<Audio::BankData>& bank_list = Audio::GetLoadedBankList();

				size_t bank_size = bank_list.size();

				// Save bank size
				scene_file.Write(&bank_size, sizeof(size_t));

				if (bank_size > 0)
				{
					for (size_t i = 0; i < bank_size; i++)
					{
						size_t n_size = bank_list[i].path.size() + 1;

						scene_file.Write(&n_size, sizeof(size_t));
						scene_file.Write(bank_list[i].path.data(), n_size);
					}
				}

				const std::vector<Audio::EventData>& event_list = Audio::GetLoadedEventList();

				size_t event_size = event_list.size();

				// Save event size
				scene_file.Write(&event_size, sizeof(size_t));

				if (event_size > 0)
				{
					for (size_t i = 0; i < event_size; i++)
					{
						size_t n_size = event_list[i].name.size() + 1;

						scene_file.Write(&n_size, sizeof(size_t));
						scene_file.Write(event_list[i].name.data(), n_size);
					}
				}
			}

			// Save entities
			EntityManager& em = sc->GetEntityManager();
			std::vector<EntityId>* pentities = em.GetParentEntitiesAlive();
			size_t p_entity_count = pentities->size();

			size_t entity_count = em.GetEntityCount();

			// Save total entity count
			scene_file.Write(&entity_count, sizeof(size_t));
			// Save parent entity count
			scene_file.Write(&p_entity_count, sizeof(size_t));

			// Save each entity
			for (size_t i = 0; i < p_entity_count; i++)
			{
				EntityId eid = pentities->at(i);
				SaveEntity(scene_file, eid, em);
			}

			WI_CORE_INFO("Saved scene in file \"{0}\" successfully!", scene_path);
		}
		else
		{
			WI_CORE_WARN("Couldn't open file \"{0}\"", scene_path);
		}

		scene_file.Close();
	}

	SceneId SceneManager::LoadScene(const char* scene_path, int flags)
	{
		if (!FileSystem::Exists(scene_path))
			return -1;

		sbyte* memblock = nullptr;
		size_t size = FileSystem::ReadAll(scene_path, &memblock);

		if (memblock)
		{
			m_LoadFlags = flags;
			m_LoadPath = scene_path;

			Memory scene_data(memblock, size);

			

			Wiwa::Resources::setGenBuffersOnLoad(false);

			// Unbind from main thread
			Wiwa::Application::Get().GetWindow().Unbind();


			// Begin loading scene in jobs
			m_LoadedScene = false;
			m_UnloadedScene = false;
			m_LoadingProgress = 0;
			m_LoadThread = std::thread(&SceneManager::LoadSceneJob, scene_data);
			m_LoadScreenThread = std::thread(&SceneManager::LoadingScreenJob);

			// Wait till loaded
			m_LoadThread.join();
			m_LoadScreenThread.join();

			// Re-bind window to main thread
			Wiwa::Application::Get().GetWindow().Bind();

			Wiwa::Resources::setGenBuffersOnLoad(true);

			m_SceneLoading->Start();

			if (flags & LOAD_SEPARATE)
			{
				SetScene(m_SceneLoadingId, !(flags & LOAD_NO_INIT));
			}

			WI_CORE_INFO("Loaded scene in file \"{0}\" successfully!", scene_path);
		}
		else
		{
			WI_CORE_WARN("Couldn't open scene file \"{0}\".", scene_path);
		}

		return m_SceneLoadingId;
	}

	void SceneManager::LoadSceneByIndex(size_t scene_index, int flags)
	{
		ProjectManager::SceneData& sd = ProjectManager::getSceneDataAt(scene_index);

		m_LoadScene = true;
		m_LoadPath = sd.scene_path;
		m_LoadFlags = flags;

		//LoadScene(sd.scene_path.c_str(), flags);
	}

	void SceneManager::LoadSceneByName(const char* scene_name, int flags)
	{
		ProjectManager::SceneData* sd = ProjectManager::getSceneByName(scene_name);

		if (!sd) return;

		m_LoadScene = true;
		m_LoadPath = sd->scene_path;
		m_LoadFlags = flags;

		//LoadScene(sd->scene_path.c_str(), flags);
	}

	void SceneManager::UnloadScene(SceneId scene_id, bool unload_resources)
	{
		if (!m_Scenes[scene_id])
			return;

		m_Scenes[scene_id]->Unload(unload_resources);
		delete m_Scenes[scene_id];
		m_Scenes[scene_id] = nullptr;

		m_RemovedSceneIds.push_back(scene_id);
	}

	void SceneManager::FixSceneLoading()
	{
		if (GameStateManager::s_CurrentCharacter == 0)
		{
			m_InstanceRenderer->EnableInstance(m_BackGround_Starlord);
		}
		else if (GameStateManager::s_CurrentCharacter == 1)
		{
			m_InstanceRenderer->EnableInstance(m_BackGround_Rocket);
		}
	}
	void SceneManager::ReLoadTransition()
	{
		ResourceId background_image_id_starlord = WI_INVALID_INDEX;
		ResourceId background_image_id_rocket = WI_INVALID_INDEX;

		background_image_id_starlord = Resources::LoadNative<Wiwa::Image>("resources/images/transitions/UI_LoadingScreen_01a.png");
		background_image_id_rocket = Resources::LoadNative<Wiwa::Image>("resources/images/transitions/UI_LoadingScreen_01b.png");


		Image* background_img_starlord = Resources::GetResourceById<Image>(background_image_id_starlord);
		Size2i size_background_starlord = { 2048,2048 };
		Rect2i clip_background_starlord = {
			0, 0,
			size_background_starlord.w, size_background_starlord.h
		};

		TextureClip tclip_background_starlord = Renderer2D::CalculateTextureClip(clip_background_starlord, size_background_starlord);

		Image* background_img_rocket = Resources::GetResourceById<Image>(background_image_id_rocket);
		Size2i size_background_rocket = { 2048,2048 };
		Rect2i clip_background_rocket = {
			0, 0,
			size_background_rocket.w, size_background_rocket.h
		};

		TextureClip tclip_background_rocket = Renderer2D::CalculateTextureClip(clip_background_rocket, size_background_rocket);

		ResourceId progress_dot_id = Resources::LoadNative<Wiwa::Image>("resources/images/transitions/UI_LoadingDot_01.png");

		Image* progress_dot_img = Resources::GetResourceById<Image>(progress_dot_id);
		Size2i size_dot = { 64,64 };
		Rect2i clip_dot = {
			0, 0,
			size_dot.w, size_dot.h
		};

		TextureClip tclip_dot1 = Renderer2D::CalculateTextureClip(clip_dot, size_dot);

		m_ProgressDot1 = m_InstanceRenderer->AddInstance(progress_dot_img->GetTextureId(), { 1600,890 }, { 64,64 }, Wiwa::Color::WHITE, tclip_dot1, Renderer2D::Pivot::UPLEFT);
		m_ProgressDot2 = m_InstanceRenderer->AddInstance(progress_dot_img->GetTextureId(), { 1700,890 }, { 64,64 }, Wiwa::Color::WHITE, tclip_dot1, Renderer2D::Pivot::UPLEFT);
		m_ProgressDot3 = m_InstanceRenderer->AddInstance(progress_dot_img->GetTextureId(), { 1800,890 }, { 64,64 }, Wiwa::Color::WHITE, tclip_dot1, Renderer2D::Pivot::UPLEFT);
		m_BackGround_Starlord = m_InstanceRenderer->AddInstance(background_img_starlord->GetTextureId(), { 0,0 }, { 2048,2048 }, Wiwa::Color::WHITE, tclip_background_starlord, Renderer2D::Pivot::UPLEFT);
		m_BackGround_Rocket = m_InstanceRenderer->AddInstance(background_img_rocket->GetTextureId(), { 0,0 }, { 2048,2048 }, Wiwa::Color::WHITE, tclip_background_rocket, Renderer2D::Pivot::UPLEFT);

		m_InstanceRenderer->DisableInstance(m_BackGround_Starlord);
		m_InstanceRenderer->DisableInstance(m_BackGround_Rocket);
		m_InstanceRenderer->DisableInstance(m_ProgressDot1);
		m_InstanceRenderer->DisableInstance(m_ProgressDot2);
		m_InstanceRenderer->DisableInstance(m_ProgressDot3);
	}

	void SceneManager::SetScene(SceneId sceneId, bool init)
	{
		m_ActiveScene = sceneId;

		Wiwa::RenderManager::SetLayerCamera(0, getScene(sceneId)->GetCameraManager().getActiveCamera());
		Wiwa::Application::Get().GetRenderer2D().BindCamera();

		if (init) {
			m_Scenes[sceneId]->Awake();
			m_Scenes[sceneId]->Init();
		}

		SceneChangeEvent event(sceneId);
		Action<Wiwa::Event&> act = { &Application::OnEvent, &Application::Get() };
		act(event);
	}

	void SceneManager::ChangeSceneByName(const char* name, int flags)
	{
		size_t index = ProjectManager::getSceneIndexByName(name);

		m_Scenes[m_ActiveScene]->ChangeScene(index, flags);
	}

	void SceneManager::ChangeSceneByIndex(SceneId sceneId, int flags)
	{
		WI_INFO("ChangeSceneByIndex() with sceneId: {}", sceneId);
		m_Scenes[m_ActiveScene]->ChangeScene(sceneId, flags);
	}

	void SceneManager::PauseCurrentScene()
	{
		m_Scenes[m_ActiveScene]->SwapPauseActive();
	}

}