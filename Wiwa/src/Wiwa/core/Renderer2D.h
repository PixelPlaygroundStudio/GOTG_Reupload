#pragma once
#pragma warning(disable : 4251)

#include <Wiwa/core/Core.h>

// 3D maths lib
// #include <MathGeoLib.h>
// 2D maths lib
#include <Wiwa/utilities/math/Math.h>

#include <Wiwa/utilities/Log.h>
#include <Wiwa/core/Resources.h>

// #include "utilities/gui/Texture.h"

#include <Wiwa/utilities/render/VertexBuffer.h>

// #include "Empathy/headers/utilities/renderer/Batch.h"
#include <Wiwa/utilities/render/FrameBuffer.h>
#include <Wiwa/utilities/render/Camera.h>

#define MAXQUADS 40500
#define MAXVERTEX MAXQUADS * 4
#define MAXINDICES MAXQUADS * 6

namespace Wiwa
{
	class InstanceRenderer;
	class Scene;

	class WI_API Renderer2D
	{
	public:
		struct InstanceData {
			uint32_t instance_id;
			uint32_t renderer_id;
		};

		enum class Pivot
		{
			UPLEFT,
			DOWNLEFT,
			CENTER
		};

	private:
		// Default FrameBuffer

		uint32_t m_RenderCallsInstancedCount;

		Camera m_ActiveCamera;

		uint32_t getInstanceRenderer(Scene* scene, uint32_t textureId);
	public:
		Renderer2D();
		~Renderer2D();

		static Vector2f CalculateScreenGlPos(const Rect2i &rect, Pivot pivot);
		static Vector2f CalculateScreenGlPos(const Vector2i &position, const Size2i &size, Pivot pivot);
		static TextureClip CalculateTextureClip(const Rect2i &rect, const Size2i &texSize);

		void BindCamera();

		bool Init();
		void Update();

		void Close();

		// Instance rendering functions
		InstanceData CreateInstancedQuadTex(Scene *scene, uint32_t textureId, const Size2i &srcSize, const Vector2i &position, const Size2i &size, const Color4f &color, const Rect2i &clip, Pivot pivot = Pivot::CENTER, const float rotation = 0.f);
		InstanceData CreateInstancedQuadTex(Scene *scene, uint32_t textureId, const Size2i &srcSize, const Vector2i &position, const Size2i &size, Pivot pivot = Pivot::CENTER, const float rotation = 0.f);
		InstanceData CreateInstancedQuadTex(Scene *scene, uint32_t textureId, const Size2i &srcSize, const Vector2i &position, const Size2i &size, const Rect2i &clip, Pivot pivot = Pivot::CENTER, const float rotation = 0.f);

		void RemoveInstance(Scene *scene, InstanceData instance);
		void EnableInstance(Scene *scene, InstanceData instance);
		void DisableInstance(Scene *scene, InstanceData instance);
		void SetInstanceEnabled(Scene *scene, InstanceData instance, bool enabled);

		void UpdateInstancedQuadTexPosition(Scene *scene, InstanceData id, const Vector2i &position, Pivot pivot = Pivot::CENTER);
		void UpdateInstancedQuadTexRotation(Scene* scene, InstanceData id, float rotation);
		void UpdateInstancedQuadTexSize(Scene* scene, InstanceData id, const Vector2i& pos, const Size2i& size, Renderer2D::Pivot pivot = Renderer2D::Pivot::CENTER);
		void UpdateInstancedQuadTexClip(Scene* scene, InstanceData id, const Size2i& srcSize, const Rect2i& clip);
		void UpdateInstancedQuadTexTexture(Scene* scene, InstanceData& id, uint32_t tex_id);
		void UpdateInstancedQuadTexColor(Scene* scene, InstanceData id, const Color4f& color);
		void UpdateInstancedQuadTexPriority(Scene* scene, InstanceData id, int priority);

		void UpdateInstancedQuad(Scene* scene, InstanceData id, const Vector2i &position, const Size2i &size, const Color4f &color);

		void UpdateInstanced(Scene *scene);
		void RenderInstanced(InstanceRenderer& instanceRenderer);

		// Getters
		uint32_t getRenderInstancedCalls() { return m_RenderCallsInstancedCount; }
		uint32_t getRenderInstanceCount() { return MAXQUADS; }
		uint32_t getColorBufferTexture() { return m_ActiveCamera.frameBuffer->getColorBufferTexture(); }

		glm::mat4 GetOrthoProjection() { return m_ActiveCamera.getProjection(); }
		glm::mat4 GetView() { return m_ActiveCamera.getView(); }
	};
}