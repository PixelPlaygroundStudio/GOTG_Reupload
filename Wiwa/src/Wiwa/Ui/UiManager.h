#pragma once

#include "Wiwa/Ui/UiControl.h"
#include "Wiwa/Ui/UiCanvas.h"
#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/utilities/render/Text.h>
#include <Wiwa/utilities/render/Video.h>
#include <vector>
#include <string>
#include <Wiwa/core/Input.h>



namespace Wiwa
{
	class WI_API GuiManager
	{
	private:
		Scene* m_Scene;
		const char* font;
	public:

		// Constructor
		GuiManager();

		// Destructor
		virtual ~GuiManager();

	
		// Init gui manager
		bool Init(Scene* scene);

		// Update gui manager
		bool Update();

		bool Draw();

		// Called before quitting
		bool CleanUp();

		//Method for canvas
		GuiCanvas* CreateGuiCanvas(unsigned int id,bool active);
		//Method for sliders / UI_BARS
		GuiControl* CreateGuiControl(GuiControlType type,unsigned int id,Rect2i bounds,const char* path, const char* slider_path, Rect2i sliderBounds,unsigned int canvas_id, int callbackID, Rect2i boundsOriginTex, Rect2i sliderOriginTex, const char* audioEventName,bool active,float rotation);
		//Method for buttons and checkboxes
		GuiControl* CreateGuiControl_Simple(GuiControlType type, unsigned int id, Rect2i bounds, const char* path, const char* extraPath, unsigned int canvas_id, int callbackID, Rect2i boundsOriginTex,const char* audioEventName,const char* audioNameFocused,const char* audioNameSelected, bool active, bool animated, float framesAnim, std::vector<Rect2i> animRects,float rotation);
		//Method for strings
		GuiControl* CreateGuiControl_Text(GuiControlType type, unsigned int id, Rect2i bounds, const char* string_text, unsigned int canvas_id, bool active,float rotation);
		//Method for abilities
		GuiControl* CreateGuiControl_Ability(GuiControlType type, unsigned int id,unsigned int canvas_id, Rect2i bounds, const char* path, size_t callbackID, Rect2i boundsOriginTex, bool active,bool animated, std::vector<Rect2i> animationRects,float rotation);
		//Method for videos
		GuiControl* CreateGuiControl_Video(GuiControlType type, unsigned int id, unsigned int canvas_id, Rect2i bounds, const char* path, bool active,size_t callbackID);

		void SwapSelectedCanvas(GuiCanvas* canvasToSelect);
		void DestroyGuiControl(GuiControl* control, GuiCanvas* canvas);
		void DestroyCanvas(GuiCanvas* canvas);
		void RemoveCanvas(GuiCanvas* canvasToDestroy);
		void RemoveControl(GuiControl* control);

		Text* InitFont(const char* path, const char* word);
		Text* InitFontForDialog(const char* path, char* word, int maxWidth);
		int ComputeX_DialogOffset(const char* path, char* word, int maxWidth, int charPos);
		int ComputeY_DialogOffset(const char* path, char* word, int maxWidth);

		
		Video* InitVideo(std::string path);

		std::vector<GuiCanvas*>& ReturnCanvas() { return canvas; }

		bool _saveGUIImpl(File& file);
		bool _loadGUIImpl(File& file);

		void SaveWiUI(const char* path);

		void LoadWiUI(const char* path);

		int getCurrentCanvas();

		const char* GetCurrentPrefabWiGui() { return PrefabWiGui.c_str(); }
		void SetCurrentPrefabWiGui(const char* currentPrefab);

	public:
		std::vector<GuiCanvas*> canvas;
		std::vector<GuiCanvas*> canvasToDestroy;
		std::vector<Text*> textToDestroy;
	private:
		std::string PrefabWiGui = "";
	};
}