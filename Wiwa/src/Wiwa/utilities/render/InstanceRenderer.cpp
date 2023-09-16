#include <wipch.h>

#include "InstanceRenderer.h"

#include <Wiwa/core/Resources.h>
#include <Wiwa/core/Renderer2D.h>

#include <Wiwa/core/Application.h>

#include <glew.h>

namespace Wiwa {
	InstanceRenderer::InstanceRenderer(uint32_t maxInstances) :
		m_MaxInstances(maxInstances),
		m_InstanceCount(0),
		m_InstanceVertex(nullptr),
		m_GeneratedBuffers(false)
	{
	}

	InstanceRenderer::~InstanceRenderer()
	{
		GL(DeleteBuffers(1, &m_VBO));
		GL(DeleteBuffers(1, &m_EBO));
		GL(DeleteBuffers(1, &m_IVBO));
		GL(DeleteVertexArrays(1, &m_VAO));

		delete[] m_InstanceVertex;
	}

	void InstanceRenderer::GenerateBuffers()
	{
		if (m_GeneratedBuffers) return;

		m_GeneratedBuffers = true;

		// Vertex quad	
		float verticesInstancedTexture[] = {
			 0.5f,  0.5f, 0.0f, // Top right -> 1.0f, 1.0f
			 0.5f, -0.5f, 0.0f, // Bottom right -> 1.0f, 0.0f
			-0.5f, -0.5f, 0.0f, // Bottom left -> 0.0f, 0.0f
			-0.5f,  0.5f, 0.0f  // Top left -> 0.0f, 1.0f
		};

		// Create buffers
		GL(GenBuffers(1, &m_VBO));
		GL(GenBuffers(1, &m_EBO));
		GL(GenVertexArrays(1, &m_VAO));

		GL(BindVertexArray(m_VAO));

		GL(BindBuffer(GL_ARRAY_BUFFER, m_VBO));
		GL(BufferData(GL_ARRAY_BUFFER, sizeof(verticesInstancedTexture), verticesInstancedTexture, GL_STATIC_DRAW));

		GL(EnableVertexAttribArray(0));
		GL(VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0));

		/*glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexture), (void*)(3 * sizeof(float)));*/

		unsigned int indexes[] = {
			0, 1, 3, 1, 2, 3
		};

		GL(BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
		GL(BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW));

		GL(GenBuffers(1, &m_IVBO));
		GL(BindBuffer(GL_ARRAY_BUFFER, m_IVBO));
		GL(BufferData(GL_ARRAY_BUFFER, sizeof(VertexInstanceTexture) * m_MaxInstances, nullptr, GL_DYNAMIC_DRAW));

