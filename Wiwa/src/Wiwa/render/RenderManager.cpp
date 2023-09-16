#include <wipch.h>
#include <Wiwa/render/RenderManager.h>
#include <Wiwa/core/Application.h>

#include <glew.h>

namespace Wiwa {
	RenderLayer RenderManager::m_RenderLayers[MAX_LAYERS];
	FrameBuffer* RenderManager::m_FrameBuffer = nullptr;
	Shader* RenderManager::m_Shader = nullptr;
	Shader* RenderManager::m_BlurShader = nullptr;
	uint32_t RenderManager::m_VAO;
	uint32_t RenderManager::m_VBO;
	uint32_t RenderManager::m_EBO;
	glm::mat4 RenderManager::m_OrthoProj;
	glm::mat4 RenderManager::m_View;
	glm::mat4 RenderManager::m_Model;
	uint32_t RenderManager::m_BoneLocation[MAX_BONES];
	uint32_t RenderManager::m_OrthoLoc;
	uint32_t RenderManager::m_ViewLoc;
	uint32_t RenderManager::m_ModelLoc;
	bool RenderManager::m_RenderOnMainWindow = false;

	RenderManager::~RenderManager()
	{
		GL(DeleteBuffers(1, &m_VBO));
		GL(DeleteBuffers(1, &m_EBO));
		GL(DeleteVertexArrays(1, &m_VAO));
	}

