#include <wipch.h>
#include "LightManager.h"
#include "Wiwa/ecs/components/Transform3D.h"
#include "Wiwa/ecs/EntityManager.h"
#include "Wiwa/scene/SceneManager.h"
#include "Wiwa/utilities/render/Camera.h"

Wiwa::LightManager::LightManager()
{
	m_LightProj = glm::mat4(1.f);
	m_LightView = glm::mat4(1.f);
	m_LightMVP = glm::mat4(1.f);
	m_LightPos = glm::vec3(0.f);
}

Wiwa::LightManager::~LightManager()
{
	m_PointLights.clear();
	m_SpotLights.clear();
}

void Wiwa::LightManager::AddPointLight(size_t light)
{
	m_PointLights.push_back(light);
}

void Wiwa::LightManager::RemovePointLight(size_t light)
{
	for (size_t i = 0; i < m_PointLights.size(); i++)
	{
		if (m_PointLights[i] == light)
		{
			m_PointLights.erase(m_PointLights.begin() + i);
			break;
		}
	}
}

void Wiwa::LightManager::SetScene(Scene* scene)
{
	m_Scene = scene;
}

void Wiwa::LightManager::Update(Camera* camera)
{
	Wiwa::Transform3D* lightTrans = m_Scene->GetEntityManager().GetComponent<Wiwa::Transform3D>(m_DirectionalLight);
	if (lightTrans)
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(-lightTrans->rotation.y)) * cos(glm::radians(-lightTrans->rotation.x));
		direction.y = sin(glm::radians(-lightTrans->rotation.x));
		direction.z = sin(glm::radians(-lightTrans->rotation.y)) * cos(glm::radians(-lightTrans->rotation.x));

		glm::vec3 front = glm::normalize(direction);
		lightTrans->localPosition = camera->getPosition();
		m_LightPos = camera->getPosition();
		m_LightView = glm::lookAt(lightTrans->localPosition, lightTrans->localPosition + front, glm::vec3(0.f, 1.0f, 0.0f));
		m_LightProj = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 100.f);

		m_LightMVP = m_LightProj * m_LightView;
	}
}

void Wiwa::LightManager::AddSpotLight(size_t light)
{
	m_SpotLights.push_back(light);
}

void Wiwa::LightManager::RemoveSpotLight(size_t light)
{
	for (size_t i = 0; i < m_SpotLights.size(); i++)
	{
		if (m_SpotLights[i] == light)
		{
			m_SpotLights.erase(m_SpotLights.begin() + i);
			break;
		}
	}
}
