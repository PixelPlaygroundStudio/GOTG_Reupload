#pragma once

#include "Wiwa/Ui/UiControl.h"
#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/utilities/render/Text.h>
#include <vector>
#include <string>


namespace Wiwa
{
	class WI_API GuiText : public Wiwa::GuiControl
	{
	public:

		GuiText(Scene* scene, unsigned int id, Rect2i bounds, const char* string_text, bool active, float rotation);
		virtual ~GuiText();

		bool Draw(Renderer2D* render);

		Text* text_res;
	};
}

