#pragma once

#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/render/RenderLayer.h>
#include <Wiwa/utilities/render/FrameBuffer.h>
#include <Wiwa/utilities/render/shaders/Shader.h>

#include <glm/mat4x4.hpp>

#include <vector>

#define MAX_LAYERS 10

namespace Wiwa {
	class WI_API RenderManager {
	private:
		RenderManager();
		~RenderManager();

		static FrameBuffer* m_FrameBuffer;
		static glm::mat4 m_OrthoProj;
		static glm::mat4 m_View;
		static glm::mat4 m_Model;
		static uint32_t m_BoneLocation[MAX_BONES];
		static RenderLayer m_RenderLayers[MAX_LAYERS];

		static Shader* m_Shader;
		static Shader* m_BlurShader;

		static uint32_t m_VAO;
		static uint32_t m_VBO;
		static uint32_t m_EBO;

		static uint32_t m_OrthoLoc;
		static uint32_t m_ViewLoc;
		static uint32_t m_ModelLoc;

		static bool m_RenderOnMainWindow;
	public:
		// Initialize render manager
		static void Init(int width, int height);

		// Set camera to specific layer
		static void SetLayerCamera(uint32_t layer_id, Camera* cam) { m_RenderLayers[layer_id].setCamera(cam); }

		// Clear all camera layers
		static void ClearCameraLayers();

		// Render all layers into one framebuffer
		static void Update();

		// Render single camera into framebuffer
		static void UpdateSingle(size_t layer_id);

		// Set if it should render the output to the main window
		static void SetRenderOnMainWindow(bool render_on_main_window) { m_RenderOnMainWindow = render_on_main_window; }

		static bool GetRenderOnMainWindow() { return m_RenderOnMainWindow; }

		// Get color texture
		static uint32_t getColorTexture() { return m_FrameBuffer->getColorBufferTexture(); }

		static void BindVAO();

		// Destroy render manager
		static void Destroy();
	};
}