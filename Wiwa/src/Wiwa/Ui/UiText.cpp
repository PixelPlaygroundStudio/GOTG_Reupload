#include <wipch.h>
#include <Wiwa/core/Input.h>
#include "UiText.h"
#include "Wiwa/ui/UiManager.h"
#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/core/Renderer2D.h>
namespace Wiwa
{
	GuiText::GuiText(Scene* scene, unsigned int id, Rect2i bounds, const char* string_text, bool active, float rotation) : GuiControl(scene, GuiControlType::TEXT, id)
	{
		this->position = bounds;
		startPos = bounds;
		name = "Text";
		text = string_text;
		this->active = active;
		audioEventForButton = "none";
		callbackID = WI_INVALID_INDEX;
		texturePosition.x = 0;
		texturePosition.y = 0;
		texturePosition.width = 512;
		texturePosition.height = 512;
		Wiwa::GuiManager& gm = Wiwa::SceneManager::getActiveScene()->GetGuiManager();
		text_res = gm.InitFont("library/Fonts/Jade_Smile.ttf", (char*)string_text);
		m_Scene = scene;
		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
		id_quad_normal = r2d.CreateInstancedQuadTex(m_Scene, text_res->GetTextureId(), text_res->GetSize(), { position.x,position.y }, { position.width,position.height }, Wiwa::Renderer2D::Pivot::CENTER);
		
		if (!active)
		{
			r2d.DisableInstance(m_Scene, id_quad_normal);

		}

		this->rotation = rotation;
	}

	GuiText::~GuiText()
	{
		delete text_res;
	}


	bool GuiText::Draw(Renderer2D* render)
	{
		Color4f color = { 0.0f,0.0f,0.0f,1.0f };
		
		render->UpdateInstancedQuadTexPriority(m_Scene, id_quad_normal, 1);
		render->UpdateInstancedQuadTexSize(m_Scene, id_quad_normal, { position.x,position.y }, { position.width,position.height }, Wiwa::Renderer2D::Pivot::CENTER);
		render->UpdateInstancedQuadTexPosition(m_Scene, id_quad_normal, { position.x,position.y }, Wiwa::Renderer2D::Pivot::CENTER);
		render->UpdateInstancedQuadTexRotation(m_Scene, id_quad_normal, rotation);
		render->UpdateInstancedQuadTexColor(m_Scene, id_quad_normal, color);
	

		return false;
	}
}