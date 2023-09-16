#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
class Reset_HubSystem : public System
	{
	public:
		//Called the first frame
		virtual void OnAwake() override;
		//Called after OnAwake()
		virtual void OnInit() override;
		//Called every frame
		virtual void OnUpdate() override;
	private:
		bool m_ResetStats;
	};
}
REGISTER_SYSTEM(Wiwa::Reset_HubSystem)
