#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
	class TestingSystem : public System
	{
	public:
		TestingSystem();
		~TestingSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		bool m_SpawnOnce = false;
		float m_Timer = 0.0f;
	};
}
REGISTER_SYSTEM(Wiwa::TestingSystem)
