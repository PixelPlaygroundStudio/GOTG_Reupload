#pragma once

#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class ShipMainMenu : public System
	{
	public:
		ShipMainMenu();
		virtual ~ShipMainMenu();

		virtual void OnAwake() override;
		virtual void OnInit() override;
		virtual void OnUpdate() override;

		void PanToCamera();
		void PanToHala();
		void SetPanToCamera(bool ret);

		void OnCollisionEnter(Object* body1, Object* body2) override;

		void OnCollisionExit(Object* body1, Object* body2) override;


	private:
		EntityManager::ComponentIterator m_ShipDataIt;

		glm::vec3 m_StartPos;
		float m_Time;
		float m_TimerToChangeScreen;
		glm::vec3 m_EndPanPos;

		bool m_SceneChange;
		bool m_GlFWeskk;
		bool m_GLFWeskk2;
	};
}
REGISTER_SYSTEM(Wiwa::ShipMainMenu);

