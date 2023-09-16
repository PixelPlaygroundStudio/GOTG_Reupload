#include <wipch.h>
#include <Wiwa/core/Input.h>
#include "UiImage.h"
#include <Wiwa/utilities/Log.h>
#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/core/Renderer2D.h>

namespace Wiwa
{
	GuiImage::GuiImage(Scene* scene, unsigned int id, Rect2i bounds, const char* path, size_t callbackID, Rect2i boundsOriginTex, bool active, bool animated, float animFrames, std::vector<Rect2i> animationRects, float rotation) : GuiControl(scene, GuiControlType::IMAGE, id)
	{
		this->position = bounds;
		startPos = bounds;
		this->texture = texture;
		texturePosition = boundsOriginTex;
		name = "Image";
		m_Scene = scene;
		this->active = active;
		text = "none";
		audioEventForButton = "none";
		this->callbackID = callbackID;
		/*if (callbackID != WI_INVALID_INDEX)
			callback = Wiwa::Application::Get().getCallbackAt(callbackID);*/

		if (path != "") {
			textId1 = Wiwa::Resources::Load<Wiwa::Image>(path);
			texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(textId1);
		}

		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
		id_quad_normal = r2d.CreateInstancedQuadTex(m_Scene, texture->GetTextureId(), texture->GetSize(), { position.x,position.y }, { position.width,position.height }, texturePosition, Wiwa::Renderer2D::Pivot::CENTER);

		state = GuiControlState::NORMAL;
		canClick = true;

		if (!active)
		{
			r2d.DisableInstance(m_Scene, id_quad_normal);

		}
		this->rotation = rotation;
		framesAnimation = 0;
		animatedControl = animated;
		animSpeed = animFrames;
		positionsForAnimations = animationRects;
	}

	GuiImage::~GuiImage()
	{

	}

	bool GuiImage::Update()
	{
		if (state != GuiControlState::DISABLED)
		{
			// L14: TODO 3_D: Update the state of the GUiButton according to the mouse position
			float mouseX, mouseY;
			mouseX = Wiwa::Input::GetMouseX();
			mouseY = Wiwa::Input::GetMouseY();
			if ((mouseX > position.x && mouseX < (position.x + position.width)) &&
				(mouseY > position.y && mouseY < position.y + position.height))
			{
				state = GuiControlState::FOCUSED;
				if (Wiwa::Input::IsMouseButtonPressed(0))
				{
					state = GuiControlState::PRESSED;
					clicked = true;
				}

				if (Wiwa::Input::IsMouseButtonReleased(0) && clicked)
				{
					state = GuiControlState::PRESSED;
				}

			}
			else
			{
				state = GuiControlState::NORMAL;
			}

			
		}

		return false;
	}

	bool GuiImage::Draw(Renderer2D* render)
	{
		render->UpdateInstancedQuadTexSize(m_Scene, id_quad_normal, { position.x,position.y }, { position.width,position.height }, Wiwa::Renderer2D::Pivot::CENTER);
		render->UpdateInstancedQuadTexPosition(m_Scene, id_quad_normal, { position.x,position.y }, Wiwa::Renderer2D::Pivot::CENTER);
		//render->UpdateInstancedQuadTexRotation(m_Scene, id_quad_normal, rotation);

		HandleAnim(render);
		return false;
	}
}