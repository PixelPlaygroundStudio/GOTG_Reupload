//#pragma once
//
//#include <Wiwa/core/Core.h>
//#include <Wiwa/utilities/Reflection.h>
//#include <Wiwa/core/Renderer2D.h>
//#include <Wiwa/ecs/systems/UiSystem.h>
//namespace Wiwa
//{
//	struct WI_API Slider
//	{
//		bool canClick;
//		ResourceId elementImageid;
//		Rect2i elementImageSection; //Aqui ya tenemos posicion y tamaño
//		char element_texture_path[128];
//
//		ResourceId movingElementImageid;
//		Rect2i movingElementImageSection;//Aqui ya tenemos posicion y tamaño
//		char moving_element_texture_path[128];
//
//		UiControlState state;
//
//		int SliderLevel;
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
//REFLECTION_BEGIN(Wiwa::Slider)
//	REFLECT_MEMBER(canClick)
//	REFLECT_MEMBER(elementImageid)
//	REFLECT_MEMBER(elementImageSection)
//	REFLECT_MEMBER(element_texture_path)
//	REFLECT_MEMBER(movingElementImageid)
//	REFLECT_MEMBER(movingElementImageSection)
//	REFLECT_MEMBER(moving_element_texture_path)
//	REFLECT_MEMBER(SliderLevel)
//	REFLECT_MEMBER(state)
//REFLECTION_END;