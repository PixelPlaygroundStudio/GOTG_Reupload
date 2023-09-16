#pragma once

#include "Wiwa/Ui/UiControl.h"
#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>

#include <vector>
#include <string>


namespace Wiwa
{
	class WI_API GuiAbility : public Wiwa::GuiControl
	{
	public:

		GuiAbility(Scene* scene, unsigned int id, Rect2i bounds, const char* path, size_t callbackID, Rect2i boundsOriginTex, bool active,bool animated,std::vector<Rect2i> animationRects, float rotation);
		virtual ~GuiAbility();

		bool Update();
		bool Draw(Renderer2D* render);

		bool canClick = true;

		bool clicked = false;

	};
}
