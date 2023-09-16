#include <wipch.h>
#include "Scene.h"
#include "SceneManager.h"

#include <Wiwa/core/Application.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/core/Renderer3D.h>
#include <Wiwa/utilities/render/LightManager.h>

#include <Wiwa/Ui/UiManager.h>
#include <Wiwa/Dialog/DialogManager.h>
#include <Wiwa/Dialog/DialogEventManager.h>
#include <Wiwa/audio/Audio.h>
#include <Wiwa/ecs/systems/game/gui/PlayerGUISystem.h>
#include <Wiwa/ecs/systems/game/gui/UltronGUISystem.h>
#include <Wiwa/AI/AI_Crowd.h>

#include <glew.h>
#include <Wiwa/utilities/Log.h>

namespace Wiwa
{
	Scene::Scene()
	{
		mMaxTimeEntering = 450;
		mMaxTimeLeaving = 450;

		m_EntityManager.SetScene(this);
		m_CameraManager = new CameraManager();
		m_LightManager = new LightManager();
		m_LightManager->SetScene(this);
		m_PhysicsManager = new PhysicsManager();

		uint32_t imgid = Resources::LoadNative<Wiwa::Image>("resources/images/transitions/transision.png");
		Wiwa::Image* img = Resources::GetResourceById<Wiwa::Image>(imgid);
		
		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

		m_TransitionInstance = r2d.CreateInstancedQuadTex(this, img->GetTextureId(), img->GetSize(), { 0,0 }, { 2000,1080 }, Wiwa::Renderer2D::Pivot::UPLEFT);
		r2d.UpdateInstancedQuadTexColor(this, m_TransitionInstance, { 0.1f, 0.1f, 0.1f, 1.0f });
		r2d.UpdateInstancedQuadTexPriority(this, m_TransitionInstance, 2);
		r2d.DisableInstance(this, m_TransitionInstance);

		m_GuiManager = new GuiManager();
		m_GuiManager->Init(this);
		m_DialogManager = new DialogManager();
		m_DialogManager->Init(this);
		m_DialogEventManager = new DialogEventManager();
		m_DialogEventManager->Init(this);

		m_PhysicsManager->InitWorld(this);

		Wiwa::Application::Get().GetRenderer2D().BindCamera();
	}

	Scene::~Scene()
	{
		delete m_CameraManager;
		delete m_LightManager;
		delete m_GuiManager;
		delete m_DialogManager;
		delete m_DialogEventManager;

		// Clear entity manager
		//m_EntityManager.Clear();
		if (m_PhysicsManager)
		{
			// Clear physics world
			m_PhysicsManager->CleanWorld();
			delete m_PhysicsManager;
		}
		RecastManager::DeAllocate();

		size_t instance_size = m_InstanceRenderers.size();

		for (size_t i = 0; i < instance_size; i++) {
			delete m_InstanceRenderers[i];
		}
	}

	void Scene::Start()
	{
		m_CameraManager->GenerateBuffers();

		size_t instance_count = m_InstanceRenderers.size();

		for (size_t i = 0; i < instance_count; i++) {
			m_InstanceRenderers[i]->GenerateBuffers();
		}

		Wiwa::Resources::GenerateAllBuffers();
	}

	void Scene::Awake()
	{
		RecastManager::Load(this);

		Crowd& crowd = Crowd::getInstance();
		crowd.Init();

		m_EntityManager.SystemsAwake();
	}

	void Scene::Init()
	{
		m_EntityManager.SystemsInit();
	}

	void Scene::Update()
	{
		PlayerGUISystem* pgs = nullptr;
		UltronGUISystem* ugs = nullptr;
		EntityId player = WI_INVALID_INDEX;
		EntityId player2 = WI_INVALID_INDEX;
		switch (m_CurrentState)
		{
		case Scene::SCENE_ENTERING:
			m_EntityManager.UpdateWhitelist();
			pgs = nullptr;
			ugs = nullptr;
			UpdateEnter();
			RenderEnter();
			SceneManager::FixSceneLoading();

			m_TransitionTimer += (size_t)Wiwa::Time::GetRealDeltaTime();
			
			if (m_TransitionTimer >= mMaxTimeEntering) {
				m_TransitionTimer = 0;

				m_CurrentState = SCENE_LOOP;

				Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

				r2d.DisableInstance(this, m_TransitionInstance);
			}
			break;
		case Scene::SCENE_LOOP:
			if (pausedGame)
			{
				Time::SetTimeScale(0.0f);
			}
			else if (!pausedGame)
			{
				Time::SetTimeScale(1.0f);
				m_EntityManager.SystemsUpdate();
			}

			if (GameStateManager::GetPlayerId() != WI_INVALID_INDEX)
			{
				pgs = m_EntityManager.GetSystem<PlayerGUISystem>(Wiwa::GameStateManager::GetPlayerId());
				if (pgs != nullptr)
					pgs->Update();

				EntityId boss = m_EntityManager.GetEntityByName("Ultron_01");
				if (boss != WI_INVALID_INDEX)
				{
					ugs = m_EntityManager.GetSystem<UltronGUISystem>(boss);
					if (ugs != nullptr)
						ugs->Update();
				}
			}
			if (m_GuiManager)
			{
				m_GuiManager->Update();
			}
			if (m_DialogManager)
			{
				if (GameStateManager::GetPlayerId() != WI_INVALID_INDEX) //old version: player != WI_INVALID_INDEX || player2 != WI_INVALID_INDEX
				{
					m_DialogManager->Update();
				}
			}
			if (m_DialogEventManager)
			{
				if (GameStateManager::GetPlayerId() != WI_INVALID_INDEX)
				{
					m_DialogEventManager->Update();
				}
			}
			ProcessInput();
			UpdateLoop();
			RenderLoop();
			break;
		case Scene::SCENE_LEAVING:
			m_EntityManager.UpdateWhitelist();

			UpdateLeave();
			RenderLeave();
			SceneManager::FixSceneLoading();

			m_TransitionTimer += (size_t)Wiwa::Time::GetRealDeltaTime();
			
			if (m_TransitionTimer >= mMaxTimeLeaving) {
				m_TransitionTimer = 0;
				Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
				r2d.DisableInstance(this, m_TransitionInstance);

				SceneManager::LoadSceneByIndex(m_SceneToChange, m_SceneChangeFlags);
			}
			break;
		default:
			break;
		}
	}

