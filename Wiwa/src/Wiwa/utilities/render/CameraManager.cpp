#include "wipch.h"
#include "CameraManager.h"
#include <Wiwa/core/Application.h>
#include <Wiwa/core/Renderer3D.h>
#include <Wiwa/physics/PhysicsManager.h>
#include <Wiwa/scene/SceneManager.h>
#include <Wiwa/utilities/render/LightManager.h>

namespace Wiwa {
	Camera* CameraManager::editorCamera = nullptr;
	CameraManager::CameraManager()
	{
		Init();
	}
	CameraManager::~CameraManager()
	{
		CleanUp();
	}
	void CameraManager::Init()
	{
		if (!editorCamera)
		{
			editorCamera = new Camera();
			Wiwa::Size2i& res = Wiwa::Application::Get().GetTargetResolution();
			float ar = res.w / (float)res.h;
			editorCamera->SetPerspective(60, ar, 0.1f, 10000.0f);
			editorCamera->setPosition({ 0.0f, 1.0f, 5.0f });
			editorCamera->lookat({ 0.0f, 0.0f, 0.0f });
		}
		m_ActiveCamera = -1;
	}

	void CameraManager::Update()
	{
		LightManager& lman = SceneManager::getActiveScene()->GetLightManager();
		if (editorCamera)
		{
			editorCamera->frameBuffer->Clear();

			
			editorCamera->shadowBuffer->Clear();
				
			if (editorCamera->drawFrustrums)
			{
				Wiwa::Application::Get().GetRenderer3D().RenderFrustrums();
				Wiwa::SceneManager::getActiveScene()->GetPhysicsManager().DebugDrawWorld();
			}
		}
		size_t cameraCount = m_CamerasAlive.size();
		for (size_t i = 0; i < cameraCount; i++)
		{
			CameraId cam_id = m_CamerasAlive[i];
			m_Cameras[cam_id]->frameBuffer->Clear();

			m_Cameras[cam_id]->shadowBuffer->Clear();

			lman.Update(m_Cameras[cam_id]);

			if (m_Cameras[cam_id]->drawFrustrums)
			{
				Wiwa::SceneManager::getActiveScene()->GetPhysicsManager().DebugDrawWorld();
			}
		}

	
	}
	void CameraManager::Clear()
	{
		size_t cameraCount = m_CamerasAlive.size();
		for (size_t i = 0; i < cameraCount; i++)
		{
			CameraId cam_id = m_CamerasAlive[i];
			delete m_Cameras[cam_id];
		}

		m_Cameras.clear();
		m_CamerasAlive.clear();
		m_RemovedCameras.clear();

		Wiwa::RenderManager::ClearCameraLayers();

		//delete editorCamera;

		m_ActiveCamera = -1;
	}
	void CameraManager::CleanUp()
	{
		Clear();
	}

	void CameraManager::GenerateBuffers()
	{
		size_t cameraCount = m_CamerasAlive.size();
		for (size_t i = 0; i < cameraCount; i++)
		{
			CameraId cam_id = m_CamerasAlive[i];
			
			m_Cameras[cam_id]->GenerateBuffers();
		}
	}

	size_t CameraManager::CreateCamera()
	{
		Camera* cam = new Camera();

		size_t r_size = m_RemovedCameras.size();

		CameraId cameraId = -1;

		if (r_size > 0) {
			cameraId = m_RemovedCameras[r_size - 1];
			m_RemovedCameras.pop_back();
		}
		else {
			cameraId = m_Cameras.size();
			m_Cameras.emplace_back();
		}

		m_Cameras[cameraId] = cam;
		m_CamerasAlive.push_back(cameraId);

		size_t ncam = -1;

		if (m_ActiveCamera == ncam) {
			setActiveCamera(cameraId);
		}

		return cameraId;
	}
	size_t CameraManager::CreatePerspectiveCamera(const float fov, const float aspectRatio, const float nearPlaneDistance, const float farPlaneDistance, bool genbuffers)
	{
		CameraId cameraId = CreateCamera();

		Camera* cam = getCamera(cameraId);
		cam->SetPerspective(fov, aspectRatio, nearPlaneDistance, farPlaneDistance, genbuffers);
		
		return cameraId;
	}

	void CameraManager::DestroyCamera(CameraId cameraId) {
		m_RemovedCameras.push_back(cameraId);

		size_t c_alive = m_CamerasAlive.size();

		for (size_t i = 0; i < c_alive; i++) {
			if (m_CamerasAlive[i] == cameraId) {
				m_CamerasAlive.erase(m_CamerasAlive.begin() + i);
				break;
			}
		}

		delete m_Cameras[cameraId];
		m_Cameras[cameraId] = NULL;

		if (cameraId == m_ActiveCamera) {
			if (m_CamerasAlive.size() > 0) {
				setActiveCamera(m_CamerasAlive[0]);
			}
			else {
				m_ActiveCamera = -1;
				RenderManager::SetLayerCamera(0, NULL);
			}
		}
	}
}
