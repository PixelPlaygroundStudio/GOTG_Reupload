#include <wipch.h>
#include <Wiwa/core/Input.h>
#include "UiCanvas.h"
#include "Wiwa/ui/UiManager.h"
#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/audio/Audio.h>
namespace Wiwa
{
	GuiCanvas::GuiCanvas(Scene* scene, unsigned int id,bool active)
	{
		this->id = id;
		this->active = active;
		selected = false;
	}

	GuiCanvas::~GuiCanvas()
	{

	}

	bool GuiCanvas::Update()
	{
		InputController();

		return true;
	}

	void GuiCanvas::SwapActive()
	{
		active = !active;
		for (int i = 0; i < controls.size(); i++)
		{
			controls.at(i)->SwapActive(Wiwa::Application::Get().GetRenderer2D());
		}
	}

	void GuiCanvas::SelectElement(int id)
	{
		for (int i = 0; i < controlsForSelection.size(); i++)
		{
			if (i == id)
			{
				controlsForSelection.at(i)->state = Wiwa::GuiControlState::FOCUSED;
			}
			else
			{
				controlsForSelection.at(i)->state = Wiwa::GuiControlState::NORMAL;
			}
		}
	
	}

	void GuiCanvas::InputController()
	{
		timer += 0.16f;

		if (timer >= MaxTimeBetweenChanges)
		{
			if (Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).y > 0.5f)
			{
				DpadUp = true;
				this->updateSelected = true;

			}
			else if (Wiwa::Input::GetRawJoystick(Wiwa::Gamepad::GamePad1, Wiwa::Gamepad::LeftX, Wiwa::Gamepad::LeftY, Wiwa::GameStateManager::s_GamepadDeadzone).y < -0.5f)
			{
				DpadDown = true;
				this->updateSelected = true;

			}
			else
			{
				DpadDown = false;
				DpadUp = false;
			}

			if (Wiwa::Input::IsKeyPressed(Wiwa::Key::Up))
			{
				DpadUp = true;
				this->updateSelected = true;

			}
			if (DpadUp)
			{
				DpadUp = false;
				idGuiSelected++;
				if (idGuiSelected >= controlsForSelection.size())
				{
					idGuiSelected = 0;
				}
				if (Audio::FindEvent("pause_sound") != Audio::INVALID_ID)
				{
					Audio::PostEvent("pause_sound");
				}
			}
			if (Wiwa::Input::IsKeyPressed(Wiwa::Key::Down))
			{
				DpadDown = true;
				this->updateSelected = true;

			}
			if (DpadDown)
			{
				DpadDown = false;
				idGuiSelected--;
				if (idGuiSelected <= -1)
				{
					idGuiSelected = controlsForSelection.size() - 1;
				}
				if (Audio::FindEvent("pause_sound") != Audio::INVALID_ID)
				{
					Audio::PostEvent("pause_sound");
				}
			}
			if (idGuiSelected > -1 && idGuiSelected < controlsForSelection.size())
			{
				SelectElement(idGuiSelected);
			}
			timer = 0.0f;
		}
		else if (timer < MaxTimeBetweenChanges)
		{
			this->updateSelected = false;
		}
	}
}