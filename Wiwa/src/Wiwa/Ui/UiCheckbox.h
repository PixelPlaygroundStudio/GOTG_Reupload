#pragma once

#include "Wiwa/Ui/UiControl.h"
#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>

#include <vector>
#include <string>

namespace Wiwa
{
	class WI_API GuiCheckbox : public GuiControl
	{
	public:

		GuiCheckbox(Scene* scene, unsigned int id, Rect2i bounds,const char* path, size_t callbackID, Rect2i boundsOriginTex, const char* audioEventName, bool active, bool animated, std::vector<Rect2i> animationRects, float rotation);
		virtual ~GuiCheckbox();

		bool Update();
		bool Draw(Renderer2D* render);

		bool canClick = true;

		bool clicked = false;
		bool pressedGamepad = false;
	};
}