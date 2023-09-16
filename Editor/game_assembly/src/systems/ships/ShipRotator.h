#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
	class ShipRotator : public System
	{
	public:
		//Called the first frame
		virtual void OnAwake() override;
		//Called after OnAwake()
		virtual void OnInit() override;
		//Called every frame
		virtual void OnUpdate() override;

	private:
		EntityManager::ComponentIterator m_ShipDataIt;
	};
}
REGISTER_SYSTEM(Wiwa::ShipRotator)
