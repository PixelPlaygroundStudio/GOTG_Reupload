#pragma once

#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/render/Camera.h>
#include <vector>

#include <Wiwa/render/RenderManager.h>

typedef size_t CameraId;

namespace Wiwa{
	class WI_API CameraManager {
	private:
		 std::vector<Camera*> m_Cameras;

		 std::vector<CameraId> m_CamerasAlive;
		 std::vector<CameraId> m_RemovedCameras;

		 CameraId m_ActiveCamera;
	public:
		static Camera* editorCamera;
	public:
		CameraManager();
		~CameraManager();
		void Init();
		void Update();
		void Clear();
		void CleanUp();

		void GenerateBuffers();

		void setActiveCamera(CameraId cameraId) { m_ActiveCamera = cameraId; RenderManager::SetLayerCamera(0, m_Cameras[cameraId]); }

		CameraId getActiveCameraId() { return m_ActiveCamera; }
		Camera* getActiveCamera() { return m_ActiveCamera == size_t(-1) ? NULL : m_Cameras[m_ActiveCamera]; }

		size_t getCameraSize() { return m_CamerasAlive.size(); }

		size_t CreateCamera();
		size_t CreatePerspectiveCamera(const float fov, const float aspectRatio, const float nearPlaneDistance = 0.1f, const float farPlaneDistance = 100.0f, bool genbuffers=true);
		void DestroyCamera(CameraId cameraId);
		Camera* getCamera(CameraId cameraId) { return m_Cameras[cameraId]; }
		std::vector<CameraId>& getCameras() { return m_CamerasAlive; }
	};

}