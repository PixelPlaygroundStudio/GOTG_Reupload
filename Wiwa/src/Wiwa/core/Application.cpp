
#include "wipch.h"
#include "Application.h"

#include <Wiwa/ecs/EntityManager.h>

#include <Wiwa/utilities/Log.h>

#include <glew.h>
#include <GLFW/glfw3.h>

#include "Input.h"

#include <shellapi.h>
#include <Windows.h>

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

#include "Renderer3D.h"
#include "Renderer2D.h"

#include <Wiwa/ecs/Components.h>
#include <Wiwa/ecs/Systems.h>

#include <Wiwa/scene/SceneManager.h>

#include <Wiwa/utilities/json/JSONDocument.h>

#include <Wiwa/utilities/AllocationMetrics.h>

#include <Wiwa/core/Resources.h>
#include <Wiwa/audio/Audio.h>

#include <Wiwa/render/RenderManager.h>

#include <Wiwa/utilities/AnimatorManager.h>

#include <Wiwa/core/ProjectManager.h>
#include <stdlib.h>
#include <time.h>

#include <Wiwa/utilities/functions/Function.h>
#include <random>

USE_REFLECTION;

namespace Wiwa
{
	Application *Application::s_Instance = nullptr;
	std::random_device Application::s_Rd;
	std::mt19937 Application::s_Gen(s_Rd());

