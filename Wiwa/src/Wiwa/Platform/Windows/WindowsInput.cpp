#include "wipch.h"
#include "WindowsInput.h"

#include <Wiwa/core/KeyCodes.h>
#include <Wiwa/core/Application.h>

#include <GLFW/glfw3.h>

namespace Wiwa
{

	glm::vec2 dz_scaled_radial(glm::vec2 stick_input, float deadzone) {
		float input_magnitude = glm::length(stick_input);
		if (input_magnitude < deadzone) {
			return glm::vec2(0.0f, 0.0f);
		}

		glm::vec2 input_normalized = stick_input / input_magnitude;
		float mapped_magnitude = glm::mix(0.0f, 1.0f, (input_magnitude - deadzone) / (1.0f - deadzone));
		glm::vec2 result = input_normalized * mapped_magnitude;
		return result;

	}
	glm::vec2 dz_sloped_scaled_axial(glm::vec2 stick_input, float deadzone) {
		float deadzone_x = deadzone * glm::abs(stick_input.y);
		float deadzone_y = deadzone * glm::abs(stick_input.x);
		glm::vec2 result(0.0f, 0.0f);
		glm::vec2 sign(glm::sign(stick_input.x), glm::sign(stick_input.y));
		if (glm::abs(stick_input.x) > deadzone_x) {
			result.x = sign.x * glm::mix(0.0f, 1.0f, (glm::abs(stick_input.x) - deadzone_x) / (1.0f - deadzone_x));
		}
		if (glm::abs(stick_input.y) > deadzone_y) {
			result.y = sign.y * glm::mix(0.0f, 1.0f, (glm::abs(stick_input.y) - deadzone_y) / (1.0f - deadzone_y));
		}
		return result;
	}
	glm::vec2 WindowsInput::GetRawJoystickImpl(int gamepadIndx, int axisX, int axisY, float deadzone)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(gamepadIndx, &state))
		{
			float inputX = GetRawAxis(gamepadIndx, axisX, GetDeadzone(axisX));
			float inputY = GetRawAxis(gamepadIndx, axisY, GetDeadzone(axisY));
			glm::vec2 partialOutput = dz_scaled_radial({ inputX, inputY }, deadzone);

			return dz_sloped_scaled_axial(partialOutput, deadzone);
		}
		return glm::vec2(0.f);
	}

	Input* Input::s_Instance = new WindowsInput();
	float Input::MouseXDelta = 0;
	float Input::MouseYDelta = 0;
	float Input::MouseX = 0;
	float Input::MouseY = 0;
	float Input::prevMouseX = 0;
	float Input::prevMouseY = 0;

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS;
	}
	bool WindowsInput::IsKeyRepeatImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS;
	}
	bool WindowsInput::IsKeyReleasedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_RELEASE;
	}
	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	bool WindowsInput::IsMouseButtonReleasedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_RELEASE;
	}
	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		return {MouseX, MouseY};
	}
	float WindowsInput::GetMouseXImpl()
	{
		return MouseX;
	}
	float WindowsInput::GetMouseYImpl()
	{
		return MouseY;
	}

	float WindowsInput::GetMouseXDeltaImpl()
	{
		return MouseXDelta;
	}

	float WindowsInput::GetMouseYDeltaImpl()
	{
		return MouseYDelta;
	}

	void WindowsInput::UpdateImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		MouseX = (float)xpos;
		MouseY = (float)ypos;

		MouseXDelta = MouseX - prevMouseX;
		MouseYDelta = MouseY - prevMouseY;
		CLAMP(MouseXDelta, -1.0f, 1.0f);
		CLAMP(MouseYDelta, -1.0f, 1.0f);

		prevMouseX = MouseX;
		prevMouseY = MouseY;
	}

	void WindowsInput::LockCursorImpl()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void WindowsInput::UnlockCursorImpl()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	float WindowsInput::GetAxisImpl(int gamepadIndx, int axis, float deadzone)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(gamepadIndx, &state))
		{
			float input = state.axes[axis];
			float abs = glm::abs(input);
			
			if (abs < deadzone)
			{
				return 0.f;
			}
			return std::ceil(input);
		}
		return 0.f;
	}
	float WindowsInput::GetRawAxisImpl(int gamepadIndx, int axis, float deadzone)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(gamepadIndx, &state))
		{
			float input = state.axes[axis];
			float abs = glm::abs(input);

			if (abs < deadzone + 0.05f)
			{
				return 0.f;
			}
			return input;

		}
		return 0.f;
	}
	

	glm::vec2 WindowsInput::GetJoystickImpl(int gamepadIndx, int axisX, int axisY, float deadzone)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(gamepadIndx, &state))
		{
			float inputX = GetRawAxis(gamepadIndx, axisX, GetDeadzone(axisX));;
			float inputY = GetRawAxis(gamepadIndx, axisY, GetDeadzone(axisY));;

			glm::vec2 partialOutput = dz_scaled_radial({ inputX, inputY }, deadzone);

			glm::vec2 finalOutput = dz_sloped_scaled_axial(partialOutput, deadzone);
			return glm::vec2(std::ceil(finalOutput.x), std::ceil(finalOutput.y));
		}
		return glm::vec2(0.f);
	}

	bool WindowsInput::IsButtonPressedImpl(int gamepadIndx, int keycode)
	{
		GLFWgamepadstate state;
		if(glfwGetGamepadState(gamepadIndx, &state))
		{
			if (state.buttons[keycode] == GLFW_PRESS)
			{
				return true;
			}
		}
		return false;
	
	}
	bool WindowsInput::IsButtonReleasedImpl(int gamepadIndx, int keycode)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(gamepadIndx, &state))
		{
			if (state.buttons[keycode] == GLFW_RELEASE)
			{
				return true;
			}
		}
		return false;

	}
	void WindowsInput::InitImpl()
	{
		InitDeadzone();
	}
	void WindowsInput::InitDeadzone()
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(Gamepad::GamePad1, &state))
		{
			m_LeftDeadzone.x = glm::abs(state.axes[Gamepad::LeftX]);
			m_LeftDeadzone.y = glm::abs(state.axes[Gamepad::LeftY]);

			m_RightDeadzone.x = glm::abs(state.axes[Gamepad::RightX]);
			m_RightDeadzone.y = glm::abs(state.axes[Gamepad::RightY]);

			return;
		}
		m_LeftDeadzone = glm::vec2(0.f);
		m_RightDeadzone = glm::vec2(0.f);
	}
	float WindowsInput::GetDeadzone(int axis)
	{
		switch (axis)
		{
		case Gamepad::LeftX:
			return m_LeftDeadzone.x;
			break;
		case Gamepad::LeftY:
			return m_LeftDeadzone.y;
			break;
		case Gamepad::RightX:
			return m_RightDeadzone.x;
			break;
		case Gamepad::RightY:
			return m_RightDeadzone.y;
			break;
		default:
			break;
		}


		return 0.0f;
	}
}