#pragma once
#include <Wiwa/core/Core.h>
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
	class WI_API RewardRoom : public System
	{
	public:
		RewardRoom() = default;
		virtual ~RewardRoom() = default;

		virtual void OnInit() override;
	};

}
REGISTER_SYSTEM(Wiwa::RewardRoom);