	Application::Application(int argc, char **argv)
	{		
		WI_CORE_ASSERT(!s_Instance, "Application already exists!");

		REFLECTION_REGISTER();

		m_ArgC = argc;

		for (int i = 0; i < argc; i++)
		{
			m_Argv.push_back(argv[i]);
		}

		m_GameAssemblyHandle = NULL;

		s_Instance = this;

		m_TargetResolution = {1920, 1080};

		
		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps("Guardians of the Galaxy: Battle for the Universe")));
		m_Window->SetEventCallback({&Application::OnEvent, this});

		int min, major, rev;
		glfwGetVersion(&min, &major, &rev);
		sprintf_s(m_SysInfo.glfwVer, 32, "%i.%i.%i", min, major, rev);

		SetHwInfo();
		WI_CORE_WARN("=======Initializing systems=======");
		m_Renderer2D = new Renderer2D();
		m_Renderer2D->Init();
		
		m_Renderer3D = new Renderer3D();
		
		m_Renderer3D->Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_RenderColor = {0.1f, 0.1f, 0.1f, 1.0f};

		RenderManager::Init(m_TargetResolution.w, m_TargetResolution.h);

		SceneManager::ModuleInit();

		Input::Init();

		bool res = Audio::Init();

		if (!res)
		{
			WI_CORE_ERROR("Audio engine error: [{}]", Audio::GetLastError());
		}
		
		
		// Copy assembly before loading
		Wiwa::FileSystem::Copy("game_assembly/build/WiwaGameAssembly.dll", "resources/WiwaGameAssembly.dll");

		// Load assembly
		LoadGameAssembly();

		WI_CORE_WARN("=======Systems initialized=======");
	}

	void Application::SetHwInfo()
	{
		SYSTEM_INFO info;
		::GetSystemInfo(&info);

		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);

		GLint total_mem_kb = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
		GLint cur_avail_mem_kb = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);
		GLint cur_reserv_mem_kb = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &cur_reserv_mem_kb);

		m_SysInfo.numCores = info.dwNumberOfProcessors;
		m_SysInfo.ram = (float)(memInfo.ullTotalPhys >> 20);
		m_SysInfo.gpu = (const unsigned char *)glGetString(GL_VENDOR);
		m_SysInfo.gpuBrand = glGetString(GL_RENDERER);
		m_SysInfo.gpuVRAM = (float)(total_mem_kb >> 10);
		m_SysInfo.gpuVRAMAV = (float)(cur_avail_mem_kb >> 10);
		m_SysInfo.gpuVRAMUsage = (float)((total_mem_kb - cur_avail_mem_kb) >> 10);
		m_SysInfo.gpuVRAMReserve = (float)(cur_reserv_mem_kb >> 10);
	}

	Application::~Application()
	{
		SceneManager::CleanUp();
		RenderManager::Destroy();
	
		Audio::Terminate();
		GameStateManager::CleanUp();
		
		AnimatorManager::CleanUp();

		delete m_Renderer2D;
		delete m_Renderer3D;
	}

	void Application::Run()
	{
		double lastTime = glfwGetTime();
		while (m_Running)
		{
			OPTICK_FRAME("Application Loop");

			

			if (FinishedImport)
			{
				FinishedImport = false;
				Wiwa::GameStateManager::DeserializeData();
			}

			// Limit the frame time if needed
			if (Time::IsFrameCap())
			{
				if ((glfwGetTime() - lastTime) < Time::GetTargetDTSeconds())
					continue;
			}
			lastTime = glfwGetTime();

			Time::Update();
			// Clear main window
			
			GL(ClearColor(m_RenderColor.r, m_RenderColor.g, m_RenderColor.b, m_RenderColor.a));
			GL(Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			
			// Update scene manager
			SceneManager::ModuleUpdate();

			// Update audio
			
			Audio::Update();

			// Execute main thread queue

			ExecuteMainThreadQueue();

			// Update inputs
			OPTICK_EVENT("Input update");
			Input::Update();


			// Update renderers
			m_Renderer2D->Update();
		
			m_Renderer3D->Update();
			//render post processing
			m_Renderer3D->PostUpdate();

			
			RenderManager::Update();

			// Update layers
			for (Layer *layer : m_LayerStack)
				layer->OnUpdate();

			// Render layers

			m_ImGuiLayer->Begin();
			{
				// TODO: Optick On ImGuiRender call
				for (Layer *layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
			
			GameStateManager::Update();
			
			// Update main window
			m_Window->OnUpdate();
			Time::PostUpdate();

			
		}
	}

	size_t Application::getCoreTypeCount() const
	{
		return TYPE_COUNT;
	}

	const Type *Application::getCoreType(size_t index) const
	{
		return GET_TYPES()->at(index);
	}

	const Type *Application::getCoreTypeH(size_t hash) const
	{
		const Wiwa::Array<const Type *, TYPE_COUNT> *types = GET_TYPES();

		const Type *type = NULL;

		for (size_t i = 0; i < TYPE_COUNT; i++)
		{
			if (types->at(i)->hash == hash)
			{
				type = types->at(i);
				break;
			}
		}

		return type;
	}

	const Type *Application::GetComponentTypeH(size_t hash) const
	{
		size_t size = m_ComponentTypes.size();

		const Type *type = NULL;

		for (size_t i = 0; i < size; i++)
		{
			if (m_ComponentTypes[i]->hash == hash)
			{
				type = m_ComponentTypes[i];
				break;
			}
		}

		return type;
	}

	const Type *Application::GetComponentType(size_t index) const
	{
		return m_ComponentTypes[index];
	}

	void Application::RegisterComponentType(const Type *component)
	{
		const Type *type = GetComponentTypeH(component->hash);

		if (!type)
			m_ComponentTypes.push_back(component);
	}

	void Application::DeleteComponentType(const Type *component)
	{
		const Type *type = GetComponentTypeH(component->hash);

		if (type) {
			for (size_t i = 0; i < m_ComponentTypes.size(); i++)
			{
				if (m_ComponentTypes[i] == type)
				{
					m_ComponentTypes.erase(m_ComponentTypes.begin() + i);
					break;
				}
			}
		}
	}

	const Type *Application::GetSystemTypeH(size_t hash) const
	{
		size_t size = m_SystemTypes.size();

		const Type *type = NULL;

		for (size_t i = 0; i < size; i++)
		{
			if (m_SystemTypes[i]->hash == hash)
			{
				type = m_SystemTypes[i];
				break;
			}
		}

		return type;
	}

	const Type *Application::GetSystemType(size_t index) const
	{
		return m_SystemTypes[index];
	}

	bool Application::HasSystemH(size_t hash) const
	{
		const Type *stype = GetSystemTypeH(hash);

		return stype != NULL;
	}

	const Func* Application::getCallback(size_t hash) const
	{
		const Func* cb = NULL;

		size_t s = m_Callbacks.size();

		for (size_t i = 0; i < s; i++) {
			if (m_Callbacks[i]->hash == hash) {
				cb = m_Callbacks[i];
				break;
			}
		}

		return cb;
	}

	void Application::ClearCallbacks()
	{
		size_t s = m_Callbacks.size();

		for (size_t i = 0; i < s; i++) {
			delete m_Callbacks[i];
		}
		
		m_Callbacks.clear();
	}

	void Application::RegisterSystemType(const Type *system)
	{
		const Type *type = GetSystemTypeH(system->hash);

		if (!type)
			m_SystemTypes.push_back(system);
	}

	void Application::DeleteSystemType(const Type *system)
	{
		const Type *type = GetSystemTypeH(system->hash);

		if (type) {
			for (size_t i = 0; i < m_SystemTypes.size(); i++)
			{
				if (m_SystemTypes[i] == type)
				{
					m_SystemTypes.erase(m_SystemTypes.begin() + i);
					break;
				}
			}
		}
	}

	void Application::OpenDir(const char *path)
	{
		ShellExecuteA(0, "open", path, NULL, NULL, SW_SHOWNORMAL);
	}

	void Application::Quit()
	{
		OnSaveEvent event;
		OnEvent(event);
		m_Running = false;
	}

	void Application::SubmitToMainThread(const std::function<void()> func)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		m_MainThreadQueue.emplace_back(func);
	}

	void Application::UnloadGameAssembly()
	{
		if (m_GameAssemblyHandle != NULL) {
			size_t s = m_GameAssemblyHandle->types.size();

			for (size_t i = 0; i < s; i++) {
				const Type* t = m_GameAssemblyHandle->types[i];

				if (t->is_function) {
					// Unregister callback
					m_Callbacks.clear();
				}
				else {
					if (t->custom_id == 0) {
						DeleteComponentType(t);
					}
					else {
						DeleteSystemType(t);
					}
				}
			}

			m_GameAssemblyHandle = NULL;

			if (m_GameAssembly) {
				FreeLibrary(m_GameAssembly);
				m_GameAssembly = NULL;
			}
		}
	}

	void Application::LoadGameAssembly()
	{
		UnloadGameAssembly();

		m_GameAssembly = LoadLibraryA("resources/WiwaGameAssembly.dll");

		if (m_GameAssembly) {
			void* _assemblyFunc = GetProcAddress(m_GameAssembly, "getAssemblyHandle");

			if (_assemblyFunc) {
				Fn::Function<GameAssemblyHandle*> getHandleFunc = _assemblyFunc;

				m_GameAssemblyHandle = getHandleFunc();

				size_t s = m_GameAssemblyHandle->types.size();

				WI_CORE_INFO("==== Loaded assembly types ====");
				for (size_t i = 0; i < s; i++) {
					const Type* t = m_GameAssemblyHandle->types[i];

					if (t->is_function) {
						// Register as callback
						const Func* func = (const Func*)t;
						RegisterCallback(func);
					}
					else {
						if (t->custom_id == 0) {
							RegisterComponentType(t);
						}
						else {
							RegisterSystemType(t);
						}
					}

					WI_CORE_INFO("Name: {0} Type: {1}", t->name.c_str(), t->is_function ? "Function" : t->custom_id == 0 ? "Component" : "System");
				}
				WI_CORE_INFO("==== End assembly types ====");
			}
			else {
				WI_CORE_ERROR("Couldn't find wrapper function in game assembly.");

				FreeLibrary(m_GameAssembly);
				m_GameAssembly = NULL;
			}
		}
		else {
			WI_CORE_ERROR("Couldn't load game assembly.");
		}
	}

	void Application::ExecuteMainThreadQueue()
	{
		OPTICK_EVENT("Execute Main thread");
		for (auto &func : m_MainThreadQueue)
			func();

		m_MainThreadQueue.clear();
	}

	bool Application::OnWindowClose(WindowCloseEvent &e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnLoad(OnLoadEvent &e)
	{
		JSONDocument config("config/application.json");

		if (!config.IsObject())
			return false;

		if (config.HasMember("vsync"))
			m_Window->SetVSync(config["vsync"].as_bool());

		if (config.HasMember("fullscreen"))
		{
			bool fullscreen = config["fullscreen"].as_bool();
			if (fullscreen)
				m_Window->SetFullScreen(fullscreen);
		}
		if (config.HasMember("resizable"))
			m_Window->SetResizable(config["resizable"].as_bool());

		if (config.HasMember("master_volume"))
			Audio::ChangeMasterVolume(config["master_volume"].as_int());
		if (config.HasMember("music_volume"))
			Audio::ChangeMusicVolume(config["music_volume"].as_int());
		if (config.HasMember("sfx_volume"))
			Audio::ChangeSFXVolume(config["sfx_volume"].as_int());
		if (config.HasMember("dialog_volume"))
			Audio::ChangeDialogVolume(config["dialog_volume"].as_int());

		if (config.HasMember("project_file"))
		{
			ProjectManager::OpenProject(config["project_file"].as_string());
		}

		return false;
	}

	bool Application::OnSave(OnSaveEvent &e)
	{
		JSONDocument config;

		config.AddMember("vsync", m_Window->IsVSync());
		config.AddMember("fullscreen", m_Window->GetFullScreen());
		config.AddMember("resizable", m_Window->GetResizable());
		config.AddMember("master_volume", m_MasterVolume);
		config.AddMember("music_volume",m_MusicVolume);
		config.AddMember("sfx_volume", m_SFXVolume);

		config.AddMember("project_file", ProjectManager::GetProjectPath().c_str());

		config.save_file("config/application.json");

		if (!Wiwa::ProjectManager::SaveProject())
		{
			WI_CORE_ERROR("Couldn't save project.");
		}

		return false;
	}
	void Application::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>({&Application::OnWindowClose, this});
		dispatcher.Dispatch<OnSaveEvent>({ &Application::OnSave, this});
		dispatcher.Dispatch<OnLoadEvent>({ &Application::OnLoad, this});

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}
	void Application::PushLayer(Layer *layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer *overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
}