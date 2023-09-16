#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
	class RewardRoomDisplay : public System
	{
	public:
		virtual void OnInit() override;
	private:
		EntityManager::ComponentIterator m_DataIt;
	};
}
REGISTER_SYSTEM(Wiwa::RewardRoomDisplay)