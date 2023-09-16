#include <wipch.h>
#include <Wiwa/core/Input.h>
#include "UiButton.h"
#include <Wiwa/utilities/Log.h>
#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/audio/Audio.h>

namespace Wiwa
{
	GuiButton::GuiButton(Scene* scene, unsigned int id, Rect2i bounds,const char* path, const char* extraPath, size_t callbackID, Rect2i boundsOriginTex, const char* audioEventName, bool active,bool animated, float animFrames, std::vector<Rect2i> animationRects, float rotation) : GuiControl(scene, GuiControlType::BUTTON, id)
	{
		this->position = bounds;
		startPos = bounds;
		texturePosition = boundsOriginTex;
		name = "Button";
		m_Scene = scene;
		this->active = active;
		text = "none";
		audioEventForButton = audioEventName;
		
		if (path != "") {
			textId1 = Wiwa::Resources::Load<Wiwa::Image>(path);
			texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(textId1);
		}
		//textId2 = Wiwa::Resources::Load<Wiwa::Image>(extraPath);
		//extraTexture = Wiwa::Resources::GetResourceById<Wiwa::Image>(textId2);
		this->callbackID = callbackID;

		if(callbackID != WI_INVALID_INDEX)
			callback = Wiwa::Application::Get().getCallbackAt(callbackID);

		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
		
		id_quad_normal = r2d.CreateInstancedQuadTex(m_Scene, texture->GetTextureId(), texture->GetSize(), { position.x,position.y }, { position.width,position.height }, texturePosition, Wiwa::Renderer2D::Pivot::CENTER);
		if (!active)
		{
			r2d.DisableInstance(m_Scene, id_quad_normal);

		}
		this->rotation = rotation;
		animatedControl = animated;

		positionsForAnimations = animationRects;
		
		state = GuiControlState::NORMAL;
		canClick = true;



	}

	GuiButton::~GuiButton()
	{

	}

	bool GuiButton::Update()
	{
		Wiwa::Renderer2D& r2d_1 = Wiwa::Application::Get().GetRenderer2D();

		if (state != GuiControlState::DISABLED)
		{
			
			float mouseX, mouseY;
			mouseX = Wiwa::Input::GetMouseX();
			mouseY = Wiwa::Input::GetMouseY();
			if ((mouseX > position.x && mouseX < (position.x + position.width)) &&
				(mouseY > position.y && mouseY < position.y + position.height))
			{
				
			}


			if (state == GuiControlState::NORMAL)
			{
				SetNextFrame(0, &r2d_1);

				position = startPos;
			}

			//state = GuiControlState::FOCUSED;

		}
		if (state == GuiControlState::FOCUSED)
		{
			if (!Focused)
			{
				//play on focused sound 
				if (Audio::FindEvent(audioEventFocused.c_str()) != Audio::INVALID_ID)
				{
					Audio::PostEvent(audioEventFocused.c_str());
				}
				Focused = !Focused;
			}

			if (Wiwa::Input::IsKeyPressed(Wiwa::Key::Space))
			{
				state = GuiControlState::PRESSED;
				clicked = true;
			}
			if (Wiwa::Input::IsKeyReleased(Wiwa::Key::Space) && clicked)
			{
				clicked = false;
				if (Audio::FindEvent(audioEventForButton.c_str()) != Audio::INVALID_ID)
				{
					Audio::PostEvent(audioEventForButton.c_str());
				}
				if (callback)
				{
					Action<>function_name = callback->func;
					function_name.execute();
				}
			}

			SetNextFrame(1, &r2d_1);

			if (Wiwa::Input::IsButtonPressed(0, 0))
			{
				state = GuiControlState::PRESSED;
				SetNextFrame(2, &r2d_1);
				pressedGamepad = true;
			}
			if (pressedGamepad && Wiwa::Input::IsButtonReleased(0, 0))
			{
				
				pressedGamepad = false;
				if (Audio::FindEvent(audioEventForButton.c_str()) != Audio::INVALID_ID)
				{
					Audio::PostEvent(audioEventForButton.c_str());
				}
				if (callback)
				{
					Action<>function_name = callback->func;
					function_name.execute();
				}
			}
			currentAnimTime += Time::GetDeltaTime();


		}
		else {
			Focused = false;
			currentAnimTime = 0;
		}
		return false;
	}

	bool GuiButton::Draw(Renderer2D* render)
	{
		
		render->UpdateInstancedQuadTexSize(m_Scene, id_quad_normal, { position.x,position.y }, { position.width,position.height }, Wiwa::Renderer2D::Pivot::CENTER);
		render->UpdateInstancedQuadTexPosition(m_Scene, id_quad_normal, { position.x,position.y }, Wiwa::Renderer2D::Pivot::CENTER);
		render->UpdateInstancedQuadTexRotation(m_Scene, id_quad_normal, rotation);	
		//HandleAnim(render);
		return false;
	}
}