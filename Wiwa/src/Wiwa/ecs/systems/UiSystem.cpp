#include <wipch.h>
//#include "UiSystem.h"
//
//
//namespace Wiwa
//{
//	UiSystem::UiSystem()
//	{
//
//	}
//
//	UiSystem::~UiSystem()
//	{
//
//	}
//
//	void UiSystem::OnAwake()
//	{
//		
//	}
//
//	void UiSystem::OnInit()
//	{
//		
//	}
//
//	void UiSystem::OnUpdate()
//	{
//		Button* button = GetComponent<Button>();
//		Checkbox* checkBox = GetComponent<Checkbox>();
//		Slider* slider = GetComponent<Slider>();
//
//		if (button != nullptr)
//		{
//			UpdateButton(button);
//		}
//		if (checkBox != nullptr)
//		{
//			UpdateCheckbox(checkBox);
//		}
//		if (slider != nullptr)
//		{
//			UpdateSlider(slider);
//		}
//	}
//
//	void UiSystem::OnDestroy()
//	{
//		
//	}
//
//	void UiSystem::UpdateButton(Button* button)
//	{
//		//Handeling button states
//		Vector2f mousePosition = { Wiwa::Input::GetMouseX() ,Wiwa::Input::GetMouseY()};
//
//		if (mousePosition.x > button->elementImageSection.x && mousePosition.x < (button->elementImageSection.x + button->elementImageSection.width)
//			&& mousePosition.y > button->elementImageSection.y && mousePosition.y < (button->elementImageSection.y + button->elementImageSection.height))
//		{
//			button->state = UiControlState::HOVER;
//
//			if (Wiwa::Input::IsMouseButtonPressed(1) && button->canClick)
//			{
//				button->state = UiControlState::PRESSED;
//			}
//		}
//		else
//		{
//			button->state = UiControlState::NORMAL;
//		}
//
//		RenderButton(button);
//	}
//
//	void UiSystem::RenderButton(Button* button)
//	{
//		switch (button->state)
//		{
//		case UiControlState::NORMAL:
//			button->elementImageid = Wiwa::Resources::Load<Wiwa::Image>(button->element_texture_path);
//			break;
//		case UiControlState::HOVER:
//			button->elementImageid = Wiwa::Resources::Load<Wiwa::Image>(button->element_texture_path_hovered);
//			break;
//		case UiControlState::PRESSED:
//			button->elementImageid = Wiwa::Resources::Load<Wiwa::Image>(button->element_texture_path_pressed);
//			break;
//		default:
//			break;
//		}
//
//		Wiwa::Image* img = Wiwa::Resources::GetResourceById<Wiwa::Image>(button->elementImageid);
//		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
//		Wiwa::Vector2i pos = { button->elementImageSection.x, button->elementImageSection.y };
//		Wiwa::Size2i size = { button->elementImageSection.width, button->elementImageSection.height };
//		r2d.CreateInstancedQuadTex(img->GetTextureId(), img->GetSize(), pos, size, Wiwa::Renderer2D::Pivot::CENTER);
//	}
//	void UiSystem::UpdateCheckbox(Checkbox* checkbox)
//	{
//		//Handeling Checkbox states
//		Vector2f mousePosition = { Wiwa::Input::GetMouseX() ,Wiwa::Input::GetMouseY() };
//
//		if (mousePosition.x > checkbox->elementImageSection.x && mousePosition.x < (checkbox->elementImageSection.x + checkbox->elementImageSection.width)
//			&& mousePosition.y > checkbox->elementImageSection.y && mousePosition.y < (checkbox->elementImageSection.y + checkbox->elementImageSection.height))
//		{
//			checkbox->state = UiControlState::HOVER;
//
//			if (Wiwa::Input::IsMouseButtonPressed(1) && checkbox->canClick)
//			{
//				checkbox->state = UiControlState::PRESSED;
//				checkbox->state_check = !checkbox->state_check;
//			}
//		}
//		else
//		{
//			checkbox->state = UiControlState::NORMAL;
//		}
//
//		RenderCheckbox(checkbox);
//	}
//
//	void UiSystem::RenderCheckbox(Checkbox* checkbox)
//	{
//		switch (checkbox->state_check)
//		{
//		case false:
//			checkbox->elementImageid = Wiwa::Resources::Load<Wiwa::Image>(checkbox->element_texture_path);
//			break;
//		case true:
//			checkbox->elementImageid = Wiwa::Resources::Load<Wiwa::Image>(checkbox->element_texture_path_checked);
//			break;
//		default:
//			break;
//		}
//
//		Wiwa::Image* img = Wiwa::Resources::GetResourceById<Wiwa::Image>(checkbox->elementImageid);
//		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
//		Wiwa::Vector2i pos = { checkbox->elementImageSection.x, checkbox->elementImageSection.y };
//		Wiwa::Size2i size = { checkbox->elementImageSection.width, checkbox->elementImageSection.height };
//		r2d.CreateInstancedQuadTex(img->GetTextureId(), img->GetSize(), pos, size, Wiwa::Renderer2D::Pivot::CENTER);
//	}
//	void UiSystem::UpdateSlider(Slider* slider)
//	{
//
//	}
//}