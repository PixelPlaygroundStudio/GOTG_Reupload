//#pragma once
//
//#include <Wiwa/core/Core.h>
//#include <Wiwa/utilities/Reflection.h>
//#include <Wiwa/core/Renderer2D.h>
//#include <Wiwa/ecs/systems/UiSystem.h>
//
//namespace Wiwa
//{
//	struct WI_API Checkbox
//	{
//		bool canClick;
//
//		ResourceId elementImageid;
//		Rect2i elementImageSection; //Aqui ya tenemos posicion y tamaño
//		UiControlState state;
//		char element_texture_path[128];
//		char element_texture_path_checked[128];
//
//		bool state_check = false;
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
//REFLECTION_BEGIN(Wiwa::Checkbox)
//	REFLECT_MEMBER(canClick)
//	REFLECT_MEMBER(elementImageid)
//	REFLECT_MEMBER(elementImageSection)
//	REFLECT_MEMBER(element_texture_path)
//	REFLECT_MEMBER(element_texture_path_checked)
//	REFLECT_MEMBER(state_check)
//	REFLECT_MEMBER(state)
//REFLECTION_END;