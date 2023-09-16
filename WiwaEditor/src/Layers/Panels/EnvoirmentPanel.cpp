#include "EnvoirmentPanel.h"

#include <Wiwa\scene\SceneManager.h>
#include <Wiwa\utilities\render\LightManager.h>

EnvironmentPanel::EnvironmentPanel(EditorLayer* instance)
	: Panel("Environment", "", instance)
{
}

EnvironmentPanel::~EnvironmentPanel()
{
}

void EnvironmentPanel::Draw()
{
	ImGui::Begin(iconName.c_str(), &active);

	for (size_t i = 0; i < 6; i++)
	{
		
	}

	if (ImGui::CollapsingHeader("Info"))
	{
		ImGui::Text("Light count");
		ImGui::Separator();
		ImGui::Text("Point lights: %i", Wiwa::SceneManager::getActiveScene()->GetLightManager().GetPointLightsSize());
		ImGui::Text("Spot lights: %i", Wiwa::SceneManager::getActiveScene()->GetLightManager().GetSpotLightsSize());
	}
	ImGui::End();
}
