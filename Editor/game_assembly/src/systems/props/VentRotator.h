#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
class VentRotator : public System
	{
	public:
		virtual void OnInit() override;
		//Called every frame
		virtual void OnUpdate() override;
	private:
		EntityManager::ComponentIterator m_VentRotatorIt;
		float m_Velocity;
	};
}
REGISTER_SYSTEM(Wiwa::VentRotator)
