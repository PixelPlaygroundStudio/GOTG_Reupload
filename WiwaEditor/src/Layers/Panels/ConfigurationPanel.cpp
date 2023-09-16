#include "ConfigurationPanel.h"

#include <imgui.h>

#include "windows.h"
#include "psapi.h"

#include <Wiwa/utilities/AllocationMetrics.h>
#include "../EditorLayer.h"
#include <Wiwa/core/Input.h>
#include <Wiwa/Platform/Windows/WindowsInput.h>

ConfigurationPanel::ConfigurationPanel(EditorLayer *instance)
	: Panel("Configuration", ICON_FK_COG, instance), info()
{
}

ConfigurationPanel::~ConfigurationPanel()
{
}

void ConfigurationPanel::Draw()
{
	m_Resizable = Wiwa::Application::Get().GetWindow().GetResizable();
	m_Fullscreen = Wiwa::Application::Get().GetWindow().GetFullScreen();
	m_VSync = Wiwa::Application::Get().GetWindow().IsVSync();
	m_MSLog.push_back(Wiwa::Time::GetRealDeltaTime());
	m_FPSLog.push_back(1.0f / Wiwa::Time::GetRealDeltaTimeSeconds());
	

	ImGui::Begin(iconName.c_str(), &active);

	if (ImGui::CollapsingHeader("External tools"))
	{
		{
			const char *types[] = {"vs2022", "vs2019"};
			static const char *currentItem =
				instance->s_SolVersion == "vs2022" ? types[0] : types[1];
			if (ImGui::BeginCombo("Solution version", currentItem))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = (currentItem == types[i]);
					if (ImGui::Selectable(types[i], isSelected))
					{
						currentItem = types[i];
						instance->s_SolVersion = currentItem;
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}
		ImGui::Separator();
		{
			const char *types[] = {"Release", "Debug"};
			static const char *currentItem =
				instance->s_BuildConf == "Release" ? types[0] : types[1];
			if (ImGui::BeginCombo("Build config", currentItem))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = (currentItem == types[i]);
					if (ImGui::Selectable(types[i], isSelected))
					{
						currentItem = types[i];
						instance->s_BuildConf = currentItem;
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (ImGui::Button("Regenerate solution"))
			{
				instance->RegenSol();
			}
		}
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		if (ImGui::Checkbox("Fullscreen", &m_Fullscreen))
			Wiwa::Application::Get().GetWindow().SetFullScreen(m_Fullscreen);
		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &m_Resizable))
			Wiwa::Application::Get().GetWindow().SetResizable(m_Resizable);
		if (ImGui::Checkbox("VSync", &m_VSync))
			Wiwa::Application::Get().GetWindow().SetVSync(m_VSync);

		int FPSCap = Wiwa::Time::GetTargetFPS();
		static	bool capFPS = false;
		ImGui::Checkbox("Cap FPS", &capFPS);
		if (!capFPS)
			ImGui::BeginDisabled();
		ImGui::SliderInt("FPS", &FPSCap, 1, 250);
		if (FPSCap > 1)
			Wiwa::Time::SetTargetFPS(FPSCap);
		if (!capFPS)
			Wiwa::Time::SetTargetFPS(-1);

		if(!capFPS)
			ImGui::EndDisabled();
	}
	char title[25];
	if (ImGui::CollapsingHeader("Info"))
	{
		sprintf_s(title, 25, "Framerate %.1f", m_FPSLog[m_FPSLog.size() - 1]);
		ImGui::PlotLines("##framerate", &m_FPSLog[0], (int)m_FPSLog.size(), 0, title, 0.0f, 1000.0f, ImVec2(200, 100));
		sprintf_s(title, 25, "Frametime %.1f", m_MSLog[m_MSLog.size() - 1]);
		ImGui::PlotLines("##frametime", &m_MSLog[0], (int)m_MSLog.size(), 0, title, 0.0f, m_MSLog[0] + 20, ImVec2(200, 100));
		sprintf_s(title, 25, "Mem used %.1f", m_MemLog[m_MemLog.size() - 1]);
		ImGui::PlotHistogram("##memory", &m_MemLog[0], (int)m_MemLog.size(), 0, title, 0.0f, 10000.0f, ImVec2(200, 100));
		
		ImGui::Text("Allocation count %i", Wiwa::AllocationMetrics::allocation_count);
		ImGui::Text("Bytes allocated %i", Wiwa::AllocationMetrics::bytes_allocated);
	
	}
	if (ImGui::CollapsingHeader("Time"))
	{

		ImGui::Text("Time since startup %.2fs", Wiwa::Time::GetRealTimeSinceStartup());
		ImGui::Text("Real time delta time %.3fms", Wiwa::Time::GetRealDeltaTime());
		ImGui::Text("Frame count %i", Wiwa::Time::GetFrameCount());

		if (Wiwa::Time::IsPlaying())
		{
			ImGui::Text("Game time since startup %.2fs", Wiwa::Time::GetTime());
			ImGui::Text("Game delta time %.2fms", Wiwa::Time::GetDeltaTime());
			ImGui::Text("Game frame count %i", Wiwa::Time::GetGameFrameCount());
			float timeScale = Wiwa::Time::GetTimeScale();
			if (ImGui::SliderFloat("Time scale", &timeScale, -2.0f, 5.0f))
			{
				Wiwa::Time::SetTimeScale(timeScale);
			}
		}
		// static ImVec2 curves[64];
		// ImGui::Editor("Chuchawiwa", ImVec2{ 64, 64 }, 64, curves);
	}
	if (ImGui::CollapsingHeader("Hardware"))
	{
		ImGui::Text("GLFW version:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", info.glfwVer);
		ImGui::Separator();
		ImGui::Text("CPUs:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", info.numCores);
		ImGui::Text("System RAM:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.1fMb", info.ram);
		ImGui::Separator();
		ImGui::Text("GPU:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", info.gpu);
		ImGui::Text("Brand:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", info.gpuBrand);
		ImGui::Text("Total VRAM:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.1fMb", info.gpuVRAM);
		ImGui::Text("VRAM Usage:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), " %0.1fMb", info.gpuVRAMUsage);
		ImGui::Text("VRAM Available:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.1fMb", info.gpuVRAMAV);
		ImGui::Text("VRAM Reserved:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.1fMb", info.gpuVRAMReserve);
	}
	if (ImGui::CollapsingHeader("Gamepad axis"))
	{
		ImGui::Text("Deadzone %f", Wiwa::GameStateManager::s_GamepadDeadzone);
		ImGui::Text("Deadzone left %f X", Wiwa::Input::GetLeftDeadzone().x);
		ImGui::Text("Deadzone left %f Y", Wiwa::Input::GetLeftDeadzone().y);
		ImGui::Text("Deadzone right %f X", Wiwa::Input::GetRightDeadzone().x);
		ImGui::Text("Deadzone right %f Y", Wiwa::Input::GetRightDeadzone().y);


		ImGui::Text("Raw Gamepad 1");
		ImGui::Text("Left X: %f", Wiwa::Input::GetRawAxis( Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::GameStateManager::s_GamepadDeadzone));
		ImGui::Text("Left Y: %f", Wiwa::Input::GetRawAxis( Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone));
		ImGui::Text("Right X: %f", Wiwa::Input::GetRawAxis( Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::RightX, Wiwa::GameStateManager::s_GamepadDeadzone));
		ImGui::Text("Right Y: %f", Wiwa::Input::GetRawAxis( Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::RightY, Wiwa::GameStateManager::s_GamepadDeadzone));
		ImGui::Text("Left trigger: %f", Wiwa::Input::GetRawAxis( Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftTrigger, Wiwa::GameStateManager::s_GamepadDeadzone));
		ImGui::Text("Right trigger: %f", Wiwa::Input::GetRawAxis( Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::RightTrigger, Wiwa::GameStateManager::s_GamepadDeadzone));

		ImGui::Text("Gamepad 1 ");
		ImGui::Text("Left X: %f", Wiwa::Input::GetAxis(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::GameStateManager::s_GamepadDeadzone));
		ImGui::Text("Left Y: %f", Wiwa::Input::GetAxis(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone));
		ImGui::Text("Right X: %f", Wiwa::Input::GetAxis(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::RightX, Wiwa::GameStateManager::s_GamepadDeadzone));
		ImGui::Text("Right Y: %f", Wiwa::Input::GetAxis(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::RightY, Wiwa::GameStateManager::s_GamepadDeadzone));
		ImGui::Text("Left trigger: %f", Wiwa::Input::GetAxis(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftTrigger, Wiwa::GameStateManager::s_GamepadDeadzone));
		ImGui::Text("Right trigger: %f", Wiwa::Input::GetAxis(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::RightTrigger, Wiwa::GameStateManager::s_GamepadDeadzone));

		ImGui::Separator();
		
		ImGui::Text("Gamepad 2");
		ImGui::Text("Left X: %f", Wiwa::Input::GetRawAxis(Wiwa::Gamepad::GamePad2, Wiwa::Gamepad::LeftX, 0.f));
		ImGui::Text("Left Y: %f", Wiwa::Input::GetRawAxis(Wiwa::Gamepad::GamePad2, Wiwa::Gamepad::LeftY, 0.f));
		ImGui::Text("Right X: %f", Wiwa::Input::GetRawAxis(Wiwa::Gamepad::GamePad2, Wiwa::Gamepad::RightX, 0.f));
		ImGui::Text("Right Y: %f", Wiwa::Input::GetRawAxis(Wiwa::Gamepad::GamePad2, Wiwa::Gamepad::RightY, 0.f));
		ImGui::Text("Left trigger: %f", Wiwa::Input::GetRawAxis(Wiwa::Gamepad::GamePad2, Wiwa::Gamepad::LeftTrigger, 0.f));
		ImGui::Text("Right trigger: %f", Wiwa::Input::GetRawAxis(Wiwa::Gamepad::GamePad2, Wiwa::Gamepad::RightTrigger, 0.f));
	}
	if (ImGui::CollapsingHeader("Gamepad joystick"))
	{
		ImGui::Text("Deadzone %f", Wiwa::GameStateManager::s_GamepadDeadzone);

		ImGui::Text("Raw Gamepad 1");
		ImGui::Text("Left X: %f", Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).x);
		ImGui::Text("Left Y: %f", Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).y);
		ImGui::Text("Right X: %f", Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::RightX, Wiwa::Gamepad::RightY, Wiwa::GameStateManager::s_GamepadDeadzone).x);
		ImGui::Text("Right Y: %f", Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::RightX, Wiwa::Gamepad::RightY, Wiwa::GameStateManager::s_GamepadDeadzone).y);

		ImGui::Text("Gamepad 1 ");
		ImGui::Text("Left X: %f", Wiwa::Input::GetJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).x);
		ImGui::Text("Left Y: %f", Wiwa::Input::GetJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).y);
		ImGui::Text("Right X: %f", Wiwa::Input::GetJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::RightX, Wiwa::Gamepad::RightY, Wiwa::GameStateManager::s_GamepadDeadzone).x);
		ImGui::Text("Right Y: %f", Wiwa::Input::GetJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::RightX, Wiwa::Gamepad::RightY, Wiwa::GameStateManager::s_GamepadDeadzone).y);
	}
	ImGui::End();
}

void ConfigurationPanel::Update()
{
	info = Wiwa::Application::Get().GetSystemInfo();

	if (m_MSLog.size() > 64)
		m_MSLog.erase(0);

	if (m_FPSLog.size() > 64)
		m_FPSLog.erase(0);

	if (m_MemLog.size() > 64)
		m_MemLog.erase(0);


	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.WorkingSetSize >> 20;
	m_MemLog.push_back((float)virtualMemUsedByMe);
}
