#include "MainMenuUISystem.h"

Wiwa::MainMenuUISystem::MainMenuUISystem()
{
}

Wiwa::MainMenuUISystem::~MainMenuUISystem()
{
}

void Wiwa::MainMenuUISystem::OnAwake()
{
}

void Wiwa::MainMenuUISystem::OnInit()
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	
	gm.canvas.at(1)->controls.at(2)->SetInitialSliderValue(Wiwa::Application::Get().ReturnMusicVolume());
	gm.canvas.at(1)->controls.at(3)->SetInitialSliderValue(Wiwa::Application::Get().ReturnSFXVolume());
	gm.canvas.at(1)->controls.at(0)->SetChecked(Wiwa::Application::Get().GetWindow().IsVSync());
	gm.canvas.at(1)->controls.at(1)->SetChecked(Wiwa::Application::Get().GetWindow().GetFullScreen());
}

void Wiwa::MainMenuUISystem::OnUpdate()
{
}

void Wiwa::MainMenuUISystem::OnDestroy()
{
}