	void RenderManager::Init(int width, int height)
	{
		// Quad vertices
		float vertices[] = {
			 // Positions			// Texture coords
			 0.5f,  0.5f, 0.0f,		1.0f, 1.0f, // Top right -> 1.0f, 1.0f
			 0.5f, -0.5f, 0.0f,		1.0f, 0.0f, // Bottom right -> 1.0f, 0.0f
			-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, // Bottom left -> 0.0f, 0.0f
			-0.5f,  0.5f, 0.0f,		0.0f, 1.0f  // Top left -> 0.0f, 1.0f
		};

		// Quad indices
		unsigned int indexes[] = {
			0, 1, 3, 1, 2, 3
		};

		// Generate buffers
		GL(GenVertexArrays(1, &m_VAO));
		GL(GenBuffers(1, &m_VBO));
		GL(GenBuffers(1, &m_EBO));

		GL(BindVertexArray(m_VAO));

		// Bind vertices
		GL(BindBuffer(GL_ARRAY_BUFFER, m_VBO));
		GL(BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

		// Bind indices
		GL(BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
		GL(BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW));

		// Position attribute
		GL(EnableVertexAttribArray(0));
		GL(VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));

		// Texture coords
		GL(EnableVertexAttribArray(1));
		GL(VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

		// Render objects
		m_FrameBuffer = new FrameBuffer();
		m_BlurShader = new Shader();
		m_Shader = new Shader();

		m_FrameBuffer->Init(width, height);
		m_BlurShader->Init("resources/shaders/renderlayer/blur");
		m_Shader->Init("resources/shaders/renderlayer/renderlayer");

		m_OrthoLoc = m_Shader->getUniformLocation("u_Proj");
		m_ViewLoc = m_Shader->getUniformLocation("u_View");
		m_ModelLoc = m_Shader->getUniformLocation("u_Model");




		for (unsigned int i = 0; i < MAX_BONES; i++) {
			char Name[128];
			memset(Name, 0, sizeof(Name));
			snprintf(Name, sizeof(Name), "u_Bones[%d]", i);
			m_BoneLocation[i] = m_Shader->getUniformLocation(Name);
		}

		// Init orthographic projection
		m_OrthoProj = glm::ortho(0.0f, (float)width, (float)height, 0.0f, 0.1f, 100.0f);
		// Init main camera view
		m_View = glm::mat4(1.0f);
		m_View = glm::translate(m_View, glm::vec3(0.0f, 0.0f, -3.0f));
		// Init model
		m_Model = glm::mat4(1.0f);
		m_Model = glm::translate(m_Model, glm::vec3(width / 2.0f, height / 2.0f, 0.0f));
		m_Model = glm::scale(m_Model, glm::vec3((float)width, (float)height, 1.0f));
	}

	void RenderManager::ClearCameraLayers()
	{
		for (int i = 0; i < MAX_LAYERS; i++) {
			m_RenderLayers[i].setCamera(NULL);
		}
	}

	void RenderManager::Update()
	{
		OPTICK_EVENT("Renderer manager Update");
		// Set viewport
		GL(Viewport(0, 0, m_FrameBuffer->getWidth(), m_FrameBuffer->getHeight()));

		// Bind framebuffer
		m_FrameBuffer->Bind(false); // No clear so we clear without transparency
		GL(ClearColor(0.1f, 0.1f, 0.1f, 1.0f));
		GL(Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		// Bind shader
		m_Shader->Bind();

		// Set MVP uniforms
		m_Shader->setUniform(m_OrthoLoc, m_OrthoProj);
		m_Shader->setUniform(m_ViewLoc, m_View);
		m_Shader->setUniform(m_ModelLoc, m_Model);


		// Bind VAO
		GL(BindVertexArray(m_VAO));

		for (int i = 0; i < MAX_LAYERS; i++) {
			// Take cameras in reverse order for proper drawing order
			Camera* cam = m_RenderLayers[MAX_LAYERS - i - 1].getCamera();
			if (cam) {
				GL(ActiveTexture(GL_TEXTURE0));
				GL(BindTexture(GL_TEXTURE_2D, cam->frameBuffer->getColorBufferTexture()));
				// Draw elements
				GL(DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
			}
		}
		
		// Unbind shader and framebuffer
		m_FrameBuffer->Unbind();
		

		if (m_RenderOnMainWindow) {
			uint32_t w = Wiwa::Application::Get().GetWindow().GetWidth();
			uint32_t h = Wiwa::Application::Get().GetWindow().GetHeight();
			GL(Viewport(0, 0, w, h));

			GL(ActiveTexture(GL_TEXTURE0));
			GL(BindTexture(GL_TEXTURE_2D, m_FrameBuffer->getColorBufferTexture()));
			GL(DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
		}

		GL(BindVertexArray(0));
		m_Shader->UnBind();
	}

	void RenderManager::UpdateSingle(size_t layer_id)
	{
		// Set viewport
		GL(Viewport(0, 0, m_FrameBuffer->getWidth(), m_FrameBuffer->getHeight()));

		// Bind framebuffer
		m_FrameBuffer->Bind(false); // No clear so we clear without transparency
		GL(ClearColor(0.1f, 0.1f, 0.1f, 1.0f));
		GL(Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		// Bind shader
		m_Shader->Bind();

		// Set MVP uniforms
		m_Shader->setUniform(m_OrthoLoc, m_OrthoProj);
		m_Shader->setUniform(m_ViewLoc, m_View);
		m_Shader->setUniform(m_ModelLoc, m_Model);


		// Bind VAO
		GL(BindVertexArray(m_VAO));

		Camera* cam = m_RenderLayers[layer_id].getCamera();
		if (cam) {
			
			GL(ActiveTexture(GL_TEXTURE0));
			GL(BindTexture(GL_TEXTURE_2D, cam->frameBuffer->getColorBufferTexture()));
			// Draw elements
			GL(DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
		}

		// Unbind shader and framebuffer
		m_Shader->UnBind();
		m_FrameBuffer->Unbind();

		if (m_RenderOnMainWindow) {
			uint32_t w = Wiwa::Application::Get().GetWindow().GetWidth();
			uint32_t h = Wiwa::Application::Get().GetWindow().GetHeight();
			GL(Viewport(0, 0, w, h));

			m_Shader->Bind();

			GL(ActiveTexture(GL_TEXTURE0));
			GL(BindTexture(GL_TEXTURE_2D, m_FrameBuffer->getColorBufferTexture()));
			GL(DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

			m_Shader->UnBind();
		}

		GL(BindVertexArray(0));
	}

	void RenderManager::BindVAO()
	{
		GL(BindVertexArray(m_VAO));
	}

	void RenderManager::Destroy()
	{
		GL(DeleteVertexArrays(1, &m_VAO));
		GL(DeleteBuffers(1, &m_VBO));
		GL(DeleteBuffers(1, &m_EBO));

		delete m_FrameBuffer;
		delete m_Shader;
		delete m_BlurShader;
	}
}