#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
class EndRewardRoom : public System
	{
	public:
		//Called the first frame
		virtual void OnAwake() override;
		//Called after OnAwake()
		virtual void OnInit() override;
		//Called every frame
		virtual void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;
		void OnCollisionExit(Object* body1, Object* body2) override;

	private:
		EntityManager::ComponentIterator m_DataIt;
		bool m_Activated = false;
		float m_Timer = 0.0f;
	};
}
REGISTER_SYSTEM(Wiwa::EndRewardRoom)
