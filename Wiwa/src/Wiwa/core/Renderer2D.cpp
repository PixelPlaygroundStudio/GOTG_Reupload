#include <wipch.h>
#include "Renderer2D.h"

#include <Wiwa/core/Application.h>

#include <Wiwa/utilities/render/InstanceRenderer.h>
#include <Wiwa/utilities/Log.h>

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Wiwa/render/RenderManager.h>
#include <Wiwa/scene/SceneManager.h>

namespace Wiwa
{
	uint32_t Renderer2D::getInstanceRenderer(Scene* scene, uint32_t textureId)
	{
		std::vector<InstanceRenderer*>& instance_list = scene->GetInstanceRenderers();
		size_t size = instance_list.size();

		int id = -1;

		for (size_t i = 0; i < size; i++) {
			int addtex = instance_list[i]->AddTexture(textureId);

			if (addtex != -1) {
				id = i;
				break;
			}
		}

		if (id == -1) {
			id = scene->CreateInstanceRenderer();
		}

		return id;
	}

	Renderer2D::Renderer2D()
	{
		// IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	}

	Renderer2D::~Renderer2D()
	{
	}

	Vector2f Renderer2D::CalculateScreenGlPos(const Rect2i &rect, Pivot pivot)
	{
		Vector2f pivot_position = {static_cast<float>(rect.x), static_cast<float>(rect.y)};

		switch (pivot)
		{
		case Pivot::DOWNLEFT:
			pivot_position.x += rect.width / 2;
			pivot_position.y -= rect.height / 2;
			break;
		case Pivot::UPLEFT:
			pivot_position.x += rect.width / 2;
			pivot_position.y += rect.height / 2;
			break;
		}

		return pivot_position;
	}

	Vector2f Renderer2D::CalculateScreenGlPos(const Vector2i &position, const Size2i &size, Pivot pivot)
	{
		Vector2f pivot_position = {static_cast<float>(position.x), static_cast<float>(position.y)};

		switch (pivot)
		{
		case Pivot::DOWNLEFT:
			pivot_position.x += size.w / 2;
			pivot_position.y -= size.h / 2;
			break;
		case Pivot::UPLEFT:
			pivot_position.x += size.w / 2;
			pivot_position.y += size.h / 2;
			break;
		}

		return pivot_position;
	}

	TextureClip Renderer2D::CalculateTextureClip(const Rect2i &rect, const Size2i &texSize)
	{
		TextureClip tclip;

		Vector2f pos = {
			(float)rect.x / texSize.w,
			(float)rect.y / texSize.h};

		Size2f size = {
			(float)rect.width / texSize.w,
			(float)rect.height / texSize.h};

		tclip.topLeft = pos;

		tclip.topRight.x = pos.x + size.w;
		tclip.topRight.y = pos.y;

		tclip.bottomLeft.x = pos.x;
		tclip.bottomLeft.y = pos.y + size.h;

		tclip.bottomRight.x = pos.x + size.w;
		tclip.bottomRight.y = pos.y + size.h;

		return tclip;
	}

	void Renderer2D::BindCamera()
	{
		RenderManager::SetLayerCamera(1, &m_ActiveCamera);
	}

	bool Renderer2D::Init()
	{
		Window &window = Application::Get().GetWindow();
		Size2i &resolution = Application::Get().GetTargetResolution();

		// Set camera
		m_ActiveCamera.SetOrthographic(resolution.w, resolution.h);
		m_ActiveCamera.setPosition({0.0f, 0.0f, 3.0f});

		BindCamera();

		WI_CORE_INFO("Renderer2D initialized");
		return true;
	}

	void Renderer2D::Update()
	{
		OPTICK_EVENT("Renderer 2D Update");
		/*m_RenderCallsInstancedCount = 0;*/
		//UpdateInstanced(Wiwa::SceneManager::getActiveScene());
	}

	void Renderer2D::Close()
	{
		// delete instanceRenderer;
	}

	Renderer2D::InstanceData Renderer2D::CreateInstancedQuadTex(Scene *scene, uint32_t textureId, const Size2i &srcSize, const Vector2i &position, const Size2i &size, const Color4f &color, const Rect2i &clip, Pivot pivot, const float rotation)
	{
		uint32_t renderer_id = getInstanceRenderer(scene, textureId);
		InstanceRenderer* instanceRenderer = scene->GetInstanceRenderer(renderer_id);

		TextureClip tclip = CalculateTextureClip(clip, srcSize);

		uint32_t instance_id = instanceRenderer->AddInstance(textureId, position, size, color, tclip, pivot, rotation);

		return { instance_id, renderer_id };
	}

	Renderer2D::InstanceData Renderer2D::CreateInstancedQuadTex(Scene *scene, uint32_t textureId, const Size2i &srcSize, const Vector2i &position, const Size2i &size, Pivot pivot, const float rotation)
	{
		uint32_t renderer_id = getInstanceRenderer(scene, textureId);
		InstanceRenderer*instanceRenderer = scene->GetInstanceRenderer(renderer_id);

		TextureClip clip = {
			{1.0f, 1.0f},
			{1.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 1.0f}
		};

		Color4f color = Color::WHITE;

		uint32_t instance_id = instanceRenderer->AddInstance(textureId, position, size, color, clip, pivot, rotation);

		return { instance_id, renderer_id };
	}

	Renderer2D::InstanceData Renderer2D::CreateInstancedQuadTex(Scene *scene, uint32_t textureId, const Size2i &srcSize, const Vector2i &position, const Size2i &size, const Rect2i &clip, Pivot pivot, const float rotation)
	{
		uint32_t renderer_id = getInstanceRenderer(scene, textureId);
		InstanceRenderer* instanceRenderer = scene->GetInstanceRenderer(renderer_id);

		TextureClip tclip = CalculateTextureClip(clip, srcSize);

		Color4f color = Color::WHITE;
		uint32_t instance_id = instanceRenderer->AddInstance(textureId, position, size, color, tclip, pivot, rotation);

		return { instance_id, renderer_id };
	}

	void Renderer2D::RemoveInstance(Scene *scene, InstanceData instance)
	{
		InstanceRenderer *instanceRenderer = scene->GetInstanceRenderer(instance.renderer_id);
		instanceRenderer->RemoveInstance(instance.instance_id);
	}

	void Renderer2D::EnableInstance(Scene *scene, InstanceData instance)
	{
		InstanceRenderer *instanceRenderer = scene->GetInstanceRenderer(instance.renderer_id);
		instanceRenderer->EnableInstance(instance.instance_id);
	}

	void Renderer2D::DisableInstance(Scene *scene, InstanceData instance)
	{
		InstanceRenderer *instanceRenderer = scene->GetInstanceRenderer(instance.renderer_id);
		instanceRenderer->DisableInstance(instance.instance_id);
	}

	void Renderer2D::SetInstanceEnabled(Scene *scene, InstanceData instance, bool enabled)
	{
		InstanceRenderer *instanceRenderer = scene->GetInstanceRenderer(instance.renderer_id);
		instanceRenderer->SetEnabled(instance.instance_id, enabled);
	}

	void Renderer2D::UpdateInstancedQuadTexPosition(Scene *scene, InstanceData id, const Vector2i &position, Pivot pivot)
	{
		InstanceRenderer *instanceRenderer = scene->GetInstanceRenderer(id.renderer_id);
		instanceRenderer->UpdateInstancePosition(id.instance_id, position, pivot);
	}

	void Renderer2D::UpdateInstancedQuadTexRotation(Scene* scene, InstanceData id, float rotation)
	{
		InstanceRenderer* instanceRenderer = scene->GetInstanceRenderer(id.renderer_id);
		instanceRenderer->UpdateInstanceRotation(id.instance_id, rotation);
	}

	void Renderer2D::UpdateInstancedQuadTexSize(Scene* scene, InstanceData id, const Vector2i& pos, const Size2i& size, Renderer2D::Pivot pivot)
	{
		InstanceRenderer* instanceRenderer = scene->GetInstanceRenderer(id.renderer_id);
		instanceRenderer->UpdateInstanceSize(id.instance_id, pos, size, pivot);
	}

	void Renderer2D::UpdateInstancedQuadTexClip(Scene* scene, InstanceData id, const Size2i& srcSize, const Rect2i& clip)
	{
		TextureClip tclip = CalculateTextureClip(clip, srcSize);

		InstanceRenderer* instanceRenderer = scene->GetInstanceRenderer(id.renderer_id);
		instanceRenderer->UpdateInstanceClip(id.instance_id, tclip);
	}

	void Renderer2D::UpdateInstancedQuadTexTexture(Scene* scene, InstanceData& id, uint32_t tex_id)
	{
		InstanceRenderer* instanceRenderer = scene->GetInstanceRenderer(id.renderer_id);

		int addTexture = instanceRenderer->AddTexture(tex_id);

		if (addTexture == -1) {
			VertexInstanceTexture vtex = instanceRenderer->getInstanceData(id.instance_id);
			instanceRenderer->RemoveInstance(id.instance_id);

			id.renderer_id = getInstanceRenderer(scene, tex_id);
			instanceRenderer = scene->GetInstanceRenderer(id.renderer_id);

			id.instance_id = instanceRenderer->AddInstance(vtex);
		}

		instanceRenderer->UpdateInstanceTexture(id.instance_id, tex_id);
	}

	void Renderer2D::UpdateInstancedQuadTexColor(Scene* scene, InstanceData id, const Color4f& color)
	{
		InstanceRenderer* instanceRenderer = scene->GetInstanceRenderer(id.renderer_id);
		instanceRenderer->UpdateInstanceColor(id.instance_id, color);
	}

	void Renderer2D::UpdateInstancedQuadTexPriority(Scene* scene, InstanceData id, int priority)
	{
		InstanceRenderer* instanceRenderer = scene->GetInstanceRenderer(id.renderer_id);
		instanceRenderer->SetPriority(id.instance_id, priority);
	}

	void Renderer2D::UpdateInstancedQuad(Scene* scene, InstanceData id, const Vector2i &position, const Size2i &size, const Color4f &color)
	{
		InstanceRenderer *instanceRenderer = scene->GetInstanceRenderer(id.renderer_id);
		instanceRenderer->UpdateInstance(id.instance_id, position, size, color);
	}

	void Renderer2D::UpdateInstanced(Scene* scene)
	{
		std::vector<InstanceRenderer*> &instanceRenderers = scene->GetInstanceRenderers();
		size_t instance_size = instanceRenderers.size();

		FrameBuffer &framebuffer = *m_ActiveCamera.frameBuffer;
		GL(Viewport(0, 0, framebuffer.getWidth(), framebuffer.getHeight()));

		framebuffer.Bind();
		GL(Enable(GL_BLEND));
		//glBlendEquation(GL_ADD);
		GL(BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		//glEnable(GL_DEPTH_TEST);
		//glDepthMask(false);
		for (size_t i = 0; i < instance_size; i++) {
			instanceRenderers[i]->Bind();

			instanceRenderers[i]->Update();
			instanceRenderers[i]->Render(m_ActiveCamera.getProjection(), m_ActiveCamera.getView());

			instanceRenderers[i]->Unbind();
		}
		//glDepthMask(true);
		GL(Disable(GL_BLEND));
		//glDisable(GL_DEPTH_TEST);
        
		framebuffer.Unbind();

		m_RenderCallsInstancedCount++;


	}

	void Renderer2D::RenderInstanced(InstanceRenderer& instanceRenderer)
	{
		FrameBuffer& framebuffer = *m_ActiveCamera.frameBuffer;
		GL(Viewport(0, 0, framebuffer.getWidth(), framebuffer.getHeight()));

		framebuffer.Bind();
		GL(Enable(GL_BLEND));
		GL(BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		instanceRenderer.Bind();

		instanceRenderer.Update();
		instanceRenderer.Render(m_ActiveCamera.getProjection(), m_ActiveCamera.getView());

		instanceRenderer.Unbind();

		GL(Disable(GL_BLEND));

		framebuffer.Unbind();
	}
}