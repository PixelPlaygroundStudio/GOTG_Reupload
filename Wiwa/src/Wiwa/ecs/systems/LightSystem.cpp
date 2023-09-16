#include <wipch.h>
#include "LightSystem.h"
#include <Wiwa\utilities\render\LightManager.h>
#include <Wiwa\scene\SceneManager.h>
#include <Wiwa\scene\Scene.h>

Wiwa::LightSystem::LightSystem()
{
	
}

Wiwa::LightSystem::~LightSystem()
{
	if (GetComponent<PointLight>())
	{
		m_Scene->GetLightManager().RemovePointLight(m_EntityId);
	}
	else if (GetComponent<DirectionalLight>())
	{
		m_Scene->GetLightManager().SetDirectionalLight(-1);
	}
	else if (GetComponent<SpotLight>())
	{
		m_Scene->GetLightManager().RemoveSpotLight(m_EntityId);
	}
}

void Wiwa::LightSystem::OnSystemAdded()
{
	if (GetComponent<PointLight>())
	{
		m_Scene->GetLightManager().AddPointLight(m_EntityId);
	}
	else if (GetComponent<DirectionalLight>())
	{
		m_Scene->GetLightManager().SetDirectionalLight(m_EntityId);
	}
	else if (GetComponent<SpotLight>())
	{
		m_Scene->GetLightManager().AddSpotLight(m_EntityId);
	}
}