#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
class SkipScene : public System
	{
	public:
		void OnInit() override;
		//Called every frame
		virtual void OnUpdate() override;
	private:
		bool m_Active;
		bool m_SceneChange;
	};
}
REGISTER_SYSTEM(Wiwa::SkipScene)
