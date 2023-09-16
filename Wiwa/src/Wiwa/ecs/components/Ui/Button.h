//#pragma once
//#include <Wiwa/utilities/>
//#include <Wiwa/core/Core.h>
//#include <Wiwa/utilities/Reflection.h>
//#include <Wiwa/core/Renderer2D.h>
//#include <Wiwa/ecs/systems/UiSystem.h>
//
//namespace Wiwa
//{
//	struct WI_API Button
//	{
//		bool canClick;
//
//		ResourceId elementImageid;
//		Rect2i elementImageSection; //Aqui ya tenemos posicion y tamaño
//		UiControlState state;
//		char element_texture_path[128];
//		char element_texture_path_hovered[128];
//		char element_texture_path_pressed[128];
//
//	};
//
//	enum UiControlState
//	{
//		DISABLED,
//		NORMAL,
//		HOVER,
//		PRESSED,
//	};
//}
//
//
//REFLECTION_BEGIN(Wiwa::Button)
//	REFLECT_MEMBER(canClick)
//	REFLECT_MEMBER(elementImageid)
//	REFLECT_MEMBER(elementImageSection)
//	REFLECT_MEMBER(element_texture_path)
//	REFLECT_MEMBER(element_texture_path_hovered)
//	REFLECT_MEMBER(element_texture_path_pressed)
//	REFLECT_MEMBER(state)
//REFLECTION_END;