	void Scene::ModuleUpdate()
	{
		m_CameraManager->Update();
		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
		
		Wiwa::Application::Get().GetRenderer3D().PreUpdate();
		r2d.UpdateInstanced(this);
		if(m_GuiManager)
			m_GuiManager->Draw();
		
		m_EntityManager.Update();
		if(m_PhysicsManager)
			m_PhysicsManager->UpdateEngineToPhysics();

		if (SceneManager::IsPlaying())
		{
			m_PhysicsManager->StepSimulation();
			m_PhysicsManager->UpdatePhysicsToEngine();

			Crowd& crowd = Crowd::getInstance();
			crowd.Update(Wiwa::Time::GetDeltaTimeSeconds());
		}
		m_PhysicsManager->DebugDrawWorld();

		if (!SceneManager::IsPlaying())
		{
			m_EntityManager.UpdateWhitelist();
		}

	}

	void Scene::Load()
	{
		m_TransitionTimer = 0;

		m_CurrentState = SCENE_ENTERING;
	}

	void Scene::Unload(bool unload_resources)
	{
		GameStateManager::s_PoolManager->UnloadAllPools();
		m_EntityManager.Clear();
		if (unload_resources)
		{
			// TODO: Fix unloading for the editor
			if(Wiwa::Application::Get().UnloadSceneResources)
				Wiwa::Resources::UnloadAllResources();
			return;
		}
	}

	void Scene::ChangeScene(size_t scene, int flags)
	{
		WI_INFO("ChangeScene() with scene: {}", scene);
		m_SceneToChange = scene;
		m_SceneChangeFlags = flags;
		m_CurrentState = SCENE_LEAVING;
		m_TransitionTimer = 0;

		SceneChangeEvent event(scene);
		Action<Event&> action = { &Application::OnEvent, &Application::Get() };
		action(event);
	}

	uint32_t Scene::CreateInstanceRenderer()
	{
		uint32_t id = m_InstanceRenderers.size();

		m_InstanceRenderers.push_back(new InstanceRenderer(405000));

		m_InstanceRenderers[id]->Init("resources/shaders/instanced_tex_color");

		return id;
	}

	void Scene::UpdateEnter()
	{
		
		/*Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

		r2d.EnableInstance(this, m_TransitionInstance);*/
	}

	void Scene::UpdateLeave()
	{
		/*Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

		r2d.EnableInstance(this, m_TransitionInstance);*/
	}

	void Scene::RenderEnter()
	{
		/*Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

		float perc = m_TransitionTimer / (float)mMaxTimeEntering;
		int w = (int)(256.f * (1.0f - perc));

		Wiwa::Size2i size = {
			(int)(2000 * (1.0f - perc)),
			1080
		};

		r2d.UpdateInstancedQuadTexClip(this, m_TransitionInstance, { 256,256 }, { 0,0,w,256 });
		r2d.UpdateInstancedQuadTexSize(this, m_TransitionInstance, {0,0}, size, Wiwa::Renderer2D::Pivot::UPLEFT);*/
	}

	void Scene::RenderLeave()
	{
		/*Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

		float perc = m_TransitionTimer / (float)mMaxTimeLeaving;
		int w = (int)(256.f * perc);

		Wiwa::Size2i size = {
			(int)(2000 * perc),
			1080
		};

		r2d.UpdateInstancedQuadTexClip(this, m_TransitionInstance, { 256,256 }, { 0,0,w,256 });
		r2d.UpdateInstancedQuadTexSize(this, m_TransitionInstance, { 0,0 }, size, Wiwa::Renderer2D::Pivot::UPLEFT);*/
	}
}