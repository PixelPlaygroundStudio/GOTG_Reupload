#include <wipch.h>
#include <Wiwa/core/Input.h>
#include "UiBar.h"
#include "Wiwa/ui/UiManager.h"
#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/core/Renderer2D.h>
namespace Wiwa
{
	GuiBar::GuiBar(Scene* scene, unsigned int id, Rect2i bounds, Rect2i sliderBounds, const char* path, const char* slider_path, Rect2i boundsOriginTex, Rect2i sliderOriginTex, bool active) : GuiControl(scene, GuiControlType::BAR, id)
	{
		this->position = bounds;
		this->extraPosition = this->position;
		texturePosition = boundsOriginTex;
		extraTexturePosition = texturePosition;
		name = "UI_bar";
		m_Scene = scene;
		this->active = active;
		text = "none";
		callbackID = WI_INVALID_INDEX;
		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
		textId1 = Wiwa::Resources::Load<Wiwa::Image>(path);
		texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(textId1);

		id_quad_normal = r2d.CreateInstancedQuadTex(m_Scene, texture->GetTextureId(), texture->GetSize(), { position.x,position.y }, { position.width,position.height }, texturePosition, Wiwa::Renderer2D::Pivot::UPLEFT);

		state = GuiControlState::NORMAL;

		if (!active)
		{
			//r2d.DisableInstance(m_Scene, id_quad_normal);
			r2d.DisableInstance(m_Scene, id_quad_normal);

		}
	}

	GuiBar::~GuiBar()
	{

	}


	bool GuiBar::Draw(Renderer2D* render)
	{
		
		render->UpdateInstancedQuadTexPosition(m_Scene, id_quad_normal, { position.x,position.y }, Wiwa::Renderer2D::Pivot::UPLEFT);
		render->UpdateInstancedQuadTexClip(m_Scene, id_quad_normal, texture->GetSize(),texturePosition);
		render->UpdateInstancedQuadTexSize(m_Scene, id_quad_normal, { position.x,position.y }, { position.width,position.height }, Wiwa::Renderer2D::Pivot::UPLEFT);
		return false;
	}
}