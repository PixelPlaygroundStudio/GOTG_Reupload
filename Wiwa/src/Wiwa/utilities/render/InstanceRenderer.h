#pragma once

#include <Wiwa/core/Core.h>

#include <Wiwa/utilities/math/Math.h>

#include "VertexBuffer.h"

#include <Wiwa/core/Resources.h>
#include <Wiwa/core/Renderer2D.h>

#define MAX_INSTANCE_TEXTURES 32

namespace Wiwa {
	class WI_API InstanceRenderer {
	private:
		ResourceId m_InstanceShaderId;
		Shader m_InstanceShader;

		std::vector<uint32_t> m_RemovedInstances;
		std::vector<int> m_Textures;

		VertexInstanceTexture* m_InstanceVertex;

		uint32_t m_VAO, m_VBO, m_EBO;
		uint32_t m_IVBO;

		uint32_t m_InstanceCount;
		uint32_t m_MaxInstances;

		uint32_t m_OrthoLocation;
		uint32_t m_ViewLocation;

		bool m_GeneratedBuffers;
	public:
		InstanceRenderer(uint32_t maxInstances);
		~InstanceRenderer();

		void GenerateBuffers();

		void Bind();
		void Unbind();

		void Init(const char* shader_path);
		void Update();
		void Render(glm::mat4& proj, glm::mat4& view);

		uint32_t AddInstance(uint32_t textureId, const Vector2i& position, const Size2i& size, const Color4f& color, const TextureClip& clip, Renderer2D::Pivot pivot = Renderer2D::Pivot::CENTER, float rotation = 0.f);
		uint32_t AddInstance(VertexInstanceTexture& data);

		void UpdateInstance(uint32_t id, const Vector2i& position, const Size2i& size, const Color4f& color, Renderer2D::Pivot pivot = Renderer2D::Pivot::CENTER);
		
		void UpdateInstancePosition(uint32_t id, const Vector2i& position, Renderer2D::Pivot pivot = Renderer2D::Pivot::CENTER);
		void UpdateInstanceSize(uint32_t id, const Vector2i& pos, const Size2i& size, Renderer2D::Pivot pivot = Renderer2D::Pivot::CENTER);
		void UpdateInstanceTexture(uint32_t id, uint32_t textureId);
		void UpdateInstanceClip(uint32_t id, const TextureClip& clip);
		void UpdateInstanceColor(uint32_t id, const Color4f& color);
		void UpdateInstanceRotation(uint32_t id, float rotation);

		VertexInstanceTexture& getInstanceData(uint32_t id) { return m_InstanceVertex[id]; }
		void setInstanceData(uint32_t id, VertexInstanceTexture& data) { m_InstanceVertex[id] = data; }

		int AddTexture(uint32_t texture);

		void DisableInstance(uint32_t instance);
		void EnableInstance(uint32_t instance);
		void SetEnabled(uint32_t instance, bool enabled);

		void RemoveInstance(uint32_t instance);

		void SetPriority(uint32_t instance, int priority);
	};
}