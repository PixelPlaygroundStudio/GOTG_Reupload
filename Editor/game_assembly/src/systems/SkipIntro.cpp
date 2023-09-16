#include "SkipIntro.h"

void Wiwa::SkipIntro::OnUpdate()
{
	if (Input::IsKeyPressed(Key::Space) || Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA))
	{
		m_SkipIntro = true;
	}
	if ((Input::IsKeyReleased(Key::Space) || Input::IsButtonReleased(Gamepad::GamePad1, Key::GamepadA)) && m_SkipIntro)
	{
		SceneManager::ChangeSceneByIndex(1);
		m_SkipIntro = false;
	}
}
