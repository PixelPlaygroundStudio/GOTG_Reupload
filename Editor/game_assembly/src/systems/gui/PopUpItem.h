#pragma once
#include "Wiwa/ecs//Systems.h"

#include <Wiwa/utilities/Reflection.h>
struct ItemCharaceristics {
	std::string itemName;
	std::string itemDescription;
	Wiwa::ItemType type;
};
namespace Wiwa {
	class  PopUpItem : public System {
	public:
		bool m_Animate;
		bool m_PopUp;
		bool m_PopOut;
		float m_TimerAnim;
		float m_TimerIn;
		float m_TimerOut;
		ItemCharaceristics item;
	public:
		PopUpItem();
		~PopUpItem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void SetPopUpParameters();

		void OnCollisionEnter(Object* body1, Object* body2) override;
	};
}

REGISTER_SYSTEM(Wiwa::PopUpItem);