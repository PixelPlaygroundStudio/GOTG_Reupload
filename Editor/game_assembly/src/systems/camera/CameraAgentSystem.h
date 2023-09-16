#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
class CameraAgentSystem : public System
	{
	public:
		//Called the first frame
		virtual void OnAwake() override;
		//Called after OnAwake()
		virtual void OnInit() override;
		//Called every frame
		virtual void OnUpdate() override;

		virtual bool OnEnabledFromPool() override;
		virtual bool OnDisabledFromPool() override;
	private:
		EntityManager::ComponentIterator m_DataIt;
	};
}
REGISTER_SYSTEM(Wiwa::CameraAgentSystem)
