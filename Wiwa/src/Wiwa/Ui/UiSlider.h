#pragma once

#include "Wiwa/Ui/UiControl.h"

#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>

#include <vector>
#include <string>

namespace Wiwa
{
	class WI_API GuiSlider : public GuiControl
	{
	public:

		GuiSlider(Scene* scene, unsigned int id, Rect2i bounds, Rect2i sliderBounds, const char* path,const char* slider_path, size_t callbackID, Rect2i boundsOriginTex,Rect2i sliderOriginTex, const char* audioEventName, bool active);
		virtual ~GuiSlider();

		bool Update();
		bool Draw(Renderer2D* render);
		float value;


		bool canClick = true;

		bool clickedLeft = false;
		bool clickedRight = false;

	};
}