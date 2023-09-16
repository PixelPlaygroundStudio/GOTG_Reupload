#pragma once

#include "Wiwa/Ui/UiControl.h"

#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>

#include <vector>
#include <string>

namespace Wiwa
{
	class WI_API GuiBar : public Wiwa::GuiControl
	{
	public:

		GuiBar(Scene* scene, unsigned int id, Rect2i bounds, Rect2i sliderBounds, const char* path, const char* slider_path,Rect2i boundsOriginTex, Rect2i sliderOriginTex,bool active);
		virtual ~GuiBar();

		bool Draw(Renderer2D* render);
		float value;


		bool canClick = true;

		bool clicked = false;

	};
}