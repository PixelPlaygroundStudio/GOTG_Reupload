//#pragma once
//
//#include "Wiwa/ecs/systems/System.h"
//#include "Wiwa/ecs/components/Transform2D.h"
//#include <Wiwa/utilities/Reflection.h>
//#include <Wiwa/core/Input.h>
//#include "Wiwa/ecs/components/Ui/Button.h"
//#include "Wiwa/ecs/components/Ui/Checkbox.h"
//#include "Wiwa/ecs/components/Ui/Slider.h"
//
//
//namespace Wiwa
//{
//
//	class WI_API UiSystem : public System
//	{
//	private:
//
//	public:
//		UiSystem();
//		~UiSystem();
//
//		void OnAwake() override;
//
//		void OnInit() override;
//
//		void OnUpdate() override;
//
//		void OnDestroy() override;
//
//		void UpdateButton(Button* button);
//		void RenderButton(Button* button);
//
//		void UpdateCheckbox(Checkbox* checkbox);
//		void RenderCheckbox(Checkbox* checkbox);
//
//		void UpdateSlider(Slider* slider);
//		void RenderSlider(Slider* slider);
//	};
//}
//
//REGISTER_SYSTEM(Wiwa::UiSystem);