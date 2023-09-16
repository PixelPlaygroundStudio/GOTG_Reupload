#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
class SkipIntro : public System
	{
	public:
		bool m_SkipIntro = false;
	public:
		//Called every frame
		virtual void OnUpdate() override;
	};
}
REGISTER_SYSTEM(Wiwa::SkipIntro)
