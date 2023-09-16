#pragma once

#include "Wiwa/Ui/UiControl.h"
#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/utilities/render/Video.h>
#include <vector>
#include <string>


namespace Wiwa
{
	class WI_API GuiVideo : public Wiwa::GuiControl
	{
	public:

		GuiVideo(Scene* scene, unsigned int id, Rect2i bounds, const char* pathVideo, bool active,size_t callbackID);
		virtual ~GuiVideo();

		bool Update();
		bool Draw(Renderer2D* render);

		Video* video_res;
		bool video_finished = false;
	};
}

