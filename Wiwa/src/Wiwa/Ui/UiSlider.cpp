#include <wipch.h>
#include <Wiwa/core/Input.h>
#include "UiSlider.h"

#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/audio/Audio.h>

namespace Wiwa
{
	GuiSlider::GuiSlider(Scene* scene, unsigned int id, Rect2i bounds, Rect2i sliderBounds, const char* path, const char* slider_path, size_t callbackID, Rect2i boundsOriginTex, Rect2i sliderOriginTex, const char* audioEventName, bool active) : GuiControl(scene, GuiControlType::SLIDER, id)
	{
		this->position = bounds;
		startPos = bounds;
		this->extraPosition = sliderBounds;
		texturePosition = boundsOriginTex;
		extraTexturePosition = sliderOriginTex;
		name = "Slider";
		m_Scene = scene;
		this->active = active;
		text = "none";
		audioEventForButton = audioEventName;
		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
		this->callbackID = callbackID;
		if (callbackID != WI_INVALID_INDEX)
			callback = Wiwa::Application::Get().getCallbackAt(callbackID);
		textId2 = Wiwa::Resources::Load<Wiwa::Image>(slider_path);
		extraTexture = Wiwa::Resources::GetResourceById<Wiwa::Image>(textId2);


		textId1 = Wiwa::Resources::Load<Wiwa::Image>(path);
		texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(textId1);
		id_quad_normal = r2d.CreateInstancedQuadTex(m_Scene, texture->GetTextureId(), texture->GetSize(), { position.x,position.y }, { position.width,position.height }, texturePosition, Wiwa::Renderer2D::Pivot::UPLEFT);
		id_quad_extra = r2d.CreateInstancedQuadTex(m_Scene, extraTexture->GetTextureId(), extraTexture->GetSize(), { extraPosition.x,extraPosition.y }, { extraPosition.width,extraPosition.height }, extraTexturePosition, Wiwa::Renderer2D::Pivot::UPLEFT);
		
		if (!active)
		{
			r2d.DisableInstance(m_Scene, id_quad_normal);
			r2d.DisableInstance(m_Scene, id_quad_extra);

		}
		state = GuiControlState::NORMAL;
	}

	GuiSlider::~GuiSlider()
	{

	}

	bool GuiSlider::Update()
	{
		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
		if (state != GuiControlState::DISABLED)
		{
			if (canClick == true)
			{
				float mouseX, mouseY;
				mouseX = Wiwa::Input::GetMouseX();
				mouseY = Wiwa::Input::GetMouseY();

				if ((mouseX > position.x && mouseX < (position.x + position.width)) &&
					(mouseY > position.y && mouseY < position.y + position.height))
				{
					state = GuiControlState::FOCUSED;
				}


				if (state == GuiControlState::NORMAL)
				{
					position = startPos;
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
					/*if (Wiwa::Input::IsMouseButtonPressed(0))
					{
						state = GuiControlState::PRESSED;
						extraPosition.width = (int)mouseX - extraPosition.x;
						clicked = true;
					}

					if (Wiwa::Input::IsMouseButtonReleased(0) && clicked)
					{
						clicked = false;
						value = (((float)extraPosition.width / (float)position.width) * 100);
						void* params[] = { &value };
						callback->Execute(params);
					}*/
					if (Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).x > 0.5f || Wiwa::Input::IsKeyPressed(Wiwa::Key::Right))
					{
						clickedRight = true;
						extraPosition.width += 1;
						if (extraPosition.width >= position.width)
						{
							extraTexturePosition.width = extraPosition.width;
							extraPosition.width = position.width;

						}
					}
					else if (Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).x < -0.5f || Wiwa::Input::IsKeyPressed(Wiwa::Key::Left))
					{
						clickedLeft = true;
						extraPosition.width -= 1;
						if (extraPosition.width <= 0)
						{
							extraTexturePosition.width = extraPosition.width;
							extraPosition.width = 0;
						}
					}
					else
					{
						clickedLeft = false;
						clickedRight = false;
					}
					if (clickedLeft || clickedRight)
					{
						value = (((float)extraPosition.width / (float)position.width) * 100);
						float params = value;
						WI_INFO(value);
						if (callback)
						{
							Action<float>function_name = callback->func;
							function_name.execute(params);
						}
					}
					
				}
			}
		}

		return false;
	}

	bool GuiSlider::Draw(Renderer2D* render)
	{
		extraPosition.x = position.x;
		extraPosition.y = position.y;
		extraTexturePosition.width = extraPosition.width;

		
		render->UpdateInstancedQuadTexPosition(m_Scene, id_quad_normal, { position.x,position.y }, Wiwa::Renderer2D::Pivot::UPLEFT);
		render->UpdateInstancedQuadTexSize(m_Scene, id_quad_extra, { extraPosition.x,extraPosition.y }, { extraPosition.width,extraPosition.height }, Wiwa::Renderer2D::Pivot::UPLEFT);
		render->UpdateInstancedQuadTexClip(m_Scene, id_quad_extra, extraTexture->GetSize(), extraTexturePosition);

		if (state != GuiControlState::FOCUSED)
		{
			render->UpdateInstancedQuadTexColor(m_Scene, id_quad_normal, { 1.0,1.0,1.0,1.0 });
			render->UpdateInstancedQuadTexColor(m_Scene, id_quad_extra, { 1.0,1.0,1.0,1.0 });
		}
		else if (state == GuiControlState::FOCUSED)
		{
			render->UpdateInstancedQuadTexColor(m_Scene, id_quad_normal, { 0.5,0.5,0.5,1.0 });
			render->UpdateInstancedQuadTexColor(m_Scene, id_quad_extra, { 0.5,0.5,0.5,1.0 });
		}
			

		return false;
	}
}