		GL(EnableVertexAttribArray(2));
		GL(VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)0));
		GL(VertexAttribDivisor(2, 1));

		GL(EnableVertexAttribArray(3));
		GL(VertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)(2 * sizeof(float))));
		GL(VertexAttribDivisor(3, 1));

		GL(EnableVertexAttribArray(4));
		GL(VertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)(4 * sizeof(float))));
		GL(VertexAttribDivisor(4, 1));

		GL(EnableVertexAttribArray(5));
		GL(VertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)(8 * sizeof(float))));
		GL(VertexAttribDivisor(5, 1));

		GL(EnableVertexAttribArray(6));
		GL(VertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)(9 * sizeof(float))));
		GL(VertexAttribDivisor(6, 1));

		GL(EnableVertexAttribArray(7));
		GL(VertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)(11 * sizeof(float))));
		GL(VertexAttribDivisor(7, 1));

		GL(EnableVertexAttribArray(8));
		GL(VertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)(13 * sizeof(float))));
		GL(VertexAttribDivisor(8, 1));

		GL(EnableVertexAttribArray(9));
		GL(VertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)(15 * sizeof(float))));
		GL(VertexAttribDivisor(9, 1));

		GL(EnableVertexAttribArray(10));
		GL(VertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)(17 * sizeof(float))));
		GL(VertexAttribDivisor(10, 1));

		GL(EnableVertexAttribArray(11));
		GL(VertexAttribPointer(11, 1, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)(18 * sizeof(float))));
		GL(VertexAttribDivisor(11, 1));

		GL(EnableVertexAttribArray(12));
		GL(VertexAttribPointer(12, 1, GL_FLOAT, GL_FALSE, sizeof(VertexInstanceTexture), (void*)(19 * sizeof(float))));
		GL(VertexAttribDivisor(12, 1));
	}

	void InstanceRenderer::Bind()
	{
		if (!m_GeneratedBuffers) {
			GenerateBuffers();
		}

		GL(BindVertexArray(m_VAO));
	}

	void InstanceRenderer::Unbind()
	{
		GL(BindVertexArray(0));
	}

	void InstanceRenderer::Init(const char* shader_path)
	{
		Application& app = Application::Get();

		// Load shader
		// TODO: Use shader pipeline
		//m_InstanceShaderId = Resources::Load<Shader>(shader_path);
		//m_InstanceShader = new Shader();//Resources::GetResourceById<Shader>(m_InstanceShaderId);
		m_InstanceShader.Init(shader_path);

		m_InstanceShader.Bind();
		uint32_t texs_id = GL(GetUniformLocation(m_InstanceShader.getID(), "u_Textures"));

		// Set samplers id
		int samplers[MAX_INSTANCE_TEXTURES];
		for (int i = 0; i < MAX_INSTANCE_TEXTURES; i++) samplers[i] = i;
		GL(Uniform1iv(texs_id, MAX_INSTANCE_TEXTURES, samplers));

		m_InstanceVertex = new VertexInstanceTexture[m_MaxInstances];

		m_OrthoLocation = m_InstanceShader.getUniformLocation("u_Proj");
		m_ViewLocation = m_InstanceShader.getUniformLocation("u_View");

		m_InstanceShader.UnBind();
	}

	void InstanceRenderer::Update()
	{
		GL(BindBuffer(GL_ARRAY_BUFFER, m_IVBO));
		GL(BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexInstanceTexture) * m_InstanceCount, m_InstanceVertex));
	}

	void InstanceRenderer::Render(glm::mat4& proj, glm::mat4& view)
	{
		m_InstanceShader.Bind();
		m_InstanceShader.setUniform(m_OrthoLocation, proj);
		m_InstanceShader.setUniform(m_ViewLocation, view);

		GLuint texSize = static_cast<GLuint>(m_Textures.size());

		for (GLuint i = 0; i < texSize; i++) {

			if (m_Textures[i] == -1) {
				WI_ERROR("Tried to bind invalid texture in instance renderer");
				continue;
			}
			GL(BindTextureUnit(i, m_Textures[i]));
		}

		GL(DrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_InstanceCount));

		m_InstanceShader.UnBind();
	}

	uint32_t InstanceRenderer::AddInstance(uint32_t textureId, const Vector2i& position, const Size2i& size, const Color4f& color, const TextureClip& clip, Renderer2D::Pivot pivot, float rotation)
	{
		size_t instance_id = WI_INVALID_INDEX;

		size_t remove_size = m_RemovedInstances.size();

		if (remove_size > 0) {
			instance_id = m_RemovedInstances[remove_size - 1];
			m_RemovedInstances.pop_back();
		}
		else {
			if (m_InstanceCount == m_MaxInstances)
				return instance_id;

			instance_id = m_InstanceCount++;
		}

		m_InstanceVertex[instance_id].position = Renderer2D::CalculateScreenGlPos(position, size, pivot);
		m_InstanceVertex[instance_id].scale = { static_cast<float>(size.x), static_cast<float>(size.y) };
		m_InstanceVertex[instance_id].color = color;
		m_InstanceVertex[instance_id].rotation = rotation;

		int texid = AddTexture(textureId);

		m_InstanceVertex[instance_id].textureId = static_cast<float>(texid);

		m_InstanceVertex[instance_id].textureClip = clip;

		m_InstanceVertex[instance_id].active = 1.0f;

		m_InstanceVertex[instance_id].priority = 0.0f;

		return instance_id;
	}

	uint32_t InstanceRenderer::AddInstance(VertexInstanceTexture& data)
	{
		size_t remove_size = m_RemovedInstances.size();
		size_t instance_id = WI_INVALID_INDEX;

		if (remove_size > 0) {
			instance_id = m_RemovedInstances[remove_size - 1];
			m_RemovedInstances.pop_back();
		}
		else {
			if (m_InstanceCount == m_MaxInstances)
				return instance_id;

			instance_id = m_InstanceCount++;
		}

		m_InstanceVertex[instance_id] = data;

		return instance_id;
	}

	void InstanceRenderer::UpdateInstance(uint32_t id, const Vector2i& position, const  Size2i& size, const Color4f& color, Renderer2D::Pivot pivot)
	{
		m_InstanceVertex[id].position = Renderer2D::CalculateScreenGlPos(position, size, pivot);
		m_InstanceVertex[id].scale = { static_cast<float>(size.x), static_cast<float>(size.y) };
		m_InstanceVertex[id].color = color;
	}

	void InstanceRenderer::UpdateInstancePosition(uint32_t id, const Vector2i& position, Renderer2D::Pivot pivot)
	{
		Size2i s = { (int)m_InstanceVertex[id].scale.w, (int)m_InstanceVertex[id].scale.h };
		
		m_InstanceVertex[id].position = Renderer2D::CalculateScreenGlPos(position, s, pivot);
	}

	void InstanceRenderer::UpdateInstanceSize(uint32_t id, const Vector2i& pos, const Size2i& size, Renderer2D::Pivot pivot)
	{
		m_InstanceVertex[id].position = Renderer2D::CalculateScreenGlPos(pos, size, pivot);
		m_InstanceVertex[id].scale = { static_cast<float>(size.x), static_cast<float>(size.y) };
	}

	void InstanceRenderer::UpdateInstanceTexture(uint32_t id, uint32_t textureId)
	{
		int texid = AddTexture(textureId);

		m_InstanceVertex[id].textureId = static_cast<float>(texid);
	}

	void InstanceRenderer::UpdateInstanceClip(uint32_t id, const TextureClip& clip)
	{
		m_InstanceVertex[id].textureClip = clip;
	}

	void InstanceRenderer::UpdateInstanceColor(uint32_t id, const Color4f& color)
	{
		m_InstanceVertex[id].color = color;
	}

	void InstanceRenderer::UpdateInstanceRotation(uint32_t id, float rotation)
	{
		m_InstanceVertex[id].rotation = rotation;
	}

	int InstanceRenderer::AddTexture(uint32_t texture)
	{
		size_t texSize = m_Textures.size();

		int index = static_cast<int>(texSize);

		for (size_t i = 0; i < texSize; i++) {
			if (m_Textures[i] == texture) {
				index = static_cast<int>(i);
				break;
			}
		}

		if (index == texSize) {
			if (texSize >= MAX_INSTANCE_TEXTURES) {
				return -1;
			}
			else
			{
				m_Textures.push_back(texture);
			}
		}

		return index;
	}

	void InstanceRenderer::DisableInstance(uint32_t instance)
	{
		m_InstanceVertex[instance].active = 0.0f;
	}

	void InstanceRenderer::EnableInstance(uint32_t instance)
	{
		m_InstanceVertex[instance].active = 1.0f;
	}

	void InstanceRenderer::SetEnabled(uint32_t instance, bool enabled)
	{
		m_InstanceVertex[instance].active = enabled ? 1.0f : 0.0f;
	}

	void InstanceRenderer::RemoveInstance(uint32_t instance)
	{
		m_InstanceVertex[instance].active = 0.0f;

		m_RemovedInstances.push_back(instance);
	}
	void InstanceRenderer::SetPriority(uint32_t instance, int priority)
	{
		m_InstanceVertex[instance].priority = priority;
	}
}