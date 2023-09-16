#pragma once
#include <Wiwa/core/Core.h>
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
	class WI_API ShopRoom : public System
	{
	public:
		ShopRoom() = default;
		virtual ~ShopRoom() = default;

		virtual void OnInit() override;
	};

}
REGISTER_SYSTEM(Wiwa::ShopRoom);