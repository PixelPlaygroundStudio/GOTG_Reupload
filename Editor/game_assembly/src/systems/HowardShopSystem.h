#pragma once
#include "Wiwa/ecs//Systems.h"

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  HowardShopSystem : public System {
	public:
		bool m_ReturnToBaseHUD;
		bool m_ActiveSecondPage;

		bool m_FirstShopActive;
		bool m_SecondShopActive;

		const char* itemsPerPage1[7] = { "Health Capacitor","Nano Boost","Easy Trigger","Fancy Boots",
		"Lethal shooter", "Shield Fan", "Nanomachines" };

		const char* itemsPerPage2[4] = { "Recovery Shield","Devourer","Fanatic","Recovery health"};

	public:
		HowardShopSystem();
		~HowardShopSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void HandleCanvasShops();

		void HandleTextPerButton();
		void OnCollisionEnter(Object* body1, Object* body2) override;
	};
}

REGISTER_SYSTEM(Wiwa::HowardShopSystem);