#pragma once

#include "Wiwa/Ui/UiControl.h"
#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/utilities/render/Text.h>
#include <vector>
#include <string>


namespace Wiwa
{
	class WI_API GuiCanvas
	{
	public:

		GuiCanvas(Scene* scene, unsigned int id,bool active);
		 ~GuiCanvas();

		 bool Update();

		 std::vector<GuiControl*> controls;
		 std::vector<GuiControl*> controlsForSelection;
		 std::vector<GuiControl*> controlsToDestroy;


		 void SwapActive();

		 void InputController();

		 void SelectElement(int id);
		 bool active;
		 bool selected;
		 int id;

		 int idGuiSelected = 0;
		 bool updateSelected = false;
		 bool DpadUp = false;
		 bool DpadDown = false;

		 float timer = 0.0f;
		 float MaxTimeBetweenChanges= 1.6f;
	};
}